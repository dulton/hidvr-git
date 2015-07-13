
#include "avenc_map.h"

static void avenc_VideoPreProc(int nVeGrp, int nVeChn)
{
}

static void avenc_AEncInit()
{
#define AENC_G711A_USE
//#define AENC_ADPCM_USE
//#define AENC_LPCM_USE
	int i = 0;
#if defined(AENC_G711A_USE)
	AENC_ATTR_G711_S stAencG711A = {0};
	AENC_CHN_ATTR_S stAencAttr = {.enType = PT_G711A, .pValue = &stAencG711A,};
#elif defined(AENC_ADPCM_USE)
	AENC_ATTR_ADPCM_S stAencADPCM = {.enADPCMType = ADPCM_TYPE_DVI4, };
	AENC_CHN_ATTR_S stAencAttr = {.enType = PT_ADPCMA, .pValue = &stAencADPCM,};
#elif defined(AENC_LPCM_USE)
	AENC_ATTR_LPCM_S stAencLPCM = {0};
	AENC_CHN_ATTR_S stAencAttr = {.enType = PT_LPCM, .pValue = &stAencLPCM,};
#else
#error "Audio Encode Format Not Supported"
#endif

//	stAencAttr.u32BufSize = 8;
//	stAencAttr.u32BufSize = MAX_AUDIO_FRAME_NUM; // ver 050 faq 1.6.3
	stAencAttr.u32BufSize = 5;

	// hisilicon SDK
	for(i = 0; i < AVENC_AUDIO_MAX_CH; ++i){
		if(g_pstSysEnv->GetEncMode(g_pstSysEnv, i) == MODE_AUDIO_VIDEO){
			if(AUDIO_IsCapLiveLoop()){
				// if with audio live loop
				// audio live init here
				DVR_ASSERT(HI_MPI_AI_EnableChn(AIN_DEV, i));
			}
			// create aenc chn
			DVR_ASSERT(HI_MPI_AENC_CreateChn(i, &stAencAttr));
			// bind AENC to AI channel
			DVR_ASSERT(HI_MPI_AENC_BindAi(i, AIN_DEV, i));
		}
	}
}

static int avenc_AEncDestroy()
{
	int i = 0;
	for(i = 0; i < AVENC_AUDIO_MAX_CH; ++i){
		// unbind AENC
		DVR_ASSERT(HI_MPI_AENC_UnBindAi(i, AIN_DEV, i));
		// destroy aenc chn
		DVR_ASSERT(HI_MPI_AENC_DestroyChn(i));
		if(AUDIO_IsCapLiveLoop()){
			DVR_ASSERT(HI_MPI_AI_DisableChn(AIN_DEV, i));
		}
	}
	return 0;
}

