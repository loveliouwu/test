#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/usb.h>
#include<linux/mod_devicetable.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/string.h>
#include <linux/kref.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <linux/ioctl.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/semaphore.h>
#include <linux/kdev_t.h>

//#include"pe100_usb_driver.h"
#include"netlink_transfer.h"
//#include"cf_log.h"
#include"lyp.h"


#define DRIVER_INFO(fmt, arg...)	printk("[DR_INFO: func: %s line: %d]"fmt,__func__,__LINE__,##arg);
#define DRIVER_ERROR(fmt, arg...)	printk("[DR_ERROR: func: %s line: %d]"fmt, __func__, __LINE__,##arg);


#define MIN(a,b) ((a <= b ) ? a : b)


/*字符设备*/
#define MAX_SSISION_COUNT	(8)	//最大回话数量
#define CF_DRIVER_MAJOR		(0)
#define MEMDEV_NUM		(1)
#define CF_IOCTL_MAGIC_W	'W'
#define CF_IOCTL_MAGIC_R	'R'
#define CF_IOCTL_MAGIC_WR	'G'
#define CMD_GET_SID		_IOW(CF_IOCTL_MAGIC_WR, 0, unsigned long)
#define CMD_RELEASE_SID		_IOW(CF_IOCTL_MAGIC_WR, 1, unsigned long)
#define CMD_GET_KERNEL_INFO	_IOW(CF_IOCTL_MAGIC_WR, 2, unsigned long)
char cdev_driver_name[] = "cf_dev";
static struct cdev g_cdev;
static struct class *g_dev_class;
static struct device *g_dev_device;
static int g_mem_major = CF_DRIVER_MAJOR;


/*会话标记*/

static spinlock_t open_spinlock; 
struct semaphore ioctl_sem;	//用于互斥分配会话编号
unsigned int g_open_count = 0;
unsigned char g_ssision_id_flag[MAX_SSISION_COUNT];
unsigned int s_id_value = 0;
g_ssision_struct g_ssision_id_str[MAX_SSISION_COUNT];




unsigned int recv_len = MAX_MSGSIZE;
struct usb_host_interface *iface_desc = NULL;
struct usb_endpoint_descriptor *endpoint = NULL;



//unsigned int cf_suspend_flag = FALSE;//睡眠唤醒标记
struct usb_skel *dev;    


/*向应用层发送处理数据的异常状态信息
data_info[in]: 	数据信息
status[in]:	异常状态类型
flag[in]:	标记异常状态来源 TASK_STATUS   NETLINK_STATUS
返回值：成功返回发送长度，失败返回-1
*/
int send_netlink_status(nl_packet_st *data_info, unsigned int status, unsigned int flag)
{
	int ret = -1;
	usb_packet_st *pheader = NULL;
	if(NULL == data_info)
	{
		return ret;
	}

	data_info->flag = SEND_DATA_FLAG;
	data_info->task_status = status;

	if(flag == TASK_STATUS)
	{
		pheader = (usb_packet_st *)(data_info + 1);
		pheader->packet_status = status;
		ret = netlink_send((unsigned char *)data_info, sizeof(nl_packet_st) + sizeof(usb_packet_st), data_info->task_pid);
	}
	else
	{
		ret = netlink_send((unsigned char *)data_info, sizeof(nl_packet_st), data_info->task_pid);
	}

	
	return ret;
}


/*检查配置启动、停止*/
int card_configuration_check(nl_packet_st * task_info, unsigned int *config_flag)
{
	int ret = SDR_OK;
	card_head *pheader = (card_head *)(task_info +1);//跳过task_info部分
	if(task_info->task_cmd != CARD_CONFIG)
	{
		if(*config_flag == CONFIG_START)
		{
			send_netlink_status(task_info, SDR_BEINGCONFIGURED, TASK_STATUS);
			ret = SDR_BEINGCONFIGURED;
		}
	}
	else
	{
		switch(pheader->packet_cmd)
		{
			case CARD_CONFIGURATION_START:
				DRIVER_INFO("Config start!\n");
				if(*config_flag == CONFIG_START)
				{
					send_netlink_status(task_info, SDR_OK, TASK_STATUS);
					ret = SDR_BEINGCONFIGURED;
				}
				else
				{
					*config_flag = CONFIG_START;
					ret = -1;
				}
				DRIVER_INFO("card_config_start flag:%d\n",card_config_start_flag);
				break;
			case CARD_CONFIGURATION_STOP:
				DRIVER_INFO("Config stop!\n");
				*config_flag = CONFIG_STOP;
				send_netlink_status(task_info, SDR_OK, TASK_STATUS);
				DRIVER_INFO("card_config_start flag:%d\n",card_config_start_flag);
				ret = 1;
				break;
			default:
				break;
		}
	}
	return ret;
}



