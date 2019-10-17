
#ifndef INCLUDE_SDF_H_
#define INCLUDE_SDF_H_

#define SDF_ALG_SERVER_MAX_PACKET 8*1024
extern unsigned int sdf_device_num;

//对称运算
typedef struct sdf_sym_server_
{
	unsigned int in_alg_id;//算法标识，用于对称加密和解密区分
	unsigned char in_key[16];
	unsigned char in_out_iv[16];
	unsigned int in_out_data_len;
	unsigned char in_out_data[0];
}sdf_sym_server;

//sm3
typedef struct sdf_sm3_server_
{
	unsigned int in_out_data_len;
	unsigned char in_out_data[0];
}sdf_sm3_server;

//sm2_server
typedef struct sdf_sm2_server_
{
	unsigned int in_key_index;
	unsigned int in_password_len;
	unsigned char in_password[16];
	unsigned char in_hash_data_len;
	unsigned char in_hash_data[32];
	unsigned char in_out_r_s_value[64];
}sdf_sm2_server;
typedef struct sdf_extern_ecc_verify_
{
	unsigned char in_hash_data[32];
	unsigned char in_pub_key[64];
	unsigned char in_out_r_s_value[64];
}sdf_extern_ecc_verify;


//获取随机数
typedef struct sdf_get_rng_
{
	unsigned int in_data_len;
	unsigned char out_data[0];
}sdf_get_rng;

//生成密钥对
typedef struct sdf_gen_asym_key_
{
	unsigned int in_out_key_index;
	unsigned int in_password_len;
	unsigned char in_password[16];
}sdf_gen_asym_key;

//生成密钥协商参数并输出
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
typedef struct sdf_gen_agreement_par_
{
	unsigned int in_key_index;
	unsigned int in_password_len;
	unsigned char in_password[16];
	unsigned int in_key_bits;
	unsigned int in_A_id_length;
	unsigned char in_A_ID[256];
	unsigned char out_A_pubkey[64];
	unsigned char out_A_mp_pubkey[64];
	sdf_gen_agreement_par_handle out_handle;
}sdf_gen_agreement_par;

//产生密钥协商参数并计算会话密钥
typedef struct sdf_gen_agreement_par_and_key_
{
	unsigned int in_key_index;
	unsigned int in_password_len;
	unsigned char in_password[16];
	unsigned int in_key_bits;
	unsigned int in_A_id_length;
	unsigned char in_A_ID[256];
	unsigned int in_B_id_length;
	unsigned char in_B_ID[256];
	unsigned char in_A_pubkey[64];
	unsigned char in_A_mp_pubkey[64];
	unsigned char out_B_pubkey[64];
	unsigned char out_B_mp_pubkey[64];
	unsigned int out_session_key_len;
	unsigned char out_session_key[0];
}sdf_gen_agreement_par_and_key;
//计算会话密钥
typedef struct sdf_count_session_key_
{
	unsigned int in_B_id_length;
	unsigned char in_B_ID[256];
	unsigned char in_B_pubkey[64];
	unsigned char in_B_mp_pubkey[64];
	sdf_gen_agreement_par_handle in_handle;
	unsigned int out_session_key_len;
	unsigned char out_session_key[0];
}sdf_count_session_key;

//销毁会话密钥
typedef struct sdf_destroy_session_key_
{
	unsigned int in_index;
}sdf_destroy_session_key;

//导出签名或加密公钥
typedef struct sdf_export_ecc_pub_key_
{
	unsigned int in_index;
	unsigned char out_pub_key[64];
}sdf_export_ecc_pub_key;

//生成会话密钥并用内部/外部ecc公钥加密输出
typedef struct sdf_export_key_
{
	unsigned int in_index;
	unsigned char out_pub_key[64];
}sdf_export_key;

//生成会话密钥并用内部/外部密钥加密
typedef struct sdf_get_session_and_enc_out_
{
	unsigned int in_key_index;
	unsigned int in_session_key_bits;
	unsigned int in_pubkey_alg_id;
	unsigned char in_pub_key[64];
	unsigned int out_session_key_index;
	unsigned int out_data_len;
	unsigned char out_data[0];
}sdf_get_session_and_enc_out;

//导入会话密钥并解密
typedef struct sdf_get_session_and_dec_
{
	unsigned int in_key_index;
	unsigned int in_password_len;
	unsigned char in_password[16];
	unsigned int in_alg_id;
	unsigned int out_session_key_index;
	unsigned int in_data_len;
	unsigned char in_data[0];
}sdf_get_session_and_dec;

