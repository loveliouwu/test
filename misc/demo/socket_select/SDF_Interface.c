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
        return SDR_INARGERR;
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
        return SDR_INARGERR;
    }
    pdev_handle = (device *)phDeviceHandle;
    psession_handle = (session_list *)malloc(sizeof(session_list));
    if(!psession_handle)
    {
        COMMON_ERROR("psession_handle is NULL\n");
        psession_handle = NULL;
        return SDR_UNKNOWERR;
    }
    memset(psession_handle,0,sizeof(session_list));

    //申请数据BUFF用于接收和发送数据
    psession_handle->sess.send_buff = malloc(MAX_SEND_BUFF_LEN);
    if(psession_handle->sess.send_buff == NULL)
    {
        printf("malloc send_buff handle error\n");
        free(psession_handle);
        return SDR_UNKNOWERR;
    }
    memset(psession_handle->sess.send_buff,0,MAX_SEND_BUFF_LEN);
    psession_handle->sess.recv_buff = malloc(MAX_SEND_BUFF_LEN);
    if(psession_handle->sess.recv_buff == NULL)
    {
        printf("malloc recv_buff handle error\n");
        free(psession_handle->sess.send_buff);
        free(psession_handle);
        return SDR_UNKNOWERR;
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
        return SDR_UNKNOWERR;
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
        // free(psession_handle->sess.recv_buff);
        // free(psession_handle->sess.send_buff);
        // free(psession_handle);
        // return ret;
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





int SDF_Encrypt(void *hSessionHanlde,unsigned char *Key,unsigned int uiAlgID,unsigned char *pucIV,unsigned char *pucData,unsigned int uiDataLength,unsigned char *pucEncData,unsigned int *puiEncDatalength)
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
        return SDR_INARGERR;
    }
    if(!Key)
    {
        COMMON_ERROR("Key is NULL!\n");
        return SDR_INARGERR;
    }
    if((uiAlgID != SGD_SM4_ECB) && (uiAlgID != SGD_SM4_CBC))
    {
        COMMON_ERROR("Input uiAlgID Error!\n");
        return SDR_ALGNOTSUPPORT;
    }
    if(!pucIV)
    {
        COMMON_ERROR("pucIV is NULL!\n");
        return SDR_INARGERR;
    }
    if(!pucData)
    {
        COMMON_ERROR("pucData is NULL!\n");
        return SDR_INARGERR;
    }
    if(uiDataLength == 0)
    {
        COMMON_ERROR("uiDataLength is 0!\n");
        return SDR_INARGERR;
    }
    if(uiDataLength % SM4_KEY_LEN)
    {
        COMMON_ERROR("uiDataLength is not 16 multiple!\n");
        return SDR_INARGERR;
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
        packet->len = data_len + sizeof(symalg_cipher) + sizeof(packet_head);
        memcpy(enc_data.key_handle,Key,16);
        //enc_data.key_handle = *(unsigned int *)phKeyHandle;
        enc_data.data_len = data_len;
        enc_data.alg_id = uiAlgID;
        //需要保存IV以供下一包使用
        memcpy(enc_data.IV,IV,IV_LENGHT);
        memcpy((char *)packet + sizeof(packet_head),&enc_data,sizeof(enc_data));
        memcpy((char *)packet + sizeof(packet_head) + sizeof(symalg_cipher),pucData + current_offset,data_len);
        ret = socket_send(psession_handle,packet->len);
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
            unsigned char *Key,
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
        return SDR_INARGERR;
    }
    if(!Key)
    {
        COMMON_ERROR("Key is NULL!\n");
        return SDR_INARGERR;
    }
    if((uiAlgID != SGD_SM4_ECB) && (uiAlgID != SGD_SM4_CBC))
    {
        COMMON_ERROR("Input uiAlgID Error!\n");
        return SDR_ALGNOTSUPPORT;
    }
    if(!pucIV)
    {
        COMMON_ERROR("pucIV is NULL!\n");
        return SDR_INARGERR;
    }
    if(!pucData)
    {
        COMMON_ERROR("pucData is NULL!\n");
        return SDR_INARGERR;
    }
    if(puiEncDatalength == 0)
    {
        COMMON_ERROR("puiEncDatalength is 0!\n");
        return SDR_INARGERR;
    }
    if(puiEncDatalength & SM4_KEY_LEN)
    {
        COMMON_ERROR("puiEncDatalength is not 16 multiple!\n");
        return SDR_INARGERR;
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
        memcpy(dec_data.key_handle,Key,16);
        memcpy(dec_data.IV,IV,IV_LENGHT);

        packet->len = data_len + sizeof(symalg_cipher) + sizeof(packet_head);
        memcpy((char *)packet + sizeof(packet_head) + sizeof(symalg_cipher),(char *)pucEncData + current_offset, data_len);
        ret = socket_send(psession_handle,packet->len);
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
        memcpy(pucData + current_offset,(char *)packet_out + sizeof(packet_head) + sizeof(symalg_cipher) + current_offset, data_len);
        memcpy(IV,((symalg_cipher*)((char *)(psession_handle->sess.recv_buff) + sizeof(packet_head)))->IV,IV_LENGHT);
        *puiDatalength += data_len;
        remain_len -= data_len;
        current_offset += data_len; 
    }
    memcpy(pucIV,IV,IV_LENGHT);
    return packet_out->status;
}



