#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>


#include"SDF_Interface.h"
#include"socket_client.h"
#include"sdf_common.h"

#define COMMON_DEBUG_LOG
#ifdef COMMON_DEBUG_LOG
#define COMMON_DEBUG(fmt,arg...)        printf("[DEBUG %s:%s:%d]:"fmt,__FILE__,__func__,__LINE__,##arg)
#else 
#define COMMON_DEBUG(fmt,arg...)        
#endif
#define COMMON_ERROR(fmt,arg...)        printf("[ERR %s:%s:%d]:"fmt,__FILE__,__func__,__LINE__,##arg)


int SDF_OpenDevice(void **phDeviceHandle)
{
    unsigned int ret;
    packet_head *packet;
    device *pdev_handle;

    if(!phDeviceHandle)
    {
        COMMON_ERROR("phDeviceHandle is NULL");
        return SDR_OUTARGERR;
    }

    pdev_handle = (device*)malloc(sizeof(device));
    if(pdev_handle == NULL)
    {
        COMMON_ERROR("malloc device handle error\n");
        return SDR_OPENDEVICE;
    }
    memset(pdev_handle,0,sizeof(device));
    pdev_handle->control_send_buff = (unsigned char *)malloc(MAX_SEND_BUFF_LEN);
    if(pdev_handle->control_send_buff == NULL)
    {
        COMMON_ERROR("malloc device_send_buff handle error\n");
        free(pdev_handle);
        return SDR_OPENDEVICE;
    }
    memset(pdev_handle->control_send_buff,0,MAX_SEND_BUFF_LEN);
    pdev_handle->control_recv_buff = (unsigned char *)malloc(MAX_SEND_BUFF_LEN);
    if(pdev_handle->control_recv_buff == NULL)
    {
        COMMON_ERROR("malloc device_recv_buff handle error\n");
        free(pdev_handle->control_send_buff);
        free(pdev_handle);
        return SDR_OPENDEVICE;
    }
    memset(pdev_handle->control_recv_buff,0,MAX_SEND_BUFF_LEN);
    ret = socket_init(pdev_handle);
    if(ret != 0)
    {
        COMMON_ERROR("socket init error\n");
        socket_exit(pdev_handle);
        free(pdev_handle->control_send_buff);
        free(pdev_handle->control_recv_buff);
        free(pdev_handle);
        return SDR_OPENDEVICE;
    }
    memset(pdev_handle->fd_flag,0,SOCKET_NUM);//初始化socket_fd是否使用的标记
    pdev_handle->session_list_head = NULL;//初始化会话链表的头和尾节点
    pdev_handle->session_list_end = NULL;

    //通过pdev_handle->socketfd[0]获取设备GUID
    packet = (packet_head*)(pdev_handle->control_send_buff);
    packet->major_cmd = control_cmd;
    packet->minor_cmd = OPEN_DEVICE;
    packet->len = sizeof(packet_head);
    ret = socket_control_send(pdev_handle,packet->len);

    
    packet = (packet_head*)(pdev_handle->control_recv_buff);
    COMMON_DEBUG("major_cmd:%d,\nminor_cmd:%d,\nlen:%d,\nread_len:%d\n",packet->major_cmd,packet->minor_cmd,packet->len,ret);
    if((packet->status != 0) || (ret <= 0))
    {
        COMMON_ERROR("get deivce GUID error status = %d\n",packet->status);
        socket_exit(pdev_handle);
        free(pdev_handle->control_send_buff);
        free(pdev_handle->control_recv_buff);
        free(pdev_handle);
        return SDR_OPENDEVICE;
    }
    //memcpy((char *)&(pdev_handle->GUID),(char *)&(packet->device_guid),sizeof(guid));
    pdev_handle->GUID = packet->device_guid;
    *phDeviceHandle = (void*)pdev_handle;

    return SDR_OK;
}