//数字信封转换
typedef struct sdf_exchange_digit_envelope_
{
	unsigned int in_key_index;
	unsigned int in_password_len;
	unsigned char in_password[16];
	unsigned int in_alg_id;
	unsigned char in_pub_key[64];
	unsigned int in_out_data_len;
	unsigned char in_out_data[0];
}sdf_exchange_digit_envelope;

/*****************************************************************************************************************
 * private F function: 打开设备
 * args:    [in] sdf_cmd_head :包头
 * return: none
 ******************************************************************************************************************/
void SDF_OpenDevice(packet_head **p);

/*****************************************************************************************************************
 * private F function: 关闭设备
 * args:    [in] sdf_cmd_head :包头
 * return: none
 ******************************************************************************************************************/
void SDF_CloseDevice(packet_head **p);

/*****************************************************************************************************************
 * private F function: 打开会话
 * args:    [in] sdf_cmd_head :包头
 * return: none
 ******************************************************************************************************************/
void SDF_OpenSession(packet_head **p);

/*****************************************************************************************************************
 * private F function: 关闭会话
 * args:    [in] sdf_cmd_head :包头
 * return: none
 ******************************************************************************************************************/
void SDF_CloseSession(packet_head **p);

/*****************************************************************************************************************
 * private F function: SDF_Encrypt
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] sdf_sym_server.in_alg_id: 算法标识
 * args:    [in] sdf_sym_server.in_key :密钥
 * args:    [in/out] sdf_sym_server.in_out_iv :iv
 * args:    [in/out] sdf_sym_server.in_out_data_len: 输入输出数据长度
 * args:    [in/out] sdf_sym_server.in_out_data: 输入输出数据
 * return: none
 ******************************************************************************************************************/
void SDF_Encrypt(packet_head **sdf_cmd_head,sdf_sym_server **p);

/*****************************************************************************************************************
 * private F function: SDF_Decrypt
 * args:    [in] df_sym_server.sdf_cmd_head :包头
 * args:    [in] sdf_sym_server.in_alg_id: 算法标识
 * args:    [in] sdf_sym_server.in_key :密钥
 * args:    [in/out] sdf_sym_server.in_out_iv :iv
 * args:    [in/out] sdf_sym_server.in_out_data_len: 输入输出数据长度
 * args:    [in/out] sdf_sym_server.in_out_data: 输入输出数据
 * return: none
 ******************************************************************************************************************/
void SDF_Decrypt(packet_head **sdf_cmd_head,sdf_sym_server **p);

/*****************************************************************************************************************
 * private F function: 三步式数据杂凑运算第一步
 * args:    [in] sdf_cmd_head :包头
 * return: none
 ******************************************************************************************************************/
void SDF_HashInit(packet_head **sdf_cmd_head);

/*****************************************************************************************************************
 * private F function: 三步式数据杂凑运算第二步
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] in_data :输入数据
 * args:    [in] in_data_len :输入数据长度
 * return: none
 ******************************************************************************************************************/
void SDF_HashUpdate(packet_head **sdf_cmd_head,sdf_sm3_server **p);

/*****************************************************************************************************************
 * private F function: 三步式数据杂凑运算第三步
 * args:    [in] sdf_cmd_head :包头
 * args:    [out] out_data :输出数据
 * args:    [out] out_data_len :输出数据长度，32
 * return: none
 ******************************************************************************************************************/
void SDF_HashUpFinal(packet_head **sdf_cmd_head,sdf_sm3_server **p);

/*****************************************************************************************************************
 * private F function: 将一个会话信息复制到一个新的会话
 * args:    [in] sdf_cmd_head :包头
 * args:    [out] sdf_out_data
 * return: none
 ******************************************************************************************************************/
void SDF_Hash_Copy(packet_head **sdf_cmd_head,packet_head *sdf_out_data);

/*****************************************************************************************************************
 * private F function: sdf_cmd_head
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] data_len :随机数数据长度
 * args:    [out] data :随机数数据
 * return: none
 ******************************************************************************************************************/
void SDF_GenerateRandom(packet_head **sdf_cmd_head,sdf_get_rng **p);

/*****************************************************************************************************************
 * private F function: 产生ECC密钥对并输出
 * args:    [in] sdf_cmd_head :包头
 * args:    [out] key_index :生成的密钥索引
 * args:    [in] password_len :私钥保护口令长度
 * args:    [in] password :私钥保护口令
 * return: none
 ******************************************************************************************************************/
void SDF_GenerateKeyPair_ECC(packet_head **sdf_cmd_head,sdf_gen_asym_key **p);

