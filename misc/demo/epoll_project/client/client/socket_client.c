#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include"socket_client.h"


//#define COMMON_DEBUG_LOG
#ifdef COMMON_DEBUG_LOG
#define COMMON_DEBUG(fmt,arg...)        printf("[DEBUG %s:%s:%d]:"fmt,__FILE__,__func__,__LINE__,##arg)
#else 
#define COMMON_DEBUG(fmt,arg...)        
#endif
#define COMMON_ERROR(fmt,arg...)        printf("[ERR %s:%s:%d]:"fmt,__FILE__,__func__,__LINE__, ##arg)

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

	opts = opts | O_NONBLOCK | O_SYNC;
	if (fcntl(sockfd, F_SETFL, opts) < 0)
	{
		perror("fcntl(sock,SETFL,opts)");
		return;
	}//if
}


int create_socket()
{
    int res,socket_fd;
    struct addrinfo addr_info,*paddr_info;
    memset(&addr_info,0,sizeof(addr_info));
    addr_info.ai_family     = AF_INET;
    addr_info.ai_socktype   = SOCK_STREAM;

    //
    res = getaddrinfo("192.168.1.73","8087",&addr_info,&paddr_info);
    if (res == -1) {
        COMMON_ERROR("error : cannot get socket address!\n");
        return -1;
    }

    socket_fd = socket(paddr_info->ai_family,paddr_info->ai_socktype,paddr_info->ai_protocol);
    if (socket_fd == -1) {
        COMMON_ERROR("error : cannot get socket file descriptor!\n");
        return -1;
    }

    res = connect(socket_fd, paddr_info->ai_addr, paddr_info->ai_addrlen);
    if (res == -1) {
        COMMON_ERROR("error : cannot connect the server!\n");
        return -1;
    }
    //setNonblocking(socket_fd);
    return socket_fd;
}
int close_socket(int fd)
{
    if(fd != 0)
    {
        close(fd);
        return 0;
    }
    else
    {
        return -1;
    }  
}

int socket_init(void *pdev)
{
    device *pdev_handle = (device *)pdev;
    int i = 0,ret;
    for(i = 0;i<SOCKET_NUM;i++)
    {
        ret = create_socket();
        COMMON_DEBUG("create socket %d\n",ret);
        if(ret > 0)
        {
            pdev_handle->socketfd[i] = ret;
        }
        else
        {
            COMMON_ERROR("create socket err!\n");
            int j = 0;
            for(j=0;j<=i;j++)
            {
                close(pdev_handle->socketfd[j]);
            }
            return -1;
        }
        
    }
    return 0;
}

int socket_exit(void *pdev)
{
    device *pdev_handle = (device *)pdev;
    int i = 0;
    for(i = 0;i<SOCKET_NUM;i++)
    {
        if(pdev_handle->socketfd[i] != 0)
        {
            close(pdev_handle->socketfd[i]);
            pdev_handle->socketfd[i] = 0;
        }
            
    }
    COMMON_DEBUG("socket_exit!\n");
    return 0;
}


int socket_control_send(device *pdev_handle,unsigned int send_len)
{
    unsigned int read_len = 0;
    //memcpy(pdev_handle->control_send_buff,send_buff,send_len);
    if(send(pdev_handle->socketfd[0],pdev_handle->control_send_buff,send_len,0) == -1)
    {
        COMMON_ERROR("send control data error\n");
        return -1;
    }

#ifdef COMMON_DEBUG_LOG
    int i = 0;
    COMMON_DEBUG("socket control send %d\n",pdev_handle->socketfd[0]);
    for(i=0;i<send_len;i++)
    {
        printf("%02x ",pdev_handle->control_send_buff[i]);
    }
    printf("\n");
#endif


    if((read_len = read(pdev_handle->socketfd[0],pdev_handle->control_recv_buff,MAX_SEND_BUFF_LEN)) > 0)
    {

#ifdef COMMON_DEBUG_LOG
    int i = 0;
    COMMON_DEBUG("socket control recv %d\n",pdev_handle->socketfd[0]);
    for(i=0;i<read_len;i++)
    {
        printf("%02x ",pdev_handle->control_recv_buff[i]);
    }
    printf("\n");
#endif
        return read_len;
    }

    COMMON_DEBUG("read data error\n");
    return -1;
}

int socket_send(session_list *psession_handle,unsigned int send_len)
{
    int read_len = 0;
    unsigned int read_ready = 0;
    packet_head *packet;
    int socket_fd = ((device *)(psession_handle->sess.pdev_handle))->socketfd[psession_handle->sess.socket_fd];
    int send_ready = 0;
    unsigned int offset = 0;
    int packet_len = 0;
    while(send_len > 0)
    {
        send_ready = send(socket_fd,psession_handle->sess.send_buff + offset,send_len,0);
        send_len -= send_ready;
        offset += send_ready;
        //printf("send_total_len:%d,send_len:%d,packet_len:%d\n",send_len,offset,packet_len);
        if(send_ready == -1)
        {
            COMMON_ERROR("send data error\n");
            return -1;
        }
    }
#ifdef COMMON_DEBUG_LOG
    int i = 0;
    COMMON_DEBUG(">>>>>>>>>socket send_fd: %d , send_len: %d\n",socket_fd,offset);
    for(i=0;i<offset;i++)
    {
        printf("%02x ",psession_handle->sess.send_buff[i]);
    }
    printf("\n");
#endif
    read_len = 0;
    read_len = read(socket_fd,psession_handle->sess.recv_buff,sizeof(packet_head));
    if(read_len <= 0)
    {
        COMMON_ERROR("read len error! read_len = %d\n",read_len);
        return -1;
    }
    while(read_len < sizeof(packet_head))
    {
        read_ready = read(socket_fd,psession_handle->sess.recv_buff + read_len,MAX_SEND_BUFF_LEN);
        read_len += read_ready;
        if(read_ready <= 0)
        {
            COMMON_ERROR("read len error! read_len = %d\n",read_ready);
            return -1;
        }
    }
    packet = (packet_head *)(psession_handle->sess.recv_buff);
    if(packet->len < sizeof(packet_head) || packet->len > MAX_SEND_BUFF_LEN)
    {
        COMMON_ERROR("read len error! packet->len = %d\n",packet->len);
        return -1;
    }
    int read_len_first = read_len;
    if(packet->len > read_len)
    {
        read_len = packet->len - read_len;
        offset = read_len_first;
        while(read_len > 0)
        {
            read_ready = read(socket_fd,psession_handle->sess.recv_buff + offset,MAX_SEND_BUFF_LEN);
            offset += read_ready;
            read_len -= read_ready;
            if(read_ready <= 0)
            {
                COMMON_ERROR("read len error! read_len = %d\n",read_ready);
                return -1;
            }
        }
    }   
#ifdef COMMON_DEBUG_LOG
    COMMON_DEBUG("socket recv_fd %d , recv_len: %d\n",socket_fd,offset);
    for(i=0;i<offset;i++)
    {
        printf("%02x ",psession_handle->sess.recv_buff[i]);
    }
    printf("\n");
#endif
    return packet->len;
    
}
