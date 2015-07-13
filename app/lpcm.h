/*
 * lpcm.h
 *
 *  Created on: 2012-3-7
 *      Author: root
 */

#ifndef LPCM_H_
#define LPCM_H_

typedef enum
{
	LPCM_5500_8BIT_MONO = 0x30,
	LPCM_5500_16BIT_MONO = 0x32,
	LPCM_11000_8BIT_MONO = 0x34,
	LPCM_11000_16BIT_MONO = 0x36,
}FLV_AUDIO_CONFIG;

extern void decompress_g711_to_lpcm(void* _src, void* _dest);
extern void compress_lpcm_16bit_to_8bit(short* _src, int _src_len, unsigned char* _dest);
extern void resample_lpcm_16bit_8000_to_11000(short* _src, short* _dest);
extern void resample_lpcm_16bit_8000_to_5500(short* _src, short* _dest);
extern void resample_lpcm_8bit_8000_to_11000(unsigned char* _src, unsigned char* _dest);
extern void resample_lpcm_8bit_8000_to_5500(unsigned char* _src, unsigned char* _dest);

#endif /* LPCM_H_ */
