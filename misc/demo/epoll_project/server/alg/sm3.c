

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sm3.h"
#include "../include/common.h"

//循环左移
static unsigned int _irol_(unsigned int a, unsigned int b)
{
	unsigned int c = 0;
	c = a >> (32 - b);
	a = a << b;
	a = a|c;

	return a;
}

static void int2char(unsigned int a, unsigned char* b)
{
	b[0] = (a >> 24) & 0xFF;
	b[1] = (a >> 16) & 0xFF ;
	b[2] = (a >> 8) & 0xFF;
	b[3] = (a & 0xFF);
}

static unsigned int char2int(const unsigned char* a)
{
	return (a[0]<<24)+(a[1]<<16)+(a[2]<<8)+a[3];
}

//初始值
static const unsigned int IV[] =
{
	0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600,
	0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E
};

//常量
static unsigned int Tj(unsigned int j)
{
	if(j < 16)
	{
		return 0x79CC4519;
	}
	else if(j < 64)
	{
		return 0x7A879D8A;
	}
	else
	{
		return 0;
	}
}

//置换函数
static unsigned int P0(unsigned int a)
{
	return a ^ (_irol_(a, 9)) ^ (_irol_(a, 17));
}

static unsigned int P1(unsigned int a)
{
	return a ^ (_irol_(a, 15)) ^ (_irol_(a, 23));
}

//布尔函数
static unsigned int FFj(unsigned int x, unsigned int y, unsigned int z, unsigned int j)
{
	if(j < 16)
	{
		return x ^ y ^ z;
	}
	else if( j < 64)
	{
		return (x & y)|(x & z)|(y & z);
	}
	else
	{
		return 0;
	}
}

static unsigned int GGj(unsigned int x, unsigned int y, unsigned int z, unsigned int j)
{
	if(j <= 15)
	{
		return x ^ y ^ z;
	}
	if(j >= 16 && j <= 63)
	{
		return (x & y)|(~x & z);
	}

	return 0;
}

//数据填充
static int SM3_Ready(const unsigned char* inBuf, int inLen, unsigned char* outBuf, int* outLen)
{
	unsigned int l = 0, k = 0;
	int n = 0;
	l = inLen * 8;
	k = (447 - l) % 512;
	n = (l+k+65)/512;

	if(*outLen < n *64)
	{
		*outLen = n * 64;
		return 1;
	}
	memmove(outBuf, inBuf, inLen);
	outBuf[inLen] = 0x80;
	*outLen = n * 64;

	memset(outBuf+inLen+1, 0, n*64-inLen-5);
	int2char(l, outBuf+n*64-4);

	return 0;
}

static int SM3_Extend(const unsigned char inBuf[64], unsigned int W[68], unsigned int W1[64])
{
	unsigned int i = 0;

	for(i=0; i<16; i++)
	{
		W[i] = char2int(inBuf+4*i);
	}

	for(i=16; i<68; i++)
	{
		W[i] = P1(W[i-16] ^ W[i-9] ^ _irol_(W[i-3], 15)) ^ _irol_(W[i-13], 7) ^ W[i-6];
	}
	for(i=0; i<64; i++)
	{
		W1[i] = W[i] ^ W[i+4];
	}

	return 0;
}

static int CF(const unsigned int Vi[8], unsigned int W[68], unsigned int W1[64], unsigned int Vn[8])
{
	unsigned int SS1, SS2, TT1, TT2, i=0;
	unsigned int A[8] = {0};

	memmove(A, Vi, 32);
	memmove(Vn, A, 32);

	for(i=0; i<64; i++)
	{
		SS1 = _irol_(_irol_(A[0], 12) + A[4] + _irol_(Tj(i), i), 7);
		SS2 = SS1 ^ _irol_(A[0], 12);
		TT1 = FFj(A[0], A[1], A[2], i) + A[3] + SS2 + W1[i];
		TT2 = GGj(A[4], A[5], A[6], i) + A[7] + SS1 + W[i];
		A[3] = A[2];
		A[2] = _irol_(A[1], 9);
		A[1] = A[0];
		A[0] = TT1;
		A[7] = A[6];
		A[6] = _irol_(A[5], 19);
		A[5] = A[4];
		A[4] = P0(TT2);
	}
	for(i=0; i<8; i++)
	{
		Vn[i] = Vn[i] ^ A[i];
	}

	return 0;
}

