#include <cstdio>

/*

int main()
{
    printf("hello from epoll_server!\n");
    return 0;
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "workQueue.h"
#include <pthread.h>
#include <fcntl.h>
#include <signal.h>
#include <sched.h>
#include <cerrno>
#include <netinet/tcp.h>
#include  <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h> 
using namespace std;

#define MAX_CLIENT_NUM			1024
#define MAIN_QUEUE_NUM			20480
#define SERVER_PORT				60000
#define LISTEN_BACKLOG          2048
#define MAX_EVENT_COUNT         MAX_CLIENT_NUM //(MAX_CLIENT_NUM * 3+1)
#define BUF_SIZE                1024
#define WORK_THREAD_NUM			4


unsigned long usedEventNum[WORK_THREAD_NUM] = { 0 }, totalEventNum = 0, threadStatus[WORK_THREAD_NUM] = { 0 }, recvNum[WORK_THREAD_NUM] = { 0 };


static int socket_num = 0;
static  workQueue mainWorkQueue;
static int server_fd, client_fd, epoll_fd;
static bool continueRun = true;
//static  pthread_cond_t cond;
//static  pthread_mutex_t mutex;
static struct epoll_event event_array[MAX_EVENT_COUNT];

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

	//int nRecvBuf = 1024 *1024;//2 * 1024;//����Ϊ32K
	//setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const char*)& nRecvBuf, sizeof(int));
	//int nSendBuf = 0;// 2 * 1024;//����Ϊ32K
	//setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const char*)& nSendBuf, sizeof(int));
}


static void clientClose(int fd)
{
	epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP;//
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd,&ev);
}

void clientConnect(int fd)
{
	epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP;//
	int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
	if (ret == -1)
	{
		printf("fd = %d\n",fd);
		perror("epoll_create failed.\n");
	}
	else
	{
	//	printf("fd = %d,success\n", fd);
	}
}


static  void *workThead(void *val)
{
	int ret;
	epoll_event *threadEvent;
	struct sockaddr_in client_addr;
	socklen_t addr_len;
	char* buf =(char*) malloc(BUF_SIZE);
	unsigned long threadNum = (unsigned long)val;
	void* ptr;
	unsigned long clientFd,eventFlag;
	struct sockaddr_in clientAddr;
	socklen_t socketAddrLen = sizeof(struct sockaddr_in);
	int sendNum,sendNumTmp,recvNumTmp;
	struct timeval now,start;
	struct timespec outtime;
	struct timeval tv;






	while (continueRun)
	{
		threadStart:
		threadStatus[threadNum] = 0;
		if (!mainWorkQueue.getWork(&ptr, &eventFlag, &clientFd))
		{
			/*gettimeofday(&now, NULL);
			outtime.tv_sec = now.tv_sec + 1;
			outtime.tv_nsec = now.tv_usec;
			pthread_cond_timedwait(&cond, &mutex, &outtime);*/
			threadStatus[threadNum] = 1;
			continue;
		}
		threadStatus[threadNum] = 2;
		usedEventNum[threadNum]++;
		if (eventFlag & EPOLLERR)
		{
			perror("EPOLLERR");
			close(clientFd);
			clientClose(clientFd);
			goto threadStart;
		}
		threadStatus[threadNum] = 3;
		//printf("%d data xfer,fd=%d,event=%#x\n", threadNum, clientFd, eventFlag);
		if (getpeername(clientFd, (struct sockaddr*) &clientAddr,&socketAddrLen) != 0)
		{
			clientAddr.sin_port = 0xffff;
			//perror("getpeername error");
			//close(clientFd);
			//clientClose(clientFd);
			//goto threadStart;
		}
		threadStatus[threadNum] = 4;
		if (eventFlag & EPOLLIN)
		{
			threadStatus[threadNum] = 5;
			gettimeofday(&start, NULL);
			recvNum[threadNum] = 0;
			do {
				threadStatus[threadNum] = 6;
				recvNumTmp = recv(clientFd, buf + recvNum[threadNum], BUF_SIZE - recvNum[threadNum], 0);
				if (recvNumTmp == 0)
				{
					//printf("port = %d,socket disconect,%s,%d\n", clientAddr.sin_port, strerror(errno), errno);
					close(clientFd);
					clientClose(clientFd);
					goto threadStart;
				}
				if (recvNumTmp == -1) {
					if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
					{
						//printf("port = %d,not recv,%s,%d\n", clientAddr.sin_port, strerror(errno), errno);
						continue;
					}
					else
					{
						//printf("port = %d,recv error,close socket,%s,%d\n", clientAddr.sin_port, strerror(errno), errno);
						close(clientFd);
						clientClose(clientFd);
						goto threadStart;
					}
				}




				recvNum[threadNum] += recvNumTmp;

				if (recvNum[threadNum] > BUF_SIZE)
				{
					printf("recvNum error,port = %d,recvNum=%d\n", clientAddr.sin_port, recvNum[threadNum]);
				}

				gettimeofday(&now, NULL);
				if (now.tv_sec - start.tv_sec >= 2)
				{
					printf("port = %d,recv time out,recvNum = %d\n", clientAddr.sin_port, recvNum[threadNum]);
					//close(clientFd);
					//clientClose(clientFd);
					goto threadStart;
				}

			} while (recvNum[threadNum] < BUF_SIZE);
			threadStatus[threadNum] = 7;

			gettimeofday(&start, NULL);
			sendNum = 0;
			do
			{
				sendNumTmp = send(clientFd, buf + sendNum, BUF_SIZE - sendNum, 0);
				if (sendNumTmp == -1) {
					printf("port = %d,send error,%s\n", clientAddr.sin_port, strerror(errno));
					close(clientFd);
					clientClose(clientFd);
					goto threadStart;
				}
				sendNum += sendNumTmp;

				gettimeofday(&now, NULL);
				if (now.tv_sec - start.tv_sec >= 2)
				{
					printf("port = %d,send time out,recvNum = %d\n", clientAddr.sin_port, sendNum);
					close(clientFd);
					clientClose(clientFd);
					goto threadStart;
				}
			} while (sendNum != BUF_SIZE);
			threadStatus[threadNum] = 8;
		}
		if (eventFlag & EPOLLMSG)
		{
			printf("port = %d,epoll msg,%s,%d\n", clientAddr.sin_port, strerror(errno), errno);
			close(clientFd);
			clientClose(clientFd);
			goto threadStart;
		}

		if (eventFlag & EPOLLERR)
		{
			printf("port = %d,epoll error,%s,%d\n", clientAddr.sin_port, strerror(errno), errno);
			close(clientFd);
			clientClose(clientFd);
			goto threadStart;
		}
	}
	printf("thread exit\n");
}


