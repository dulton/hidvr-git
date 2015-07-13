#include "conf.h"  //Import MAX_CAM_CH Definition
#include "hddvr_code.h"

#include "bsp/decoder_drv.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

static pthread_mutex_t gI2C_Locker;
#define I2C_InLock() pthread_mutex_lock(&gI2C_Locker)
#define I2C_UnLock() pthread_mutex_unlock(&gI2C_Locker)

int const HDTVI_CHN_TBL[MAX_CAM_CH][2] = { //The Real Video&Audio Channel Order
#if (MAX_CAM_CH == 4)
	{0, 2}, {0, 3}, {0, 0}, {0, 1},
#endif
#if (MAX_CAM_CH == 8)
	{0, 2}, {0, 3}, {0, 0}, {0, 1},
	{1, 2}, {1, 3}, {1, 0}, {1, 1},
#endif
};

#define HDTVI_CHIP_SIZE (4)
#define HDTVI_ADDR0 (0x88)
#define HDTVI_ADDR1 (0x8A)
unsigned char HDTVI_ADDR[2] = { HDTVI_ADDR0, HDTVI_ADDR1 };

int HDTVI_ChipGetMap(int Chnl, int *RealChip, int *RealChnl, int *RealAddr)
{
	int tmpRealChip;
	int tmpRealChnl;
	int tmpRealAddr;

	if(!CHECK_SIZE(Chnl, HDTVI_CHN_TBL)) {
		return -1;
	}

	tmpRealChip = HDTVI_CHN_TBL[Chnl][0];
	tmpRealChnl = HDTVI_CHN_TBL[Chnl][1];
	tmpRealAddr = HDTVI_ADDR[tmpRealChip];

	if(RealChip) {
		*RealChip = tmpRealChip;
	}
	if(RealChnl) {
		*RealChnl = tmpRealChnl%4;
	}
	if(RealAddr) {
		*RealAddr = tmpRealAddr;
	}

	return 0;
}

const unsigned char TP2802_720P60[] = {
	0x02, 0xC2,

	0x15, 0x12,
	0x16, 0x15,
	0x17, 0x00,
	0x18, 0x19,
	0x19, 0xD0,
	0x1A, 0x25,
	0x1C, 0x06,
	0x1D, 0x72,

	0xFF, 0xFF,
};

const unsigned char TP2802_720P50[] = {
	0x02, 0xC2,

	0x15, 0x12,
	0x16, 0x15,
	0x17, 0x00,
	0x18, 0x19,
	0x19, 0xD0,
	0x1A, 0x25,
	0x1C, 0x07,
	0x1D, 0xBC,

	0xFF, 0xFF,
};

const unsigned char TP2802_720P30[]={
	0x02, 0xC2,

	0x15, 0x12,
	0x16, 0x15,
	0x17, 0x00,
	0x18, 0x19,
	0x19, 0xD0,
	0x1A, 0x25,
	0x1C, 0x0C,
	0x1D, 0xE4,

	0xFF, 0xFF,
};

const unsigned char TP2802_720P25[]={
	0x02, 0xC2,

	0x15, 0x12,
	0x16, 0x15,
	0x17, 0x00,
	0x18, 0x19,
	0x19, 0xD0,
	0x1A, 0x25,
	0x1C, 0x0F,
	0x1D, 0x78,

	0xFF, 0xFF,
};

const unsigned char TP2802_1080P30[]={
	0x02, 0xC0,

	0x15, 0x02,
	0x16, 0xD2,
	0x17, 0x80,
	0x18, 0x29,
	0x19, 0x38,
	0x1A, 0x47,
	0x1C, 0x08,
	0x1D, 0x98,

	0xFF, 0xFF,
};

const unsigned char TP2802_1080P25[] = {
	0x02, 0xC0,

	0x15, 0x02,
	0x16, 0xD2,
	0x17, 0x80,
	0x18, 0x29,
	0x19, 0x38,
	0x1A, 0x47,
	0x1C, 0x0A,
	0x1D, 0x50,

	0xFF, 0xFF,
};