int SDF_HashInit(void *hSessionHandle)
{
    int ret = SDR_OK;
    packet_head *packet;
    session_list *psession_handle;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INARGERR;
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
    if(ret <= 0)
    {
        COMMON_ERROR("hash_init error!\n");
        return SDR_COMMFAIL;
    }
    if(packet->status != SDR_OK)
    {
        return packet->status;
    }

    return SDR_OK;
}

int SDF_HashUpdate(void *hSessionHandle,unsigned char *pucData,unsigned int uiDataLength)
{
    int ret = SDR_OK;
    packet_head *packet,*packet_out;
    session_list *psession_handle;
    unsigned int data_len,offset,current_len;
    SM3_st *psm3;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INARGERR;
    }
    if(!pucData)
    {
        COMMON_ERROR("The pucData is NULL!\n");
        return SDR_INARGERR;
    }
    if(uiDataLength == 0)
    {
        COMMON_ERROR("The uiDataLength is error!\n");
        return SDR_INARGERR;
    }
    psession_handle = (session_list *)hSessionHandle;
    packet = (packet_head *)psession_handle->sess.send_buff;
    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = HASH_UPDATE;
    packet->server_session_handle = psession_handle->sess.session_handle;
    psm3 = (SM3_st *)((char *)packet + sizeof(packet_head));

    data_len = uiDataLength;
    offset = 0;
    while(data_len>0)
    {
        if(data_len > MAX_SEND_BUFF_LEN - sizeof(packet_head) - sizeof(SM3_st))
        {
            current_len = MAX_SEND_BUFF_LEN - sizeof(packet_head) - sizeof(SM3_st);
            packet->len = MAX_SEND_BUFF_LEN;
        }
        else
        {
            current_len = data_len;
            packet->len = sizeof(packet_head) + sizeof(SM3_st) + data_len;
        }
        psm3->data_len = current_len;
        memcpy(psm3->data,pucData + offset,current_len);
        ret = socket_send(psession_handle,packet->len);
        packet_out = (packet_head *)psession_handle->sess.send_buff;
        if(ret <= 0)
        {
            COMMON_ERROR("HashUpdata send error!\n");
            return SDR_COMMFAIL;
        }
        if(packet->status != SDR_OK)
        {
            return packet->status;
        }
        data_len -= current_len;
        offset += current_len;
    }
    return SDR_OK;
}


int SDF_HashFinal(void *hSessionHandle,unsigned char *pucData,unsigned int *puiHashLength)
{
    int ret = SDR_OK;
    packet_head *packet;
    session_list *psession_handle;
    SM3_st *psm3;
    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INARGERR;
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
    packet->len = sizeof(packet_head) + sizeof(SM3_st);
    ret = socket_send(psession_handle,packet->len);
    packet = (packet_head *)psession_handle->sess.recv_buff;
    psm3 = (SM3_st *)((void *)packet + sizeof(packet_head));
    if(ret <= 0)
    {
        COMMON_ERROR("HashFinal error!\n");
        return SDR_COMMFAIL;
    }
    if(packet->status != SDR_OK)
    {
        return packet->status;
    }
    *puiHashLength = psm3->data_len;
    memcpy(pucData,psm3->data,*puiHashLength);
    return SDR_OK;
}

