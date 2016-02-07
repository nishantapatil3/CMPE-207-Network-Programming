/* TCP dvdstore server
	Nishant Patil
	SJSU ID: 010720918
	18 Oct 2015

	./ex2_server <ip_address> <port>

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
#define MAX_CONNECT 5
/******************** End of Define *******************************/

/******************** Defining a structure for dvdstore data *****************/
struct
{
    int item;
    char title[20];
    int quantity;
}dvdstore;
/******************** End of Defining a structure ***************************/

/******************** Start of main logic *************************/
void TCPdvd(int sock)
{
	pthread_t self_id;
	self_id = pthread_self();
	printf("%u",self_id);

	FILE *fp, *fp1;
	int cli;
	unsigned int len;
	int sent, i=0, itemnumber, found, bytes_recieved1, bytes_recieved2, bytes_recieved3, n=0;
	struct sockaddr_in server, client;
	char mesg[999];
	char ch;
	char recv_data1[1024], recv_data2[1024], recv_data3[1];

	//printf("\n---------------Start of DVD store TCP Server program------------------\n");
	


/************************** Accept if any request arrives *************************************/
	
	if((cli = accept(sock, (struct sockaddr*)&client, &len)) == ERROR)
	{
		perror("accept");
		exit(ERROR);
	}

/**************************** logic to send list to client **************************************/	

	sleep(1);
	//puts("start of while loop");
	bytes_recieved1 = recv(cli,recv_data1,1024,0);	//recieve data from client for list or order
	//recv_data1[bytes_recieved1] = '\0';

	i=0;
	if(strcmp(recv_data1,"list") == 0)	//compare recvdata with list, if true.. send dvd list to client
	{	
		fp=fopen("dvd_database.txt","r");
		while (!feof(fp))
		{
			ch = fgetc(fp);
			mesg[i]=ch;
			i++;
		}
		fclose(fp);	
		puts("\ndvdstore list sent to client\n");
		sent = send(cli, mesg, strlen(mesg), 0);
		memset(recv_data1,0,strlen(recv_data1));
	}

/**************************** logic to order dvd(s) **********************************************/	
	if(strcmp(recv_data1,"order") == 0)
	{
		send(cli,"ordered",sizeof("ordered"),0);
		bytes_recieved2 = recv(cli,recv_data2,1024,0);	//flag to be raised when item being searched is found
        	send(cli,"\t",sizeof("\t"),0);
		itemnumber = atoi(recv_data2);
		
        	found=0;
        	if((fp=fopen("dvd_database.txt","r+"))==NULL)
        	{
			send(cli,"\n ! The File is Empty...\n\n",sizeof("\n ! The File is Empty...\n\n"),0);
        		printf("\n ! The File is Empty...\n\n");
        	}
        	else
        	{
			fp1=fopen("temp.txt","w");	//using a temp file to temporarily store database
        	    	while(!feof(fp)&& found==0)
        	    	{
        	        	fscanf(fp,"\n%d\t%s\t%d\t",&dvdstore.item,dvdstore.title,&dvdstore.quantity);
        	        
        	        	if(dvdstore.item==itemnumber)
        	        	{ 
					found=1;	//raise found flag
					send(cli,"ordernos",sizeof("ordernos"),0);
					bytes_recieved3 = recv(cli,recv_data3,1,0);	//recieve no. of items to be ordered
					n = atoi(recv_data3);
					dvdstore.quantity = dvdstore.quantity - n;
					fprintf(fp1,"\n%d\t%s\t%d\t",dvdstore.item,dvdstore.title,dvdstore.quantity);
				}
				else
				{
					fprintf(fp1,"\n%d\t%s\t%d\t",dvdstore.item,dvdstore.title,dvdstore.quantity);
				}
        	         }
        	 }

/********************************* Send to client order successful ************************/
		if(found)
        	{
			send(cli,"\nDVD(s) has been ordered successfully...\n",sizeof("\nDVD(s) has been ordered successfully...\n"),0);
				 
			while(!feof(fp))
			{
				fscanf(fp,"\n%d\t%s\t%d\t",&dvdstore.item,dvdstore.title,&dvdstore.quantity);
				fprintf(fp1,"\n%d\t%s\t%d\t",dvdstore.item,dvdstore.title,dvdstore.quantity);
			}
		puts("DVD record has been updated successfully on server...\n");		 
		send(cli,"DVD record has been updated successfully on server...",sizeof("DVD record has been updated successfully on server..."),0);
       		}

/********************************* Send to client if dvd is not found**********************/
		if(found==0)
		{
			printf("\nDVD not found please try again\n");
			send(cli,"DVD not found please try again\n",sizeof("DVD not found please try again\n"),0);
		}
	
       		fclose(fp);
       		fclose(fp1);

/************************************* Update database *****************************************/
        	fp=fopen("dvd_database.txt","w");
        	fp1=fopen("temp.txt","r");
        	while(!feof(fp1))
		{
			fscanf(fp1,"\n%d\t%s\t%d\t",&dvdstore.item,dvdstore.title,&dvdstore.quantity);
			fprintf(fp,"\n%d\t%s\t%d\t",dvdstore.item,dvdstore.title,dvdstore.quantity);
		}
			
		fclose(fp);
        	fclose(fp1);
	 
	}
	

/************************************* clear any temporary files **************************************/
	
	memset(recv_data1,0,strlen(recv_data1));
	memset(recv_data2,0,strlen(recv_data2));
	memset(recv_data3,0,strlen(recv_data3));
	fflush(fp);
	fflush(fp1);



/************************** Close connection and clear any temporary files **************************************/
	close(cli);
	remove("temp.txt");

	pthread_exit();
}
/************************************* End of main logic **********************************************/
