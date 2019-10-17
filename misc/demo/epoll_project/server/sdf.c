/*
 * sdf.c
 *
 *  Created on: 2019年8月6日
 *      Author: jjn
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/common.h"
#include "../alg/sm2.h"
#include  "../alg/sm3.h"
#include "../alg/sm4.h"
#include "../include/sdf.h"
#include "../include/sdf_manage.h"

unsigned int sdf_device_num;//实际打开的设备个数
device_info device_open_info;//需要维护的设备信息
unsigned int sdf_session_num[DEVICE_NUM];//实际打开会话个数

/*****************************************************************************************************************
 * private F function: 打开设备
 * args:    [in] sdf_cmd_head :包头
 * return: none
 ******************************************************************************************************************/
void SDF_OpenDevice(packet_head **p)
{
	//int i=0;
//	socket_info *socket_head,*socket_p,*socket_new;


	if(sdf_device_num>=DEVICE_NUM)
	{
		(*p)->sdf_cmd_head.status=EXIT_FAILURE;
		(*p)->sdf_cmd_head.len=sizeof(packet_head);
		printfS("open_device too many\n");
		return;
	}
	device_open_info.dev[sdf_device_num].device_guid.index=sdf_device_num;
	device_open_info.dev[sdf_device_num].device_guid.type=DEVICE_GUID;
	get_random((unsigned char*)&(device_open_info.dev[sdf_device_num].device_guid.random),8);
	/*if(fd_A[0]==0)
	{
		//此设备未创建socket
		(*p)->sdf_cmd_head.status=EXIT_FAILURE;
		(*p)->sdf_cmd_head.len=sizeof(packet_head);
		printfS("open_device no creat socket\n");
		return;
	}
	socket_new=malloc(sizeof(socket_info));
	if(socket_new==NULL)
	{
		(*p)->sdf_cmd_head.status=EXIT_FAILURE;
		(*p)->sdf_cmd_head.len=sizeof(packet_head);
		printfS("malloc socket_new error\n");
		return;
	}
	socket_new->next=NULL;
	socket_new->sockfd=fd_A[0];
	socket_new->prev=(struct socket_info *)socket_new;
	socket_head=socket_new;
	socket_p=socket_new;
	device_open_info.dev[sdf_device_num].socket_list_head=socket_head;

	//创建socket链表
	for(i=1;i<SOCKET_NUM;i++)
	{
		if(fd_A[i]==0)
		{
			break;
		}
		socket_new=malloc(sizeof(socket_info));
		if(socket_new==NULL)
		{
			(*p)->sdf_cmd_head.status=EXIT_FAILURE;
			(*p)->sdf_cmd_head.len=sizeof(xmit_protocol);
			printfS("malloc socket_n error\n");
			return;
		}
		socket_p->next=(struct socket_info *)socket_new;
		socket_new->prev=(struct socket_info *)socket_p;
		socket_new->sockfd=fd_A[i];
		socket_new->next=NULL;
		socket_p=socket_new;
		socket_head->prev=(struct socket_info *)socket_new;
		if(i==1)
			socket_head->next=(struct socket_info *)socket_new;
	}*/
	(*p)->sdf_cmd_head.status=SDR_OK;
	(*p)->sdf_cmd_head.len=sizeof(packet_head);
	(*p)->sdf_device_info.device_guid=device_open_info.dev[sdf_device_num].device_guid;
	if(sdf_device_num<DEVICE_NUM)
		sdf_device_num++;
	printfS("open device success\n");
}
/*****************************************************************************************************************
 * private F function: 关闭设备
 * args:    [in] sdf_cmd_head :包头
 * return: none
 ******************************************************************************************************************/