static  void* acceptThread(void* val)
{
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(struct sockaddr_in);
	
	while (continueRun)
	{
		client_fd = accept(server_fd, (struct sockaddr*) & client_addr, &addr_len);
		if (client_fd == -1) {
			continue;
		}
		clientConnect(client_fd);
		setNonblocking(client_fd);
		socket_num++;
		printf("new connect,port = %d,fd = %d,socket_num = %d\n", client_addr.sin_port, client_fd, socket_num);
	}
}

static  void* waitThread(void* val)
{
	int ready_count,i;
	while (continueRun) {
		ready_count = epoll_wait(epoll_fd, event_array, MAX_CLIENT_NUM - 32, -1);
		if (ready_count == -1) {
			perror("epoll_wait failed.\n");
			continue;
		}
		totalEventNum += ready_count;
		for (i = 0; i < ready_count; i++)
		{
			while (mainWorkQueue.putWork(&event_array[i], event_array[i].events, event_array[i].data.fd) == false)
			{
				//printf("main work queue full\n");
			}
			//pthread_cond_signal(&cond);
		}
	}
}




int main() {
	
	int ret, i;
	
	struct sockaddr_in server_addr;
	struct epoll_event event;
	char* buf;
	pthread_t workThreadT[WORK_THREAD_NUM], acceptThreadT, waitThreadT;

	mainWorkQueue = workQueue(MAIN_QUEUE_NUM);
	buf = (char*)malloc(sizeof(char) * BUF_SIZE);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("192.168.1.250");// htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);

	struct sockaddr_in clientAddr;
	socklen_t socketAddrLen = sizeof(struct sockaddr_in);
	//signal(SIGPIPE, SIG_IGN);

	server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (server_fd <= 0) {
		perror("create socket failed.\n");
		return 1;
	}
	printf("server_fd = %d\n",server_fd);
	int value = 1;
	
	setNonblocking(server_fd);

	int one = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)& one, sizeof(int)) < 0) /*����socket�����������׽ӿں�һ������ʹ���еĵ�ַ����*/
	{
		return -2;
	}


	one = 1;
	if (setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, (char*)& one, sizeof(int)) < 0) /*����socket��������ֹNagle�㷨��Nagle�㷨ͨ����δȷ�ϵ����ݴ��뻺����ֱ������һ����һ���͵ķ������������������͵�����С���ݰ�����Ŀ��*/
	{
		return 1;
	}


	struct linger m_linger;
	m_linger.l_onoff = 1;
	m_linger.l_linger = 0;
	if (setsockopt(server_fd, SOL_SOCKET, SO_LINGER, (const char*)& m_linger, sizeof(m_linger)) < 0) /*����socket��������ر�(closesocket()������ִ��)ʱ��δ�������ݣ�������*/
	{
		return 1;
	}


	bool bConditionalAccept = true;
	setsockopt(server_fd, SOL_SOCKET, SO_ACCEPTCONN, (const char*)& bConditionalAccept, sizeof(bool));

	int nRecvBuf = 1024*1024;//4 * 1024;//����Ϊ32K
	setsockopt(server_fd, SOL_SOCKET, SO_RCVBUF, (const char*)& nRecvBuf, sizeof(int));
	int nSendBuf = 0;// 4 * 1024;//����Ϊ32K
	setsockopt(server_fd, SOL_SOCKET, SO_SNDBUF, (const char*)& nSendBuf, sizeof(int));


	ret = bind(server_fd, (struct sockaddr*) & server_addr, sizeof(server_addr));
	if (ret == -1) {
		perror("bind failed.\n");
		return 1;
	}
	ret = listen(server_fd, LISTEN_BACKLOG);
	if (ret == -1) {
		perror("listen failed.\n");
		return 1;
	}

	epoll_fd = epoll_create(MAX_CLIENT_NUM);
	if (epoll_fd == -1) {
		perror("epoll_create failed.\n");
		return 1;
	}

	setNonblocking(epoll_fd);

	/*event.events = EPOLLIN | EPOLLET;
	event.data.fd = server_fd;
	ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);
	if (ret == -1) {
		perror("epoll_ctl failed.\n");
		return 1;
	}*/

	//cpu_set_t mask;
	//CPU_ZERO(&mask);
	//CPU_SET(1, &mask);
	//if (sched_setaffinity(0, sizeof(mask),&mask) < 0) {
	//	perror("sched_setaffinity");
	//}
	//sleep(1);

	/*pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);*/


	pthread_create(&acceptThreadT, NULL, acceptThread, NULL);
	pthread_create(&waitThreadT, NULL, waitThread, NULL);

	for (i = 0; i < WORK_THREAD_NUM; i++)
	{
		pthread_create(&workThreadT[i],NULL,workThead,(void *)(i));
	}
	
	while (1)
	{
		char str[128];
		scanf("%s", str);
		if (str[0] == 'q')
		{
			continueRun = false;
		}
		if (str[0] == 'p')
		{
			printf("totalEventNum = %d\n",totalEventNum);
			printf("usedTotalEventNum[3] = %d\n", usedEventNum[3] + usedEventNum[2] + usedEventNum[1] + usedEventNum[0]);
			printf("workNum = %d\n", mainWorkQueue.getWorkNum());
			if (totalEventNum != (usedEventNum[3] + usedEventNum[2] + usedEventNum[1] + usedEventNum[0] + mainWorkQueue.getWorkNum()))
			{
				printf("event not all used\n");
			}
			printf("Thread0:usedEventNum = %d,threadStatus=%d,recvNum=%d\n", usedEventNum[0],threadStatus[0], recvNum[0]);
			printf("Thread1:usedEventNum = %d,threadStatus=%d,recvNum=%d\n", usedEventNum[1], threadStatus[1], recvNum[1]);
			printf("Thread2:usedEventNum = %d,threadStatus=%d,recvNum=%d\n", usedEventNum[2], threadStatus[2], recvNum[2]);
			printf("Thread3:usedEventNum = %d,threadStatus=%d,recvNum=%d\n", usedEventNum[3], threadStatus[3], recvNum[3]);
			
		}
	}

	for (i = 0; i < WORK_THREAD_NUM; i++)
	{
		pthread_join(workThreadT[i],NULL);
	}
	pthread_join(acceptThreadT, NULL);
	pthread_join(waitThreadT, NULL);

	printf("main thread exit\n");

	close(epoll_fd);
	close(server_fd);
	free(event_array);
	free(buf);


	
	return 0;
}