
#include <string.h>


#include "conf.h"  //Import MAX_CAM_CH Definition
#include "envfix.h"

#include "hddvr_code.h"

int   HDDVR_i2c_port_init(void);
void  HDDVR_i2c_port_exit(void);

enum {
	HDCHIP_NONE = 0,
	HDCHIP_HDCVI,
	HDCHIP_HDTVI,
	HDCHIP_HDAHD,
	HDCHIP_HDFHD,
};

int gHDChip = HDCHIP_NONE;

int HDDVR_Probe(void)
{
	if(0 == HDTVI_Probe()) {
		return HDCHIP_HDTVI;
	}

	if(0 == HDAHD_Probe()) {
		return HDCHIP_HDAHD;
	}

	return HDCHIP_NONE;
}

int HDDVR_VSupportCheck(void)
{
	if(gHDChip == HDCHIP_HDTVI) {
		return HDTVI_VSupportCheck();
	}

	if(gHDChip == HDCHIP_HDAHD) {
		return HDAHD_VSupportCheck();
	}

	return 0;
}

int HDDVR_Init(int Fmt)
{
	HDDVR_i2c_port_init();

	gHDChip = HDDVR_Probe();

	if(gHDChip == HDCHIP_HDTVI) {
		HDTVI_Init(Fmt);
	}
	if(gHDChip == HDCHIP_HDAHD) {
		HDAHD_Init(Fmt);
	}

	return 0;
}

char * HDDVR_GetMODELType(void)
{
	switch(gHDChip) {
	case HDCHIP_NONE:
		return "";
	case HDCHIP_HDCVI:
		return "CVI";
	case HDCHIP_HDTVI:
		return "TVI";
	case HDCHIP_HDAHD:
		return "AHD";
	case HDCHIP_HDFHD:
		return "FHD";
	}

	return "";
}

char * HDDVR_GetCHNLType(int Chnl)
{
	switch(gHDChip) {
	case HDCHIP_NONE:
		return "";
	case HDCHIP_HDCVI:
		return "";
	case HDCHIP_HDTVI:
		return "";//HDTVI_GetCHNLType(Chnl);
	case HDCHIP_HDAHD:
		return "";//HDAHD_GetCHNLType(Chnl);
	case HDCHIP_HDFHD:
		return "";
	}

	return "";
}

int HDDVR_Exit(void)
{
	if(gHDChip == HDCHIP_HDTVI) {
		HDTVI_Exit();
	}
	if(gHDChip == HDCHIP_HDAHD) {
		HDAHD_Exit();
	}

	HDDVR_i2c_port_exit();

	return 0;
}

int HDDVR_InputFormatCheck(int Chnl)
{
	if(gHDChip == HDCHIP_HDTVI) {
		return HDTVI_InputFormatCheck(Chnl);
	}
	if(gHDChip == HDCHIP_HDAHD) {
		return HDAHD_InputFormatCheck(Chnl);
	}

	return HDVIDEO_UNKNOWN;
}

int HDDVR_InputFormatSetting(int Chnl, HD_INPUT_FORMAT Ifrmt, HD_OUTPUT_FORMAT Ofrmt)
{
	if(gHDChip == HDCHIP_HDTVI) {
		return HDTVI_InputFormatSetting(Chnl, Ifrmt);
	}
	if(gHDChip == HDCHIP_HDAHD) {
		return HDAHD_InputFormatSetting(Chnl, Ifrmt, Ofrmt);
	}

	return -1;
}

int HDDVR_VideoSetColor(int Chnl, int nHue, int nBrightness, int nContrast, int nSaturation, int nSharpness)
{
	if(gHDChip == HDCHIP_HDTVI) {
		return HDTVI_VideoSetColor(Chnl, nHue, nBrightness, nContrast, nSaturation, nSharpness);
	}

	if(gHDChip == HDCHIP_HDAHD) {
		return HDAHD_VideoSetColor(Chnl, nHue, nBrightness, nContrast, nSaturation, nSharpness);
	}

	return 0;
}

int HDDVR_VideoLossChnl(int Chnl)
{
	if(gHDChip == HDCHIP_HDTVI) {
		return (HDVLOSS_INVIDEO != HDTVI_VideoVLossCheck(Chnl));
	}

	if(gHDChip == HDCHIP_HDAHD) {
		return (HDVLOSS_INVIDEO != HDAHD_VideoVLossCheck(Chnl));
	}

	return 0;
}