int SDF_CloseDevice(void *phDeviceHandle)
{
    unsigned int ret;
    packet_head *packet;
    device *pdev_handle;
    if(!phDeviceHandle)
    {
        return SDR_INANGERR;
    }
    pdev_handle = (device *)phDeviceHandle;
    packet = (packet_head*)(pdev_handle->control_recv_buff);
    packet->major_cmd = control_cmd;
    packet->minor_cmd = CLOSE_DEVICE;
    packet->len = sizeof(packet_head);
    ret = socket_control_send(pdev_handle,packet->len);
    if((packet->status != 0) || (ret <= 0))
    {
        COMMON_ERROR("release deivce GUID error status = %d\n",packet->status);
        socket_exit(pdev_handle);
        free(pdev_handle->control_send_buff);
        free(pdev_handle->control_recv_buff);
        free(pdev_handle);
        return SDR_UNKNOWERR;
    }
    socket_exit(pdev_handle);//释放socket

    if(pdev_handle->session_list_head != NULL)//如果有未关闭的会话，释放会话资源
    {
        session_list *p_session_list = NULL; 
        session_list *p_session_list_next = NULL;
        p_session_list = pdev_handle->session_list_head;
        while(p_session_list != NULL)
        {
            
            p_session_list_next = p_session_list->next;
            free(p_session_list->sess.recv_buff);
            free(p_session_list->sess.send_buff);
            free(p_session_list);
            if(p_session_list_next != NULL)
                p_session_list = p_session_list_next;
            else
                p_session_list = NULL;
        }
        pdev_handle->session_list_head = NULL;
        pdev_handle->session_list_end = NULL;
    }

    
    free(pdev_handle->control_send_buff);//释放发送接收buffer
    free(pdev_handle->control_recv_buff);
    free(pdev_handle);

    return SDR_OK;
}

int SDF_OpenSession(void *phDeviceHandle,void **phSessionHandle)
{
    session_list *psession_handle;
    packet_head *packet;
    device *pdev_handle;
    int i = 0;
    int ret = 0;
    if(!phDeviceHandle)
    {
        COMMON_ERROR("phDeviceHandle is NULL\n");
        return SDR_INANGERR;
    }
    pdev_handle = (device *)phDeviceHandle;
    psession_handle = (session_list *)malloc(sizeof(session_list));
    if(!psession_handle)
    {
        COMMON_ERROR("psession_handle is NULL\n");
        psession_handle = NULL;
        return SDR_MALLOCERR;
    }
    memset(psession_handle,0,sizeof(session_list));

    //申请数据BUFF用于接收和发送数据
    psession_handle->sess.send_buff = malloc(MAX_SEND_BUFF_LEN);
    if(psession_handle->sess.send_buff == NULL)
    {
        printf("malloc send_buff handle error\n");
        free(psession_handle);
        return SDR_MALLOCERR;
    }
    memset(psession_handle->sess.send_buff,0,MAX_SEND_BUFF_LEN);
    psession_handle->sess.recv_buff = malloc(MAX_SEND_BUFF_LEN);
    if(psession_handle->sess.recv_buff == NULL)
    {
        printf("malloc recv_buff handle error\n");
        free(psession_handle->sess.send_buff);
        free(psession_handle);
        return SDR_MALLOCERR;
    }
    memset(psession_handle->sess.recv_buff,0,MAX_SEND_BUFF_LEN);
    packet = (packet_head*)(pdev_handle->control_send_buff);
    packet->major_cmd = control_cmd;
    packet->minor_cmd = OPEN_SESSION;
    packet->status = 0;
    packet->len = sizeof(packet_head);
    packet->device_guid = pdev_handle->GUID;

    ret = socket_control_send(pdev_handle,packet->len);  
    packet = (packet_head*)(pdev_handle->control_recv_buff);
    COMMON_DEBUG("major_cmd:%d,\nminor_cmd:%d,\nlen:%d,\nread_len:%d\n",packet->major_cmd,packet->minor_cmd,packet->len,ret);
    if((packet->status != 0) || (ret <= 0))
    {
        COMMON_ERROR("get session handle error status = %d\n",packet->status);
        free(psession_handle->sess.recv_buff);
        free(psession_handle->sess.send_buff);
        free(psession_handle);
        return SDR_OPENSESSION;
    }
    //i=1
    for(i = 1;i<SOCKET_NUM;i++)//寻找未用到的socket绑定到会话
    {
        if(pdev_handle->fd_flag[i] == 0)
        {
            pdev_handle->fd_flag[i] = 1;
            psession_handle->sess.socket_fd = i;//保存索引
            break;
        }
    }
    if(i == SOCKET_NUM)
    {
        COMMON_ERROR("create session_list error\n");
        free(psession_handle->sess.recv_buff);
        free(psession_handle->sess.send_buff);
        free(psession_handle);
        return SDR_OPENSESSION;
    }


    //维护会话链表
    if(pdev_handle->session_list_head == NULL)
    {
        psession_handle->next = NULL;
        psession_handle->prev = NULL;
        pdev_handle->session_list_head = psession_handle;
        pdev_handle->session_list_head->next = NULL;
        pdev_handle->session_list_head->prev = NULL;
        pdev_handle->session_list_end = pdev_handle->session_list_head;
    }
    else
    {
        psession_handle->prev = pdev_handle->session_list_end;
        psession_handle->next = NULL;
        pdev_handle->session_list_end->next = psession_handle;
        pdev_handle->session_list_end = psession_handle;
    }
    psession_handle->sess.session_handle = packet->server_session_handle;//保存服务端会话句柄
    psession_handle->sess.pdev_handle = (void*)pdev_handle;
    psession_handle->sess.Dev_GUID = pdev_handle->GUID;
    *phSessionHandle = (void *)psession_handle;
    return SDR_OK;
}

