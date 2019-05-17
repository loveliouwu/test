#ifndef _PE100_USB_DRIVER_H_
#define _PE100_USB_DRIVER_H_

#include <linux/init.h>
#include <linux/timer.h>
#include<linux/module.h>
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h>

#define TRUE 		(1)
#define FALSE 		(0)

#define MAX_SEND_DATA_LEN			8*1024 //SDF大包的最大数据长度
#define MAX_MSGSIZE 				512
#define MAX_BUFFER_SIZE 			1024*8+512 //驱动申请的buffer大小

/*临时声明*/
#define SDR_BEINGCONFIGURED 	0x00000022 //正在配置过程中
#define SDR_OK					0x00000000 //结果正确
#define SDR_UNKNOWERR			0x00000001
#define SDR_OPENSSISION			0x00000006
#define SDR_COMMFAIL			0x00000003//与设备通信失败

#define CARD_CONFIG		0x33	//NETLINK_CMD

#define CARD_CONFIGURATION_START 0xc0
#define CARD_CONFIGURATION_STOP 0xc1

#define IV_LENGTH		(16)
typedef struct symalg_cipher_st
{
	unsigned int key_handle;		//密钥句柄
	unsigned int alg_id;			//算法标识
	unsigned int data_len;			//加解密数据总长度
	unsigned char IV[IV_LENGTH];	//IV数据
}symalg_cipher;
#define ECCref_MAX_BITS		(512)
#define ECCref_MAX_LEN		((ECCref_MAX_BITS+7)/8)
typedef struct ECCrefPublicKey_st
{
	unsigned int bits;
	unsigned char x[ECCref_MAX_LEN];
	unsigned char y[ECCref_MAX_LEN];
}ECCrefPublicKey;


#define MAX_ID_LEN (256)
typedef struct hash_para
{
	unsigned int alg_id;
	ECCrefPublicKey public;
	unsigned int ID_length;
	unsigned char ID[MAX_ID_LEN];
}hash_para;

#define HASH_INIT		0x25
#define HASH_UPDATE		0x26
#define HASH_FINAL		0x27
#define HASH_PRE_FINAL	0x28
/*临时声明结束*/





/*会话 会话对应的存放地址应该是 g_ssision_struct[pid-1]*/
typedef struct g_ssision_struct_st
{
	unsigned int ssision_id_flag;
	unsigned int ssision_id;


	unsigned char hash_misc_data[75];//保存hash运算中返回的７４字节中间数据
	
	//在申请会话的时候初始化三个用于sym任务的全局变量
	unsigned char sym_flag;//标记一个sym任务 0表示任务未开始 1表示任务进行中
	unsigned int sym_data_len;//保存sym大包时任务的总长度
	unsigned int sym_offset;//保存sym大包时已发送的总长度,当offset和任务总长度相同时，表明该次sym任务完成，清零这两个变量
}g_ssision_struct;



/*启动停止管理配置*/
unsigned int card_config_start_flag = 0;
typedef enum
{
	CONFIG_STOP,
	CONFIG_START,
}config_status;

//向应用返回异常状态信息时用于标记异常状态来源
typedef enum
{
	NETLINK_STATUS,
	TASK_STATUS,
}status_flag;

/*usb驱动自定义结构体*/
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
        struct mutex io_mutex;            //同步的IO互斥锁

};


/*
netlink收到的包由以下部分组成：
【 nlmsghdr 】【 nl_packet_st(task_info) 】【 card_head 】【 data 】
*/

/*netlink接收包头card_head部分*/
typedef struct card_head_st
{
	unsigned int packet_cmd;//SDF填充
	unsigned int status;//驱动填充
	unsigned int packet_len;//SDF填充
	unsigned int reserved;//驱动未用到
}card_head;

/*
netlink接收发送包头nl_packet_st部分，对应SDF接口中 task_info
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

/*对应nl_packet_st中的flag*/
typedef enum
{
	RECV_TASK_FLAG = 1,		//接收下发任务的标记
	RECV_DATA_FLAG,			//接收下发数据的标记
	REQ_DATA_FLAG,			//发送请求数据的标记
	SEND_DATA_FLAG,			//发送已处理数据的标记
	RECV_KERNEL_TASK_FLAG,
}flag_type;

/*对应nl_packe_st中的task_type*/
typedef enum
{
	TYPE_CPU = 1,
	TYPE_SYM,
	TYPE_ASYM,
	TYPE_HASH,
}task_type;



/*
USB包头由以下部分组成
【usb_packet_st】【data】
*/

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
	unsigned int packet_pid;
	unsigned int packet_cmd;	//具体命令编号
	unsigned int packet_status;		//执行状态
	unsigned int packet_len;	//整个数据包长度
}usb_packet_st;




static int driver_cdev_init(void);
static void driver_cdev_exit(void);

int send_netlink_status(nl_packet_st *data_info, unsigned int status, unsigned int flag);
int netlink_recv_manage(char *data_addr, unsigned int nlmsg_pid);
static unsigned int  usb_data_send(unsigned char * pbuff,unsigned int length);
static unsigned int usb_data_recv(unsigned char * pbuff);
int usb_plug_device(struct usb_interface *interface, const struct usb_device_id *id);
static int probe_usb(struct usb_interface *interface, const struct usb_device_id *id);
static void disconnect_usb(struct usb_interface *interface);


#endif


