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

#define PORT 7777
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



void *socket_read_thread(void *data)
{
    int fd;
    int read_len;
    int i,ret;
    char orand = 0xaa;
    unsigned char *buffer;
    data_t que_data;
    int fd_tmp;
    while(1)
    {
        
        if(isEmpty_queue(queue_read))
            continue;
        ret = De_queue(queue_read,&que_data);
        if(ret != 0)
        {
            continue;
        }
        buffer = (unsigned char *)malloc(BUFF_SIZE);
        // if(buffer == NULL)
        // {
        //     SERVER_ERR("malloc buffer error!\n");
        //     ret = En_queue(queue_read,que_data);
        //     if(ret != 0)
        //     {
        //         SERVER_ERR("enqueue error!\n");
        //     }
        //     free(buffer);
        //     goto exit1;
        // }
        fd = que_data.fd;
        SERVER_DEBUG("start read fd:%d\n",fd);
        if((read_len = read(fd , buffer , BUFF_SIZE)) <= 0)
        {
            close(fd);  
            SERVER_ERR("read error! close socket %d !\n\n",fd);
            free(buffer);
            ev_read.data.fd = fd;
            ev_read.events = EPOLLOUT| EPOLLET;	
            epoll_ctl(epfd , EPOLL_CTL_DEL , fd , &ev_read);
            continue;
        }//if
        que_data.fd_data = buffer;
        for(i = 0;i<BUFF_SIZE;i++)
        {
            buffer[i] = buffer[i]^orand;
        }
        if((ret = write(fd , buffer , BUFF_SIZE)) != BUFF_SIZE)	
        {
            SERVER_ERR("error writing to the sockfd!\n");
        }//if


        // SERVER_DEBUG("fd %d read ok\n",fd);
        // ret = En_queue(queue,que_data);
        // if(ret != 0)
        // {
        //     SERVER_ERR("en write queue err!\n");
        //     free(buffer);
        //     continue;
        // }
        // SERVER_DEBUG("fd %d En queue\n",fd);
        // ev.data.fd = fd;
        // ev.events = EPOLLOUT| EPOLLET;	
        // epoll_ctl(epfd , EPOLL_CTL_MOD , fd , &ev);
        

 exit1:   
        ev_read.data.fd = fd;
        ev_read.events = EPOLLIN| EPOLLET;	
        epoll_ctl(epfd , EPOLL_CTL_MOD , fd , &ev_read);	
        continue;



    }

}

void *socket_write_thread(void *data)
{
    int ret;
    int fd,fd_tmp;
    unsigned char *buffer;
    double_list *node;
    data_t que_data;
    while(1)
    {
        
        if(isEmpty_queue(queue_write))
            continue;
        SERVER_DEBUG("queue_write ");
        Show_queue(queue_write);
        ret = De_queue(queue_write,&que_data);
        if(ret != 0)
        {
            SERVER_ERR("dequeue error!\n");
            continue;
        }
        fd = que_data.fd;
        fd_tmp = fd;
        if(fd <= 0)
        {
            SERVER_ERR("fd error!\n");
            continue;
        }
        SERVER_DEBUG("fd %d is ready to write!\n",fd);



        // ret = De_queue_by_fd(queue,fd,&que_data);
        // if(ret != 0)
        // {
        //     SERVER_ERR("not find fd %d in queue\n",fd);
        //     goto exit;
        // }
        // fd = que_data.fd;
        // buffer = que_data.fd_data;
        // if((ret = write(fd , buffer , BUFF_SIZE)) < 0)	
        // {
        //     SERVER_ERR("error writing to the sockfd!\n");
        //     ret = En_queue(queue,que_data);
        //     if(ret != 0)
        //     {
        //         SERVER_ERR("En_queue queue error! fd %d\n",fd);
        //     }
        //     goto exit;
        // }//if
        //释放资源
        SERVER_DEBUG("fd %d write ok!\n",fd);
        if(buffer != NULL)
            free(buffer);
        ev_write.data.fd = fd;
        ev_write.events = EPOLLIN | EPOLLET;
        /*修改*/
        epoll_ctl(epfd , EPOLL_CTL_MOD , fd , &ev_write);
        continue;

exit:        
        ev_write.data.fd = fd;
        ev_write.events = EPOLLOUT | EPOLLET;
        /*修改*/
        epoll_ctl(epfd , EPOLL_CTL_MOD , fd , &ev_write);
    }


}