int HDDVR_VideoLoss(void)
{
	int tmpLoss = 0;
	int tmpChck;
	int ii;

	for(ii = MAX_CAM_CH-1; ii >= 0; ii --) {

		tmpLoss <<= 1;

		if(gHDChip == HDCHIP_HDTVI) {
			tmpChck = (HDVLOSS_INVIDEO != HDTVI_VideoVLossCheck(ii));
		}

		if(gHDChip == HDCHIP_HDAHD) {
			tmpChck = (HDVLOSS_INVIDEO != HDAHD_VideoVLossCheck(ii));
		}

		tmpLoss |= (tmpChck ?1 :0);
	}

	return tmpLoss;
}

int HDDVR_VideoLossFix(int Chnl, int Fix)
{
	if(gHDChip == HDCHIP_HDTVI) {
		return HDTVI_VideoLossFix(Chnl, Fix);
	}
	if(gHDChip == HDCHIP_HDAHD) {
		return HDAHD_VideoLossFix(Chnl, Fix);
	}

	return 0;
}

int HDDVR_VideoStVideoFix(int Chnl)
{
	if(gHDChip == HDCHIP_HDTVI) {
		return HDTVI_VideoStVideoFix(Chnl);
	}
	if(gHDChip == HDCHIP_HDAHD) {
		return HDAHD_VideoStVideoFix(Chnl);
	}

	return 0;
}

int HDDVR_GetFormatInfo(HD_INPUT_FORMAT Frmt, int * SizW, int * SizH, int * Fps)
{
	int ViInfoTBL[HDVIDEO_CNT2][3] = {
		{ 720,	576, 25}, { 720,  480, 30}, //SD720H
		{ 960,	576, 25}, { 960,  480, 30}, //SD960H
		{1280,	720, 25}, {1280,  720, 30}, //HD720P
		{1280,	720, 50}, {1280,  720, 60}, //HD720P
		{1920, 1080, 25}, {1920, 1080, 30}, //HD1080P
		{ 0, 0, 0},
		{ 960, 0, 0},
		{ 1280, 720, 0},
		{ 1920, 1080, 0},
	};
	
	if((Frmt <= HDVIDEO_UNKNOWN) || (Frmt >= HDVIDEO_CNT2)) {
		return -1;
	}
	if (ViInfoTBL[Frmt][0] == 0) {
		return -1;
	}

	if(SizW) {
		*SizW = ViInfoTBL[Frmt][0];
	}

	if(SizH) {
		*SizH = ViInfoTBL[Frmt][1];
		if (*SizH == 0) {
			*SizH = VIDEO_IS_PAL() ? 576 : 480;
		}
	}

	if(Fps) {
		*Fps  = ViInfoTBL[Frmt][2];
		if (*Fps == 0) {
			*Fps = VIDEO_IS_PAL() ? 25: 30;
		}
	}
	

	return 0;
}

int HDDVR_AudioLiveLoop(int Chnl)
{
	if(gHDChip == HDCHIP_HDAHD) {
		return HDAHD_AudioLiveLoop(Chnl);
	}

	return 0;
}

int HDDVR_AudioVolume(int Chnl, int Vol)
{
	if(gHDChip == HDCHIP_HDAHD) {
		return HDAHD_AudioVolume(Chnl, Vol);
	}

	return 0;
}

int HDDVR_AudioPlayBack(int Chnl)
{
	if(gHDChip == HDCHIP_HDAHD) {
		return HDAHD_AudioPlayBack(Chnl);
	}

	return 0;
}

int HDDVR_UTC_Init(int video_mode)
{
	if(gHDChip == HDCHIP_HDAHD)
	{
		HDAHD_UTC_Init(video_mode);
	}

	return 0;

}

int HDDVR_UTC_Send(int Chnl, HDDVR_UTC_CMD Cmd, unsigned char value)
{
	int Revalue = 0;

	if(gHDChip == HDCHIP_HDAHD)
	{
		Revalue = HDAHD_UTC_Send(Chnl, Cmd, value);
	}

	return Revalue;
}

int HDDVR_UTC_Exit()
{
	if(gHDChip == HDCHIP_HDAHD)
	{
		return HDAHD_UTC_Exit();
	}

	return 0;
}
