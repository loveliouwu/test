
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include <sys/time.h>
#include <time.h>
#include "socket_client.h"
#include "SDF_Interface.h"
#include "sdf_common.h"

static int test_times = 10000;
static int test_data_len = 2048;


int test_success[SOCKET_NUM];

void *sm4_test(void *session_handle)
{
    session_list *psession_handle = session_handle;


    unsigned int uiAlgID = SGD_SM4_ECB;
    unsigned int recv_len;

    unsigned char input_f[4096] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	unsigned char key[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	unsigned char iv[16] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};
	unsigned char output_f[4096],compare[32];
	unsigned long i;

	unsigned char *input;
	unsigned char *output;
	unsigned int block_num=2;
	printf("=======================ecb=======================\n");
	//encrypt ecb
	input = input_f;
	output= output_f;
    memset(input_f,9,4096);
    
    
    int loop_times = test_times;
    int send_len = test_data_len;
    struct timeval start,end;
	double speed;
    gettimeofday( &start, NULL );
	for(i=0;i<loop_times;i++)
	{
        
		if(0!=SDF_Encrypt(psession_handle,key,uiAlgID,iv,input,send_len,output,&recv_len))
        {
            printf("%x error*****************************\n",session_handle);
            break;
        }
        
        //printf("i=%d,%x\n",i,session_handle);
	}
    gettimeofday( &end, NULL );
    int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    printf("+++++++++sm4_ecb time:%d us\n", timeuse);
    printf("end\n");


   /* clock_t start_t,end_t;

	start_t=clock(); 
    printf("sm4_ecb start:%lf\n", (double)start); 
    for(i=0;i<loop_times;i++)
	{
		SDF_Encrypt(psession_handle,key,uiAlgID,iv,input,2048,output,&recv_len);
	}
    end_t=clock();
	printf("sm4_ecb end:%lf\n", (double)end);
	speed = (double)(end_t-start_t)/CLOCKS_PER_SEC;
	printf("sm4_ecb time:%lfs\n", (double)speed);
	printf("sm4_ecb speed:%lfMbps\n", (double)100000*(SOCKET_NUM-1)*2*8/speed/1024);
    printf("end\n");*/
   /* SDF_Encrypt(psession_handle,key,uiAlgID,iv,input,block_num*16,output,&recv_len);
		for(i=0;i<16*block_num;i++){
		  printf("%02x ", output_f[i]);
		}
		printf("\n");
     
    SDF_Decrypt(psession_handle,key,uiAlgID,iv,output,block_num*16,compare,&recv_len);
		for(i=0;i<16*block_num;i++){
		  printf("%02x ", compare[i]);
		}
	printf("\n");


	printf("=======================cbc=======================\n");
    uiAlgID = SGD_SM4_CBC;
	//encrypt cbc
	input = input_f;
	output= output_f;
    SDF_Encrypt(psession_handle,key,uiAlgID,iv,input,block_num*16,output,&recv_len);
		for(i=0;i<16*block_num;i++){
		  printf("%02x ", output[i]);
		}
		printf("\n");
    SDF_Decrypt(psession_handle,key,uiAlgID,iv,output,block_num*16,compare,&recv_len);
		for(i=0;i<16*block_num;i++){
		  printf("%02x ", compare[i]);
		}
	printf("\n");*/
    
}


