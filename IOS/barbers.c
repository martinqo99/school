/**
 * Program: barbers
 * Datum  : 10.4.2011
 * @author: Michal Lukac, xlukac05@stud.fit.vutbr.cz
 * @file  : barbers.c
 * @brief : Projekt do predmetu IOS na VUT FIT. Ulohou je synchronizacia
 *          procesov problemu spiaceho holica.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/shm.h>
#include <signal.h>
#include <wait.h>
#include <time.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

/** Konstanty. */
const int PRMSCOUNT = 6;    /**< Pocet parametrov.      */
const int DECIMAL = 10;     /**< Desiatkova sustava.    */
const int TOMSECOND = 1000; /**< Prevod do mil. sekund. */

/** Kody chyb programu. */
enum tecodes
{
  EOK = 0,         /**< Vsetko je v poriadku.                        */
  ECLWRONG = 1,    /**< Chyba pri spracovani parametrov.             */
  EFILEO = 2,      /**< Chyba pri otvarani-zatvarani suboru.         */
  EFORK = 3,       /**< Chyba pri vytvarani procesu.                 */
  ESHARED = 4,     /**< Chyba pri vytvarani-mazani zdielanej pamati. */
  ESEMAP = 5,      /**< Chyba pri vytvarani-ukonceni semafora.       */
};

/** Stavove kody programu. */
enum tstates
{
  CHELP,    /**< Stav pre napovedu. */
  CFILE,    /**< Stav pre subor.    */
  CSTDOUT,  /**< Stav pre stdout.   */
};

/** Chybove spravy odpovedajuce tecodes. */
const char *ECODEMSG[] =
{
  "EOK\n",
  "Chybne parametre prikazoveho riadku!\n",
  "Chyba pri otvarani-zatvarani suboru!\n",
  "Chyba pri vytvarani procesu!\n",
  "Chyba pri vytvarani-mazani zdielanej pamati!\n",
  "Chyba pri inicializaci semaforov\n",
};

/** Nazvy pre semafory. */
char *SEMAP[] =
{
  "smfrxl501xx", "smfrxl511xx", "smfrxl521xx", "smfrxl531xx", "smfrxl541xx",
  "smfrxl551xx", "smfrxl561xx", "smfrxl571xx",
};

/** Sprava napovedy. */
const char *HELPMSG =
  "Program  : barbers\n"
  "Autor    : Michal Lukac, xlukac05@stud.fit.vutbr.cz\n"
  "Info     : Projekt do predmetu IOS na VUT FIT.\n"
  "Parametre: Q GenC GenB N F\n"
  "Popis parametrov: \n"
  "           Q = pocet stoliciek\n"
  "           GenC = rozsah pre generovanie zakaznikov.[ms]\n"
  "           GenB = rozsah pre generovanie doby obsluhy.[ms]\n"
  "           N = pocet vygenerovanych zakaznikov.\n"
  "           F = subor pre vystup.";

/** Struktura pre parametre programu. */
typedef struct params
{
  int error;                     /**< Chybovy stav.     */
  int state;                     /**< Stav programu.    */
  int filew;                     /**< Stdout alebo file.*/
  unsigned long int chairs;      /**< Pocet stoliciek.  */
  unsigned long int t_interval;  /**< Interval GenC.    */
  unsigned long int t_service;   /**< Interval GenB.    */
  unsigned long int customers;   /**< Pocet zakaznikov. */
  int sharedid;                  /**< Proces programu.  */
  pid_t mainpid;                 /**< Id main procesu.  */
  pid_t barberid;                /**< Id holica.        */
  FILE *fw;                      /**< Subor pre zapis.  */
} TParams;

/** Struktura pre zdielanu pamat. */
typedef struct smem
{
  pid_t *pids;      /**< Pids  vytv.  proce.. */
  int allservice;   /**< Pocet vsetkych zakaz.*/
  int action;       /**< Poradie stavu.       */
  int waitroom;     /**< Obsadene stolicky.   */
  int served;       /**< Obsluzeny zakaznici. */
  int created;      /**< Vytvoreny zakaznici. */
} TSmem;

