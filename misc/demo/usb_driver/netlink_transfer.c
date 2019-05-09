#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/string.h>

#include "netlink_transfer.h"
#include "cf_log.h"

#define NL_ERROR(fmt, arg...)	printk("[NL_ERROR: func: %s line: %d]"fmt, __func__, __LINE__, ##arg);

#define NL_INFO(fmt, arg...)	printk("[NL_INFO: func: %s line: %d]"fmt,__func__,__LINE__,##arg);


#define MAX_MSGSIZE 1024

struct sock *nl_sk = NULL;




int netlink_send(unsigned char *pbuff, unsigned int length, unsigned int user_pid)
{
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	int len = NLMSG_SPACE(length);
	int ret = -1;
	if (!pbuff || !nl_sk) {
		NL_ERROR("err: pbuff or nl_sk is NULL.\n");
		return -1;
	}
	//printk(KERN_INFO "pid:%d\n", pid);
	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb) {
	    NL_ERROR("nel_link:alloc_skb error\n");
	    return -1;
	}
	nlh = nlmsg_put(skb, 0, 0, NETLINK_TEST, (len-sizeof(struct nlmsghdr)), 0);

	memcpy(NLMSG_DATA(nlh), pbuff, length);
	log_print((const unsigned char *)pbuff, length);//打印发送的信息
	nlh->nlmsg_len = NLMSG_SPACE(length);

	printk(KERN_INFO "net_link:send message '%s'.\n",(char *)NLMSG_DATA(nlh));
	ret = netlink_unicast(nl_sk, skb, user_pid, MSG_DONTWAIT);
	if(ret < 0)
	{
		NL_ERROR("can not unicast skb (%d)\n", ret);
	}
	return ret;

}


static void netlink_recv(struct sk_buff *__skb)
{
	struct nlmsghdr *nlh = NULL;
	struct sk_buff *skb = NULL;
	skb = skb_get(__skb);
	if(skb->len >= NLMSG_SPACE(0))
	{
		nlh = nlmsg_hdr(skb);
	}
	else
	{
		NL_ERROR("netlink_rec error!\n");
		kfree_skb(skb);
		return ;
	}
	
	if(nlh->nlmsg_len < sizeof(*nlh) || skb->len < nlh->nlmsg_len)
	{
		NL_ERROR("netlink_recv length error!\n");
		return ;
	}
	/*接收到数据后分析并通过USB发送*/
	//netlink_recv_manage(char * data_addr,unsigned int nlmsg_pid);
	kfree_skb(skb);

}


struct netlink_kernel_cfg cfg = {
	.input = netlink_recv,
};

int netlink_init(unsigned int unit)
{
	int ret = -1;
	//nl_sk = netlink_kernel_create(&init_net, unit, 0, netlink_recv, NULL, THIS_MODULE);
	nl_sk = netlink_kernel_create(&init_net, unit, &cfg);
	if(!nl_sk)
	{
		NL_ERROR("net_link: cannot create netlink socket.\n");
		ret = -EIO; //操作错误
	}
	else
		ret = 0;
	return ret;
}



void netlink_exit(void)
{
	if(nl_sk)
	{
		netlink_kernel_release(nl_sk);
		nl_sk = NULL;
	}
}


































































































