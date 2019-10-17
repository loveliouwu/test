/*
 * sm4.h
 *
 *  Created on: Aug 9, 2019
 *      Author: jjn
 */

#ifndef INCLUDE_SM4_H_
#define INCLUDE_SM4_H_

typedef enum sm4_mode_
{
	SM4_ENC,
	SM4_DEC,

}sm4_mode;

/*****************************************************************************************************************
 * private F function: sm4 ecb mode enc and dec
 * args:    [in] key: key
 * args:    [in] dec_en: enc or dec
 * args:    [in] block_num: sm4 block num
 * args:    [in] input: inut data
 * args:    [out]output: output data;
 * return: none
 ******************************************************************************************************************/
void sm4_ecb(unsigned char key[16],
		sm4_mode dec_en,
		unsigned int block_num,
         unsigned char *input,
         unsigned char *output);

/*****************************************************************************************************************
 * private F function: sm4 cbc mode enc and dec
 * args:    [in] key: key
 * args:    [in] dec_en: enc or dec
 * args:    [in] block_num: sm4 block num
 * args:    [in] iv: iv
 * args:    [in] input: inut data
 * args:    [out]output: output data;
 * return: none
 ******************************************************************************************************************/
void sm4_cbc( unsigned char key[16],sm4_mode dec_en,unsigned int block_num,
                    unsigned char iv[16],
                    unsigned char *input,
                    unsigned char *output );
#endif /* INCLUDE_SM4_H_ */