/** Struktura pre semafory. */
typedef struct mutxs
{
  sem_t *mut0;      /**< Semafory 0 pre zamknutie cakarne. */
  sem_t *mut1;      /**< Pockanie na prichod zakaznika.    */
  sem_t *mut2;      /**< Cakanie na barber ready.          */
  sem_t *mut3;      /**< Pockanie na customer ready.       */
  sem_t *mut4;      /**< Pockanie na barber finished.      */
  sem_t *mut5;      /**< Pockanie na customer served.      */
  sem_t *mut6;      /**< Opatovne pockanie na zakaznika.   */
  sem_t *mut7;      /**< Semafor pre vypis spravy.         */
} TMutx;

/** Globalne premenne pre uvolnenie pri chybe.   */
TMutx *g_mutxs;     /**< Ukazatel na semafory.   */
TSmem *g_mems;      /**< Ukazatel na pamat.      */
int g_shmid;        /**< Premenna pre id pamate. */
FILE *g_fr;         /**< Subor.                  */

/** Prototypy funkcii. *//////////////////////////////////////////////////
void printHelp(TParams *param);
void printEcode(int ecode);
void killbarber(int sig);
TParams getParams(int argc, char *argv[]);
void syncBarber(TSmem *shm, TMutx *smphrs, TParams *param);
void syncCustomer(TSmem *shm, TMutx *smphrs, int number, TParams *param);
void errorCleanAll(TMutx *smphrs, TSmem *mem, TParams *param);
int createCustomers(TSmem *shm, TParams *param, TMutx *smphrs);
TSmem *createSharedMem(int *error, TParams *param,char *path);
void unlinkSharedMem(TSmem *memory, int sharedid);
void destroySemaphores(TMutx *smphrs);
int initSemaphores(TMutx *smphrs);
int syncProcess(TParams *param);
void setSignals();
void killMainFromChild(int signal);
void killMain(int signal);
//////////////////////////////////////////////////////////////////////////

/** Funkcia printHelp vypise napovedu. */
void printHelp(TParams *param)
{
  fprintf(param->fw,"%s\n",HELPMSG);
  return;
}

/**
 * Funkcia printEcode vypise chybovy kod.
 * @param ecode Index pre chybovu spravu.
 */
void printECode(int ecode)
{
  fprintf(stderr, "%s", ECODEMSG[ecode]);
  return;
}

/**
 * Funkcia getParams spracuje argumenty z terminalu.
 * @param argc Pocet argumentov.
 * @param argv Argumenty.
 * @return Strukturu so spracovanymy vysledkami.
 */
TParams getParams(int argc, char *argv[])
{
  char *echar;  // Pomocna premmenna pre chybu v strtol().
  TParams param =
  {
    .error = 0, .state = 0, .filew = 0 , .chairs = 0,
    .t_interval = 0, .t_service = 0, .customers = 0, .fw = NULL,
  };

  // Zistenie ci je v programu napoveda.
  if(argc == 2)
  {
    if(argv[1][0] == '-' && argv[1][1] == 'h')
    {
      param.state = CHELP;
      return param;
    }
    else
      param.error = ECLWRONG;
  }
  else if(argc != PRMSCOUNT)
    param.error = ECLWRONG;

  // Spracovanie parametrov, prevod retazca na cislo pomocou strtol().
  if(param.error == EOK)
  {
    param.chairs = strtol(argv[1], &echar, DECIMAL);
    if(errno == ERANGE || *echar != '\0' || argv[1][0] == '-')
      param.error = ECLWRONG;
    param.t_interval = strtol(argv[2], &echar, DECIMAL);
    if(errno == ERANGE || *echar != '\0' || argv[2][0] == '-')
      param.error = ECLWRONG;
    param.t_service = strtol(argv[3], &echar, DECIMAL);
    if(errno == ERANGE || *echar != '\0' || argv[3][0] == '-')
      param.error = ECLWRONG;
    param.customers = strtol(argv[4], &echar, DECIMAL);
    if(errno == ERANGE || *echar != '\0' || argv[4][0] == '-')
      param.error = ECLWRONG;
    param.state = CFILE;

    // Zistenie ci je vystup do suboru.
    if(argv[5][0] == '-' && argv[5][1] == '\0')
      param.filew = CSTDOUT;
    else
      param.filew = CFILE;
  }
  return param;
}

