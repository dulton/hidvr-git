
#ifndef __AVENC_MAP_H__
#define __AVENC_MAP_H__

static const int FRAMERATE_MAP[] =
{
	[FR_1FPS] = 3,
	[FR_2FPS] = 3,
	[FR_3FPS] = 3,
	[FR_4FPS] = 4,
	[FR_5FPS] = 5,
	[FR_6FPS] = 6,
	[FR_7FPS] = 7,
	[FR_8FPS] = 8,
	[FR_9FPS] = 9,
	[FR_10FPS] = 10,
	[FR_11FPS] = 11,
	[FR_12FPS] = 12,
	[FR_13FPS] = 13,
	[FR_14FPS] = 14,
	[FR_15FPS] = 15,
	[FR_16FPS] = 16,
	[FR_17FPS] = 17,
	[FR_18FPS] = 18,
	[FR_19FPS] = 19,
	[FR_20FPS] = 20,
	[FR_21FPS] = 21,
	[FR_22FPS] = 22,
	[FR_23FPS] = 23,
	[FR_24FPS] = 24,
	[FR_25FPS] = 25,
	[FR_30FPS] = 30,
};

static const int VBR_RANK_MAP[] =
{
	[RANK_HIGHEST] = 0,
	[RANK_HIGH] = 1,
	[RANK_MEDIUM] = 2,
	[RANK_LOW] = 3,
	[RANK_LOWEST] = 4,
};

static const int BITRATE_MAP[] =
{
	[ENC_BR_64KPS] = 64L,
	[ENC_BR_128KPS] = 128L,
	[ENC_BR_256KPS] = 256L,
	[ENC_BR_384KPS] = 384L,
	[ENC_BR_512KPS] = 512L,
	[ENC_BR_768KPS] = 768L,
	[ENC_BR_1MPS] = 1024L,
	[ENC_BR_1P5MPS] = 1536L,
	[ENC_BR_2MPS] = 2048L,
	[ENC_BR_3MPS] = 3072L,
	[ENC_BR_4MPS] = 4096L,
	[ENC_BR_5MPS] = 5120L,
	[ENC_BR_6MPS] = 6144L,
	[ENC_BR_8MPS] = 8192L,
	[ENC_BR_10MPS] = 10240,
};

#endif //__AVENC_MAP_H__

