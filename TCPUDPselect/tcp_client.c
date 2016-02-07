/* TCP dvdstore client 
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
#define BUFFER 1024
/******************** End of Define *******************************/

/******************** Start of main logic *************************/
main(int argc, char **argv)
{
	printf("\n---------------Start of DVD store TCP Client program------------------\n");
	int sock;
	int len, flag=0, i=0;
	struct sockaddr_in remote_server;
	char input[BUFFER], output[BUFFER];
	char cpystrt[20], cpystrt1[20], cpystrt2[20], cpystrt3[20];
	
/**************************** Creating a new TCP CLIENT socket **********************************/
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
	{
		perror("socket: ");
		exit(ERROR);
	}

	remote_server.sin_family = AF_INET;
	remote_server.sin_port = htons(atoi(argv[2]));
	remote_server.sin_addr.s_addr = inet_addr(argv[1]);
	bzero(&remote_server.sin_zero, 8);


//connect----------------------------------------------------------------
	if((connect(sock, (struct sockaddr *)&remote_server, sizeof(struct sockaddr_in))) == ERROR)
	{
		perror("connect: ");
		exit(ERROR);
	}	
	
	sleep(3);

/**************************** logic to request server list of dvds ******************************/
	strcpy(cpystrt1,argv[3]);//order or list	

		if(strcmp(argv[3],"list") == 0)
		{
			for(i=0;i<1;i++)
			printf("Itemno\tDVDTitle\tQuantity");
			send(sock, cpystrt1, strlen(cpystrt1), 0);
			len = recv(sock,input,1024,0);
			input[len] = '\0';
			puts(input);
		}	
	
/**************************** logic to order dvd(s) from server  ******************************/
		if(strcmp(argv[3],"order") == 0)
		{
			send(sock, cpystrt1, strlen(cpystrt1), 0);
			
			len = recv(sock,input,1024,0);
			input[len] = '\0';
			if(strcmp(input,"ordered") == 0)
			{	
				flag = 1;	
				strcpy(cpystrt2,argv[4]);	//itemnumber
				send(sock, cpystrt2, strlen(cpystrt2), 0);
				len = recv(sock,input,1024,0);
				input[len] = '\0';
				puts(input);
			}

			len = recv(sock,input,1024,0);
			input[len] = '\0';
			if(strcmp(input,"ordernos") == 0)
			{		
				strcpy(cpystrt3,argv[5]);	//no of items to order
				send(sock, cpystrt3, strlen(cpystrt3), 0);
				len = recv(sock,input,1024,0);
				input[len] = '\0';
				puts(input);
				printf("\nDVD(s) has been ordered successfully...\n");
			}
			if(!flag)
			{
				printf("\nDVD not found please try again\n");
				exit(1);
			}
	
		/*len = recv(sock,input,1024,0);
		input[len] = '\0';
		puts(input);
		len = recv(sock,input,1024,0);
		input[len] = '\0';
		puts(input);*/
	
	len = recv(sock,input,1024,0);
	puts(input);	
			
		}

/************************** Close connection **************************************/
	close(sock);
		
}
/**************************** End of main logic ****************************************************/