int SDF_Hash_Copy(void *hSessionHandle,void **hSessionHandle_out)
{
    session_list *psession_handle_in;
    session_list *psession_handle;
    packet_head *packet;
    device *pdev_handle;
    int ret = 0;
    int i = 0;
    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INARGERR;
    }
    if(!hSessionHandle_out)
    {
        COMMON_ERROR("hSessionHandle_out is NULL!\n");
        return SDR_OUTARGERR;
    }
    psession_handle = (session_list *)hSessionHandle;
    pdev_handle = (device *)(psession_handle->sess.pdev_handle);//找到设备句柄
    psession_handle_in = (session_list *)hSessionHandle;
    psession_handle = (session_list *)malloc(sizeof(session_list));
    if(!psession_handle)
    {
        COMMON_ERROR("malloc session_handle error!\n");
        psession_handle = NULL;
        return SDR_UNKNOWERR;
    }
    memset(psession_handle,0,sizeof(session_list));

    //申请数据BUFF用于接收和发送数据
    psession_handle->sess.send_buff = malloc(MAX_SEND_BUFF_LEN);
    if(psession_handle->sess.send_buff == NULL)
    {
        printf("malloc send_buff handle error\n");
        free(psession_handle);
        return SDR_UNKNOWERR;
    }
    memset(psession_handle->sess.send_buff,0,MAX_SEND_BUFF_LEN);
    psession_handle->sess.recv_buff = malloc(MAX_SEND_BUFF_LEN);
    if(psession_handle->sess.recv_buff == NULL)
    {
        printf("malloc recv_buff handle error\n");
        free(psession_handle->sess.send_buff);
        free(psession_handle);
        return SDR_UNKNOWERR;
    }
    memset(psession_handle->sess.recv_buff,0,MAX_SEND_BUFF_LEN);

    packet = (packet_head*)(psession_handle_in->sess.send_buff);
    packet->major_cmd = control_cmd;
    packet->minor_cmd = HASH_COPY;
    packet->status = 0;
    packet->len = sizeof(packet_head);
    packet->device_guid = pdev_handle->GUID;

    ret = socket_send(psession_handle_in,packet->len);  
    packet = (packet_head*)(psession_handle_in->sess.recv_buff);
    //COMMON_DEBUG("major_cmd:%d,\nminor_cmd:%d,\nlen:%d,\nread_len:%d\n",packet->major_cmd,packet->minor_cmd,packet->len,ret);
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
    packet = (packet_head*)((char *)psession_handle_in->sess.recv_buff+ sizeof(packet_head));
    psession_handle->sess.session_handle = packet->server_session_handle;//保存服务端会话句柄
    psession_handle->sess.pdev_handle = (void*)pdev_handle;
    psession_handle->sess.Dev_GUID = pdev_handle->GUID;
    *hSessionHandle_out = (void *)psession_handle;
    return SDR_OK;

}


int SDF_GenerateRandom(void *hSessionHandle,unsigned int uiLength,unsigned char *pucRandom)
{
    int ret = SDR_OK;
    packet_head *packet;
    session_list *psession_handle;
    GetRandom * prandom;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INARGERR;
    }
    if(uiLength == 0 || uiLength > MAX_RANDOM_LEN)
    {
        COMMON_ERROR("The uiLenght is error!\n");
        return SDR_INARGERR;
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
    if( ret<=0 )
    {
            return SDR_COMMFAIL;
    }
    if(packet->status != SDR_OK)
    {
        return packet->status;
    }
    prandom = (GetRandom *)((char *)packet + sizeof(packet_head));
    memcpy(pucRandom,prandom->data,uiLength);
    return SDR_OK;
}

