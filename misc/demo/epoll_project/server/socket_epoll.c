/*
 * socket_epoll.c
 *
 *  Created on: 2019å¹´9æœˆ27æ—¥
 *      Author: jjn
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <fcntl.h>
#include <signal.h>
#include <sched.h>
#include <netinet/tcp.h>
#include  <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>
#include "../include/common.h"
#include "../include/queue.h"


#define SERVER_PORT				8087

#define MAIN_QUEUE_NUM			2048
#define MAX_CLIENT_NUM			1024
#define LISTEN_BACKLOG          2048
#define MAX_EVENT_COUNT         100
#define BUF_SIZE                1024*4
#define THREAD_NUMS				4

static int socket_num = 0;
struct epoll_event events[MAX_EVENT_COUNT];

static work_queue *mainWorkQueue;
struct epoll_event ev;
pthread_t thread_tid[THREAD_NUMS];
unsigned int thread_index[THREAD_NUMS];
pthread_mutex_t mutex;
int sock_pip[2];
int epfd;


static void setNonblocking(int sockfd)
{
	return;
	int opts;
	opts = fcntl(sockfd, F_GETFL);
	if (opts < 0)
	{
		perror("fcntl(sock,GETFL)");
		return;
	}//if

	opts = opts | O_NONBLOCK;
	if (fcntl(sockfd, F_SETFL, opts) < 0)
	{
		perror("fcntl(sock,SETFL,opts)");
		return;
	}//if
}



static  void *server_thread(void *index)
{
	int sockfd = 0;
    int pool_index = 0;
    int p = 0;
    unsigned char buff[BUF_SIZE];
    int i = 0;
    p = *(int *)index;
    pthread_detach(pthread_self());
    int data_len;
    printf("thread %d start!!!\n",p);
	int read_fd,read_len;
	int offset = 0;
	int remain_len = 0;
    int thread_times = 0;
	int close_flag = 0;
	packet_head *packet = 0;
	int ii = 0;
	while (1)
	{
			close_flag = 0;
			//printf("thread still running  %d\n",p);
			read_len = read(sock_pip[0],&read_fd,sizeof(int));
        	if(read_len == sizeof(int))
        	{
            		//printf("[%d]---GET---%d\n",p,read_fd);
        	}
        	else
            		continue;
        	read_len = read(read_fd,buff,BUF_SIZE);
			offset = read_len;
			if(read_len <= 0)
			{
				if(read_len == 0)
					close_flag = 1;
				else
				{
					close_flag = 2;
				}
			}
			else if(read_len < sizeof(packet_head))
			{
				while(offset < sizeof(packet_head))
				{
					printf("read first time < sizeof(packet_head)\n");
					read_len = read(read_fd,buff + offset,BUF_SIZE);
					offset += read_len;
					if(read_len <= 0)
					{
						if(read_len == 0)
							close_flag = 1;
						else
						{
							close_flag = 2;
						}
						break;
					}
				}
			}
			if(offset >= sizeof(packet_head))
			{
				packet = (packet_head *)buff;
				if(packet->sdf_cmd_head.len > offset)
				{
					remain_len = packet->sdf_cmd_head.len - offset;
					while(remain_len > 0)
					{
						read_len = read(read_fd,buff + offset,BUF_SIZE);
						remain_len -= read_len;
						offset += read_len;
						//printf("total_len:%d,recv_len:%d,remain_len:%d\n",packet->sdf_cmd_head.len,offset,remain_len);
						if(read_len <= 0)
						{
							if(read_len == 0)
								close_flag = 1;
							else
							{
								close_flag = 2;
							}	
							break;
						}
					}
				}
			}
			if(close_flag == 1)
        	{
				close(read_fd);
				struct epoll_event ev1;
				ev1.data.fd = read_fd;
				ev1.events = EPOLLIN| EPOLLET;	
				epoll_ctl(epfd , EPOLL_CTL_DEL , read_fd , &ev1);	
				printf("close sockfd %d\n",read_fd);
				continue;
        	}
        	else if(close_flag == 2)
        	{
				close(read_fd);
				struct epoll_event ev1;
				ev1.data.fd = read_fd;
				ev1.events = EPOLLIN| EPOLLET;	
				epoll_ctl(epfd , EPOLL_CTL_DEL , read_fd , &ev1);	
				printf("close ****8sockfd %d\n",read_fd);
				continue;
        	}	
			else
			{
				//printf("%d read_fd=%d\n",p,read_fd);
				sdf_rev_cmd((int)read_fd,(char *)buff ,offset);
				struct epoll_event ev1;
				ev1.data.fd = read_fd;
                ev1.events = EPOLLIN| EPOLLET;	
                epoll_ctl(epfd , EPOLL_CTL_ADD , read_fd , &ev1);	
				//printf("[%d]--OK--%d\n",p,read_fd);

				// ii++;
				// if(ii%10000 == 0)
				// {
				// 	printf("thread %d still runing\n",p);
				// }
			}

	}
	printf("thread exit\n");
	return NULL;
}


int create_thread_poll(void)
{
    int i,ret;
    for(i = 0;i<THREAD_NUMS;i++)
    {
        thread_index[i] = i;
    }
	//pthread_mutex_init(&mutex,NULL);
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



int socket_epoll()
{

	int i, listenfd, connfd, sockfd, nfds;
	ssize_t n, ret;
	socklen_t clilen;
	struct sockaddr_in servaddr , cliaddr;

	/*(1) 得到监听描述符*/
	listenfd = socket(AF_INET , SOCK_STREAM , 0);
	setNonblocking(listenfd);

	/*生成用于处理accept的epoll专用文件描述符*/	
	epfd = epoll_create(MAX_CLIENT_NUM);
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
	servaddr.sin_port = htons(SERVER_PORT);

	bind(listenfd , (struct sockaddr *)&servaddr , sizeof(servaddr));

	/*(3) 监听*/
	listen(listenfd , LISTEN_BACKLOG);
	workQueue(&mainWorkQueue,MAIN_QUEUE_NUM);
	create_thread_poll();
	pipe(sock_pip);

    int main_times = 0;
	int num = 0;//链接次数
	while (1)
	{
		/*等待事件发生*/
        memset(events,0,sizeof(struct epoll_event)*MAX_EVENT_COUNT);
		nfds = epoll_wait(epfd , events , MAX_EVENT_COUNT , 500);
		if(nfds <= 0)
			continue;
		if(nfds >= MAX_CLIENT_NUM)
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
                ev.data.fd = connfd;
                ev.events = EPOLLIN| EPOLLET;	
                epoll_ctl(epfd , EPOLL_CTL_ADD , connfd , &ev);	
			}//if
			else if(events[i].events & EPOLLIN)/*如果是已链接用户，并且收到数据，进行读入*/
            {
				if((sockfd = events[i].data.fd) < 0)
                {
                    continue;
                }
				//printf("recv new event %d\n",sockfd);
				// ret = putWork(&mainWorkQueue,sockfd);
				// if(ret == 0)
				// {
				// 	printf("put work %d\n",sockfd);
				// }

				ev.data.fd = sockfd;
                ev.events = EPOLLIN| EPOLLET;	
                epoll_ctl(epfd , EPOLL_CTL_DEL , sockfd , &ev);	
		        ret = write(sock_pip[1],&sockfd,sizeof(int));
				if(ret <= 0)
				{
					printf("pipe is error!!!\n");
					return -1;
				}
				
				
                // main_times++;
				// if(main_times%1000 == 0)
                // 	printf("main_thread times == %d\n",main_times);										
			}//else
		}
	}


	printf("main thread exit\n");
	return 0;
}