int SDF_CloseSession(void *phSessionHandle)
{
    session_list *psession_handle;
    packet_head *packet;
    device *pdev_handle;
    int ret = 0; 

    if(!phSessionHandle)
    {
        COMMON_ERROR("psession_handle is NULL\n");
        psession_handle = NULL;
        return SDR_MALLOCERR;
    }
    psession_handle = (session_list*)phSessionHandle;
    packet = (packet_head*)psession_handle->sess.send_buff;
    packet->major_cmd = control_cmd;
    packet->minor_cmd = CLOSE_SESSION;
    packet->status = 0;
    packet->len = sizeof(packet_head);
    packet->server_session_handle = psession_handle->sess.session_handle;
    ret = socket_send(psession_handle,packet->len);
    packet = (packet_head*)psession_handle->sess.recv_buff;
    if((packet->status != 0) || (ret < 0))
    {
        ret = packet->status;
        COMMON_ERROR("release session error status = %d\n",packet->status);
        free(psession_handle->sess.recv_buff);
        free(psession_handle->sess.send_buff);
        free(psession_handle);
        return ret;
    }
    pdev_handle = psession_handle->sess.pdev_handle;
    pdev_handle->fd_flag[psession_handle->sess.socket_fd] = 0;//清除设备fd_flag使用标记
    
    //维护会话链表
    if(psession_handle->prev == NULL)//如果是头结点
    {
        if(pdev_handle->session_list_end == pdev_handle->session_list_head)
        {
            pdev_handle->session_list_end = psession_handle->next;
        }
        pdev_handle->session_list_head = psession_handle->next;
        if(pdev_handle->session_list_head != NULL)//如果不止有一个结点，需要修改新头结点的Prev指向NULL
        {
            pdev_handle->session_list_head->prev = NULL;
        }
        
    }
    else
    {
        if(psession_handle->next == NULL)//如果是尾结点
        {
            pdev_handle->session_list_end = psession_handle->prev;
            pdev_handle->session_list_end->next = NULL;
        }
        else
        {
            psession_handle->prev->next = psession_handle->next;
            psession_handle->next->prev = psession_handle->prev; 
        }
        
    }
    
    free(psession_handle->sess.recv_buff);
    free(psession_handle->sess.send_buff);
    free(psession_handle);
    return SDR_OK;
}