void *sm3test(void *session_handle)
{
    int ret = SDR_OK;
    void *session_new;
    unsigned int data_len = test_data_len;
    unsigned char data_in[4096] = {0x61,0x62,0x63};
    unsigned char data_out[32];
    unsigned int data_out_len;
    //初始化阶段
    memset(data_in,8,data_len);
    struct timeval start,end;
	double speed;
    int loop_times = test_times;
    int send_len = test_data_len;
    int i = 0;
    gettimeofday( &start, NULL );
    for(i=0;i<loop_times;i++)
    {
        ret = SDF_HashInit(session_handle);
        if(ret != SDR_OK)
        {
            printf("HashInit error!\n");
            break;
        }
        // printf("In Data:\n");
        // for(i=0;i<data_len;i++)
        // {
        //     printf("%02X ",data_in[i]);
        // }
        // printf("\n");

        
        // ret = SDF_Hash_Copy(session_handle,&session_new);
        // if(ret != 0)
        // {
        //     printf("hash copy error!\n");
        //     exit(-1);
        // }
    //-----------1
        ret = SDF_HashUpdate(session_handle,data_in,data_len);
        if(ret != SDR_OK)
        {
            printf("HashUpdate error!\n");
            break;
        }
        ret = SDF_HashFinal(session_handle,data_out,&data_out_len);
        if(ret != SDR_OK)
        {
            printf("HashFinal is error!\n");
            break;
        }
    }
    
    gettimeofday( &end, NULL );
    int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    printf("+++++++++sm4_ecb time:%d us\n", timeuse);

//-----------2
    // ret = SDF_HashUpdate(session_new,data_in,data_len);
    // if(ret != SDR_OK)
    // {
    //     printf("HashUpdate error!\n");
    //     exit(-1);
    // }


    // ret = SDF_HashFinal(session_new,data_out,&data_out_len);
    // if(ret != SDR_OK)
    // {
    //     printf("HashFinal is error!\n");
    //     exit(-1);
    // }
    // // printf("Out Data:\n");
    // // for(i=0;i<data_out_len;i++)
    // // {
    // //     printf("%02X ",data_out[i]);
    // // }
    // // printf("\n");
    
    // ret = SDF_CloseSession(session_new);
    // if(ret != 0)
    // {
    //     printf("closesession error!\n");
    //     exit(-1);
    // }

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
    unsigned int uiISKIndex_1;
    unsigned int uiISKIndex_2;
    unsigned int uiISKIndex_3;
    unsigned int uiKeyBits = 128;
    void *phAgreementHandle = NULL;
    unsigned char pswd_1[32];
    unsigned char pswd_2[32];
    unsigned char pswd_3[PIN_MAX_LEN];
    unsigned int pwd_len = 8;
    ECCrefPublicKey sponsorpub,sponsortmppub;
    ECCrefPublicKey responsorpub,responsortmppub;
    unsigned char sponsorID[15] = "zyxxsponsortest";
    unsigned int sponsorIDLen = 15;
    unsigned char responsorID[17] = "zyxxresponsortest";
    unsigned int responsorIDLen = 17;
    unsigned char sk[128];
    unsigned int sk_len;

    memset(pswd_3,8,pwd_len);
    ret = SDF_GenerateKeyPair_ECC(session_handle,&uiISKIndex_1,pswd_1,pwd_len);
    if(ret != SDR_OK)
    {
        printf("GenerateKeyPair error!\n");
        exit(-1);
    }
    ret = SDF_GenerateKeyPair_ECC(session_handle,&uiISKIndex_2,pswd_2,pwd_len);
    if(ret != SDR_OK)
    {
        printf("GenerateKeyPair error!\n");
        exit(-1);
    }

    struct timeval start,end;
	double speed;
    int loop_times = test_times;
    int send_len = test_data_len;
    int i = 0;
    gettimeofday( &start, NULL );
    for(i=0;i<loop_times;i++)
    {

        ret = SDF_GenerateAgreementDataWithECC(session_handle,uiISKIndex_1,pswd_1,pwd_len,uiKeyBits,sponsorID,sponsorIDLen,&sponsorpub,&sponsortmppub,&phAgreementHandle);
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

        ret = SDF_GenerateKeyWith_ECC(session_handle,responsorID,responsorIDLen,&responsorpub,&responsortmppub,phAgreementHandle,sk,&sk_len);
        if(ret != 0)
        {
            printf("GenerateKeyWith_ECC error!\n");
            exit(-1);
        }

    }
    gettimeofday( &end, NULL );
    int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    printf("+++++++++sm4_ecb time:%d us\n", timeuse);


    ret = SDF_DeleteKeyPair_ECC(session_handle,&uiISKIndex_1,pswd_1,pwd_len);
    if(ret != 0)
    {
        printf("delete key error!\n");
    }
    ret = SDF_DeleteKeyPair_ECC(session_handle,&uiISKIndex_2,pswd_2,pwd_len);
    if(ret != 0)
    {
        printf("delete key error!\n");
    }

}