/**
 * Funkcia killbarber ukonci barbra pri chybe.
 * @param sig Typ signalu.
 */
void killbarber(int sig)
{
  destroySemaphores(g_mutxs);
  unlinkSharedMem(g_mems,g_shmid);
  if(g_fr != NULL)
    fclose(g_fr);
  signal(sig, SIG_DFL);
  exit(EXIT_FAILURE);
  return;
}

/** 
 * MainfromChild zasle signal sigterm hlavnemu procesu pri chybe.
 * @param signal Typ signalu.
 */
void killMainFromChild(int sig)
{
  destroySemaphores(g_mutxs);
  unlinkSharedMem(g_mems,g_shmid);
  if(g_fr != NULL)
    fclose(g_fr);
  signal(SIGTERM,SIG_DFL);
  kill(sig,getppid());
  exit(EXIT_FAILURE);
  return;
}

/**
 * Funkcia killMain zasle signal sigterm hlavnemu procesu pri chybe.
 * @param signal Typ signalu.
 */
void killMain(int sig)
{
  destroySemaphores(g_mutxs);
  unlinkSharedMem(g_mems,g_shmid);
  if(g_fr != NULL)
    fclose(g_fr);
  signal(SIGTERM,SIG_DFL);
  kill(sig,getpid());
  exit(EXIT_FAILURE);
  return;
}

/**
 * Funkcia setSignals nastavi zachytenie signalov pre processy.
 * @param mainpid PID mainu.
 */
void setSignals(pid_t mainpid)
{
  if(mainpid == getpid())
  {
    signal(SIGINT, killMain);
    signal(SIGTERM, killMain);
  }
  else
  {
    signal(SIGINT, killMainFromChild);
    signal(SIGTERM, killMainFromChild);
  }
  signal(SIGUSR1,killbarber);
  return;
}

/**
 * Funkcia syncBarber, sluzi na synchronizovanie holica.
 * @param shm Struktura so zdielanou pamatou.
 * @param smphrs Struktura so semaformi.
 * @param param Struktura s parametrami programu.
 */
void syncBarber(TSmem *shm, TMutx *smphrs, TParams *param)
{
  if(shm->allservice == 0)
  {
    setbuf(param->fw, NULL);
    fprintf(param->fw,"%d: barber: checks\n",++(shm->action));
  }
  else
  {
    // Holic musi byt v nekonecnom cykle.
    while(1)
    {
      // Zablokovanie pre vypis spravy.
      sem_wait(smphrs->mut7);
      setbuf(param->fw, NULL);
      fprintf(param->fw,"%d: barber: checks\n",++(shm->action));
      sem_post(smphrs->mut7);

      if(shm->allservice == 0)
        break;

      // Pockanie na prichod zakaznika.
      sem_wait(smphrs->mut1);
      if(shm->served == shm->allservice)
        break;

      // Opatovne pockanie na zakaznika.
      sem_wait(smphrs->mut6);
      if(shm->served == shm->allservice)
        break;

      sem_wait(smphrs->mut7);
      setbuf(param->fw, NULL);
      fprintf(param->fw,"%d: barber: ready\n",++(shm->action));
      sem_post(smphrs->mut7);
      sem_post(smphrs->mut2);
      sem_wait(smphrs->mut3);
      usleep(rand()%(param->t_service + 1)*TOMSECOND);
      sem_wait(smphrs->mut7);
      setbuf(param->fw, NULL);
      fprintf(param->fw,"%d: barber: finished\n",++(shm->action));
      sem_post(smphrs->mut7);

      // Odblokovanie semaforu pre zakaznika.
      sem_post(smphrs->mut4);

      // Pockanie na "customer served".
      sem_wait(smphrs->mut5);
      if(shm->served == shm->allservice)
        break;
    }
  }
  return;
}

/**
 * Funkcia syncCustomer, sluzi na synchronizovanie holica.
 * @param shm Struktura so zdielanou pamatou.
 * @param smphrs Struktura so semaformi.
 * @param number Id zakaznika.
 * @param param Struktura obsahujuca subor pre zapis.
 */