/*NetLink回调函数中对数据解析封USB包
收到的包组成：

*/
int netlink_recv_manage(char *data_addr, unsigned int nlmsg_pid)
{
	unsigned int recv_data_len = 0;
	nl_packet_st * nl_data_header = NULL;
	usb_packet_st * usb_data_header = NULL;


	int ret = 0;
	recv_data_len = ((nl_packet_st *)data_addr)->data_len;//当前包数据长度
	nl_data_header = kmalloc(recv_data_len + sizeof(nl_packet_st), GFP_KERNEL);	//存放netlink收到的数据

	if(nl_data_header == NULL)
	{
		DRIVER_ERROR("Malloc nl_data_header failed..\n");
		return -1;
	}
	else
	{
		memset(nl_data_header, 0, recv_data_len + sizeof(nl_packet_st));
		nl_data_header->task_pid = nlmsg_pid;
		memcpy(nl_data_header, data_addr, recv_data_len + sizeof(nl_packet_st));//复制包数据到申请的空间
		DRIVER_INFO("recv netlink data ssid:%d, cmd:%d\n",nlmsg_pid, nl_data_header->task_cmd);
	}

	ret = card_configuration_check(nl_data_header, &card_config_start_flag);//检查是否处于配置状态
	if(ret != SDR_OK)	
	{
		if(ret > 0)
		{
			kfree(nl_data_header);
			nl_data_header = NULL;
		}
		return -1;
	}
	
	switch(nl_data_header->task_type)//不同的命令封包大小和操作不同   注意申请和释放空间
	{
		case TYPE_CPU://CPU任务Netlink封包格式为  【nl_packet_st】【card_head】【data】
				//card_head部分是原pe200中IC卡用到的数据，现在保留不用
			usb_data_header = kmalloc(recv_data_len + sizeof(usb_packet_st) - sizeof(card_head), GFP_KERNEL);//去掉card_head
			if(usb_data_header == NULL)
			{
				DRIVER_ERROR("Malloc usb_data_header failed..\n");
				kfree(nl_data_header);
				nl_data_header = NULL;
				return -1;
			}
			memset(usb_data_header, 0, recv_data_len + sizeof(usb_packet_st) - sizeof(card_head));
			memcpy(usb_data_header + sizeof(usb_packet_st), nl_data_header + sizeof(nl_packet_st) + sizeof(card_head), recv_data_len);
			usb_data_header->packet_len = nl_data_header->data_len - sizeof(card_head);	//驱动去掉了一个card_head部分，需要调整发送长度
			usb_data_header->packet_cmd = nl_data_header->task_cmd;	//执行具体命令
			usb_data_header->packet_status = SDR_OK;		//执行的状态
			break;

		case TYPE_SYM://sym计算  encrypt decrypt calculate_mac 三个接口用到  涉及到大包，需要请求数据
			/*
				大包第一包时：recv_data_len = 【symalg_cipher】【data】 //data在SDF中限定为8k
				大包后续包时：recv_data_len = 8k或者剩余字节
				大于（8*1024-包头）字节的数据包为大包，需要多次传输，小包的info->flag为 TASK_INFO_FLAG
				如果是大包，应用需要根据接收的任务头中 info->flag == REQUEST_INFO_FLAG 来进行下一个包的传输
				所以驱动要对info->flag进行填充，
				netlink收到的数据包格式为：【nl_packet_st】【symalg_cipher】【data】
			*/

			if(((symalg_cipher *)(nl_data_header + sizeof(nl_packet_st)))->data_len <= MAX_SEND_DATA_LEN)//任务长度小于最大包长度（8k）则为小包
			{
				usb_data_header = kmalloc(recv_data_len + sizeof(usb_packet_st), GFP_KERNEL);//包头：【usb_packet_st】数据部分：【symalg_cipher】【data】
				if(usb_data_header == NULL)
				{
					DRIVER_ERROR("Malloc usb_data_header failed..\n");
					kfree(nl_data_header);
					nl_data_header = NULL;
					return -1;
				}
				usb_data_header->packet_cmd = nl_data_header->task_cmd;
				usb_data_header->packet_len = nl_data_header->data_len;
				usb_data_header->packet_status = nl_data_header->task_status;
				memcpy(usb_data_header + sizeof(usb_packet_st), nl_data_header + sizeof(nl_packet_st), recv_data_len);//复制数据部分
			}
			else//大包
			{
				if(g_ssision_id_str[nlmsg_pid - 1].sym_flag == 0)//标记，该会话开始进行sym大包传输时设为1
				{//发送的第一包：需要发送对称加解密数据 这里的recv_data_len = 【symalg_cipher】【data】
					g_ssision_id_str[nlmsg_pid - 1].sym_data_len = (symalg_cipher *)(nl_data_header + sizeof(nl_packet_st))->data_len;//指向对称加解密数据头结构体，取出任务的最大包长度
					g_ssision_id_str[nlmsg_pid - 1].sym_flag = 1;
					usb_data_header = kmalloc(recv_data_len + sizeof(usb_packet_st), GFP_KERNEL);//【usb_packet_st】【symalg_cipher】【data】第一包需要发送symalg_cipher
					if(usb_data_header == NULL)
					{
						DRIVER_ERROR("Malloc usb_data_header failed..\n");
						kfree(nl_data_header);
						nl_data_header = NULL;
						g_ssision_id_str[nlmsg_pid - 1].sym_data_len = 0;
						g_ssision_id_str[nlmsg_pid - 1].sym_flag = 0;
						return -1;
					}
					usb_data_header->packet_cmd = nl_data_header->task_cmd;
					usb_data_header->packet_len = nl_data_header->data_len;
					usb_data_header->packet_status = nl_data_header->task_status;
					memcpy(usb_data_header + sizeof(usb_packet_st), nl_data_header + sizeof(nl_packet_st), recv_data_len);//复制数据部分 
					g_ssision_id_str[nlmsg_pid - 1].sym_offset += recv_data_len - sizeof(symalg_cipher);//记录发送的数据长度
				}
				else
				{//发送后续包，只用发送数据 这里的recv_data_len = 【data】
					usb_data_header = kmalloc(recv_data_len + sizeof(usb_packet_st), GFP_KERNEL);//
					if(usb_data_header == NULL)
					{
						DRIVER_ERROR("Malloc usb_data_header failed..\n");
						kfree(nl_data_header);
						nl_data_header = NULL;
						g_ssision_id_str[nlmsg_pid - 1].sym_data_len = 0;
						g_ssision_id_str[nlmsg_pid - 1].sym_flag = 0;
						return -1;
					}
					g_ssision_id_str[nlmsg_pid - 1].sym_offset += recv_data_len;//更新已发送的数据长度
					usb_data_header->packet_cmd = nl_data_header->task_cmd;
					usb_data_header->packet_len = nl_data_header->data_len;
					usb_data_header->packet_status = nl_data_header->task_status;
					memcpy(usb_data_header + sizeof(usb_packet_st), nl_data_header + sizeof(nl_packet_st), recv_data_len);//复制数据部分
				}
			}
			break;
		case TYPE_ASYM: //非对称包组成【nl_packet_st】【data】   recv_data_len = 【data】  注：非对称任务没有大包
			usb_data_header = kmalloc(recv_data_len + sizeof(usb_packet_st), GFP_KERNEL);
			if(usb_data_header == NULL)
			{
				DRIVER_ERROR("Malloc usb_data_header failed..\n");
				kfree(nl_data_header);
				nl_data_header = NULL;
				return -1;
			}
			memset(usb_data_header, 0, recv_data_len + sizeof(usb_packet_st));
			memcpy(usb_data_header + sizeof(usb_packet_st), nl_data_header + sizeof(nl_packet_st), recv_data_len);
			usb_data_header->packet_len = nl_data_header->data_len;	//USB包数据部分的大小
			usb_data_header->packet_cmd = nl_data_header->task_cmd;	//执行具体命令
			usb_data_header->packet_status = SDR_OK;		//执行的状态
			break;

		case TYPE_HASH://包含init update 和 final 
			/*
				init阶段会收到一个74字节的中间数据  需要保存用于update阶段使用
				update阶段发送数据需要带上init阶段的74字节中间数据
			*/
			break;
		/* 管理软件发送的包都是cpu任务
		case CARD_CONFIG:
			break;
		*/
		default:
			
			break;
	}	
	
	//封包之后调用usb发送等待接收
	ret = usb_data_send((unsigned char*)usb_data_header, recv_data_len + sizeof(usb_packet_st));
	if(ret != 0)
	{
		DRIVER_ERROR("send usb data ERROR\n");
		send_netlink_status((nl_packet_st *)data_addr, SDR_OK, NETLINK_STATUS);
		kfree(usb_data_header);
	}
	ret = usb_data_recv((unsigned char*)usb_data_header, recv_data_len + sizeof(usb_packet_st));
	if(ret != 0)
	{
		DRIVER_ERROR("recv usb data ERROR\n");
		send_netlink_status((nl_packet_st *)data_addr, SDR_OK, NETLINK_STATUS);
		kfree(usb_data_header);
	}

	nl_data_header->task_cmd = usb_data_header->packet_cmd;
	nl_data_header->task_status = usb_data_header->packet_status;
	nl_data_header->data_len = usb_data_header->packet_len;

	//复制接收到的数据到Netlink buffer   nl_data_header
	switch(nl_data_header->task_type)
	{
		case TYPE_CPU://CPU任务Netlink封包格式为  【nl_packet_st】【card_head】【data】
				//card_head部分是原pe200中IC卡用到的数据，发送时需要填充
			memcpy(nl_data_header + sizeof(nl_packet_st) + sizeof(card_head), usb_data_header + sizeof(usb_packet_st), usb_data_header->data_len);//填充数据
			//填充card_head部分以适配SDF接口
			((card_head *)(nl_data_header + sizeof(nl_packet_st)))->status = usb_data_header->packet_status;
			((card_head *)(nl_data_header + sizeof(nl_packet_st)))->packet_len = usb_data_header->packet_len + sizeof(card_head);//pcard_head的packet_len为数据部分长度
			nl_data_header->data_len = usb_data_header->packet_len + sizeof(card_head);//驱动添加了一个pcard_head，需要补足长度
			break;

		case TYPE_SYM:
			nl_data_header->task_cmd = usb_data_header->packet_cmd;
			nl_data_header->data_len = usb_data_header->packet_len;
			nl_data_header->task_status = usb_data_header->packet_status;
			memcpy(nl_data_header + sizeof(nl_packet_st), usb_data_header + sizeof(usb_packet_st), usb_data_header->packet_len);
			//大包最后一包时：packet_len = 【symalg_cipher】【data】
			//大包中间包时：packet_len = 【data】
			//小包：packet_len = 【symalg_cipher】【data】
			if(g_ssision_id_str[nlmsg_pid - 1].sym_flag == 1)//标记，该会话开始进行sym大包传输时设为1
			{
				if(g_ssision_id_str[nlmsg_pid - 1].sym_offset >= g_ssision_id_str[nlmsg_pid - 1].sym_data_len)//如果发送结束，则清除任务标记，这时返回的数据为最终的结果
				{
					g_ssision_id_str[nlmsg_pid - 1].sym_data_len = 0;
					g_ssision_id_str[nlmsg_pid - 1].sym_flag = 0;
					nl_data_header->flag = SEND_DATA_FLAG;//返回处理完的数据
				}
				else
				{
					nl_data_header->flag = REQ_DATA_FLAG;//设置请求数据
				}
			}
			break;

		case TYPE_ASYM:
			nl_data_header->task_cmd = usb_data_header->packet_cmd;
			nl_data_header->data_len = usb_data_header->packet_len;
			nl_data_header->task_status = usb_data_header->packet_status;
			memcpy(nl_data_header + sizeof(nl_packet_st), usb_data_header + sizeof(usb_packet_st), usb_data_header->packet_len);
			break;

		case TYPE_HASH:

			break;

		case default:
			break;
	}
	
	ret = netlink_send((unsigned char *)nl_data_header, sizeof(nl_packet_st) + nl_data_header->data_len, nl_data_header->task_pid);
	



	return 0;
}