int SDF_GenerateAgreementDataWithECC(void *hSessionHandle,
                                unsigned int uiISKIndex, 
                                unsigned char *Password,
                                unsigned int password_len,
                                unsigned int uiKeyBits,
                                unsigned char *pucSponsorID,
                                unsigned int uiSponsorIDLength,
                                ECCrefPublicKey *pucSponsorPublicKey,
                                ECCrefPublicKey *pucSponsorTmpPublicKey,
                                void **phAgreementHandle)
{
    int ret = SDR_OK;
    packet_head *packet = NULL;
    session_list *psession_handle = NULL;
    Gen_agreemen_para *pgreement_para = NULL;
    sdf_gen_agreement_par_handle *agreement_handle;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INARGERR;
    }
    if(uiISKIndex >= MAX_KEY_INDEX)
    {
        COMMON_ERROR("Input uiISKIndex more than the max index !\n");
        return SDR_KEYNOTEXIST;
    }
    if(uiKeyBits > SM4_BITS || uiKeyBits == 0)
    {
        COMMON_ERROR("Input uiKeyBits Err!\n");
        return SDR_INARGERR;
    }
    if(uiKeyBits % 8)
    {
        COMMON_ERROR("Input uiKeuBits Err!\n");
        return SDR_INARGERR;
    }
        if(!Password)
    {
        COMMON_ERROR("Password is NULL!\n");
        return SDR_INARGERR;
    }
    if(password_len > PIN_MAX_LEN || password_len <= PIN_MIN_LEN)
    {
        COMMON_ERROR("password_len error!\n");
        return SDR_INARGERR;
    }
    if(!pucSponsorID)
    {
        COMMON_ERROR("pucSponsorID is NULL!\n");
        return SDR_INARGERR;
    }
    if(uiSponsorIDLength > MAX_ID_LEN)
    {
        COMMON_ERROR("Input uiSponsorIDLength too long!\n");
        return SDR_INARGERR;
    }
    if(uiSponsorIDLength == 0)
    {
        COMMON_ERROR("Input uiSponsorIDLength is 0!\n");
        return SDR_INARGERR;
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
    agreement_handle = (sdf_gen_agreement_par_handle *)malloc(sizeof(sdf_gen_agreement_par_handle));
    pgreement_para = (Gen_agreemen_para *)malloc(sizeof(Gen_agreemen_para));
    psession_handle = (session_list *)hSessionHandle;
    packet = (packet_head *)psession_handle->sess.send_buff;
    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->server_session_handle = psession_handle->sess.session_handle;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = GENERATE_AGREEMENT_PARA;

    pgreement_para->ID_length = uiSponsorIDLength;
    pgreement_para->key_index = uiISKIndex;
    pgreement_para->key_bits = uiKeyBits;
    pgreement_para->password_len = password_len;
    memcpy(pgreement_para->password,Password,password_len);
    memcpy(pgreement_para->sponsor_ID,pucSponsorID,uiSponsorIDLength);

    memcpy((char *)packet + sizeof(packet_head),pgreement_para,sizeof(Gen_agreemen_para));
    packet->len = sizeof(packet_head) + sizeof(Gen_agreemen_para);
    ret = socket_send(psession_handle,packet->len);
    packet = (packet_head *)psession_handle->sess.recv_buff;
    if(ret <= 0)
    {
        COMMON_ERROR("Generate agreement para err!\n");
        return SDR_COMMFAIL;
    }
    if(packet->status != SDR_OK)
    {
        return packet->status;
    }
    memcpy(pgreement_para,(char *)packet + sizeof(packet_head),sizeof(Gen_agreemen_para));
    pucSponsorPublicKey->bits = SM2_BITS;
    memcpy(&(pucSponsorPublicKey->x),pgreement_para->pubkey,SM2_BYTES);
    memcpy(pucSponsorPublicKey->y,pgreement_para->pubkey + SM2_BYTES,SM2_BYTES);
    pucSponsorTmpPublicKey->bits = SM2_BITS;
    memcpy(pucSponsorTmpPublicKey->x,pgreement_para->tmp_pubkey,SM2_BYTES);
    memcpy(pucSponsorTmpPublicKey->y,pgreement_para->tmp_pubkey + SM2_BYTES,SM2_BYTES);
    memcpy(agreement_handle,&(pgreement_para->agreement_handle),sizeof(sdf_gen_agreement_par_handle));

    *phAgreementHandle = (void *)agreement_handle;
    free(pgreement_para);
    return SDR_OK;
}

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
                                unsigned int *session_key_len)
{
    int ret = SDR_OK;
    packet_head *packet;
    session_list *psession_handle;
    unsigned int *keyhandle;
    Gen_agreement_key *pagreement_handle;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INARGERR;
    }
    if(uiISKIndex > MAX_KEY_INDEX)
    {
        COMMON_ERROR("uiISKInde is more than the max index!\n");
        return SDR_INARGERR;
    }
    if(!PassWord)
    {
        COMMON_ERROR("PassWord is NULL!\n");
        return SDR_INARGERR;
    }
    if(uiKeyBits > SM4_BITS || uiKeyBits == 0)
    {
        COMMON_ERROR("uiKeyBits is error!\n");
        return SDR_INARGERR;
    }
    if(uiKeyBits % 8)
    {
        COMMON_ERROR("uiKeyBits is error!\n");
        return SDR_INARGERR;
    }
    if(!pucResponsorID)
    {
        COMMON_ERROR("pucResponsorID is NULL!\n");
        return SDR_INARGERR;
    }
    if(uiResponsorIDLength == 0)
    {
        COMMON_ERROR("uiResponsorIDLength is 0!\n");
        return SDR_INARGERR;
    }
    if(uiResponsorIDLength > MAX_ID_LEN)
    {
        COMMON_ERROR("uiResponsorIDLength is too long!\n");
        return SDR_INARGERR;
    }
    if(!pucSponsorID)
    {
        COMMON_ERROR("pucSponsorID is NULL!\n");
        return SDR_INARGERR;
    }
    if(uiSponsorIDLength > MAX_ID_LEN)
    {
        COMMON_ERROR("uiSponsorIDLength is too long!\n");
        return SDR_INARGERR;
    }
    if(uiSponsorIDLength == 0)
    {
        COMMON_ERROR("uiSponsorIDLength is 0!\n");
        return SDR_INARGERR;
    }
    if(!pucSponsorPublicKey)
    {
        COMMON_ERROR("pucSponsorPublicKey is NULL!\n");
        return SDR_INARGERR;
    }
    if(pucSponsorPublicKey->bits != SM2_BITS)
    {
        COMMON_ERROR("pucSponsorPublicKey bit is error!\n");
        return SDR_INARGERR;
    }
    if(!pucSponsorTmpPublicKey)
    {
        COMMON_ERROR("pucSponsorTmpPublicKey is NULL!\n");
        return SDR_INARGERR;
    }
    if(pucSponsorTmpPublicKey->bits != SM2_BITS)
    {
        COMMON_ERROR("pucSponsorTmpPublicKey bits is error!\n");
        return SDR_INARGERR;
    }
    if(!pucResponsorTmpPublicKey)
    {
        COMMON_ERROR("pucResponsorTmpPublicKey is NULL!\n");
        return SDR_OUTARGERR;
    }
    if(!pucResponsorPublicKey)
    {
        COMMON_ERROR("pucResponsorPublicKey is NULL!\n");
        return SDR_OUTARGERR;
    }
    if(!session_key)
    {
        COMMON_ERROR("session_key is NULL");
        return SDR_OUTARGERR;
    }
    if(!session_key_len)
    {
        COMMON_ERROR("session_key_len is NULL");
        return SDR_OUTARGERR;
    }
    psession_handle = (session_list *)hSessionHandle;
    packet = (packet_head *)psession_handle->sess.send_buff;
    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->server_session_handle = psession_handle->sess.session_handle;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = GENERATE_AGREEMENT_PARA_KEY;
    pagreement_handle = (Gen_agreement_key *)((char *)packet + sizeof(packet_head));
    pagreement_handle->key_index = uiISKIndex;
    pagreement_handle->key_bits = uiKeyBits;
    pagreement_handle->responseID_length = uiResponsorIDLength;
    memcpy(pagreement_handle->response_ID,pucResponsorID,uiResponsorIDLength);
    pagreement_handle->sponsorID_length = uiSponsorIDLength;
    memcpy(pagreement_handle->sponsor_ID,pucSponsorID,uiSponsorIDLength);
    pagreement_handle->password_len = password_len;
    memcpy(pagreement_handle->password,PassWord,password_len);

    memcpy(pagreement_handle->sponsor_pubkey,pucSponsorPublicKey->x,SM2_BYTES);
    memcpy(pagreement_handle->sponsor_pubkey + SM2_BYTES,pucSponsorPublicKey->y,SM2_BYTES);
    memcpy(pagreement_handle->sponsor_tmp_pubkey,pucSponsorTmpPublicKey->x,SM2_BYTES);
    memcpy(pagreement_handle->sponsor_tmp_pubkey + SM2_BYTES,pucSponsorTmpPublicKey->y,SM2_BYTES);
    
    packet->len = sizeof(packet_head) + sizeof(Gen_agreement_key);
    ret = socket_send(psession_handle,packet->len);
    packet = (packet_head *)psession_handle->sess.recv_buff;
    if(ret <= 0)
    {
        COMMON_ERROR("Generate agreement data and key error!\n");
        return SDR_COMMFAIL;
    }
    if(packet->status != SDR_OK)
    {
        return packet->status;
    }

    pagreement_handle = (Gen_agreement_key *)((char *)packet + sizeof(packet_head));
    pucResponsorPublicKey->bits = SM2_BITS;
    memcpy(pucResponsorPublicKey->x,pagreement_handle->response_pubkey,SM2_BYTES);
    memcpy(pucResponsorPublicKey->y,pagreement_handle->response_pubkey + SM2_BYTES,SM2_BYTES);
    pucResponsorTmpPublicKey->bits = SM2_BITS;
    memcpy(pucResponsorTmpPublicKey->x,pagreement_handle->response_tmp_pubkey,SM2_BYTES);
    memcpy(pucResponsorTmpPublicKey->y,pagreement_handle->response_tmp_pubkey + SM2_BYTES,SM2_BYTES);
    *session_key_len = pagreement_handle->session_key_out_len;
    memcpy(session_key,pagreement_handle->session_key,pagreement_handle->session_key_out_len);
    return SDR_OK;

}                                