const unsigned char TP2802_Comm[] = {
	0x40, 0x04,

	0x07, 0x0F,
	0x08, 0xE0,

	0x2C, 0x0A,
	0x2E, 0x60,
	0x2F, 0x08,
	0x30, 0x48,
	0x31, 0xBB,
	0x32, 0x2E,
	0x33, 0x90,
	0x38, 0xC0,
	0x39, 0xCA,
	0x3A, 0x0A,
	0x3D, 0x20,
	0x3E, 0x02,
	0x40, 0x04,
	0x42, 0x00,
	0x43, 0x12,
	0x44, 0x07,
	0x45, 0x49,
	0x4B, 0x10,
	0x4C, 0x32,
	0x4D, 0x0F,
	0x4E, 0xFF,
	0x4F, 0x0F,
	0x50, 0x00,
	0x51, 0x0A,
	0x52, 0x0B,
	0x53, 0x1F,
	0x54, 0xFA,
	0x55, 0x27,
	0x7E, 0x2F,
	0x7F, 0x00,
	0x80, 0x07,
	0x81, 0x08,
	0x82, 0x04,
	0x83, 0x00,
	0x84, 0x00,
	0x85, 0x60,
	0x86, 0x10,
	0x87, 0x06,
	0x88, 0xBE,
	0x89, 0x39,
	0x8A, 0xA7,
	0xB3, 0xFA,
	0xB4, 0x1C,
	0xB5, 0x0F,
	0xB6, 0xFF,
	0xFC, 0x00,
	0xFD, 0x00,

	0xFF, 0xFF,
};

void I2CDeviceInitialize(unsigned char addr, const unsigned char *RegSet)
{
	unsigned char index, val;

	while (( RegSet[0] != 0xFF ) || ( RegSet[1]!= 0xFF )) {			// 0xff, 0xff is end of data
		index = *RegSet;
		val = *(RegSet+1);

		HDDVR_i2c_WriteByte(addr, index, val);

		RegSet+=2;
	}
}

int HDTVI_Probe(void)
{
	int ii;
	int Result = -1;

	for(ii = 0; ii < ARRAY_SIZE(HDTVI_ADDR); ii ++) {
		I2C_InLock();
		if(0x2802 == ((HDDVR_i2c_ReadByte(HDTVI_ADDR[ii], 0xFE) << 8)
				| (HDDVR_i2c_ReadByte(HDTVI_ADDR[ii], 0xFF) << 0))) {
			Result = 0;
		}
		I2C_UnLock();
	}

	return Result;
}

int HDTVI_Init(int Fmt)
{
	pthread_mutex_init(&gI2C_Locker, NULL);

	int ii;
	int tmpAddr;
	for(ii = 0; ii < ARRAY_SIZE(HDTVI_ADDR); ii ++) {
		tmpAddr = HDTVI_ADDR[ii];
		I2C_InLock();
		HDDVR_i2c_WriteByte(tmpAddr, 0x40, 0x04);

		I2CDeviceInitialize(tmpAddr, TP2802_720P25);

		I2CDeviceInitialize(tmpAddr, TP2802_Comm);

		HDDVR_i2c_WriteByte(tmpAddr, 0x44, 0x47);  //Force TVI Chip PLL Power Down & Up
		I2C_UnLock();

		usleep(30000);

		I2C_InLock();
		HDDVR_i2c_WriteByte(tmpAddr, 0x40, 0x04);
		HDDVR_i2c_WriteByte(tmpAddr, 0x44, 0x07);
		I2C_UnLock();
	}

	return 0;
}

int HDTVI_Exit(void)
{
	pthread_mutex_destroy(&gI2C_Locker);

	return 0;
}

int HDTVI_VideoVLossCheck(int Chnl)
{
	int tmpRealChip;
	int tmpRealChnl;
	int tmpRealAddr;

	unsigned char tmpRead;

	if(0 != HDTVI_ChipGetMap(Chnl, &tmpRealChip, &tmpRealChnl, &tmpRealAddr)) {
		return HDVLOSS_ERROR;
	}

	I2C_InLock();
	HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);

	tmpRead  = HDDVR_i2c_ReadByte(tmpRealAddr, 0x01);

	I2C_UnLock();

	return (tmpRead & 0x80)
			? HDVLOSS_NOVIDEO
			: HDVLOSS_INVIDEO;
}

