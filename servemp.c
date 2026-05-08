#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<strings.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>


int main()
{
	int sockfd,client_fd;
	struct sockaddr_in my_addr;
	struct sockaddr_in remote_addr;
	pid_t pid;
	int sin_size;
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket");
		exit(1);
	}
	
	int val=1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char *)&val,sizeof(val));
	
	//bzero(&my_addr,sizeof(my_addr));	
	my_addr.sin_family=AF_INET;
	my_addr.sin_port=htons(3333);
	my_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	bzero(&(my_addr.sin_zero),8);
	
	if(bind(sockfd,(struct sockaddr * )&my_addr,sizeof(struct sockaddr_in))==-1)
	{
		perror("bind");
		exit(1);
	}
	
	if(listen(sockfd,10)==-1)
	{
		perror("listen");
		exit(1);
	}
	
	sin_size=sizeof(struct sockaddr_in);

	while(1)
	{
	if((client_fd=accept(sockfd,(struct sockaddr *)&remote_addr,&sin_size))==-1)
	{
		perror("accept");
		exit(1);
	}
	
	if((pid=fork())>0)
	{
		close(client_fd);
		continue;
	}

	else if(pid==0)
	{
		close(sockfd);
		printf("%dreceived a connection from %s\n",getpid(),inet_ntoa(remote_addr.sin_addr));
		if(send(client_fd,"Hello,you are connected!\n",26,0)==-1)
		{
			perror("send");
			close(client_fd);
			exit(2);
		}
	
		printf("%dSimulation processing start(5s)......\n",getpid());
		sleep(5);
		printf("%dSimulation processing stop......\n",getpid());
		close(client_fd);
		exit(0);
	}
	else
	{
		perror("fork");
		exit(0);
	}
	}
	
	close(sockfd);
	return 0;
}