void *sign_verify_test(void *session_handle)
{
    unsigned int uiISKIndex = 1;
    unsigned char pswd[16] = "ALI";
    unsigned int pswd_len = 3;
    unsigned char sign_data[32] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    ECCSignature signature;
    int ret = 0;

    struct timeval start,end;
	double speed;
    int loop_times = test_times;
    int send_len = test_data_len;
    int i = 0;

    gettimeofday( &start, NULL );
    for(i = 0;i<3;i++)
    {
        ret = SDF_GenerateKeyPair_ECC(session_handle,&uiISKIndex,pswd,pswd_len);
        if(ret != SDR_OK)
        {
            printf("GenerateKeyPair error! %d\n",i);
            exit(-1);
        }
    }
    gettimeofday( &end, NULL );
    int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    printf("+++++++++sm2_generate_key_pair time:%d us\n", timeuse);
    
    gettimeofday( &start, NULL );

    for(i = 0;i<loop_times;i++)
    {
        ret = SDF_Sign_ECC(session_handle,uiISKIndex,pswd,pswd_len,sign_data,32,&signature);
        if(ret != SDR_OK)
        {
            printf("sign error!\n");
            exit(-1);
        }
    }

    gettimeofday( &end, NULL );
    timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    printf("+++++++++sm2_sign time:%d us\n", timeuse);

    ECCrefPublicKey pubkey;
    unsigned char data[64];
    unsigned int data_len = 16;
    loop_times = 10000;
    gettimeofday( &start, NULL );
    for(i = 0;i<loop_times;i++)
    {
        ret = SDF_ExtVerify_ECC(session_handle,&pubkey,data,data_len,&signature);
        if(ret != SDR_OK)
        {
            printf("verify error! %x\n",ret);
            exit(-1);
        }
        else
        {
            printf("times %d\n",i);
        }
        
    }
    gettimeofday( &end, NULL );
    timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    printf("+++++++++sm2_verify time:%d us\n", timeuse);

    ret = SDF_DeleteKeyPair_ECC(session_handle,&uiISKIndex,pswd,pswd_len);
    if(ret != SDR_OK)
    {
        printf("GenerateKeyPair error!\n");
        exit(-1);
    }
}

void *thread_test(void *psession_handle)
{
    //sm4_test(psession_handle);
    //sign_verify_test(psession_handle);
    sm3test(psession_handle);
    //pthread_exit(0);
}

int main()
{
    int i = 0;
    int ret = 0;
    unsigned char buff[256];
    void *pdev_handle;
    void *psession_handle[1024];
    pthread_t thread_id[SOCKET_NUM];


    printf("SOCKET_NUM=%d\n",SOCKET_NUM);

    ret = SDF_OpenDevice(&pdev_handle);
    if(ret != 0)
    {
        printf("open device error!\n");
        exit(1);
    }

    for(i=1;i<SOCKET_NUM;i++)
    {
        ret = SDF_OpenSession(pdev_handle,&psession_handle[i]);
        printf("i=%d,ret=%d\n",i,ret);
    } 

    //sm3test(psession_handle[1]);
    // sign_verify_test(psession_handle[1]);
    // sm4_test(psession_handle[1]);
    // return 0;

    for(i=1;i<SOCKET_NUM;i++)
    {
        pthread_create(&thread_id[i],NULL,thread_test,psession_handle[i]);
    }  
    for(i=1;i<SOCKET_NUM;i++)
    {
        pthread_join(thread_id[i],NULL);
    }

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
