#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 1234
#define MAXDATASIZE 100

int main()
{
	int sockfd;
	struct sockaddr_in serve;
	struct sockaddr_in client;
	int sin_size;
	int num;
	char msg[MAXDATASIZE];
	char sbuf[100]=" ";
	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))==-1)
	{
		perror("socket");
		exit(1);
	}
	
	bzero(&serve,sizeof(struct sockaddr_in));
	serve.sin_family=AF_INET;
	serve.sin_port=htons(PORT);
	serve.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(sockfd,(struct sockaddr *)&serve,sizeof(struct sockaddr))==-1)
	{
		perror("bind");
		exit(1);
	}
	
	sin_size=sizeof(struct sockaddr_in);
	while(1)
	{
		num=recvfrom(sockfd,msg,MAXDATASIZE,0,(struct sockaddr *)&client,&sin_size);
		if(num<0)
		{
			perror("recvfrom");
			exit(1);
		}
		msg[num]='\0';
		printf("You got a message (%s) from %s\n",msg,inet_ntoa(client.sin_addr));
		strcpy(sbuf,msg);
		sendto(sockfd,sbuf,strlen(sbuf),0,(struct sockaddr *)&client,sin_size);
		if(!strcpy(msg,"quit"))
			break;
	}
	close(sockfd);
	return 0;
}