void test(void *hSessionHanlde)
{
    packet_head *packet;
    session_list *psession_handle;
    char buff[128];
    memset(buff,88,128);

    psession_handle = (session_list*)hSessionHanlde;
    packet = (packet_head *)(psession_handle->sess.send_buff);
    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->len = sizeof(packet_head) + 128;
    packet->server_session_handle = psession_handle->sess.session_handle;
    packet->minor_cmd = 4;
    memcpy((void *)packet+sizeof(packet_head),buff,128);
    socket_send(psession_handle,packet->len);
    packet = (packet_head*)psession_handle->sess.recv_buff;
}


int SDF_Encrypt(void *hSessionHanlde,void *phKeyHandle,unsigned int uiAlgID,unsigned char *pucIV,unsigned char *pucData,unsigned int uiDataLength,unsigned char *pucEncData,unsigned int *puiEncDatalength)
{
    packet_head *packet;
    session_list *psession_handle;
    symalg_cipher enc_data;
    int ret = SDR_OK;
    int current_offset,data_len,remain_len;
    unsigned char IV[IV_LENGHT];

    if(!hSessionHanlde)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INANGERR;
    }
    if(!phKeyHandle)
    {
        COMMON_ERROR("phKeyHandle is NULL!\n");
        return SDR_INANGERR;
    }
    if((uiAlgID != SGD_SM4_ECB) && (uiAlgID != SGD_SM4_CBC))
    {
        COMMON_ERROR("Input uiAlgID Error!\n");
        return SDR_ALGNOTSUPPORT;
    }
    if(!pucIV)
    {
        COMMON_ERROR("pucIV is NULL!\n");
        return SDR_INANGERR;
    }
    if(!pucData)
    {
        COMMON_ERROR("pucData is NULL!\n");
        return SDR_INANGERR;
    }
    if(uiDataLength == 0)
    {
        COMMON_ERROR("uiDataLength is 0!\n");
        return SDR_INANGERR;
    }
    if(uiDataLength & SM4_KEY_LEN)
    {
        COMMON_ERROR("uiDataLength is not 16 multiple!\n");
        return SDR_INANGERR;
    }
    if(!pucEncData)
    {
        COMMON_ERROR("pucEncData id NULL!\n");
        return SDR_OUTARGERR;
    }
    if(!puiEncDatalength)
    {
        COMMON_ERROR("puiEncDatalength is NULL!\n");
        return SDR_OUTARGERR;
    }
    psession_handle = (session_list*)hSessionHanlde;
    packet = (packet_head*)psession_handle->sess.send_buff;
    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = 4;//SYM_ENCRYPT;
    packet->status = SDR_OK;
    packet->server_session_handle = psession_handle->sess.session_handle;
    memcpy(IV,pucIV,IV_LENGHT);
    remain_len = uiDataLength;
    current_offset = 0;
    while(remain_len > 0)
    {
        if(remain_len <= MAX_SEND_BUFF_LEN - sizeof(packet_head) - sizeof(symalg_cipher))
        {
            data_len = remain_len;
        }
        else
        {
            data_len = MAX_SEND_BUFF_LEN - sizeof(packet_head) - sizeof(symalg_cipher);         
        }
        packet->len = data_len + sizeof(symalg_cipher);
        enc_data.key_handle = *(unsigned int *)phKeyHandle;
        enc_data.data_len = data_len;
        enc_data.alg_id = uiAlgID;
        //需要保存IV以供下一包使用
        memcpy(enc_data.IV,IV,IV_LENGHT);
        memcpy((char *)packet + sizeof(packet_head),&enc_data,sizeof(enc_data));
        memcpy((char *)packet + sizeof(packet_head) + sizeof(symalg_cipher),pucData + current_offset,data_len);
        ret = socket_send(psession_handle,packet->len + sizeof(packet_head));
        packet = (packet_head*)psession_handle->sess.recv_buff;
        if(ret <= 0)
        {
            COMMON_ERROR("socket error!\n");
            return SDR_COMMFAIL;
        }
        if(packet->status != SDR_OK)
        {
            return packet->status;
        }
        memcpy(pucEncData + current_offset,(char *)packet + sizeof(packet_head) + sizeof(symalg_cipher) + current_offset, data_len);
        memcpy(IV,((symalg_cipher*)((char *)(psession_handle->sess.recv_buff) + sizeof(packet_head)))->IV,IV_LENGHT);
        remain_len -= data_len;
        current_offset += data_len; 
    }
    *puiEncDatalength = current_offset;
    
    return packet->status;

}


