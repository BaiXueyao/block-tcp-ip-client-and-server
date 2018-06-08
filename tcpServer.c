#include<stdio.h>
#include<sys/wait.h>
#include<netinet/in.h>
#include<errno.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<strings.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<signal.h>
#define SERV_PORT 9877
#define SA struct sockaddr
#define MAXLINE 4096
#define LISTENQ 1024

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	pid = wait(&stat);
	printf("child %d terminated\n", pid);
	return;
}

ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	
	ptr = vptr;
	nleft = n;
	while(nleft > 0)
	{
		if((nwritten = write(fd, ptr, nleft)) <= 0){
		
			if(nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1;
	  }	
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];
	again:
		while((n = read(sockfd, buf, MAXLINE)) > 0)
			writen(sockfd, buf, n);
		if(n < 0 && errno == EINTR)
			goto again;
		else if(n < 0)
			printf("str_echo: read error");
}

int main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	/*create a socket to listen*/
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	/*set seraddr be 0 at each bit*/
	bzero(&servaddr, sizeof(servaddr));

	/* AF_INET means IPv4*/
	servaddr.sin_family = AF_INET;

	/*32-bit IPv4 address
	s means short, h means host, n means net	
	change the host byte order to network byte order
	the IP address is long type. So use l
	INADDR_ANY means 0.
	It means tell the kernl to choose a IP 
	address for this address.
	*/
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/*16-bit TCP or UDP port numbere
	SERV_PORT means we define the port to 
	be SERV_PORT, 7988
	The port number is short type. So use s.
	*/

	servaddr.sin_port = htons(SERV_PORT);

	/*SA is generic socket address structure
	we bind this protocol address servaddr to 
	the socket listenfd. And the third argument is 
	the address length.			
	*/
	bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	/*
	make the socket listenfd be LISTEN stat from CLOSED
	and LISTENQ is the max number of connections	
	*/
	listen(listenfd, LISTENQ);

	/*signal handle function*/
	signal(SIGCHLD,sig_chld); 
	
	
	/*when we get request, connect*/
	for( ; ; ){
		clilen = sizeof(cliaddr);
		/*blocking because accept function*/
		connfd = accept(listenfd, (SA *)&cliaddr, &clilen);
		/*create another process to handle this connection
		make the main process to maintian listen*/
		if((childpid = fork()) == 0){
			close(listenfd);
			/*execute the jobs*/
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
	}
	return 0;
}
