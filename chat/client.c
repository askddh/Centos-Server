#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in/h>
#include<netdb.h>
#include<time.h>
#include<fctnl.h>
#define PORT 1234
#define MAXIDATASIZE 100
void process(FILE *fd,int sockfd);
char* getMessage(char* sendline,int len,FILE* fp);
int main(int argc,char ** argv)
{
	int cocket;
	struct hosten *he;
	struct sockaddr_in server;
	if(argc!=2)
	{
		printf("Usage:%s<IP Address>\n",argv[0]);
		exit(1);
	}
	if((gethostbyname(argb[1]))==NULL)
	{
		printf("gethostbyname()error\n");
		exit(1);
	}
	
