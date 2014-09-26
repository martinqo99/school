/**
 * Author: Michal Lukac, xlukac05@stud.fit.vutbr.cz
 * Date: 29.9.2012
 * Description: This is the project for the ISA course 2012/2013
 *              at BUT FIT. This program detects the path MTU of
 *              selected address. Works with IP4 and IP6! Because
 *              of creating RAW SOCKETS, please run program with
 *              root rights.
 *
 * IMPLEMENTED AND TESTED ON LINUX UBUNTU 12!
 */

/**
 * In this project I used this sources:
 * A.Wesley: Unix network programming, 3rd edition
 * http://tools.ietf.org/html/rfc792,4443,1191,1981,2923,4821,1885,1071,1191,3542
 * http://www.networksorcery.com/enp/protocol/icmpv6.htm
 * http://www.root.cz/clanky/sokety-a-c-raw-soket/
 * http://www.root.cz/clanky/sokety-a-c-mtu-a-ip-fragmentace/
 * http://www.root.cz/clanky/sokety-a-c-prijimani-icmp-paketu/
 * http://en.wikipedia.org/wiki/Path_MTU_discovery
 */

#include <iostream>
#include <string>
#include <stdlib.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#include <sys/time.h>

using namespace std;

/** Constants */
#define ETHERMAXMTU 1500
#define IPV4VMAXMTU 65535
#define IPV6VMAXMTU 65575
#define IPV4VMINMTU 68
#define IPV6VMINMTU 1280
#define IP_HEADER_LEN 40
#define TIMEOUT 3
#define MYHOPLIMIT 30

/** Error codes of program. */
enum tecodes
{
  EOK = 0,       /* Everything is ok. */
  ESOCKET = 1,   /* Error with creating socket. */
  EPARAM = 2,    /* Error with parameters. */
  EADDRESS = 3,  /* Error with address. */
  EUNREHOP = 4,  /* Error with destination unreach or hop limit exceeded. */
  EPACKETUNE = 5,/* Unexpected icmp packet occurs. */
  ESOCKOPT = 6,  /* Error with seting socket options. */
  EPACKETS = 7   /* Error with packet size settings.(-m) */
};

/** Error messages equals to tecodes. */
const char *ECODEMSG[] =
{
  "Everything is ok.\n",
  "Error with creating socket.\n",
  "Error with parameters.\n",
  "Error with address.\n",
  "Error with destination unreach or hop limit exceeded.\n",
  "Unexpected icmp packet occurs.\n",
  "Error with seting socket options.\n",
  "Error with packet size settings.(-m)\n",
};

/** Structure for parameters */
typedef struct param
{
  int maxmtu;
  int error;
} TParams;

/** Network things */
hostent *g_server;
addrinfo hints;
addrinfo *res = NULL;
addrinfo hintslocal;
addrinfo *reslocal = NULL;

/** Prototypes of function. */
void printError(int error);
void errorExit(int error);
int getAddress(char *address);
int findMTU4(int max);
int findMTU6(int max);
void printMTU();
TParams processParams(int argc, char **argv);

/**
 * Print help.
 */
