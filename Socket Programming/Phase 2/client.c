#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	int sum;
	char fileName[8];
	char stringConst[8] = "nullfile";
    int sockfd, portno, ret;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	int clientID = atoi(argv[1]);
    if (argc < 3) {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
    }
    portno = atoi(argv[3]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    server = gethostbyname(argv[2]);
    if (server == NULL) error ("ERROR, no such host");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
          (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
      error("ERROR connecting");
	
	char msg[50];
	char *token;
	while(1){
		bzero (fileName, 8);
		printf ("Input file name: ");
		scanf("%s",fileName);
		sprintf (msg, "%d %s", clientID, fileName);
		ret = write (sockfd, msg, strlen(msg));
		if (ret < 0) error ("ERROR writing to socket");
		ret = read (sockfd, msg, strlen(msg));
		if (ret < 0) error ("ERROR reading from socket");
		if(strcmp(fileName,stringConst)==0)
			break;
		token = strtok(msg, " ");
		clientID = atoi(token);
		token = strtok(NULL," ");
		sum = atoi(token);
		printf("%d, %s, %d\n", clientID, fileName, sum);
	}
    return 0;
}