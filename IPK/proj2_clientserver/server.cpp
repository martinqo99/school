/**
 * @author : Michal Lukac, xlukac05@stud.fit.vutbr.cz
 * @date   : 25.2.2012
 * @brief  : Simple c++ "email" server. This is the school project
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
#include <regex.h>
#include <stdlib.h>
#include <pthread.h>

using namespace std;

#define G_BUFFER 2048
/** Mutex for critical section. */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/** Error codes of program. */
enum tecodes
{
  EOK = 0,          /* Everything is ok. */
  EBADPARAMS = 1,   /* Error with params. */
  ESOCKET = 2,      /* Error with creating socket */
  EBIND = 3,        /* Error with binding socket. */
  ELISTEN = 4,      /* Error with listening. */
  EWMSG = 5,        /* Error with writing message to server. */
  ERMSG = 6,        /* Error with reading message from server. */
  EREGEX = 7,       /* Error with regular expression. */
  EACCEPT = 8       /* Error with accepting client. */
};

/** Error messages equals to tecodes. */
const char *ECODEMSG[] =
{
  "Everything is ok.\n",
  "Error with parameters.\n",
  "Error with creating socket.\n",
  "Error with binding socket.\n",
  "Error with listening.\n",
  "Error with writing message to server.\n",
  "Error with reading message from server.\n",
  "Error with regular expression.\n",
  "Error with acepting client.\n",
};

/** Structure for processed arguments of program. */
typedef struct param
{
  int state;
  int error;
  int port;
} TParams;

/** Item for list */
typedef struct tItem {
  struct tItem *lptr;
  struct tItem *rptr;
  char *user;
  char *message;
} TItemPtr;

/** Linked-List */
typedef struct {
  TItemPtr *Actual;
  TItemPtr *First;
  TItemPtr *Last;
} TList;

/** Global pointer for list.Used for threads. */
TList *G_LIST;

/** Prototypes of functions. */
void printError(int error);
void processParams(TParams *param,int argc);
int client(TParams *param, char **argv);
int getPropperLink(const char *link, string *uri, string *speclink, int *port);

/**
 * Initialize linked list.
 */
void InitList(TList *list)
{
  list->Actual = NULL;
  list->First = NULL;  
  list->Last = NULL;
}

/**
 * Dispose linked list.
 */
void DisposeList(TList *list)
{
  if(list->First != NULL)
  {
    TItemPtr *item;
    while(list->First)
    {
      item = list->First;
      list->First = item->rptr;
      free(item->user);
      free(item->message);
      free(item);
    }
  }
  list->Actual = NULL; list->First = NULL; list->Last = NULL;
}

/**
 * Insert new item at end of the list.
 */
void InsertLast(TList *list, string name, string msg)
{
  // create item
  TItemPtr *item = NULL;
  if((item = (TItemPtr *)malloc(sizeof(TItemPtr))) == NULL)
    return;

  // fill item
  int len = name.length();
  item->user = (char *)malloc(((sizeof(char)*len) +1));
  strncpy(item->user,name.c_str(),name.length());
  item->user[len] = '\0';
  len = msg.length();
  item->message = (char *)malloc(((sizeof(char)*len) +1));
  item->message[len] = '\0';
  strncpy(item->message,msg.c_str(),msg.length());

  // if is list empty
  if(list->First == NULL)
  {
    item->lptr = NULL;
    item->rptr = NULL;
    list->First = item;
    list->Last = item;
  }
  // else append at end of list
  else
  {
    item->rptr = NULL;
    item->lptr = list->Last;
    list->Last->rptr = item;
    list->Last = item;
  }
}

/**
 * Print items of list.
 * @param list Linked list
 * @param name User name.
 * @return string of all items.
 */
string SearchPrintList(TList *list, string name)
{
  // print list
  string tosend("");
  TItemPtr *item = list->First;
  while(item != NULL)
  {
    if(strcmp(item->user,name.c_str()) == 0)
    {
      tosend.append(item->message);
      tosend.append("\n");
    }
    item = item->rptr;
  }
  return tosend;  
}

/**
 * Function remove item/message on index.
 * @param list Linked list.
 * @param name user name of message.
 * @param index Index of message of user.
 * @return 1 if item was removed else 0.
 */
int deleteItemList(TList *list, string name, int index)
{
  // go through list
  int indexs = 0;
  list->Actual = list->First;
  while(list->Actual != NULL)
  {
    // if is username equals
    if(strcmp(list->Actual->user,name.c_str()) == 0)
    {
      // if indexs equals
      indexs++;
      if(indexs == index)
      {
        TItemPtr *item = list->Actual;
        // if is item only one in list
        if(item == list->Last && item == list->First)
        {
          list->Last = NULL;
          list->First = NULL;
        }
        // if is item last
        else if(item == list->Last)
        {
          list->Last->lptr->rptr = NULL;
          list->Last = item->lptr;
        }
        // if is item first
        else if(item == list->First)
        {
          list->First = list->First->rptr;
        }
        // else is middle of two items
        else
        {
          item->lptr->rptr = item->rptr;
          item->rptr->lptr = item->lptr;
        }
        // free it
        free(item->user);
        free(item->message);
        free(item);
        return 1;
      }
    }
    // move to next item
    list->Actual = list->Actual->rptr;
  }
  list->Actual = list->First;
  return 0;
}

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
  param->port = 0;

  if(argc == 3)
  {
    if(strcmp("-p",argv[1]) == 0)
    {
      param->port = atoi(argv[2]);
      if(param->port == 0){param->error = 1;}
    }
    else
      param->error = 1;
  }
  else
    param->error = 1;
  return;
}

