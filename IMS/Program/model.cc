/**
 * Project: Simulation of Web Server
 * Author: Michal Lukac, xlukac05@stud.fit.vutbr.cz
 *         Jakub Sznapka, xsznap01@stud.fit.vutbr.cz
 * Description: This is the project to IMS course at BUT FIT 2012/2013.
 *              This project simulates Server with web+database, system
 *              and user processes. The Server is 1-N core/processor computer.
 *              We assume that web server can be concurrent.
 *              Run program with param -h to get help about starting params.
 */

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "simlib.h"

using namespace std;

#define DEBUG 1

#define SYSPROCS      270   // system processes
#define PROCESSORS    8     // processors
#define CONCWEBSERVS  6     // max number of concurrent web servers
#define HOURS         1     // length of simulation
#define MEMCACHE      33    // min percents of memcached queries
#define MAXMEMCACHE   99    // max percents of memcached queries
#define MIMMEMCACHE   0     // memory speed coeficient
#define DAYUSER       125   // day user 
#define NIGHTUSER     333   // nigh user income time in ms
#define DISKTYPE      0     // default is first seagate

// 0 = 7200 Seagate Barracuda xT 3TB
// 1 = Seagate Barracuda Cheetah 15k7
// 2 = Intel SSD DC S3700 800 GB
const int diskType[] = {0,1,2};
const int diskSpeed[] = {123,172,497};         // in MB/S
const int diskSpeedOnDB[] = {49,32,6};         // in ms on 0.8mb
const int diskSpeedMem[] = {16,9,2};           // similiar, without database query, we use disk for static data, other data are in sql or memcached
const int accesDiskTime[] = {9,4,0};           // disk acces time

Facility *Processors;
Facility Disk("Disk");
Store WebServers("WebServers",CONCWEBSERVS);      // concurrent web server process
Facility DatabaseServer("Database system");       // process of database
Queue WaitProcesses("Waiting processes");         // one wait que for all process

Histogram timeInWebsite("Time spent on loading website", 0, 10, 20);
Histogram timeInSys("Time spent on processing system process", 0, 5, 20);
Histogram timeInUsr("Time spent on processing user process", 0, 40, 20);

int doWeb = 0;        // How many iterations on web server 
int processesWeb = 0; // Web users comming to server
int dbCache = 0;      // Pass through database cache + disk part for static resources
int dbWeb = 0;        // Pass through database + disk part for static resources
int memCache = 0;     // Pass through memcached + disk part for static resources

int doSys = 0;        // How many iterations of system processes
int processesSys = 0; // System processes worked on server
int diskSys = 0;      // Pass through disk part
int memSys = 0;       // Pass through RAM part

int doUsr = 0;        // How many iterations of user processes
int processesUsr = 0; // User processes worked on server
int dbUsr = 0;        // Pass through database + disk part
int diskUsr = 0;      // Pass through disk part

int processesAll = 0; // All processes in system

// global variable for save params from console.
typedef struct params
{
  int webrequest;
  int disktype;
  int sysprocs;
  int processors;
  int concwebservs;
  int hours;
  int memcache;
  int memspeed;
} TParams;
TParams modelParams;

/** Error codes of program. */
enum tecodes
{
  EOK = 0,
  EBADPARAMS = 1 // error in params, maybe negative numbers,...
};

const char *ECODEMSG[]=
{
  "EOK\n",
  "Error with parameters.\n",
};

/**
 * Print an error and exit program.
 */
void error(int code)
{
  cout << ECODEMSG[code];
  exit(0);
}

/**
 * Print help message.
 */
void printHelp()
{
  cout << "Simulation of SERVER\n";
  cout << "Authors: Michal Lukac, xlukac05\n";
  cout << "         Jakub Sznapka, xsznap01\n";
  cout << "Description: This is project at BUT FIT to IMS course 2012/2013.\n";
  cout << "Run program with this params: \n";
  cout << "  -h      | help message       \n";
  cout << "  -d INT  | 0 = HDD 7200, 1 = HDD 15000, 2 = SSD Intel\n";
  cout << "  -p INT  | Count of processors \n";
  cout << "  -s INT  | Count of system processes\n";
  cout << "  -ho INT | Number of hours of simulation\n";
  cout << "  -m INT  | Percents of memcached queries (0-99)\n";
  exit(0);
}

/**
 * Process params of model from console and set them to structure.
 * @param argc Count of arguments.
 * @param argv Arguments.
 */