void SDF_CloseDevice(packet_head **p)
{
	socket_info *socket_delete;
	//关闭设备清除所有信息
	if(sdf_device_num>=DEVICE_NUM)
	{
		(*p)->sdf_cmd_head.status=EXIT_FAILURE;
		(*p)->sdf_cmd_head.len=sizeof(packet_head);
		printfS("open_device too many\n");
		return;
	}
	if((*p)->sdf_device_info.device_guid.index>=sdf_device_num)//链表为空链表
	{
		(*p)->sdf_cmd_head.status=EXIT_FAILURE;
		(*p)->sdf_cmd_head.len=sizeof(packet_head);
		printfS("rev_device_info.device_guid.index error\n");
		return;
	}
	if(strcmp((char *)&(*p)->sdf_device_info.device_guid,(char *)&device_open_info.dev[(*p)->sdf_device_info.device_guid.index].device_guid)!=0)//循环查找要删除的节点
	{
		(*p)->sdf_cmd_head.status=EXIT_FAILURE;
		(*p)->sdf_cmd_head.len=sizeof(packet_head);
		printfS("rev_device_info.device_guid error\n");
		return;
	}

	//循环释放socket,从尾部开始释放
	while(device_open_info.dev[(*p)->sdf_device_info.device_guid.index].socket_list_head->prev!=(struct socket_info *)device_open_info.dev[(*p)->sdf_device_info.device_guid.index].socket_list_head)
	{
		socket_delete=(socket_info *)device_open_info.dev[(*p)->sdf_device_info.device_guid.index].socket_list_head->prev;
		socket_delete->next=NULL;
		device_open_info.dev[(*p)->sdf_device_info.device_guid.index].socket_list_head->prev=socket_delete->prev;
		free(socket_delete);
		socket_delete=NULL;
	}
	if(device_open_info.dev[(*p)->sdf_device_info.device_guid.index].socket_list_head->prev!=(struct socket_info *)device_open_info.dev[(*p)->sdf_device_info.device_guid.index].socket_list_head)
	{
		free(device_open_info.dev[(*p)->sdf_device_info.device_guid.index].socket_list_head);
		device_open_info.dev[(*p)->sdf_device_info.device_guid.index].socket_list_head=NULL;
	}
	if(sdf_device_num<DEVICE_NUM)
		sdf_device_num--;
	memset(&device_open_info.dev[(*p)->sdf_device_info.device_guid.index].device_guid,0,sizeof(guid));

	(*p)->sdf_cmd_head.status=SDR_OK;
	(*p)->sdf_cmd_head.len=sizeof(packet_head);

	printfS("close device success\n");
}
/*****************************************************************************************************************
 * private F function: 打开会话
 * args:    [in] sdf_cmd_head :包头
 * return: none
 ******************************************************************************************************************/
void SDF_OpenSession(packet_head **p)
{
	session_info *session_new,*session;
	device rev_device_info;

	printfS("SDF_OpenSession\n");
	memcpy(&rev_device_info,&(*p)->sdf_device_info,sizeof(device));
	if(rev_device_info.device_guid.index>=sdf_device_num)
	{
		(*p)->sdf_cmd_head.status=EXIT_FAILURE;
		(*p)->sdf_cmd_head.len=sizeof(packet_head);
		printfS("rev_device_info.device_guid.index=%d error\n",rev_device_info.device_guid.index);
		return;
	}
	if(sdf_session_num[rev_device_info.device_guid.index]>=SESSION_NUM)
	{
		(*p)->sdf_cmd_head.status=EXIT_FAILURE;
		(*p)->sdf_cmd_head.len=sizeof(packet_head);
		printfS("sdf_session_num[%d] error\n",rev_device_info.device_guid.index);
		return;
	}
	if(strcmp((char *)&rev_device_info.device_guid,(char *)&device_open_info.dev[rev_device_info.device_guid.index].device_guid)!=0)//循环查找要删除的节点
	{
		(*p)->sdf_cmd_head.status=EXIT_FAILURE;
		(*p)->sdf_cmd_head.len=sizeof(packet_head);
		printfS("rev_device_info.device_guid error\n");
		return;
	}

	session_new=malloc(sizeof(session_info));
	if(session_new==NULL)
	{
		(*p)->sdf_cmd_head.status=EXIT_FAILURE;
		(*p)->sdf_cmd_head.len=sizeof(packet_head);
		printfS("malloc session_new error\n");
		return;
	}
	session_new->next=NULL;
	session_new->GUID.index=sdf_session_num[rev_device_info.device_guid.index];
	session_new->GUID.type=SESSION_GUID;
	get_random((unsigned char*)&session_new->GUID.random,8);
	session_new->prev=(struct session_info *)session_new;
	if(sdf_session_num[rev_device_info.device_guid.index]==0)//链表头
	{
		device_open_info.dev[rev_device_info.device_guid.index].session_list_head=session_new;
	}
	else
	{
		session=( session_info *)device_open_info.dev[rev_device_info.device_guid.index].session_list_head->prev;//链表尾部
		session_new->prev=(struct session_info *)session;
		session->next=(struct session_info *)session_new;
		device_open_info.dev[rev_device_info.device_guid.index].session_list_head->prev=(struct session_info *)session_new;
		if(sdf_session_num[rev_device_info.device_guid.index]==1)
		{
			device_open_info.dev[rev_device_info.device_guid.index].session_list_head->next=(struct session_info *)session_new;
		}
	}
	(*p)->sdf_cmd_head.status=SDR_OK;
	(*p)->sdf_cmd_head.len=sizeof(xmit_protocol)+sizeof(device);
	(*p)->sdf_device_info.session_list_head=session_new;
	if(sdf_session_num[rev_device_info.device_guid.index]<SESSION_NUM)
		sdf_session_num[rev_device_info.device_guid.index]++;
	printfS("open session success\n");
}

/*****************************************************************************************************************
 * private F function: 关闭会话
 * args:    [in] sdf_cmd_head :包头
 * return: none
 ******************************************************************************************************************/