void *write_thread(void *data)
{
    int ret;
    int fd;
    unsigned char *buffer;
    data_t que_data;

    while(1)
    {
        // if(isEmpty_queue(queue_write))
        //     continue;
        ret = De_queue(queue_write,&que_data);
        //SERVER_DEBUG("thread_write running!\n");
        if(ret != 0)
        {
            //SERVER_ERR("dequeue error!\n");
            //En_queue(queue_write,que_data);
            continue;
        }
        fd = que_data.fd;
        buffer = que_data.fd_data;
        if(fd <= 0)
        {
            SERVER_ERR("fd error!\n");
            continue;
        }
        printf("write data fd:%d\n",fd);
        if((ret = write(fd , buffer , BUFF_SIZE)) < 0)	
        {
            SERVER_ERR("error writing to the sockfd!\n");
        }//if
        ev.data.fd = fd;
        ev.events = EPOLLIN | EPOLLET;
        /*修改*/
        epoll_ctl(epfd , EPOLL_CTL_MOD , fd , &ev);
        // close(fd);
        // printf("thread close %d\n",fd);
        //SERVER_DEBUG("thread_write_continue\n");
        if(buffer)
            free(buffer);
        
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
    ret = Create_queue(&queue);
    ret = Create_queue(&queue_read);
    ret = Create_queue(&queue_write);
	ret = Create_double_list(&plist_info);

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
    pthread_mutex_init(&thread_mutex,NULL);
    pthread_t thread_read,thread_write;
    pthread_create(&thread_read,NULL,socket_read_thread,NULL);
    pthread_create(&thread_write,NULL,socket_write_thread,NULL);
    //pthread_create(&thread_write,NULL,write_thread,NULL);
	/*(4) 进入服务器接收请求死循环*/
    int num = 0,num2 = 0;
    unsigned int socket_fds[CONNECT_SIZE];
    memcpy(&ev_read,&ev,sizeof(ev));
    memcpy(&ev_write,&ev,sizeof(ev));
	while(1)
	{
		/*等待事件发生*/
        //SERVER_DEBUG("waiting fo events!\n");
		nfds = epoll_wait(epfd , events , EVENT_SIZE , -1);
		if(nfds <= 0)
			continue;
	
		//printf("nfds = %d  \n" , nfds);
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
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd , EPOLL_CTL_ADD , connfd , &ev);
                socket_fds[num-1] = connfd;
                int ii = 0;
                printf("socket:\n");
                for(ii = 0;ii<num;ii++)
                {
                    printf("%d:%d ",ii,socket_fds[ii]);
                }
                printf("\n");
                /*
                lyp:第一次连接时触发的是EPOLLIN,所以修改以等待客户端调用write
                */
                // ev.data.fd = connfd;
                // ev.events = EPOLLIN| EPOLLET;	
                // epoll_ctl(epfd , EPOLL_CTL_MOD , connfd , &ev);	
			}//if
			else if(events[i].events & EPOLLIN)/*如果是已链接用户，并且收到数据，进行读入*/
            {
				if((sockfd = events[i].data.fd) < 0)
                {
                    SERVER_ERR("socket fd error!\n");
                    continue;
                }
					

                //buffer = (unsigned char *)malloc(BUFF_SIZE);
                num2++;
                printf("recv clint[%d] message: fd:%d\n", num2-1 , sockfd);
                data.fd = sockfd;
                data.fd_data = buffer;
                
                if(Inor_queue(queue_read,data) == 1)
                {
                    SERVER_DEBUG("fd %d is already in queue_read\n",sockfd);
                    printf("queue_read [%d] " ,sockfd);
                    Show_queue(queue_read);
                    continue;
                }
                ret = En_queue(queue_read,data);
                if(ret != 0)
                {
                    printf("Enqueue err!\n");
                }
                SERVER_DEBUG("%d is ready En_queue_Read\n",sockfd);
                // ev.data.fd = sockfd;
                // ev.events = EPOLLOUT| EPOLLET;	
                // epoll_ctl(epfd , EPOLL_CTL_MOD , sockfd , &ev);
                
                


				// bzero(buf , MAX_LINE);
				// printf("reading the socket~~~\n");
				// if((n = read(sockfd , buf , MAX_LINE)) <= 0)
				// {
				// 	close(sockfd);
				// 	events[i].data.fd = -1;
				// }//if
				// else{
				// 	buf[n] = '\0';
				// 	printf("clint[%d] send message: %s\n", i , buf);
				
				// 	/*设置用于注册写操作文件描述符和事件*/
                // ev.data.fd = sockfd;
                // ev.events = EPOLLOUT| EPOLLET;	
                // epoll_ctl(epfd , EPOLL_CTL_MOD , sockfd , &ev);			
				// }//else											
			}//else
			else if(events[i].events & EPOLLOUT)
			{
				if((sockfd = events[i].data.fd) < 0)
				    continue;
                data_t write_fd;
                write_fd.fd = sockfd;
                write_fd.fd_data = NULL;
                
                if(Inor_queue(queue_write,write_fd) != 0)
                {
                    SERVER_DEBUG("fd %d is already in queue_write\n",sockfd);
                    continue;
                }
                ret = En_queue(queue_write,write_fd);
                if(ret != 0)
                {
                    printf("En_queue error!\n");
                    continue;
                }
                SERVER_DEBUG("%d is ready En_queue_Write\n",sockfd);
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