int HDTVI_VideoVLockCheck(int Chnl)
{
	int tmpRealChip;
	int tmpRealChnl;
	int tmpRealAddr;

	unsigned char tmpRead;

	if(0 != HDTVI_ChipGetMap(Chnl, &tmpRealChip, &tmpRealChnl, &tmpRealAddr)) {
		return HDVLOSS_ERROR;
	}

	I2C_InLock();
	HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);

	tmpRead  = HDDVR_i2c_ReadByte(tmpRealAddr, 0x01);

	I2C_UnLock();

	return ((tmpRead & 0x68) == 0x68) //Check VLOCK & HLOCK bits
			? HDVLOSS_INVIDEO
			: HDVLOSS_NOVIDEO;
}

static int SleepWithVideoCheck(int Chnl, int Unit, int Numb)
{
	while(Numb --) {
		if(HDTVI_VideoVLossCheck(Chnl)) {
			return -1;
		}
		usleep(Unit);
	}

	return 0;
}

#define MAX_GAINCOUNT (6)
#define MAX_CHCKCOUNT (20)
int ViDiffs[MAX_CAM_CH][MAX_GAINCOUNT];
int ViGains[MAX_CAM_CH];
int ViFlags[MAX_CAM_CH];
int ViCount[MAX_CAM_CH];
int VsFlags[MAX_CAM_CH];

int HDTVI_VideoNoVideoFix(int Chnl)
{
	int tmpRealChip;
	int tmpRealChnl;
	int tmpRealAddr;

	if(0 != HDTVI_ChipGetMap(Chnl, &tmpRealChip, &tmpRealChnl, &tmpRealAddr)) {
		return -1;
	}

	I2C_InLock();
	HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);

	HDDVR_i2c_WriteByte(tmpRealAddr, 0x3A, 0x0A);
	HDDVR_i2c_WriteByte(tmpRealAddr, 0x07, 0x0F);

	I2C_UnLock();

	ViGains[Chnl] = 0;
	ViFlags[Chnl] = 0;
	ViCount[Chnl] = 0;
	VsFlags[Chnl] = 0;

	return 0;
}

unsigned char agcfixTBL[] = {0xE0, 0xE0, 0xE0, 0xE8, 0xE8, 0xE8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8}; //0x08
unsigned char satfixTBL[] = {0x40, 0x40, 0x40, 0x38, 0x38, 0x38, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30}; //0x12
unsigned char huefixTBL[] = {0x00, 0x00, 0x0C, 0x0C, 0x18, 0x18, 0x24, 0x24, 0x28, 0x28, 0x0C, 0x0C}; //0x13
unsigned char shpfixTBL[] = {0x07, 0x07, 0x07, 0x10, 0x10, 0x10, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}; //0x14

