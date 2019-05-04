#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

#define SIZE 10

struct ReadyQueue
{
	int clientID;
	char fileName[256];
	int sockfd;
	int port;
};
void enQueue(int, char[256], int, int);
void deQueue();

struct ReadyQueue readyQueue[SIZE];
int front = -1, rear = -1;
FILE *fp;
int compR, compW, adminR, adminW;
int timeout;

void error(char *msg)
{
    perror(msg);
    exit(-1);
}

void dumpQueue(int sig_num){
	int size, var1, sum;
	char msg[50];
	if(front==-1 && rear==-1){
		sprintf (msg, "Empty Ready Queue!");
    	write(compW, msg, sizeof(msg));	
    	sprintf (msg, "QueueEnd");
    	write(compW, msg, sizeof(msg));	
		return;
	}	
	while(1){
		sleep(timeout/1000);
		size=0;
		sum=0;
		bzero(msg,50);
		fp = fopen(readyQueue[front].fileName,"r");
		if (fp == NULL)
			error ("ERROR opening the file");
		fscanf(fp,"%d\n",&size);
		while(size>0){
			size--;
			fscanf(fp,"%d\n",&var1);
			sum+=var1;
		}
		fclose(fp);
		sprintf (msg, "%d, %s, %d, %d, %d", readyQueue[front].clientID, readyQueue[front].fileName, sum, readyQueue[front].sockfd, readyQueue[front].port);
		write(compW, msg, sizeof(msg));		
        front++;
        if(front > rear){
            front = rear = -1;
			break;
		}
    }
    sprintf (msg, "QueueEnd");
    write(compW, msg, sizeof(msg));	
}

int main (int argc, char *argv[])
{
	int fds1[2], fds2[2];
	char cmd[1];
	pipe(fds1);
	pipe(fds2);
	compR = fds1[0];
	compW = fds2[1];
	adminR = fds2[0];
	adminW = fds1[1];
	timeout = atoi(argv[2]);
	bool action1 = true;
	bool action2 = true;
	bool action3 = true;
	char fileName[256];
	
	pid_t pid = fork();
	if(pid>0){
		close(compR);
		close(compW);
		printf("Admin\t %d %d\n", pid, getpid());
		sleep(1);
		char dumpMsg[50];
		while(1){
			bzero(dumpMsg,50);
			bzero (cmd, 1);
			printf("Admin command: ");
			scanf("%s", cmd);
			write(adminW, cmd, sizeof(cmd));
			if(cmd[0]=='T')
				break;
			if(cmd[0]=='Q'){
				kill(pid, SIGRTMIN);
				while(strcmp(dumpMsg,"QueueEnd")!=0){
					read (adminR, dumpMsg, 50);
					if(strcmp(dumpMsg,"QueueEnd")!=0)
						printf("%s\n",dumpMsg);
				}
			}
		}
    }
    else{
	 close(adminR);
	 close(adminW);
		
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
     signal(SIGRTMIN, dumpQueue);
	 
	 printf ("Computer process server socket ready.\n");
	  
	 if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
       error("ERROR binding");

	 if(listen(sockfd,5)== -1)
		error("ERROR binding");
	 
	 clilen = sizeof(cli_addr);
     newsockfd1 = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	 int port1 = ntohs(cli_addr.sin_port);
     
	 if (newsockfd1 == -1) 
       error ("ERROR accepting Client 1");
   
     newsockfd2 = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	 int port2 = ntohs(cli_addr.sin_port);
     
	 if (newsockfd2 == -1) 
       error ("ERROR accepting Client 2");
   
     newsockfd3 = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	 int port3 = ntohs(cli_addr.sin_port);
     
	 if (newsockfd3 == -1) 
       error ("ERROR accepting Client 3");
   
	 char msg[50];
	 char *token;
	 int clientID;
	 while(action1 || action2 || action3){	
	 
		if(action1){
			bzero(msg,50);
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
				enQueue(clientID, fileName, newsockfd1, port1);
			}
		}
		
		if(action2){
			bzero(msg,50);
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
				enQueue(clientID, fileName, newsockfd2, port2);				
			}
		}
		
		if(action3){
			bzero(msg,50);
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
				enQueue(clientID, fileName, newsockfd3, port3);
			}
		}
		read(compR, cmd, sizeof (cmd));
		if(cmd[0]=='T'){
			close(newsockfd1);
			close(newsockfd2);
			close(newsockfd3);
			break;
		}
		else if(cmd[0]=='x'){
			while(front!=-1 || rear!=-1){
				deQueue();
			}
		}
		else if(cmd[0]=='Q'){
		}
	 }
	}
	return 0;
}

void enQueue(int clientID, char fileName[256], int sockfd, int port){
	if(rear == SIZE-1)
        printf("\nQueue is Full!!");
    else {
        if(front == -1)
            front = 0;
        rear++;
        readyQueue[rear].clientID = clientID;
		readyQueue[rear].sockfd = sockfd;
		readyQueue[rear].port = port;
		bzero(readyQueue[rear].fileName,256);
		strncpy(readyQueue[rear].fileName, fileName,256);
    }
}

void deQueue(){
	int size=0, var1, sum=0;
	char msg[50];
	bzero(msg,50);
	if(front == -1)
        printf("\nQueue is Empty!!");
    else{
		fp = fopen(readyQueue[front].fileName,"r");
		if (fp == NULL)
			error ("ERROR opening the file");
		fscanf(fp,"%d\n",&size);
		while(size>0){
			size--;
			fscanf(fp,"%d\n",&var1);
			sum+=var1;
		}
		fclose(fp);
		sprintf (msg, "%d %d", readyQueue[front].clientID, sum);
		int ret = write (readyQueue[front].sockfd, msg, strlen(msg));
		if (ret < 0) 
			error ("ERROR writing to socket");
        front++;
        if(front > rear)
            front = rear = -1;
    }
}
