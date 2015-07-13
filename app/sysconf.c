
#include "sysconf.h"
#include "conf.h"
#include "environment.h"

stJA_SYS_CONFIG g_JSysConfig = {0, 0, 0, 0};

void JA_SYS_CONFIG_init0(int nBncCam, int nNetCam, int nstream)
{
	g_JSysConfig.nBncCam = nBncCam;
	g_JSysConfig.nNetCam = nNetCam;

	g_JSysConfig.nstream = nstream;
}


stJA_SYS_RESOLUTION JA_SYS_CONFIG_getResolMap(enJA_SYS_RESOLUTION_TYPE resol_type)
{
	const int mapRecsol[JA_RESOL_CNT][2] = {
												{704,576},//D1
												{960,576},//960H
												{1280,720},//720P
												{960, 1080},//HFHD
												{1280,960},//960P
												{1920,1080},//1080P
											}; 

	stJA_SYS_RESOLUTION stResolution={0,0};

	if(resol_type<0 || resol_type>=JA_RESOL_CNT){
		return stResolution;
	}

	stResolution.width = mapRecsol[resol_type][0];
	stResolution.height = mapRecsol[resol_type][1];

	return stResolution;
}

#ifdef _HVR
stJA_SYS_RESOLUTION JA_SYS_CONFIG_getDecMaxCap(unsigned int Chn,int totalDecChn)
{
	//totalChn   Chn  MAX_DEC_RESOLUTION 
	int i;
	stJA_SYS_RESOLUTION stResolution = {0,0};
	unsigned int decLossByEnc = 0; // FIX ME
	
#if defined _HVR  && defined _HI3520D_EXT
	//ensure chn 0 can decode
	if(totalDecChn<3){
		if(Chn==0){
			stResolution = JA_SYS_CONFIG_getResolMap(JA_1080P);
			return stResolution;
		}else{
			decLossByEnc = 1920*1080*10;
		}
	}else{
		decLossByEnc = 1920*1080;	//reduce deviate decode resolution
	}
#endif

#ifdef 	DEC_FULL_FRAME
	int fps = FULL_DEC_FPS;
#else
	int fps = MIN_DEC_FPS;
#endif

	for(i = 0;i<JA_RESOL_CNT;++i)
	{
		stResolution = JA_SYS_CONFIG_getResolMap(i);
		if(stResolution.width*stResolution.height*fps >= (MAX_DEC_RESOLUTION - decLossByEnc)/totalDecChn){
			if(i == 0){
				stResolution = JA_SYS_CONFIG_getResolMap(i);
			}else{
				stResolution = JA_SYS_CONFIG_getResolMap(i-1);
			}
			break;
		}
	}
	return stResolution;
}

#define LIVE_DEC_MAX_FPS (30)
int JA_SYS_CONFIG_getMaxMainStream(int liveDecChn,int totalDecChn)
{
	int i;
	int max_mainstr_chn = 0;
	unsigned int decLossByEnc = 0;

	stJA_SYS_RESOLUTION stResolution_D1 = JA_SYS_CONFIG_getResolMap(JA_D1);
	stJA_SYS_RESOLUTION stResolution_1080P = JA_SYS_CONFIG_getResolMap(JA_1080P);

	unsigned int DecCap_D1 = stResolution_D1.width * stResolution_D1.height * LIVE_DEC_MAX_FPS;
	unsigned int DecCap_1080P = stResolution_1080P.width * stResolution_1080P.height * LIVE_DEC_MAX_FPS;



#if defined _HI3520D_EXT
	decLossByEnc = DecCap_D1;
#else if defined SDK_PLATFORM_HI3531
	decLossByEnc = DecCap_1080P;
#endif

	for(max_mainstr_chn = liveDecChn; max_mainstr_chn > 0; max_mainstr_chn--)
	{
		if((DecCap_1080P * max_mainstr_chn + DecCap_D1 * (totalDecChn - max_mainstr_chn)) <= (MAX_DEC_RESOLUTION - decLossByEnc))
		{
			return max_mainstr_chn;
		}
	}

	return max_mainstr_chn;
}


#endif /* #ifdef _HVR */

static unsigned int _enc_format_to_pixel(unsigned int format)
{
	unsigned int ret = 0;
	switch (format) {
		case ENC_FMT_QCIF:
			ret = 176*144;
			break;
		case ENC_FMT_CIF:
			ret = 352*288;
			break;
		case ENC_FMT_HD1:
			ret = 352 * 576;
			break;
		case ENC_FMT_D1:
			ret = 704*576;
			break;
		case ENC_FMT_WCIF:
			ret = 480*288;
			break;
		case ENC_FMT_960H:
			ret = 960*576;
			break;
		case ENC_FMT_HD720P:
			ret = 1280*720;
			break;	
		case ENC_FMT_HD1080P_HALF:
			ret = 960*1080;
			break;
		case ENC_FMT_HD1080P:
			ret = 1920*1080;
			break;
		default:
			ret = 0;
			break;
	}
	return ret;
}

unsigned int JA_SYS_CONFIG_getEncMaxFps(unsigned int fmt, unsigned int streamIndex)
{
	int ret = 0;
	unsigned int pixels = _enc_format_to_pixel(fmt);
	if (pixels == 0) {
		return 0;
	}
	if (streamIndex == 0) {
		ret = (MAX_BNC_1ST_RESOL*MAX_BNC_1ST_FPS) /pixels;
	}
	else{
		ret = MAX_BNC_2ST_FPS;
	}
	return ret;
}

