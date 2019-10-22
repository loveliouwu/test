/*
 * sdf_manage.h
 *
 *  Created on: 2019年8月19日
 *      Author: jjn
 */

#ifndef INCLUDE_SDF_MANAGE_H_
#define INCLUDE_SDF_MANAGE_H_

extern unsigned char sdf_device_key[16];
#define sdf_max_session_key_len 128
#define sdf_sym_key_file_name "./alg.bin"//"/home/jjn/Desktop/新建文件.bin"

typedef enum sdf_key_type_
{
	SDF_KEY_BLANK,
	SDF_KEY_ACTIVE,
}sdf_key_type;

typedef struct sdf_pool_asym_key_
{
	unsigned int key_status;
	unsigned int session_private_limit[SESSION_NUM];
	unsigned int key_password_len;
	unsigned char key_password[16];
	unsigned char sig_pub_key[64];
	unsigned char sig_pri_key[32];
	unsigned char enc_pub_key[64];
	unsigned char enc_pri_key[32];
}sdf_pool_asym_key;

extern sdf_pool_asym_key* sdf_asym_key_pool;

typedef struct sdf_key_manage_
{
	packet_head sdf_cmd_head;
	unsigned int key_index;
	unsigned int sym_asym_flag;
	unsigned int password_len;
	unsigned char password[16];
}sdf_key_manage;
/*****************************************************************************************************************
 * private F function: 把文件中密钥密文解密成明文进行存储
 * return: 0 success,-1 error
 ******************************************************************************************************************/
int sdf_key_init();

/*****************************************************************************************************************
 * private F function: 生成一组对称密钥
 * args:    [in] key_index :生成密钥索引
 * args:    [in] sym_asym_flag : 0对称，1非对称
 * args:    [in] password_len : 私钥保护口令长度
 * args:    [in] password : 私钥保护口令
 * return: 0 success,-1 error,1 密钥已经存在
 ******************************************************************************************************************/
int sdf_generate_key(unsigned int key_index,unsigned int sym_asym_flag,unsigned int password_len,unsigned char *password);

/*****************************************************************************************************************
 * private F function: 删除一组密钥
 * args:    [in] key_index :删除密钥索引
 * args:    [in] sym_asym_flag : 0对称，1非对称
 * args:    [in] password_len : 私钥保护口令长度
 * args:    [in] password : 私钥保护口令
 * return: 0 success,-1 error
 ******************************************************************************************************************/
int sdf_delete_key(unsigned int key_index,unsigned int sym_asym_flag,unsigned int password_len,unsigned char *password);

#endif /* INCLUDE_SDF_MANAGE_H_ */