int SDF_GenerateKeyPair_ECC(void *hSessionHandle,unsigned int *index, unsigned char *password, unsigned int length)
{
    int ret = SDR_OK;
    packet_head *packet;
    session_list *psession_handle;
    Generate_key_pair *pgenerate_key_pair;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INARGERR;
    }
    if(!password)
    {
        COMMON_ERROR("password is NULL!\n");
        return SDR_INARGERR;
    }
    if(length < PIN_MIN_LEN || length > PIN_MAX_LEN)
    {
        COMMON_ERROR("length is error!\n");
        return SDR_INARGERR;
    }

    psession_handle = (session_list *)hSessionHandle;
    packet = (packet_head *)psession_handle->sess.send_buff;
    pgenerate_key_pair = (Generate_key_pair *)((char *)packet + sizeof(packet_head));

    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->server_session_handle = psession_handle->sess.session_handle;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = GENERATE_KEY_PAIR;

    pgenerate_key_pair->length = length;
    memcpy(pgenerate_key_pair->password,password,length);

    packet->len = sizeof(packet_head) + sizeof(Gen_agreement_key);
    ret = socket_send(psession_handle,packet->len);
    packet = (packet_head *)psession_handle->sess.recv_buff;
    pgenerate_key_pair = (Generate_key_pair *)((char *)packet + sizeof(packet_head));
    if(ret <= 0)
    {
        COMMON_ERROR("Generate_key_pair ERROR!\n");
        return SDR_COMMFAIL;
    }
    if(packet->status != SDR_OK)
    {
        return packet->status;
    }
    *index = pgenerate_key_pair->index;
    return SDR_OK;

}