int HDTVI_VideoStVideoFix(int Chnl)
{
	int tmpRealChip;
	int tmpRealChnl;
	int tmpRealAddr;

	if(0 != HDTVI_ChipGetMap(Chnl, &tmpRealChip, &tmpRealChnl, &tmpRealAddr)) {
		return -1;
	}

	int Gain, Reg04;

	if(!ViFlags[Chnl]) {
		return 0;
	}

	{
		I2C_InLock();
		HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);
		Reg04 = HDDVR_i2c_ReadByte(tmpRealAddr, 0x04);
		I2C_UnLock();

		if((Reg04 > 0x0A) && (0 == VsFlags[Chnl])) {
			VsFlags[Chnl] = 1;

			I2C_InLock();
			HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);
			HDDVR_i2c_WriteByte(tmpRealAddr, 0x09, 0x64);
			I2C_UnLock();

			if(0 > SleepWithVideoCheck(Chnl, 300000, 6)) { //1.8s
				return -1;
			}

			I2C_InLock();
			HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);
			HDDVR_i2c_WriteByte(tmpRealAddr, 0x09, 0x24);
			I2C_UnLock();

			if(0 > SleepWithVideoCheck(Chnl, 300000, 2)) { //0.6s
				return -1;
			}
		}

		I2C_InLock();
		HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);
		Gain  = ((HDDVR_i2c_ReadByte(tmpRealAddr, 0x03) & 0xF0) >> 4);
		I2C_UnLock();

		if(Gain > 0x7) Gain = 0x7;

		ViDiffs[Chnl][ViCount[Chnl]%MAX_GAINCOUNT] = Gain-ViGains[Chnl];
		ViGains[Chnl] = Gain;

		ViCount[Chnl] ++;

		if(ViCount[Chnl] >= MAX_GAINCOUNT) {
			int tmpSum = 0;

			int  ii;
			for(ii = 0; ii < MAX_GAINCOUNT; ii ++) {
				tmpSum += ViDiffs[Chnl][ii];
			}

			if(tmpSum < (MAX_GAINCOUNT*2/5)) {
				Gain = Gain;
				ViCount[Chnl] = MAX_CHCKCOUNT;
			}
		}
	}

	printf("ViFlags[%d] : 0x%d\n", Chnl, ViFlags[Chnl]);
	printf("ViCount[%d] : 0x%d\n", Chnl, ViCount[Chnl]);
	printf("ViGains[%d] : 0x%02X\n", Chnl, Gain);

	if(ViCount[Chnl] >= MAX_CHCKCOUNT) {
		ViFlags[Chnl] = 0;
		ViCount[Chnl] = 0;

		printf("\n%s - Chnl:%d Gain:0x%02X\n", __FUNCTION__, Chnl, Gain);

		if(Gain > 0x7) Gain = 0x7;

		I2C_InLock();
		HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);

		if(Gain < 0x3) {
			HDDVR_i2c_WriteByte(tmpRealAddr, 0x07, 0x0B);
			HDDVR_i2c_WriteByte(tmpRealAddr, 0x3A, 0x0A);
		}
		else {
			HDDVR_i2c_WriteByte(tmpRealAddr, 0x07, ((Gain << 4) | 0x0B));
			HDDVR_i2c_WriteByte(tmpRealAddr, 0x3A, 0x02);
		}

		HDDVR_i2c_WriteByte(tmpRealAddr, 0x08, agcfixTBL[Gain]);
		HDDVR_i2c_WriteByte(tmpRealAddr, 0x12, satfixTBL[Gain]);
		HDDVR_i2c_WriteByte(tmpRealAddr, 0x13, huefixTBL[Gain]);
		HDDVR_i2c_WriteByte(tmpRealAddr, 0x14, shpfixTBL[Gain]);

		I2C_UnLock();
	}

	return 0;
}

int HDTVI_VideoInVideoFix(int Chnl)
{
	int tmpRealChip;
	int tmpRealChnl;
	int tmpRealAddr;

	if(0 != HDTVI_ChipGetMap(Chnl, &tmpRealChip, &tmpRealChnl, &tmpRealAddr)) {
		return -1;
	}

	int Gain;

	I2C_InLock();
	HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);
	HDDVR_i2c_WriteByte(tmpRealAddr, 0x06, 0x80);
	I2C_UnLock();

	if(0 > SleepWithVideoCheck(Chnl, 300000, 10)) { //3.0s
		return -1;
	}

	I2C_InLock();
	HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);
	HDDVR_i2c_WriteByte(tmpRealAddr, 0x07, 0x0F);
	I2C_UnLock();

	if(0 > SleepWithVideoCheck(Chnl, 300000, 2)) { //0.6s
		return -1;
	}

	I2C_InLock();
	HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);
	HDDVR_i2c_WriteByte(tmpRealAddr, 0x3A, 0x02);
	I2C_UnLock();

	if(0 > SleepWithVideoCheck(Chnl, 300000, 2)) { //0.6s
		return -1;
	}

	I2C_InLock();
	HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);
	Gain  = ((HDDVR_i2c_ReadByte(tmpRealAddr, 0x03) & 0xF0) >> 4);
	I2C_UnLock();

	ViFlags[Chnl] = 1;
	ViCount[Chnl] = 0;
	ViGains[Chnl] = Gain;
	VsFlags[Chnl] = 0;

	return 0;
}

