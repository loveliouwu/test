#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#define SERVER_PORT 60000
#define BUF_SIZE                1024

void printf_buff(unsigned char* data, int len)
{
	while (len >= 16)
	{
		printf("%02x %02x %02x %02x %02x %02x %02x %02x    %02x %02x %02x %02x %02x %02x %02x %02x\r\n",
			data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
			data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
		len -= 16;
		data += 16;
	}
	if (len >= 8)
	{
		printf("%02x %02x %02x %02x %02x %02x %02x %02x    ",
			data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
		len -= 8;
		data += 8;
	}

	if (len > 0)
	{
		while (len > 0)
		{
			printf("%02x ", *data++);
			len--;
		}
		printf("\r\n");
	}
}

char sendbuf[BUF_SIZE];
char recvbuf[BUF_SIZE];

#define WORK_THREAD_NUM			2000
#define LOOP_TIMES   100
unsigned int t[WORK_THREAD_NUM];

void* workThead(void* val)
{

	//客户端只需要一个套接字文件描述符，用于和服务器通信
	long v= (long)val;
	int clientSocket,ret;
	//描述服务器的socket
	struct sockaddr_in serverAddr;
	int iDataNum, oData;
	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error\n");
		return NULL;
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	//指定服务器端的ip，本地测试：127.0.0.1
	//inet_addr()函数，将点分十进制IP转换成网络字节序IP
	serverAddr.sin_addr.s_addr = inet_addr("192.168.1.250");
	if (connect(clientSocket, (struct sockaddr*) & serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("connect fail\n");
		return NULL;
	}
	struct sockaddr_in clientAddr;
	socklen_t socketAddrLen = sizeof(struct sockaddr_in);
	if (getsockname(clientSocket, (struct sockaddr*) & clientAddr, &socketAddrLen) != 0)
	{
		perror("get clientSocket name error");
	}
	printf("port = %d conect ok...\n", clientAddr.sin_port);
	memset(sendbuf, clientSocket, BUF_SIZE);
	t[v] = 0;
	int sendNum, recvNum, sendNumTmp, recvNumTmp;
	while (t[v] < LOOP_TIMES)
	{
		t[v]++;
		sendNum = 0;
		do {
			sendNumTmp = send(clientSocket, sendbuf + sendNum, BUF_SIZE - sendNum, 0);
			if (sendNumTmp == -1)
			{
				printf("port = %d,send data error,t=%d,%s,%d\n", clientAddr.sin_port,t[v], strerror(errno), errno);
				goto exit;
			}
			sendNum += sendNumTmp;
			//printf("sendNum = %d,sendNumTmp=%d\n",sendNum,sendNumTmp);
			if (sendNum > BUF_SIZE)
			{
				printf("send error,%d\n", clientAddr.sin_port);
				close(clientSocket);
				goto exit;
			}
		} while (sendNum != BUF_SIZE);

		recvNum = 0;
		do {
			recvNumTmp = recv(clientSocket, recvbuf + recvNum, BUF_SIZE - recvNum, 0);
			if (recvNumTmp == -1)
			{
				printf("port = %d,recv data error,t=%d,%s\n", clientAddr.sin_port, t[v],strerror(errno));
				goto exit;
			}
			recvNum += recvNumTmp;
		} while (recvNum != BUF_SIZE);
	}
	exit:
	printf("%d finsih ********************************\n", clientAddr.sin_port);
	close(clientSocket);
	return 0;
}



int main()
{
	pthread_t workThreadT[WORK_THREAD_NUM];
	for (int i = 0; i < WORK_THREAD_NUM; i++)
	{
		pthread_create(&workThreadT[i], NULL, workThead, (void *)i);
	}
	//while (1)
	//{
	//	char str[128];
	//	scanf("%s", str);
	//	if (str[0] == 'q')
	//		break;
	//	if (str[0] == 'p')
	//	{
	//		for (int i = 0; i < WORK_THREAD_NUM; i++)
	//		{
	//			printf("t[%d] = %d\n",i,t[i]);
	//		}
	//	}
	//}
	for (int i = 0; i < WORK_THREAD_NUM; i++)
	{
		pthread_join(workThreadT[i],NULL);
	}
}

