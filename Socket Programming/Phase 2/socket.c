/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

void error(char *msg)
{
    perror(msg);
    exit(-1);
}

int main (int argc, char *argv[])
{
	 FILE *fp;
	 bool action1 = true;
	 bool action2 = true;
	 bool action3 = true;
	 char fileName[256];
	 int c, size, var1, sum;
	 pid_t pid = fork();
	 if(pid>0){
        printf("Admin\t %d %d\n", pid, getpid());
		wait(pid);
    }
    else{
	 printf("Computer %d %d\n", pid, getpid());
	 
	 if (argc < 2)
     { fprintf (stderr, "ERROR, no port provided\n");
       exit(1);
     }
     int sockfd, newsockfd1, newsockfd2, newsockfd3, portno, clilen;
     char filename[256];
     struct sockaddr_in serv_addr, cli_addr;
     int ret;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
	 
     if (sockfd == -1) 
       error("ERROR opening socket");
	 
     bzero ((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     
	 printf ("Computer process server socket ready.\n");
	  
	 if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
       error("ERROR binding");

	 if(listen(sockfd,5)== -1)
		error("ERROR binding");
	 
	 clilen = sizeof(cli_addr);
     newsockfd1 = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     
	 if (newsockfd1 == -1) 
       error ("ERROR accepting Client 1");
   
     newsockfd2 = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     
	 if (newsockfd2 == -1) 
       error ("ERROR accepting Client 2");
   
     newsockfd3 = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     
	 if (newsockfd3 == -1) 
       error ("ERROR accepting Client 3");
   
	 char msg[50];
	 char *token;
	 int clientID;
	 
	 while(action1 || action2 || action3){	
	 
		if(action1){
			bzero(msg,50);
			sum=0;
			size=0;
			c=0;
			bzero(fileName,256);
			ret = read (newsockfd1, msg, 255);
			if (ret < 0) 
				error ("ERROR reading from socket");
			token = strtok(msg, " ");
			clientID = atoi(token);
			token = strtok(NULL," ");
			sprintf(fileName, "%s", token);
			if(strcmp(fileName,"nullfile")==0){
				action1=false;
				close(newsockfd1);
			}
			else{
				strcat(fileName,".txt");
				fp = fopen(fileName,"r");
				if (fp == NULL)
					error ("ERROR opening the file");
				fscanf(fp,"%d\n",&size);
				while(size>0){
					size--;
					fscanf(fp,"%d\n",&var1);
					sum+=var1;
				}
				fclose(fp);
				sprintf (msg, "%d %d", clientID, sum);
				ret = write (newsockfd1, msg, strlen(msg));
			
				if (ret < 0) 
					error ("ERROR writing to socket");
			}
		}
		
		if(action2){
			bzero(msg,50);
			sum=0;
			size=0;
			c=0;
			bzero(fileName,256);
			ret = read (newsockfd2, msg, 255);
			if (ret < 0) 
				error ("ERROR reading from socket");
			token = strtok(msg, " ");
			clientID = atoi(token);
			token = strtok(NULL," ");
			sprintf(fileName, "%s", token);
			if(strcmp(fileName,"nullfile")==0){
				action2=false;
				close(newsockfd2);
			}
			else{
				strcat(fileName,".txt");
					fp = fopen(fileName,"r");
				if (fp == NULL)
					error ("ERROR opening the file");
				fscanf(fp,"%d\n",&size);
				while(size>0){
					size--;
					fscanf(fp,"%d\n",&var1);
					sum+=var1;
				}
				fclose(fp);
				sprintf (msg, "%d %d", clientID, sum);
				ret = write (newsockfd2, msg, strlen(msg));
			
				if (ret < 0) 
					error ("ERROR writing to socket");
			}
		}
		
		if(action3){
			bzero(msg,50);
			sum=0;
			size=0;
			c=0;
			bzero(fileName,256);
			ret = read (newsockfd3, msg, 255);
			if (ret < 0) 
				error ("ERROR reading from socket");
			token = strtok(msg, " ");
			clientID = atoi(token);
			token = strtok(NULL," ");
			sprintf(fileName, "%s", token);
			if(strcmp(fileName,"nullfile")==0){
				action3=false;
				close(newsockfd3);
			}
			else{
				strcat(fileName,".txt");
				fp = fopen(fileName,"r");
				if (fp == NULL)
					error ("ERROR opening the file");
				fscanf(fp,"%d\n",&size);
				while(size>0){
					size--;
					fscanf(fp,"%d\n",&var1);
					sum+=var1;
				}
				fclose(fp);
				sprintf (msg, "%d %d", clientID, sum);
				ret = write (newsockfd3, msg, strlen(msg));
		
				if (ret < 0) 
					error ("ERROR writing to socket");
			}
		}
	 }
	}
	return 0;
}