void SDF_CloseSession(packet_head **p)
{
	session_info *delete_session_info;

	printfS("SDF_CloseSession\n");
	if((*p)->sdf_device_info.device_guid.index>=sdf_device_num)
	{
		(*p)->sdf_cmd_head.status=EXIT_FAILURE;
		(*p)->sdf_cmd_head.len=sizeof(packet_head);
		printfS("rev_device_info.device_guid.index error\n");
		return;
	}
	if(sdf_session_num[(*p)->sdf_device_info.device_guid.index]>=SESSION_NUM)
	{
		(*p)->sdf_cmd_head.status=EXIT_FAILURE;
		(*p)->sdf_cmd_head.len=sizeof(packet_head);
		printfS("sdf_session_num[%d] error\n",(*p)->sdf_device_info.device_guid.index);
		return;
	}
	if(strcmp((char *)&(*p)->sdf_device_info.device_guid,(char *)&device_open_info.dev[(*p)->sdf_device_info.device_guid.index].device_guid)!=0)//循环查找要删除的节点
	{
		(*p)->sdf_cmd_head.status=EXIT_FAILURE;
		(*p)->sdf_cmd_head.len=sizeof(packet_head);
		printfS("rev_device_info.device_guid error\n");
		return;
	}
	if((*p)->sdf_device_info.session_list_head==device_open_info.dev[(*p)->sdf_device_info.device_guid.index].session_list_head)//删除表头
	{
		if(device_open_info.dev[(*p)->sdf_device_info.device_guid.index].session_list_head->next!=NULL)//有后续
		{
			delete_session_info=(session_info *)device_open_info.dev[(*p)->sdf_device_info.device_guid.index].session_list_head->next;
			delete_session_info->prev=device_open_info.dev[(*p)->sdf_device_info.device_guid.index].session_list_head->prev;
			device_open_info.dev[(*p)->sdf_device_info.device_guid.index].session_list_head=delete_session_info;
		}
		free((*p)->sdf_device_info.session_list_head);
		(*p)->sdf_device_info.session_list_head=NULL;
	}
	else
	{
		//释放session
		if((*p)->sdf_device_info.session_list_head->next!=NULL)
		{
			delete_session_info=(session_info *)(*p)->sdf_device_info.session_list_head->next;
			delete_session_info->prev=(*p)->sdf_device_info.session_list_head->prev;
			delete_session_info=(session_info *)(*p)->sdf_device_info.session_list_head->prev;
			delete_session_info->next=(*p)->sdf_device_info.session_list_head->next;
		}
		else//删除尾部
		{
			delete_session_info=(session_info *)(*p)->sdf_device_info.session_list_head->prev;
			delete_session_info->next=(*p)->sdf_device_info.session_list_head->next;
			device_open_info.dev[(*p)->sdf_device_info.device_guid.index].session_list_head->prev=(*p)->sdf_device_info.session_list_head->prev;
		}
		free((*p)->sdf_device_info.session_list_head);
		(*p)->sdf_device_info.session_list_head=NULL;
	}
	if(sdf_session_num[(*p)->sdf_device_info.device_guid.index]<SESSION_NUM)
		sdf_session_num[(*p)->sdf_device_info.device_guid.index]--;
	(*p)->sdf_cmd_head.status=SDR_OK;
	(*p)->sdf_cmd_head.len=sizeof(packet_head);
	printfS("close session success\n");
}

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
void SDF_Encrypt(packet_head **sdf_cmd_head,sdf_sym_server **p)
{
	unsigned char *out_data = malloc(SDF_ALG_SERVER_MAX_PACKET);
	int block_num =0;

	printfS("\n SDF_Encrypt start \n");
	//需上层判断是不是一个分组的整数倍
	block_num=(*p)->in_out_data_len/16;
	if((*p)->in_alg_id==0x401)
	{
		sm4_ecb((*p)->in_key,SM4_ENC,block_num,(*p)->in_out_data,out_data);
	}
	else
	{
		sm4_cbc((*p)->in_key,SM4_ENC,block_num,(*p)->in_out_iv,(*p)->in_out_data,out_data);
		memcpy((*p)->in_out_iv,out_data+(*p)->in_out_data_len-0x10,0x10);
	}
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	(*sdf_cmd_head)->sdf_cmd_head.len=sizeof(packet_head)+sizeof(sdf_sym_server)+(*p)->in_out_data_len;
	memcpy((*p)->in_out_data,out_data,(*p)->in_out_data_len);
	free(out_data);
	out_data=NULL;
	printfS("\n SDF_Encrypt end \n");
}
/*****************************************************************************************************************
 * private F function: SDF_Decrypt
 * args:    [in] df_sym_server.sdf_cmd_head :包头
 * args:    [in] in_alg_id: 算法标识
 * args:    [in] in_key :密钥
 * args:    [in/out] in_out_iv :iv
 * args:    [in/out] in_out_data_len: 输入输出数据长度
 * args:    [in/out] in_out_data: 输入输出数据
 * return: none
 ******************************************************************************************************************/
