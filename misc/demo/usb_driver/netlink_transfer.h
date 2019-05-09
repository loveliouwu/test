
#ifndef _NETLINK_TRANSFER_H_
#define _NETLINK_TRANSFER_H_

#include<linux/types.h>
#include<linux/netdevice.h>


#define NETLINK_TEST (25)	//与内核通信的协议编号






void log_print(const unsigned char *buff, unsigned int len);

int netlink_send(unsigned char *pbuff, unsigned int length, unsigned int user_pid);


int netlink_init(unsigned int unit);


void netlink_exit(void);

#endif
