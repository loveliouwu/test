
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
    int time = 1;
    unsigned char dec_data[128];
    int phKeyHandle = 1;
    unsigned int uiAlgID = SGD_SM4_ECB;
    unsigned char pucIV[16];
    unsigned int recv_len;
    unsigned char enc_data[128];
    unsigned char dec_dec_data[128];
    unsigned int dec_dec_data_len;
    int j = 0;
    while(time--)
    {
        memset(pucIV,6,16);
        for(j=0;j<128/16;j++)
        {
            int i = 0;
            for(i=0;i<16;i++)
            {
                enc_data[i+j*16] = j;
            }
        }
            
        printf("--------thread_fd_index = %d\n",psession_handle->sess.socket_fd);
        SDF_Encrypt(psession_handle,&phKeyHandle,uiAlgID,pucIV,enc_data,128,dec_data,&recv_len);
        printf("Encrypt :\n");
        for(j=0;j<recv_len;j++)
        {
            printf("%02x ",dec_data[j]);
        }
        printf("\n");
        printf("--------%d\n",psession_handle->sess.socket_fd);


        memset(pucIV,6,16);
        int ret = SDF_Decrypt(psession_handle,
            &phKeyHandle,
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
        printf("Decrypt :\n");
        for(j=0;j<dec_dec_data_len;j++)
        {
            printf("%02x ",dec_dec_data[j]);
        }
        printf("\n");
        pthread_exit(NULL);
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


    ret = SDF_OpenDevice(&pdev_handle);
    if(ret != 0)
    {
        printf("open device error!\n");
        exit(1);
    }

    for(i=1;i<SOCKET_NUM;i++)
    {
        ret = SDF_OpenSession(pdev_handle,&psession_handle[i]);
    
        printf("ssid %d\n",i);
        pthread_create(&thread_id[i],NULL,sm4_test,psession_handle[i]);
    }
    
    //sm4_test(psession_handle[1]);
    sleep(2);
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