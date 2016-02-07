/*
Author: Nishant Patil 	SJSU ID: 010720918

******Ping program******
complie: $make
		 or
		 $gcc q1.c -o q1 -lpthread

run: $sudo ./q1 <ip/url>

*/
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>

#define	BUFSIZE		1500

			/* globals */
char	 sendbuf[BUFSIZE];

char *servername;
int		 datalen;			/* # bytes of data following ICMP header */
int 	 ttl=32;
char	*host;
int		 nsent;				/* add 1 for each sendto() */
pid_t	 pid;				/* our PID */
int		 sockfd;
int		 verbose;

char			recvbuf[BUFSIZE];
ssize_t			n;
struct msghdr	msg;
struct timeval	tval;


			/* function prototypes */
void	 proc_v4(char *, ssize_t, struct msghdr *, struct timeval *);
//void	 proc_v6(char *, ssize_t, struct msghdr *, struct timeval *);
void	 send_v4(void);
//void	 send_v6(void);
void* 	 threadSend();
void* 	 threadReceive();
void	 readloop(void);
void	 sig_alrm(int);
void	 tv_sub(struct timeval *, struct timeval *);
uint16_t in_chksum(uint16_t *addr, int len);


struct proto {
  void	 (*fproc)(char *, ssize_t, struct msghdr *, struct timeval *);
  void	 (*fsend)(void);

  struct sockaddr_in  *sasend;	/* sockaddr{} for send, from getaddrinfo */
  struct sockaddr_in  *sarecv;	/* sockaddr{} for receiving */
  socklen_t	    salen;		/* length of sockaddr{}s */
  int	   	    icmpproto;	/* IPPROTO_xxx value for ICMP */
} *pr;



//main.c
struct proto	proto_v4 = { proc_v4, send_v4, NULL, NULL, 0, IPPROTO_ICMP };


int	datalen = 56;		/* data that goes with ICMP echo request */

int main(int argc, char **argv)
{
	int	c;
	struct hostent *hp;
	char *h;
	struct sockaddr_in servaddr;
	opterr = 0;		/* don't want getopt() writing to stderr */

	while ( (c = getopt(argc, argv, "v")) != -1) {
		switch (c) {
		case 'v':
			verbose++;
			break;

		case '?':	
			printf("unrecognized option: %c\n", c);
			exit(1);
		}
	}
	//puts("here\n");
	if (optind != argc-1){
		
		printf("usage: ping [ -v ] <hostname>");
			exit(1);
		}
	host = argv[optind];	

	//host = argv[1];

	pid = getpid() & 0xffff;	/* ICMP ID field is 16 bits */
	//signal(SIGALRM, sig_alrm);


	hp = gethostbyname(host);
	memset(&servaddr, '0',sizeof(servaddr));


	servaddr.sin_family = AF_INET;
	servaddr.sin_port	= htons(8000);
	memcpy(&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);
	//printf("Official name: %s\n ",hp->h_name);
	int i;
	servername = inet_ntoa( *( struct in_addr*)( hp -> h_addr_list[0]));
	/*while ( hp -> h_addr_list[i] != NULL) 
	{
    		printf("IP address #%d: %s\n", i, inet_ntoa( *( struct in_addr*)( hp -> h_addr_list[i])));
    		i++;
        }*/
	
	printf("Ping %s(%s) with %d bytes of data: ttl=%d\n", hp->h_name, servername, datalen, ttl);

	pr = &proto_v4;
	pr->sasend = (struct sockaddr_in *)&servaddr;
	pr->sarecv =(struct sockaddr_in *)calloc(1, sizeof(servaddr));
	pr->salen = sizeof(servaddr);
	

	readloop();

	exit(0);
}