void SDF_Decrypt(packet_head **sdf_cmd_head,sdf_sym_server **p)
{
	unsigned char *out_data = malloc(SDF_ALG_SERVER_MAX_PACKET);
	int block_num =0;

	printfS("\n SDF_Decrypt start \n");
	//需上层判断是不是一个分组的整数倍
	block_num=(*p)->in_out_data_len/16;
	if((*p)->in_alg_id==0x401)
	{
		sm4_ecb((*p)->in_key,SM4_DEC,block_num,(*p)->in_out_data,out_data);
	}
	else
	{
		sm4_cbc((*p)->in_key,SM4_DEC,block_num,(*p)->in_out_iv,(*p)->in_out_data,out_data);
		memcpy((*p)->in_out_iv,out_data+(*p)->in_out_data_len-0x10,0x10);
	}
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	(*sdf_cmd_head)->sdf_cmd_head.len=sizeof(packet_head)+sizeof(sdf_sym_server)+(*p)->in_out_data_len;
	memcpy((*p)->in_out_data,out_data,(*p)->in_out_data_len);
	free(out_data);
	out_data=NULL;
	printfS("\n SDF_Decrypt end \n");
}

/*****************************************************************************************************************
 * private F function: 三步式数据杂凑运算第一步
 * args:    [in] sdf_cmd_head :包头
 * return: none
 ******************************************************************************************************************/
void SDF_HashInit(packet_head **sdf_cmd_head)
{
	SM3_CTX *p=(SM3_CTX *)(*sdf_cmd_head)->sdf_device_info.session_list_head->sm3_par_p;

	printfS("\n SDF_HashInit start \n");
	SM3_Init((SM3_CTX **)&p);
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	printfS("\n SDF_HashInit end \n");
}

/*****************************************************************************************************************
 * private F function: 三步式数据杂凑运算第二步
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] in_data :输入数据
 * args:    [in] in_data_len :输入数据长度
 * return: none
 ******************************************************************************************************************/
void SDF_HashUpdate(packet_head **sdf_cmd_head,sdf_sm3_server **p)
{
	SM3_CTX *sm3_p=(SM3_CTX *)(*sdf_cmd_head)->sdf_device_info.session_list_head->sm3_par_p;

	printfS("\n SDF_HashUpdate start \n");
	SM3_Update((SM3_CTX **)&sm3_p, (*p)->in_out_data, (*p)->in_out_data_len);
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	printfS("\n SDF_HashUpdate end \n");
}

/*****************************************************************************************************************
 * private F function: 三步式数据杂凑运算第三步
 * args:    [in] sdf_cmd_head :包头
 * args:    [out] out_data :输出数据
 * args:    [out] out_data_len :输出数据长度，32
 * return: none
 ******************************************************************************************************************/
void SDF_HashUpFinal(packet_head **sdf_cmd_head,sdf_sm3_server **p)
{
	SM3_CTX *sm3_p=(SM3_CTX *)(*sdf_cmd_head)->sdf_device_info.session_list_head->sm3_par_p;

	printfS("\n SDF_HashUpFinal start \n");
	SM3_Final((SM3_CTX **)&sm3_p, (*p)->in_out_data);
	(*p)->in_out_data_len=32;
	(*sdf_cmd_head)->sdf_cmd_head.len=sizeof(packet_head)+sizeof(sdf_sm3_server)+(*p)->in_out_data_len;
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	printfS("\n SDF_HashUpFinal end \n");
}
/*****************************************************************************************************************
 * private F function: sdf_cmd_head
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] data_len :随机数数据长度
 * args:    [out] data :随机数数据
 * return: none
 ******************************************************************************************************************/
void SDF_GenerateRandom(packet_head **sdf_cmd_head,sdf_get_rng **p)
{
	printfS("\n SDF_GenerateRandom start \n");
	get_random((*p)->out_data,(*p)->in_data_len);
	(*sdf_cmd_head)->sdf_cmd_head.len=sizeof(packet_head)+sizeof(sdf_get_rng)+(*p)->in_data_len;
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	printfS("\n SDF_GenerateRandom end \n");
}
/*****************************************************************************************************************
 * private F function: 产生ECC密钥对并输出
 * args:    [in] sdf_cmd_head :包头
 * args:    [out] key_index :生成的密钥索引
 * args:    [in] password_len :私钥保护口令长度
 * args:    [in] password :私钥保护口令
 * return: none
 ******************************************************************************************************************/
