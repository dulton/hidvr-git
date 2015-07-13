///*
// * lpcm.c
// *
// *  Created on: 2012-3-7
// *      Author: root
// */
//
//#include "g711.h"
//#include "lpcm.h"
//
//void decompress_g711_to_lpcm(void* _src, void* _dest)//_src 320bytes, _dest 640bytes
//{
//	unsigned char* src = _src;
//	short* dest = _dest;
//	int i;
//	for(i = 0; i < 320; i++)
//	{
//		dest[i] = alaw2linear(src[i]);
//	}
//}
//
//void compress_lpcm_16bit_to_8bit(short* _src, int _src_len, unsigned char* _dest)
//{
//	short* pcm16 = _src;
//	unsigned char* pcm8 = _dest;
//	int i;
//	for(i = 0; i < _src_len/2; i++)
//	{
//		*pcm8++ = (unsigned char)((*pcm16++ >> 8) + 128);
//	}
//}
//
//void resample_lpcm_16bit_8000_to_11000(short* _src, short* _dest)//_src 640bytes, _dest 880bytes
//{
//	short* src = _src;
//	short* dest = _dest;
//	int i;
//	for(i = 0;i < 320/8;i++)
//	{
//		dest[0 + i*11] = src[0 + i*8];
//		dest[1 + i*11] = src[1 + i*8];
//		dest[2 + i*11] = src[2 + i*8];
//		dest[3 + i*11] = src[2 + i*8]/2 + src[3 + i*8]/2;
//
//		dest[4 + i*11] = src[3 + i*8];
//		dest[5 + i*11] = src[4 + i*8];
//		dest[6 + i*11] = src[5 + i*8];
//		dest[7 + i*11] = src[5 + i*8]/2 + src[6 + i*8]/2;
//
//		dest[8 + i*11] = src[6 + i*8];
//		dest[9 + i*11] = src[7 + i*8];
//		dest[10 + i*11] = src[7 + i*8];
//	}
//}
//
//void resample_lpcm_16bit_8000_to_5500(short* _src, short* _dest)//_src 640bytes, _dest 440bytes
//{
//	short* src = _src;
//	short* dest = _dest;
//	int i;
//	for(i = 0;i < 320/16;i++)
//	{
//		dest[0 + i*11] = src[0 + i*16]/2 + src[1 + i*16]/2;
//		dest[1 + i*11] = src[2 + i*16]/2 + src[3 + i*16]/2;
//		dest[2 + i*11] = src[3 + i*16]/2 + src[4 + i*16]/2;
//
//		dest[3 + i*11] = src[4 + i*16]/2 + src[5 + i*16]/2;
//		dest[4 + i*11] = src[5 + i*16]/2 + src[6 + i*16]/2;
//		dest[5 + i*11] = src[7 + i*16]/2 + src[8 + i*16]/2;
//
//		dest[6 + i*11] = src[8 + i*16]/2 + src[9 + i*16]/2;
//		dest[7 + i*11] = src[9 + i*16]/2 + src[10 + i*16]/2;
//		dest[8 + i*11] = src[10 + i*16]/2 + src[11 + i*16]/2;
//
//		dest[9 + i*11] = src[12 + i*16]/2 + src[13 + i*16]/2;
//		dest[10 + i*11] = src[14 + i*16]/2 + src[15 + i*16]/2;
//	}
//}
//
//void resample_lpcm_8bit_8000_to_11000(unsigned char* _src, unsigned char* _dest)//_src 320bytes, _dest 440bytes
//{
//	unsigned char* src = _src;
//	unsigned char* dest = _dest;
//	int i;
//	for(i = 0;i < 320/8;i++)
//	{
//		dest[0 + i*11] = src[0 + i*8];
//		dest[1 + i*11] = src[1 + i*8];
//		dest[2 + i*11] = src[2 + i*8];
//		dest[3 + i*11] = src[2 + i*8]/2 + src[3 + i*8]/2;
//
//		dest[4 + i*11] = src[3 + i*8];
//		dest[5 + i*11] = src[4 + i*8];
//		dest[6 + i*11] = src[5 + i*8];
//		dest[7 + i*11] = src[5 + i*8]/2 + src[6 + i*8]/2;
//
//		dest[8 + i*11] = src[6 + i*8];
//		dest[9 + i*11] = src[7 + i*8];
//		dest[10 + i*11] = src[7 + i*8];
//	}
//}
//
//void resample_lpcm_8bit_8000_to_5500(unsigned char* _src, unsigned char* _dest)//_src 320bytes, _dest 220bytes
//{
//	unsigned char* src = _src;
//	unsigned char* dest = _dest;
//	int i;
//	for(i = 0;i < 320/16;i++)
//	{
//		dest[0 + i*11] = src[0 + i*16]/2 + src[1 + i*16]/2;
//		dest[1 + i*11] = src[2 + i*16]/2 + src[3 + i*16]/2;
//		dest[2 + i*11] = src[3 + i*16]/2 + src[4 + i*16]/2;
//
//		dest[3 + i*11] = src[4 + i*16]/2 + src[5 + i*16]/2;
//		dest[4 + i*11] = src[5 + i*16]/2 + src[6 + i*16]/2;
//		dest[5 + i*11] = src[7 + i*16]/2 + src[8 + i*16]/2;
//
//		dest[6 + i*11] = src[8 + i*16]/2 + src[9 + i*16]/2;
//		dest[7 + i*11] = src[9 + i*16]/2 + src[10 + i*16]/2;
//		dest[8 + i*11] = src[10 + i*16]/2 + src[11 + i*16]/2;
//
//		dest[9 + i*11] = src[12 + i*16]/2 + src[13 + i*16]/2;
//		dest[10 + i*11] = src[14 + i*16]/2 + src[15 + i*16]/2;
//	}
//}
