#ifndef SDF_INTERFACE_H_
#define SDF_INTERFACE_H_

#define SDR_OK	0x0 									
#define SDR_BASE				0x01000000 				
#define SDR_UNKNOWERR			SDR_BASE+0x00000001		
#define SDR_NOTSUPPORT			SDR_BASE+0x00000002		
#define SDR_COMMFAIL 			SDR_BASE+0x00000003		
#define SDR_HARDFAIL 			SDR_BASE+0x00000004		
#define SDR_OPENDEVICE 			SDR_BASE+0x00000005		
#define SDR_OPENSESSION 		SDR_BASE+0x00000006		
#define SDR_PARDENY				SDR_BASE+0x00000007		
#define SDR_KEYNOTEXIST			SDR_BASE+0x00000008		
#define SDR_ALGNOTSUPPORT 		SDR_BASE+0x00000009		
#define SDR_ALGMODNOTSUPPORT 	SDR_BASE+0x0000000A		
#define SDR_PKOPERR 			SDR_BASE+0x0000000B		
#define SDR_SKOPERR 			SDR_BASE+0x0000000C		
#define SDR_SIGNERR				SDR_BASE+0x0000000D 	
#define SDR_VERIFYERR 			SDR_BASE+0x0000000E		
#define SDR_SYMOPERR 			SDR_BASE+0x0000000F		
#define SDR_STEPERR 			SDR_BASE+0x00000010		
#define SDR_FILESIZEERR 		SDR_BASE+0x00000011		
#define SDR_FILENOEXIST			SDR_BASE+0x00000012 	
#define SDR_FILEOFSERR			SDR_BASE+0x00000013		
#define SDR_KEYTYPEERR			SDR_BASE+0x00000014		
#define SDR_KEYERR 				SDR_BASE+0x00000015		
#define SDR_ENCDATAERR			SDR_BASE+0x00000016		
#define SDR_RANDERR				SDR_BASE+0x00000017		
#define SDR_PRKRERR				SDR_BASE+0x00000018		
#define SDR_MACERR 				SDR_BASE+0x00000019		
#define SDR_FILEEXISTS 			SDR_BASE+0x0000001A	
#define SDR_FILEWERR			SDR_BASE+0x0000001B 	
#define SDR_NOBUFFER 			SDR_BASE+0x0000001C		
#define SDR_INARGERR			SDR_BASE+0x0000001D		
#define SDR_OUTARGERR 			SDR_BASE+0x0000001E	

#define SGD_SM2					0x00020100				
#define SGD_SM2_1				0x00020200				
#define SGD_SM2_2				0x00020400				
#define SGD_SM2_3				0x00020800				

#define SGD_SM1_ECB   			0x00000101					
#define SGD_SM1_CBC   			0x00000102				
#define SGD_SM1_CFB   			0x00000104			
#define SGD_SM1_OFB   			0x00000108				
#define SGD_SM1_MAC   			0x00000110				

#define SGD_SF33_ECB    		    0x00000201				
#define SGD_SF33_CBC    		    0x00000202				
#define SGD_SF33_CFB    		    0x00000204				
#define SGD_SF33_OFB   		 	0x00000208			
#define SGD_SF33_MAC    		    0x00000210		

#define SGD_SM4_ECB   			0x00000401				
#define SGD_SM4_CBC   			0x00000402				
#define SGD_SM4_CFB   			0x00000404				
#define SGD_SM4_OFB   			0x00000408			
#define SGD_SM4_MAC   			0x00000410			

#define SGD_ZUC_EEA3   			0x00000801				
#define SGD_ZUC_EIA3   			0x00000802				


#define	SGD_SM3 				    0x00000001			
#define	SGD_SHA1 				0x00000002				
#define	SGD_SHA256 				0x00000004				

#define ECCref_MAX_BITS 		512
#define ECCref_MAX_LEN  		((ECCref_MAX_BITS+7)/8)


/*ECC公钥结构体*/
typedef struct ECCrefPublicKey_st{
    unsigned int bits;
    unsigned char x[ECCref_MAX_LEN];
    unsigned char y[ECCref_MAX_LEN];
}ECCrefPublicKey;

