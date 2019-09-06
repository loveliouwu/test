
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>

#include "socket_client.h"
#include "SDF_Interface.h"
#include "sdf_common.h"



void *sm4_test(void *session_handle)
{
    session_list *psession_handle = session_handle;

    unsigned char dec_data[128];
    unsigned char phKeyHandle[16];
    unsigned int uiAlgID = SGD_SM4_ECB;
    unsigned char pucIV[16];
    unsigned int recv_len;
    unsigned char enc_data[128];
    unsigned char dec_dec_data[128];
    unsigned int dec_dec_data_len;
    int j = 0;

    memset(pucIV,6,16);
    for(j=0;j<128/16;j++)
    {
        int i = 0;
        for(i=0;i<16;i++)
        {
            enc_data[i+j*16] = j;
        }
    }
        
    //printf("--------thread_fd_index = %d\n",psession_handle->sess.socket_fd);
    SDF_Encrypt(psession_handle,phKeyHandle,uiAlgID,pucIV,enc_data,128,dec_data,&recv_len);
    // printf("Encrypt :\n");
    // for(j=0;j<recv_len;j++)
    // {
    //     printf("%02x ",dec_data[j]);
    // }
    // printf("\n");
    //printf("--------%d\n",psession_handle->sess.socket_fd);


    memset(pucIV,6,16);
    int ret = SDF_Decrypt(psession_handle,
        phKeyHandle,
        uiAlgID,
        pucIV,
        dec_data,
        recv_len,
        dec_dec_data,
        &dec_dec_data_len);
    if(ret != SDR_OK)
    {
        printf("SDF_Decrypt error!\n");
    }
    // printf("Decrypt :\n");
    // for(j=0;j<dec_dec_data_len;j++)
    // {
    //     printf("%02x ",dec_dec_data[j]);
    // }
    // printf("\n");
    
}


void *sm3test(void *session_handle)
{
    int i;
    int ret = SDR_OK;
    void *session_new;
    unsigned int data_len = 3;
    unsigned char data_in[3] = {0x61,0x62,0x63};
    unsigned char data_out[data_len];
    unsigned int data_out_len;
    //初始化阶段
    ret = SDF_HashInit(session_handle);
    if(ret != SDR_OK)
    {
        printf("HashInit error!\n");
        exit(-1);
    }
    // printf("In Data:\n");
    // for(i=0;i<data_len;i++)
    // {
    //     printf("%02X ",data_in[i]);
    // }
    // printf("\n");

    
    ret = SDF_Hash_Copy(session_handle,&session_new);
    if(ret != 0)
        printf("hash copy error!\n");


//-----------1
    ret = SDF_HashUpdate(session_handle,data_in,data_len);
    if(ret != SDR_OK)
    {
        printf("HashUpdate error!\n");
        exit(-1);
    }


    ret = SDF_HashFinal(session_handle,data_out,&data_out_len);
    if(ret != SDR_OK)
    {
        printf("HashFinal is error!\n");
        exit(-1);
    }
    
//-----------2
    ret = SDF_HashUpdate(session_new,data_in,data_len);
    if(ret != SDR_OK)
    {
        printf("HashUpdate error!\n");
        exit(-1);
    }


    ret = SDF_HashFinal(session_new,data_out,&data_out_len);
    if(ret != SDR_OK)
    {
        printf("HashFinal is error!\n");
        exit(-1);
    }
    // printf("Out Data:\n");
    // for(i=0;i<data_out_len;i++)
    // {
    //     printf("%02X ",data_out[i]);
    // }
    // printf("\n");
    
    ret = SDF_CloseSession(session_new);


}

void *generaterandom_test(void *session_handle)
{
    int ret = SDR_OK;
    int i = 0;
    unsigned int buffsize = 512;
    unsigned char buff[buffsize];
    memset(buff,0,buffsize);
    unsigned int data_len;

    data_len = 2;
    ret = SDF_GenerateRandom(session_handle,data_len,buff);
    if(ret != SDR_OK)
    {
        printf("getrandom error!\n");
        exit(-1);
    }
    printf("Get random %d\n",data_len);
    for(i=0;i<data_len;i++)
    {
        printf("%02X ",buff[i]);
    }
    printf("\n");

    data_len = 5;
    ret = SDF_GenerateRandom(session_handle,data_len,buff);
    if(ret != SDR_OK)
    {
        printf("getrandom error!\n");
        exit(-1);
    }
    printf("Get random %d\n",data_len);
    for(i=0;i<data_len;i++)
    {
        printf("%02X ",buff[i]);
    }
    printf("\n");

    data_len = 16;
    ret = SDF_GenerateRandom(session_handle,data_len,buff);
    if(ret != SDR_OK)
    {
        printf("getrandom error!\n");
        exit(-1);
    }
    printf("Get random %d\n",data_len);
    for(i=0;i<data_len;i++)
    {
        printf("%02X ",buff[i]);
    }
    printf("\n");

    data_len = 512;
    ret = SDF_GenerateRandom(session_handle,data_len,buff);
    if(ret != SDR_OK)
    {
        printf("getrandom error!\n");
        exit(-1);
    }
    printf("Get random %d\n",data_len);
    for(i=0;i<data_len;i++)
    {
        printf("%02X ",buff[i]);
    }
    printf("\n");

}