/*usb 发送数据接口,最大一次发送512字节*/
static unsigned int  usb_data_send(unsigned char * pbuff,unsigned int length)
{
	int retval = -1;
	unsigned int datalen = MIN(length,MAX_BUFFER_SIZE); //如果大于8.5k则只传输8.5k
	unsigned int offset = 0;
	unsigned int sendlen = 0;

	if(!pbuff)
	{
		DRIVER_ERROR("send buff is NULL !\n");
		return -1;
	}
	DRIVER_INFO("start usb_bulk_msg send data\n");

	memcpy(dev->bulk_out_buffer, pbuff, datalen);  //复制发送的数据到usb发送buffer

	//if(copy_from_user(dev->bulk_out_buffer,pbuff,datalen))//不涉及用户空间指针  无需转换
	if(datalen > MAX_MSGSIZE)
	{
		offset = 0;
		sendlen = MAX_MSGSIZE;
		while(datalen>0)
		{
			retval = usb_bulk_msg(
					dev->udev,
					usb_sndbulkpipe(
						dev->udev,
						dev->bulk_out_endpointAddr
						),
					dev->bulk_out_buffer + offset,//一次把数据全部拷贝进内核空间 发送时添加偏移
					sendlen,
					&recv_len,
					2000			
				);
			if(retval == 0)
			{
				datalen -= sendlen;
				offset += sendlen;
				if(datalen >= MAX_MSGSIZE)
				{
					sendlen = MAX_MSGSIZE;
				}
				else
				{
					sendlen = datalen;					
				}
				
				DRIVER_INFO("usb_bulk_msg send_data ok !\n");
				DRIVER_INFO("send_data = %d\n",dev->bulk_out_buffer[offset-1]);
			}
			else
			{
				DRIVER_ERROR("usb_bulk_msg send_data err ! ret = %d\n",retval);
				return retval;
			}
		}

	}
	else
	{
		retval = usb_bulk_msg(
			dev->udev,
			usb_sndbulkpipe(
				dev->udev,
				dev->bulk_out_endpointAddr
				),
			dev->bulk_out_buffer,
			datalen,
			&recv_len,
			2000			
			);
		if(retval == 0)
		{
			DRIVER_INFO("usb_bulk_msg send_data ok !\n");
			DRIVER_INFO("send_data = %d\n",dev->bulk_out_buffer[1]);
		}
		else
		{
			DRIVER_ERROR("usb_bulk_msg send_data err ! ret = %d\n",retval);
			return retval;
		}
	}

	return recv_len;
	
	
}

