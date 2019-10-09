/*
 * alg_test.c
 *
 *  Created on: 2019年8月20日
 *      Author: jjn
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sm2.h"
#include "sm3.h"
#include "sm4.h"
#include "../include/common.h"


void sm2_change_key_test()
{
        unsigned char ida[19] = "ALICE123@YAHOO.COM";
        unsigned char idb[18] = "BILL456@YAHOO.COM";
        unsigned char da[] = {0x6F,0xCB,0xA2,0xEF,0x9A,0xE0,0xAB,0x90,0x2B,0xC3,0xBD,0xE3,0xFF,0x91,0x5D,0x44,
        						0xBA,0x4C,0xC7,0x8F,0x88,0xE2,0xF8,0xE7,0xF8,0x99,0x6D,0x3B,0x8C,0xCE,0xED,0xEE};
        unsigned char xa[] = {0x30,0x99,0x09,0x3B,0xF3,0xC1,0x37,0xD8,0xFC,0xBB,0xCD,0xF4,0xA2,0xAE,0x50,0xF3,
        						0xB0,0xF2,0x16,0xC3,0x12,0x2D,0x79,0x42,0x5F,0xE0,0x3A,0x45,0xDB,0xFE,0x16,0x55};
        unsigned char ya[] = {0x3D,0xF7,0x9E,0x8D,0xAC,0x1C,0xF0,0xEC,0xBA,0xA2,0xF2,0xB4,0x9D,0x51,0xA4,0xB3,
        						0x87,0xF2,0xEF,0xAF,0x48,0x23,0x39,0x08,0x6A,0x27,0xA8,0xE0,0x5B,0xAE,0xD9,0x8B};
        unsigned char db[] = {0x5E,0x35,0xD7,0xD3,0xF3,0xC5,0x4D,0xBA,0xC7,0x2E,0x61,0x81,0x9E,0x73,0x0B,0x01,
        						0x9A,0x84,0x20,0x8C,0xA3,0xA3,0x5E,0x4C,0x2E,0x35,0x3D,0xFC,0xCB,0x2A,0x3B,0x53};
        unsigned char xb[] = {0x24,0x54,0x93,0xD4,0x46,0xC3,0x8D,0x8C,0xC0,0xF1,0x18,0x37,0x46,0x90,0xE7,0xDF,
        						0x63,0x3A,0x8A,0x4B,0xFB,0x33,0x29,0xB5,0xEC,0xE6,0x04,0xB2,0xB4,0xF3,0x7F,0x43};
        unsigned char yb[] = {0x53,0xC0,0x86,0x9F,0x4B,0x9E,0x17,0x77,0x3D,0xE6,0x8F,0xEC,0x45,0xE1,0x49,0x04,
        						0xE0,0xDE,0xA4,0x5B,0xF6,0xCE,0xCF,0x99,0x18,0xC8,0x5E,0xA0,0x47,0xC6,0x0A,0x4C};

        unsigned char kabuf[32], sa[32];
        unsigned char kbbuf[32], sb[32], s1[32], s2[32];
        unsigned char kx2[32], ky2[32];
        int kx2len, ky2len;
        unsigned char kx1[256], ky1[256], ra[256], xv[32], yv[32];
        int kx1len,ky1len, ralen, xvlen, yvlen;

        /////test
        sm2_keygen(xa, &kx1len, ya, &ky1len, da, &ralen);
        sm2_keygen(xb, &kx1len, yb, &ky1len, db, &ralen);

        sm2_keyagreement_a1_3(kx1, &kx1len, ky1, &ky1len, ra, &ralen);

        sm2_keyagreement_b1_9(
                                                  kx1, kx1len,
                                                  ky1, ky1len,
                                                  xa, 32,
                                                  ya, 32,
                                                  db, 32,
                                                  xb, 32,
                                                  yb, 32,
                                                  ida, 18,
                                                  idb, 17,
                                                  16,
                                                  kbbuf,
                                                  kx2, &kx2len,
                                                  ky2, &ky2len,
                                                  xv, &xvlen,
                                                  yv, &yvlen,
                                                  sb
                                                  );


        sm2_keyagreement_a4_10(
                                                  kx1, kx1len,
                                                  ky1, ky1len,
                                                  xa, 32,
                                                  ya, 32,
                                                  da, 32,
                                                  xb, 32,
                                                  yb, 32,
                                                  ida, 18,
                                                  idb, 17,
                                                  kx2, kx2len,
                                                  ky2, ky2len,
                                                  ra, ralen,
                                                  16,
                                                  kabuf,
                                                  s1,
                                                  sa
                                                  );

        sm2_keyagreement_b10(
                                                  xa, 32,
                                                  ya, 32,
                                                  xb, 32,
                                                  yb, 32,
                                                  kx1, 32,
                                                  ky1, 32,
                                                  kx2, 32,
                                                  ky2, 32,
                                                  xv, xvlen,
                                                  yv, yvlen,
                                                  ida, 18,
                                                  idb, 17,
                                                  s2
                );

        if(memcmp(s1, sb, 32) != 0)
        {
                printfS("key_test error ! \n");
                return;
        }
        if(memcmp(kabuf, kbbuf, 16) != 0)
        {
                printfS("key_test error ! \n");
                return;
        }

        if(memcmp(s2, sa, 32) != 0)
        {
                printfS("key_test error ! \n");
                return;
        }

        printfS("key_test OK ! \n");


}

void sm4_test()
{
	unsigned char input_f[2048] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	unsigned char key[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	unsigned char iv[16] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};
	unsigned char output_f[2048],compare[32];
	unsigned long i;

	unsigned char *input;
	unsigned char *output;
	unsigned int block_num=2;
	printfS("=======================ecb=======================\n");
	//encrypt ecb
	input = input_f;
	output= output_f;
	sm4_ecb(key,SM4_ENC,block_num,input,output);
		for(i=0;i<16*block_num;i++){
		  printfS("%02x ", output_f[i]);
		}
		printfS("\n");
	sm4_ecb(key,SM4_DEC,block_num,output,compare);
		for(i=0;i<16*block_num;i++){
		  printfS("%02x ", compare[i]);
		}
	printfS("\n");


	printfS("=======================cbc=======================\n");
	//encrypt cbc
	input = input_f;
	output= output_f;
	sm4_cbc(key,SM4_ENC,block_num,iv,input,output);
		for(i=0;i<16*block_num;i++){
		  printfS("%02x ", output[i]);
		}
		printfS("\n");
	sm4_cbc(key,SM4_DEC,block_num,iv,output,compare);
		for(i=0;i<16*block_num;i++){
		  printfS("%02x ", compare[i]);
		}
	printfS("\n");

	int loop_times=100000;
	clock_t start,end;
	double speed;

	start=clock();
	for(i=0;i<loop_times;i++)
	{
		sm4_ecb(key,SM4_ENC,128,input,output);
	}
	end=clock();
	printfS("sm4_ecb start:%lf\n", (double)start);
	printfS("sm4_ecb end:%lf\n", (double)end);
	speed = (double)(end-start)/CLOCKS_PER_SEC;
	printfS("sm4_ecb time:%lfs\n", (double)speed);
	printfS("sm4_ecb speed:%lfMbps\n", (double)loop_times*2*8/speed/1024);

	time_t timep,t_start,t_end;
	struct tm *p1,*p2;
	double start_,end_;

	t_start=time(NULL);

	//start_=p1->tm_hour*60*60+p1->tm_min*60+p1->tm_sec;
	for(i=0;i<loop_times;i++)
	{
		sm4_ecb(key,SM4_ENC,128,input,output);
	}
	t_end=time(NULL);
	p2=gmtime(&timep);
	//end_=p2->tm_hour*60*60+p2->tm_min*60+p2->tm_sec;
	printfS("sm4_ecb start:%lf\n", (double)start);
	printfS("sm4_ecb end:%lf\n", (double)end);
	speed = (double)difftime(t_end,t_start);
	printfS("sm4_ecb time:%lfs\n", (double)speed);
	printfS("sm4_ecb speed:%lfMbps\n", (double)loop_times*2*8/speed/1024);


	start=clock();
	for(i=0;i<loop_times;i++)
	{
		sm4_cbc(key,SM4_ENC,128,iv,input,output);
	}
	end=clock();
	printfS("sm4_cbc start:%lf\n", (double)start);
	printfS("sm4_cbc end:%lf\n", (double)end);
	speed = (double)(end-start)/CLOCKS_PER_SEC;
	printfS("sm4_cbc time:%lfs\n", (double)speed);
	printfS("sm4_cbc speed:%lfMbps\n", (double)loop_times*2*8/speed/1024);
}
void sm3_test()
{
	unsigned char input[2*1024],output[32];
	unsigned int i=0;

	for(i=0;i<3;i++)
	{
		input[i]=i+0x61;
		printfS("%02x", input[i]);
	}
	printfS("\n");
	SM3(input,3,output);
	for(i=0;i<32;i++)
	{
		printfS("%02x", output[i]);
	}
	printfS("\n");


	int loop_times=100000;
	clock_t start,end;
	double speed;

	start=clock();
	for(i=0;i<loop_times;i++)
	{
		SM3(input,2048,output);
	}
	end=clock();
	printfS("sm3 start:%lf\n", (double)start);
	printfS("sm3 end:%lf\n", (double)end);
	speed = (double)(end-start)/CLOCKS_PER_SEC;
	printfS("sm3 time:%lfs\n", (double)speed);
	printfS("sm3 speed:%lfMbps\n", (double)loop_times*2*8/speed/1024);

	memset(output,0,32);
	SM3_CTX *p=(SM3_CTX *)malloc(sizeof(SM3_CTX));
	SM3_CTX *q=(SM3_CTX *)malloc(sizeof(SM3_CTX));
	SM3_Init((SM3_CTX **)&p);
	memcpy(q,p,sizeof(SM3_CTX));
	SM3_Update((SM3_CTX **)&p, input,3);
	SM3_Final((SM3_CTX **)&p, output);
	PrintBuf(output,32);
	free(p);
	memset(output,0,32);
	SM3_Update((SM3_CTX **)&q, input,3);
	SM3_Final((SM3_CTX **)&q, output);
	PrintBuf(output,32);
	free(q);
}
void sm2_sign_verify_test()
{
	unsigned char pub_key[64]={0x0a,0xe4,0xc7,0x79,0x8a,0xa0,0xf1,0x19,0x47,0x1b,0xee,0x11,0x82,0x5b,0xe4,0x62
								,0x02,0xbb,0x79,0xe2,0xa5,0x84,0x44,0x95,0xe9,0x7c,0x04,0xff,0x4d,0xf2,0x54,0x8a
								,0x7c,0x02,0x40,0xf8,0x8f,0x1c,0xd4,0xe1,0x63,0x52,0xa7,0x3c,0x17,0xb7,0xf1,0x6f
								,0x07,0x35,0x3e,0x53,0xa1,0x76,0xd6,0x84,0xa9,0xfe,0x0c,0x6b,0xb7,0x98,0xe8,0x57};
	unsigned char pri_key[32]={0x12,0x8b,0x2f,0xa8,0xbd,0x43,0x3c,0x6c,0x06,0x8c,0x8d,0x80,0x3d,0xff,0x79,0x79
								,0x2a,0x51,0x9a,0x55,0x17,0x1b,0x1b,0x65,0x0c,0x23,0x66,0x1d,0x15,0x89,0x72,0x63};
	unsigned char userid[18]={0x41,0x4c,0x49,0x43,0x45,0x31,0x32,0x33,0x40,0x59,0x41,0x48,0x4f,0x4f,0x2e,0x43,0x4f,0x4d};
	int userid_len=18,msg_len=14,sign_r_len,sign_s_len,i;
	unsigned char msg[14]={0x6d,0x65,0x73,0x73,0x61,0x67,0x65,0x20,0x64,0x69,0x67,0x65,0x73,0x74};
	unsigned char e[32],sign_r[32],sign_s[32];

	printfS("\n******sm2_sign****\n");
	if(0==sm3_e(userid,userid_len, pub_key,32, pub_key+32, 32,msg, msg_len, e))
	{
		printfS("\n******get e success****\n");
		printfS("******e ***\n");
		for(i=0;i<32;i++)
		{
			printfS("%02X", e[i]);
		}
	}
	else
	{
		printfS("\n******get e error****\n");
	}
	sm2_sign(e,32,pri_key,32,sign_r,&sign_r_len,sign_s,&sign_s_len);
	printfS("\n******sign_r_len=%d***\n",sign_r_len);
	printfS("\n******sign_s_len=%d***\n",sign_s_len);
	printfS("\n******sign_r***\n");
	for(i=0;i<sign_r_len;i++)
	{
		printfS("%02X", sign_r[i]);
	}
	printfS("\n******sign_s***\n");
	for(i=0;i<sign_s_len;i++)
	{
		printfS("%02X", sign_s[i]);
	}
	printfS("\n******sm2_verify****\n");
	if(0==sm2_verify(e,32,sign_r,sign_r_len,sign_s,sign_s_len, pub_key,32, pub_key+32,32))
	{
		printfS("\n******verify success****\n");
	}
	else
	{
		printfS("\n******verify error****\n");
		return;
	}

	printfS("\n******seed test****\n");
	int loop_times=10000;
	clock_t start,end;
	double speed;

	start=clock();
	for(i=0;i<loop_times;i++)
	{
		sm2_sign(e,32,pri_key,32,sign_r,&sign_r_len,sign_s,&sign_s_len);
	}
	end=clock();
	printfS("sm2_sign start:%lf\n", (double)start);
	printfS("sm2_sign end:%lf\n", (double)end);
	speed = (double)(end-start)/CLOCKS_PER_SEC;
	printfS("sm2_sign speed:%lf/s\n", (double)loop_times/speed);

	start=clock();
	for(i=0;i<loop_times;i++)
	{
		if(0!=sm2_verify(e,32,sign_r,sign_r_len,sign_s,sign_s_len, pub_key,32, pub_key+32,32))
		{
			printfS("\n******verify error****\n");
			return;
		}
	}
	end=clock();
	printfS("sm2_verify start:%lf\n", (double)start);
	printfS("sm2_verify end:%lf\n", (double)end);
	speed = (double)(end-start)/CLOCKS_PER_SEC;
	printfS("sm2_verify speed:%lf/s\n", (double)loop_times/speed);
}
void sm2_get_keypair_test()
{
	unsigned char pub_key[64]={0x0a,0xe4,0xc7,0x79,0x8a,0xa0,0xf1,0x19,0x47,0x1b,0xee,0x11,0x82,0x5b,0xe4,0x62
								,0x02,0xbb,0x79,0xe2,0xa5,0x84,0x44,0x95,0xe9,0x7c,0x04,0xff,0x4d,0xf2,0x54,0x8a
								,0x7c,0x02,0x40,0xf8,0x8f,0x1c,0xd4,0xe1,0x63,0x52,0xa7,0x3c,0x17,0xb7,0xf1,0x6f
								,0x07,0x35,0x3e,0x53,0xa1,0x76,0xd6,0x84,0xa9,0xfe,0x0c,0x6b,0xb7,0x98,0xe8,0x57};
	unsigned char pri_key[32]={0x12,0x8b,0x2f,0xa8,0xbd,0x43,0x3c,0x6c,0x06,0x8c,0x8d,0x80,0x3d,0xff,0x79,0x79
								,0x2a,0x51,0x9a,0x55,0x17,0x1b,0x1b,0x65,0x0c,0x23,0x66,0x1d,0x15,0x89,0x72,0x63};
	int wxlen,wylen,privkeylen,i=0;

	printfS("\n******sm2_keygen****\n");
	sm2_keygen(pub_key,&wxlen, pub_key+32,&wylen,(unsigned char *)pri_key ,(int *)&privkeylen);
	printfS("\nwxlen=%d,wylen=%d,privkeylen=%d\n",wxlen,wylen,privkeylen);
	printfS("\n******pub****\n");
	for(i=0;i<64;i++)
	{
		printfS("%02X", pub_key[i]);
	}
	printfS("\n");
	printfS("\n******pri****\n");
	for(i=0;i<32;i++)
	{
		printfS("%02X", pri_key[i]);
	}
	printfS("\n");


	printfS("\n******seed test****\n");
	int loop_times=10000;
	clock_t start,end;
	double speed;

	start=clock();
	for(i=0;i<loop_times;i++)
	{
		sm2_keygen(pub_key,&wxlen, pub_key+32,&wylen,(unsigned char *)pri_key ,(int *)&privkeylen);
	}
	end=clock();
	printfS("sm2_get_keypair start:%lf\n", (double)start);
	printfS("sm2_get_keypair end:%lf\n", (double)end);
	speed = (double)(end-start)/CLOCKS_PER_SEC;
	printfS("sm2_get_keypair speed:%lf/s\n", (double)loop_times/speed);
}
void sm2_enc_dec_test()
{
	printfS("\n******sm2_enc****\n");
	unsigned char enc_msg[19]={0x65,0x6e,0x63,0x72,0x79,0x70,0x74,0x69,0x6f,0x6e,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x72,0x64};
	unsigned char enc_pub_key[64]={0x43,0x5B,0x39,0xCC,0xA8,0xF3,0xB5,0x08,0xC1,0x48,0x8A,0xFC,0x67,0xBE,0x49,0x1A,
									0x0F,0x7B,0xA0,0x7E,0x58,0x1A,0x0E,0x48,0x49,0xA5,0xCF,0x70,0x62,0x8A,0x7E,0x0A,
									0x75,0xDD,0xBA,0x78,0xF1,0x5F,0xEE,0xCB,0x4C,0x78,0x95,0xE2,0xC1,0xCD,0xF5,0xFE,
									0x01,0xDE,0xBB,0x2C,0xDB,0xAD,0xF4,0x53,0x99,0xCC,0xF7,0x7B,0xBA,0x07,0x6A,0x42};
	unsigned char enc_pri_key[32]={0x16,0x49,0xab,0x77,0xa0,0x06,0x37,0xbd,0x5e,0x2e,0xfe,0x28,0x3f,0xbf,0x35,0x35,
									 0x34,0xaa,0x7f,0x7c,0xb8,0x94,0x63,0xf2,0x08,0xdd,0xbc,0x29,0x20,0xbb,0x0d,0xa0};
	unsigned char enc_outmsg[400],enc_msg_test[200];
	int enc_out_msg_len,enc_msg_len,i;

	printfS("\n******pub_key***\n");
	for(i=0;i<64;i++)
	{
		printfS("%02X", enc_pub_key[i]);
	}
	printfS("\n******pri_key***\n");
	for(i=0;i<32;i++)
	{
		printfS("%02X", enc_pri_key[i]);
	}
	if(0==sm2_encrypt(enc_msg,19, enc_pub_key,32, enc_pub_key+32,32, enc_outmsg, &enc_out_msg_len))
	{
		printfS("\n******enc success****\n");
		printfS("\nenc_out_msg_len=%d\n",enc_out_msg_len);
		printfS("\n******out msg***\n");
		for(i=0;i<enc_out_msg_len;i++)
		{
			printfS("%02X", enc_outmsg[i]);
		}
		printfS("\n");
	}
	else
	{
		printfS("\n******enc error****\n");
		return;
	}
	printfS("\n******sm2_dec****\n");
	if(0==sm2_decrypt(enc_outmsg,enc_out_msg_len, enc_pri_key, 32, enc_msg_test, &enc_msg_len))
	{
		printfS("\n******dec success****\n");
		printfS("\nenc_msg_len=%d\n",enc_msg_len);
		printfS("\n******out msg***\n");
		for(i=0;i<enc_msg_len;i++)
		{
			printfS("%02X", enc_msg_test[i]);
		}
		printfS("\n");
	}
	else
	{
		printfS("\n******dec error****\n");
		return;
	}

	printfS("\n******seed test****\n");
	int loop_times=10000;
	clock_t start,end;
	double speed;

	start=clock();
	for(i=0;i<loop_times;i++)
	{
		if(0!=sm2_encrypt(enc_msg,19, enc_pub_key,32, enc_pub_key+32,32, enc_outmsg, &enc_out_msg_len))
		{
			printfS("\n******enc error****\n");
			return;
		}
	}
	end=clock();
	printfS("sm2_encrypt start:%lf\n", (double)start);
	printfS("sm2_encrypt end:%lf\n", (double)end);
	speed = (double)(end-start)/CLOCKS_PER_SEC;
	printfS("sm2_encrypt speed:%lf/s\n", (double)loop_times/speed);

	start=clock();
	for(i=0;i<loop_times;i++)
	{
		if(0!=sm2_decrypt(enc_outmsg,enc_out_msg_len, enc_pri_key, 32, enc_msg_test, &enc_msg_len))
		{
			printfS("\n******dec error****\n");
			return;
		}
	}
	end=clock();
	printfS("sm2_decrypt start:%lf\n", (double)start);
	printfS("sm2_decrypt end:%lf\n", (double)end);
	speed = (double)(end-start)/CLOCKS_PER_SEC;
	printfS("sm2_decrypt speed:%lf/s\n", (double)loop_times/speed);
}

void sm2_test()
{

	printfS("\n******sm2_get_keypair_test****\n");
	sm2_get_keypair_test();

	printfS("\n******sm2_sign_verify_test****\n");
	sm2_sign_verify_test();

	printfS("\n******sm2_enc_dec_test****\n");
	sm2_enc_dec_test();

	printfS("\n******change key****\n");
	sm2_change_key_test();
}


