#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<dirent.h>
#include<fcntl.h>

#define MAXDATASIZE 128
#define PORT 1235
#define BACKLOG 5

void process_cli(int connfd,struct sockaddr_in client);
void proc_ls(int);
void proc_get(int,char *);
void proc_put(int,char *);
int parse(char *,char **);

int main(int argc,int **argv)
{
	int listenfd,connfd;
	struct sockaddr_in ser_addr,cli_addr;
	int len;
	pid_t pid;
	
	if((listenfd==socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Socket Error!\n");
		return -1;	
	}
	
	bzero(&ser_addr,sizeof(ser_addr));
	ser_addr.sin_family=AF_INET;
	ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	ser_addr.sin_port=htons(PORT);
	


	if(bind(listenfd,(struct sockaddr *)&ser_addr,sizeof(struct sockaddr_in))<0)
	{
		printf("Bind Error!\n");
		return -1;
	}
	
	if(listen(listenfd,BACKLOG)<0)
	{
		printf("Listen Error!\n");
		return -1;
	}

	bzero(&cli_addr,sizeof(cli_addr));
	
	len=sizeof(struct sockaddr_in);
	while(1)
	{
		printf("--------------\n");
		
		if((connfd=accept(listenfd,(struct sockaddr *)&cli_addr,&len))<0)
		{
			perror("accepr");
			exit(1);	
		}
		
		if((pid=fork())>0)
		{
			close(connfd);
			continue;
		}
		else if(pid==0)
		{
			close(listenfd);
			process_cli(connfd,cli_addr);
			exit(0);
		}
		else
		{
			perror("fork");
			exit(0);
		}
	}
	close(listenfd);
	return 0;
}


void process_cli(int connfd,struct sockaddr_in client)
{
	char cmd[MAXDATASIZE];
	char *cmds[64];
	int cmdnum,num;

	bzero(cmd,MAXDATASIZE);
	
	num=recv(connfd,cmd,MAXDATASIZE,0);
	if(num==-1)
	{
		close(connfd);
		perror("recv");
		exit(1);
	}
	cmd[num-1]='\0';
	printf("%s command is %s.\n",inet_ntoa(client.sin_addr),cmd);
	cmdnum=parse(cmd,cmds);
	if(strcmp(cmds[0],"exit")==0)
	{
		close(connfd);
		exit(0);
	}
	else if(strcmp(cmds[0],"ls")==0)
	{
		proc_ls(connfd);
	}
	else if(strcmp(cmds[0],"get")==0)
	{
		if(cmd[1]!=0)
			proc_get(connfd,cmds[1]);
		else
		{
			printf("error:get command missing filename\n");
			close(connfd);
		}
	}
	else if(strcmp(cmds[0],"put")==0)
	{
		if(cmds[1]!=0)
			proc_put(connfd,cmds[1]);
		else
		{
			printf("error:put command missing filename\n");
			close(connfd);
		}
	}
}
int parse(char *buf,char **args)
{
	int num=0;
	while(*buf!='\0')
	{
		while((*buf==' ')||(*buf=='\t'||(*buf=='\n')))
			*buf++='\0';
		*args++=buf;
		++num;
		while((*buf!='\0')&&(*buf!=' ')&&(*buf!='\t')&&(*buf!='\n'))
			buf++;
	}
	*args='\0';
	return num;
}

void proc_ls(int sockfd)
{
	DIR * mydir=NULL;
	struct dirent *myitem=NULL;
	char cmd[MAXDATASIZE];
	bzero(cmd,MAXDATASIZE);
	
	if((mydir=opendir("."))==NULL)
	{
		perror("opendir");
		exit(1);
	}
	
	while((myitem=readdir(mydir))!=NULL)
	{
		if(sprintf(cmd,myitem->d_name,MAXDATASIZE)<0)
		{
			printf("Sprintf Error!\n");
			exit(1);
		}
		
		if(write(sockfd,cmd,MAXDATASIZE)<0)
		{
			perror("write");
			exit(1);
		}
	}
	closedir(mydir);
	close(sockfd);
	return;
}
void proc_get(int sockfd,char *filename)
{
	int fd,nbytes;
	char buffer[MAXDATASIZE];
	bzero(buffer,MAXDATASIZE);
	
	printf("get filename:[%s]\n",filename);
	if((fd=open(filename,O_RDONLY))<0)
	{
		perror("open");
		buffer[0]='N';
		if(write(sockfd,buffer,MAXDATASIZE)<0)
		{
			perror("proc_get write1");
			exit(1);
		}
		return;
	}
	
	buffer[0]='Y';
	if(write(sockfd,buffer,MAXDATASIZE)<0)	
	{
		perror("proc_get write2");
		close(fd);
		exit(1);
	}
	
	while((nbytes=read(fd,buffer,MAXDATASIZE))>0)
	{
		if(write(sockfd,buffer,nbytes)<0)
		{
			perror("proc_get write3");
			close(fd);
			exit(1);
		}
	}
	close(fd);
	close(sockfd);
	
	return;
}
void proc_put(int sockfd,char *filename)
{
	int fd,nbytes;
	char buffer[MAXDATASIZE];
	bzero(buffer,MAXDATASIZE);
	
	printf("get filename:[%s]\n",filename);
	if((fd=open(filename,O_WRONLY|O_CREAT|O_TRUNC,0644))>0)	
	{
		perror("open");
		return;
	}
	
	while((nbytes=read(sockfd,buffer,MAXDATASIZE))>0)
	{
		if(write(fd,buffer,nbytes)<0)
		{
			perror("proc_put write");
			close(fd);
			exit(1);
		}
	}
	close(fd);
	close(sockfd);
	
	return;
}
