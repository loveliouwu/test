#ifndef SOCKET_CLIENT_H_
#define SOCKET_CLIENT_H_

#include "SDF_Interface.h"
#include "sdf_common.h"
int create_socket();//返回创建的socket_fd，错误返回-1
int socket_init(void *pdev);
int socket_exit(void *pdev);
int socket_control_send(device *pdev_handle,unsigned int send_len);
int socket_send(session_list *psession_handle,unsigned int send_len);
static void setNonblocking(int sockfd);


#endif