int SDF_GenerateKeyWith_ECC(void *hSessionHandle,
                        unsigned char *pucResponseID,
                        unsigned int uiResponseIDLength,
                        ECCrefPublicKey *pucResponsePublicKey,
                        ECCrefPublicKey *pucResponseTmpPublicKey,
                        void *phAgreementHandle,
                        unsigned char *session_key,
                        unsigned int *session_key_len
                        )
{
    int ret = SDR_OK;
    packet_head *packet;
    session_list *psession_handle;
    Cipher_agreement_key *pagreement_handle;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INARGERR;
    }

    if(!pucResponseID)
    {
        COMMON_ERROR("pucResponseID is NULL!\n");
        return SDR_INARGERR;
    }
    if(uiResponseIDLength > MAX_ID_LEN)
    {
        COMMON_ERROR("uiResponseIDLength is too long!\n");
        return SDR_INARGERR;
    }
    if(!pucResponsePublicKey)
    {
        COMMON_ERROR("pucResponsePublicKey is NULL!\n");
        return SDR_INARGERR;
    }
    if(pucResponsePublicKey->bits != SM2_BITS)
    {
        COMMON_ERROR("pucResponsePublicKey bits is error!\n");
        return SDR_INARGERR;
    }  
    if(!pucResponseTmpPublicKey)
    {
        COMMON_ERROR("pucResponseTmpPublicKey is NULL!\n");
        return SDR_INARGERR;
    }
    if(pucResponseTmpPublicKey->bits != SM2_BITS)
    {
        COMMON_ERROR("pucResponseTmpPublicKey bits is error!\n");
        return SDR_INARGERR;
    }
    if(!phAgreementHandle)
    {
        COMMON_ERROR("phAgreementHandle is NULL!\n");
        return SDR_OUTARGERR;
    }
    if(!session_key)
    {
        COMMON_ERROR("session_key is NULL!\n");
        return SDR_OUTARGERR;
    }
    if(!session_key_len)
    {
        COMMON_ERROR("session_key_len is NULL!\n");
        return SDR_OUTARGERR;
    }

    psession_handle = (session_list *)hSessionHandle;
    packet = (packet_head *)psession_handle->sess.send_buff;
    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->server_session_handle = psession_handle->sess.session_handle;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = GENERATE_KEY_ECC;
    pagreement_handle = (Cipher_agreement_key *)((char *)packet + sizeof(packet_head));
    
    pagreement_handle->ID_Length = uiResponseIDLength;
    memcpy(pagreement_handle->response_ID,pucResponseID,uiResponseIDLength);
    memcpy(pagreement_handle->pubkey,pucResponsePublicKey->x,SM2_BYTES);
    memcpy(pagreement_handle->pubkey + SM2_BYTES,pucResponsePublicKey->y,SM2_BYTES);
    memcpy(pagreement_handle->tmp_pubkey,pucResponseTmpPublicKey->x,SM2_BYTES);
    memcpy(pagreement_handle->tmp_pubkey + SM2_BYTES,pucResponseTmpPublicKey->y,SM2_BYTES);
    memcpy(&(pagreement_handle->key_handle),phAgreementHandle,sizeof(sdf_gen_agreement_par_handle));

    packet->len = sizeof(packet_head) + sizeof(Cipher_agreement_key);
    ret = socket_send(psession_handle,packet->len);
    packet = (packet_head *)psession_handle->sess.recv_buff;
    if(ret <= 0)
    {
        COMMON_ERROR("Generate key with ECC error!\n");
        return SDR_COMMFAIL;
    }
    if(packet->status != SDR_OK)
    {
        return packet->status;
    }
    pagreement_handle = (Cipher_agreement_key *)((char *)packet + sizeof(packet_head));
    memcpy(session_key,&(pagreement_handle->session_key),pagreement_handle->session_key_out_len);
    *session_key_len = pagreement_handle->session_key_out_len;
    
    free(phAgreementHandle);
    return SDR_OK;
}


