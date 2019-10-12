#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <pthread.h>

#include"workfifo.h"
#include"double_list.h"


#ifndef CONNECT_SIZE
#define CONNECT_SIZE 1000
#endif

#define PORT 7778
#define MAX_LINE 2048
#define LISTENQ 1
#define EVENT_SIZE 20

#define BUFF_SIZE   256


//#define DEBUG 
#ifdef DEBUG
#define SERVER_DEBUG(fmt,arg...) printf("[DEBUG %s:%s:%d]"fmt,__FILE__,__func__,__LINE__,##arg)
#else
#define SERVER_DEBUG(fmt,arg...)
#endif
#define SERVER_ERR(fmt,arg...) printf("[ERR %s:%s:%d]"fmt,__FILE__,__func__,__LINE__,##arg)



pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;

/*声明epoll_event结构体变量，ev用于注册事件，数组用于回传要处理的事件*/
struct epoll_event ev,ev_write,ev_read, events[EVENT_SIZE];
int epfd;
/*队列，用于存放要处理事件的fd*/
sequeue_t *queue,*queue_read,*queue_write;

/*链表，用于存储将要发送的数据*/
list_info *plist_info;


#define THREAD_NUMS  20
unsigned int thread_data[THREAD_NUMS][3];
pthread_t thread_tid[THREAD_NUMS];
unsigned int thread_index[THREAD_NUMS];
int sock_pip[2];


void *server_thread(void *index)
{
    int sockfd = 0;
    int pool_index = 0;
    int p = 0;
    unsigned char buff[MAX_LINE];
    int i = 0;
    p = *(int *)index;
    pthread_detach(pthread_self());
    pool_index = thread_data[p][1];
    int data_len;
    printf("thread %d start!!!\n",p);
	int read_fd,read_len;

    int thread_times = 0;
    while(1)
    {
        // if(thread_data[p][0] != 1)
        //     continue;
        // else
        // {
        //     printf("thread %d recv a new event!\n",thread_data[p][0]);
        // }
        // sockfd = thread_data[p][2];
        // if(sockfd <= 0)
        // {
        //     printf("thread_index %d sockfd %d is error!\n",pool_index,sockfd);
        //     continue;
        // }
	    read_len = read(sock_pip[0],&read_fd,sizeof(int));
        if(read_len == sizeof(int))
        {
            printf("read_fd %d\n",read_fd);
        }
        else
            continue;
        //data_len = recv(sockfd,buff,MAX_LINE,0);
        data_len = recv(read_fd,buff,MAX_LINE,0);
        if(data_len == 0)
        {
            struct epoll_event ev1;
            ev1.data.fd = read_fd;
            ev1.events = EPOLLIN| EPOLLET;	
            epoll_ctl(epfd , EPOLL_CTL_DEL , read_fd , &ev1);	
            close(read_fd);
            printf("close sockfd %d\n",read_fd);
            continue;
        }
        // for(i = 0;i<MAX_LINE;i++)
        // {
        //     buff[i] = buff[i]^0x5a;
        // }
        //data_len = send(sockfd,buff,data_len,0);
        data_len = send(read_fd,buff,data_len,0);
        thread_times++;
        printf("thread %d times %d,send_len %d\n",p,thread_times,data_len);
        //thread_data[p][0] = 0;
    }
}