/*USB 接收数据接口*/
static unsigned int usb_data_recv(unsigned char * pbuff,unsigned int length)
{
	int retval = -1;
	unsigned int recv_datalen = MIN(length,MAX_BUFFER_SIZE);//总的接收长度
	unsigned int offset = 0;//接收时放入buffer的偏移
	unsigned int recvlen = 0;//每次发送的长度
	if(!pbuff)
	{
		DRIVER_ERROR("recv buff is NULL !\n");
		return -1;
	}
	
	if(recv_datalen > MAX_MSGSIZE)
	{
		recvlen = MAX_MSGSIZE;
		while(recv_datalen > 0)
		{
			retval = usb_bulk_msg(
				dev->udev,
				usb_rcvbulkpipe(
					dev->udev,
					dev->bulk_in_endpointAddr
					),
				dev->bulk_in_buffer + offset,  //接收时候也一次放入内核空间，然后统一取出数据
				recvlen,
				&recv_len,
				2000			
				);
			if(!retval)
			{
				//if (copy_to_user(pbuff+offset, dev->bulk_in_buffer+offset, recvlen))
				recv_datalen -= recv_len;
				offset += recv_len;//实际接收的数据长度
				if(recv_datalen > MAX_MSGSIZE)
				{
					recv_len = MAX_MSGSIZE;
				}
				else
				{
					recv_len = recv_datalen;
				}
				DRIVER_INFO("==========recv_data ok !\n");
				DRIVER_INFO("==========recv len = %d\n",offset);
				DRIVER_INFO("==========recv_data = %d\n",dev->bulk_in_buffer[offset-1]);
					
			}
			else
			{
				DRIVER_ERROR("usb_bulk_msg recv data err !\n");
				return retval;
			}
		}
	}
	else
	{
		retval = usb_bulk_msg(
			dev->udev,
			usb_rcvbulkpipe(
				dev->udev,
				dev->bulk_in_endpointAddr
				),
			dev->bulk_in_buffer,
			recv_datalen,
			&recv_len,
			2000			
			);
		if(!retval)
		{
			if (copy_to_user(pbuff, dev->bulk_in_buffer,recv_len))
			{
				DRIVER_INFO("==========send_data ok !\n");
				DRIVER_INFO("==========recv len = %d\n",recv_len);
				DRIVER_INFO("==========recv_data = %d\n",pbuff[1]);
			}
			else
			{
				retval = -1;
				DRIVER_ERROR("data copy_to_user err !\n");
				return retval;
			}
		        
		}
		else
		{
			DRIVER_ERROR("usb_bulk_msg recv data err !\n");
			return retval;
		}

	}
	memcpy(pbuff, dev->bulk_in_buffer, MIN(length,MAX_BUFFER_SIZE));  //从接收buffer中取出数据
	
	return 0;
}


