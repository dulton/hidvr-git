
#include "sdk_pri_api.h"

#if defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)

HI_S32 HI_MPI_AO_BindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn)
{
	MPP_CHN_S stSrcChn, stDesChn;

	stSrcChn.enModId  = HI_ID_ADEC;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = AdChn;

	stDesChn.enModId  = HI_ID_AO;
	stDesChn.s32DevId = AoDev;
	stDesChn.s32ChnId = AoChn;

	return HI_MPI_SYS_Bind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_AO_UnBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn)
{
	MPP_CHN_S stSrcChn, stDesChn;

	stSrcChn.enModId  = HI_ID_ADEC;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = AdChn;

	stDesChn.enModId  = HI_ID_AO;
	stDesChn.s32DevId = AoDev;
	stDesChn.s32ChnId = AoChn;

	return HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_AENC_BindAi(AENC_CHN AeChn, AUDIO_DEV AiDev, AI_CHN AiChn)
{
	MPP_CHN_S stSrcChn, stDesChn;

	stSrcChn.enModId  = HI_ID_AI;
	stSrcChn.s32DevId = AiDev;
	stSrcChn.s32ChnId = AiChn;

	stDesChn.enModId  = HI_ID_AENC;
	stDesChn.s32DevId = 0;
	stDesChn.s32ChnId = AeChn;

	return HI_MPI_SYS_Bind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_AENC_UnBindAi(AENC_CHN AeChn, AUDIO_DEV AiDev, AI_CHN AiChn)
{
	MPP_CHN_S stSrcChn, stDesChn;

	stSrcChn.enModId  = HI_ID_AI;
	stSrcChn.s32DevId = AiDev;
	stSrcChn.s32ChnId = AiChn;

	stDesChn.enModId  = HI_ID_AENC;
	stDesChn.s32DevId = 0;
	stDesChn.s32ChnId = AeChn;

	return HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_Ao_BindAi(AUDIO_DEV AoDev, AUDIO_DEV AiDev, AI_CHN AiChn)
{
	MPP_CHN_S stSrcChn, stDesChn;

	stSrcChn.enModId  = HI_ID_AI;
	stSrcChn.s32DevId = AiDev;
	stSrcChn.s32ChnId = AiChn;

	stDesChn.enModId  = HI_ID_AO;
	stDesChn.s32DevId = AoDev;
	stDesChn.s32ChnId = 0;

	return HI_MPI_SYS_Bind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_Ao_BindAiByForce(AUDIO_DEV AoDev, AUDIO_DEV AiDev, AI_CHN AiChn)
{
	MPP_CHN_S stSrcChn, stDesChn;

	memset(&stSrcChn, 0, sizeof(stSrcChn));

	stDesChn.enModId  = HI_ID_AO;
	stDesChn.s32DevId = AoDev;
	stDesChn.s32ChnId = 0;

	if(!HI_MPI_SYS_GetBindbyDest(&stDesChn, &stSrcChn)) {
		HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn);
	}

	stSrcChn.enModId  = HI_ID_AI;
	stSrcChn.s32DevId = AiDev;
	stSrcChn.s32ChnId = AiChn;

	return HI_MPI_SYS_Bind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_Ao_UnBindAi(AUDIO_DEV AoDev, AUDIO_DEV AiDev, AI_CHN AiChn)
{
	MPP_CHN_S stSrcChn, stDesChn;

	stSrcChn.enModId  = HI_ID_AI;
	stSrcChn.s32DevId = AiDev;
	stSrcChn.s32ChnId = AiChn;

	stDesChn.enModId  = HI_ID_AO;
	stDesChn.s32DevId = AoDev;
	stDesChn.s32ChnId = 0;

	return HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_Ao_BindAdecByForce(AUDIO_DEV AoDev, ADEC_CHN AdChn)
{
	MPP_CHN_S stSrcChn, stDesChn;

	memset(&stSrcChn, 0, sizeof(stSrcChn));

	stDesChn.enModId  = HI_ID_AO;
	stDesChn.s32DevId = AoDev;
	stDesChn.s32ChnId = 0;

	if(!HI_MPI_SYS_GetBindbyDest(&stDesChn, &stSrcChn)) {
		HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn);
	}

	stSrcChn.enModId  = HI_ID_ADEC;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = AdChn;

	return HI_MPI_SYS_Bind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_VENC_BindVpss(VENC_GRP VeGroup, HI_S32 VpssGrp, HI_S32 VpssChn)
{
	MPP_CHN_S stSrcChn,stDesChn;

	stSrcChn.enModId  = HI_ID_VPSS;
	stSrcChn.s32DevId = VpssGrp;
	stSrcChn.s32ChnId = VpssChn;

	stDesChn.enModId  = HI_ID_GROUP;
	stDesChn.s32DevId = VeGroup;
	stDesChn.s32ChnId = 0;

	return HI_MPI_SYS_Bind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_VENC_UnBindVpss(VENC_GRP VeGroup, HI_S32 VpssGrp, HI_S32 VpssChn)
{
	MPP_CHN_S stSrcChn,stDesChn;

	stSrcChn.enModId  = HI_ID_VPSS;
	stSrcChn.s32DevId = VpssGrp;
	stSrcChn.s32ChnId = VpssChn;

	stDesChn.enModId  = HI_ID_GROUP;
	stDesChn.s32DevId = VeGroup;
	stDesChn.s32ChnId = 0;

	return HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_VO_BindVpss(HI_S32 VoGrp, HI_S32 VoChn, HI_S32 VpssGrp, HI_S32 VpssChn)
{
	MPP_CHN_S stSrcChn,stDesChn;

	stSrcChn.enModId  = HI_ID_VPSS;
	stSrcChn.s32DevId = VpssGrp;
	stSrcChn.s32ChnId = VpssChn;

	stDesChn.enModId  = HI_ID_VOU;
	stDesChn.s32DevId = VoGrp;
	stDesChn.s32ChnId = VoChn;

	return HI_MPI_SYS_Bind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_VO_UnBindVpss(HI_S32 VoGrp, HI_S32 VoChn, HI_S32 VpssGrp, HI_S32 VpssChn)
{
	MPP_CHN_S stSrcChn,stDesChn;

	stSrcChn.enModId  = HI_ID_VPSS;
	stSrcChn.s32DevId = VpssGrp;
	stSrcChn.s32ChnId = VpssChn;

	stDesChn.enModId  = HI_ID_VOU;
	stDesChn.s32DevId = VoGrp;
	stDesChn.s32ChnId = VoChn;

	return HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_Vpss_BindVi(HI_S32 VpssGrp, HI_S32 ViChn)
{
	MPP_CHN_S stSrcChn,stDesChn;

	stSrcChn.enModId  = HI_ID_VIU;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = ViChn;

	stDesChn.enModId  = HI_ID_VPSS;
	stDesChn.s32DevId = VpssGrp;
	stDesChn.s32ChnId = 0;

	return HI_MPI_SYS_Bind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_Vpss_UnBindVi(HI_S32 VpssGrp, HI_S32 ViChn)
{
	MPP_CHN_S stSrcChn,stDesChn;

	stSrcChn.enModId  = HI_ID_VIU;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = ViChn;

	stDesChn.enModId  = HI_ID_VPSS;
	stDesChn.s32DevId = VpssGrp;
	stDesChn.s32ChnId = 0;

	return HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_Vpss_BindVdec(HI_S32 VpssGrp, HI_S32 VdecChn)
{
	MPP_CHN_S stSrcChn,stDesChn;

	stSrcChn.enModId  = HI_ID_VDEC;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = VdecChn;

	stDesChn.enModId  = HI_ID_VPSS;
	stDesChn.s32DevId = VpssGrp;
	stDesChn.s32ChnId = 0;

	return HI_MPI_SYS_Bind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_Vpss_UnBindVdec(HI_S32 VpssGrp, HI_S32 VdecChn)
{
	MPP_CHN_S stSrcChn,stDesChn;

	stSrcChn.enModId  = HI_ID_VDEC;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = VdecChn;

	stDesChn.enModId  = HI_ID_VPSS;
	stDesChn.s32DevId = VpssGrp;
	stDesChn.s32ChnId = 0;

	return HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_Sys_BindByForce(
	HI_S32 DesTyp, HI_S32 DesGrp, HI_S32 DesChn,
	HI_S32 SrcTyp, HI_S32 SrcGrp, HI_S32 SrcChn)
{
	MPP_CHN_S stSrcChn,stDesChn;

	stDesChn.enModId  = DesTyp;
	stDesChn.s32DevId = DesGrp;
	stDesChn.s32ChnId = DesChn;	

	if(HI_SUCCESS == HI_MPI_SYS_GetBindbyDest(&stDesChn, &stSrcChn)) {
		if(SrcTyp == stSrcChn.enModId
		&& SrcGrp == stSrcChn.s32DevId
		&& SrcChn == stSrcChn.s32ChnId) {
			return HI_SUCCESS;
		}

		HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn);
	}

	stSrcChn.enModId  = SrcTyp;
	stSrcChn.s32DevId = SrcGrp;
	stSrcChn.s32ChnId = SrcChn;

	return HI_MPI_SYS_Bind(&stSrcChn, &stDesChn);
}

HI_S32 HI_MPI_Sys_UnBindByForce(
	HI_S32 DesTyp, HI_S32 DesGrp, HI_S32 DesChn)
{
	MPP_CHN_S stSrcChn,stDesChn;

	stDesChn.enModId  = DesTyp;
	stDesChn.s32DevId = DesGrp;
	stDesChn.s32ChnId = DesChn;	

	if(HI_SUCCESS == HI_MPI_SYS_GetBindbyDest(&stDesChn, &stSrcChn)) {
		return HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn);
	}

	return HI_SUCCESS;
}

const HI_U32 HDMI_CONVTBL[][2] = {
{VO_OUTPUT_PAL,          HI_HDMI_VIDEO_FMT_PAL},
{VO_OUTPUT_NTSC,         HI_HDMI_VIDEO_FMT_NTSC},
{VO_OUTPUT_1080P24,      HI_HDMI_VIDEO_FMT_1080P_24},
{VO_OUTPUT_1080P25,      HI_HDMI_VIDEO_FMT_1080P_25},
{VO_OUTPUT_1080P30,      HI_HDMI_VIDEO_FMT_1080P_30},
{VO_OUTPUT_720P50,       HI_HDMI_VIDEO_FMT_720P_50},
{VO_OUTPUT_720P60,       HI_HDMI_VIDEO_FMT_720P_60},
{VO_OUTPUT_1080I50,      HI_HDMI_VIDEO_FMT_1080i_50},
{VO_OUTPUT_1080I60,      HI_HDMI_VIDEO_FMT_1080i_60},
{VO_OUTPUT_1080P50,      HI_HDMI_VIDEO_FMT_1080P_50},
{VO_OUTPUT_1080P60,      HI_HDMI_VIDEO_FMT_1080P_60},
{VO_OUTPUT_576P50,       HI_HDMI_VIDEO_FMT_576P_50},
{VO_OUTPUT_480P60,       HI_HDMI_VIDEO_FMT_480P_60},
{VO_OUTPUT_800x600_60,   HI_HDMI_VIDEO_FMT_VESA_800X600_60},
{VO_OUTPUT_1024x768_60,  HI_HDMI_VIDEO_FMT_VESA_1024X768_60},
{VO_OUTPUT_1280x1024_60, HI_HDMI_VIDEO_FMT_VESA_1280X1024_60},
{VO_OUTPUT_1366x768_60,  HI_HDMI_VIDEO_FMT_VESA_1366X768_60},
{VO_OUTPUT_1440x900_60,  HI_HDMI_VIDEO_FMT_VESA_1440X900_60},
{VO_OUTPUT_1280x800_60,  HI_HDMI_VIDEO_FMT_VESA_1280X800_60},
};

HI_S32 HI_VO_HdmiStart(VO_INTF_SYNC_E enIntfSync)
{
    int ii = 0;
    while (HDMI_CONVTBL[ii][0] != enIntfSync) {
        if(ii >= ARRAY_SIZE(HDMI_CONVTBL)) {
            return -1;
        }
        ii ++;
    }

    HI_MPI_HDMI_Stop(HI_HDMI_ID_0);
    HI_MPI_HDMI_Close(HI_HDMI_ID_0);
    HI_MPI_HDMI_DeInit();

    {
        HI_HDMI_INIT_PARA_S stHdmiPara;
        stHdmiPara.enForceMode          = HI_HDMI_FORCE_HDMI;
        stHdmiPara.pCallBackArgs        = NULL;
        stHdmiPara.pfnHdmiEventCallback = NULL;
        HI_MPI_HDMI_Init(&stHdmiPara);
    }

    {
        HI_HDMI_ATTR_S      stAttr;
        HI_MPI_HDMI_Open(HI_HDMI_ID_0);
        HI_MPI_HDMI_GetAttr(HI_HDMI_ID_0, &stAttr);

        stAttr.bEnableHdmi          = HI_TRUE;
        stAttr.bEnableVideo         = HI_TRUE;
        stAttr.enVideoFmt           = HDMI_CONVTBL[ii][1];
        stAttr.enVidOutMode         = HI_HDMI_VIDEO_MODE_RGB444;
        stAttr.enDeepColorMode      = HI_HDMI_DEEP_COLOR_24BIT;
        stAttr.bxvYCCMode           = HI_FALSE;
        stAttr.bEnableAudio         = HI_TRUE;
        stAttr.enSoundIntf          = HI_HDMI_SND_INTERFACE_I2S;
        stAttr.bIsMultiChannel      = HI_FALSE;
        stAttr.enBitDepth           = HI_HDMI_BIT_DEPTH_16;
		stAttr.enSampleRate         = HI_HDMI_SAMPLE_RATE_8K;
		stAttr.u8DownSampleParm     = HI_FALSE;
		stAttr.u8I2SCtlVbit         = 0;
        stAttr.bEnableAviInfoFrame  = HI_TRUE;
        stAttr.bEnableAudInfoFrame  = HI_TRUE;
        stAttr.bEnableSpdInfoFrame  = HI_FALSE;
        stAttr.bEnableMpegInfoFrame = HI_FALSE;
        stAttr.bDebugFlag           = HI_FALSE;
        stAttr.bHDCPEnable          = HI_FALSE;
        stAttr.b3DEnable            = HI_FALSE;

        HI_MPI_HDMI_SetAttr(HI_HDMI_ID_0, &stAttr);
        HI_MPI_HDMI_Start(HI_HDMI_ID_0);
    }

    return HI_SUCCESS;
}

HI_S32 HI_VO_HdmiStop(HI_VOID)
{
    HI_MPI_HDMI_Stop(HI_HDMI_ID_0);
    HI_MPI_HDMI_Close(HI_HDMI_ID_0);
    HI_MPI_HDMI_DeInit();

    return HI_SUCCESS;
}

HI_S32 HI_SYS_SetMemConf(HI_S32 Grp, HI_S32 Dev, HI_S32 Chn, HI_CHAR *Mmz)
{
	MPP_CHN_S stMppChnGRP;

	stMppChnGRP.enModId  = Grp;
	stMppChnGRP.s32DevId = Dev;
	stMppChnGRP.s32ChnId = Chn;

	return HI_MPI_SYS_SetMemConf(&stMppChnGRP, Mmz);
}

#endif
