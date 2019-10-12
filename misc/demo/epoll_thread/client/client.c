#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <pthread.h>


#define PORT 7778	
#define MAX_LINE 1024	//socket数量
#define MAX_THREAD_NUM	5 //线程数量
#define MAX_SOCKET_NUM 	150


pthread_t thread[MAX_SOCKET_NUM];
int max(int a , int b)
{
	return a > b ? a : b;
}

/*readline函数实现*/
ssize_t readline(int fd, char *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = read(fd, &c,1)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}

/*普通客户端消息处理函数*/
void *str_cli(void *sock_fd)
{
	int sockfd = *(int *)sock_fd;
	printf("start write_read data fd %d\n",sockfd);
	/*发送和接收缓冲区*/
	char sendline[MAX_LINE] , recvline[MAX_LINE];
	//while(fgets(sendline , MAX_LINE , stdin) != NULL)	
	memset(sendline,sockfd,MAX_LINE);
	int i = 0;
	int times = 10;
	while(times--)
	{
		int ret = 0;
		//printf("pre to write fd %d\n",sockfd);
		if(sockfd <= 0)
			continue;
		ret = write(sockfd , sendline , MAX_LINE);
		if(ret <= 0)
		{
			perror("write error!");
			//return;
			//exit(1);
		}
		printf("write fd:%d times:%d\n",sockfd,times);
		//printf("pre to read fd %d\n",sockfd);
		if(read(sockfd,recvline,MAX_LINE) <= 0)
		{
			perror("server terminated prematurely");
			printf("sockfd %d\n",sockfd);
			//return;
			//exit(1);
		}

		for(i=0;i<16;i++)
		{
			printf("%X",recvline[i]);
		}
		printf(" --%d--\n",sockfd);

	}//while
	printf("thread finish! %d\n",times);
	//close(sockfd);
}

void setNonblocking(int sockfd)
{
	int opts;
    opts=fcntl(sockfd,F_GETFL);
    if(opts<0)
    {
        perror("fcntl(sock,GETFL)");
        return;
    }//if

    opts = opts|O_NONBLOCK;
    if(fcntl(sockfd,F_SETFL,opts)<0)
    {
 		perror("fcntl(sock,SETFL,opts)");
        return;
    }//if
}


int main(int argc , char **argv)
{
	/*声明套接字和链接服务器地址*/
    int sockfd;
    int ret;
    struct sockaddr_in servaddr;
	int socket_fd_all[MAX_SOCKET_NUM];
    /*判断是否为合法输入*/
    // if(argc != 2)
    // {
    //     perror("usage:tcpcli <IPaddress>");
    //     exit(1);
    // }//if
	int i = 0;
	for(i=0;i<MAX_SOCKET_NUM;i++)
	{
		/*(1) 创建套接字*/
		if((sockfd = socket(AF_INET , SOCK_STREAM , 0)) == -1)
		{
			perror("socket error");
			exit(1);
		}//if

		/*(2) 设置链接服务器地址结构*/
		bzero(&servaddr , sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(PORT);
		unsigned char ipaddr[64] = "192.168.1.26:7777";
		if(inet_pton(AF_INET , ipaddr , &servaddr.sin_addr) < 0)
		{
			printf("inet_pton error for %s\n",ipaddr);
			exit(1);
		}//if

		/*(3) 发送链接服务器请求*/
		if(connect(sockfd , (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0)
		{
			perror("connect error");
			exit(1);
		}//if
		//setNonblocking(sockfd);
		socket_fd_all[i] = sockfd;
		printf("create new socket i:%d  fd:%d\n",i,sockfd);
		// str_cli(socket_fd_all[i]);
		// close(socket_fd_all[i]);
		
	}
	
	// for(i = 0;i<MAX_SOCKET_NUM;i++)
	// {
	// 	pthread_create(&thread[i],NULL,(void *)str_cli,(void *)&socket_fd_all[i]);
	// }
	int j = 100000;
	while(j--)
	{
		printf("--%d--:",j);
		i = random()%(MAX_SOCKET_NUM-1);
		str_cli(&socket_fd_all[i]);
	}

//	for(i = 0;i<MAX_SOCKET_NUM-1;i++)
//	{
//		printf("%d:",i);
//		str_cli((void *)&socket_fd_all[i]);
//		close(socket_fd_all[i]);
//	}
	// ret = close(socket_fd_all[MAX_SOCKET_NUM]);
	// if(ret != 0)
	// 	printf("closer last one error %d\n",ret);
	printf("test over!!!\n");
	char a = 0;
	scanf("%c",&a);
}
