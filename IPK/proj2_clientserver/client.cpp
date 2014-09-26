/**
 * @author : Michal Lukac, xlukac05@stud.fit.vutbr.cz
 * @date   : 23.2.2012
 * @brief  : Simple c++ "email" client. This is the school project
 *           to IPK at BUT FIT.
 */

#include <iostream>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <regex.h>
#include <stdlib.h>

using namespace std;

#define G_BUFFER 2048
#define G_BUFFER_1 1024

/** Error codes of program. */
enum tecodes
{
  EOK = 0,          /* Everything is ok. */
  EBADPARAMS = 1,   /* Error with params. */
  ESOCKET = 2,      /* Error with creating socket */
  EHOST = 3,        /* Error with gethostbyname. */
  ECONNECT = 4,     /* Error with connecting to server. */
  EWMSG = 5,        /* Error with writing message to server. */
  ERMSG = 6,        /* Error with reading message from server. */
  EREGEX = 7,       /* Error with regular expression. */
  EPORT = 8         /* Error, port is not specified. */
};

/** Error messages equals to tecodes. */
const char *ECODEMSG[] =
{
  "Everything is ok.\n",
  "Error with parameters.\n",
  "Error with creating socket.\n",
  "Error with gethostbyname.\n",
  "Error with connecting to server.\n",
  "Error with writing message to server.\n",
  "Error with reading message from server.\n",
  "Error with regular expression.\n",
  "Error, port is not specified.\n",
};

/** Pattern strings, regular experssions */
const char *REGEXSTRINGS[] =
{
  // url address
  "^(http://)*(www\\.)*([^:/]+)\\:*([[:digit:]]*)([/]*.*)$",
};

/** Structure for processed arguments of program. */
typedef struct param
{
  int state; // 0 = nothing, r = read, s = send, d = delete
  int error;
} TParams;

/** Prototypes of functions. */
void printError(int error);
void processParams(TParams *param,int argc, char **argv);
int client(TParams *param, char **argv);
int getPropperLink(const char *link, string *uri, string *speclink, int *port);

/**
 * Function print error message.
 * @param error Index for ECODEMSG.
 */
void printError(int error)
{
  cerr << ECODEMSG[error];
}

/**
 * Proces params of program.
 * @param argc Count of params.
 * @param argv params of program.
 * @return processed params.
 */
void processParams(TParams *param, int argc, char **argv)
{
  param->state = 0;
  param->error = 0;

  if(argc >= 2)
  {
    if(argc == 4)
    {
      if(strcmp("-r",argv[2]) == 0)
        param->state = (int)'r';
      else
        param->error = 1;
    }
    else if(argc == 5)
    {
      if(strcmp("-s",argv[2]) == 0)
        param->state = (int)'s';
      else if(strcmp("-d",argv[2]) == 0)
      {
        param->state = (int)'d';
        int number = atoi(argv[4]);
        if(number == 0 || number < 0) {param->error = 1;}
      }
      else
        param->error = 1;
    }
    else
      param->error = 1;    
  }
  else
    param->error = 1;
  return;
}

/**
 * This function use regex for matching adress.
 * @param link Url address
 * @param uri Returned part of link
 * @param speclink Returned part of link 2 for HEAD.
 * @param port Port.
 * return If ok then 0. 
 */