int create_thread_poll(void)
{
    int i,ret;
    for(i = 0;i<THREAD_NUMS;i++)
    {
        thread_data[i][0] = 0;//运行标记
        thread_data[i][1] = i;//线程池索引
        thread_data[i][2] = 0;//传递参数fd
        pthread_mutex_init(&thread_mutex,NULL);
    }
    
    for(i = 0;i<THREAD_NUMS;i++)
    {
        thread_index[i] = i;
    }
    for(i = 0;i<THREAD_NUMS;i++)
    {
        ret = pthread_create(&thread_tid[i],NULL,(void *)server_thread,(void *)&thread_index[i]);
        if(ret != 0)
        {
            printf("create thread error!\n");
            return -1;
        }
    }
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
	int i, listenfd, connfd, sockfd, nfds;

	ssize_t n, ret;
		
	char buf[MAX_LINE];

	socklen_t clilen;

	struct sockaddr_in servaddr , cliaddr;

    unsigned char *buffer;
    data_list list_data;
    data_t data;

	/*队列初始化*/


	/*(1) 得到监听描述符*/
	listenfd = socket(AF_INET , SOCK_STREAM , 0);
	setNonblocking(listenfd);

	/*生成用于处理accept的epoll专用文件描述符*/	
	epfd = epoll_create(CONNECT_SIZE);
	/*设置监听描述符*/
	ev.data.fd = listenfd;
	/*设置处理事件类型*/
	ev.events = EPOLLIN | EPOLLET;
	/*注册事件*/
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);		

	/*(2) 绑定套接字*/	
	bzero(&servaddr , sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	bind(listenfd , (struct sockaddr *)&servaddr , sizeof(servaddr));

	/*(3) 监听*/
	listen(listenfd , LISTENQ);
    
    // pthread_t thread_read,thread_write;
    // pthread_create(&thread_read,NULL,socket_read_thread,NULL);
    // pthread_create(&thread_write,NULL,socket_write_thread,NULL);
    //pthread_create(&thread_write,NULL,write_thread,NULL);
	/*(4) 进入服务器接收请求死循环*/
    int num = 0,num2 = 0;
    unsigned int socket_fds[CONNECT_SIZE];

    create_thread_poll();
	pipe(sock_pip);
    int main_times = 0;
	while(1)
	{
		/*等待事件发生*/
        //SERVER_DEBUG("waiting fo events!\n");
        memset(events,0,sizeof(struct epoll_event)*EVENT_SIZE);
		nfds = epoll_wait(epfd , events , EVENT_SIZE , 500);
		if(nfds <= 0)
			continue;
	
		printf("nfds = %d  \n" , nfds);
		/*处理发生的事件*/
		for(i=0 ; i<nfds ; ++i)
		{
			/*检测到用户链接*/
			if(events[i].data.fd == listenfd)
			{	
				/*接收客户端的请求*/
				clilen = sizeof(cliaddr);

				if((connfd = accept(listenfd , (struct sockaddr *)&cliaddr , &clilen)) < 0)
				{
					perror("accept error.\n");
					exit(1);
				}//if		
                num++;
				printf("accpet a new client i:%d  fd:%d : %s:%d \n", num-1,connfd,inet_ntoa(cliaddr.sin_addr) , cliaddr.sin_port);
			
				/*设置为非阻塞*/
				setNonblocking(connfd);
                /*
                lyp:第一次连接时触发的是EPOLLIN,所以修改以等待客户端调用write
                */
                ev.data.fd = connfd;
                ev.events = EPOLLIN| EPOLLET;	
                epoll_ctl(epfd , EPOLL_CTL_ADD , connfd , &ev);	
			}//if
			else if(events[i].events & EPOLLIN)/*如果是已链接用户，并且收到数据，进行读入*/
            {
				if((sockfd = events[i].data.fd) < 0)
                {
                    SERVER_ERR("socket fd error!\n");
                    continue;
                }
				int j = 0;
                //printf("new event!\n");
                for(j = 0;j<THREAD_NUMS;j++)
                {
                    if(0 == thread_data[j][0])
                        break;
                }
                if(j>=THREAD_NUMS)
                {
                    printf("thread_poll is full!\n");
                    continue;
                }
                //printf("sockfd %d EVENT!\n",sockfd);
                //thread_data[j][2] = sockfd;
                //thread_data[j][0] = 1;
		        write(sock_pip[1],&sockfd,sizeof(int));
                main_times++;
                printf("main_thread times == %d\n",main_times);										
			}//else
			else if(events[i].events & EPOLLOUT)
			{
                
                //continue;
                /*设置用于读的文件描述符和事件*/
				// ev.data.fd = sockfd;
				// ev.events = EPOLLOUT | EPOLLET;
				// /*修改*/
				// epoll_ctl(epfd , EPOLL_CTL_DEL , sockfd , &ev);

				// if((ret = write(sockfd , buf , n)) != n)	
				// {
				// 	printf("error writing to the sockfd!\n");
				// 	break;
				// }//if
				// /*设置用于读的文件描述符和事件*/
				// ev.data.fd = sockfd;
				// ev.events = EPOLLIN | EPOLLET;
				// /*修改*/
				// epoll_ctl(epfd , EPOLL_CTL_MOD , sockfd , &ev);				
			}//else
		}//for
	}//while
	free(events);
	close(epfd);
	exit(0);
}
