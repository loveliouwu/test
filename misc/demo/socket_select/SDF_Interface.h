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
#define SDR_FILEEXISTS 			SDR_BASE+0x000000lA	
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

#define SGD_SF33_ECB    		0x00000201				
#define SGD_SF33_CBC    		0x00000202				
#define SGD_SF33_CFB    		0x00000204				
#define SGD_SF33_OFB   		 	0x00000208			
#define SGD_SF33_MAC    		0x00000210		

#define SGD_SM4_ECB   			0x00000401				
#define SGD_SM4_CBC   			0x00000402				
#define SGD_SM4_CFB   			0x00000404				
#define SGD_SM4_OFB   			0x00000408			
#define SGD_SM4_MAC   			0x00000410			

#define SGD_ZUC_EEA3   			0x00000801				
#define SGD_ZUC_EIA3   			0x00000802				


#define	SGD_SM3 				0x00000001			
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



int SDF_OpenDevice(void **phDeviceHandle);

int SDF_CloseDevice(void *phDeviceHandle);

int SDF_OpenSession(void *phDeviceHandle,void **phSessionHandle);

int SDF_CloseSession(void *phSessionHandle);

int SDF_Encrypt(void *hSessionHanlde,void *phKeyHandle,
                unsigned int uiAlgID,
                unsigned char *pucIV,
                unsigned char *pucData,
                unsigned int uiDataLength,
                unsigned char *pucEncData,
                unsigned int *puiEncDatalength);

int SDF_Decrypt(void *hSessionHandle,
                void *phKeyHandle,
                unsigned int uiAlgID,
                unsigned char *pucIV,
                unsigned char *pucEncData,
                unsigned int puiEncDatalength,
                unsigned char *pucData,
                unsigned int *puiDatalength);

int HashInit(void *hSessionHandle);

int HashUpdate(void *hSessionHandle,unsigned char *pucData,unsigned int uiDataLength);

int HashFinal(void *hSessionHandle,unsigned char *pucData,unsigned int *puiHashLength);

int GenerateRandom(void *hSessionHandle,unsigned int uiLength,unsigned char *pucRandom);


/*
Function:GenerateKeyWith_ECC
Description:使用ECC秘钥协商算法，使用自身协商举兵和相应方的协商参数
            计算会话秘钥，同时返回会话秘钥
Ａrgs:      [in]    hSessionHandle:会话句柄
        
*/
int GenerateKeyWith_ECC(void *hSessionHandle,
                        unsigned char *pucResponseID,
                        unsigned int uiResponseIDLength,
                        ECCrefPublicKey *pucResponsePublicKey,
                        ECCrefPublicKey *pucResponseTmpPublicKey,
                        void *phAgreementHandle,
                        void **phKey);

int GenerateAgreementDataWithECC(void *hSessionHandle,
                                unsigned int uiISKIndex, 
                                unsigned char *PassWord,
                                unsigned int uiKeyBits,
                                unsigned char *pucSponsorID,
                                unsigned int uiSponsorIDLength,
                                ECCrefPublicKey *pucSponsorPublicKey,
                                ECCrefPublicKey *pucSponsorTmpPublicKey,
                                void **phAgreementHandle);

int GenerateAgreementDataAndKeyWithECC(void *hSessionHandle,
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
                                void **phKey);

//输入password,length，输出索引
int GenerateKeyPair_ECC(void *hSessionHandle,unsigned int *index, unsigned char *password, unsigned int length);



int Sign_ECC(void *hSessionHandle,
            unsigned int uiISKIndex,
            unsigned char *PassWord,
            unsigned int  passwordlen,
            unsigned char *pucData,
            unsigned int uiDataLength,
            ECCSignature *pucSignature
            );

int Verfiy_ECC(void *hSessionHandle,
                unsigned char *pucData,
                unsigned int uiDataLength,
                ECCSignature *pucSignature);

int SDF_GetPrivateKeyAccessRight(
    void *hSessionHandle,
    unsigned int uiKeyIndex,
    unsigned char * pucPassword,
    unsigned int uiPwdLength
);

int SDF_ReleasePrivateKeyAccessRight(
    void *hSessionHandle,
    unsigned int uiKeyIndex
);


int SDF_ExportSignPublicKey_ECC(
    void * hSessionHandle,
    unsigned int uiKeyIndex,
    ECCrefPublicKey * pucPublicKey
);

int SDF_ExportEncPublicKey_ECC(
    void *hSessionHandle,
    unsigned int uiKeyIndex,
    ECCrefPublicKey *pucPublicKey
);

int SDF_GenerateKeyWithIPK_ECC(
    void * hSessionHandle,
    unsigned int uiIPKIndex,
    unsigned int uiKeyBits,
    ECCCipher * pucKey,
    void ** phKeyHandle
);

int ExtVerify_ECC(
    void *hSessionHandle,
    ECCrefPublicKey *pucPublicKey,
    unsigned char *pucDataInput,
    unsigned int uiInputLength,
    ECCSignature *pucSignature
);

int Destroy_session_key(
    void *hSessionHandle,
    void *phKeyHandle
);

void test(void *hSessionHanlde);



#endif