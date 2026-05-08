
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdio.h>

#define PORT 1234
#define MAXDATASIZE 100

int main(int argc,char **argv)
{
	int fd,numbytes;
	char buf[MAXDATASIZE];
	char sendbuf[20]="hello";
	struct hostent *he;
	struct sockaddr_in serve,reply;
	int len;
	if(argc!=2)
	{
		printf("Usage:%s <IP Address> \n",argv[10]);
		exit(1);
	}	
	if((he=gethostbyname(argv[1]))==NULL)
	{
		perror("gethostbyname");
		exit(1);
	}

	if((fd=socket(AF_INET,SOCK_DGRAM,0))==-1)
	{
		perror("socket");
		exit(1);
	}
	
	bzero(&serve,sizeof(serve));
	serve.sin_family=AF_INET;
	serve.sin_port=htons(PORT);
	serve.sin_addr=*((struct in_addr *)he->h_addr);
	sendto(fd,sendbuf,strlen(sendbuf),0,(struct sockaddr *)&serve,sizeof(struct sockaddr));
	while(1)
	{
		len=sizeof(struct sockaddr_in);
		if((numbytes=recvfrom(fd,buf,MAXDATASIZE,0,(struct sockaddr *)&reply,&len))==-1)
		{
			perror("recvfrom");	
			exit(1);
		}
		if(len!=sizeof(struct sockaddr)||memcmp((const void *)&serve,(const void *)&reply,len)!=0)
		{
			printf("Receive message from other server.\n");
			continue;
		}
		buf[numbytes]='\0';
		printf("Serve Message:%s\n",buf);
		printf("input:");
		fgets(sendbuf,sizeof(sendbuf),stdin);
		sendbuf[strlen(sendbuf)-1]='\0';
		sendto(fd,sendbuf,strlen(sendbuf),0,(struct sockaddr *)&serve,sizeof(struct sockaddr));
		if(strcmp(sendbuf,"quit")==0)
			break;
	}
	close(fd);
	return 0;
}
