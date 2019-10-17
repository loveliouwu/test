#ifndef SDF_COMMON_H_
#define SDF_COMMON_H_

#include"SDF_Interface.h"


#define SOCKET_NUM                  (21)
#define MAX_PACKET_LEN              (1024)
#define MAX_SEND_BUFF_LEN           (8*1024)
#define MAX_KEY_INDEX               (4)
#define SM2_BITS                    (256)
#define SM2_BYTES                   (32)
#define SM4_BITS                    (128*8)
#define IV_LENGHT                   (16)
#define SM4_KEY_LEN                 (16)
#define MAX_RANDOM_LEN              (2048)
#define MAX_ID_LEN                  (256)
#define PUB_KEY_LEN                 (64)
#define PRIV_KEY_LEN                (32)

#define PIN_MIN_LEN                 (0)
#define PIN_MAX_LEN                 (16)


//服务类命令
enum MINOR_CMD_ENUM{
    OPEN_DEVICE = 0x0,
    CLOSE_DEVICE,
    OPEN_SESSION,
    CLOSE_SESSION,
    SYM_ENCRYPT,
    SYM_DECRYPT,
    HASH_INIT,
    HASH_UPDATE,
    HASH_FINAL,
    HASH_COPY,
    ECC_SIGN,
    ECC_VERIGY,
    ECC_EX_VERIFY,//
    GET_RANDOM,
    GENERATE_KEY_PAIR,
    DELETE_KEY_PAIR,
    GENERATE_AGREEMENT_PARA,
    GENERATE_AGREEMENT_PARA_KEY,
    GENERATE_KEY_ECC,
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
    int len;//数据长度，包含包头
    guid device_guid;
    void *session_key_list;//未用到
    void *socket_list;//未用到
    void *server_session_handle;//服务器维护的会话链表中的会话句柄
}packet_head;//大小56字节

/*
SDF算法相关的数据结构
*/
typedef struct  symalg_cipher
{
    unsigned int alg_id;
    unsigned char key_handle[16];//
    unsigned char IV[IV_LENGHT];
    unsigned int data_len;
    unsigned char data[0];
}symalg_cipher;

/*sm3*/
typedef struct sdf_sm3_st
{
    unsigned int data_len;
    unsigned char data[0];
}SM3_st;



//获取随机数包头
typedef struct GetRandom_st{
    unsigned int RandomLen;
    unsigned char data[0];
}GetRandom;

/*生成秘钥协商参数*/
typedef struct sdf_gen_agreement_par_handle_
{
	unsigned int A_prikey_index;
	unsigned  int session_key_len;
	unsigned char rng[32];
	unsigned char A_pubkey[64];
	unsigned char A_mp_pubkey[64];
	unsigned int A_id_len;
	unsigned char A_id[256];
}sdf_gen_agreement_par_handle;

typedef struct gen_agreement_para_st{
    unsigned int key_index;
    unsigned int password_len;
    unsigned char password[PIN_MAX_LEN];
    unsigned int key_bits;
    unsigned int ID_length;
    unsigned char sponsor_ID[MAX_ID_LEN];
    unsigned char pubkey[PUB_KEY_LEN];
    unsigned char tmp_pubkey[PUB_KEY_LEN];
    sdf_gen_agreement_par_handle agreement_handle;
}Gen_agreemen_para;

/*生成协商参数并计算会话秘钥*/
typedef struct gen_agreement_key
{
    unsigned int key_index;
    unsigned int password_len;
    unsigned char password[PIN_MAX_LEN];
    unsigned int key_bits;
    unsigned int sponsorID_length;
    unsigned char sponsor_ID[MAX_ID_LEN];
    unsigned int responseID_length;
    unsigned char response_ID[MAX_ID_LEN];
    unsigned char sponsor_pubkey[PUB_KEY_LEN];
    unsigned char sponsor_tmp_pubkey[PUB_KEY_LEN];
    unsigned char response_pubkey[PUB_KEY_LEN];
    unsigned char response_tmp_pubkey[PUB_KEY_LEN];
    unsigned int session_key_out_len;
    unsigned char session_key[0];
}Gen_agreement_key;


/*计算会话秘钥*/
typedef struct cipher_agreement_key{
    unsigned int ID_Length;
    unsigned char response_ID[MAX_ID_LEN];
    unsigned char pubkey[PUB_KEY_LEN];
    unsigned char tmp_pubkey[PUB_KEY_LEN];
    sdf_gen_agreement_par_handle key_handle;
    unsigned int session_key_out_len;
    unsigned char session_key[0];
}Cipher_agreement_key;


/*生成或销毁密钥对*/
typedef struct Generate_key_pair_st
{
    unsigned int index;
    unsigned int length;
    unsigned char password[PIN_MAX_LEN];
}Generate_key_pair;


/*签名验签*/
typedef struct Sign_Verify_ECC_st
{
    unsigned int index;
    unsigned int password_len;
    unsigned char password[PIN_MAX_LEN];
    unsigned char datalen;
    unsigned char data[SM2_BYTES];
    unsigned char sign[64];
}Sign_Verify_ECC;

/*外部验签*/
typedef struct sdf_extern_ecc_verify_
{
	unsigned char in_hash_data[32];
	unsigned char in_pub_key[64];
	unsigned char in_out_r_s_value[64];
}sdf_extern_ecc_verify;

#endif
