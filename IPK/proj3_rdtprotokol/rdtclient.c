/**
 * @author : Michal Lukac, xlukac05@stud.fit.vutbr.cz
 * @date   : 23.2.2012
 * @brief  : Simple c rdt client with sliding window and go-back-n algorithm.
 *           This is the school project to IPK at BUT FIT.
 *
 * For this project I use this external sources:
 *           http://www.lemoda.net/unix/base64/index.html
 *           http://xmlsoft.org/
 *           http://xmlsoft.org/tutorial/xmltutorial.pdf
 *           and example of timer.c and udtdemo.c from wis.
 */

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netdb.h>
#include <regex.h>
#include <stdlib.h>
#include <fcntl.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sys/time.h>
#include <signal.h>
#include "udt.h"

/** base64 */
#define b64_ntop __b64_ntop
#define b64_pton __b64_pton
int b64_ntop(unsigned char const *src, size_t srclength,char *target, size_t targsize);
int b64_pton(char const *src, unsigned char *target, size_t targsize);
#define targsize 250
/** end of base64 */

/** udt protocol */
in_addr_t remote_addr = 0x7f000001;
in_port_t sourceport;
in_port_t destport;
int udt;

#define MAXWINDOW 5
#define MAXLINE 150
#define MAXPACKET 500


/** The Sliding window for go-back-n algorithm */
char G_SWindow[MAXWINDOW][MAXPACKET];
int G_SWindowI[MAXWINDOW];

/** indexes for sliding window */
int start_SWin = 0;
int end_SWin = -1;
int send_SWin = 0;
int index_SWin = 0;

/** sigmask,timer... */
sigset_t sigmask;
struct itimerval itv;

/** Error codes of program. */
enum tecodes
{
  EOK = 0,          /* Everything is ok. */
  EBADPARAMS = 1,   /* Error with params. */
  EPARSING = 2,     /* Error with parsing xml. */
};

/** Error messages equals to tecodes. */
const char *ECODEMSG[] =
{
  "Everything is ok.\n",
  "Error with parameters.\n",
  "Error with parsing xml.\n",
};


/** Prototypes of functions. */
void printError(int error);
int client();

/**
 * Function print error message.
 * @param error Index for ECODEMSG.
 */
void printError(int error)
{
  fprintf(stderr, "%s", ECODEMSG[error]);
}

/**
 * Proces params of program.
 * @param argc Count of params.
 * @param argv params of program.
 * @return processed params.
 */
int processParams(int argc, char **argv)
{ 
  if(argc == 5)
  {
    for(int i = 1; i < 5; i++)
    {
      if(strcmp("-s",argv[i]) == 0)
      {
        if((i + 1) < argc)
          sourceport = atoi(argv[i+1]);
        else
          return EBADPARAMS;
        if(sourceport == 0)
          return EBADPARAMS;
        i++;
      }
      else if(strcmp("-d",argv[i]) == 0)
      {
        if((i + 1) < argc)
          destport = atoi(argv[(i+1)]);
        else
          return EBADPARAMS;
        if(destport == 0)
          return EBADPARAMS;
        i++;
      }
      else
        return EBADPARAMS;
    }
  }
  else
    return EBADPARAMS;
  return 0;
}

/**
 * Parse xml and return index packet.
 * @param message Accepted message from packet.
 * @return Succes value.
 */
int myParseXML(char *message,int *index,int *endshake)
{
  xmlDocPtr doc;
  xmlNodePtr cur;

  // parse xml document
  doc = xmlParseMemory(message,strlen(message));

  if(doc == NULL)
    return EPARSING;

  // get root element
  cur = xmlDocGetRootElement(doc);
  if(cur == NULL)
  {
    xmlFreeDoc(doc);
    return EPARSING;
  }

  // if is not root == rdt-segment
  if(xmlStrcmp(cur->name,(const xmlChar *)"rdt-segment"))
  {
    xmlFreeDoc(doc);
    return EPARSING;
  }
  cur = cur->xmlChildrenNode;

  int state = 0;
  while (cur != NULL)
  {
    // get header
    if(!xmlStrcmp(cur->name,(const xmlChar *)"header") && state == 0)
    {
      state = 1;
      cur = cur->xmlChildrenNode;
    }
    // parse <endshake>VALUE</endshake>
    else if(!xmlStrcmp(cur->name,(const xmlChar *)"endshake") && state == 1)
    {
      xmlChar *key;
      key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
      *endshake = atoi((const char*)key);
      xmlFree(key);
      break;
    }
    // parse <ACK>VALUE</ACK>
    else if(!xmlStrcmp(cur->name,(const xmlChar *)"ACK") && state == 1)
    {
      xmlChar *key;  
      key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
      *index = atoi((const char*)key);
      xmlFree(key);
      cur = cur->next;
    }
    else
      cur = cur->next;
  }

  // free it
  xmlFreeDoc(doc);
  return EOK;
}

/**
 * Simple function/sigalarm handler for clock timeout.
 */
void sigalrm_handler(int sig)
{
  sigprocmask(SIG_BLOCK, &sigmask, NULL);
  int len = start_SWin + MAXWINDOW;  
  for(int i = start_SWin; i < len; i++)
  {
    //if(G_SWindowI[i%MAXWINDOW] != - 1)
      udt_send(udt, remote_addr, destport, G_SWindow[i%MAXWINDOW], strlen(G_SWindow[i%MAXWINDOW]));
  }

  setitimer(ITIMER_REAL, &itv, NULL);
  sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
  // Reinstall handler.
  signal(SIGALRM, sigalrm_handler);
}


/**
 * This is simple client function.
 * @param param Processed parameters.
 * @param argv Arguments of program.
 * @return Succesful value if is everything ok.
 */
