#ifndef SDF_COMMON_H_
#define SDF_COMMON_H_

#include"SDF_Interface.h"


#define SOCKET_NUM                  (200)
#define MAX_SEND_BUFF_LEN           (1024*1024)
#define MAX_KEY_INDEX               (4)
#define SM4_BITS                    (128*8)
#define IV_LENGHT                   (16)
#define SM4_KEY_LEN                 (16)
#define MAX_RANDOM_LEN              (2048)
#define MAX_ID_LEN                  (256)
#define PUB_KEY_LEN                 (64)
#define PRIV_KEY_LEN                (32)
//控制类命令
enum MAJOR_CMD_ENUM{
    OPEN_DEVICE = 0x0,
    CLOSE_DEVICE,
    OPEN_SESSION,
    CLOSE_SESSION,
};
//服务类命令
enum MINOR_CMD_ENUM{
    SYM_ENCRYPT,
    SYM_DECRYPT,
    GET_RANDOM,
    HASH_INIT,
    HASH_UPDATE,
    HASH_FINAL,
};



/*
维护会话双向链表和设备创建的socket相关的结构体
*/
typedef struct guid{
    unsigned short type;//设备GUID还是会话GUID
    unsigned short index;
    unsigned short reserve[2];
    unsigned long long random;
}guid;

typedef struct session{
    struct  guid Dev_GUID;
    void *session_handle;//服务端返回的会话句柄
    unsigned int socket_fd;//会话绑定的socket_fd索引，从索引1开始,0用于设备控制类通信
    void  *pdev_handle;//指向该回话绑定的设备句柄以便于维护会话列表
    unsigned char *send_buff;
    unsigned char *recv_buff;
}session;


typedef struct session_list{
    struct session sess;
    struct session_list *prev;
    struct session_list *next;
}session_list;

typedef struct device{
    struct guid GUID;
    struct session_list *session_list_head;//会话双向链表的表头
    struct session_list *session_list_end;//链表的最后一个元素，便于添加新节点
    unsigned int socketfd[SOCKET_NUM];
    unsigned int fd_flag[SOCKET_NUM];
    unsigned char *control_send_buff;
    unsigned char *control_recv_buff;

}device;

enum MAJOR_CMD{
    control_cmd,
    server_cmd,
};



typedef struct packet_head{
    int major_cmd;
    int minor_cmd;
    int status;
    int len;//数据长度，不包含包头
    guid device_guid;
    void *session_key_list;//未用到
    void *socket_list;//未用到
    void *server_session_handle;//服务器维护的会话链表中的会话句柄
}packet_head;//大小５６字节

/*
SDF算法相关的数据结构
*/
typedef struct  symalg_cipher
{
    unsigned int key_handle;
    unsigned int alg_id;
    unsigned int data_len;
    unsigned char IV[IV_LENGHT];
}symalg_cipher;

//获取随机数包头
typedef struct GetRandom_st{
    unsigned int RandomLen;
    unsigned char data[1];
}GetRandom;

/*生成秘钥协商参数*/
typedef struct gen_agreement_para_st{
    unsigned int key_index;
    unsigned int sid;
    unsigned int key_bits;
    unsigned int ID_length;
    unsigned char sponsor_ID[MAX_ID_LEN];
    unsigned char pubkey[PUB_KEY_LEN];
    unsigned char tmp_pubkey[PUB_KEY_LEN];
    unsigned char agreemen_handle[64];
}Gen_agreemen_para;





#endif