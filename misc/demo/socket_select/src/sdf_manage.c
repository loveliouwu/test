/*
 * sdf_manage.c
 *
 *  Created on: 2019年8月19日
 *      Author: jjn
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/common.h"
#include "../include/sdf_manage.h"
#include "miracl/miracl.h"
#include "../alg/sm4.h"
#include "../alg/sm2.h"
#include "unistd.h"
#include "stdatomic.h"

unsigned char sdf_device_key[16]={0x1,0x11,0x34,0x45,0x23,0x12,0x1,0x80,0xa,0xd,0xad,0xcd,0xcd,0xa,0xed,0xad};
sdf_pool_asym_key* sdf_asym_key_pool;


/*****************************************************************************************************************
 * private F function: 把文件中密钥密文解密成明文进行存储
 * return: 0 success,-1 error
 ******************************************************************************************************************/
int sdf_key_init()
{
	FILE *sdf_key_file=NULL;
	unsigned int i=0,asym_fill_data_len,len;
	unsigned char *dec_input,*dec_output;

	printfS("\n sdf_key_init \n");
	asym_fill_data_len=16-(sizeof(sdf_pool_asym_key)%16);
	sdf_key_file=fopen(sdf_sym_key_file_name, "at+");
	if(sdf_key_file==NULL)
	{
		printfS("open key file error\n");
		return -1;
	}
    fseek(sdf_key_file,0L,SEEK_END); /* 定位到文件末尾 */
    len=ftell(sdf_key_file); /* 得到文件大小 */
    if(len!=((sizeof(sdf_pool_asym_key)+asym_fill_data_len)*ASYM_KEY_NUM))
    {
    	printfS("file size=%d error\n",len);
    	printfS("creat file\n");
    	remove(sdf_sym_key_file_name);
    	sdf_key_file=fopen(sdf_sym_key_file_name, "at+");
		if(sdf_key_file==NULL)
		{
			printfS("creat file error\n");
			return -1;
		}
    	fseek(sdf_key_file, 0, SEEK_SET);
    	for(i=0;i<ASYM_KEY_NUM;i++)
    	{
    		len=fwrite(sdf_asym_key_pool,sizeof(char),asym_fill_data_len+sizeof(sdf_pool_asym_key),sdf_key_file);
			if(len!=(asym_fill_data_len+sizeof(sdf_pool_asym_key)))
			{
				printfS("creat file write asym file len=%d error\n",len);
				fclose(sdf_key_file);
				return -1;
			}
    	}
    }
	fseek(sdf_key_file, 0, SEEK_SET);
	//asym key
	dec_input=(unsigned char*)malloc(asym_fill_data_len+sizeof(sdf_pool_asym_key));
	dec_output=(unsigned char*)malloc(asym_fill_data_len+sizeof(sdf_pool_asym_key));
	for(i=0;i<ASYM_KEY_NUM;i++)
	{
		len=fread(dec_input,sizeof(char),asym_fill_data_len+sizeof(sdf_pool_asym_key),sdf_key_file);
		if(len!=(asym_fill_data_len+sizeof(sdf_pool_asym_key)))
		{
			printfS("len=0x%x\n",len);
			free_point(dec_input);
			free_point(dec_output);
			fclose(sdf_key_file);
			return -1;
		}
		sm4_ecb((unsigned char *)sdf_device_key,SM4_DEC,len/16,dec_input,dec_output);
		memcpy((unsigned char *)sdf_asym_key_pool+i*(sizeof(sdf_pool_asym_key)),dec_output,sizeof(sdf_pool_asym_key));
		if(sdf_asym_key_pool[i].key_status==SDF_KEY_ACTIVE)
		{
			printfS("asym index=%d,status=%d\n",i,sdf_asym_key_pool[i].key_status);
			printfS("key password\n");
			PrintBuf(sdf_asym_key_pool[i].key_password,sdf_asym_key_pool[i].key_password_len);
			printfS("enc pub key\n");
			PrintBuf(sdf_asym_key_pool[i].enc_pub_key,64);
			printfS("enc pri key\n");
			PrintBuf(sdf_asym_key_pool[i].enc_pri_key,32);
			printfS("sign pub key\n");
			PrintBuf(sdf_asym_key_pool[i].sig_pub_key,64);
			printfS("sign pri key\n");
			PrintBuf(sdf_asym_key_pool[i].sig_pri_key,32);
		}
	}
	free_point(dec_input);
	free_point(dec_output);
	fclose(sdf_key_file);
	return 0;
}
/*****************************************************************************************************************
 * private F function: 生成一组对称密钥
 * args:    [in] key_index :生成密钥索引
 * args:    [in] sym_asym_flag : 0对称，1非对称
 * args:    [in] password_len : 私钥保护口令长度
 * args:    [in] password : 私钥保护口令
 * return: 0 success,-1 error,1 密钥已经存在
 ******************************************************************************************************************/
