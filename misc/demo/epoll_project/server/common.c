#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../include/common.h"
#include "../include/sdf.h"
#include "../alg/sm3.h"
#include "../include/sdf_manage.h"
#include "openssl/rand.h"
#include "openssl/bn.h"

#define MYPORT 8088    // the port users will be connecting to
#define BACKLOG SOCKET_NUM    // how many pending connections queue will hold
#define BUF_SIZE SOCKET_MAX_DATA_LEN
int fd_A[BACKLOG];    // accepted connection fd
pthread_t thread_num[BACKLOG];
pthread_mutex_t thread_lock;
int sock_fd;
int maxsock;
int conn_amount;      // current connection amount

void get_random(unsigned char *data,int len)
{
	BIGNUM *bn=NULL;

	bn=BN_new();
	BN_rand(bn, len*8, -1, 1); //生成指定bits的随机数
	data = (unsigned char *)BN_bn2hex(bn); //转化成16进制字符串
	BN_free(bn); //释放BIGNUM结构
}
void PrintBuf(unsigned char *buf, int buflen)
{
	int i;
	for(i=0; i<buflen; i++) {
  	if (i % 32 != 31)
  	  printfS("%02x", buf[i]);
  	  else
  	  printfS("%02x\n", buf[i]);
  }
  printfS("\n");
}
void free_point(void *p)
{
	if(p!=NULL)
	{
		free(p);
		p=NULL;
	}
}

