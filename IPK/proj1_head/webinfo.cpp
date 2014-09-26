/**
 * @author : Michal Lukac, xlukac05@stud.fit.vutbr.cz
 * @date   : 20.2.2012
 * @brief  : Simple c++ header http client. This is the school project
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

#define G_BUFFER_1 100
#define G_BUFFER_2 1024
#define G_HTTPPORT 80
#define G_OKCODE 200
#define G_MOVED_1 301
#define G_MOVED_2 302
int G_MAXREDIRECT = 5;

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
  EBADURL = 8       /* Bad url address. */
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
  "Bad url address.\n",
};

/** Pattern strings, regular experssions */
const char *REGEXSTRINGS[] =
{
	// HTTP/1.1 200 ...
  "[Hh][Tt][Tt][Pp]/1.[[:digit:]][ 	]*([[:digit:]]+)([^\r\n]*)\r*\n",
  // Server: ...
  "([Ss][Ee][Rr][Vv][Ee][Rr][  ]*\\:[^\r\n]*)",
  // Content-Length: 1234...
  "([Cc][Oo][Nn][Tt][Ee][Nn][Tt][ 	]*-[ 	]*[Ll][Ee][Nn][Gg][Tt][Hh][ 	]*\\:[^\r\n]*)",
  // Last-Modified: 12.3....
  "([Ll][Aa][Ss][Tt][ 	]*-[ 	]*[Mm][Oo][Dd][Ii][Ff][Ii][Ee][Dd][ 	]*\\:[^\r\n]*)",
  // Content-Type: img/jpg
  "([Cc][Oo][Nn][Tt][Ee][Nn][Tt][ 	]*-[ 	]*[Tt][Yy][Pp][Ee][ 	]*\\:[^\r\n]*)",
  // Location: www.fit.vutbr.cz
  "[Ll][Oo][Cc][Aa][Tt][Ii][Oo][Nn][ 	]*\\:[ 	]*([^\r\n]*)",
  // header without last newline
  "(.+[^\r\n][^\r\n])\r*\n\r*\n",
  // url address
  "^(http://)*(www\\.)*([^:/]+)\\:*([[:digit:]]*)([/]*.*)$",
};

/** Structure for processed arguments of program. */
typedef struct param
{
  int urlindex;
  int state;
  int length;
  int serveri;
  int modify;
  int typeobject;
  int error;
} TParams;

/** Prototypes of functions. */
void printError(int error);
TParams processParams(int argc, char **argv);
int client(TParams *param, char *url);
string repairLink(char *link);
int getPropperLink(const char *link, string *uri, string *speclink, int *port);
int printPartOfHead(const char * pattern, char *header);
int parsePrintHead(char *head, TParams *params);

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
TParams processParams(int argc, char **argv)
{
  TParams param = { param.error = 0, param.state = 1, param.length = 0,
  param.modify = 0, param.serveri = 0, param.typeobject = 0, param.urlindex = 0,};

  if(argc >= 2)
  {
    param.urlindex = argc -1;
    int ch, i = 0;
    while ((ch = getopt(argc-1, argv, "lsmt")) != -1)
    {
      i++;
      switch (ch)
      {
        case 'l':
          param.length = i; param.state = 2;
          break;
        case 's':
          param.serveri = i; param.state = 2;
          break;
        case 'm':
          param.modify = i; param.state = 2;
          break;
        case 't':
          param.typeobject = i; param.state = 2;
          break;
        default:
          param.error = 1;
          break;
      }
    }
  }
  return param;
}

/**
 * Repair link with special chars like blank spaces.
 * @param link Url adress.
 * @return repaired link.
 */