int sdf_generate_key(unsigned int key_index,unsigned int sym_asym_flag,unsigned int password_len,unsigned char *password)
{
	FILE *sdf_key_file=NULL;
	sdf_pool_asym_key asym_key;
	int i=0,len,asym_fill_data_len;
	int pub_x_len,pub_y_len,pri_len;
	unsigned char *enc_input,*enc_output;

	printfS("\n sdf_generate_key \n");
	asym_fill_data_len=16-(sizeof(sdf_pool_asym_key)%16);
	sdf_key_file=fopen(sdf_sym_key_file_name, "rb+");
	if(sdf_key_file==NULL)
	{
		printfS("open key file error\n");
		return -1;
	}
	if(sym_asym_flag==0)
	{
		printfS("sym key\n");
	}
	else if(sym_asym_flag==1)
	{
		if(key_index>ASYM_KEY_NUM)
		{
			printfS("key_index=%d error\n",key_index);
			fclose(sdf_key_file);
			return -1;
		}
		if((password_len>16)||(password_len==0))
		{
			printfS("key password len=%d error\n",password_len);
			fclose(sdf_key_file);
			return -1;
		}
		if((password==NULL))
		{
			printfS("password  error\n");
			fclose(sdf_key_file);
			return -1;
		}
		if(sdf_asym_key_pool[key_index].key_status==SDF_KEY_ACTIVE)
		{
			printfS("key have exist\n");
			fclose(sdf_key_file);
			return 1;
		}
		asym_key.key_status=SDF_KEY_ACTIVE;
		for(i=0;i<SESSION_KEY_NUM;i++)
			asym_key.session_private_limit[i]=0;
		enc_input=(unsigned char*)malloc(asym_fill_data_len+sizeof(sdf_pool_asym_key));
		enc_output=(unsigned char*)malloc(asym_fill_data_len+sizeof(sdf_pool_asym_key));
		if(enc_input==NULL)
		{
			printfS("enc_input malloc error\n");
			fclose(sdf_key_file);
			return -1;
		}
		if(enc_output==NULL)
		{
			printfS("enc_output malloc error\n");
			free_point(enc_input);
			fclose(sdf_key_file);
			return -1;
		}
		sm2_keygen((unsigned char *)asym_key.enc_pub_key,(int *)&pub_x_len, (unsigned char *)(asym_key.enc_pub_key+32),
						(int *)&pub_y_len,(unsigned char *)asym_key.enc_pri_key ,(int *)&pri_len);
		sleep(1);
		sm2_keygen(asym_key.sig_pub_key,&pub_x_len, asym_key.sig_pub_key+32,&pub_y_len,(unsigned char *)asym_key.sig_pri_key ,(int *)&pri_len);

		asym_key.key_password_len=password_len;
		memcpy(asym_key.key_password,password,password_len);
		memcpy(enc_input,&asym_key,sizeof(sdf_pool_asym_key));
		memset(enc_input+sizeof(sdf_pool_asym_key),asym_fill_data_len,asym_fill_data_len);
		fseek(sdf_key_file, (asym_fill_data_len+sizeof(sdf_pool_asym_key))*key_index, SEEK_SET);
		sm4_ecb((unsigned char *)sdf_device_key,SM4_ENC,(asym_fill_data_len+sizeof(sdf_pool_asym_key))/16,enc_input,enc_output);
		len=fwrite(enc_output,sizeof(char),asym_fill_data_len+sizeof(sdf_pool_asym_key),sdf_key_file);
		if(len!=(asym_fill_data_len+sizeof(sdf_pool_asym_key)))
		{
			printfS("len=0x%x\n",len);
			free_point(enc_input);
			free_point(enc_output);
			fclose(sdf_key_file);
			return -1;
		}
		free_point(enc_input);
		free_point(enc_output);
		//write key pool
		memcpy((unsigned char *)&sdf_asym_key_pool[key_index],(unsigned char*)&asym_key,sizeof(sdf_pool_asym_key));
		printfS("asym key index=%d\n",key_index);
		printfS("password \n");
		PrintBuf(sdf_asym_key_pool[key_index].key_password,sdf_asym_key_pool[key_index].key_password_len);
		printfS("enc pub key\n");
		PrintBuf(sdf_asym_key_pool[key_index].enc_pub_key,64);
		printfS("enc pri key\n");
		PrintBuf(sdf_asym_key_pool[key_index].enc_pri_key,32);
		printfS("sign pub key\n");
		PrintBuf(sdf_asym_key_pool[key_index].sig_pub_key,64);
		printfS("sign pri key\n");
		PrintBuf(sdf_asym_key_pool[key_index].sig_pri_key,32);
	}
	else
	{
		printfS("sym_asym_flag error ,it is 0 or 1\n");
		fclose(sdf_key_file);
		return -1;
	}
	fclose(sdf_key_file);
	return 0;
}
/*****************************************************************************************************************
 * private F function: 删除一组密钥
 * args:    [in] key_index :删除密钥索引
 * args:    [in] sym_asym_flag : 0对称，1非对称
 * args:    [in] password_len : 私钥保护口令长度
 * args:    [in] password : 私钥保护口令
 * return: 0 success,-1 error
 ******************************************************************************************************************/