/*****************************************************************************************************************
 * private F function: 删除非对称密钥
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] key_index :生成的密钥索引
 * args:    [in] password_len :私钥保护口令长度
 * args:    [in] password :私钥保护口令
 * return: none
 ******************************************************************************************************************/
void SDF_DeleteKeyPair_ECC(packet_head **sdf_cmd_head,sdf_gen_asym_key **p);

/*****************************************************************************************************************
 * private F function: 使用内部ECC私钥对数据进行签名运算
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] key_index :密钥索引
 * args:    [in] password_len :私钥保护口令长度
 * args:    [in] password :私钥保护口令
 * args:    [in] in_data_len :输入数据长度
 * args:    [in] in_data :输入数据
 * args:    [out] out_r_s_value: 输出数据
 * return: none
 ******************************************************************************************************************/
void SDF_InternalSign_ECC(packet_head **sdf_cmd_head,sdf_sm2_server **p);


/*****************************************************************************************************************
 * private F function: 使用外部ECC公钥对数据进行验证运算
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] in_data_len :输入数据长度
 * args:    [in] in_data :输入数据
 * args:    [in] r_s_value:签名数据
 * return: none
 ******************************************************************************************************************/
void SDF_ExternalVerify_ECC(packet_head **sdf_cmd_head,sdf_extern_ecc_verify **p);

/*****************************************************************************************************************
 * private F function: 生成密钥协商参数并输出
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] in_key_index :密钥索引
 * args:    [in] in_password_len:密钥私钥保护口令长度
 * args:    [in] in_password:密钥私钥保护口令
 * args:    [in] in_key_bits :协商密钥位数
 * args:    [in] in_A_id_length :发起方ID长度
 * args:    [in] in_A_ID:发起方ID
 * args:    [out] out_A_pubkey :发起方公钥
 * args:    [out] out_A_mp_pubkey:发起方临时公钥
 * args:    [out] out_handle: 密钥协商句柄
 * return: none
 ******************************************************************************************************************/
void SDF_GenerateAgreementDataWithECC(packet_head **sdf_cmd_head,sdf_gen_agreement_par **p);

/*****************************************************************************************************************
 * private F function: 产生协商数据并计算会话密钥
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] in_key_index :密钥索引
 * args:    [in] in_key_bits :协商密钥位数
 * args:    [in] in_A_id_length :发起方ID长度
 * args:    [in] in_A_ID:发起方ID
 * args:    [in] in_B_id_length :响应方ID长度
 * args:    [in] in_B_ID:响应方ID
 * args:    [in] in_A_pubkey :发起方公钥
 * args:    [in] in_A_mp_pubkey:发起方临时公钥
 * args:    [out] out_B_pubkey :响应方公钥
 * args:    [out] out_B_mp_pubkey:响应方临时公钥
 * args:    [out] out_session_key_len:会话密钥长度
 * args:    [out] out_session_key:会话密钥
 * return: none
 ******************************************************************************************************************/
void SDF_GenerateAgreementDataAndKeyWithECC( packet_head **sdf_cmd_head,sdf_gen_agreement_par_and_key **p);

/*****************************************************************************************************************
* private F function: 计算会话密钥
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] in_B_id_length :响应方ID长度
 * args:    [in] in_B_ID:响应方ID
 * args:    [in] in_B_pubkey :响应方公钥
 * args:    [in] in_B_mp_pubkey:响应方临时公钥
 * args:    [in] in_handle: 密钥协=协商参数时候得到的密钥协商句柄
 * args:    [out] out_session_key_len:会话密钥长度
 * args:    [out] out_session_key:会话密钥
 * return: none
 ******************************************************************************************************************/
void SDF_GenerateKeyWithECC(packet_head **sdf_cmd_head,sdf_count_session_key **p);

/*****************************************************************************************************************
 * private F function: 导出ECC加密公钥
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] in_key_index :密钥索引
 * args:    [out] out_pub_key  :输出公钥
 * return: none
 ******************************************************************************************************************/
void SDF_ExportEncPublicKey_ECC(packet_head **sdf_cmd_head,sdf_export_ecc_pub_key **p);

/*****************************************************************************************************************
 * private F function: 导出ECC签名公钥
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] in_key_index :密钥索引
 * args:    [out] out_pub_key  :输出公钥
 * return: none
 ******************************************************************************************************************/
void SDF_ExportSignPublicKey_ECC(packet_head **sdf_cmd_head,sdf_export_ecc_pub_key **p);


#endif /* INCLUDE_SDF_H_ */
