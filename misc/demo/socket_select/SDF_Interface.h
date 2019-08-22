#ifndef SDF_INTERFACE_H_
#define SDF_INTERFACE_H_

#define SDR_BASE                0x01000000
#define SDR_OK                  0x0
#define SDR_INANGERR            SDR_BASE + 1
#define SDR_OUTARGERR           SDR_BASE + 2
#define SDR_OPENDEVICE          SDR_BASE + 3
#define SDR_MALLOCERR           SDR_BASE + 4
#define SDR_OPENSESSION         SDR_BASE + 5
#define SDR_UNKNOWERR           SDR_BASE + 6
#define SDR_ALGNOTSUPPORT       SDR_BASE + 7
#define SDR_COMMFAIL            SDR_BASE + 8
#define SDR_KEYNOTEXIST         SDR_BASE + 9
#define SDR_PARDENY             SDR_BASE + 10

#define SGD_SM4_ECB             0x00000401
#define SGD_SM4_CBC             0x00000402

#define ECCref_MAX_BITS         (512)
#define ECCref_MAX_LEN          ((ECCref_MAX_BITS+7)/8)

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

int GenerateAgreementDataWithECC(void *hSessionHandle,
                                unsigned int uiISKIndex, 
                                unsigned char *PassWord,
                                unsigned int uiKeyBits,
                                unsigned char *pucSponsorID,
                                unsigned int uiSponsorIDLength,
                                ECCrefPublicKey *pucSponsorPublicKey,
                                ECCrefPublicKey *pucSponsorTmpPublicKey,
                                void *phAgreementHandle);

int GenerateAgreementDataAndKeyWithECC(void *hSessionHandle,
                                unsigned int uiISKIndex, 
                                unsigned char *PassWord,
                                unsigned int uiKeyBits,
                                unsigned char *pucResponsorID,
                                unsigned int uiResponsorIDLength,
                                unsigned char *pucSponsorID,
                                unsigned int uiSponsorIDLength,
                                ECCrefPublicKey *pucSponsorPublicKey,
                                ECCrefPublicKey *pucSponsorTmpPublicKey,
                                ECCrefPublicKey *pucResponsorPublicKey,
                                ECCrefPublicKey *pucResponsorTmpPublicKey,
                                void *phKey);

int GenerateKeyPair_ECC(void *hSessionHandle,unsigned int *index, unsigned char *password, unsigned int *length);

int Sign_ECC(void *hSessionHandle,
            unsigned int uiISKIndex,
            unsigned char *PassWord,
            unsigned char *pucData,
            unsigned int uiDataLength,
            ECCSignature *pucSignature
            );

int Verfiy_ECC(void *hSessionHandle,
                unsigned char *pucData,
                unsigned int uiDataLength,
                ECCSignature *pucSignature);


void test(void *hSessionHanlde);



#endif