/*匹配设备，枚举端点，申请发送接受数据空间*/
int usb_plug_device(struct usb_interface *interface, const struct usb_device_id *id)
{

	int i = 0;

	DRIVER_INFO("USB (%04X:%04X) plugged\n", id->idVendor, id->idProduct);//打印出枚举到的设备PID VID

	
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);//申请空间
	if(!dev)
	{
		DRIVER_ERROR("Could not allocate dev\n");
		return -1;
	}
	dev->udev = usb_get_dev(interface_to_usbdev(interface));
	dev->interface = interface;
	iface_desc = interface->cur_altsetting;

	DRIVER_INFO("+++++Pen i/f %d now probed: (%04X:%04X)\n",iface_desc->desc.bInterfaceNumber, id->idVendor, id->idProduct);
	DRIVER_INFO("+++++ID->bNumEndpoints: %02X\n",iface_desc->desc.bNumEndpoints);
	DRIVER_INFO("+++++ID->bInterfaceClass: %02X\n",iface_desc->desc.bInterfaceClass);

	
	if(iface_desc!=NULL)
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i)
	{

		endpoint = &iface_desc->endpoint[i].desc;
		if(endpoint!=NULL)
		if (!dev->bulk_in_endpointAddr &&
						(endpoint->bEndpointAddress & USB_DIR_IN) &&
						((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
						== USB_ENDPOINT_XFER_BULK)) 
		{
			DRIVER_INFO("++++++MaxPacketSize = %d\n",endpoint->wMaxPacketSize);
			dev->bulk_data_size = MAX_MSGSIZE;//设置发送最大长度
			dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
			DRIVER_INFO("++++++bulk_in_endpointaddr = %02x\n",dev->bulk_in_endpointAddr);

			
			/*
			dev->bulk_in_buffer = kmalloc(MAX_BUFFER_SIZE, GFP_KERNEL);//申请接收数据空间 8.5K
			if (!dev->bulk_in_buffer) 
			{
					DRIVER_ERROR("Could not allocate bulk_in_buffer\n");
					return 1;
			}
			else
					DRIVER_INFO("++++++allocate bulk_in_buffer\n");
			*/
			
		}

		if (!dev->bulk_out_endpointAddr &&
						!(endpoint->bEndpointAddress & USB_DIR_IN) &&
						((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
						== USB_ENDPOINT_XFER_BULK)) 
		{ 
			dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
			DRIVER_INFO("++++++bulk_out_endpointaddr = %02x\n",dev->bulk_in_endpointAddr);
			/*
			dev->bulk_out_buffer = kmalloc(MAX_BUFFER_SIZE, GFP_KERNEL);//申请发送数据空间 8.5K
			if (!dev->bulk_out_buffer) 
			{
					DRIVER_ERROR("Could not allocate bulk_out_buffer\n");
					return 1;
			}
			else
					DRIVER_INFO("++++++allocate bulk_out_buffer\n");
			*/
			
		}
	}
	if (!(dev->bulk_in_endpointAddr && dev->bulk_out_endpointAddr))
	{

		DRIVER_ERROR("Could not find both bulk-in and bulk-out endpoints");
		return 1;
	}
	return 0;
}

long usb_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned int trans_id = 0;
	unsigned int loop_value = 0;
	//	kernel_info info;
	int i = 0;
	//int j = 0;
	int ret = SDR_UNKNOWERR;
	if(down_interruptible(&ioctl_sem))
	{
		return ret;
	}
	DRIVER_INFO("ioctl cmd:%d\n",cmd);
	switch(cmd)
	{
		case CMD_GET_SID:
			if(copy_from_user((unsigned char *)&trans_id, (char*)arg, sizeof(unsigned int)))
			{
				DRIVER_ERROR("Failed copy from user. \n");
				ret = SDR_OPENSSISION;
				break;
			}
			for(i = 0; i < MAX_SSISION_COUNT; i++)
			{
				loop_value = (s_id_value + i)%MAX_SSISION_COUNT;
				if(g_ssision_id_str[loop_value].ssision_id_flag == 0)
				{
					g_ssision_id_str[loop_value].sym_flag = 0;//初始化该会话的sym全局变量
					g_ssision_id_str[loop_value].sym_offset = 0;
					g_ssision_id_str[loop_value].sym_data_len = 0;

					g_ssision_id_str[loop_value].ssision_id_flag = 1;
					g_ssision_id_str[loop_value].ssision_id = loop_value + 1;
					trans_id = loop_value + 1;
					filp->private_data = (void*)(&g_ssision_id_str[loop_value]);
					s_id_value = (s_id_value + 1)%MAX_SSISION_COUNT;
					break;
				}
			}
			if(i == MAX_SSISION_COUNT)
			{
				ret = SDR_OPENSSISION;
				DRIVER_ERROR("loop too much.\n\n");
				break;
			}
			if(copy_to_user((char *)arg, (unsigned char *)&trans_id, sizeof(unsigned int)))
			{
				DRIVER_ERROR("Failed copy from user.\n\n");
				ret = SDR_OPENSSISION;
				break;
			}
			DRIVER_INFO("create ss_sid:%d\n",trans_id);
			ret = SDR_OK;
			break;


		case CMD_RELEASE_SID:
			if(copy_from_user(&trans_id, (unsigned char *)arg, sizeof(unsigned int)))
			{
				DRIVER_ERROR("Failed copy from user.\n\n");
				ret = SDR_UNKNOWERR;
				break;
			}
			ret = SDR_OK;
			break;
		case CMD_GET_KERNEL_INFO:
			DRIVER_INFO("Get kernel info\n");
			break;
		default:
			DRIVER_INFO("command error !!\n");
			break;
		
	}
	up(&ioctl_sem);//释放信号量
	return ret;
}