int SDF_Decrypt(void *hSessionHandle,
            void *phKeyHandle,
            unsigned int uiAlgID,
            unsigned char *pucIV,
            unsigned char *pucEncData,
            unsigned int puiEncDatalength,
            unsigned char *pucData,
            unsigned int *puiDatalength)
{
    int ret = SDR_OK;
    packet_head *packet,*packet_out;
    symalg_cipher dec_data;
    session_list *psession_handle;
    int current_offset,data_len,remain_len;
    unsigned char IV[IV_LENGHT];

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INANGERR;
    }
    if(!phKeyHandle)
    {
        COMMON_ERROR("phKeyHandle is NULL!\n");
        return SDR_INANGERR;
    }
    if((uiAlgID != SGD_SM4_ECB) && (uiAlgID != SGD_SM4_CBC))
    {
        COMMON_ERROR("Input uiAlgID Error!\n");
        return SDR_ALGNOTSUPPORT;
    }
    if(!pucIV)
    {
        COMMON_ERROR("pucIV is NULL!\n");
        return SDR_INANGERR;
    }
    if(!pucData)
    {
        COMMON_ERROR("pucData is NULL!\n");
        return SDR_INANGERR;
    }
    if(puiEncDatalength == 0)
    {
        COMMON_ERROR("puiEncDatalength is 0!\n");
        return SDR_INANGERR;
    }
    if(puiEncDatalength & SM4_KEY_LEN)
    {
        COMMON_ERROR("puiEncDatalength is not 16 multiple!\n");
        return SDR_INANGERR;
    }
    if(!pucEncData)
    {
        COMMON_ERROR("pucEncData id NULL!\n");
        return SDR_OUTARGERR;
    }
    if(!puiDatalength)
    {
        COMMON_ERROR("puiDatalength is NULL!\n");
        return SDR_OUTARGERR;
    }
    psession_handle = (session_list *)hSessionHandle;
    packet = (packet_head*)psession_handle->sess.send_buff;
    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = SYM_DECRYPT;
    packet->status = SDR_OK;
    packet->server_session_handle = psession_handle->sess.session_handle;
    
    memcpy(IV,pucIV,IV_LENGHT);
    remain_len = puiEncDatalength;
    current_offset = 0;
    while(remain_len > 0)
    {
        if(remain_len < MAX_SEND_BUFF_LEN - sizeof(symalg_cipher) - sizeof(packet_head))
        {
            data_len = remain_len;
        }
        else
        {
            data_len = MAX_SEND_BUFF_LEN - sizeof(symalg_cipher) - sizeof(packet_head);
        }
        dec_data.alg_id = uiAlgID;
        dec_data.data_len = data_len;
        dec_data.key_handle = *(unsigned int *)phKeyHandle;
        memcpy(dec_data.IV,IV,IV_LENGHT);

        packet->len = data_len + sizeof(symalg_cipher);
        memcpy(packet + sizeof(packet_head) + sizeof(symalg_cipher),pucEncData + current_offset, data_len);
        ret = socket_send(psession_handle,packet->len + sizeof(packet_head));
        packet_out = (packet_head*)psession_handle->sess.recv_buff;
        if(ret <= 0)
        {
            COMMON_ERROR("socket error!\n");
            return SDR_COMMFAIL;
        }
        if(packet_out->status != SDR_OK)
        {
            return packet_out->status;
        }
        memcpy(pucData + current_offset,(char *)packet_out + sizeof(packet_head) + current_offset, data_len);
        memcpy(IV,((symalg_cipher*)((char *)(psession_handle->sess.recv_buff) + sizeof(packet_head)))->IV,IV_LENGHT);
        puiDatalength += data_len;
        remain_len -= data_len;
        current_offset += data_len; 
    }
    memcpy(pucIV,IV,IV_LENGHT);
    return packet_out->status;
}