void *threadClient(void *threadid)
{
  // initial variables
  int indexremove = 0;
  int socket;
  socket = *((int *) threadid);
  char tosend[G_BUFFER];
  char toread[G_BUFFER];
  bzero(toread,G_BUFFER);
  bzero(tosend,G_BUFFER);

  // reading from server
  read(socket,toread,G_BUFFER);

  // process message
  regex_t re;
  regmatch_t pmatch[5];
  char pattern[] = "PROT: proj2#\nTYPE: (..)#\nUSER: ([^#]*)#\nMSG: ([^#]*)#\nDEL: ([[:digit:]]*)#\n";
  char user[100];
  char type[20];
  char msg[G_BUFFER];
  char del[20];
  bzero(user,100);
  bzero(type,20);
  bzero(msg,G_BUFFER);
  bzero(del,20);

  // init regex with pattern
  if(regcomp(&re,pattern,REG_EXTENDED)!= 0)
    pthread_exit(NULL);

  // match url address
  int status = regexec(&re,toread,5,pmatch,0);
  if(status == 0)
  {
    strncpy(type,toread+pmatch[1].rm_so,pmatch[1].rm_eo-pmatch[1].rm_so);
    strncpy(user,toread+pmatch[2].rm_so,pmatch[2].rm_eo-pmatch[2].rm_so);
    strncpy(msg,toread+pmatch[3].rm_so,pmatch[3].rm_eo-pmatch[3].rm_so);
    strncpy(del,toread+pmatch[4].rm_so,pmatch[4].rm_eo-pmatch[4].rm_so);

    if(strlen(del) > 0) {indexremove = atoi(del);}
  }
  else
    pthread_exit(NULL);

  string mess(msg);
  string usr(user);
  int delvalue = 0;

  // critical section
  pthread_mutex_lock (&mutex);
  if(strcmp(type,"-s") == 0)
    InsertLast(G_LIST,usr,mess);
  else if(strcmp(type,"-r") == 0)
  {
    mess = "";
    mess = SearchPrintList(G_LIST,usr);
  }
  else if(strcmp(type,"-d") == 0)
    delvalue = deleteItemList(G_LIST,usr,indexremove);
  pthread_mutex_unlock (&mutex);
  // end of critical section

  // save response to "tosend"
  if(strcmp(type,"-s") == 0)
  {
    strncpy(tosend,"Ok.\n",5);
    tosend[4] = '\0';
  }
  else if(strcmp(type,"-r") == 0)
  {
    strncpy(tosend,mess.c_str(),mess.length());
  }
  else if(strcmp(type,"-d") == 0)
  {
    if(indexremove == 0 or !delvalue)
    {
      strncpy(tosend,"Err.\n",6);
      tosend[5] = '\0';
    }
    else
    {
      strncpy(tosend,"Ok.\n",5);
      tosend[4] = '\0';
    }
  }

  // write response from server
  write(socket,tosend,strlen(tosend));

  // exit thread
  close(socket);
  free(threadid);
  pthread_exit(NULL);
  return NULL;
}


/**
 * This is simple client function.
 * @param param Processed parameters.
 * @param argv Arguments of program.
 * @return Succesful value if is everything ok.
 */
int client(TParams *param)
{ 
  // init variables, socket, ...
  int mysocket;
  sockaddr_in address;
  sockaddr_in client;
  int port = param->port;
  socklen_t addreslength;
  pthread_t thread;
  TList list;
  InitList(&list);

  //SearchPrintList(&list, user);
  G_LIST = &list;
  // create socket
  if((mysocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    printError(ESOCKET);    
    return ESOCKET;
  }

  // create address
  bzero(&address,sizeof(sockaddr_in));
  address.sin_port = htons(port);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;

  // connect to server
  if(bind(mysocket, (sockaddr *)&address, sizeof(address)) == -1)
  {  
    printError(EBIND);
    close(mysocket);
    return EBIND;
  }

  // listening on clients
  if(listen(mysocket,5) < 0)
  {
    printError(ELISTEN);
    close(mysocket);
    return ELISTEN;
  }

  addreslength = sizeof(client);
  int *ret;  

  // inifity server loop
  while(1)
  {
    ret = (int *)malloc(sizeof(int));
    *ret = accept(mysocket, (sockaddr *)&client, &addreslength);
    if(*ret == -1)
      continue;

    pthread_create(&thread, NULL, &threadClient, ret);
  }

  DisposeList(&list);
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
  int result = client(&param);
  return result;
}
/*** End of file server.cpp ***/
