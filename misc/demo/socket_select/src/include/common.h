/*
 * hsm.h
 *
 *  Created on: 2019年8月8日
 *      Author: jjn
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_




#define DEBUG
#ifdef DEBUG
#define printfS printf
#else
#define printfS(format,...)  ((void)0)
#endif


#define SOCKET_NUM 20
#define DEVICE_NUM 20
#define SESSION_NUM 20
#define SESSION_KEY_NUM 20
#define SYM_KEY_NUM 4
#define ASYM_KEY_NUM 4
#define SOCKET_MAX_DATA_LEN 8*1024

extern int fd_A[SOCKET_NUM];//链接的socket


typedef enum GUID_TYPE_
{
	DEVICE_GUID=0xcc,
	SESSION_GUID,
	SESSION_KEY_GUID,
	SOCKET_GUID,
}GUID_TYPE;
typedef struct guid_{
	unsigned char type;
	unsigned char index;
	unsigned char exist_flag;
	unsigned char use_flag;//标志任何情况下1为真0为假
	unsigned char random[8];
}guid;

typedef struct session_info_{
	struct session_info *prev;
	guid 	GUID;
	unsigned char sm3_par_p[104];//sm3结构体大小
	struct session_info *next;
}session_info;

//会话密钥
typedef struct session_key_info_{
	guid GUID;
	int key_len;
	unsigned char key[0];
}session_key_info;

//socket信息
typedef struct socket_info_{
	struct socket_info *prev;
	int sockfd;//sockfd不为0则建立socket链接
	int use;//读写原子操作
	struct socket_info *next;
}socket_info;

//设备信息
typedef struct device_{
	guid device_guid;
	session_key_info *session_key_list_head;
	socket_info *socket_list_head;
	session_info *session_list_head;
}device;
typedef struct device_info_{
	 device dev[DEVICE_NUM];
}device_info;

//传输数据
typedef enum MAJOR_CMD_{
	control_cmd,
	server_cmd,
}MAJOR_CMD;

typedef struct xmit_protocol_{
	int manage_cmd;
	int sdf_cmd;
    int status;
	int len;
}xmit_protocol;
typedef struct packet_head_{
	xmit_protocol sdf_cmd_head;
	device sdf_device_info;
}packet_head;

//SDF接口命令
typedef enum sdf_cmd_
{
	SDF_OPEN_DEVICE,
	SDF_CLOSE_DEVICE,
	SDF_CREAT_SESSION,
	SDF_CLOSE_SESSION,
	SDF_SYM_ENC,
	SDF_SYM_DEC,
	SDF_HASH_INIT,
	SDF_HASH_UPDATE,
	SDF_HASH_FINIAL,
	SDF_HASH_COPY_SESSION,
	SDF_ASYM_SIGN,
	SDF_ASYM_INTER_VERIFY,
	SDF_ASYM_EXTER_VERIFY,
	SDF_GET_RNG,
	SDF_GET_ASYM_KEYPAIR,
	SDF_DELETE_ASYM_KEYPAIR,
	SDF_GET_AGREEMENT_DATA_WITH_ECC,//生成密钥协商参数
	SDF_GET_AGREEMENT_DATA_AND_KEY_WITH_ECC,//生成密钥协商参数并计算会话密钥
	SDF_GET_KEY_WITH_ECC,//计算会话密钥
	SDF_DELETE_SESSION_KEY,
}sdf_cmd;

//管理命令
typedef enum manage_cmd_
{
	MAN_GEN_KEY,
	MAN_DELETE_KEY,
}manage_cmd;

//SDF返回值
#define SDR_OK	0x0 									//操作成功
#define SDR_BASE				0x01000000 				//错误码基础值
#define SDR_UNKNOWERR			SDR_BASE+0x00000001		//未知错误
#define SDR_NOTSUPPORT			SDR_BASE+0x00000002		//不支持的接口调用
#define SDR_COMMFAIL 			SDR_BASE+0x00000003		//与设备通信失败
#define SDR_HARDFAIL 			SDR_BASE+0x00000004		//运算模块元响应
#define SDR_OPENDEVICE 			SDR_BASE+0x00000005		//打开设备失败
#define SDR_OPENSESSION 		SDR_BASE+0x00000006		//创建会话失败
#define SDR_PARDENY				SDR_BASE+0x00000007		//无私钥使用权限
#define SDR_KEYNOTEXIST			SDR_BASE+0x00000008		//不存在的密钥调用
#define SDR_ALGNOTSUPPORT 		SDR_BASE+0x00000009		//不支持的算法调用
#define SDR_ALGMODNOTSUPPORT 	SDR_BASE+0x0000000A		//不支持的算法模式调用
#define SDR_PKOPERR 			SDR_BASE+0x0000000B		//公钥运算失败
#define SDR_SKOPERR 			SDR_BASE+0x0000000C		//私钥运算失败
#define SDR_SIGNERR				SDR_BASE+0x0000000D 	//签名运算失败
#define SDR_VERIFYERR 			SDR_BASE+0x0000000E		//验证签名失败
#define SDR_SYMOPERR 			SDR_BASE+0x0000000F		//对称算法运算失败
#define SDR_STEPERR 			SDR_BASE+0x00000010		//多步运算步骤错误
#define SDR_FILESIZEERR 		SDR_BASE+0x00000011		//文件长度超出限制
#define SDR_FILENOEXIST			SDR_BASE+0x00000012 	//指定的文件不存在
#define SDR_FILEOFSERR			SDR_BASE+0x00000013		//文件起始位置错误
#define SDR_KEYTYPEERR			SDR_BASE+0x00000014		//密钥类型错误
#define SDR_KEYERR 				SDR_BASE+0x00000015		//密钥错误
#define SDR_ENCDATAERR			SDR_BASE+0x00000016		//ECC加密数据错误
#define SDR_RANDERR				SDR_BASE+0x00000017		//随机数产生失败
#define SDR_PRKRERR				SDR_BASE+0x00000018		//私钥使用权限获取失败
#define SDR_MACERR 				SDR_BASE+0x00000019		//MAC运算失败
#define SDR_FILEEXISTS 			SDR_BASE+0x000000lA		//指定文件巳存在
#define SDR_FILEWERR			SDR_BASE+0x0000001B 	//文件写入失败
#define SDR_NOBUFFER 			SDR_BASE+0x0000001C		//存储空间不足
#define SDR_INARGERR			SDR_BASE+0x0000001D		//输人参数错误
#define SDR_OUTARGERR 			SDR_BASE+0x0000001E		//输出参数错误
/*SDR_BASE+0x0000001F -SDR_BASE+0x00FFFFFF预留备用*/
void get_random(unsigned char *data,int len);
void free_point(void *p);
void PrintBuf(unsigned char *buf, int	buflen);
int socket_server_test(void);
void sdf_rev_cmd(int sockfd,char* data,int len);
int socket_epoll();
#endif /* INCLUDE_COMMON_H_ */