int SDF_Sign_ECC(void *hSessionHandle,
            unsigned int uiISKIndex,
            unsigned char *PassWord,
            unsigned int passwordlen,
            unsigned char *pucData,
            unsigned int uiDataLength,
            ECCSignature *pucSignature
            )
{
    packet_head *packet;
    session_list *psession_handle;
    int ret = SDR_OK;
    ECCSignature *psignature;
    Sign_Verify_ECC *psign_verify;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INARGERR;
    }
    if(uiISKIndex >= MAX_KEY_INDEX)
    {
        COMMON_ERROR("uiISKIndex is more than the max_index!\n");
        return SDR_INARGERR;
    }
    if(!PassWord)
    {
        COMMON_ERROR("PassWord is NULL!\n");
        return SDR_INARGERR;
    }
    if(passwordlen <= PIN_MIN_LEN || passwordlen > PIN_MAX_LEN)
    {
        COMMON_ERROR("passwordlen is error!\n");
        return SDR_INARGERR;
    }
    if(!pucData)
    {
        COMMON_ERROR("pucData is NULL!\n");
        return SDR_INARGERR;
    }
    if(uiDataLength != SM2_BYTES)
    {
        COMMON_ERROR("uiDataLength is error!\n");
        return SDR_INARGERR;
    }
    if(!pucSignature)
    {
        COMMON_ERROR("pucSignature is NULL!\n");
        return SDR_OUTARGERR;
    }
    psession_handle = (session_list *)hSessionHandle;
    packet = (packet_head *)psession_handle->sess.send_buff;
    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->server_session_handle = psession_handle->sess.session_handle;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = ECC_SIGN;

    psign_verify = (Sign_Verify_ECC *)((char *)packet + sizeof(packet_head));
    psign_verify->datalen = uiDataLength;
    psign_verify->index = uiISKIndex;
    psign_verify->password_len = passwordlen;
    memcpy(psign_verify->data,pucData,uiDataLength);
    memcpy(psign_verify->password,PassWord,passwordlen);

    packet->len = sizeof(packet_head) + sizeof(Sign_Verify_ECC);
    ret = socket_send(psession_handle,packet->len);
    packet = (packet_head *)psession_handle->sess.recv_buff;
    psign_verify = (Sign_Verify_ECC *)((char *)packet + sizeof(packet_head));
    if(ret <= 0)
    {
        COMMON_ERROR("Sign ECC error!\n");
        return SDR_COMMFAIL;
    }
    if(packet->status != SDR_OK)
    {
        return packet->status;
    }

    memcpy(pucSignature->r,psign_verify->sign,SM2_BYTES);
    memcpy(pucSignature->s,psign_verify->sign + SM2_BYTES,SM2_BYTES);

    return SDR_OK;
}