int HashInit(void *hSessionHandle)
{
    int ret = SDR_OK;
    packet_head *packet;
    session_list *psession_handle;
    
    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INANGERR;
    }

    psession_handle = (session_list *)hSessionHandle;
    packet = (packet_head *)psession_handle->sess.send_buff;
    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->server_session_handle = psession_handle->sess.session_handle;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = HASH_INIT;
    packet->len = sizeof(packet_head);

    ret = socket_send(psession_handle,packet->len);
    packet = (packet_head *)psession_handle->sess.recv_buff;
    if(ret <= 0 || packet->status != SDR_OK)
    {
        COMMON_ERROR("hash_init error!\n");
        return SDR_COMMFAIL;
    }

    return SDR_OK;
}

int HashUpdate(void *hSessionHandle,unsigned char *pucData,unsigned int uiDataLength)
{
    int ret = SDR_OK;
    packet_head *packet,*packet_out;
    session_list *psession_handle;
    unsigned int data_len,offset,current_len;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INANGERR;
    }
    if(!pucData)
    {
        COMMON_ERROR("The pucData is NULL!\n");
        return SDR_INANGERR;
    }
    if(uiDataLength == 0)
    {
        COMMON_ERROR("The uiDataLength is error!\n");
        return SDR_INANGERR;
    }
    psession_handle = (session_list *)hSessionHandle;
    packet = (packet_head *)psession_handle->sess.send_buff;
    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = HASH_UPDATE;
    packet->server_session_handle = psession_handle->sess.session_handle;
    data_len = uiDataLength;
    offset = 0;
    while(data_len>0)
    {
        if(data_len > MAX_SEND_BUFF_LEN - sizeof(packet_head))
        {
            current_len = MAX_SEND_BUFF_LEN - sizeof(packet_head);
            packet->len = MAX_SEND_BUFF_LEN;
        }
        else
        {
            current_len = data_len;
            packet->len = sizeof(packet_head) + data_len;
        }
        memcpy(packet + sizeof(packet_head),pucData + offset,current_len);
        ret = socket_send(psession_handle,packet->len);
        packet_out = (packet_head *)psession_handle->sess.send_buff;
        if(ret <= 0 || packet_out->status != SDR_OK)
        {
            COMMON_ERROR("HashUpdata send error!\n");
            return SDR_COMMFAIL;
        }
        data_len -= current_len;
        offset += current_len;
    }
    return SDR_OK;
}


int HashFinal(void *hSessionHandle,unsigned char *pucData,unsigned int *puiHashLength)
{
    int ret = SDR_OK;
    packet_head *packet;
    session_list *psession_handle;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INANGERR;
    }
    if(!pucData)
    {
        COMMON_ERROR("The pucData is NULL!\n");
        return SDR_OUTARGERR;
    }
    if(!puiHashLength)
    {
        COMMON_ERROR("The puiHashLength is NULL!\n");
        return SDR_OUTARGERR;
    }
    psession_handle = (session_list *)hSessionHandle;
    packet = (packet_head *)psession_handle->sess.send_buff;
    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->server_session_handle = psession_handle->sess.session_handle;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = HASH_FINAL;
    packet->len = sizeof(packet_head);
    ret = socket_send(psession_handle,packet->len);
    packet = (packet_head *)psession_handle->sess.recv_buff;
    if(packet->status != SDR_OK || ret <= 0)
    {
        COMMON_ERROR("HashFinal error!\n");
        return SDR_COMMFAIL;
    }
    *puiHashLength = packet->len;
    memcpy(pucData,(char *)packet + sizeof(packet_head),*puiHashLength);
    return SDR_OK;
}