void sdf_rev_cmd(int sockfd,char* data,int len)
{
	xmit_protocol *rev_xmit_protocol;
	//rev_xmit_protocol=malloc(SOCKET_MAX_DATA_LEN);
	rev_xmit_protocol = (xmit_protocol *)data;
	xmit_protocol *sdf_cmd_data=(xmit_protocol *)((unsigned char*)rev_xmit_protocol+sizeof(packet_head));
	//memcpy(rev_xmit_protocol,data,len);

	//printfS("len=%d\n",len);
	//printfS("rev_xmit_protocol->len=%d\n",rev_xmit_protocol->len);
	//PrintBuf((unsigned char *)rev_xmit_protocol,rev_xmit_protocol->len);
	if(rev_xmit_protocol->len!=len)
	{
		printf("rev_xmit_protocol->len!=len\n");
		//send(sockfd, rev_xmit_protocol, rev_xmit_protocol->len, 0);
		//printfS("send success\n");
		//free_point(rev_xmit_protocol);
		return;
	}
	switch(rev_xmit_protocol->sdf_cmd)
	{
	case SDF_OPEN_DEVICE:
		SDF_OpenDevice((packet_head **)&rev_xmit_protocol);
		break;
	case SDF_CLOSE_DEVICE:
        printf("close device\n");
		SDF_CloseDevice((packet_head **)&rev_xmit_protocol);
		break;
	case SDF_CREAT_SESSION:
		SDF_OpenSession((packet_head **)&rev_xmit_protocol);
		break;
	case SDF_CLOSE_SESSION:
		SDF_CloseSession((packet_head **)&rev_xmit_protocol);
		break;
	case SDF_SYM_ENC:
		SDF_Encrypt((packet_head **)&rev_xmit_protocol,(sdf_sym_server **)&sdf_cmd_data);
		break;
	case SDF_SYM_DEC:
		SDF_Decrypt((packet_head **)&rev_xmit_protocol,(sdf_sym_server **)&sdf_cmd_data);
		break;
	case SDF_HASH_INIT:
		SDF_HashInit((packet_head **)&rev_xmit_protocol);
		break;
	case SDF_HASH_UPDATE:
		SDF_HashUpdate((packet_head **)&rev_xmit_protocol,(sdf_sm3_server **)&sdf_cmd_data);
		break;
	case SDF_HASH_FINIAL:
		SDF_HashUpFinal((packet_head **)&rev_xmit_protocol,(sdf_sm3_server **)&sdf_cmd_data);
		break;
	case SDF_HASH_COPY_SESSION:
		SDF_Hash_Copy((packet_head **)&rev_xmit_protocol,(packet_head *)sdf_cmd_data);
		break;
	case SDF_ASYM_SIGN:
		pthread_mutex_lock(&thread_lock);
		SDF_InternalSign_ECC((packet_head **)&rev_xmit_protocol,(sdf_sm2_server **)&sdf_cmd_data);
		pthread_mutex_unlock(&thread_lock);
		break;
	case SDF_ASYM_EXTER_VERIFY:
		pthread_mutex_lock(&thread_lock);
		SDF_ExternalVerify_ECC((packet_head **)&rev_xmit_protocol,(sdf_extern_ecc_verify **)&sdf_cmd_data);
		pthread_mutex_unlock(&thread_lock);
		break;
	case SDF_GET_RNG:
		SDF_GenerateRandom((packet_head **)&rev_xmit_protocol,(sdf_get_rng **)&sdf_cmd_data);
		break;
	case SDF_GET_ASYM_KEYPAIR:
		pthread_mutex_lock(&thread_lock);
		SDF_GenerateKeyPair_ECC((packet_head **)&rev_xmit_protocol,(sdf_gen_asym_key **)&sdf_cmd_data);
		pthread_mutex_unlock(&thread_lock);
		break;
	case SDF_DELETE_ASYM_KEYPAIR:
		pthread_mutex_lock(&thread_lock);
		SDF_DeleteKeyPair_ECC((packet_head **)&rev_xmit_protocol,(sdf_gen_asym_key **)&sdf_cmd_data);
		pthread_mutex_unlock(&thread_lock);
		break;
	case SDF_GET_AGREEMENT_DATA_WITH_ECC:
		pthread_mutex_lock(&thread_lock);
		SDF_GenerateAgreementDataWithECC((packet_head **)&rev_xmit_protocol,(sdf_gen_agreement_par **)&sdf_cmd_data);
		pthread_mutex_unlock(&thread_lock);
		break;
	case SDF_GET_AGREEMENT_DATA_AND_KEY_WITH_ECC:
		pthread_mutex_lock(&thread_lock);
		SDF_GenerateAgreementDataAndKeyWithECC((packet_head **)&rev_xmit_protocol,(sdf_gen_agreement_par_and_key **)&sdf_cmd_data);
		pthread_mutex_unlock(&thread_lock);
		break;
	case SDF_GET_KEY_WITH_ECC:
		pthread_mutex_lock(&thread_lock);
		SDF_GenerateKeyWithECC((packet_head **)&rev_xmit_protocol,(sdf_count_session_key **)&sdf_cmd_data);
		pthread_mutex_unlock(&thread_lock);
		break;
	}
	int send_ready = 0;
	int send_len = send(sockfd, (unsigned char *)rev_xmit_protocol, rev_xmit_protocol->len, 0);
	if(send_len < rev_xmit_protocol->len)
	{
		if(send_len <= 0)
		{
			printf("send data error! ret = %d\n",send_len);
		}
		else
		{
			while(send_len < rev_xmit_protocol->len)
			{
				send_ready = send(sockfd, (unsigned char *)rev_xmit_protocol + send_len, rev_xmit_protocol->len, 0);
				if(send_ready <= 0)
				{
					printf("send data error! ret = %d\n",send_len);
					break;
				}
				send_len += send_ready;
			}
			
		}
		
		
	}
//分包发送
	// int send_ready = 0;
	// int packet_len = 0;
	// int offset = 0;
	// int total_len = rev_xmit_protocol->len;
	// while(offset < total_len)
	// {
	// 	if(total_len - offset >= MAX_PACKET_LEN)
	// 	{
	// 		packet_len = MAX_PACKET_LEN;
	// 	}
	// 	else
	// 	{
	// 		packet_len = total_len - offset;
	// 	}
	// 	send_ready = send(sockfd, (unsigned char *)rev_xmit_protocol + offset, packet_len, 0);
	// 	offset += send_ready;
	// }
	// if(rev_xmit_protocol->len != offset)
	// {
	// 	printf(" fd=%d,send->len=%d   error\n",sockfd,offset);
	// }
//先发送包头
	/*int semd_len=send(sockfd, (unsigned char *)rev_xmit_protocol, sizeof(packet_head), 0);
	if(sizeof(packet_head)!=semd_len)
	{
		printf("head fd=%d,send->len=%d   error\n",sockfd,semd_len);
	}
	int send_len=1024,offset=0;
	int len_send=rev_xmit_protocol->len-sizeof(packet_head);
	while(len_send>send_len)
	{
		semd_len=send(sockfd, (unsigned char *)rev_xmit_protocol+sizeof(packet_head)+offset,send_len, 0);
		if(send_len!=semd_len)
		{
			printf("mid fd=%d,send->len=%d   error\n",sockfd,semd_len);
		}
		offset+=semd_len;
		len_send-=send_len;
	}
	if(len_send>0)
	{
		semd_len=send(sockfd, (unsigned char *)rev_xmit_protocol+sizeof(packet_head)+offset,len_send, 0);
		if(len_send!=semd_len)
		{
			printf("last fd=%d,send->len=%d   error\n",sockfd,semd_len);
		}
	}*/
	printfS("fd=%d,send->len=%d\n",sockfd,send_len);
	//PrintBuf((unsigned char *)rev_xmit_protocol,rev_xmit_protocol->len);
	//printfS("send success\n");

	//free_point(rev_xmit_protocol);
}