void syncCustomer(TSmem *shm, TMutx *smphrs, int number, TParams *param)
{
  // Zablokovanie vstupu do cakarne.
  sem_wait(smphrs->mut0);

  // Zablokovanie vypisu.
  sem_wait(smphrs->mut7);
  setbuf(param->fw, NULL);
  fprintf(param->fw,"%d: customer %d: enters\n",++(shm->action),number);
  // Odblokovanie vypisu.
  sem_post(smphrs->mut7);

  // Pokial je miesto v cakarni.
  if(shm->waitroom > 0)
  {
    shm->waitroom -= 1;
    sem_post(smphrs->mut6);
    sem_post(smphrs->mut1);
    // Odblokovanie vstupu do cakarne.
    sem_post(smphrs->mut0);

    // Pockanie na "barber rady".
    sem_wait(smphrs->mut2);
    sem_wait(smphrs->mut7);
    shm->waitroom += 1;
    setbuf(param->fw, NULL);
    fprintf(param->fw,"%d: customer %d: ready\n",++(shm->action),number);
    sem_post(smphrs->mut7);
    sem_post(smphrs->mut3);

    // Pockanie na "barber finished".
    sem_wait(smphrs->mut4);
    sem_wait(smphrs->mut7);
    setbuf(param->fw, NULL);
    fprintf(param->fw,"%d: customer %d: served\n",++(shm->action),number);
    shm->served += 1;
    sem_post(smphrs->mut7);

    // Odblokovanie semaforu pre holica.
    sem_post(smphrs->mut5);
  }
  // Pokial nie je miesto v cakarni.
  else
  {
    sem_wait(smphrs->mut7);
    setbuf(param->fw, NULL);
    fprintf(param->fw,"%d: customer %d: refused\n",++(shm->action),number);
    sem_post(smphrs->mut7);
    shm->served += 1;
    sem_post(smphrs->mut1);

    // Odblokovanie vstupu do cakarne.
    sem_post(smphrs->mut0);
    if(shm->served == shm->allservice)
      sem_post(smphrs->mut6);
  }
  return;
}

/**
 * Funkcia errorCleanAll odstrani vytvorene procesy, semafory a pamat.
 * @param error Premenna urcujuca typ chyby.
 * @param smphrs Struktura pre semafory.
 * @param mem  Zdielana struktura.
 * @param param Struktura s doplnkovymi parametrami.
 */
void errorCleanAll(TMutx *smphrs, TSmem *mem, TParams *param)
{
  destroySemaphores(smphrs);
  unlinkSharedMem(mem,param->sharedid);
  fclose(param->fw);
  kill(param->mainpid, SIGTERM);
  exit(EXIT_FAILURE);
}

/**
 * Funkcia createCustomers vytvara zakaznikov.
 * @param shm Struktura zdielanej pamati.
 * @param param Struktura s parametrami programu.
 * @param smphrs Struktura so semaformi.
 */
int createCustomers(TSmem *shm, TParams *param, TMutx *smphrs)
{
  pid_t pid;                // PID pre zakaznika.
  int customerid = 0;       // Poradove cislo zakaznika.
  signal(SIGCHLD, SIG_IGN); // Ignorovanie sigchld a uvolnovanie procesov.

  // Vytvorenie zakaznikov.
  for(unsigned long int i = 0; i < param->customers; i++)
  {
    usleep(rand()%(param->t_interval+1)*TOMSECOND);
    if((pid = fork()) == -1)
      return EFORK;
    // Volanie zakaznika.
    else if(pid == 0)
    {
      shm->pids[i] = getpid();
      sem_wait(smphrs->mut7);
      setbuf(param->fw, NULL);
      fprintf(param->fw,"%d: customer %d: created\n",++(shm->action),
      ++(shm->created));
      customerid = shm->created;
      sem_post(smphrs->mut7);
      syncCustomer(shm,smphrs,customerid,param);
      unlinkSharedMem(shm,param->sharedid);
      destroySemaphores(smphrs);
      fclose(param->fw);
      exit(EXIT_SUCCESS);
    }
  }
  return EOK;
}

/**
 * Funkcia createSharedMem vytvori zdielanu strukturu pre procesy.
 * @param error Chybovy kod funkcie.
 * @param param Struktura s parametrami programu.
 * @param path Cesta k suboru pre ftok.
 * @return Zdielana struktura.
 */