void SDF_GenerateKeyPair_ECC(packet_head **sdf_cmd_head,sdf_gen_asym_key **p)
{
	unsigned int i=0,asym_num=0;

	printfS("\n SDF_GenerateKeyPair_ECC start \n");

	if(((*p)->in_password_len==0)||((*p)->in_password_len>16))
	{
		printfS("password_len=%d error\n",(*p)->in_password_len);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if((*p)->in_password==NULL)
	{
		printfS("password error\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	for(i=0;i<ASYM_KEY_NUM;i++)
	{
		if(sdf_asym_key_pool[i].key_status!=SDF_KEY_ACTIVE)
		{
			if(0==sdf_generate_key(i,1,(*p)->in_password_len,(*p)->in_password))
			{
				printfS("gen asym key index=%d success\n",i);
				if(0!=sdf_key_init())
				{
					printfS("sdf key init error\n");
					(*sdf_cmd_head)->sdf_cmd_head.status=SDR_UNKNOWERR;
					return;
				}
				(*p)->in_out_key_index=i;
				(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
				return;
			}
		}
		asym_num++;
	}
	if(asym_num>=ASYM_KEY_NUM)
	{
		printfS("aysm key is full\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_NOBUFFER;
		return;
	}
	printfS("\n SDF_GenerateKeyPair_ECC end \n");
}
/*****************************************************************************************************************
 * private F function: 删除非对称密钥
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] key_index :生成的密钥索引
 * args:    [in] password_len :私钥保护口令长度
 * args:    [in] password :私钥保护口令
 * return: none
 ******************************************************************************************************************/
void SDF_DeleteKeyPair_ECC(packet_head **sdf_cmd_head,sdf_gen_asym_key **p)
{
	printfS("\n SDF_DeleteKeyPair_ECC start \n");

	if(((*p)->in_password_len==0)||((*p)->in_password_len>16))
	{
		printfS("password_len=%d error\n",(*p)->in_password_len);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if((*p)->in_password==NULL)
	{
		printfS("password error\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if(sdf_asym_key_pool[(*p)->in_out_key_index].key_status!=SDF_KEY_ACTIVE)
	{
		printfS("key_index=%d status error\n",sdf_asym_key_pool[(*p)->in_out_key_index].key_status);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
		return;
	}
	if((*p)->in_password_len!=sdf_asym_key_pool[(*p)->in_out_key_index].key_password_len)
	{
		printfS("password_len=%d not same\n",(*p)->in_password_len);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if(memcmp(sdf_asym_key_pool[(*p)->in_out_key_index].key_password, (*p)->in_password, (*p)->in_password_len) != 0)
	{
		printfS("password not same\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if(0!=sdf_delete_key((*p)->in_out_key_index,1,(*p)->in_password_len,(*p)->in_password))
	{
		printfS("sdf_delete_key error\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_UNKNOWERR;
		return;
	}
	sdf_asym_key_pool[(*p)->in_out_key_index].key_status=SDF_KEY_BLANK;
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	printfS("\n SDF_DeleteKeyPair_ECC end \n");
}
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
void SDF_InternalSign_ECC(packet_head **sdf_cmd_head,sdf_sm2_server **p)
{
	int r_len,s_len;

	printfS("\n SDF_InternalSign_ECC start \n");
	if(((*p)->in_password_len==0)||((*p)->in_password_len>16))
	{
		printfS("password_len=%d error\n",(*p)->in_password_len);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if((*p)->in_password==NULL)
	{
		printfS("password error\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if((*p)->in_key_index>SYM_KEY_NUM)
	{
		printfS("key_index=%d error\n",(*p)->in_key_index);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if(sdf_asym_key_pool[(*p)->in_key_index].key_status!=SDF_KEY_ACTIVE)
	{
		printfS("key_index=%d status error\n",sdf_asym_key_pool[(*p)->in_key_index].key_status);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_KEYNOTEXIST;
		return;
	}
	if((*p)->in_password_len!=sdf_asym_key_pool[(*p)->in_key_index].key_password_len)
	{
		printfS("password_len=%d not same\n",(*p)->in_password_len);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if(memcmp(sdf_asym_key_pool[(*p)->in_key_index].key_password, (*p)->in_password, (*p)->in_password_len) != 0)
	{
		printfS("password not same\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	sm2_sign((*p)->in_hash_data,32,sdf_asym_key_pool[(*p)->in_key_index].sig_pri_key,32,(*p)->in_out_r_s_value,&r_len,(*p)->in_out_r_s_value+32,&s_len);
	if((s_len!=32)||(r_len!=32))
	{
		printfS("sm2 sign error\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_SIGNERR;
		return;
	}
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	printfS("\n SDF_InternalSign_ECC end \n");
}
/*****************************************************************************************************************
 * private F function: 使用内部ECC公钥对数据进行验证运算
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] key_index :密钥索引
 * args:    [in] in_data_len :输入数据长度
 * args:    [in] in_data :输入数据
 * args:    [in] r_s_value:签名数据
 * return: none
 ******************************************************************************************************************/
void SDF_InternalVerify_ECC(packet_head **sdf_cmd_head,sdf_sm2_server **p)
{
	printfS("\n SDF_InternalVerify_ECC start \n");
	if((*p)->in_key_index>SYM_KEY_NUM)
	{
		printfS("key_index=%d error\n",(*p)->in_key_index);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if(sdf_asym_key_pool[(*p)->in_key_index].key_status!=SDF_KEY_ACTIVE)
	{
		printfS("key_index=%d status error\n",sdf_asym_key_pool[(*p)->in_key_index].key_status);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_KEYNOTEXIST;
		return;
	}
	if(0!=sm2_verify((*p)->in_hash_data,(*p)->in_hash_data_len,(*p)->in_out_r_s_value,32,(*p)->in_out_r_s_value+32,32,sdf_asym_key_pool[(*p)->in_key_index].sig_pub_key,32, sdf_asym_key_pool[(*p)->in_key_index].sig_pub_key+32,32))
	{
		printfS("sm2 verify error\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_VERIFYERR;
		return;
	}
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	printfS("\n SDF_InternalVerify_ECC end \n");
}
/*****************************************************************************************************************
 * private F function: 使用外部ECC公钥对数据进行验证运算
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] in_data_len :输入数据长度
 * args:    [in] in_data :输入数据
 * args:    [in] r_s_value:签名数据
 * return: none
 ******************************************************************************************************************/
void SDF_ExternalVerify_ECC(packet_head **sdf_cmd_head,sdf_extern_ecc_verify **p)
{
	printfS("\n SDF_ExternalVerify_ECC start \n");
	if(0!=sm2_verify((*p)->in_hash_data,32,(*p)->in_out_r_s_value,32,(*p)->in_out_r_s_value+32,32,(*p)->in_pub_key,32, (*p)->in_pub_key+32,32))
	{
		printfS("sm2 verify error\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_VERIFYERR;
		return;
	}
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	printfS("\n SDF_ExternalVerify_ECC end \n");
}
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
void SDF_GenerateAgreementDataWithECC(packet_head **sdf_cmd_head,sdf_gen_agreement_par **p)
{
	int pub_x_len,pub_y_len,rng_len;

	printfS("\n SDF_GenerateAgreementDataWithECC start \n");
	if((*p)->in_key_index>SYM_KEY_NUM)
	{
		printfS("key_index=%d error\n",(*p)->in_key_index);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if(sdf_asym_key_pool[(*p)->in_key_index].key_status!=SDF_KEY_ACTIVE)
	{
		printfS("key_index=%d status error\n",sdf_asym_key_pool[(*p)->in_key_index].key_status);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_KEYNOTEXIST;
		return;
	}
	if((*p)->in_password_len!=sdf_asym_key_pool[(*p)->in_key_index].key_password_len)
	{
		printfS("password_len=%d not same\n",(*p)->in_password_len);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if(memcmp(sdf_asym_key_pool[(*p)->in_key_index].key_password, (*p)->in_password, (*p)->in_password_len) != 0)
	{
		printfS("password not same\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	sm2_keyagreement_a1_3((*p)->out_A_mp_pubkey, &pub_x_len, (*p)->out_A_mp_pubkey+32,&pub_y_len, (*p)->out_handle.rng, &rng_len);
	if((pub_x_len!=32)||(pub_y_len!=32)||(rng_len!=32))
	{
		printfS("pub_x_len=%d,pub_y_len=%d,rng_len=%d  error\n",pub_x_len,pub_y_len,rng_len);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_UNKNOWERR;
		return;
	}
	printfS("out_A_mp_pubkey\n");
	PrintBuf((*p)->out_A_mp_pubkey,64);
	printfS("rng\n");
	PrintBuf((*p)->out_handle.rng,32);
	memcpy((*p)->out_A_pubkey,sdf_asym_key_pool[(*p)->in_key_index].enc_pub_key,64);

	(*p)->out_handle.A_id_len=(*p)->in_A_id_length;
	memcpy((*p)->out_handle.A_id,(*p)->in_A_ID,(*p)->in_A_id_length);
	(*p)->out_handle.A_prikey_index=(*p)->in_key_index;
	memcpy((*p)->out_handle.A_mp_pubkey,(*p)->out_A_mp_pubkey,64);
	memcpy((*p)->out_handle.A_pubkey,(*p)->out_A_pubkey,64);
	(*p)->out_handle.session_key_len=(*p)->in_key_bits/8;
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	printfS("\n SDF_GenerateAgreementDataWithECC end \n");
}
/*****************************************************************************************************************
 * private F function: 产生协商数据并计算会话密钥
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] in_key_index :密钥索引
 * args:    [in] in_password_len:密钥私钥保护口令长度
 * args:    [in] in_password:密钥私钥保护口令
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
void SDF_GenerateAgreementDataAndKeyWithECC( packet_head **sdf_cmd_head,sdf_gen_agreement_par_and_key **p)
{
	int m_pub_key_x_len,m_pub_key_y_len;

	printfS("\n SDF_GenerateAgreementDataAndKeyWithECC start \n");
	if((*p)->in_key_index>SYM_KEY_NUM)
	{
		printfS("key_index=%d error\n",(*p)->in_key_index);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if(sdf_asym_key_pool[(*p)->in_key_index].key_status!=SDF_KEY_ACTIVE)
	{
		printfS("key_index=%d status error\n",sdf_asym_key_pool[(*p)->in_key_index].key_status);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_KEYNOTEXIST;
		return;
	}
	if((*p)->in_password_len!=sdf_asym_key_pool[(*p)->in_key_index].key_password_len)
	{
		printfS("password_len=%d not same\n",(*p)->in_password_len);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if(memcmp(sdf_asym_key_pool[(*p)->in_key_index].key_password, (*p)->in_password, (*p)->in_password_len) != 0)
	{
		printfS("password not same\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	printfS("in_A_mp_pubkey\n");
	PrintBuf((*p)->in_A_mp_pubkey,64);
	printfS("in_A_pubkey\n");
	PrintBuf((*p)->in_A_pubkey,64);
	printfS("in_B_prikey\n");
	PrintBuf(sdf_asym_key_pool[(*p)->in_key_index].enc_pri_key,32);
	printfS("in_B_pubkey\n");
	PrintBuf(sdf_asym_key_pool[(*p)->in_key_index].enc_pub_key,64);

	if(1!=sm2_keyagreement_b1_9(
			(*p)->in_A_mp_pubkey, 32,
			(*p)->in_A_mp_pubkey+32, 32,
			(*p)->in_A_pubkey, 32,
			(*p)->in_A_pubkey+32, 32,
			sdf_asym_key_pool[(*p)->in_key_index].enc_pri_key, 32,
			sdf_asym_key_pool[(*p)->in_key_index].enc_pub_key, 32,
			sdf_asym_key_pool[(*p)->in_key_index].enc_pub_key+32, 32,
			(*p)->in_A_ID, (*p)->in_A_id_length,
			(*p)->in_B_ID, (*p)->in_B_id_length,
			(*p)->in_key_bits/8,
			(*p)->out_session_key,
			(*p)->out_B_mp_pubkey, &m_pub_key_x_len,
			(*p)->out_B_mp_pubkey+32, &m_pub_key_y_len,
	                                                  NULL,NULL,
													  NULL,NULL,
	                                                  NULL))
	{
		printfS("sm2 error\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_UNKNOWERR;
		return;
	}
	printfS("out_B_mp_pubkey\n");
	PrintBuf((*p)->out_B_mp_pubkey,64);
	if((m_pub_key_x_len!=32)||(m_pub_key_y_len!=32))
	{
		printfS("m_pub_key_x_len=%d,m_pub_key_y_len=%d  error\n",m_pub_key_x_len,m_pub_key_y_len);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_UNKNOWERR;
		return;
	}
	memcpy((*p)->out_B_pubkey,sdf_asym_key_pool[(*p)->in_key_index].enc_pub_key,64);
	(*p)->out_session_key_len=(*p)->in_key_bits/8;
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	(*sdf_cmd_head)->sdf_cmd_head.len=sizeof(packet_head)+sizeof(sdf_gen_agreement_par_and_key)+(*p)->out_session_key_len;
	PrintBuf((*p)->out_session_key,(*p)->in_key_bits/8);
	printfS("\n SDF_GenerateAgreementDataAndKeyWithECC end \n");
}
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
void SDF_GenerateKeyWithECC(packet_head **sdf_cmd_head,sdf_count_session_key **p)
{
	unsigned char s1[32],sa[32];

	printfS("\n SDF_GenerateKeyWithECC start \n");
	printfS("A_mp_pubkey\n");
	PrintBuf((*p)->in_handle.A_mp_pubkey,64);
	printfS("A_pubkey\n");
	PrintBuf((*p)->in_handle.A_pubkey,64);
	printfS("A_prikey\n");
	PrintBuf(sdf_asym_key_pool[(*p)->in_handle.A_prikey_index].enc_pri_key,32);
	printfS("in_B_pubkey\n");
	PrintBuf((*p)->in_B_pubkey,64);
	printfS("A_id\n");
	PrintBuf((*p)->in_handle.A_id,(*p)->in_handle.A_id_len);
	printfS("in_B_ID\n");
	PrintBuf((*p)->in_B_ID,(*p)->in_B_id_length);
	printfS("in_B_mp_pubkey\n");
	PrintBuf((*p)->in_B_mp_pubkey,64);
	printfS("rng\n");
	PrintBuf((*p)->in_handle.rng,32);
	if(0==sm2_keyagreement_a4_10(
	                                                  (*p)->in_handle.A_mp_pubkey, 32,
													  (*p)->in_handle.A_mp_pubkey+32, 32,
													  (*p)->in_handle.A_pubkey, 32,
													  (*p)->in_handle.A_pubkey+32, 32,
													  sdf_asym_key_pool[(*p)->in_handle.A_prikey_index].enc_pri_key, 32,
													  (*p)->in_B_pubkey, 32,
													  (*p)->in_B_pubkey+32, 32,
													  (*p)->in_handle.A_id, (*p)->in_handle.A_id_len,
													  (*p)->in_B_ID, (*p)->in_B_id_length,
													  (*p)->in_B_mp_pubkey, 32,
													  (*p)->in_B_mp_pubkey+32, 32,
													  (*p)->in_handle.rng, 32,
													  (*p)->in_handle.session_key_len,
													  (*p)->out_session_key,
	                                                  s1,
	                                                  sa
	                                                  ))
	{
		printfS("sm2  error\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_UNKNOWERR;
		return;
	}
	(*p)->out_session_key_len=(*p)->in_handle.session_key_len;
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	(*sdf_cmd_head)->sdf_cmd_head.len=sizeof(packet_head)+sizeof(sdf_count_session_key)+(*p)->out_session_key_len;
	PrintBuf( (*p)->out_session_key,(*p)->out_session_key_len);
	printfS("\n SDF_GenerateAgreementDataAndKeyWithECC end \n");
}

/*****************************************************************************************************************
 * private F function: 导出ECC加密公钥
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] in_key_index :密钥索引
 * args:    [out] out_pub_key  :输出公钥
 * return: none
 ******************************************************************************************************************/
void SDF_ExportEncPublicKey_ECC(packet_head **sdf_cmd_head,sdf_export_ecc_pub_key **p)
{
	printfS("\n SDF_ExportEncPublicKey_ECc start \n");
	if((*p)->in_index>=ASYM_KEY_NUM)
	{
		printfS("asym key in_index=%d error\n",(*p)->in_index);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if(sdf_asym_key_pool[(*p)->in_index].key_status!=SDF_KEY_ACTIVE)
	{
		printfS("asym key key_status error\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_KEYNOTEXIST;
		return;
	}
	memcpy((*p)->out_pub_key,sdf_asym_key_pool[(*p)->in_index].enc_pub_key,64);
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	printfS("\n SDF_ExportEncPublicKey_ECc end \n");
}

/*****************************************************************************************************************
 * private F function: 导出ECC签名公钥
 * args:    [in] sdf_cmd_head :包头
 * args:    [in] in_key_index :密钥索引
 * args:    [out] out_pub_key  :输出公钥
 * return: none
 ******************************************************************************************************************/
void SDF_ExportSignPublicKey_ECC(packet_head **sdf_cmd_head,sdf_export_ecc_pub_key **p)
{
	printfS("\n SDF_ExportSignPublicKey_ECC start \n");
	if((*p)->in_index>=ASYM_KEY_NUM)
	{
		printfS("asym key in_index=%d error\n",(*p)->in_index);
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_INARGERR;
		return;
	}
	if(sdf_asym_key_pool[(*p)->in_index].key_status!=SDF_KEY_ACTIVE)
	{
		printfS("asym key key_status error\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=SDR_KEYNOTEXIST;
		return;
	}
	memcpy((*p)->out_pub_key,sdf_asym_key_pool[(*p)->in_index].sig_pub_key,64);
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	printfS("\n SDF_ExportSignPublicKey_ECC end \n");
}


/*****************************************************************************************************************
 * private F function: 将一个会话信息复制到一个新的会话
 * args:    [in] sdf_cmd_head :包头
 * args:    [out] sdf_out_data
 * return: none
 ******************************************************************************************************************/
void SDF_Hash_Copy(packet_head **sdf_cmd_head,packet_head *sdf_out_data)
{
	packet_head *p=sdf_out_data;

	printfS("\n SDF_Hash_Copy start \n");
	memcpy(p,*sdf_cmd_head,sizeof(packet_head));
	SDF_OpenSession((packet_head **)&p);
	if(p->sdf_cmd_head.status!=SDR_OK)
	{
		printfS("SDF_OpenSession error\n");
		(*sdf_cmd_head)->sdf_cmd_head.status=p->sdf_cmd_head.status;
		return;
	}
	memcpy(p->sdf_device_info.session_list_head->sm3_par_p,(*sdf_cmd_head)->sdf_device_info.session_list_head->sm3_par_p,sizeof(SM3_CTX));
	(*sdf_cmd_head)->sdf_cmd_head.len=2*sizeof(packet_head);
	(*sdf_cmd_head)->sdf_cmd_head.status=SDR_OK;
	printfS("\n SDF_Hash_Copy end \n");
}
