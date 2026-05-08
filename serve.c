#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<strings.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define SERVPORT 3333
#define BACKLOG 10

int main()
{
	int sockfd,connfd;
	struct sockaddr_in my_addr;
	struct sockaddr_in remote_addr;
	int val=1;
	int sin_size;
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket");
		exit(1);
	}
	
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char *)&val,sizeof(val));
	
	//bzero(&my_addr,sizeof(my_addr));	
	my_addr.sin_family=AF_INET;
	my_addr.sin_port=htons(SERVPORT);
	my_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	bzero(&(my_addr.sin_zero),8);
	
	if(bind(sockfd,(struct sockaddr * )&my_addr,sizeof(struct sockaddr_in))==-1)
	{
		perror("bind");
		exit(1);
	}
	
	if(listen(sockfd,BACKLOG)==-1)
	{
		perror("listen");
		exit(1);
	}
	
	while(1)
	{
	if((connfd=accept(sockfd,(struct sockaddr *)&remote_addr,&sin_size))==-1)
	{
		perror("accept");
		exit(1);
	}
	
	printf("received a connection from %s\n",inet_ntoa(remote_addr.sin_addr));

	if(send(connfd,"Hello,you are connected!\n",26,0)==-1)
	{
		perror("send");
		close(connfd);
		exit(2);
	}
	
	printf("Simulation processing start(5s)......\n");
	sleep(5);
	printf("Simulation processing stop......\n");
	close(connfd);
	}
	close(sockfd);
	return 0;
}	
	