int client()
{
  //char *sendline = malloc(sizeof(char)*MAXLINE);
  char sendline[MAXLINE];
  char receivedline[targsize];
  char recvline[MAXPACKET];
  char sendline64[targsize];
  char sendMessage[MAXPACKET] = "hello world\n";
  fd_set readfds;
  char strindex[80];
  int acked = 0;
  int endshake = 0;
  int state = 1;
  int state2 = 1;

  // set read settings
  FD_ZERO(&readfds);
  FD_SET(udt, &readfds);
  FD_SET(STDIN_FILENO, &readfds);

  // init window
  for(int i = 0; i < MAXWINDOW; i++)
    G_SWindowI[i] = -1;

  // It is important to init UDT!
  udt = udt_init(sourceport);

  // make stdin reading non-clocking
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

  int len = 0;
  // Set timing
  signal(SIGALRM, sigalrm_handler);
  itv.it_interval.tv_sec = 0;    // sets an interval of the timer
  itv.it_interval.tv_usec = 6000;
  itv.it_value.tv_sec = 0;       // sets an initial value
  itv.it_value.tv_usec = 6000;
  setitimer(ITIMER_REAL, &itv, NULL);
  sigemptyset(&sigmask);
  sigaddset(&sigmask, SIGALRM);
  sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
  int a = 0;
  // writing/sending cycle
  while(select(udt+1, &readfds, NULL, NULL, NULL))
  {
    // if is index in sliding window free to write
    if((index_SWin%MAXWINDOW) != (end_SWin%MAXWINDOW))
    {
      // read from stdin?
      if(FD_ISSET(STDIN_FILENO, &readfds))
      {
        bzero(sendline64,targsize);
        bzero(receivedline,targsize);
        bzero(sendline,MAXLINE);
        // block timer
        sigprocmask(SIG_BLOCK, &sigmask, NULL);
        if((len = fread(sendline,1,MAXLINE,stdin)) > 0)
        {
          bzero(sendMessage,MAXPACKET);  
          if(state2 == 1) {state2 = 0; end_SWin = 0;}
          // b64 code
          a = b64_ntop((unsigned char*)sendline,MAXLINE,sendline64,targsize);
          sendline64[a] = '\0';
          int end = b64_pton ((char*)sendline64,(unsigned char *)receivedline,targsize);
          strcpy(sendMessage,"<rdt-segment id=\"xlukac05\"><header><endshake>0</endshake><length>");
          sprintf(strindex,"%d",len);
          strcat(sendMessage,strindex);
          strcat(sendMessage,"</length><packetindex>");
          // create index
          bzero(strindex,80);
          sprintf(strindex, "%d", index_SWin);
          strcat(sendMessage,strindex);
          strcat(sendMessage,"</packetindex>");
          strcat(sendMessage,"</header><data>");
          strncat(sendMessage,sendline64,targsize);
          strcat(sendMessage,"</data></rdt-segment>\n");
          // add to window
          strcpy(G_SWindow[index_SWin%MAXWINDOW],sendMessage);
          G_SWindowI[index_SWin%MAXWINDOW] = 0;
          // send packet
          udt_send(udt, remote_addr, destport, G_SWindow[send_SWin], strlen(G_SWindow[send_SWin]));
          index_SWin= index_SWin + 1;
          //setitimer(ITIMER_REAL, &itv, NULL);
        }
        // is it eof
        else if(state == 1)
        {
          state = 2;
          bzero(sendMessage,MAXPACKET);
          sprintf(strindex, "%d", index_SWin);
          strcpy(sendMessage,"<rdt-segment id=\"xlukac05\"><header><endshake>1</endshake><packetindex>");
          strcat(sendMessage,strindex);
          strcat(sendMessage,"</packetindex></header><data></data></rdt-segment>\n");
          // add to window
          strcpy(G_SWindow[index_SWin%MAXWINDOW],sendMessage);
          G_SWindowI[index_SWin%MAXWINDOW] = 0;
          send_SWin = index_SWin%MAXWINDOW;
          udt_send(udt, remote_addr,destport, G_SWindow[send_SWin], strlen(G_SWindow[send_SWin]));
          //setitimer(ITIMER_REAL, &itv, NULL);
        }
        sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
      }
    }
    
    if(FD_ISSET(udt, &readfds))
    {
      // block timer
      sigprocmask(SIG_BLOCK, &sigmask, NULL);
      // receive packet
      int n = udt_recv(udt, recvline, MAXLINE, NULL, NULL);
      recvline[n] = 0;
      // parse xml and get acked info
      if(strlen(recvline) > 0)
      {
        if(myParseXML(recvline,&acked,&endshake))
          return EPARSING;
        // if it is endshakeack end cycle
        if(endshake) break;
        // set acked 
        G_SWindowI[(acked%MAXWINDOW)] = 1;
        if(acked > end_SWin)
          end_SWin = acked + 1;
        if(end_SWin == index_SWin)
          end_SWin--;
        // lets start timer again
        setitimer(ITIMER_REAL, &itv, NULL);
        sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
      }
    }

    // and again!
    FD_ZERO(&readfds);
    FD_SET(udt, &readfds);
    FD_SET(STDIN_FILENO, &readfds);
  }
  return EOK;
}

/**
 * The main function.
 * @param argc Counts of params.
 * @param argv Params of program.
 * @return successful of program.
 */
int main(int argc, char **argv)
{
  int error = processParams(argc,argv);
  if(error != 0) {printError(EBADPARAMS); return EBADPARAMS;}  
  
  // starting client
  int result = client();
  if(result) printError(result);
  return result;
}
/*** End of file client.cpp ***/