int getPropperLink(const char *link, string *uri, string *speclink, int *port)
{
  regex_t re;
  regmatch_t pmatch[6];

  // empty arrays for matches
  char address[G_BUFFER_1];
  char behindcomma[G_BUFFER_1];
  char portstring[G_BUFFER_1];

  // clear them
  bzero(address,G_BUFFER_1);
  bzero(behindcomma,G_BUFFER_1);
  bzero(portstring,G_BUFFER_1);

  // init regex with pattern
  if(regcomp(&re,REGEXSTRINGS[0],REG_EXTENDED)!= 0)
  {
    printError(EREGEX);
    return EREGEX;
  }

  // match url address
  int status = regexec(&re,link,6,pmatch,0);
  if(status == 0)
  {
    strncpy(address,link+pmatch[3].rm_so,pmatch[3].rm_eo-pmatch[3].rm_so);
    strncpy(behindcomma,link+pmatch[5].rm_so,pmatch[5].rm_eo-pmatch[5].rm_so);
    strncpy(portstring,link+pmatch[4].rm_so,pmatch[4].rm_eo-pmatch[4].rm_so);

    uri->append(address);
    if(strlen(behindcomma) > 0){speclink->append(behindcomma);}
    else {speclink->append("/");}
    if(strlen(portstring) > 0){*port = atoi(portstring);}
    else{printError(EPORT); return EPORT;}
  }
  else
  {
    regfree(&re);
    printError(EREGEX);
    return EREGEX;
  }

  regfree(&re);
  return 0;
}

/**
 * This is simple client function.
 * @param param Processed parameters.
 * @param argv Arguments of program.
 * @return Succesful value if is everything ok.
 */
int client(TParams *param,char **argv)
{ 
  // init variables, socket, ...
  int mysocket;
  sockaddr_in address;
  hostent *server;
  string url1("");
  string url2("");
  string url3("");
  string url4("");
  int port = 80;

  int value = getPropperLink(argv[1],&url1,&url2,&port);
  if(value) {return value;}
  url4.append(url1.c_str());

  // if is localhost
  if(strcmp(url1.c_str(),"localhost") == 0)
  {url3.append("localhost");url2.append("/");}
  // if is ip address
  else if((url1[0] >= '0' && url1[0] <= '9'))
  {url3.append(url1.c_str());}
  // else is url link
  else
  {url3.append("www.");url3.append(url1.c_str());}

  // create socket
  if((mysocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    printError(ESOCKET);    
    return ESOCKET;
  }

  // get server info
  if((server = gethostbyname(url3.c_str())) == NULL)
  {
    url3 = "";
    url3.append(url4.c_str());
    if((server = gethostbyname(url3.c_str())) == NULL)
    {
      printError(EHOST);
      close(mysocket);
      return EHOST;
    }
  }

  // create address
  address.sin_port = htons(port);
  address.sin_family = AF_INET;
  memcpy(&(address.sin_addr),server->h_addr, server->h_length);

  // connect to server
  if(connect(mysocket, (sockaddr *)&address, sizeof(address)) == -1)
  {  
    printError(ECONNECT);
    close(mysocket);
    return ECONNECT;
  }

  // create my protocol message 
  string msg("PROT: proj2#\n");
  msg.append("TYPE: ");
  msg.append(argv[2]);
  msg.append("#\n");
  msg.append("USER: ");
  msg.append(argv[3]);
  msg.append("#\n");
  msg.append("MSG: ");
  if(param->state == 's')
    msg.append(argv[4]);
  msg.append("#\n");
  msg.append("DEL: ");
  if(param->state == 'd')
    msg.append(argv[4]);
  msg.append("#\n");

  // send HEAD message to server
  if(write(mysocket, msg.c_str(), msg.size()) < 0)
  {
    printError(EWMSG);
    close(mysocket);
    return EWMSG;
  }

  // receiving data from server
  int readed = 0; char buffer[G_BUFFER];
  bzero(buffer,G_BUFFER);
  if((readed = read(mysocket, buffer, G_BUFFER)) < 0)
  {
    printError(ERMSG);
    close(mysocket);
    return ERMSG;
  }

  if(param->state == 's')
    cerr << buffer;
  else if(param->state == 'd')
    cerr << buffer;
  else
    cout << buffer;

  close(mysocket);
  // process head message
  return 0;
}

/**
 * The main function.
 * @param argc Counts of params.
 * @param argv Params of program.
 * @return successful of program.
 */
int main(int argc, char **argv)
{
  TParams param;
  processParams(&param,argc, argv);
  if(param.error != 0) {printError(EBADPARAMS); return EBADPARAMS;}  

  // starting client
  int result = client(&param,argv);
  return result;
}
/*** End of file client.cpp ***/