void processParams(int argc, char **argv)
{
  // this is default set
  modelParams.webrequest = DAYUSER;
  modelParams.disktype = DISKTYPE;
  modelParams.sysprocs = SYSPROCS;
  modelParams.processors = PROCESSORS;
  modelParams.concwebservs = CONCWEBSERVS;
  modelParams.hours = HOURS;
  modelParams.memcache = MEMCACHE;
  //modelParams.memspeed = MEMSPEED;
  if(argc > 1)
  {
    for(int i = 1; i < argc; i++)
    {
      if(strcmp("-h",argv[i]) == 0)
        printHelp();
      else if((strcmp("-p",argv[i]) == 0) && (i+1 < argc))
      {
        modelParams.processors = atoi(argv[i+1]);
        if(modelParams.processors <= 0) error(EBADPARAMS);
      }
      else if((strcmp("-w",argv[i]) == 0) && (i+1 < argc))
      {
        modelParams.concwebservs = atoi(argv[i+1]);
        if(modelParams.concwebservs <= 0) error(EBADPARAMS);
      }
      else if((strcmp("-s",argv[i]) == 0) && (i+1 < argc))
      {
        modelParams.sysprocs = atoi(argv[i+1]);
        if(modelParams.sysprocs <= 0) error(EBADPARAMS);
      }
      else if((strcmp("-ho",argv[i]) == 0) && (i+1 < argc))
      {
        modelParams.hours = atoi(argv[i+1]);
        if(modelParams.hours <= 0) error(EBADPARAMS);
      }
      else if((strcmp("-m",argv[i]) == 0) && (i+1 < argc))
      {
        modelParams.memcache = atoi(argv[i+1]);
        if(modelParams.memcache <= 0) error(EBADPARAMS);
      }
      else if((strcmp("-ms",argv[i]) == 0) && (i+1 < argc))
      {
        modelParams.memspeed = atoi(argv[i+1]);
        if(modelParams.memspeed <= 0) error(EBADPARAMS);
      }
      else if((strcmp("-d",argv[i]) == 0) && (i+1 < argc))
      {
        modelParams.disktype = atoi(argv[i+1]);
        if(modelParams.disktype < 0 || modelParams.disktype > 2) error(EBADPARAMS);
      }
    }
  }
}

/**
 * Web requests to server.
 */
class WebRequest : public Process
{
  double IncomeTime;
  double TimeProcessWebReq;
  double ServeTime;

  void Behavior()
  {
    IncomeTime = Time;
    repeatWEB:
    int san = -1;

    for(int a = 0; a < modelParams.processors; a++)
    {
      if(!Processors[a].Busy())
        san = a;
    }

    if(san!=-1)
    {
      Seize(Processors[san]);
      doWeb++;
    }
    else
    {
      WaitProcesses.Insert(this);
      Passivate();
      goto repeatWEB;
    }

    Enter(WebServers,1);
    Seize(Disk);
    double r = Random()*100;

    // use database
    if(r <= (100 - modelParams.memcache))
    {
      Seize(DatabaseServer);
      r = Random();
      if(r <= 0.66)
      {
        Wait(Uniform(diskSpeedOnDB[modelParams.disktype]-1,diskSpeedOnDB[modelParams.disktype]+20));
        dbWeb++;
      }
      else
      {
        Wait(Uniform(diskSpeedMem[modelParams.disktype]-1,diskSpeedMem[modelParams.disktype]+20));
        dbCache++;
      }
      Release(DatabaseServer);
    }
    // memcached
    else
    {
      Wait(Uniform(diskSpeedMem[modelParams.disktype]-1,diskSpeedMem[modelParams.disktype]+20));
      memCache++;
    }

    Release(Disk);
    Leave(WebServers,1);
    Release(Processors[san]);
    if(WaitProcesses.Length() > 0)
      (WaitProcesses.GetFirst())->Activate();

    timeInWebsite(Time - IncomeTime);
  }
};

/**
 * Default system processes.
 */
class SystemProcess: public Process
{
  double IncomeTime;
  double ServeTime;

  void Behavior()
  {
    while(1)
    {
      Wait(Uniform(10,500));
      if(Random() < 0.02)
      {
        IncomeTime = Time;
        repeatSYS:
        int san = -1;

        // try to find free processor
        for(int a = 0; a < modelParams.processors; a++)
        {
          if(!Processors[a].Busy())
            san = a;
        }

        if(san!=-1)
        {
          Seize(Processors[san]);
          doSys++;
        }
        else
        {
          // no free processor
          WaitProcesses.Insert(this);
          Passivate();
          goto repeatSYS;
        }

        // if system needs disk
        if(Random() <= 0.32)
        {
          Seize(Disk);
          Wait(Uniform((400/diskSpeed[modelParams.disktype])+accesDiskTime[modelParams.disktype],
                       5000/(diskSpeed[modelParams.disktype])+accesDiskTime[modelParams.disktype]));
          Release(Disk);
          diskSys++;
        }
        // or only RAM
        else
        {
          Wait(Uniform(1,10));
          memSys++;
        }

        Release(Processors[san]);
        if(WaitProcesses.Length() > 0)
          (WaitProcesses.GetFirst())->Activate();

        timeInSys(Time - IncomeTime);
      }
    }
  }
};

/**
 * The process of User on server.
 */
