/* UDP dvdstore server
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
void UDPdvd(int sockfd)
{
	pthread_t self_id;
	self_id = pthread_self();
	printf("%u",self_id);

	FILE *fp, *fp1;
	int sent, i=0, itemnumber, found, bytes_recieved1, bytes_recieved2, bytes_recieved3, n;
	char mesg[1000];
	char ch;
	char recv_data1[1024], recv_data2[1024], recv_data3[1];
	char filename[] = "temp.txt";
   	//int sockfd;
   	struct sockaddr_in servaddr,cliaddr;
   	socklen_t len;



	//printf("\n---------------Start of DVD store UDP Server program------------------\n");


/**************************** logic to send list to client **************************************/
	len = sizeof(cliaddr);
	bytes_recieved1 = recvfrom(sockfd,recv_data1,1000,0,(struct sockaddr *)&cliaddr,&len);	//recieve data from client for list or order
	//strcpy(mesg,"");
	i=0;	
	if(strcmp(recv_data1,"list") == 0)	//compare recvdata with list, if true.. send dvd list to client
	{	

		fp=fopen("dvd_database.txt","r");
		while (!feof(fp))
		{
			ch = fgetc(fp);
			mesg[i]=ch;
			i++;
		}mesg[i] = '\0';
		fclose(fp);	
		puts("\ndvdstore list sent to client\n");
		sent = sendto(sockfd,mesg,sizeof(mesg),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
		memset(recv_data1,0,strlen(recv_data1));
	}
/**************************** End of list sending to client **************************************/

/**************************** logic to order dvd(s) **********************************************/	
	if(strcmp(recv_data1,"order") == 0)
	{
		memset(recv_data1,0,strlen(recv_data1));
		sendto(sockfd,"ordered",sizeof("ordered"),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
		bytes_recieved2 = recvfrom(sockfd,recv_data2,1000,0,(struct sockaddr *)&cliaddr,&len);	//recieve from client, item to order
		
		sendto(sockfd,"\t",sizeof("\t"),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

		itemnumber = atoi(recv_data2);
		
        	found=0;	//flag to be raised when item being searched is found
        	if((fp=fopen("dvd_database.txt","r+"))==NULL)
        	{
			sendto(sockfd,"\n ! The File is Empty...\n\n",sizeof("\n ! The File is Empty...\n\n"),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
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
					sendto(sockfd,"ordernos",sizeof("ordernos"),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					bytes_recieved3 = recvfrom(sockfd,recv_data3,1024,0,(struct sockaddr *)&cliaddr,&len); //recieve no. of items to be ordered
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
			sendto(sockfd,"\nThe dvd is found.\n",sizeof("\nThe dvd is found.\n"),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
				 
			while(!feof(fp))
			{
				fscanf(fp,"\n%d\t%s\t%d\t",&dvdstore.item,dvdstore.title,&dvdstore.quantity);
				fprintf(fp1,"\n%d\t%s\t%d\t",dvdstore.item,dvdstore.title,dvdstore.quantity);
			}
		sendto(sockfd,"\nDVD record has been updated successfully on server...\n",sizeof("\nDVD record has been updated successfully on server...\n"),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
		printf("\nDVD record has been updated successfully on server...\n\n");
       		}
/********************************* End of send to client order ****************************/

/********************************* Send to client if dvd is not found **********************/
		if(found==0)
		{
			printf("\nDVD not found please try again\n");
			sendto(sockfd,"DVD not found please try again\n",sizeof("DVD not found please try again\n"),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
		}
	
       		fclose(fp);
       		fclose(fp1);
/********************************* End of send to client if dvd is not found ***************/
       
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
/************************************* End of Update database *****************************************/
	}
/************************************* End of logic to order DVD(s) ***********************************/

/************************************* If invalid input ***********************************************/

	memset(recv_data1,0,strlen(recv_data1));
	memset(recv_data2,0,strlen(recv_data2));
	memset(recv_data3,0,strlen(recv_data3));

/************************************* End of invalid input *******************************************/

/************************************* clear any temporary files **************************************/
	fflush(fp);
	fflush(fp1);	
	remove("temp.txt");	//remove temp file

/************************************* End of while loop **********************************************/
}
/************************************* End of main logic **********************************************/