int sdf_delete_key(unsigned int key_index,unsigned int sym_asym_flag,unsigned int password_len,unsigned char *password)
{
	FILE *sdf_key_file=NULL;
	sdf_pool_asym_key asym_key;
	int len,asym_fill_data_len;

	printfS("\n sdf_delete_key \n");
	asym_fill_data_len=16-(sizeof(sdf_pool_asym_key)%16);
	sdf_key_file=fopen(sdf_sym_key_file_name, "rb+");
	if(sdf_key_file==NULL)
	{
		printfS("open key file error\n");
		return -1;
	}
	if(sym_asym_flag==0)
	{
		printfS("sym key\n");
	}
	else if(sym_asym_flag==1)
	{
		if((password_len>16)||(password_len==0))
		{
			printfS("key password len=%d error\n",password_len);
			fclose(sdf_key_file);
			return -1;
		}
		if((password==NULL))
		{
			printfS("password  error\n");
			fclose(sdf_key_file);
			return -1;
		}

		if(key_index>ASYM_KEY_NUM)
		{
			printfS("key_index=%d error\n",key_index);
			fclose(sdf_key_file);
			return -1;
		}

		if(sdf_asym_key_pool[key_index].key_status!=SDF_KEY_ACTIVE)
			return 0;
		else
		{
			if(sdf_asym_key_pool[key_index].key_password_len!=password_len)
			{
				fclose(sdf_key_file);
				printfS("password len=%d error\n",sdf_asym_key_pool[key_index].key_password_len);
				return -1;
			}
			if(memcmp(password, sdf_asym_key_pool[key_index].key_password, sdf_asym_key_pool[key_index].key_password_len) != 0)
			{
				fclose(sdf_key_file);
				printfS("password error\n");
				return -1;
			}
			sdf_asym_key_pool[key_index].key_status=SDF_KEY_BLANK;
			memset(&asym_key,0,sizeof(sdf_pool_asym_key));
			fseek(sdf_key_file, (asym_fill_data_len+sizeof(sdf_pool_asym_key))*key_index, SEEK_SET);
			len=fwrite(&asym_key,sizeof(char),sizeof(sdf_pool_asym_key),sdf_key_file);
			if(len!=sizeof(sdf_pool_asym_key))
			{
				printfS("len=0x%x\n",len);
				fclose(sdf_key_file);
				return -1;
			}
		}
	}
	else
	{
		printfS("sym_asym_flag=%d error\n",sym_asym_flag);
		fclose(sdf_key_file);
		return -1;
	}
	fclose(sdf_key_file);
	return 0;
}

