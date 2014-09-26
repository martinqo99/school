/**
 * @author : Michal Lukac, xlukac05@stud.fit.vutbr.cz
 * @date   : 25.2.2012
 * @brief  : Simple c rdt server with go-back-n and sliding window.
 *           This is the school project to IPK at BUT FIT.
 *           Implemented with go-back-n and sliding window.
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
#include <stdarg.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <signal.h>
#include "udt.h"

// base64
#define b64_ntop __b64_ntop
#define b64_pton __b64_pton
int b64_ntop(unsigned char const *src, size_t srclength,char *target, size_t targsize);
int b64_pton(char const *src, unsigned char *target, size_t targsize);
#define targsize 250
// end of base64

#define MAXWINDOW 5
#define MAXLINE 150
#define MAXPACKET 500

in_addr_t remote_addr = 0x7f000001;
int timeout = 1;
int resttime = 10;

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

/** Structure for processed arguments of program. */
typedef struct param
{
  int error;
  in_port_t sourceport;
  in_port_t destport;
} TParams;

/** Prototypes of functions. */
void printError(int error);
void processParams(TParams *param,int argc, char **argv);
int server(TParams *param);

/**
 * Exit program when sigterm occurs
 */
void exitProgram(int sig)
{
  exit(EOK);
}

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
void processParams(TParams *param, int argc, char **argv)
{
  param->error = 0;
  param->sourceport = 0;
  param->destport = 0;
 
  if(argc == 5)
  {
    for(int i = 1; i < 5; i++)
    {
      if(strcmp("-s",argv[i]) == 0)
      {
        if((i + 1) < argc)
          param->sourceport = atoi(argv[i+1]);
        else
          param->error = EBADPARAMS;
        if(param->sourceport == 0)
          param->error = EBADPARAMS;
        i++;
      }
      else if(strcmp("-d",argv[i]) == 0)
      {
        if((i + 1) < argc)
          param->destport = atoi(argv[(i+1)]);
        else
          param->error = EBADPARAMS;
        if(param->destport == 0)
          param->error = EBADPARAMS;
        i++;
      }
      else
        param->error = EBADPARAMS;
    }
  }
  else
    param->error = EBADPARAMS;
  return;
}

/**
 * Parse xml and return index packet.
 * @param message Accepted message from packet.
 */
int myParseXML(char *message,int *index,int *myindex,int *endshake)
{
  unsigned char receivedline[targsize];
  xmlDocPtr doc;
  xmlNodePtr cur;
  xmlNodePtr cur2;

  // xml parse memory
  doc = xmlParseMemory(message,strlen(message));

  if(doc == NULL)
    return EPARSING;

  cur = xmlDocGetRootElement(doc);
  if(cur == NULL)
  {
    xmlFreeDoc(doc);
    return EPARSING;
  }

  // root element
  if(xmlStrcmp(cur->name,(const xmlChar *)"rdt-segment"))
  {
    xmlFreeDoc(doc);
    return EPARSING;
  }

  cur = cur->xmlChildrenNode;
  *endshake = 0;
  int state = 0;
  int length = 0;

  // reading cycle
  while (cur != NULL)
  {
    // header element
    if(!xmlStrcmp(cur->name,(const xmlChar *)"header") && state == 0)
    {
      state = 1;
      cur2 = cur;
      cur = cur->xmlChildrenNode;
    }
    else if(!xmlStrcmp(cur->name,(const xmlChar *)"length") && state == 1)
    {
      xmlChar *key;
      key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
      length = atoi((const char*)key);
      xmlFree(key);
      cur = cur->next;
    }
    // endshake element
    else if(!xmlStrcmp(cur->name,(const xmlChar *)"endshake") && state == 1)
    {
      xmlChar *key;
      key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
      *endshake = atoi((const char*)key);
      xmlFree(key);
      cur = cur->next;
    }
    // packetindex element
    else if(!xmlStrcmp(cur->name,(const xmlChar *)"packetindex") && state == 1)
    {
      xmlChar *key;
      key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
      *index = atoi((const char*)key);
      xmlFree(key);
      if(*index != *myindex)
        break;
      cur = cur2->next;
    }
    // data element
    else if(!xmlStrcmp(cur->name,(const xmlChar *)"data") && state == 1)
    {
      if(*endshake == 0)
      {
        xmlChar *key;
        key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
        if(key != NULL)
        {
          bzero(receivedline,targsize);
          int end = b64_pton ((char *)key,(unsigned char *)receivedline,targsize);
          fwrite(receivedline,1,length,stdout);
          fflush (stdout);
          xmlFree(key);
        }
      }
      break;
    }
    else
      cur = cur->next;
  }

  xmlFreeDoc(doc);
  return 0;
}

/**
 * This is simple server function.
 * @param param Processed parameters.
 * @param argv Arguments of program.
 * @return Succesful value if is everything ok.
 */
int server(TParams *param)
{
  char ch;
  char buf[80];
  char sendline[MAXPACKET];
  char recvline[MAXPACKET];
  int udt;
  char sendMessage[50];
  char strindex[20];
  int index = 0;
  int myindex = 0;
  int endshake = 0;

  bzero(sendline,MAXPACKET);
  bzero(recvline,MAXPACKET);

  // It is important to init UDT!
  udt = udt_init(param->sourceport);

  // make stdin reading non-clocking
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(udt, &readfds);
  FD_SET(STDIN_FILENO, &readfds);

  // writing/sending cycle
  while(select(udt+1, &readfds, NULL, NULL, NULL))
  {
    if(FD_ISSET(udt, &readfds))
    {
      // We have a new message to print
      int n = udt_recv(udt, recvline, MAXPACKET, NULL, NULL);
      recvline[n] = 0;
      if(strlen(recvline))
      {
        fflush(stdout);

        // lets parse data
        if(myParseXML(recvline,&index,&myindex,&endshake))
          return EPARSING;

        bzero(recvline,MAXPACKET); 
        // is it index what i want
        if(index == myindex)
        {
          // send ACK message
          sprintf(strindex, "%d", index);
          strcpy(sendMessage,"<rdt-segment id=\"xlukac05\"><header><ACK>");
          strcat(sendMessage,strindex);
          strcat(sendMessage,"</ACK><endshake>");
          if(endshake) {strcat(sendMessage,"1");}else{strcat(sendMessage,"0");}
          strcat(sendMessage,"</endshake></header><data></data></rdt-segment>\n");
          udt_send(udt, remote_addr, param->destport, sendMessage, strlen(sendMessage));
          myindex++;
        }
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
  (void)signal(SIGTERM,exitProgram);
  TParams param;
  processParams(&param,argc, argv);
  if(param.error != 0) {printError(EBADPARAMS); return EBADPARAMS;}

  // starting client
  int result = server(&param);
  if(result) printError(EBADPARAMS);
  return result;
}
/*** End of file server.cpp ***/