/*ECC签名数据结构体*/
typedef struct ECCSignature_st{
    unsigned char r[ECCref_MAX_LEN];
    unsigned char s[ECCref_MAX_LEN];
}ECCSignature;

typedef struct ECCCipher_st
{
	unsigned char x[ECCref_MAX_LEN];
	unsigned char y[ECCref_MAX_LEN];
	unsigned char M[32];
	unsigned int  L;
	unsigned char C[1];
}ECCCipher;


/*****************************************************************************************************************
 * private F function: 打开设备
 * args:    [out] phDeviceHandle: 输出的设备句柄
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_OpenDevice(void **phDeviceHandle);

/*****************************************************************************************************************
 * private F function: 关闭设备
 * args:    [in] phDeviceHandle: 输入的设备句柄
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_CloseDevice(void *phDeviceHandle);

/*****************************************************************************************************************
 * private F function: 打开会话
 * args:    [in] phDeviceHandle: 输出的设备句柄
* args:    [out] phSessionHandle: 输出的会话句柄
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_OpenSession(void *phDeviceHandle,void **phSessionHandle);

/*****************************************************************************************************************
 * private F function: 关闭会话
 * args:    [in] phSessionHandle: 输入的会话句柄
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_CloseSession(void *phSessionHandle);

/*****************************************************************************************************************
 * private F function: 对称加密
 * args:    [in] phSessionHandle: 输入的会话句柄
 * args:    [in] Key: 对称16B密钥地址
 * args:    [in] uiAlgID: 算法ID
 * args:    [in] pucIV: 16B IV数据地址
 * args:    [in] pucData: 待加密数据地址
 * args:    [in] uiDataLength: 待加密数据长度
 * args:    [out] pucEncData: 密文数据地址
 * args:    [out] puiEncDatalength: 密文数据长度
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_Encrypt(void *hSessionHanlde,
                unsigned char *Key,//16字节秘钥的地址
                unsigned int uiAlgID,//算法ID
                unsigned char *pucIV,//IV，16字节
                unsigned char *pucData,//in 加密原数据
                unsigned int uiDataLength,//in  加密数据长度
                unsigned char *pucEncData,//out 加密密文存放地址
                unsigned int *puiEncDatalength//out 密文长度存放地址
                );

/*****************************************************************************************************************
 * private F function: 对称解密
 * args:    [in] phSessionHandle: 输入的会话句柄
 * args:    [in] Key: 对称16B密钥地址
 * args:    [in] uiAlgID: 算法ID
 * args:    [in] pucIV: 16B IV数据地址
 * args:    [in] pucData: 待加密数据地址
 * args:    [in] uiDataLength: 待加密数据长度
 * args:    [out] pucEncData: 密文数据地址
 * args:    [out] puiEncDatalength: 密文数据长度
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_Decrypt(void *hSessionHandle,
                unsigned char *Key,//16字节秘钥的地址
                unsigned int uiAlgID,
                unsigned char *pucIV,
                unsigned char *pucEncData,//in
                unsigned int puiEncDatalength,//in
                unsigned char *pucData,//out
                unsigned int *puiDatalength//out
                );

/*****************************************************************************************************************
 * private F function: HASH运算第一步
 * args:    [in] hSessionHandle: 输入的会话句柄
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_HashInit(void *hSessionHandle);

/*****************************************************************************************************************
 * private F function: HASH运算第二步
 * args:    [in] hSessionHandle: 输入的会话句柄
 * args:    [in] pucData: 输入的数据地址
 * args:    [in] uiDataLength: 输入数据长度
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_HashUpdate(void *hSessionHandle,unsigned char *pucData,unsigned int uiDataLength);

/*****************************************************************************************************************
 * private F function: HASH运算第三步
 * args:    [in] hSessionHandle: 输入的会话句柄
 * args:    [out] pucData: 输出的数据地址
 * args:    [out] puiHashLength: 输出数据长度
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_HashFinal(void *hSessionHandle,unsigned char *pucData,unsigned int *puiHashLength);

/*****************************************************************************************************************
 * private F function: HASH CTX复制，可节省部分hash运算
 * args:    [in] hSessionHandle: 输入的会话句柄
 * args:    [out] hSessionHandle_out: 输出的会话句柄
 * eg:
第一步：打开设备
第二步：打开会话1->hash_init（会话1）->hash_update（会话1）->hash_copy（会话1，新会话）->hash_finial（会话1）->关闭会话1
第三步：hash_finial（新会话）->关闭新会话
第四步：关闭设备
第二步和第三步输出值应一致。
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_Hash_Copy(void *hSessionHandle,void **hSessionHandle_out);

/*****************************************************************************************************************
 * private F function: 获取随机数
 * args:    [in] hSessionHandle: 输入的会话句柄
 * args:    [in] uiLength: 输入数据长度
 * args:    [out] pucRandom: 输出随机数数据
注释：未做大包处理，根据socket单包传输大小进行测试
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_GenerateRandom(void *hSessionHandle,unsigned int uiLength,unsigned char *pucRandom);


/*****************************************************************************************************************
* private F function: 计算会话密钥
 * args:    [in] hSessionHandle :会话句柄
 * args:    [in] pucResponseID:响应方ID
 * args:    [in] uiResponseIDLength :响应方ID长度
 * args:    [in] pucResponsePublicKey :响应方公钥
 * args:    [in] pucResponseTmpPublicKey:响应方临时公钥
 * args:    [in] phAgreementHandle: 密钥协商参数时候得到的密钥协商句柄
 * args:    [out] out_session_key:会话密钥
 * args:    [out] out_session_key_len:会话密钥长度
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_GenerateKeyWith_ECC(void *hSessionHandle,
                        unsigned char *pucResponseID,
                        unsigned int uiResponseIDLength,
                        ECCrefPublicKey *pucResponsePublicKey,
                        ECCrefPublicKey *pucResponseTmpPublicKey,
                        void *phAgreementHandle,
                        unsigned char *session_key,
                        unsigned int *session_key_len
                        );


/*****************************************************************************************************************
 * private F function: 生成密钥协商参数并输出
 * args:    [in] hSessionHandle :会话句柄
 * args:    [in] uiISKIndex :密钥索引
 * args:    [in] PassWord:密钥私钥保护口令
 * args:    [in] password_len:密钥私钥保护口令长度
 * args:    [in] uiKeyBits :协商密钥位数（0-128B,8的倍数）
 * args:    [in] pucSponsorID:发起方ID
 * args:    [in] uiSponsorIDLength :发起方ID长度
 * args:    [out] pucSponsorPublicKey :发起方公钥
 * args:    [out] pucSponsorTmpPublicKey:发起方临时公钥
 * args:    [out] phAgreementHandle: 密钥协商句柄
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_GenerateAgreementDataWithECC(void *hSessionHandle,
                                unsigned int uiISKIndex,
                                unsigned char *PassWord,
                                unsigned int password_len,
                                unsigned int uiKeyBits,
                                unsigned char *pucSponsorID,
                                unsigned int uiSponsorIDLength,
                                ECCrefPublicKey *pucSponsorPublicKey,
                                ECCrefPublicKey *pucSponsorTmpPublicKey,
                                void **phAgreementHandle);

/*****************************************************************************************************************
* private F function: 产生协商数据并计算会话密钥
 * args:    [in] hSessionHandle :会话句柄
 * args:    [in] uiISKIndex :密钥索引
 * args:    [in] PassWord:密钥私钥保护口令
 * args:    [in] password_len:密钥私钥保护口令长度
 * args:    [in] uiKeyBits :协商密钥位数（0-128B,8的倍数）
 * args:    [in] pucResponsorID:响应方ID
 * args:    [in] uiResponsorIDLength :响应方ID长度
 * args:    [in] pucSponsorID:发起方ID
 * args:    [in] uiSponsorIDLength :发起方ID长度
 * args:    [in] pucSponsorPublicKey :发起方公钥
 * args:    [in] pucSponsorTmpPublicKey:发起方临时公钥
 * args:    [out] pucResponsorPublicKey :响应方公钥
 * args:    [out] pucResponsorTmpPublicKey:响应方临时公钥
 * args:    [out] session_key:会话密钥
 * args:    [out] session_key_len:会话密钥长度
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_GenerateAgreementDataAndKeyWithECC(void *hSessionHandle,
                                unsigned int uiISKIndex, 
                                unsigned char *PassWord,
                                unsigned int password_len,
                                unsigned int uiKeyBits,
                                unsigned char *pucResponsorID,
                                unsigned int uiResponsorIDLength,
                                unsigned char *pucSponsorID,
                                unsigned int uiSponsorIDLength,
                                ECCrefPublicKey *pucSponsorPublicKey,
                                ECCrefPublicKey *pucSponsorTmpPublicKey,
                                ECCrefPublicKey *pucResponsorPublicKey,
                                ECCrefPublicKey *pucResponsorTmpPublicKey,
                                unsigned char *session_key,
                                unsigned int *session_key_len);

/*****************************************************************************************************************
 * private F function: 产生ECC密钥对并输出
 * args:    [in] hSessionHandle :会话句柄
 * args:    [out] index :生成的密钥索引
 * args:    [in] password :私钥保护口令
 * args:    [in] password_len :私钥保护口令长度
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_GenerateKeyPair_ECC(void *hSessionHandle,unsigned int *index, unsigned char *password, unsigned int password_len);

/*****************************************************************************************************************
 * private F function: 使用内部ECC私钥对数据进行签名运算
 * args:    [in] hSessionHandle :会话句柄
 * args:    [in] uiISKIndex :密钥索引
 * args:    [in] PassWord :私钥保护口令
 * args:    [in] passwordlen :私钥保护口令长度
 * args:    [in] pucData :待签名数据
 * args:    [in] uiDataLength :待签名数据长度
 * args:    [out] pucSignature: 签名值
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_Sign_ECC(void *hSessionHandle,
            unsigned int uiISKIndex,//秘钥索引
            unsigned char *PassWord,//密码
            unsigned int  passwordlen,//密码长度
            unsigned char *pucData,//in 
            unsigned int uiDataLength,//in 待签名数据长度
            ECCSignature *pucSignature//out 签名值
            );

/*****************************************************************************************************************
 * private F function: 使用外部ECC公钥对数据进行验证运算
 * args:    [in] hSessionHandle :会话句柄
 * args:    [in] pucPublicKey :输入外部公钥
 * args:    [in] pucDataInput :输入hash e值地址
 * args:    [in] uiInputLength :输入hash e值数据长度
 * args:    [in] pucSignature:签名数据
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_ExtVerify_ECC(
    void *hSessionHandle,
    ECCrefPublicKey *pucPublicKey,
    unsigned char *pucDataInput,
    unsigned int uiInputLength,
    ECCSignature *pucSignature
);

/*****************************************************************************************************************
 * private F function: 导出ECC签名公钥
 * args:    [in] hSessionHandle :会话句柄
 * args:    [in] uiKeyIndex :密钥索引
 * args:    [out] pucPublicKey  :输出公钥
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_ExportSignPublicKey_ECC(
    void * hSessionHandle,
    unsigned int uiKeyIndex,
    ECCrefPublicKey * pucPublicKey
);

/*****************************************************************************************************************
 * private F function: 导出ECC加密公钥
 * args:    [in] hSessionHandle :会话句柄
 * args:    [in] uiKeyIndex :密钥索引
 * args:    [out] pucPublicKey  :输出公钥
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_ExportEncPublicKey_ECC(
    void *hSessionHandle,
    unsigned int uiKeyIndex,
    ECCrefPublicKey *pucPublicKey
);


/*****************************************************************************************************************
 * private F function: 销毁ECC密钥
 * args:    [in] hSessionHandle :会话句柄
 * args:    [in] index :生成的密钥索引
 * args:    [in] password :私钥保护口令
 * args:    [in] password_len :私钥保护口令长度
 * return: SDF返回值
 ******************************************************************************************************************/
int SDF_DeleteKeyPair_ECC(void *hSessionHandle,unsigned int *index, unsigned char *password, unsigned int length);


#endif