/*字符设备打开时资源申请*/
int usb_open(struct inode* inode, struct file* filp)
{
	DRIVER_INFO("device open!\n");
	spin_lock(&open_spinlock);
	g_open_count += 1;
	spin_lock(&open_spinlock);
	return 0;
}

/*字符设备关闭时的资源释放*/
int usb_release(struct inode * inode, struct file * filp)
{
	g_ssision_struct *ss_str = NULL;
	DRIVER_INFO("device release! \n");
	spin_lock(&open_spinlock);
	g_open_count -= 1;
	spin_unlock(&open_spinlock);
	if(filp->private_data != NULL)
	{
		if(down_interruptible(&ioctl_sem))
		{
			return -1;
		}
		ss_str = (g_ssision_struct *)(filp->private_data);
		if(ss_str->ssision_id_flag == 1)
		{
			DRIVER_INFO("ss_id:%d\n", ss_str->ssision_id);
			ss_str->ssision_id_flag = 0;
			ss_str->ssision_id = 0;
		}
		up(&ioctl_sem);
	}
	return 0;
}

struct file_operations cf_usb_init = 
{
	open:		usb_open,
	release:	usb_release,
	unlocked_ioctl:	usb_ioctl
};


/*完成字符设备的注册与设备节点的动态申请*/
static int driver_cdev_init(unsigned int major)
{
	int result = -1;
	int err;
	dev_t devno;
	if(major)
	{
		result = register_chrdev_region(devno, MEMDEV_NUM, cdev_driver_name);
	}
	else
	{
		result = alloc_chrdev_region(&devno, 0, MEMDEV_NUM, cdev_driver_name);
		major = MAJOR(devno);
		DRIVER_INFO("major dev num:%d\n", major);
	}
	if(result < 0)
	{
		DRIVER_ERROR("can't get major dev_num:%d\n", major);
		return result;
	}
	/*注册设备驱动*/
	cdev_init(&g_cdev, &cf_usb_init);/*初始化cdev结构*/
	g_cdev.owner = THIS_MODULE;
	
	err = cdev_add(&g_cdev, MKDEV(major, 0), MEMDEV_NUM); //如果有N个设备就要添加N个设备号
	if(err)
	{
		DRIVER_ERROR("add cdev failed, err is %d\n",err);
		result = err;
		goto free_chrdev_region;
	}

	/*自动注册设备节点*/
	g_dev_class = class_create(THIS_MODULE, cdev_driver_name); //创建一个类
	if(IS_ERR(g_dev_class))
	{
		result = PTR_ERR(cdev_driver_name);//创建一个类
		goto free_cdev_add;
	}
	g_dev_device = device_create(g_dev_class, NULL, MKDEV(major, 0), NULL, cdev_driver_name);
	if(IS_ERR(g_dev_device))
	{
		result = PTR_ERR(g_dev_device);
		goto free_class_create;
	}
	result = 0;
	return result;
	free_class_create:
		class_destroy(g_dev_class);
	free_cdev_add:
		cdev_del(&g_cdev);
	free_chrdev_region:
		unregister_chrdev_region(MKDEV(major, 0), MEMDEV_NUM);
		return result;
}