void *generate_agreement_data_and_key_test(void *session_handle)
{
    int ret = 0;
    unsigned int uiISKIndex_1 = 0;
    unsigned int uiISKIndex_2 = 1;
    unsigned int uiKeyBits = 128;
    void *phAgreementHandle = NULL;
    unsigned char pswd_1[3] = {0x68,0x65,0x6c};
    unsigned char pswd_2[3] = "ALI";
    unsigned int pwd_len = 3;
    ECCrefPublicKey sponsorpub,sponsortmppub;
    ECCrefPublicKey responsorpub,responsortmppub;
    unsigned char sponsorID[15] = "zyxxsponsortest";
    unsigned int sponsorIDLen = 15;
    unsigned char responsorID[17] = "zyxxresponsortest";
    unsigned int responsorIDLen = 17;
    unsigned char sk[128];
    unsigned int sk_len;

    // ret = GenerateKeyPair_ECC(session_handle,&uiISKIndex_1,pswd_1,pwd_len);
    // if(ret != SDR_OK)
    // {
    //     printf("GenerateKeyPair error!\n");
    //     exit(-1);
    // }

    ret = SDF_GenerateAgreementDataWithECC(session_handle,uiISKIndex_1,uiKeyBits,sponsorID,sponsorIDLen,&sponsorpub,&sponsortmppub,&phAgreementHandle);
    if(ret != SDR_OK)
    {
        printf("GenerateAgreementDataWithECC is error!\n");
        exit(-1);
    }
    ret = SDF_GenerateAgreementDataAndKeyWithECC(session_handle,uiISKIndex_2,pswd_2,pwd_len,uiKeyBits,responsorID,responsorIDLen,sponsorID,sponsorIDLen,&sponsorpub,&sponsortmppub,&responsorpub,&responsortmppub,sk,&sk_len);
    if(ret != 0)
    {
        printf("GenerateAgreementDataAndKeyWithECC is error!\n");
        exit(-1);
    }

    ret = SDF_GenerateKeyWith_ECC(session_handle,pswd_1,pwd_len,responsorID,responsorIDLen,&responsorpub,&responsortmppub,phAgreementHandle,sk,&sk_len);
    if(ret != 0)
    {
        printf("GenerateKeyWith_ECC error!\n");
        exit(-1);
    }

}

void *sign_verify_test(void *session_handle)
{
    unsigned int uiISKIndex = 1;
    unsigned char pswd[] = "ALI";
    unsigned int pswd_len = 3;
    unsigned char sign_data[32] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    ECCSignature signature;

    int ret = SDF_Sign_ECC(session_handle,uiISKIndex,pswd,pswd_len,sign_data,32,&signature);
    if(ret != SDR_OK)
    {
        printf("sign error!\n");
        exit(-1);
    }

    ret = SDF_Verfiy_ECC(session_handle,sign_data,32,&signature);
    if(ret != SDR_OK)
    {
        printf("verify error!\n");
        exit(-1);
    }


}

void *thread_test(void *psession_handle)
{
    int time = 2;
    while(time)
    {
        sm3test(psession_handle);
        sm4_test(psession_handle);
        sign_verify_test(psession_handle);
        generate_agreement_data_and_key_test(psession_handle);
    }
    

}

void main()
{
    int i = 0;
    int ret = 0;
    unsigned char buff[256];
    void *pdev_handle;
    void *psession_handle[1024];
    pthread_t thread_id[SOCKET_NUM];

 printf("sizeof(Gen_agreement_para %d\n)",sizeof(Gen_agreemen_para));

    ret = SDF_OpenDevice(&pdev_handle);
    if(ret != 0)
    {
        printf("open device error!\n");
        exit(1);
    }

   
    // for(i=1;i<SOCKET_NUM;i++)
    // {
    //     ret = SDF_OpenSession(pdev_handle,&psession_handle[i]);
    
    //     printf("ssid %d\n",i);
    //     pthread_create(&thread_id[i],NULL,thread_test,psession_handle[i]);
    // }
    SDF_OpenSession(pdev_handle,&psession_handle[1]);
    //sm3test(psession_handle[1]);
    //sm4_test(psession_handle[1]);
    //sign_verify_test(psession_handle[1]);
    generate_agreement_data_and_key_test(psession_handle[1]);
    //(thread_id[1],NULL);
    for(i=1;i<SOCKET_NUM;i++)
    {
        ret = SDF_CloseSession(psession_handle[i]);
    }

    
 


    ret = SDF_CloseDevice(pdev_handle);
    if(ret != 0)
    {
        printf("CloseDevice err!\n");
    }

}