int SDF_ExtVerify_ECC(void *hSessionHandle,
                ECCrefPublicKey *pucPublicKey,
                unsigned char *pucDataInput,
                unsigned int uiInputLength,
                ECCSignature *pucSignature)
{
    int ret = SDR_OK;
    packet_head *packet;
    session_list *psession_handle;
    sdf_extern_ecc_verify *psign_verify;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INARGERR;
    }
    if(!pucPublicKey)
    {
        COMMON_ERROR("pucPublicKey is NULL!\n");
        return SDR_INARGERR;
    }
    if(!pucDataInput)
    {
        COMMON_ERROR("pucDataInput is NULL!\n");
        return SDR_INARGERR;
    }
    if(uiInputLength > 64 || uiInputLength <= 0)
    {
        COMMON_ERROR("uiInputLength is error!\n");
        return SDR_INARGERR;
    }
    if(!pucSignature)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_OUTARGERR;
    }
    psession_handle = (session_list *)hSessionHandle;
    packet = (packet_head *)psession_handle->sess.send_buff;
    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->server_session_handle = psession_handle->sess.session_handle;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = ECC_VERIGY;
    
    psign_verify = (sdf_extern_ecc_verify *)((char *)packet + sizeof(packet_head));
    memcpy(psign_verify->in_pub_key,pucPublicKey->x,SM2_BYTES);
    memcpy(psign_verify->in_pub_key + SM2_BYTES,pucPublicKey->y,SM2_BYTES);
    memcpy(psign_verify->in_hash_data,pucDataInput,uiInputLength);
    memcpy(psign_verify->in_out_r_s_value,pucSignature->r,SM2_BYTES);
    memcpy(psign_verify->in_out_r_s_value + SM2_BYTES,pucSignature->s,SM2_BYTES);

    packet->len = sizeof(packet_head) + sizeof(sdf_extern_ecc_verify);
    ret = socket_send(psession_handle,packet->len);
    packet = (packet_head *)psession_handle->sess.recv_buff;
    if(ret <= 0)
    {
        COMMON_ERROR("verify ecc error!\n");
        return SDR_COMMFAIL;
    }
    if(packet->status != SDR_OK)
    {
        return packet->status;
    }
    return SDR_OK;
}   



int SDF_DeleteKeyPair_ECC(void *hSessionHandle,unsigned int *index, unsigned char *password, unsigned int length)
{
    int ret = SDR_OK;
    packet_head *packet;
    session_list *psession_handle;
    Generate_key_pair *pgenerate_key_pair;

    if(!hSessionHandle)
    {
        COMMON_ERROR("hSessionHandle is NULL!\n");
        return SDR_INARGERR;
    }
    if(!password)
    {
        COMMON_ERROR("password is NULL!\n");
        return SDR_INARGERR;
    }
    if(!index)
    {
        COMMON_ERROR("index is NULL!\n");
        return SDR_INARGERR;
    }
    if(length < PIN_MIN_LEN || length > PIN_MAX_LEN)
    {
        COMMON_ERROR("length is error!\n");
        return SDR_INARGERR;
    }

    psession_handle = (session_list *)hSessionHandle;
    packet = (packet_head *)psession_handle->sess.send_buff;
    pgenerate_key_pair = (Generate_key_pair *)((char *)packet + sizeof(packet_head));

    packet->device_guid = psession_handle->sess.Dev_GUID;
    packet->server_session_handle = psession_handle->sess.session_handle;
    packet->major_cmd = server_cmd;
    packet->minor_cmd = DELETE_KEY_PAIR;

    pgenerate_key_pair->index = *index;
    pgenerate_key_pair->length = length;
    memcpy(pgenerate_key_pair->password,password,length);

    packet->len = sizeof(packet_head) + sizeof(Gen_agreement_key);
    ret = socket_send(psession_handle,packet->len);
    packet = (packet_head *)psession_handle->sess.recv_buff;
    pgenerate_key_pair = (Generate_key_pair *)((char *)packet + sizeof(packet_head));
    if(ret <= 0)
    {
        COMMON_ERROR("Delete_key_pair ERROR!\n");
        return SDR_COMMFAIL;
    }
    if(packet->status != SDR_OK)
    {
        return packet->status;
    }
    return SDR_OK;

}