//readloop.c
void readloop(void)
{
	int				size;
	//char			recvbuf[BUFSIZE];
	char			controlbuf[BUFSIZE];
	//struct msghdr	msg;
	struct iovec	iov;
	//ssize_t			n;
	//struct timeval	tval;
//	int on = 30;
	pthread_t threads[2];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	//
	
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	setuid(getuid());		/* don't need special permissions any more */
	//if (pr->finit)
		//(*pr->finit)();

	size = 60 * 1024;		/* OK if setsockopt fails */

	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
	
	setsockopt(sockfd, IPPROTO_IP, IP_TTL, (char *)&ttl, sizeof(ttl));
	//setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
	//sig_alrm(SIGALRM);		/* send first packet */
	if(pthread_create(&threads[0], &attr, threadSend, NULL))
	{
		printf("Failed to create threadSend\n");
		exit(1);
	}

	iov.iov_base = recvbuf;
	iov.iov_len = sizeof(recvbuf);
	msg.msg_name = pr->sarecv;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = controlbuf;
	for ( ; ; ) {
		msg.msg_namelen = pr->salen;
		msg.msg_controllen = sizeof(controlbuf);
		n = recvmsg(sockfd, &msg, 0);
		if (n < 0) {
			if (errno == EINTR)
				continue;
			else
				perror("recvmsg error");
		}

		gettimeofday(&tval, NULL);
		//(*pr->fproc)(recvbuf, n, &msg, &tval);
		if(pthread_create(&threads[1], &attr, threadReceive, NULL))
		{
			printf("Failed to create threadReceive\n");
			exit(1);
		}
	}
}


//proc_v4.c
void
proc_v4(char *ptr, ssize_t len, struct msghdr *msg, struct timeval *tvrecv)
{
	int				hlen1, icmplen;
	double			rtt;
	struct ip		*ip;
	struct icmp		*icmp;
	struct timeval	*tvsend;

	ip = (struct ip *) ptr;		/* start of IP header */
	hlen1 = ip->ip_hl << 2;		/* length of IP header */
	if (ip->ip_p != IPPROTO_ICMP)
		return;				/* not ICMP */

	icmp = (struct icmp *) (ptr + hlen1);	/* start of ICMP header */
	if ( (icmplen = len - hlen1) < 8)
		return;				/* malformed packet */

	if (icmp->icmp_type == ICMP_ECHOREPLY) {
		if (icmp->icmp_id != pid)
			return;			/* not a response to our ECHO_REQUEST */
		if (icmplen < 16)
			return;			/* not enough data to use */

		tvsend = (struct timeval *) icmp->icmp_data;
		tv_sub(tvrecv, tvsend);
		rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec / 1000.0;

		printf("%d bytes from %s: seq=%u, ttl=%d, rtt=%.3f ms\n",
				icmplen, host,
				icmp->icmp_seq, ip->ip_ttl, rtt);

	} else if (verbose) {
		printf("  %d bytes from %s: type = %d, code = %d\n",
				icmplen, host,
				icmp->icmp_type, icmp->icmp_code);
	}
}

//send_v4.c
void
send_v4(void)
{
	int			len;
	struct icmp	*icmp;

	icmp = (struct icmp *) sendbuf;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_id = pid;
	icmp->icmp_seq = nsent++;
	memset(icmp->icmp_data, 0xa5, datalen);	/* fill with pattern */
	gettimeofday((struct timeval *) icmp->icmp_data, NULL);

	len = 8 + datalen;		/* checksum ICMP header and data */
	icmp->icmp_cksum = 0;
	icmp->icmp_cksum = in_chksum((u_short *) icmp, len);

	sendto(sockfd, sendbuf, len, 0, (struct sockaddr *)pr->sasend, pr->salen);
	
}

//threadSend
void* threadSend()
{
	while(1)
	{
		send_v4();
		sleep(1);
	}
}

//threadReceive
void* threadReceive()
{
	proc_v4(recvbuf, n, &msg, &tval);
	sleep(1);
	
}

/*void sig_alrm(int signo)
{
	(*pr->fsend)();

	alarm(1);
	return;
}*/

void tv_sub(struct timeval *out, struct timeval *in)
{
	if ( (out->tv_usec -= in->tv_usec) < 0) {	/* out -= in */
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}

uint16_t in_chksum(uint16_t *addr, int len){
	int nleft=len;
	uint32_t sum=0;
	uint16_t *w=addr;
	uint16_t answer=0;

	while(nleft>1){
		sum+=*w++;
		nleft-=2;
	}

	if(nleft ==1){
		*(unsigned char *)(&answer) = *(unsigned char *)w;
		sum+=answer;
	}

	sum=(sum>>16) + (sum & 0xffff);
	sum+= (sum>>16);
	answer=~sum;
	return (answer);
}