int HDTVI_InputFormatCheck(int Chnl)
{
	int tmpRealChip;
	int tmpRealChnl;
	int tmpRealAddr;

	int tmpFormatVal = HDVIDEO_UNKNOWN;
	int tmpTryTimes  = 10;

	if(0 != HDTVI_ChipGetMap(Chnl, &tmpRealChip, &tmpRealChnl, &tmpRealAddr)) {
		return HDVIDEO_UNKNOWN;
	}

	if(HDVLOSS_INVIDEO != HDTVI_VideoVLossCheck(Chnl)) {
		return HDVIDEO_UNKNOWN;
	}

	while(tmpTryTimes --) {
		int HDTVI_FORMAT_CONVT_TBLS[HDVIDEO_CNT] = {
				HDVIDEO_HD720P60FPS,   HDVIDEO_HD720P50FPS,
				HDVIDEO_HD1080P30FPS,  HDVIDEO_HD1080P25FPS,
				HDVIDEO_HD720P30FPS,   HDVIDEO_HD720P25FPS,
			};

//		if(HDVLOSS_INVIDEO == HDTVI_VideoVLockCheck(Chnl))
		{
			unsigned char tmpRead;

			I2C_InLock();
			HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);
			tmpRead  = HDDVR_i2c_ReadByte(tmpRealAddr, 0x03); //Check Input Video Format
			I2C_UnLock();

			tmpRead &= 0x07;
			if(tmpRead < 0x06) {
				tmpFormatVal = HDTVI_FORMAT_CONVT_TBLS[tmpRead];
				break;
			}
		}
		usleep(50000);
	}

	return tmpFormatVal;
}

int HDTVI_InputFormatSetting(int Chnl, HD_INPUT_FORMAT Frmt)
{
	int tmpRealChip;
	int tmpRealChnl;
	int tmpRealAddr;

	if(0 != HDTVI_ChipGetMap(Chnl, &tmpRealChip, &tmpRealChnl, &tmpRealAddr)) {
		return -1;
	}

	if((Frmt >= HDVIDEO_CNT) || (Frmt < HDVIDEO_UNKNOWN)) {
		return -1;
	}

	I2C_InLock();
	HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);

	switch(Frmt) {
	default:
	case HDVIDEO_HD720P25FPS:
		I2CDeviceInitialize(tmpRealAddr, TP2802_720P25);
		break;
	case HDVIDEO_HD720P30FPS:
		I2CDeviceInitialize(tmpRealAddr, TP2802_720P30);
		break;
	case HDVIDEO_HD720P50FPS:
		I2CDeviceInitialize(tmpRealAddr, TP2802_720P50);
		break;
	case HDVIDEO_HD720P60FPS:
		I2CDeviceInitialize(tmpRealAddr, TP2802_720P60);
		break;
	case HDVIDEO_HD1080P25FPS:
		I2CDeviceInitialize(tmpRealAddr, TP2802_1080P25);
		break;
	case HDVIDEO_HD1080P30FPS:
		I2CDeviceInitialize(tmpRealAddr, TP2802_1080P30);
		break;
	}

	I2C_UnLock();

	return 0;
}

int HDTVI_VideoLossFix(int Chnl, int Fix)
{
	static int ttt[MAX_CAM_CH] =  //Force Enable HDTVI_VideoInVideoFix
#if (MAX_CAM_CH == 4)
	{ 1, 1, 1, 1, };
#endif
#if (MAX_CAM_CH == 8)
	{ 1, 1, 1, 1, 1, 1, 1, 1, };
#endif
#if (MAX_CAM_CH == 16)
	{ 1, 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1, 1, };
#endif

	if(!CHECK_SIZE(Chnl, ttt)) {
		return -1;
	}

	Fix = (HDVLOSS_INVIDEO != HDTVI_VideoVLossCheck(Chnl));
	if(Fix) {  //When Video Lost
		ttt[Chnl] = 1;

		HDTVI_VideoNoVideoFix(Chnl);
	}
	if(!Fix) {
		if((HDVLOSS_INVIDEO == HDTVI_VideoVLockCheck(Chnl)) && ttt[Chnl]) { //Once Video Input
			ttt[Chnl] = 0;

			HDTVI_VideoInVideoFix(Chnl);
		}
	}

	return 0;
}