string repairLink(char *link)
{
  string url("");
  int len = strlen(link);
  for(int i = 0; i < len; i++)
  {
    if(link[i] == ' ')
      url.append("%20");
    else if(link[i] == '	')
      url.append("%09");
    else
      url.append(1,link[i]); 
  }
  return url;
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
  char urladdr[G_BUFFER_1];

  // clear them
  bzero(address,G_BUFFER_1);
  bzero(behindcomma,G_BUFFER_1);
  bzero(portstring,G_BUFFER_1);
  bzero(urladdr,G_BUFFER_1);

  // init regex with pattern
  if(regcomp(&re,REGEXSTRINGS[7],REG_EXTENDED)!= 0)
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
    strncpy(urladdr,link+pmatch[1].rm_so,pmatch[1].rm_eo-pmatch[1].rm_so);

    if(!(strlen(urladdr) > 0)) {printError(EBADURL);return EBADURL;}
    uri->append(address);
    if(strlen(behindcomma) > 0){speclink->append(behindcomma);}
    else {speclink->append("/");}
    if(strlen(portstring) > 0){*port = atoi(portstring);}
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
 * If -l -m ... params are specified this
 * function prints only specified tags.
 * @param pattern Specified regex pattern for specified tag.
 * @param header Full header response from server.
 * @return int Ok/Error value.
 */
int printPartOfHead(const char * pattern, char *header)
{
  // init variables for regex
  regex_t re;
  regmatch_t pmatch[2];
  char text[G_BUFFER_2];
  bzero(text,G_BUFFER_2);
 
  if(regcomp(&re,pattern, REG_EXTENDED)!= 0)
  {
    printError(EREGEX);
    return EREGEX;
  }

  // if match
  int status = regexec(&re,header,2,pmatch,0);
  if(status == 0)
  {
    strncpy(text,header+pmatch[1].rm_so,pmatch[1].rm_eo-pmatch[1].rm_so);
    cout << text << "\r\n";
  }
  // else is not available
  else
  {
    regfree(&re);
    cout << "N/A\r\n";
    return 0;
  }

  regfree(&re);
  return 0;
}

/**
 * This function process text of HEAD response from server.
 * @param Text of header message.
 * @param Processed params of program.
 * return 0 if everything is ok.
 */
int parsePrintHead(char *head, TParams *params)
{
  // init regex variables
  regex_t re; 
  regmatch_t pmatch[3];
  regmatch_t pmatch2[2];
  regmatch_t pmatch3[2];
  char errormsg[G_BUFFER_1];
  char codemsg[G_BUFFER_1];
  char header[G_BUFFER_2];
  char urllink[G_BUFFER_2];
  int returncode = G_OKCODE;

  // clear them
  bzero(errormsg,G_BUFFER_1);
  bzero(codemsg,G_BUFFER_1);
  bzero(header,G_BUFFER_2);
  bzero(urllink,G_BUFFER_2);
  
  // create regex
  if(regcomp(&re,REGEXSTRINGS[0],REG_EXTENDED) != 0)
  {
    printError(EREGEX);
    return EREGEX;
  }

  // execute regex over string
  int status = regexec(&re, head, 3, pmatch, 0);
  if(status == 0)
  {
    strncpy(codemsg,head+pmatch[1].rm_so,pmatch[1].rm_eo-pmatch[1].rm_so);
    if(strlen(codemsg) > 0) {returncode = atoi(codemsg);}
  }

  regfree(&re);
  // if is 200 response code
  if((returncode >= G_OKCODE && returncode < 300))
  {
    // write full message  
    if(params->state == 1)
    {
      if(regcomp(&re,REGEXSTRINGS[6],REG_EXTENDED) != 0)
      {
        printError(EREGEX);
        regfree(&re);
        return EREGEX;
      }
      // remove from full header last newline
      int status = regexec(&re, head, 2, pmatch2, 0);
      if(status == 0)
      {
        strncpy(header,head+pmatch2[1].rm_so,pmatch2[1].rm_eo-pmatch2[1].rm_so);
        cout << header << "\n";
      }
    }
    // write only parts of message
    else
    {
      // iterate all params
      for(int i = 1; i < 6; i++)
      { 
        if(params->serveri == i)
          printPartOfHead(REGEXSTRINGS[1],head);
        else if(params->length == i)
          printPartOfHead(REGEXSTRINGS[2],head);
        else if(params->modify == i)
          printPartOfHead(REGEXSTRINGS[3],head);
        else if(params->typeobject == i)
          printPartOfHead(REGEXSTRINGS[4],head);          
      }
    }
  }
  // if is page moved
  else if((returncode == 301 || returncode == 302) && (G_MAXREDIRECT > 0))
  {
    // only 5 times redirect inception
    G_MAXREDIRECT--;
    if(regcomp(&re,REGEXSTRINGS[5],REG_EXTENDED) != 0)
    {
      printError(EREGEX);
      regfree(&re);
      return EREGEX;
    }

    // get new url address
    int status = regexec(&re, head, 2, pmatch3, 0);
    if(status == 0)
    {
      strncpy(urllink,head+pmatch3[1].rm_so,pmatch3[1].rm_eo-pmatch3[1].rm_so);
      client(params,urllink);
    }
  }
  // else is some other error/message
  else if((returncode >= 400 && returncode < 600))
  {
    strncpy(errormsg,head+pmatch[2].rm_so,pmatch[2].rm_eo-pmatch[2].rm_so);
    cerr << "Chyba: " << returncode <<errormsg << "\n";
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
int client(TParams *param, char *url)
{
  // init variables, socket, ...
  int mysocket;
  sockaddr_in address;
  hostent *server;
  string url1("");
  string url2("");
  string url3("");
  string url4("");
  int port = G_HTTPPORT;

  string repaired = repairLink(url);
  int value = getPropperLink(repaired.c_str(),&url1,&url2,&port);
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

  // create HEAD message
  string msg("HEAD ");
  msg.append(url2);
  msg.append(" HTTP/1.1\r\nHost: ");
  msg.append(url3+"\r\n");
  //msg.append("Connection: close\r\n");
  msg.append("\r\n\r\n"); 

  // send HEAD message to server
  if(write(mysocket, msg.c_str(), msg.size()) < 0)
  {
    printError(EWMSG);
    close(mysocket);
    return EWMSG;
  }

  // receiving data from server
  int readed = 0; char buffer[G_BUFFER_2];
  bzero(buffer,G_BUFFER_2);
  if((readed = read(mysocket, buffer, 1024)) < 0)
  {
    printError(ERMSG);
    close(mysocket);
    return ERMSG;
  }

  close(mysocket);
  // process head message
  return parsePrintHead(buffer,param);
}

/**
 * The main function.
 * @param argc Counts of params.
 * @param argv Params of program.
 * @return successful of program.
 */
int main(int argc, char **argv)
{
  TParams param = processParams(argc, argv);
  if(param.error != 0) {printError(EBADPARAMS); return EBADPARAMS;}

  // starting client
  int result = client(&param, argv[param.urlindex]);
  return result;
}
/*** End of file webinfo.cpp ***/