TSmem *createSharedMem(int *error, TParams *param,char *path)
{
  TSmem *memory = NULL;    // Zdielana struktura.
  key_t key;               // Kluc pre shmget.
  int shmid;               // Id segmentu.

  // Vytvorenie kluca a segmentu pamate.
  if((key = ftok(path,1)) != -1)
  {
    shmid = shmget(key, sizeof(TSmem), 0666|IPC_CREAT);
    if(shmid == -1) *error = ESHARED;
  }
  else
    *error = ESHARED;

  // Spristupnenie pamate.
  if(*error == EOK)
  {
    memory = (TSmem *)shmat(shmid,0,0);
    if(memory == (void *)-1) *error = ESHARED;
  }

  if(*error == EOK)
  {
    if((memory->pids=(pid_t *)malloc(sizeof(pid_t)*param->customers))==NULL)
      *error = ESHARED;
  }

  // Inicializacia hodnot.
  if(*error == EOK)
  {
    param->sharedid = shmid;
    memory->allservice = param->customers;
    memory->served = 0;
    memory->action = 0;
    memory->created = 0;
    memory->waitroom = param->chairs;
  }
  return memory;
}

/**
 * Funkcia unlinkSharedMem odpoji usek zdielanej pamate.
 * @param memory Struktura zdielanej pamate.
 * @param sharedid Id zdielanej pamate.
 */
void unlinkSharedMem(TSmem *memory, int sharedid)
{
  // Uvolnenie pidov.
  free(memory->pids);

  // Odpojenie pamate.
  shmdt((void *)memory);

  // Odstranenie pamate.
  shmctl(sharedid, IPC_RMID, 0);
  return;
}

/**
 * Funkcia destroySemaphores odstrani semafory.
 * @param Struktura obsahujuca semaforov.
 */
void destroySemaphores(TMutx *smphrs)
{
  if(smphrs->mut0 != NULL)
  {
    sem_close(smphrs->mut0);
    sem_unlink(SEMAP[0]);
  }
  if(smphrs->mut1 != NULL)
  {
    sem_close(smphrs->mut1);
    sem_unlink(SEMAP[1]);
  }
  if(smphrs->mut2 != NULL)
  {
    sem_close(smphrs->mut2);
    sem_unlink(SEMAP[2]);
  }
  if(smphrs->mut3 != NULL)
  {
    sem_close(smphrs->mut3);
    sem_unlink(SEMAP[3]);
  }
  if(smphrs->mut4 != NULL)
  {
    sem_close(smphrs->mut4);
    sem_unlink(SEMAP[4]);
  }
  if(smphrs->mut5 != NULL)
  {
    sem_close(smphrs->mut5);
    sem_unlink(SEMAP[5]);
  }
  if(smphrs->mut6 != NULL)
  {
    sem_close(smphrs->mut6);
    sem_unlink(SEMAP[6]);
  }
  if(smphrs->mut7 != NULL)
  {
    sem_close(smphrs->mut7);
    sem_unlink(SEMAP[7]);
  }
  return;
}

/**
 * Funkcia initSemaphores inicializuje semafory, pomocou funkcie sem_open.
 * @param Struktura obsahujuca semafory.
 * @return Chybovy navrat funkcie.
 */
int initSemaphores(TMutx *smphrs)
{
  if((smphrs->mut0=sem_open(SEMAP[0],O_CREAT,S_IRUSR|S_IWUSR,1))==SEM_FAILED)
    return ESEMAP;
  if((smphrs->mut1=sem_open(SEMAP[1],O_CREAT,S_IRUSR|S_IWUSR,0))==SEM_FAILED)
    return ESEMAP;
  if((smphrs->mut2=sem_open(SEMAP[2],O_CREAT,S_IRUSR|S_IWUSR,0))==SEM_FAILED)
    return ESEMAP;
  if((smphrs->mut3=sem_open(SEMAP[3],O_CREAT,S_IRUSR|S_IWUSR,0))==SEM_FAILED)
    return ESEMAP;
  if((smphrs->mut4=sem_open(SEMAP[4],O_CREAT,S_IRUSR|S_IWUSR,0))==SEM_FAILED)
    return ESEMAP;
  if((smphrs->mut5=sem_open(SEMAP[5],O_CREAT,S_IRUSR|S_IWUSR,0))==SEM_FAILED)
    return ESEMAP;
  if((smphrs->mut6=sem_open(SEMAP[6],O_CREAT,S_IRUSR|S_IWUSR,0))==SEM_FAILED)
    return ESEMAP;
  if((smphrs->mut7=sem_open(SEMAP[7],O_CREAT,S_IRUSR|S_IWUSR,1))==SEM_FAILED)
    return ESEMAP;
  return EOK;
}