/*完成字符设备的卸载与设备节点的销毁*/
static void driver_cdev_exit(unsigned int major)
{
	device_destroy(g_dev_class, MKDEV(major, 0));
	class_destroy(g_dev_class);
	cdev_del(&g_cdev);
	unregister_chrdev_region(MKDEV(major, 0), MEMDEV_NUM);	
}


static int probe_usb(struct usb_interface *interface, const struct usb_device_id *id){

	spin_lock_init(&open_spinlock);//初始化锁、信号量
	sema_init(&ioctl_sem, 1);

	netlink_init(NETLINK_TEST);//初始化Netlink
	driver_cdev_init(g_mem_major);//初始化字符设备


	
	unsigned char data[1024*8+512];
	memset(data, 6, 1024*8+512);


	usb_plug_device(interface, id);

	usb_data_send(data,1024*8+512);
	usb_data_recv(data,1024*8+512);

	
	return 0;

}

static void disconnect_usb(struct usb_interface *interface){

	driver_cdev_exit(g_mem_major);
	DRIVER_INFO("cdev_exit success\n");
	netlink_exit();
	DRIVER_INFO("netlink_exit success\n");
/*
	if(dev->bulk_in_buffer!=NULL)
		kfree(dev->bulk_in_buffer);
	if(dev->bulk_out_buffer!=NULL)
		kfree(dev->bulk_out_buffer);

	if(dev!=NULL)
		kzfree(dev);
*/
	printk("USB removed\n");

}

