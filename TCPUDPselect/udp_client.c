/* UDP dvdstore client 
	Nishant Patil
	SJSU ID: 010720918
	18 Oct 2015

	./ex2_client <ip_address> <port> <list>

	./ex2_client <ip_address> <port> <order> <itemnumber> <quantity>

*/

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

/******************** Defining the ERROR **************************/
#define ERROR -1
/******************** End of Define *******************************/

/******************** Start of main logic *************************/
int main(int argc, char**argv)
{
	printf("\n---------------Start of DVD store UDP Client program------------------\n");
	char cpystrt[20], cpystrt1[20], cpystrt2[20], cpystrt3[20];
   	int sockfd, len;
   	struct sockaddr_in servaddr,cliaddr;
   	char sendline[1000];
   	char recvline[1000];

/**************************** Creating a new UDP CLIENT socket **********************************/
	if ((sockfd=socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		perror("socket: ");
		exit(ERROR);
	}

   	bzero(&servaddr,sizeof(servaddr));
   	servaddr.sin_family = AF_INET;
   	servaddr.sin_addr.s_addr=inet_addr(argv[1]);
   	servaddr.sin_port=htons(atoi(argv[2]));


/**************************** logic to request server list of dvds ******************************/
	strcpy(cpystrt1,argv[3]);		//copy "order" or "list" to a string

	if(strcmp(argv[3],"list") == 0)		//compare string with "list", if true recieve list of dvds from server
	{
		printf("Itemno\tDVDTitle\tQuantity");
		printf("\n---------------------------------------------\n");
		sendto(sockfd, cpystrt1, strlen(cpystrt1), 0, (struct sockaddr *)&servaddr,sizeof(servaddr));
		len = recvfrom(sockfd,recvline,10000,0,NULL,NULL);
		puts(recvline);
		exit(1);
	}	
/**************************** End of recieve dvd list from server  ******************************/

/**************************** logic to order dvd(s) from server  ******************************/
	else if(strcmp(argv[3],"order") == 0)	//compare string with "list", if true order item from server
	{
		sendto(sockfd, cpystrt1, strlen(cpystrt1), 0, (struct sockaddr *)&servaddr,sizeof(servaddr));
		
		len = recvfrom(sockfd,recvline,10000,0,NULL,NULL); 
		if(strcmp(recvline,"ordered") == 0)
		{		
			strcpy(cpystrt2,argv[4]);	//itemnumber
			sendto(sockfd, cpystrt2, strlen(cpystrt2), 0, (struct sockaddr *)&servaddr,sizeof(servaddr));
			len = recvfrom(sockfd,recvline,10000,0,NULL,NULL);
			recvline[len] = '\0';
		}

		len = recvfrom(sockfd,recvline,10000,0,NULL,NULL);
		if(strcmp(recvline,"ordernos") == 0)
		{	
			strcpy(cpystrt3,argv[5]);	//no of items to order
			sendto(sockfd, cpystrt3, strlen(cpystrt3), 0, (struct sockaddr *)&servaddr,sizeof(servaddr));
			len = recvfrom(sockfd,recvline,10000,0,NULL,NULL);
			recvline[len] = '\0';
		}
		else
		{	
			printf("\nDVD not found...\n");
			exit(1);
		}
	
	printf("\nDVD(s) has been ordered successfully...\n");
	len = recvfrom(sockfd,recvline,10000,0,NULL,NULL);
	puts(recvline);		
	}
/**************************** End of logic to order dvd(s) to server  ******************************/

/**************************** For invalid input ****************************************************/
	else
	{
		puts("\nPlease Enter valid dvd information(list/order)\n");
	}
/**************************** End of invalid input *************************************************/

}
/**************************** End of main logic ****************************************************/
