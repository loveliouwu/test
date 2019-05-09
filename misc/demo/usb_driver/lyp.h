#ifndef _PE100_USB_DRIVER_H_
#define _PE100_USB_DRIVER_H_

#include <linux/init.h>
#include <linux/timer.h>
#include<linux/module.h>
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h>

#define MAX_MSGSIZE 	512
#define MAX_BUFFER_SIZE 1024*8+512

/*临时声明*/
#define SDR_BEINGCONFIGURED 0x00000022 //正在配置过程中







unsigned int card_config_start_flag = 0;
typedef enum
{
	CONFIG_STOP,
	CONFIG_START,
}config_status;



struct usb_skel {

        struct usb_device *udev;          //该设备的usb_device指针
        struct usb_interface *interface;  //该设备的usb_interface指针
        struct semaphore limit_sem;       //限制进程写的数据量
        unsigned char  bulk_in_buffer[MAX_BUFFER_SIZE];   //接收数据的缓冲区
	unsigned char  bulk_out_buffer[MAX_BUFFER_SIZE];   //发送数据的缓冲区
        unsigned int bulk_data_size;         //接收缓冲区大小
        __u8 bulk_in_endpointAddr;        //批量IN端点的地址
        __u8 bulk_out_endpointAddr;       //批量OUT端点的地址
        struct kref kref;                 //sturct kref作为内核中最基本的引用计数而存在
        struct mutex io_mutex;            //同步的IO互斥锁，保证

};


/*
netlink接收发送的数据头信息
*/
typedef struct nl_task_type_comment_st
{
	unsigned char flag;		//任务flag
	unsigned char task_type;	//任务类型属性
	unsigned char task_cmd;		//任务ID
	unsigned char task_pid;		//任务的PID
	unsigned short data_len;	//当前的数据长度
	unsigned char task_priority;	//任务优先级属性
	unsigned char cache_num;	//任务所在缓冲区编号
	unsigned int task_status;	//任务状态
	unsigned int task_len;		//任务所需要处理的数据的长度	
}nl_packet_st;

typedef enum
{
	RECV_TASK_FLAG = 1,		//接收下发任务的标记
	RECV_DATA_FLAG,			//接收下发数据的标记
	REQ_DATA_FLAG,			//发送请求数据的标记
	SEND_DATA_FLAG,			//发送已处理数据的标记
	RECV_KERNEL_TASK_FLAG,
}flag_type;

typedef enum
{
	TYPE_CPU = 1,
	TYPE_SYM,
	TYPE_ASYM,
	TYPE_HASH,
}task_type;



/*
usb发送数据头中区分cmd命令
*/
typedef enum interface_cmd_st
{
	DATA_INPUT_CMD,
	DATA_OUTPUT_CMD,
	CONTROL_INPUT_CMD,
	CONTROL_OUTPUT_CMD,
	STATUS_OUTPUT_CMD,
}interface_cmd;


/*
usb接收发送的数据头信息
*/
typedef struct _usb_packet_st
{
	interface_cmd head;	//SDF与管理软件区分
	unsigned int packet_cmd;	//具体命令编号
	unsigned int status;		//执行状态
	unsigned int packet_len;	//整个数据包长度
}usb_packet_st;





int send_netlink_status(nl_packet_st *data_info, unsigned int status);
int netlink_recv_manage(char *data_addr, unsigned int nlmsg_pid);
static unsigned int  usb_data_send(unsigned char * pbuff,unsigned int length);
static unsigned int usb_data_recv(unsigned char * pbuff,unsigned int length);
int usb_plug_device(struct usb_interface *interface, const struct usb_device_id *id);
static int probe_usb(struct usb_interface *interface, const struct usb_device_id *id);
static void disconnect_usb(struct usb_interface *interface);


#endif