/**
 * Funkcia syncProcess vytvara procesy a synchronizuje ich.
 * @param param Struktura s vlastnostami pre procesy.
 * @return Chybovy navrat funkcie.
 */
int syncProcess(TParams *param)
{
  int error = EOK;           // Premmena pre chybu pri zdielanej pamati.
  param->mainpid = getpid(); // Proces id hlavneho programu.
  setSignals(getpid());      // Zachytenie signalov.
  signal(SIGCHLD, SIG_IGN);  // Ignorovanie sigchld a uvolnovanie procesov.

  // Inicializacia semaforov
  TMutx semaphores = {.mut0 = NULL, .mut1 = NULL, .mut2 = NULL ,.mut3 = NULL,
  .mut4 = NULL, .mut5 = NULL, .mut6 = NULL, .mut7 = NULL,};
  if(initSemaphores(&semaphores) != EOK)
  {
   destroySemaphores(&semaphores);
   return ESEMAP;
  }
  g_mutxs = &semaphores;

  // Vytvorenie zdielanej pamate.
  TSmem *memory = createSharedMem(&error,param,"/tmp");
  if(error) return ESHARED;
  g_mems = memory;
  g_shmid = param->sharedid;

  // Vytvorenie procesa holica.
  if((param->barberid = fork()) == -1)
  {
    destroySemaphores(&semaphores);
    unlinkSharedMem(memory,param->sharedid);
    return EFORK;
  }
  // Volanie holica.
  else if(param->barberid == 0)
  {
    syncBarber(memory, &semaphores, param);
    destroySemaphores(&semaphores);
    unlinkSharedMem(memory,param->sharedid);
    fclose(param->fw);
    exit(EXIT_SUCCESS);
  }
  // Vytvorenie procesov zakaznikov.
  else if(param->barberid > 0)
  {
    if(createCustomers(memory,param,&semaphores) == EFORK)
    {
      printECode(EFORK);
      kill(param->barberid, SIGUSR1);
      for(int i = 0; i < memory->created; i++)
        kill(memory->pids[0],SIGTERM);
      errorCleanAll(&semaphores,memory,param);
    }

    // Pockanie na proces barbra a uvolnenie zdielanej pamati.
    waitpid(param->barberid, NULL, 0);
    unlinkSharedMem(memory,param->sharedid);
    destroySemaphores(&semaphores);
  }
  return EOK;
}

/**
 * Hlavna funkcia main vola prislusne funkcie programu.
 * @param argc Pocet parametrov programu.
 * @param argv Parametre programu.
 */
int main(int argc, char *argv[])
{
  srand(time(NULL));
  TParams param = getParams(argc,argv);
  g_fr = NULL;

  // Otvorenie suboru.
  if(param.filew == CFILE)
  {
    if((param.fw = fopen(argv[5],"w")) == NULL)
      param.error = EFILEO;
    else
      g_fr = param.fw;
  }
  else
    param.fw = stdout;

  // Ak su parametre v poriadku.
  if(param.error == EOK)
  {
    if(param.state == CHELP)
      printHelp(&param);
    else
      param.error = syncProcess(&param);
  }

  // Pokial je chyba vypis error.
  if(param.error != EOK)
  {
    // Uzavretie suboru.
    if(param.fw != NULL)
      fclose(param.fw);
    printECode(param.error);
    return EXIT_FAILURE;
  }

  // Uzavretie suboru.
  if(param.fw != NULL)
    fclose(param.fw);

  return EXIT_SUCCESS;
}
/*** Koniec suboru barbers.c ***/
