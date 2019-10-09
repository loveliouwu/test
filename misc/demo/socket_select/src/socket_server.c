/*
 ============================================================================
 Name        : socket.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "../include/common.h"
#include<inttypes.h>
#include "../include/sdf.h"
#include "../alg/alg_test.h"
#include "../include/sdf_manage.h"


void sdf_key_test()
{
	int i=0;
	unsigned char passward[]={"hello"};

	if(-1==sdf_key_init())
	{
		printfS("init error\n");
		while(1);
	}
	for(i=0;i<4;i++)
	{
		if(-1==sdf_generate_key(i,0,0,NULL))
		{
			printfS("sdf_generate_key sym index=%d error\n",i);
			while(1);
		}
		if(-1==sdf_generate_key(i,1,3,passward))
		{
			printfS("sdf_generate_key asym index=%d error\n",i);
			while(1);
		}
	}
	if(-1==sdf_key_init())
	{
		printfS("init error\n");
		while(1);
	}
	for(i=0;i<SYM_KEY_NUM;i++)
	{
		if(-1==sdf_delete_key(i,0,0,NULL))
		{
			printfS("sdf_delete_key sym index=%d error\n",i);
			while(1);
		}
		if(-1==sdf_delete_key(i,1,3,passward))
		{
			printfS("sdf_delete_key asym index=%d error\n",i);
			while(1);
		}
	}
	if(-1==sdf_key_init())
	{
		printfS("init error\n");
		while(1);
	}
}
void test_init()
{
	sdf_asym_key_pool=(sdf_pool_asym_key*)malloc(ASYM_KEY_NUM*sizeof(sdf_pool_asym_key));
	if(-1==sdf_key_init())
	{
		printfS("sdf_key_init error\n");
		while(1);
	}
	else
		return ;
}
void change_key_test()
{
	unsigned char ida[19] = "ALICE123@YAHOO.COM";
	unsigned char idb[18] = "BILL456@YAHOO.COM";

	packet_head *p=(packet_head *)malloc(sizeof(packet_head)*1000);
	test_init();
	sdf_gen_agreement_par *sdf_cmd_head=(sdf_gen_agreement_par *)malloc(sizeof(sdf_gen_agreement_par));
	sdf_cmd_head->in_A_id_length=18;
	memcpy(sdf_cmd_head->in_A_ID,ida,sdf_cmd_head->in_A_id_length);
	sdf_cmd_head->in_key_bits=128;
	sdf_cmd_head->in_key_index=0;
	sdf_cmd_head->in_password_len=3;
	memcpy(sdf_cmd_head->in_password,idb,sdf_cmd_head->in_password_len);
	SDF_GenerateAgreementDataWithECC(&p,&sdf_cmd_head);

	sdf_gen_agreement_par_and_key *sdf_cmd_head1=(sdf_gen_agreement_par_and_key *)malloc(sizeof(sdf_gen_agreement_par_and_key));
	sdf_cmd_head1->in_key_index=1;
	sdf_cmd_head1->in_A_id_length=18;
	memcpy(sdf_cmd_head1->in_A_ID,ida,sdf_cmd_head1->in_A_id_length);
	sdf_cmd_head1->in_key_bits=128;
	sdf_cmd_head1->in_B_id_length=17;
	memcpy(sdf_cmd_head1->in_B_ID,idb,sdf_cmd_head1->in_B_id_length);
	memcpy(sdf_cmd_head1->in_A_mp_pubkey,sdf_cmd_head->out_A_mp_pubkey,64);
	memcpy(sdf_cmd_head1->in_A_pubkey,sdf_cmd_head->out_A_pubkey,64);
	sdf_cmd_head1->in_password_len=3;
	memcpy(sdf_cmd_head1->in_password,ida,sdf_cmd_head1->in_password_len);
	SDF_GenerateAgreementDataAndKeyWithECC(&p,&sdf_cmd_head1);
	printfS("sdf_cmd_head1 session key index=%d\n",sdf_cmd_head1->out_session_key_len);
	PrintBuf(sdf_cmd_head1->out_session_key,sdf_cmd_head1->out_session_key_len);

	sdf_count_session_key *sdf_cmd_head2=(sdf_count_session_key *)malloc(sizeof(sdf_count_session_key));
	sdf_cmd_head2->in_B_id_length=17;
	memcpy(sdf_cmd_head2->in_B_ID,idb,sdf_cmd_head2->in_B_id_length);
	memcpy(sdf_cmd_head2->in_B_mp_pubkey,sdf_cmd_head1->out_B_mp_pubkey,64);
	memcpy(sdf_cmd_head2->in_B_pubkey,sdf_cmd_head1->out_B_pubkey,64);
	memcpy((unsigned char *)&sdf_cmd_head2->in_handle,(unsigned char *)&sdf_cmd_head->out_handle,sizeof(sdf_gen_agreement_par_handle));
	SDF_GenerateKeyWithECC(&p,&sdf_cmd_head2);
	printfS("sdf_cmd_head2 session key index=%d\n",sdf_cmd_head2->out_session_key_len);
	PrintBuf(sdf_cmd_head2->out_session_key,sdf_cmd_head2->out_session_key_len);
	printfS("key_test OK ! \n");

	SDF_CloseDevice(&p);
	free_point(p);
	free_point(sdf_cmd_head);
	free_point(sdf_cmd_head1);
	free_point(sdf_cmd_head2);
}


void *test_sm4(void *num)
{
	/*unsigned char input_f[32] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	unsigned char key[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	unsigned char iv[16] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};
	unsigned char output_f[2048],compare[32];
	unsigned long i,j=0;

	unsigned char *input;
	unsigned char *output;
	unsigned int block_num=2;

	printfS("=======================*((int *)num)=%d=======================\n",*((int *)num));
	input_f[0]=*((int *)num);
	key[0]=*((int *)num);
	iv[0]=*((int *)num);
	printfS("=======================ecb input_f[0]=%d=======================\n",input_f[0]);
	//encrypt ecb
	input = input_f;
	output= output_f;
	sm4_ecb(key,0,block_num,input,output);
	//PrintBuf(output_f,16*block_num);
	sm4_ecb(key,1,block_num,output,compare);
	//PrintBuf(compare,16*block_num);
	for(j=0;j<16*block_num;j++)
	{
		if(compare[j]!=input[j])
		{
			printfS("ecb error\n");
			return NULL;
		}
	}

	printfS("=======================cbc=======================\n");
	//encrypt cbc
	input = input_f;
	output= output_f;
	sm4_cbc(key,0,block_num,iv,input,output);
	//PrintBuf(output,16*block_num);
	sm4_cbc(key,1,block_num,iv,output,compare);
	//PrintBuf(compare,16*block_num);
	for(j=0;j<16*block_num;j++)
	{
		if(compare[j]!=input[j])
		{
			printfS("cbc error\n");
			return NULL;
		}
	}
	*/
	printfS("=======================*((int *)num)=%d=======================\n",*((int *)num));
	sm2_test();
	sm3_test();
	sm4_test();

	return NULL;
}

int  sm4_thread_test()
{
	pthread_t ntid[10];
	int i=0;

	for(i=0;i<10;i++)
	{
		int err = pthread_create(&ntid[i],NULL,test_sm4,&i);
		if(err!=0)
		{
			printfS("thread_create Failed\n");
			return -1;
		}
		pthread_join(ntid[i],NULL);

	}
	return 0;
}


int main(void)
{
	//sm4_test();
	socket_epoll();
	test_init();
	socket_server_test();
	return EXIT_SUCCESS;
}