/******************************************************************************
*功能：
*	计算SM3
*参数：
*	pInBuf		[in] 需要计算摘要的源数据
*	inLen		[in] 数据长度
*	pOutBuf  	[out]消息摘要
*返回值：
*
*******************************************************************************/
void SM3(const unsigned char *pInBuf, int inLen, unsigned char *pOutBuf)
{
	unsigned int W[68], W1[64], mVn[8], Vn[8];
	unsigned char *midBuf = NULL;

	int midLen = ((inLen+8)/64+1)*64, i = 0;
	midBuf = (unsigned char*)malloc(midLen);
	memmove(Vn, IV, 32);
	SM3_Ready(pInBuf, inLen, midBuf, &midLen);
	for(i=0; i<midLen; i+=64)
	{
		memset(W, 0, 68*4);
		memset(W1, 0, 64*4);
		memmove(mVn, Vn, 32);
		SM3_Extend(midBuf+i, W, W1);
		CF(mVn, W, W1, Vn);
	}

	for(i=0; i<8; i++)
	{
		int2char(Vn[i], pOutBuf+4*i);
	}
	if(midBuf)
	{
		free(midBuf);
	}

	return;
}

/******************************************************************************
*功能：
*	SM3初始化
*参数：
*	ctx		[in] 缓存结构(void指针,与平台相关)
*
*返回值：
*	无
*******************************************************************************/
void SM3_Init(SM3_CTX **ctx)
{
	memmove((unsigned char *)(*ctx)->state, IV, 32);
	(*ctx)->curlen = 0;
	(*ctx)->len = 0;
	memset((*ctx)->buf, 0, 64);
}

/******************************************************************************
*功能：
*	计算SM3
*参数：
*	ctx		[in] 缓存结构(void指针,与平台相关)
*	pInBuf	[in] 输入数据
*	inLen	[in] 数据长度
*返回值：
*	无
*******************************************************************************/
void SM3_Update(SM3_CTX **ctx, const unsigned char *pInBuf, unsigned int inLen)
{
	int midLen = 0, ll = 0, i = 0;
	unsigned char *midBuf = NULL;
	unsigned int W[68], W1[64], mVn[8], Vn[8];

	if((*ctx)->curlen + inLen < 64)
	{
		memmove((*ctx)->buf+(*ctx)->curlen, pInBuf, inLen);
		(*ctx)->curlen += inLen;
		return;
	}

	midLen = inLen+64;
	ll = ((*ctx)->curlen + inLen) % 64;
	midBuf = (unsigned char*)malloc(inLen + 64);
	memmove(midBuf, (*ctx)->buf, (*ctx)->curlen);
	memmove(midBuf+(*ctx)->curlen, pInBuf, inLen-ll);
	midLen = (*ctx)->curlen + inLen - ll;
	(*ctx)->len += midLen;
	memmove((*ctx)->buf, pInBuf + inLen -ll, ll);
	(*ctx)->curlen = ll;

	memmove(Vn, (*ctx)->state, 32);
	for(i=0; i<midLen; i+=64)
	{
		memset(W, 0, 68*4);
		memset(W1, 0, 64*4);
		memmove(mVn, Vn, 32);
		SM3_Extend(midBuf+i, W, W1);
		CF(mVn, W, W1, Vn);
	}
	memmove((*ctx)->state, Vn, 32);
	if(midBuf)
	{
		free(midBuf);
	}
	return;
}

/******************************************************************************
*功能：
*	输出消息摘要
*参数：
*	ctx		[in] 缓存结构(void指针,与平台相关)
*	pOutBuf	[out]消息摘要值
*返回值：
*	无
*******************************************************************************/
void SM3_Final(SM3_CTX **ctx, unsigned char *pOutBuf)
{
	unsigned int l = 0, k = 0;
	int n = 0, i=0;
	unsigned int W[68], W1[64], mVn[8], Vn[8];
	unsigned char midBuf[128] = {0};

	l = ((*ctx)->len + (*ctx)->curlen) * 8;
	k = (447 - l) % 512;
	n = ((*ctx)->curlen*8+k+65)/512;

	memmove(midBuf, (*ctx)->buf, (*ctx)->curlen);
	midBuf[(*ctx)->curlen] = 0x80;
	int2char(l, midBuf+n*64-4);

	memmove(Vn, (*ctx)->state, 32);
	for(i=0; i<n; i++)
	{
		memset(W, 0, 68*4);
		memset(W1, 0, 64*4);
		memmove(mVn, Vn, 32);
		SM3_Extend(midBuf+i*64, W, W1);
		CF(mVn, W, W1, Vn);
	}

	for(i=0; i<8; i++)
	{
		int2char(Vn[i], pOutBuf+4*i);
	}
	return;
}