class UserProcess : public Process
{
  double IncomeTime;
  void Behavior()
  {
    IncomeTime = Time;
    repeatUP: // goto jump
    int san = -1;

    // try to find free processor
    for(int a = 0; a < modelParams.processors; a++)
    {
      if(!Processors[a].Busy())
        san = a;
    }

    if(san!=-1)
    {
      Seize(Processors[san]);
      doUsr++;
    }
    else
    {
      // no free processor
      WaitProcesses.Insert(this);
      Passivate();
      goto repeatUP;
    }

    Seize(Disk);
    // do something with database
    if(Random() <= 0.10)
    {
      Seize(DatabaseServer);
      // 1mb - 3mb
      Wait(Uniform((1000/diskSpeed[modelParams.disktype])+accesDiskTime[modelParams.disktype],
                   (3000/diskSpeed[modelParams.disktype])+accesDiskTime[modelParams.disktype]));
      Release(DatabaseServer);
      dbUsr++;
    }
    // some simple operation
    else
    {
      Wait(Uniform(13,80));
      diskUsr++;
    }
    Release(Disk);

    // Release Disk and Processor
    Release(Processors[san]);
    if(WaitProcesses.Length() > 0)
      (WaitProcesses.GetFirst())->Activate();

    timeInUsr(Time - IncomeTime);
  }
};

/**
 * Kind of timeout class. Every X hours change
 * the coeficient of generating user web requests
 * so the model is through day more realistic.
 * Every 12 hours change
 */
class UsersThroughDay : public Process
{
  void Behavior()
  {
    int i = 0;
    modelParams.webrequest = DAYUSER;
    while(1)
    {
      Wait(1*3600*1000);
      i++;
      if(i == 12)
      {
        if(modelParams.webrequest == DAYUSER)
          modelParams.webrequest = NIGHTUSER;
        else
          modelParams.webrequest = DAYUSER;
        i = 0;
      }
    }
  }
};

/**
 * This class generates Web Request processes to server.
 */
class GeneratorWeb : public Event
{
  void Behavior()
  {
    (new WebRequest)->Activate();
    Activate(Time+Exponential(modelParams.webrequest));
    processesWeb++;
    processesAll++;
  }
};

/**
 * This class generates Users which works on server
 * possible with ssh,...
 */
class GeneratorUser : public Event
{
  void Behavior()
  {
    (new UserProcess)->Activate();
    Activate(Time+Exponential(10000));
    processesUsr++;
    processesAll++;
  }
};

/**
 * Main function of model.
 * @param argc count of arguments.
 * @param argv paraams from command line.
 */
int main(int argc, char **argv)
{
  processParams(argc,argv);
  RandomSeed(time(NULL));

  // create processors
  Processors = new Facility[modelParams.processors];
  
  // print info message
  if(DEBUG)
  {
    cout << "IMS, SERVER SIMULATION!\n";
    cout << "Disk type: " << modelParams.disktype << "\n";
    cout << "Processors: " << modelParams.processors << "\n";
    cout << "System processes: " << modelParams.sysprocs << "\n";
    cout << "Percents of Memcache in web services: " << modelParams.memcache << "\n";
    cout << "Concurrent WEBServer processes " << modelParams.concwebservs << "\n";
    cout << "Length of simulation: " << modelParams.hours << "[hours]\n";
  }

  // initialize one hour in milliseconds
  SetOutput("model.out");
  Init(0,modelParams.hours*3600*1000);

  // timeout
  (new UsersThroughDay)->Activate();

  // lets generate some default system processes
  for(int a = 0; a < modelParams.sysprocs; a++)
    (new SystemProcess)->Activate();

  // generating web request and user processes
  (new GeneratorWeb)->Activate();
  (new GeneratorUser)->Activate();

  Disk.Clear();
  Run();

  double avg = 0;
  for(int a = 0; a < modelParams.processors; a++)
  {
    Print("Processor %d\n",a);
    Processors[a].Output();
    avg +=  Processors[a].tstat.MeanValue();
  }

  avg = avg / modelParams.processors;
  Print("Average utilization of all processors: %f\n",avg);

  Disk.Output();
  DatabaseServer.Output();
  WebServers.Output();
  WaitProcesses.Output();

  Print("\nALL Processes %d\n\n",processesAll);
  Print("WEBSITE\n");
  Print("WEB processes: %d\n", processesWeb);
  Print("Iteration on web server: %d\n", doWeb);
  Print("Memcached: %d\n", memCache);
  Print("SQL Cache: %d\n", dbCache);
  Print("SQL DB: %d\n", dbWeb);
  timeInWebsite.Output();

  Print("SYSTEM PROCESS\n");
  Print("SYSTEM processes: %d\n", processesSys);
  Print("Iterations of system processes: %d\n", doSys);
  Print("Disk: %d\n", diskSys);
  Print("Mem: %d\n", memSys);
  timeInSys.Output();

  Print("USER PROCESS\n");
  Print("USER processes: %d\n", processesUsr);
  Print("Iteration of user processes: %d\n", doUsr);
  Print("DB + Disk: %d\n", dbUsr);
  Print("Disk: %d\n", diskUsr);
  timeInUsr.Output();

  Print("Total processes: %d\n", doWeb + doSys + doUsr);

  delete [] Processors;
  return 0;
}

