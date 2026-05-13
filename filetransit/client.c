#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<fcntl.h>

#define N 256
#define PORT 1235

typedef struct sockaddr SA;

void proc_menu();
void proc_exit();
void proc_ls(struct sockaddr_in,char *);
void proc_get(struct sockaddr_in,char *);
void proc_put(struct sockaddr_in,char *);

int main(int argc,char ** argv)
{
	char cmd[N];
	struct sockaddr_in  addr;
	int len;
	
	proc_menu();
	
	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	addr.sin_port=htons(PORT);
	len=sizeof(addr);
	
	while(1)
	{
		printf(">");
		bzero(cmd,N);
		if(fgets(cmd,N,stdin)==NULL)
		{
			printf("Fgets Error!\n");
			return -1;
		}
		
		cmd[strlen(cmd)-1]='\0';
		

		//printf("Input Commend Is [%s]\n",cmd);
		

		if(strncmp(cmd,"help",4)==0)
		{
			proc_menu();
		}
		else if(strncmp(cmd,"exit",4)==0)
		{
			proc_exit(addr);
		}
		else if(strncmp(cmd,"ls",2)==0)
		{
			proc_ls(addr,cmd);
		}
		else if(strncmp(cmd,"get",3)==0)
		{
			proc_get(addr,cmd);
		}
		else if(strncmp(cmd,"put",3)==0)
		{
			proc_put(addr,cmd);
		}
		else
		{
			printf("Commend Is Error!Please Try Again\n");
		}
	}
	return 0;
}
int parse(char *buf,char ** args)
{
	int num;
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

void proc_menu()
{
	printf("\n--------------------------------\n");
	printf("|  help:show all commends         |\n");
	printf("|  exit:exit                      |\n");
	printf("|  ls  :show the file name list on server        |\n");
	printf("|  get filename:download file nnamed filename from server  |\n");
	printf("|  put filename:upload file named filename to server       |\n");
	printf("--------------------------------------------------------------");

	return;
}

void proc_exit(struct sockaddr_in addr)
{
	int sockfd;
	printf("Byte!\n");
	

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Socket Error!\n");
		exit(1);
	}

	if(connect(sockfd,(struct sockaddr *)&addr,sizeof(addr))<0)
	{
		printf("Connect Error!\n");
		exit(1);
	}
	
	if(write(sockfd,"exit",N)<0)
	{
		printf("Write Error!\n");
		exit(1);
	}
	

	close(sockfd);
	return;
}

void proc_ls(struct sockaddr_in addr,char *cmd)
{
	int sockfd;
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Socket Error!\n");
		exit(1);
	}
	
	if(connect(sockfd,(struct sockaddr *)&addr,sizeof(addr))<0)
	{
		printf("Connect Error!\n");
		exit(1);
	}
	
	if(write(sockfd,cmd,N)<0)
	{
		printf("Write Error!\n");
		exit(1);
	}
	
	while(read(sockfd,cmd,N)>0)
	{
		printf("%s",cmd);
	}
	printf("\n");
	
	close(sockfd);
	return;
}

void proc_get(struct sockaddr_in addr,char *cmd)
{
	int fd;
	int sockfd;
	char buffer[N];
	int nbytes;
	char * cmds[64];
	int cmdnum;
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Socket Error!\n");
		exit(1);
	}
	
	if(connect(sockfd,(struct sockaddr *)&addr,sizeof(addr))<0)
	{
		printf("Connect Error!\n");
		exit(1);
	}
	
	if(write(sockfd,cmd,N)<0)
	{
		printf("Write Error!A tproc_get 1\n");
		exit(1);
	}

	if(read(sockfd,buffer,N)<0)
	{
		printf("Read Error!A tproc_get 1\n");
		exit(1);
	}
	
	if(buffer[0]=='N')
	{
		close(sockfd);
		printf("Can't Open The File!\n");
		return;
	}
	cmdnum=parse(cmd,cmds);
	if((fd=open(cmds[1],O_WRONLY|O_CREAT|O_TRUNC,0644))<0)
	{
		printf("Open Error!\n");
		exit(1);
	}
	
	while((nbytes=read(sockfd,buffer,N))>0)
	{
		if(write(fd,buffer,nbytes)<0)
		{	
			printf("Write Error!A tproc_get 2");
		}
	}
	close(fd);	
	close(sockfd);
	return;
}

void proc_put(struct sockaddr_in addr,char *cmd)
{
	int fd;
	int sockfd;
	char buffer[N];
	int nbytes;
	char *cmds[64];
	int cmdnum;
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Socket Error!\n");
		exit(1);
	}
	
	if(connect(sockfd,(struct sockaddr *)&addr,sizeof(addr))<0)
	{
		printf("Connect Error!\n");
		exit(1);
	}
		
	if(write(sockfd,cmd,N)<0)
	{
		printf("Write Error!At proc_put 1\n");
		exit(1);
	}
	cmdnum=parse(cmd,cmds);
	
	if((fd=open(cmds[1],O_RDONLY))<0)
	{
		printf("Open Error!\n");
		exit(1);
	}
	while((nbytes=read(fd,buffer,N))>0)
	{
		if(write(sockfd,buffer,nbytes)<0)
		{
			printf("Write Error!At proc_put 2");
		}
	}
	close(fd);
	close(sockfd);
	return;
}

	
