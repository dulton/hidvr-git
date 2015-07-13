#ifndef __SDK_FIX_H__
#define __SDK_FIX_H__

#if defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
HI_S32 HI_MPI_AO_BindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
HI_S32 HI_MPI_AO_UnBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);

HI_S32 HI_MPI_AENC_BindAi(AENC_CHN AeChn, AUDIO_DEV AiDev, AI_CHN AiChn);
HI_S32 HI_MPI_AENC_UnBindAi(AENC_CHN AeChn, AUDIO_DEV AiDev, AI_CHN AiChn);

HI_S32 HI_MPI_Ao_BindAi(AUDIO_DEV AoDev, AUDIO_DEV AiDev, AI_CHN AiChn);
HI_S32 HI_MPI_Ao_BindAiByForce(AUDIO_DEV AoDev, AUDIO_DEV AiDev, AI_CHN AiChn);
HI_S32 HI_MPI_Ao_UnBindAi(AUDIO_DEV AoDev, AUDIO_DEV AiDev, AI_CHN AiChn);

HI_S32 HI_MPI_Ao_BindAdecByForce(AUDIO_DEV AoDev, ADEC_CHN AdChn);

HI_S32 HI_MPI_VENC_BindVpss(VENC_GRP VeGroup, HI_S32 VpssGrp, HI_S32 VpssChn);
HI_S32 HI_MPI_VENC_UnBindVpss(VENC_GRP VeGroup, HI_S32 VpssGrp, HI_S32 VpssChn);

HI_S32 HI_MPI_VO_BindVpss(HI_S32 VoGrp, HI_S32 VoChn, HI_S32 VpssGrp, HI_S32 VpssChn);
HI_S32 HI_MPI_VO_UnBindVpss(HI_S32 VoGrp, HI_S32 VoChn, HI_S32 VpssGrp, HI_S32 VpssChn);

HI_S32 HI_MPI_Vpss_BindVi(HI_S32 VpssGrp, HI_S32 ViChn);
HI_S32 HI_MPI_Vpss_UnBindVi(HI_S32 VpssGrp, HI_S32 ViChn);

HI_S32 HI_MPI_Vpss_BindVdec(HI_S32 VpssGrp, HI_S32 VdecChn);
HI_S32 HI_MPI_Vpss_UnBindVdec(HI_S32 VpssGrp, HI_S32 VdecChn);

HI_S32 HI_MPI_Sys_BindByForce(
	HI_S32 DesTyp, HI_S32 DesGrp, HI_S32 DesChn,
	HI_S32 SrcTyp, HI_S32 SrcGrp, HI_S32 SrcChn);
HI_S32 HI_MPI_Sys_UnBindByForce(
	HI_S32 DesTyp, HI_S32 DesGrp, HI_S32 DesChn);

HI_S32 HI_SYS_SetMemConf(HI_S32 Grp, HI_S32 Dev, HI_S32 Chn, HI_CHAR *Mmz);

HI_S32 HI_VO_HdmiStart(VO_INTF_SYNC_E enIntfSync);
HI_S32 HI_VO_HdmiStop(HI_VOID);

#define HI_MPI_VB_GetBlock(x, y) HI_MPI_VB_GetBlock(x, y, NULL)

#if   defined(SDK_PLATFORM_HI3521)
#define MMZ_ZONE_NAME0 NULL
#define MMZ_ZONE_NAME1 NULL

#elif defined(SDK_PLATFORM_HI3531)
#define MMZ_ZONE_NAME0 NULL
#define MMZ_ZONE_NAME1 "ddr1"

#endif

#define GET_MMZ_ZONE_NAME(x) ((x) ? (x) : "")

#endif

#endif //__SDK_FIX_H__
