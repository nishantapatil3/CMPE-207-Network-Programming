/* TCPdaytimed.c - server

Name:Nishant Patil
SJSU ID:010720918

18 Oct 2015

*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int		errexit(const char *format, ...);
void		TCPdaytimed(int fd);
int		passiveTCP(const char *service, int qlen);

#define QLEN	32

/*------------------------------------------------------------------------
 * main - Iterative TCP server for DAYTIME service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	struct	sockaddr_in fsin;	/* the from address of a client	*/
	char	*service = "daytime";	/* service name or port number	*/
	int	msock, ssock;		/* master & slave sockets	*/
	unsigned int	alen;		/* from-address length		*/
	pid_t pid;
	int status;

	switch (argc) {
	case	1:
		break;
	case	2:
		service = argv[1];
		break;
	default:
		errexit("usage: TCPdaytimed [port]\n");
	}

	msock = passiveTCP(service, QLEN);

	while (1) {
		alen = sizeof(fsin);
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
		if (ssock < 0)
			errexit("accept failed: %s\n", strerror(errno));
		
		if((pid = fork()) == 0)		/*create child process and run if successful*/
		{
			close(msock);		/*In slave prcess close master socket*/ 
			printf("\nI am in child process, pid is:%d and parent id is:%d\n", getpid(), getppid());
			TCPdaytimed(ssock);	/*call tcpdaytimed function*/
			(void) close(ssock);	/*after function call close slave socket*/
			exit(0);
		}
	}
}

/*------------------------------------------------------------------------
 * TCPdaytimed - do TCP DAYTIME protocol
 *------------------------------------------------------------------------
 */
void
TCPdaytimed(int fd)
{
	char	*pts;			/* pointer to time string	*/
	time_t	now;			/* current time			*/
	char	*ctime();

	(void) time(&now);
	pts = ctime(&now);
	(void) write(fd, pts, strlen(pts));
}
