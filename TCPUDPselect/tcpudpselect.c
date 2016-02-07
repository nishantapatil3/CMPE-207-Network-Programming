/* TCP/UDP select dvdstore server
	Nishant Patil
	SJSU ID: 010720918
	18 Oct 2015

	./tcpudpselect <ip_address> <port>

*/

#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/errno.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <pthread.h>

/******************** #define **************************/
#define ERROR -1
#define MAX_CONNECT 5

#define	QLEN		  32	/* maximum connection queue length	*/
#define	BUFSIZE		4096

#define	INTERVAL	1	/* secs */

/******************** Defining structure for mutex and count variables *******************************/
struct {
	pthread_mutex_t	st_mutex;
	unsigned int	st_concount;
	unsigned int	st_contotal;
	unsigned int	st_concount1;
	unsigned int	st_contotal1;
} stats;

/******************** Function declaration *******************************/
void *TCPdvd (int s1);
void *UDPdvd (int s2);
void prstats(void);

int argc;
char **argv;

/******************** Start of main *******************************/
main(int argc,char **argv)
{
	pthread_t thread1, thread2;
	char *msg1 = argv[2];
	char *msg2 = "second thread";
	int ret1, ret2;

	pthread_t self_id;
	self_id = pthread_self();
	printf("%u",self_id);

	int s1, s2, n1, rv;
	fd_set readfds;
	struct timeval tv;
	char buf1[256], buf2[256];

	int     listenfd, connfd, udpfd, nready, maxfdp1;
	char    mesg[1024];
	pid_t   childpid;
	ssize_t n;
	socklen_t len;
	const int on = 1;
	struct sockaddr_in cliaddr, servaddr;
	void    sig_chld(int);

	pthread_t	th;
	pthread_attr_t	ta;


/******************** create listening TCP socket ********************************/
	if((s1 = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
	{
		perror("socket tcp: ");
		exit(ERROR);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	servaddr.sin_addr.s_addr = INADDR_ANY;

	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(bind(s1, (struct sockaddr *)&servaddr, sizeof(servaddr)) == ERROR)
	{
		perror("bind:");
		exit(ERROR);
	}

	if(listen(s1, MAX_CONNECT) == ERROR)
	{
		perror("listen:");
		exit(ERROR);
	}

/******************** create listening UDP socket ********************************/
	if((s2 = socket(AF_INET, SOCK_DGRAM, 0)) == ERROR)
	{
		perror("socket: ");
		exit(ERROR);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	servaddr.sin_addr.s_addr = INADDR_ANY;

	if(bind(s2, (struct sockaddr *)&servaddr, sizeof(servaddr)) == ERROR)
	{
		perror("bind:");
		exit(ERROR);
	}

/******************** defining thread attributes ********************************/
	(void) pthread_attr_init(&ta);
	(void) pthread_attr_setdetachstate(&ta, PTHREAD_CREATE_DETACHED);
	(void) pthread_mutex_init(&stats.st_mutex, 0);

	if (pthread_create(&th, &ta, (void * (*)(void *))prstats, 0) < 0)
		printf("pthread_create(prstats): %s\n", strerror(errno));
	
/******************** create listening TCP socket ********************************/
	while(1)
	{	
	// clear the set ahead of time
	FD_ZERO(&readfds);

	// add our descriptors to the set
	FD_SET(s1, &readfds);
	FD_SET(s2, &readfds);


	// since we got s2 second, it's the "greater", so we use that for
	// the n param in select()
	n1 = s2 + 1;
	
	// wait until either socket has data ready to be recv()d (timeout 5.5 secs)
	tv.tv_sec = 10;
	tv.tv_usec = 500000;
	
	//select listen on s1 and s2
	rv = select(n1, &readfds, NULL, NULL, &tv);

	if (rv == -1) 
	{
	    perror("select"); // error occurred in select()
	} 
	else if (rv == 0) 
	{
	    //printf("Timeout occurred!  No data after 5.5 seconds.\n");
	} 

	else 
	{
	    // one or both of the descriptors have data
	    if (FD_ISSET(s1, &readfds)) //call tcpdvd thread
	    {		
		time_t	start;
		char	buf[BUFSIZ];
		int	cc;

		start = time(0);
		(void) pthread_mutex_lock(&stats.st_mutex);
		stats.st_concount++;
		(void) pthread_mutex_unlock(&stats.st_mutex);

		//printf("inside tcp select\n\n\n");
		sleep(2);
		if (pthread_create(&thread1, &ta, (void * (*)(void *))TCPdvd, (void *)(long)s1) < 0);

		pthread_join(thread1, NULL);

		(void) pthread_mutex_lock(&stats.st_mutex);
		stats.st_concount--;
		stats.st_contotal++;
		(void) pthread_mutex_unlock(&stats.st_mutex);	

	    }


	    if (FD_ISSET(s2, &readfds)) //call udpdvd thread
	    {
		time_t	start;
		char	buf[BUFSIZ];
		int	cc;

		start = time(0);
		(void) pthread_mutex_lock(&stats.st_mutex);
		stats.st_concount1++;
		(void) pthread_mutex_unlock(&stats.st_mutex);

		//printf("inside udp select\n\n\n");
		sleep(2);
		if (pthread_create(&thread2, &ta, (void * (*)(void *))UDPdvd, (void *)(long)s2) < 0);

		pthread_join(thread2, NULL);

		(void) pthread_mutex_lock(&stats.st_mutex);
		stats.st_concount1--;
		stats.st_contotal1++;
		(void) pthread_mutex_unlock(&stats.st_mutex);	

	    }
        }

	void prstats(void);
	}
}

/*------------------------------------------------------------------------
 * prstats - print server statistical data
 *------------------------------------------------------------------------
 */
void
prstats(void)
{
	time_t	now;

	while (1)
	{
		(void) sleep(INTERVAL);

		(void) pthread_mutex_lock(&stats.st_mutex);
		now = time(0);
		(void) printf("\n--- %s", ctime(&now));
		(void) printf("%-32s: %u\n", "Current TCP connections", stats.st_concount);
		(void) printf("%-32s: %u\n", "Current UDP connections", stats.st_concount1);
		(void) printf("%-32s: %u\n", "Completed TCP connections", stats.st_contotal);
		(void) printf("%-32s: %u\n", "Completed UDP connections", stats.st_contotal1);
		
		(void) pthread_mutex_unlock(&stats.st_mutex);

	}
}