/*
//睡眠唤醒
static int usb_suspend(struct usb_interface *intf, pm_message_tmessage)
{
	DRIVER_INFO("dev suspend\n");
	cf_suspend_flag = TRUE;
	return 0;
}

static int usb_resume(struct pci_dev *pci)
{
	DRIVER_INFO("dev resume\n");
	cf_suspend_flag = FALSE;
	return 0;
}
*/
static struct usb_device_id usb_soc_table[] =

{
	{ USB_DEVICE(0x1234, 0xabcd) },
	//{USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,USB_INTERFACE_PROTOCOL_MOUSE)},
	{}/* Terminating entry */

};
MODULE_DEVICE_TABLE (usb, usb_soc_table);

static struct usb_driver usb_driver =

{
	.name = "PE_USB_DRIVER",
	.id_table = usb_soc_table,
	.probe = probe_usb,
	.disconnect = disconnect_usb,
	//.suspend = usb_suspend,
	//.resume = usb_resume,
};






static int __init usb_init(void)

{	

	int result = usb_register(&usb_driver);
	if(result != 0){
		DRIVER_INFO("usb_init fialed!\n");
		usb_deregister(&usb_driver);
		return -ENODEV;
	}
	DRIVER_INFO("register the USB Drive OK!\n");
	return 0;

}

static void  __exit usb_exit(void)
{
/*
	if(dev->bulk_in_buffer!=NULL)
		kfree(dev->bulk_in_buffer);
	if(dev->bulk_out_buffer!=NULL)
		kfree(dev->bulk_out_buffer);
*/
	if(dev!=NULL)
		kzfree(dev);

	usb_deregister(&usb_driver);
	printk("++++++USB_EXIT : I am in USB_EXIT function\n");

}

module_init(usb_init);
module_exit(usb_exit);


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("liyapeng");
MODULE_DESCRIPTION("HX201901 Driver");
MODULE_VERSION("0.0.1");