int GenerateRandom(void *hSessionHandle,unsigned int uiLength,unsigned char *pucRandom)
{
    int ret = SDR_OK;
    packet_head *packet;
    session_list *psession_handle;
    GetRandom * prandom;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INANGERR;
    }
    if(uiLength == 0 || uiLength > MAX_RANDOM_LEN)
    {
        COMMON_ERROR("The uiLenght is error!\n");
        return SDR_INANGERR;
    }
    if(!pucRandom)
    {
        COMMON_ERROR("The pucRandom is NULL!\n");
        return SDR_OUTARGERR;
    }

    psession_handle = (session_list*)hSessionHandle;
    packet = (packet_head *)psession_handle->sess.send_buff;
    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = GET_RANDOM;
    packet->len = sizeof(packet_head) + sizeof(GetRandom);
    packet->server_session_handle = psession_handle->sess.session_handle;
    prandom = (GetRandom *)((char *)packet + sizeof(packet_head));
    prandom->RandomLen = uiLength;

    ret = socket_send(psession_handle,packet->len);
    packet = (packet_head *)psession_handle->sess.recv_buff;
    if( ret<=0 || packet->status != SDR_OK)
    {
        return packet->status;
    }
    prandom = (GetRandom *)((char *)packet + sizeof(packet_head));
    memcpy(pucRandom,prandom->data,uiLength);
    return SDR_OK;
}

int GenerateAgreementDataWithECC(void *hSessionHandle,
                                unsigned int uiISKIndex, 
                                unsigned char *PassWord,
                                unsigned int uiKeyBits,
                                unsigned char *pucSponsorID,
                                unsigned int uiSponsorIDLength,
                                ECCrefPublicKey *pucSponsorPublicKey,
                                ECCrefPublicKey *pucSponsorTmpPublicKey,
                                void *phAgreementHandle)
{
    int ret = SDR_OK;
    packet_head *packet;
    session_list *psession_handle;
    Gen_agreemen_para gereement_para;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INANGERR;
    }
    if(uiISKIndex >= MAX_KEY_INDEX)
    {
        COMMON_ERROR("Input uiISKIndex more than the max index !\n");
        return SDR_KEYNOTEXIST;
    }
    if(uiISKIndex == 0)
    {
        COMMON_ERROR("N0.0 key is not allowed!\n");
        return SDR_PARDENY;
    }
    if(!PassWord)
    {
        COMMON_ERROR("PassWord is NULL!\n");
        return SDR_INANGERR;
    }
    if(uiKeyBits > SM4_BITS || uiKeyBits == 0)
    {
        COMMON_ERROR("Input uiKeyBits Err!\n");
        return SDR_INANGERR;
    }
    if(uiKeyBits % 8)
    {
        COMMON_ERROR("Input uiKeuBits Err!\n");
        return SDR_INANGERR;
    }
    if(!pucSponsorID)
    {
        COMMON_ERROR("pucSponsorID is NULL!\n");
        return SDR_INANGERR;
    }
    if(uiSponsorIDLength > MAX_ID_LEN)
    {
        COMMON_ERROR("Input uiSponsorIDLength too long!\n");
        return SDR_INANGERR;
    }
    if(uiSponsorIDLength == 0)
    {
        COMMON_ERROR("Input uiSponsorIDLength is 0!\n");
        return SDR_INANGERR;
    }
    if(!pucSponsorPublicKey)
    {
        COMMON_ERROR("pucSponsorPublicKey is NULL!\n");
        return SDR_OUTARGERR;
    }
    if(!pucSponsorTmpPublicKey)
    {
        COMMON_ERROR("pucSponsorTmpPublickey is NULL!\n");
        return SDR_OUTARGERR;
    }
    if(!phAgreementHandle)
    {
        COMMON_ERROR("phAgreement is NULL!\n");
        return SDR_OUTARGERR;
    }
    


}