void printHelp()
{
  cout << "Program mymptud, for discovering PATH MTU.\n";
  cout << "Please run on linux ubuntu 12+\n";
  cout << "Author: Michal Lukac\n";
  cout << "Parameters:\n";
  cout << "          -m (optional) max size of datagram\n";
  cout << "          address/hostname\n";
  return;
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
 * Function handle errors and exit properly program.
 * @param error Index for ECODEMSG.
 */
void errorExit(int error)
{
  printError(error);
  exit(1);
}

//////////////////////////////////////////////////////////////////////////////
// This function is from: bloof.de/tcp_checksumming
unsigned short checksum(unsigned short *ptr, int length)
{
  register int sum = 0;
  u_short answer = 0;
  register u_short *w = ptr;
  register int nleft = length;

  while(nleft > 1){
    sum += *w++;
    nleft -= 2;
  }

  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  answer = ~sum;
  return(answer);
}
/////////////////////////////////////////////////////////////////////////////

/**
 * Proces params of program, called from main function.
 * @return Processed params.
 */
TParams processParams(int argc, char **argv)
{
  TParams param;
  param.maxmtu = -1;
  param.error = EOK;

  if(argc == 2)
  {
    // if is help
    if(strcmp(argv[1],"-h") == 0)
    {
      printHelp();
      exit(0);
    }
    param.error = getAddress(argv[1]);
    if(param.error == EADDRESS)
      param.error = EADDRESS;
  }
  else if(argc == 4)
  {
    // if is max size
    if(strcmp(argv[1],"-m") == 0)
      param.maxmtu = atoi(argv[2]);
    else{param.error = EPARAM;}
    param.error = getAddress(argv[3]);
    if(param.error == EADDRESS)
      param.error = EADDRESS;
  }
  else{param.error = EPARAM;}
  return param;
}

/**
 * Return version of ip address and set address.
 * @param address in string
 * @return version of ip address
 */
int getAddress(char *address)
{
  char ipstr[128];
  char ipstr2[128];
  char hostname[255];

  bzero(&hints,sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  int code = getaddrinfo(address, NULL, &hints, &res);
  if (code != 0)
    return EADDRESS;

  // lets get ipv6 address
  if(res->ai_family != AF_INET)
  {
    bzero(&hintslocal,sizeof(hintslocal));
    hintslocal.ai_family = AF_INET6;
    code = getaddrinfo ("::1", NULL, &hintslocal, &reslocal);

    inet_ntop(AF_INET6, &res->ai_addr, ipstr2, sizeof(ipstr2));
    inet_ntop(AF_INET6, &reslocal->ai_addr, ipstr, sizeof(ipstr));
  }
  // if ipv4
  else
  {
    bzero(&hintslocal,sizeof(hintslocal));
    hintslocal.ai_family = AF_INET;
    code = getaddrinfo ("127.0.0.1", NULL, &hintslocal, &reslocal);

    inet_ntop(AF_INET, &res->ai_addr, ipstr2, sizeof(ipstr2));
    inet_ntop(AF_INET, &reslocal->ai_addr, ipstr, sizeof(ipstr));

    gethostname(hostname,sizeof(hostname));
    g_server = gethostbyname(address);
  }
  return EOK;
}

/**
 * Reduce size of packet.
 * @param min minimum value
 * @param packetsize
 */
int reducePacketSize(int min,int *max,int packetsize)
{
  *max = packetsize;
  return (min + *max)/2;
}

/**
 * Reduce size of packet.
 * @param max maximum value
 * @param packetsize
 */
int risePacketSize(int *min, int max, int packetsize)
{
  *min = packetsize;
  return (*min + max)/2;
}

/**
 * Get and Print Max Trasmission Unit for ipv4
 * @param max maximum size of packet
 * @return error code
 */
int findMTU4(int max)
{
  cout << "In ipv4 version\n";
  // help variables
  int ON = 1;
  int countSeq = 1;
  char buffer[IPV6VMAXMTU];
  socklen_t size;
  fd_set socketsFD;
  struct timeval timeout;
  int mypid = getpid();

  // set minimum and maximum size of packet
  int maxs = ETHERMAXMTU+1;
  int mins = IPV4VMINMTU;

  // if is param -m set
  if(max != -1)
  {
    if(max < IPV4VMINMTU)
      return EPACKETS;
    else
      maxs = max + 1;
  }

  // lets create headers of ip and icmp
  iphdr *ipheaderS;       // own ip packet with TTL = 30 and DF flag
  icmphdr *icmpheaderS;   // for ECHO REQUEST
  iphdr *ipheaderR;       // receive
  icmphdr *icmpheaderR;   // receive

  // Let's set address of dest.server
  struct sockaddr_in address, recvaddress;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(0);

  // IPV4
  int socketIP;
  if((socketIP = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP)) < 0)
    return ESOCKET;

  // set that program sets own packets
  if(setsockopt(socketIP, IPPROTO_IP, IP_HDRINCL,&ON, sizeof(ON)) < 0)
    return ESOCKOPT;

  // set mtu discovery
  int okk = IP_PMTUDISC_PROBE;
  setsockopt(socketIP, IPPROTO_IP, IP_MTU_DISCOVER, &okk, sizeof(okk));

  int temp;
  // binary search
  int packetsize = (mins + maxs) / 2;
  while(1)
  {
    // clear buffer
    bzero(&buffer,IPV4VMAXMTU);

    // set timeout
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    FD_ZERO(&socketsFD);
    FD_SET(socketIP, &socketsFD);

    // create IPV4 header and don't fragment it
    ipheaderS = (iphdr *)malloc(packetsize);
    icmpheaderS = (icmphdr *)((char *)ipheaderS +sizeof(iphdr));
    memset((void *) ipheaderS, 0, packetsize);

    // set settings according to root.cz and A.Wesley: Unix network programming
    ipheaderS->version = 4;
    ipheaderS->ihl = 5;
    ipheaderS->tos = 0;
    ipheaderS->tot_len = htons(packetsize);
    ipheaderS->id = 0;
    ipheaderS->frag_off = htons(16384);     //IP_DF == DONT_FRAGMENT
    ipheaderS->ttl = MYHOPLIMIT;
    ipheaderS->protocol = IPPROTO_ICMP;
    ipheaderS->check = 0;
    ipheaderS->saddr = 0;
    ipheaderS->daddr = *((unsigned long int*)g_server->h_addr);

    // create ICMP header with echo request
    icmpheaderS->type = ICMP_ECHO;
    icmpheaderS->code = 0;
    icmpheaderS->checksum = 0;
    icmpheaderS->un.echo.id = htons(mypid);
    icmpheaderS->un.echo.sequence = countSeq;
    icmpheaderS->checksum = checksum((unsigned short *)icmpheaderS, packetsize - sizeof(iphdr));

    cout << "Try to send: " << packetsize << "\n";
    // sending
    if(sendto(socketIP, (char *)ipheaderS, packetsize, 0, res->ai_addr, res->ai_addrlen) < 0)
      packetsize = reducePacketSize(mins,&maxs,packetsize);
    else
    {
      // wait three seconds
      if(select(socketIP+1, &socketsFD, NULL, NULL, &timeout) > 0)
      {
        if(FD_ISSET(socketIP,&socketsFD))
        {
          // receive
          if(recvfrom(socketIP, buffer, IPV4VMAXMTU, 0, (struct sockaddr *)&recvaddress, &size) > 0)
          {
            ipheaderR = (iphdr*)buffer;
            icmpheaderR = (icmphdr *)(buffer + ipheaderR->ihl * 4);
            
            // is REPLY from server?
            if(icmpheaderR->type == ICMP_ECHOREPLY)
            {
              //cout << mypid << " " << ntohs(icmpheaderR->un.echo.id) << endl;
              // check id
              if(mypid == ntohs(icmpheaderR->un.echo.id))
              {
                //cout << countSeq << " " << (icmpheaderR->un.echo.sequence) << endl;
                if(countSeq == icmpheaderR->un.echo.sequence)
                {
                  temp = abs(mins - maxs);
                  if (temp <= 1)
                  {
                    if(max == -1)
                      cout << "resume: " << packetsize << "\n";
                    else if(max < packetsize)
                      cout << "resume: " << max << "\n";
                    else
                      cout << "resume: " << packetsize << "\n";

                    free(ipheaderS);
                    close(socketIP);
                    return EOK;
                  }
                  packetsize = risePacketSize(&mins,maxs,packetsize);
                }
              }
            }
            // check if is some weird type
            else if(icmpheaderR->code == ICMP_FRAG_NEEDED)
              packetsize = reducePacketSize(mins,&maxs,packetsize);
            else if(icmpheaderR->code == ICMP_DEST_UNREACH)
            {
              free(ipheaderS);
              close(socketIP);
              return EUNREHOP;
            }
          }
        }
      }
      else
        cout << "Time expired!...3sec\n";
    }
    countSeq += 1;
    free(ipheaderS);
  }
  close(socketIP);
  return EOK;
}

/**
 * Get and Print Max Trasmission Unit for ipv6
 * @param max maximum size of packet
 * @return error code
 */
int findMTU6(int max)
{
  cout << "In ipv6 version\n";
  struct timeval timeout;
  fd_set socketsFD;

  // set minimum and maximum size of packet
  int length = 0;
  int mins = IPV6VMINMTU;
  int maxs = ETHERMAXMTU+1;
  char buffer[IPV6VMAXMTU];
  int socketIP;
  socklen_t size;
  int countSeq = 1;
  int temp = 0;
  int mypid = getpid();

  // Let's set address of dest.server
  struct sockaddr_in6 recvaddress;

  // lets create headers of ip and icmp
  icmp6_hdr *icmpheaderS;   // for ECHO REQUEST
  icmp6_hdr *icmpheaderR;

  // create raw socket
  if((socketIP = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) < 0)
    return ESOCKET;

  // set hop limit
  int hops = MYHOPLIMIT;
  setsockopt(socketIP, IPPROTO_IPV6, IPV6_HOPLIMIT, &hops, sizeof(hops));
  setsockopt(socketIP, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &hops, sizeof(hops));
  setsockopt(socketIP, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hops, sizeof(hops));

  // set mtu discovery
  int okk = IPV6_PMTUDISC_PROBE;
  setsockopt(socketIP, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &okk, sizeof(okk));

  // if is param -m set
  if(max != -1)
  {
    if(max < IPV6VMINMTU)
      return EPACKETS;
    else
      maxs = max + 1;
  }

  int packetsize = (mins + maxs) / 2 - IP_HEADER_LEN;
  while(1)
  {
    // clear buffer
    bzero(&buffer,IPV6VMAXMTU);

    // select
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    FD_ZERO(&socketsFD);
    FD_SET(socketIP, &socketsFD);

    // checksum
    icmpheaderS = (icmp6_hdr *)malloc(packetsize);
    bzero(icmpheaderS,packetsize);
    icmpheaderS->icmp6_type = ICMP6_ECHO_REQUEST;
    icmpheaderS->icmp6_code = 0;
    icmpheaderS->icmp6_id = mypid & 0xffff;
    icmpheaderS->icmp6_seq = countSeq;
    icmpheaderS->icmp6_cksum = 0;

    cout << "Try to send: " << packetsize + IP_HEADER_LEN << "\n";
    // sending
    if(sendto(socketIP, (char *)icmpheaderS, packetsize, 0, res->ai_addr, res->ai_addrlen) < 0)
      packetsize = reducePacketSize(mins,&maxs,packetsize);
    else
    {
      // wait three seconds
      if(select(socketIP+1, &socketsFD, NULL, NULL, &timeout) > 0)
      {
        if(FD_ISSET(socketIP,&socketsFD))
        {
          // receive
          if((length =recvfrom(socketIP, buffer, IPV6VMAXMTU, 0,(struct sockaddr *)&recvaddress, &size)) > 0)
          {
            icmpheaderR = (icmp6_hdr *)buffer;
            // if is packet too big
            if(((uint32_t)icmpheaderR->icmp6_type) == ICMP6_PACKET_TOO_BIG)
            {
              packetsize = htonl(icmpheaderR->icmp6_mtu)-IP_HEADER_LEN;
              maxs = packetsize;
            }
            else if(((uint32_t)icmpheaderR->icmp6_type) == ICMP6_ECHO_REPLY)
            {
              // if is good type
              if(icmpheaderR->icmp6_id == mypid)
              {
                temp = abs(mins - maxs);
                if (temp <= 1)
                {
                  if(max == -1)
                    cout << "resume: " << packetsize + IP_HEADER_LEN << "\n";
                  else if(max < (packetsize + IP_HEADER_LEN))
                    cout << "resume: " << max << "\n";
                  else
                    cout << "resume: " << packetsize + IP_HEADER_LEN << "\n";

                  free(icmpheaderS);
                  close(socketIP);
                  return EOK;
                }
                // get high
                packetsize = risePacketSize(&mins,maxs,packetsize);
              }
            }
            // if is destination unreach
            else if(((uint32_t)icmpheaderR->icmp6_type) == ICMP6_DST_UNREACH )
            {
              free(icmpheaderS);
              close(socketIP);
              return EUNREHOP;
            }
          }
        }
      }
      else
        cout << "Time expired!...3sec\n";

      // free
      free(icmpheaderS);
      countSeq += 1;
    }
  }
  close(socketIP);
  return EOK;
}

/**
 * Free malloced address.
 */
void freeAddr()
{
  if(res != NULL) freeaddrinfo(res);
  if(reslocal != NULL) freeaddrinfo(reslocal);
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
  int error;

  if(param.error > 0)
  {
    printError(param.error);
    freeAddr();
    return EXIT_FAILURE;
  }

  // ipv4 or ipv6
  if(res->ai_family == AF_INET)
    error = findMTU4(param.maxmtu);
  else
    error = findMTU6(param.maxmtu);
  freeAddr();

  if(error)
  {
    printError(error);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
/** end of file main.c */