void *sdf_rev(void *num)
{
	fd_set fdsr_test;
	int ret=0,max_socket_num=fd_A[(*(int *)num)];
	char buf[BUF_SIZE];
	FD_ZERO(&fdsr_test);
	FD_SET(sock_fd, &fdsr_test);  // add fd
	printfS("listion socket_num=%d\n",max_socket_num);
	while(1)
	{
		FD_SET(max_socket_num, &fdsr_test);  // add fd
		ret = select(max_socket_num+ 1, &fdsr_test, NULL, NULL, NULL);
		if (ret < 0) {          // error
			perror("select");
			break;
		} else if (ret == 0) {  // time out
			printfS("timeout\n");
			continue;
		}
		printfS("rev data\n");
		if (FD_ISSET(max_socket_num, &fdsr_test)) // check which fd is ready
		{
			ret = recv(max_socket_num, buf, sizeof(buf), 0);
			if (ret <= 0)
			{        // client close
				close(max_socket_num);
				FD_CLR(max_socket_num, &fdsr_test);  // delete fd
				for(int i=0;i<BACKLOG;i++)
				{
					if(fd_A[i]==max_socket_num)
					{
						fd_A[i]=0;
						printfS("close fd_A[%d]=%d\n",i,fd_A[i]);
						break;
					}
				}
				conn_amount--;
				pthread_exit(&thread_num[(*(int *)num)]);
			}
			else
			{
				printfS("fd_A=%d,rev_len=%d\n",max_socket_num,ret);
				PrintBuf((unsigned char *)buf,ret);
				sdf_rev_cmd(max_socket_num,buf,ret);
			}
		}
	}
	return NULL;
}

int socket_server_test(void)
{
	fd_set fdsr;
	int new_fd;             // listen on sock_fd, new connection on new_fd
	struct sockaddr_in server_addr;  // server address information
	struct sockaddr_in client_addr;  // connector's address information
	socklen_t sin_size;
	int yes = 1;
	int ret,i;

    pthread_mutex_init(&thread_lock, NULL);
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
	server_addr.sin_family = AF_INET;         // host byte order
	server_addr.sin_port = htons(MYPORT);     // short, network byte order
	server_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));
	if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		perror("bind");
		exit(1);
	}
	if (listen(sock_fd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	printfS("listen port %d\n", MYPORT);
	conn_amount = 0;
	sin_size = sizeof(client_addr);

	while (1)
	{
		FD_ZERO(&fdsr);
		FD_SET(sock_fd, &fdsr);  // add fd
		maxsock = sock_fd;

		ret = select(maxsock + 1, &fdsr, NULL, NULL, NULL);

		if (ret < 0) {          // error
			perror("select");
			break;
		} else if (ret == 0) {  // time out
			printfS("timeout\n");
			continue;
		}

		// check whether a new connection comes
		if (FD_ISSET(sock_fd, &fdsr))  // accept new connection
		{
			new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);
			if (new_fd <= 0)
			{
				perror("accept");
				continue;
			}
			// add to fd queue
			if (conn_amount < BACKLOG)
			{
				for(i=0;i<BACKLOG;i++)
				{
					if(fd_A[i]==0)
					{
						fd_A[i] = new_fd;
						break;
					}
				}
				printfS("new connection client[%d] %s:%d\n", i,
						inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
				if (new_fd > maxsock)  // update the maxsock fd for select function
					maxsock = new_fd;
				printfS("new client%d\n\n ",new_fd);

				//开启一个线程
				int err = pthread_create(&thread_num[i],NULL,sdf_rev,&fd_A[i]);
				if(err!=0)
				{
					printfS("thread_create Failed,new_fd=%d\n",new_fd);
				}
			}
			else
			{
				printfS("max connections arrive, exit\n");
				send(new_fd, "bye", 4, 0);
				close(new_fd);
				break;
			}
		}

	}
	exit(0);
}
