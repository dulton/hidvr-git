#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "jdk_common.h"

#include "bsp.h"

#include "hddvr_code.h"

#define VIDEO_DEV "/dev/video"

JDK_BOOL jdk_ad_standard_init(JDK_STANDARD_MODE standard, JDK_BOOL bBnc960H)
{
	// init adc standard pal / ntsc
	int fd = open(VIDEO_DEV, O_RDWR);
	if(fd > 0){
		video_std_t stStd;
		strcpy(stStd.std, (JDK_STANDARD_IS_PAL() ? "pal" : "ntsc"));
		strcpy(stStd.siz, (bBnc960H ? "960h" : "720h"));
		//strcpy(stStd.siz, (VIDEO_IS_960() ? "960h" : "720h"));
		ioctl(fd, VIDEO_DRV_SET_STANDARD, &stStd);
		close(fd);
		return JDK_TRUE;
	}
	return JDK_FALSE;
}


static JDK_BOOL jdk_def_ad_init(JDK_STANDARD_MODE standard, JDK_BOOL bBnc960H)
{
	if (jdk_ad_standard_init(standard, bBnc960H) == JDK_TRUE)
		return JDK_TRUE;
	else
		return JDK_FALSE;
}

static JDK_BOOL jdk_def_ad_set_color(JDK_UINT32 chn, JDK_FLOAT brightness, JDK_FLOAT contrast, JDK_FLOAT saturation, JDK_FLOAT hue, JDK_FLOAT sharpness)
{
	int ret = -1;
	int nHue = -1, nBrightness = -1, nContrast = -1, nSaturation = -1, nSharpness = -1;

	if(brightness >= 0 && brightness <= 1)
		nBrightness = brightness * VIDEO_MAX_COLOR_GAIN;
	if(saturation >= 0 && saturation <= 1)
		nSaturation = saturation * VIDEO_MAX_COLOR_GAIN;
	if(contrast >= 0 && contrast <= 1)
		nContrast = contrast * VIDEO_MAX_COLOR_GAIN;
	if(hue >= 0 && hue <= 1)
		nHue = hue * VIDEO_MAX_COLOR_GAIN;
	if(sharpness >= 0 && sharpness <= 1)
		nSharpness = sharpness * VIDEO_MAX_COLOR_GAIN;
	
	//printf("Set Color [%d] (%d,%d,%d,%d)\n", chn, nBrightness, nContrast, nSaturation, nHue);
//#if  defined(_EXT_HDDVR)
	ret = HDDVR_VideoSetColor(chn, nHue, nBrightness, nContrast, nSaturation, nSharpness);
//#else
//	ret = VIDEODRV_SetColor(nFileD, nChn, nHue, nBrightness, nContrast, nSaturation);
//#endif
	if (ret == 0)
		return JDK_TRUE;
	else
		return JDK_FALSE;
}

static JDK_BOOL jdk_def_ad_get_color(JDK_UINT32 chn, JDK_FLOAT *brightness, JDK_FLOAT *contrast, JDK_FLOAT *saturation, JDK_FLOAT *hue, JDK_FLOAT *sharpness)
{
	return JDK_FALSE;
}

static JDK_BOOL jdk_def_ad_is_loss(JDK_UINT32 chn)
{
	if (HDDVR_VideoLossChnl(chn) != 0)
		return JDK_TRUE;
	else
		return JDK_FALSE;
}

static JDK_INT32 jdk_def_ad_probe_vi_format(JDK_UINT32 chn)
{
	int tmpVideoStds;
	int tmpVideoLoss;
	int tmpStdCheck = JDK_INPUT_FORMAT_UNKNOWN;

	tmpVideoStds = HDDVR_InputFormatCheck(chn);
	tmpVideoLoss = HDDVR_VideoLossChnl(chn);

	//if (tmpVideoLoss != 0) {
	//	return JDK_INPUT_FORMAT_UNKNOWN;
	//}
	switch(tmpVideoStds) {
		case HDVIDEO_SD720H25FPS: case HDVIDEO_SD720H30FPS:
		case HDVIDEO_SD960H25FPS: case HDVIDEO_SD960H30FPS:
		case HDVIDEO_SD960H:
			tmpStdCheck = JDK_INPUT_BNC_960H;
			break;
		case HDVIDEO_HD720P25FPS: case HDVIDEO_HD720P30FPS:
		case HDVIDEO_HD720P50FPS: case HDVIDEO_HD720P60FPS:
		case HDVIDEO_HD720P:
			tmpStdCheck = JDK_INPUT_BNC_720P;
			break;
		case HDVIDEO_HD1080P25FPS: case HDVIDEO_HD1080P30FPS:
		case HDVIDEO_HD1080P:
			tmpStdCheck = JDK_INPUT_BNC_1080P;
		default:
		case HDVIDEO_UNKNOWN:
			break;
	}

	return tmpStdCheck;
}

static JDK_INT32 jdk_def_ad_fix_eq(JDK_UINT32 chn)
{
	HDDVR_VideoStVideoFix(chn);

	return 0;
}

static JDK_BOOL jdk_def_ad_deinit()
{
	return JDK_TRUE;
}


stJDK_AD_IFACE g_jdkAdIface = 
{
	jdk_def_ad_init,
	jdk_def_ad_is_loss,
	jdk_def_ad_probe_vi_format,
	jdk_def_ad_set_color,
	jdk_def_ad_get_color,
	jdk_def_ad_fix_eq,
	jdk_def_ad_deinit,
};