int HDTVI_VideoSetColor(int Chnl, int nHue, int nBrightness, int nContrast, int nSaturation, int nSharpness)
{
	int tmpRealChip;
	int tmpRealChnl;
	int tmpRealAddr;

	#define VIDEO_COLOR_ADJTBL0_2 \
			{ -64,-62,-60,-58,-56,-54,-52,-50,\
			-48,-46,-44,-42,-40,-38,-36,-34,\
			-32,-30,-28,-26,-24,-22,-20,-18,\
			-16,-14,-12,-10,-8,-6,-4,-2,\
			0,\
			2,4,6,8,10,12,14,16,\
			18,20,22,24,26,28,30,32,\
			34,36,38,40,42,44,46,48,\
			50,52,54,56,58,60,62,63}
	#define VIDEO_COLOR_ADJTBL1_2 \
			{0,2,4,6,8,10,12,14, \
			16,18,20,22,24,26,28,30, \
			32,34,36,38,40,42,44,46, \
			48,50,52,54,56,58,60,62, \
			64, \
			66,68,70,72,74,76,78,80, \
			82,84,86,88,90,92,94,96, \
			98,100,102,104,106,108,110,112, \
			114,116,118,120,122,124,126,127}
	enum {
		HDTVI_COLOR_HUE = 0,
		HDTVI_COLOR_BRI,
		HDTVI_COLOR_CON,
		HDTVI_COLOR_SAT,
		HDTVI_COLOR_LST
	};
	const unsigned char COLOR_MAP[HDTVI_COLOR_LST][VIDEO_MAX_COLOR_GAIN+1] = {
		VIDEO_COLOR_ADJTBL0, //HUE
		VIDEO_COLOR_ADJTBL0_2, //BRI
		VIDEO_COLOR_ADJTBL1_2, //CON
		VIDEO_COLOR_ADJTBL1_2, //SAT
	};
	const unsigned char COLOR_TBL[HDTVI_COLOR_LST] = {0x13, 0x10, 0x11, 0x12};

	if(0 != HDTVI_ChipGetMap(Chnl, &tmpRealChip, &tmpRealChnl, &tmpRealAddr)) {
		return -1;
	}

	I2C_InLock();
	HDDVR_i2c_WriteByte(tmpRealAddr, 0x40, tmpRealChnl);
	if(nHue >= 0 && nHue <= VIDEO_MAX_COLOR_GAIN){
		HDDVR_i2c_WriteByte(tmpRealAddr,
					COLOR_TBL[HDTVI_COLOR_HUE],
					COLOR_MAP[HDTVI_COLOR_HUE][nHue]);
	}
	if(nBrightness >= 0 && nBrightness <= VIDEO_MAX_COLOR_GAIN){
		HDDVR_i2c_WriteByte(tmpRealAddr,
					COLOR_TBL[HDTVI_COLOR_BRI],
					COLOR_MAP[HDTVI_COLOR_BRI][nBrightness]);
	}
	if(nContrast >= 0 && nContrast <= VIDEO_MAX_COLOR_GAIN){
		HDDVR_i2c_WriteByte(tmpRealAddr,
					COLOR_TBL[HDTVI_COLOR_CON],
					COLOR_MAP[HDTVI_COLOR_CON][nContrast]);
	}
	if(nSaturation >= 0 && nSaturation <= VIDEO_MAX_COLOR_GAIN){
		HDDVR_i2c_WriteByte(tmpRealAddr,
					COLOR_TBL[HDTVI_COLOR_SAT],
					COLOR_MAP[HDTVI_COLOR_SAT][nSaturation]);
	}
	I2C_UnLock();

	return 0;
}

char * HDTVI_GetCHNLType(int Chnl)
{
	switch(HDTVI_InputFormatCheck(Chnl)) {
	default:
	case HDVIDEO_UNKNOWN:
	return "UNKNOWN";

    case HDVIDEO_SD720H25FPS:
    case HDVIDEO_SD960H25FPS:
	return "SD-PAL";
    case HDVIDEO_SD720H30FPS:
    case HDVIDEO_SD960H30FPS:
	return "SD-NTSC";

    case HDVIDEO_HD720P25FPS:
	return "TVI-720P@25FPS";
    case HDVIDEO_HD720P30FPS:
	return "TVI-720P@30FPS";

    case HDVIDEO_HD720P50FPS:
	return "TVI-720P@50FPS";
    case HDVIDEO_HD720P60FPS:
	return "TVI-720P@60FPS";

    case HDVIDEO_HD1080P25FPS:
	return "TVI-1080P@25FPS";
    case HDVIDEO_HD1080P30FPS:
	return "TVI-1080P@30FPS";
	}

	return "UNKNOWN";
}
