
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
    int j = 0;
    if(psession_handle->sess.socket_fd <= 0)
        exit(1);
    while(time--)
    {
        
        for(j=0;j<128/16;j++)
        {
            int i = 0;
            for(i=0;i<16;i++)
            {
                enc_data[i+j*16] = j;
            }
        }
        

        SDF_Encrypt(psession_handle,&phKeyHandle,uiAlgID,pucIV,enc_data,128,dec_data,&recv_len);
        printf("\n");
        for(j=0;j<recv_len;j++)
        {
            printf("%02x ",dec_data[j]);
        }
        printf("\n");
        printf("--------%d\n",psession_handle->sess.socket_fd);
    }
    pthread_exit(NULL);
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


    for(i=0;i<SOCKET_NUM - 1;i++)//最多打开１９９个会话，因为有一个socket负责传输命令
    {
        ret = SDF_OpenSession(pdev_handle,&psession_handle[i]);
        if(ret != 0)
        {
            printf("open session err!  %d\n",i);
            break;
        }
            
        printf("ssid %d\n",i);
        pthread_create(&thread_id[i],NULL,sm4_test,psession_handle[i]);
    }


    int ss=0;
    printf("input->");
    scanf("%d",&ss);

    for(i=SOCKET_NUM - 2;i>=0;i--)
    {
        ret = SDF_CloseSession(psession_handle[i]);
        if(ret != 0)
            continue;
    }


    ret = SDF_CloseDevice(pdev_handle);
    if(ret != 0)
    {
        printf("CloseDevice err!\n");
    }

}