

// gnu headers
//#define	NDEBUG
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <math.h>
#include <errno.h>

#include "jn_sdk/sdk_pri_api.h"

// my headers
#include "conf.h"
#include "environment.h"
#include "osd.h"
#include "timetick.h"
#include "dvr_debug.h"
#include "audio.h"
#include "video.h"
//#include "playback.h"
#include "vimap.h"

#include "envfix.h"

#if defined(_NVR)
#include "nvrpc_function.h"
#endif

#if defined(_EXT_HDDVR)
#include "hddvr_code.h"
#endif

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
#define VO_BG_COLOR (0xf0f0f0) // 24 bits black
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
#define VO_BG_COLOR (0x0)
#endif
#define VIDEO_DEV "/dev/video"


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
#ifdef _NVR
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
const unsigned int VO_DEV_ID[] = {VO_VGA_DEVICE};
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
const unsigned int VO_DEV_ID[] = {VO_VGA_DEVICE, VO_CVBS_DEVICE};
#endif
#else
const unsigned int VO_DEV_ID[] = {VO_VGA_DEVICE, VO_CVBS_DEVICE};
#endif

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
#define VO_ORIGIN_WIDTH  (VRES_SIZE_W_SD)
#define VO_ORIGIN_HEIGHT (VIDEO_IS_PAL() ? VRES_SIZE_H_PL : VRES_SIZE_H_NT)

#define DBG_VIDEO
#ifdef DBG_VIDEO
#define	VIDEO_TRACE(fmt,arg...) \
	do{printf("\033[1;35m""VIDEO->[%s]:%d ", __FILE__, __LINE__);printf(fmt,##arg);printf("\033[m\r\n");}while(0)
#else
#define	VIDEO_TRACE(fmt, args...)
#endif

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

typedef struct _DigitalZoom
{
	int nRatioX;
	int nRatioY;
	int nRatioW;
	int nRatioH;
}DigitalZoom;

typedef struct {
	int nEdgeX;
	int nEdgeY;
	int nEdgeW;
	int nEdgeH;
} ScreenEdgeS;

typedef struct Video
{
	// screen w,h
	int nScrWidth[2];
	int nScrHeight[2];
	ScreenEdgeS ScrnEdges[2];

	//int fdDev;
	int nCurDivi;
	int nCurPage;
	unsigned int nLossStat;
	int nAutoSwitch;
	unsigned char abDigitalZoom[MAX_CAM_CH];
	DigitalZoom astDigitalZoomRatio[MAX_CAM_CH];
	unsigned char bD1[MAX_CAM_CH];
	unsigned char bOutput;

//	unsigned char bEnable;
//	unsigned int bEnableChn;
	unsigned char bLiveMode;
}Video;
static Video s_stVideo;


void video_ScreenInit(void)
{
	VO_PUB_ATTR_S stVgaPubAttr;
	VO_PUB_ATTR_S stCvbsPubAttr;

	memset(&stVgaPubAttr,  0, sizeof(stVgaPubAttr));
	memset(&stCvbsPubAttr, 0, sizeof(stCvbsPubAttr));

	// vga output
	{
		VGA_RESOLUTION const enRes = g_pstSysEnv->GetVGAResolution(g_pstSysEnv);
		VIDEO_TRACE("VGA resolution = %d", enRes);

		stVgaPubAttr.u32BgColor = VO_BG_COLOR;
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
		stVgaPubAttr.enIntfType = VO_INTF_VGA;
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
		stVgaPubAttr.enIntfType = VO_INTF_VGA | VO_INTF_HDMI;
#endif
		switch(enRes) {
		default:
		case VGA_800x600:   stVgaPubAttr.enIntfSync = VO_OUTPUT_800x600_60;   break;
		case VGA_1024x768:  stVgaPubAttr.enIntfSync = VO_OUTPUT_1024x768_60;  break;
#if defined(_JA7216NC)
		case VGA_1280x1024: stVgaPubAttr.enIntfSync = VO_OUTPUT_1024x768_60; break;
#else
		case VGA_1280x1024: stVgaPubAttr.enIntfSync = VO_OUTPUT_1280x1024_60;  break;
#endif
		case VGA_1366x768:  stVgaPubAttr.enIntfSync = VO_OUTPUT_1366x768_60;  break;
		case VGA_1440x900:  stVgaPubAttr.enIntfSync = VO_OUTPUT_1440x900_60;  break;
#if   defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
		case VGA_1920x1080P50Hz:   stVgaPubAttr.enIntfSync = VO_OUTPUT_1080P50; break;
		case VGA_1920x1080P60Hz:   stVgaPubAttr.enIntfSync = VO_OUTPUT_1080P60; break;
#endif
		}
	}
	// cvbs
	{
		stCvbsPubAttr.u32BgColor = VO_BG_COLOR;
		stCvbsPubAttr.enIntfType = VO_INTF_CVBS;
		stCvbsPubAttr.enIntfSync = VIDEO_IS_PAL()
							? VO_OUTPUT_PAL
							: VO_OUTPUT_NTSC;
	}
	{
		// configuration active
		const VO_PUB_ATTR_S astPubAttr[] = {stVgaPubAttr, stCvbsPubAttr};
		int i;
		for(i = 0; i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0]); ++i){
#if   defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
			DVR_ASSERT(HI_SYS_SetMemConf(HI_ID_VOU, HI_VO_DEV(i), 0, i%2 ?MMZ_ZONE_NAME0 :MMZ_ZONE_NAME1));
#endif
			DVR_ASSERT(HI_MPI_VO_Disable(VO_DEV_ID[i]));
			DVR_ASSERT(HI_MPI_VO_SetPubAttr(VO_DEV_ID[i], &astPubAttr[i]));
#if  defined(_HI3520D_RAM_LITE)
			DVR_ASSERT(HI_MPI_VO_SetDispBufLen(VO_DEV_ID[i], (VO_DEV_ID[i] == VO_VGA_DEVICE) ?4 :3));
#endif
			DVR_ASSERT(HI_MPI_VO_Enable(VO_DEV_ID[i]));
#if   defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
            if(astPubAttr[i].enIntfType & VO_INTF_HDMI) {
                DVR_ASSERT(HI_VO_HdmiStart(astPubAttr[i].enIntfSync));
			}
#endif
		}
	}
}

void video_ScreenExit(void)
{
	int i = 0;
	for(i = 0; i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0]); ++i) {
		// disable device
		DVR_ASSERT(HI_MPI_VO_Disable(VO_DEV_ID[i]));
	}
#if   defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
	DVR_ASSERT(HI_VO_HdmiStop());
#endif
}

HI_S32 HI_MPI_VO_SENDFRAME(VO_DEV VoDev, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVFrame)
{
	int nRet = 0;
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
	nRet = HI_MPI_VO_SendFrame(VoDev, VoChn, pstVFrame);
	switch(nRet) {
	case HI_SUCCESS:
		break;
	case HI_ERR_VO_INVALID_CHNID:
	case HI_ERR_VO_NULL_PTR:
	case HI_ERR_VO_CHN_NOT_ENABLE:
	case HI_ERR_VO_ILLEGAL_PARAM:
	default:
		DVR_ASSERT(nRet);
		break;
	case HI_ERR_VO_BUSY:
		//DVR_ASSERT(HI_MPI_VO_ClearChnBuffer(VoDev, VoChn, HI_TRUE));
		break;
	}
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)

#ifdef _NVR
	HI_S32 is_bind = 0;
	MPP_CHN_S stSrcChn, stDesChn;
	stDesChn.enModId  = HI_ID_VPSS;
	stDesChn.s32DevId = 0;
	stDesChn.s32ChnId = VoChn + MAX_CAM_CH;
	memset(&stSrcChn, 0, sizeof(MPP_CHN_S));
	is_bind = HI_MPI_SYS_GetBindbyDest(&stDesChn, &stSrcChn);
	VIDEO_TRACE("stSrcChn.enModId=%d, stSrcChn.s32DevId=%d, stSrcChn.s32ChnId=%d",
			stSrcChn.enModId, stSrcChn.s32DevId, stSrcChn.s32ChnId);
	if(is_bind == 0 && stSrcChn.enModId != 0)
	{
		HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn);
	}
#else
#endif

    if(VoChn >= MAX_CAM_CH && MAX_CAM_CH < VIDEO_GetMaxDiv() && VoChn < VO_MAX_CHN_NUM) {
        int i;
        for(i = 0; i < VO_DEV_CNT; ++i){
            HI_S32 tmpVpssChn = VPSS_PRE0_CHN;
#if defined(FORCE_NORMAL_CVBS_OUTPUT)
            if(VO_CVBS_DEVICE == VO_DEV_ID[i]) {
                tmpVpssChn = VPSS_PRE1_CHN;
            }
#endif
            DVR_ASSERT(HI_MPI_VPSS_ResetGrp(VoChn+MAX_CAM_CH));
            (HI_MPI_Sys_BindByForce(HI_ID_VOU, VO_DEV_ID[i], VoChn,
                                    HI_ID_VPSS, VoChn+MAX_CAM_CH, tmpVpssChn));
        }
    }
	nRet = HI_MPI_VPSS_UserSendFrame(VoChn+MAX_CAM_CH, pstVFrame);
	switch(nRet) {
	case HI_SUCCESS:
		break;
	default:
		if(nRet != -1) {
			VIDEO_TRACE("HI_MPI_VPSS_UserSendFrame nRet = 0x%08X", nRet);
		}
		break;
	}

#ifdef _NVR
	if(is_bind == 0 && stSrcChn.enModId != 0)
	{
		HI_MPI_SYS_Bind(&stSrcChn, &stDesChn);
	}
#else
#endif
#endif
	return HI_SUCCESS;
}

#define VO_CHN_IS_ONTOP(div,page,ch) ((ch>=page*div) && (ch<(page+1)*div))

void video_SetCSC(int nDev,
	unsigned int nLuma, /* [0, 100] -1 no use */
	unsigned int nContr, /* [0, 100] -1 no use */
	unsigned int nHue, /* [0, 100] -1 no use */
	unsigned int nSatu /* [0, 100] -1 no use */)
{
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
	VO_CSC_S stVoCSC = {0};
	if(nLuma >= 0 && nLuma <= 100){
		stVoCSC.enCSCType = VO_CSC_LUMA;
		stVoCSC.u32Value = nLuma;
		DVR_ASSERT(HI_MPI_VO_SetDevCSC(nDev, &stVoCSC));
	}
	if(nContr >= 0 && nContr <= 100){
		stVoCSC.enCSCType = VO_CSC_CONTR;
		stVoCSC.u32Value = nContr;
		DVR_ASSERT(HI_MPI_VO_SetDevCSC(nDev, &stVoCSC));
	}
	if(nHue >= 0 && nHue <= 100){
		stVoCSC.enCSCType = VO_CSC_HUE;
		stVoCSC.u32Value = nHue;
		DVR_ASSERT(HI_MPI_VO_SetDevCSC(nDev, &stVoCSC));
	}
	if(nSatu >= 0 && nSatu <= 100){
		stVoCSC.enCSCType = VO_CSC_SATU;
		stVoCSC.u32Value = nSatu;
		DVR_ASSERT(HI_MPI_VO_SetDevCSC(nDev, &stVoCSC));
	}
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
	VO_CSC_S stVoCSC = {0};

    DVR_ASSERT(HI_MPI_VO_GetDevCSC(nDev, &stVoCSC));

	stVoCSC.enCscMatrix = VO_CSC_MATRIX_BT709_TO_BT601;
	if(nLuma >= 0 && nLuma <= 100){
		stVoCSC.u32Luma = nLuma;
	}
	if(nContr >= 0 && nContr <= 100){
		stVoCSC.u32Contrast = nContr;
	}
	if(nHue >= 0 && nHue <= 100){
		stVoCSC.u32Hue = nHue;
	}
	if(nSatu >= 0 && nSatu <= 100){
		stVoCSC.u32Satuature = nSatu;
	}

	DVR_ASSERT(HI_MPI_VO_SetDevCSC(nDev, &stVoCSC));
#endif
}

void video_SetVGACSC(int nDev,
	unsigned int nLuma, /* [0, 100] -1 no use */
	unsigned int nContr, /* [0, 100] -1 no use */
	unsigned int nHue, /* [0, 100] -1 no use */
	unsigned int nSatu, /* [0, 100] -1 no use */
	unsigned int nGain
)
{
#if defined(SDK_PLATFORM_HI3531)
	VO_VGA_PARAM_S stVgaParam = {0};

    DVR_ASSERT(HI_MPI_VO_GetVgaParam(nDev, &stVgaParam));

	if(nLuma >= 0 && nLuma <= 100){
		stVgaParam.u32Luma = nLuma;
	}
	if(nContr >= 0 && nContr <= 100){
		stVgaParam.u32Contrast = nContr;
	}
	if(nHue >= 0 && nHue <= 100){
		stVgaParam.u32Hue = nHue;
	}
	if(nSatu >= 0 && nSatu <= 100){
		stVgaParam.u32Satuature = nSatu;
	}
	if(nGain >= 0 && nGain <= 0x3F){
		stVgaParam.u32Gain = nGain;
	}

	DVR_ASSERT(HI_MPI_VO_SetVgaParam(nDev, &stVgaParam));
#endif
}

static void video_CSCInit()
{
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
	if(GPIODRV_GetVerFix() == GPIO_PROBE_VER1) {
		video_SetCSC(VO_VGA_DEVICE, 50, 50, 50, 50);
	}
	else {
		video_SetCSC(VO_VGA_DEVICE, 50, 30, 50, 58);
	}
#ifndef _NVR
	video_SetCSC(VO_CVBS_DEVICE, 50, 55, 50, 49);
#endif
#elif defined(SDK_PLATFORM_HI3521)
#if   defined(_HI3515A_EXT) || defined(_HI3520D_EXT)
	video_SetCSC(VO_VGA_DEVICE,  45, 55, 50, 55);
	video_SetVGACSC(VO_VGA_DEVICE, 50, 55, 50, 55, 15);
	video_SetCSC(VO_CVBS_DEVICE, 50, 50, 50, 55);
	video_SetVGACSC(VO_CVBS_DEVICE, 50, 50, 50, 50, 10);
#else
	video_SetCSC(VO_VGA_DEVICE,  45, 60, 50, 52);
	video_SetVGACSC(VO_VGA_DEVICE, 45, 52, 50, 45, 28);
	video_SetCSC(VO_CVBS_DEVICE, 50, 45, 50, 50);
	video_SetVGACSC(VO_CVBS_DEVICE, 50, 50, 50, 45, 10);
#endif
#elif defined(SDK_PLATFORM_HI3531)
	video_SetCSC(VO_VGA_DEVICE,  65, 60, 50, 50);
	video_SetVGACSC(VO_VGA_DEVICE, 45, 48, 50, 50, 13);
	video_SetCSC(VO_CVBS_DEVICE, 50, 45, 50, 50);
	video_SetVGACSC(VO_CVBS_DEVICE, 50, 50, 50, 45, 10);
#endif
}

#ifdef _NVR
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
void __CSC_SET(int nLuma, int nContr, int nHue, int nSatu)
{
	video_SetCSC(VO_VGA_DEVICE, nLuma, nContr, nHue, nSatu);
}
void __CSC_GET(int* nLuma, int* nContr, int* nHue, int* nSatu)
{
	int nDev = VO_VGA_DEVICE;
	VO_CSC_S stVoCSC = {0};
	if(nLuma != NULL){
		stVoCSC.enCSCType = VO_CSC_LUMA;
		DVR_ASSERT(HI_MPI_VO_GetDevCSC(nDev, &stVoCSC));
		*nLuma = stVoCSC.u32Value;
	}
	if(nContr != NULL){
		stVoCSC.enCSCType = VO_CSC_CONTR;
		DVR_ASSERT(HI_MPI_VO_GetDevCSC(nDev, &stVoCSC));
		*nContr = stVoCSC.u32Value;
	}
	if(nHue != NULL){
		stVoCSC.enCSCType = VO_CSC_HUE;
		DVR_ASSERT(HI_MPI_VO_GetDevCSC(nDev, &stVoCSC));
		*nHue = stVoCSC.u32Value;
	}
	if(nSatu != NULL){
		stVoCSC.enCSCType = VO_CSC_SATU;
		DVR_ASSERT(HI_MPI_VO_GetDevCSC(nDev, &stVoCSC));
		*nSatu = stVoCSC.u32Value;
	}
}
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
void __CSC_SET(int nLuma, int nContr, int nHue, int nSatu)
{

}
void __CSC_GET(int* nLuma, int* nContr, int* nHue, int* nSatu)
{

}
#endif
#endif

int video_StandardInit(void)
{
	// init adc standard pal / ntsc
	int fd = open(VIDEO_DEV, O_RDWR);
	if(fd > 0){
		video_std_t stStd;
		strcpy(stStd.std, (VIDEO_IS_PAL() ? "pal" : "ntsc"));
		strcpy(stStd.siz, (VIDEO_IS_960() ? "960h" : "720h"));
		ioctl(fd, VIDEO_DRV_SET_STANDARD, &stStd);
		close(fd);
		return 0;
	}
	return -1;
}

static VIDEO_FRAME_INFO_S* s_pstLossPic = NULL;
#ifdef _NVR
static VIDEO_FRAME_INFO_S* s_pstDisablePic = NULL;
#endif
static VIDEO_FRAME_INFO_S* s_pstLogoPic = NULL;
static VIDEO_FRAME_INFO_S* video_LoadUserPic(const char* szPath, int nWidth, int nHeight)
{
	int i = 0;
	unsigned int const nImageWidth = nWidth;
	unsigned int const nImageHeight = nHeight;
	unsigned int const nLumaSize = (nImageWidth * nImageHeight);
	unsigned int const nChrmSize = nLumaSize >> 2;
	unsigned int const nSize = nLumaSize + (nChrmSize << 1);
	VB_BLK VbBlk;
	unsigned int u32PhyAddr;
	unsigned char* pVirAddr;
	unsigned char* pDst = NULL;
	unsigned char* pU = NULL;
	unsigned char* pV = NULL;
	unsigned char* PTMPU = NULL;
	unsigned char* PTMPV = NULL;
	unsigned char* pTmpU = NULL;
	unsigned char* pTmpV = NULL;
	unsigned int nUVSize = 0;
	FILE* fp = NULL;
	VIDEO_FRAME_INFO_S* pstUserFrame = (VIDEO_FRAME_INFO_S*)(calloc(sizeof(VIDEO_FRAME_INFO_S), 1));

	VbBlk = HI_MPI_VB_GetBlock(VB_INVALID_POOLID, nSize);
	assert(VB_INVALID_HANDLE != VbBlk);
	u32PhyAddr = HI_MPI_VB_Handle2PhysAddr(VbBlk);
	assert(u32PhyAddr > 0);
	pVirAddr = (unsigned char*)(HI_MPI_SYS_Mmap(u32PhyAddr, nSize));
	assert(pVirAddr);
	pstUserFrame->u32PoolId = HI_MPI_VB_Handle2PoolId(VbBlk);
	assert(VB_INVALID_POOLID != pstUserFrame->u32PoolId);
	pstUserFrame->stVFrame.u32PhyAddr[0] = u32PhyAddr;
	pstUserFrame->stVFrame.u32PhyAddr[1] = pstUserFrame->stVFrame.u32PhyAddr[0] + nLumaSize;
	pstUserFrame->stVFrame.u32PhyAddr[2] = pstUserFrame->stVFrame.u32PhyAddr[1] + nChrmSize;

	pstUserFrame->stVFrame.pVirAddr[0] = pVirAddr;
	pstUserFrame->stVFrame.pVirAddr[1] = pstUserFrame->stVFrame.pVirAddr[0] + nLumaSize;
	pstUserFrame->stVFrame.pVirAddr[2] = pstUserFrame->stVFrame.pVirAddr[1] + nChrmSize;

	pstUserFrame->stVFrame.u32Width  = nImageWidth;
	pstUserFrame->stVFrame.u32Height = nImageHeight;
	pstUserFrame->stVFrame.u32Stride[0] = nImageWidth;
	pstUserFrame->stVFrame.u32Stride[1] = nImageWidth;
	pstUserFrame->stVFrame.u32Stride[2] = nImageWidth;
	pstUserFrame->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	pstUserFrame->stVFrame.u32Field = VIDEO_FIELD_FRAME;
	pstUserFrame->stVFrame.u64pts = 0;
	pstUserFrame->stVFrame.u32TimeRef = 0;

	// read Y U V data from file to the addr
	fp = fopen(szPath, "rb");
	assert(fp > 0);

	pDst = pstUserFrame->stVFrame.pVirAddr[0];
	for(i = 0; i < nImageHeight; ++i){
		fread(pDst, nImageWidth, 1, fp);
		pDst += nImageWidth;
	}
	pDst = pstUserFrame->stVFrame.pVirAddr[1];
	for(i = 0; i < (nImageHeight/2); ++i){
		fread(pDst, nImageWidth/2, 1, fp);
		pDst += nImageWidth/2;
	}
	pDst = pstUserFrame->stVFrame.pVirAddr[2];
	for(i = 0; i < (nImageHeight/2); ++i){
		fread(pDst, nImageWidth/2, 1, fp);
		pDst += nImageWidth/2;
	}
	fclose(fp);
	fp = NULL;

	// read Y U V data from file to the addr
	nUVSize = (nImageHeight * pstUserFrame->stVFrame.u32Stride[1]) >> 2;
	pTmpU = (unsigned char*)(calloc(1, nUVSize)); PTMPU = pTmpU;
	pTmpV = (unsigned char*)(calloc(1, nUVSize)); PTMPV = pTmpV;
	pU = pstUserFrame->stVFrame.pVirAddr[1];
	pV = pstUserFrame->stVFrame.pVirAddr[2];
	memcpy(pTmpU, pU, nUVSize);
	memcpy(pTmpV, pV, nUVSize);

	for(i = 0; i < nUVSize / 2; ++i){
		*pU++ = *pTmpV++;
		*pU++ = *pTmpU++;
	}

	for(i = 0; i < nUVSize / 2; ++i){
		*pV++ = *pTmpV++;
		*pV++ = *pTmpU++;
	}

	free(PTMPU);
	free(PTMPV);
	HI_MPI_SYS_Munmap(pVirAddr, nSize);
	return pstUserFrame;
}

static void video_UserPicInit()
{
#ifdef _NVR
	s_pstDisablePic = video_LoadUserPic("skin/disable.yuv", 352, 240);
	s_pstLossPic = video_LoadUserPic("skin/reconnect.yuv", 352, 240);
	s_pstLogoPic = video_LoadUserPic("skin/disable.yuv", 352, 240);
#else
	s_pstLossPic = video_LoadUserPic("skin/loss.yuv", 704, 480);
	s_pstLogoPic = video_LoadUserPic("skin/logo.yuv", 704, 480);
#endif
	{
		VI_USERPIC_ATTR_S tmpUserLossPic;

		tmpUserLossPic.bPub = HI_TRUE;
		tmpUserLossPic.enUsrPicMode = VI_USERPIC_MODE_PIC;
		tmpUserLossPic.unUsrPic.stUsrPicFrm = *s_pstLossPic;

		DVR_ASSERT(HI_MPI_VI_SetUserPic(0, &tmpUserLossPic));
	}
}

void* VIDEO_GetUserPic(int _chn)
{
#ifdef _NVR
	ENV_BOOLEAN enable = g_pstSysEnv->IsEnableIPCChannel(g_pstSysEnv, _chn);
	if(enable == ENV_TRUE)
	{
		return s_pstLossPic;
	}
	else
	{
		return s_pstDisablePic;
	}
#else
	return s_pstLossPic;
#endif
}

void VIDEO_SetBg(int nLayer, unsigned char u8RR, unsigned char u8GG, unsigned char u8BB)
{
}

int VIDEO_GetMaxGrid()
{
	static int s_nMaxGrid = -1;
	if(-1 == s_nMaxGrid){
		int i = 0;
		for(i = 1;; ++i){
			if(MAX_CAM_CH <= i*i){
				s_nMaxGrid = i;
				break;
			}
		}
	}
	return s_nMaxGrid;
}

int VIDEO_GetMaxDiv()
{
	static int s_nMaxDiv = -1;
	if(-1 == s_nMaxDiv){
		s_nMaxDiv = pow(VIDEO_GetMaxGrid(), 2);
	}
	return s_nMaxDiv;
}

static void video_BindOutput(int nDev, int nChn, unsigned char bBind)
{
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
	int nViDev = 0;
	int nViChn = 0;
	if(0 == VIMAP_Get(nChn, &nViDev, &nViChn)){
		if(bBind){
			DVR_ASSERT(HI_MPI_VI_BindOutput(nViDev, nViChn, nDev, nChn));
		}else{
			DVR_ASSERT(HI_MPI_VI_UnBindOutput(nViDev, nViChn, nDev, nChn));
		}
	}
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
	{
		HI_S32 tmpVpssChn = VPSS_PRE0_CHN;
#if defined(FORCE_NORMAL_CVBS_OUTPUT)
		if(VO_CVBS_DEVICE == nDev) {
			tmpVpssChn = VPSS_PRE1_CHN;
		}
#endif
#ifdef _NVR
		(HI_MPI_Sys_BindByForce(HI_ID_VPSS, nChn+MAX_CAM_CH, 0,
								HI_ID_VDEC, 0, nChn));
		(HI_MPI_Sys_BindByForce(HI_ID_VOU, nDev, nChn,
								HI_ID_VPSS, nChn+MAX_CAM_CH, tmpVpssChn));
#else
	if(s_stVideo.bLiveMode) {
		(HI_MPI_Sys_BindByForce(HI_ID_VOU, nDev, nChn,
								HI_ID_VPSS, nChn, tmpVpssChn));
		if(1 == s_stVideo.nCurDivi && nChn == s_stVideo.nCurPage) {
			int tmpVpss = VIDEO_GetMaxDiv()+MAX_CAM_CH;
			(HI_MPI_Sys_BindByForce(HI_ID_VOU, nDev, nChn,
									HI_ID_VPSS, tmpVpss, tmpVpssChn));
		}
	}
	else {
		(HI_MPI_Sys_BindByForce(HI_ID_VOU, nDev, nChn,
								HI_ID_VPSS, nChn+MAX_CAM_CH, tmpVpssChn));
	}
#endif
	}
#endif
}

#ifdef _NVR
// nvr solution
static void video_LossHock(int nChn, unsigned char bLoss)
{
	printf("%d, nChn=%d, bLoss=%d", __LINE__, nChn, bLoss);
	NVRPC_LiveDecode(nChn, !bLoss);
	int i = 0;
	for(i = 0; i < VO_DEV_CNT; ++i){
		if(bLoss){
			// close video output and send a user picture to output
//			DVR_ASSERT(HI_MPI_VO_ClearChnBuffer(VO_DEV_ID[i], nChn, HI_TRUE));
			DVR_ASSERT(HI_MPI_VO_SENDFRAME(VO_DEV_ID[i], nChn, VIDEO_GetUserPic(nChn)));
		}
	}
}



static int video_SetColor(int nChn, int nHue, int nBrightness, int nContrast, int nSaturation)
{
	if(nChn < MAX_CAM_CH)
	{
//		printf("%s nHue=%d, nBrightness=%d, nContrast=%d, nSaturation=%d\n",
//				__FUNCTION__, nHue, nBrightness, nContrast, nSaturation);
		//note:old format for hangang "-hue=200"
		//     new format compatible hanggang and ja3518 "-hue=200,50/65"
		char cmd[256] = {0};
		int tmp;
		strcat(cmd, "/cgi-bin/hi3510/param.cgi?cmd=setimageattr");
		if(nHue != -1)
		{
			tmp = nHue;
			if(nHue == 64)
			{
				nHue = 63;
			}
			nHue *= 4;
			nHue %= 256;
			sprintf(cmd + strlen(cmd), "&-hue=%d%%2c%d%%2f65", nHue, tmp);
		}
		if(nBrightness != -1)
		{
			tmp = nBrightness;
			if(nBrightness == 64)
			{
				nBrightness = 63;
			}
			nBrightness *= 4;
			nBrightness %= 256;
			sprintf(cmd + strlen(cmd), "&-brightness=%d%%2c%d%%2f65", nBrightness, tmp);
		}
		if(nSaturation != -1)
		{
			tmp = nSaturation;
			if(nSaturation == 64)
			{
				nSaturation = 63;
			}
			nSaturation *= 4;
			nSaturation %= 256;
			sprintf(cmd + strlen(cmd), "&-saturation=%d%%2c%d%%2f65", nSaturation, tmp);
		}
		if(nContrast != -1)
		{
			tmp = nContrast;
			if(nContrast == 64)
			{
				nContrast = 63;
			}
			nContrast ++;
			nContrast /= 8;
			nContrast %= 8;
			sprintf(cmd + strlen(cmd), "&-contrast=%d%%2c%d%%2f65", nContrast, tmp);
		}

//		printf("cmd=%s\n", cmd);
		__HG_send_cmd(cmd, nChn);
	}
	return 0;
}

#else
// dvr solution

static void video_LossHock(int nChn, unsigned char bLoss)
{
	int i = 0;
	for(i = 0; i < VO_DEV_CNT; ++i){
		if(bLoss){
			video_BindOutput(VO_DEV_ID[i], nChn, FALSE);
			DVR_ASSERT(HI_MPI_VO_ClearChnBuffer(VO_DEV_ID[i], nChn, HI_TRUE));
			DVR_ASSERT(HI_MPI_VO_SENDFRAME(VO_DEV_ID[i], nChn, VIDEO_GetUserPic(nChn)));
		}else{
			video_BindOutput(VO_DEV_ID[i], nChn, TRUE);
		}
	}
}

static unsigned int video_GetLoss(void)
{
	int nRet = 0;
	int fd = 0;
	unsigned int nLossStat = 0;
	video_loss_t stVideoLoss = {0};
	fd = open(VIDEO_DEV, O_RDWR);
	if(fd < 0){
		VIDEO_TRACE("open "VIDEO_DEV" error->%s", strerror(errno));
		return 0;
	}
	nRet = ioctl(fd, VIDEO_DRV_DET_LOSS, &stVideoLoss);
	assert(0 == nRet);
	close(fd);
	return stVideoLoss.loss;
}

static int video_SetColor(int nChn, int nHue, int nBrightness, int nContrast, int nSaturation)
{
	if(nChn < MAX_CAM_CH){
		int nFileD = -1;
		int nItem = 0;
		nFileD = open(VIDEO_DEV, O_RDWR);
		if(nFileD < 0){
			VIDEO_TRACE("open "VIDEO_DEV" error->%s", strerror(errno));
			return -1;
		}
		if(nHue >= 0 && nHue <= VIDEO_MAX_COLOR_GAIN){
			nItem |= VIDEO_COLOR_HUE;
			g_pstSysEnv->SetHue(g_pstSysEnv, nChn, nHue);
		}
		if(nBrightness >= 0 && nBrightness <= VIDEO_MAX_COLOR_GAIN){
			nItem |= VIDEO_COLOR_BRIGHTNESS;
			g_pstSysEnv->SetBrightness(g_pstSysEnv, nChn, nBrightness);
		}
		if(nContrast >= 0 && nContrast <= VIDEO_MAX_COLOR_GAIN){
			nItem |= VIDEO_COLOR_CONTRAST;
			g_pstSysEnv->SetContrast(g_pstSysEnv, nChn, nContrast);
		}
		if(nSaturation >= 0 && nSaturation <= VIDEO_MAX_COLOR_GAIN){
			nItem |= VIDEO_COLOR_SATURATION;
			g_pstSysEnv->SetSaturation(g_pstSysEnv, nChn, nSaturation);
		}
		if(nItem){
#if  defined(_EXT_HDDVR)
			HDDVR_VideoSetColor(nChn, nHue, nBrightness, nContrast, nSaturation, -1);
#else
			VIDEODRV_SetColor(nFileD, nChn, nHue, nBrightness, nContrast, nSaturation);
#endif
		}
		close(nFileD);
		nFileD = -1;
	}
	return 0;
}

#endif

#ifdef _NVR
	#include "video/video_nvr.c"
#elif defined(_EXT_HDDVR)
	#include "video/video_ja2104hd.c"
#else
	#if defined(_R2004) || defined(_R2008) \
	|| defined(_JA7204) || defined(_JA7204S) || defined(_JA7204I) || defined(_JA7204E) \
	|| defined(_JA7208) || defined(_JA7208S) || defined(_JA7208I)
	#include "video/video_ja7204_ja7208.c"
	#elif defined(_JA7216CX) || defined(_JA7216ICX)
	#include "video/video_ja7216cx.c"
	#elif defined(_JA7216NC) || defined(_JA7216INC)
	#include "video/video_ja7216nc.c"
	#elif defined(_JA2008NC)
	#include "video/video_ja2008nc.c"
	#elif defined(_JA2016NC)
	#define VI_X_OFFSET 0
	#include "video/video_ja2016nc.c"
	#elif defined(_JA2016CX) || defined(_JA2016ICX)
	#define VI_X_OFFSET 16
	#include "video/video_ja2016nc.c"
	#elif defined(_JA2024NC)
	#include "video/video_ja2024nc.c"
	#elif defined(_JA2032NC)
	#include "video/video_ja2032nc.c"
	#elif defined(_JA9204RN)
	#include "video/video_ja9204rn.c"
	#elif defined(_JA2104) || defined(_JA2106) || defined(_JA2108) || defined(_JA2116) || defined(_JA2116D) || defined(_JA3116TW)
	#include "video/video_ja3116tw.c"
	#elif defined(_JA2124) || defined(_JA2132)
	#include "video/video_ja2132.c"
	#else
	#error "Unknown Device!"
	#endif
#endif

static void video_PrintLoss(unsigned int nLossStat)
{
//	return;
	int dd = 0;
	printf("video loss: ");
	for(dd = 0; dd < MAX_CAM_CH; ++dd){
		if(nLossStat & (1<<dd)){printf("o");}else{printf("x");}
	}
	printf("\r\n");
}

static void video_UpdateFullScreen(int nChn)
{
	int i = 0;
	if(1 == s_stVideo.nCurDivi && nChn == s_stVideo.nCurPage){
		for(i = 0; i < VO_DEV_CNT; ++i){
			video_DisplayD1(VO_DEV_ID[i], nChn, TRUE);
		}
	}
}

static void video_DetectLoss(void)
{
	int i = 0;
	unsigned int nLossStat = 0;
#if defined(_EXT_HDDVR)
	nLossStat = HDDVR_VideoLoss();
#else
	nLossStat = video_GetLoss();
#endif
	if(nLossStat != s_stVideo.nLossStat){
		unsigned int const nLossDiff = (nLossStat ^ s_stVideo.nLossStat); // detect status changed
		video_PrintLoss(nLossStat);
		for(i = 0; i < MAX_CAM_CH; ++i){
			if(nLossDiff & (1<<i)){
				if(s_stVideo.bLiveMode){
					video_LossHock(i, (nLossStat & (1<<i)) ? TRUE : FALSE);
					video_UpdateFullScreen(i);
				}
#if defined(_EXT_HDDVR)
				if(s_stVideo.nLossStat & (1<<i)){
					video_InputAutoFit(i);
				}
#endif
			}
		}
		// update the state
		s_stVideo.nLossStat = nLossStat;
	}
	for(i = 0; i < MAX_CAM_CH; ++i) {
		HDDVR_VideoLossFix(i, s_stVideo.nLossStat & (1 << i));
		HDDVR_VideoStVideoFix(i);
	}
}

typedef struct {
	pthread_t Tid[MAX_CAM_CH];
	int       TRunFlag[MAX_CAM_CH];
	pthread_mutex_t TLocker;
} VideoLossDetect_Cxt;

VideoLossDetect_Cxt gVLossCxt;

void* VideoLossDetect_Proc(void* arg)
{
	int Chnl = *((int *)arg);
	int LossCur = HDDVR_VideoLossChnl(Chnl);
	int LossLst = LossCur;
	free(arg);

	gVLossCxt.TRunFlag[Chnl] = 1;
	while(gVLossCxt.TRunFlag[Chnl]) {
		LossCur = HDDVR_VideoLossChnl(Chnl);

		if(LossCur != LossLst) {
			if(s_stVideo.bLiveMode) {
				video_LossHock(Chnl, LossCur ? TRUE : FALSE);
				video_UpdateFullScreen(Chnl);
			}
		}

		if(Chnl == 0 && VideoModeSwitchAutoCheck()) { //Only One Test Thread
			VideoModeSwitchAuto();
		}

		video_InputAutoFit(Chnl);

		FixViChnFpsByForce(Chnl);
		FixVencPicByViChnInfo(Chnl, 0);
		FixVencPicByViChnInfo(Chnl, 1);

		LossLst = LossCur;

		{
			int tmpRealChnl;

			tmpRealChnl = VIMAP_Get(Chnl);
			if(tmpRealChnl >= 0) {
				if(LossCur) {
					DVR_ASSERT(HI_MPI_VI_EnableUserPic(tmpRealChnl*nViChnStp));
				}
				else {
					DVR_ASSERT(HI_MPI_VI_DisableUserPic(tmpRealChnl*nViChnStp));
				}
			}
		}

		if(gVLossCxt.TRunFlag[Chnl]) {
			HDDVR_VideoLossFix(Chnl, LossCur);
			HDDVR_VideoStVideoFix(Chnl);
		}

		//printf("\nHDDVR_GetCHNLType : %s\n", HDDVR_GetCHNLType(Chnl));

		pthread_mutex_lock(&(gVLossCxt.TLocker));
		s_stVideo.nLossStat &= ~(1 << Chnl);
		s_stVideo.nLossStat |= (LossCur << Chnl);
		pthread_mutex_unlock(&(gVLossCxt.TLocker));

		usleep(400000);
	}

	pthread_exit(NULL);
}

int VideoLossDetect_Init()
{
	int ii;

	memset(&gVLossCxt, 0, sizeof(gVLossCxt));

	pthread_mutex_init(&(gVLossCxt.TLocker), NULL);

	for(ii = 0; ii < MAX_CAM_CH; ii ++) {
		int * tmpChnl = (int *)malloc(sizeof(*tmpChnl));
		*tmpChnl = ii;
		pthread_create(&gVLossCxt.Tid[ii], NULL, VideoLossDetect_Proc, tmpChnl);
	}
}

int VideoLossDetect_Exit()
{
	int ii;

	for(ii = 0; ii < MAX_CAM_CH; ii ++) {
		gVLossCxt.TRunFlag[ii] = 0;
	}

	pthread_mutex_destroy(&(gVLossCxt.TLocker));
}

int VideoLossGetStat(void)
{
	unsigned int tmpLossStat;

	pthread_mutex_lock(&(gVLossCxt.TLocker));
	tmpLossStat = s_stVideo.nLossStat;
	pthread_mutex_unlock(&(gVLossCxt.TLocker));

	return tmpLossStat;
}

void VIDEO_Init()
{
#if   defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
	video_ParamInit();
#endif
	s_stVideo.bLiveMode = TRUE;

#ifdef _NVR
//fixme:the s_stVideo.nScrWidth[0] and s_stVideo.nScrHeight[0] setting at file video_nvr.c, function video_OutputInit
	s_stVideo.nScrWidth[0]  = 0;
	s_stVideo.nScrHeight[0] = 0;
	s_stVideo.nScrWidth[1]  = VRES_SIZE_W_SD;
	s_stVideo.nScrHeight[1] = VIDEO_IS_PAL() ?VRES_SIZE_H_PL :VRES_SIZE_H_NT;
#elif defined(_EXT_HDDVR)
	s_stVideo.nScrWidth[0]  = VRES_SIZE_W_HD720P;
	s_stVideo.nScrHeight[0] = VRES_SIZE_H_HD720P;
	s_stVideo.nScrWidth[1]  = VRES_SIZE_W_SD;
	s_stVideo.nScrHeight[1] = VIDEO_IS_PAL() ?VRES_SIZE_H_PL :VRES_SIZE_H_NT;
#else
	s_stVideo.nScrWidth[0]  = VIDEO_IS_960() ?VRES_SIZE_W_EF :VRES_SIZE_W_SD;
	s_stVideo.nScrWidth[1]  = VRES_SIZE_W_SD;
	s_stVideo.nScrHeight[0] = VIDEO_IS_PAL() ?VRES_SIZE_H_PL :VRES_SIZE_H_NT;
	s_stVideo.nScrHeight[1] = VIDEO_IS_PAL() ?VRES_SIZE_H_PL :VRES_SIZE_H_NT;
#endif
#if   defined(FORCE_NORMAL_CVBS_OUTPUT) \
		|| defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
	video_TvAdjust(
		g_pstSysEnv->GetTvSize(g_pstSysEnv, TV_SIZE_LEFT),
		g_pstSysEnv->GetTvSize(g_pstSysEnv, TV_SIZE_TOP),
		g_pstSysEnv->GetTvSize(g_pstSysEnv, TV_SIZE_RIGHT),
		g_pstSysEnv->GetTvSize(g_pstSysEnv, TV_SIZE_BOTTOM),
		FALSE);
#endif
	video_UserPicInit();
	video_InputInit();
	video_OutputInit();
	video_CSCInit();
#if   !defined(FORCE_NORMAL_CVBS_OUTPUT) \
	&& (defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531))
	video_WbcAdjust(
		g_pstSysEnv->GetTvSize(g_pstSysEnv, TV_SIZE_LEFT),
		g_pstSysEnv->GetTvSize(g_pstSysEnv, TV_SIZE_TOP),
		g_pstSysEnv->GetTvSize(g_pstSysEnv, TV_SIZE_RIGHT),
		g_pstSysEnv->GetTvSize(g_pstSysEnv, TV_SIZE_BOTTOM));
#endif
	VideoLossDetect_Init();
}

void VIDEO_Destroy()
{
//	VIDEO_PowerOn();
	VideoLossDetect_Exit();
	video_InputDestroy();
	video_OutputDestroy();
#if   defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
	video_ParamExit();
#endif
}

unsigned char VIDEO_IsLoss(int nChn)
{
	if(nChn < MAX_CAM_CH){
		return (VideoLossGetStat() & (1<<nChn)) ? TRUE : FALSE;
	}
	return FALSE;
}

static void video_EnableLiveChn(int nChn)
{
	if(nChn < MAX_CAM_CH){
		video_LossHock(nChn, VIDEO_IsLoss(nChn));
	}
}

static void video_DisableLiveChn(int nChn)
{
	if(nChn < MAX_CAM_CH){
		video_LossHock(nChn, TRUE);
	}
}

void VIDEO_SetLiveMode()
{
	int i = 0;
	s_stVideo.bLiveMode = TRUE;
	for(i = 0; i < MAX_CAM_CH; ++i){
		video_EnableLiveChn(i);
		VIDEO_ClearDigitalZoom(i);
	}
}

void VIDEO_SetPlaybackMode()
{
	int i = 0;
	s_stVideo.bLiveMode = FALSE;
	for(i = 0; i < MAX_CAM_CH; ++i){
		// must to set to lost
		video_DisableLiveChn(i);
		VIDEO_ClearDigitalZoom(i);
	}
}

unsigned char VIDEO_IsLiveMode()
{
	return s_stVideo.bLiveMode;
}

int VIDEO_Switch(int nDiv, int nPage)
{
	int i = 0, ii = 0;;
//	if(nDiv == s_stVideo.nCurDivi && nPage == s_stVideo.nCurPage){
//		// needless to switch
//		return -1;
//	}
	// video switch
	for(i = 0; i < VO_DEV_CNT; ++i){
		DVR_ASSERT(HI_MPI_VO_SetAttrBegin(VO_DEV_ID[i]));
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
		if(1 == nDiv){
			if(s_stVideo.bLiveMode){
				video_DisplayD1(VO_DEV_ID[i], nPage, TRUE);
			}
			video_Division(VO_DEV_ID[i], s_stVideo.nScrWidth[i], s_stVideo.nScrHeight[i], nDiv, nPage, s_stVideo.bLiveMode);
		}else{

			video_Division(VO_DEV_ID[i], s_stVideo.nScrWidth[i], s_stVideo.nScrHeight[i], nDiv, nPage, s_stVideo.bLiveMode);
			video_DisplayD1(VO_DEV_ID[i], s_stVideo.nCurPage, FALSE);
		}
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
		if(1 == nDiv){
			if(s_stVideo.bLiveMode){
				video_DisplayD1(VO_DEV_ID[i], nPage, TRUE);
			}
			video_Division(VO_DEV_ID[i], s_stVideo.nScrWidth[i], s_stVideo.nScrHeight[i], nDiv, nPage, s_stVideo.bLiveMode);
		}else{

			video_Division(VO_DEV_ID[i], s_stVideo.nScrWidth[i], s_stVideo.nScrHeight[i], nDiv, nPage, s_stVideo.bLiveMode);
			video_DisplayD1(VO_DEV_ID[i], s_stVideo.nCurPage, FALSE);
		}
#endif
		DVR_ASSERT(HI_MPI_VO_SetAttrEnd(VO_DEV_ID[i]));
	}

	//Refresh Global nCurDivi & nCurPage
	s_stVideo.nCurDivi = nDiv;
	s_stVideo.nCurPage = nPage;

	// update the background picture
	if(s_stVideo.bLiveMode){
		for(ii = 0; ii < MAX_CAM_CH; ++ii){
			video_LossHock(ii, VIDEO_IsLoss(ii));
		}
	}
	for(i = 0; i < VO_DEV_CNT; ++i){
		for(ii = MAX_CAM_CH; ii < VIDEO_GetMaxDiv() && ii < VO_MAX_CHN_NUM; ++ii){
			DVR_ASSERT(HI_MPI_VO_SENDFRAME(VO_DEV_ID[i], ii, s_pstLogoPic));
		}
	}

	// audio switch
	AUDIO_Switch(nDiv * nPage);

//	return OSD_Switch(s_stVideo.nCurDivi, s_stVideo.nCurPage);
	return 0;
}

int VIDEO_SwitchFull(int nPage)
{
	return VIDEO_Switch(1, nPage);
}

int VIDEO_SwitchQuad(int nPage)
{
	return VIDEO_Switch(4, nPage);
}

int VIDEO_SwitchX9(int nPage)
{
	return VIDEO_Switch(9, nPage);
}

int VIDEO_SwitchX16(int nPage)
{
	return VIDEO_Switch(16, nPage);
}

int VIDEO_SwitchDefault()
{
	return VIDEO_Switch(VIDEO_GetMaxDiv(), 0);
}

int VIDEO_GetCurDivi()
{
	return s_stVideo.nCurDivi;
}

int VIDEO_GetCurPage()
{
	return s_stVideo.nCurPage;
}

// nValue: 0 - 16, other : ignor
int VIDEO_SetColor(int nChn, int nHue, int nBrightness, int nContrast, int nSaturation)
{
	return video_SetColor(nChn, nHue, nBrightness, nContrast, nSaturation);
}

// nValue: 0 - 15
int VIDEO_SetHue(int nChn, int nValue)
{
	return VIDEO_SetColor(nChn, nValue, -1, -1, -1);
}

int VIDEO_SetBrightness(int nChn, int nValue)
{
	return VIDEO_SetColor(nChn, -1, nValue, -1, -1);
}

int VIDEO_SetContrast(int nChn, int nValue)
{
	return VIDEO_SetColor(nChn, -1, -1, nValue, -1);
}

int VIDEO_SetSaturation(int nChn, int nValue)
{
	return VIDEO_SetColor(nChn, -1, -1, -1, nValue);
}


void video_AutoSwitchTask(void)
{
	return; // auto swithing active @ gui
	int nPage = 0;
	switch(s_stVideo.nAutoSwitch)
	{
	case 1:
		if(s_stVideo.nCurDivi != 1){
			VIDEO_SwitchFull(0);
		}else{
			nPage = s_stVideo.nCurPage + 1;
			nPage %= MAX_CAM_CH;
			VIDEO_SwitchFull(nPage);
		}
		VIDEO_TRACE("%s switch %d", ASCTIME(), nPage);
		break;

#if (MAX_CAM_CH >= 8)
	case 4:
		if(s_stVideo.nCurDivi != 4){
			VIDEO_SwitchQuad(0);
		}else{
			nPage = s_stVideo.nCurPage + 1;
			nPage %= (MAX_CAM_CH/4);
			VIDEO_SwitchQuad(nPage);

		}
		VIDEO_TRACE("%s switch %d", ASCTIME(), nPage);
		break;
#endif

	default:
		break;
	}
}

int VIDEO_StartAutoSwitch(int nDivi, int nTickSec)
{
	s_stVideo.nAutoSwitch = nDivi;
	return TIMETICK_AddTask(video_AutoSwitchTask, nTickSec);
}

void VIDEO_StopAutoSwitch()
{
	TIMETICK_RemoveTask(video_AutoSwitchTask);
}

void VIDEO_ResetAutoSwitch()
{
	VIDEO_StopAutoSwitch();
	if(g_pstSysEnv->IsAutoSwi(g_pstSysEnv)){
		const int TICK_MAP[] =
		{
			[AUTOSWI_2SEC] = 2,
			[AUTOSWI_3SEC] = 3,
			[AUTOSWI_4SEC] = 4,
			[AUTOSWI_5SEC] = 5,
			[AUTOSWI_8SEC] = 8,
			[AUTOSWI_10SEC] = 10,
		};
		const int AUTOSWI_MAP[] =
		{
			[AUTOSWI_X1] = 1,
			[AUTOSWI_X4] = 4,
			[AUTOSWI_X9] = 9,
		};
		int const nTick = TICK_MAP[g_pstSysEnv->GetAutoSwiInterval(g_pstSysEnv)];
		int const nDivi = AUTOSWI_MAP[g_pstSysEnv->GetAutoSwiMode(g_pstSysEnv)];
		VIDEO_StartAutoSwitch(nDivi, nTick);
	}
}

unsigned char VIDEO_IsPower()
{
	return !!s_stVideo.bOutput;
}

void VIDEO_PowerOff()
{
	if(VIDEO_IsPower()){
		video_OutputDestroy();
	}
}

void VIDEO_PowerOn()
{
	if(!VIDEO_IsPower()){
		video_OutputInit();
		video_CSCInit();
		VIDEO_SwitchDefault();
	}
}

void VIDEO_Power()
{
	if(VIDEO_IsPower()){
		VIDEO_PowerOff();
	}else{
		VIDEO_PowerOn();
	}
}

int VIDEO_DigitalZoom(int nChn, int nRatioX, int nRatioY, int nRatioW, int nRatioH)
{
	return video_DigitalZoom(nChn, nRatioX, nRatioY, nRatioW, nRatioH, VIDEO_IsLiveMode());
}

void VIDEO_ClearDigitalZoom(int nChn)
{
	video_ClearDigitalZoom(nChn);
}

void VIDEO_GetTvSize(int* ret_nSrcX, int* ret_nSrcY, int* ret_nWidth, int* ret_nHeight)
{
	if(ret_nSrcX){
		*ret_nSrcX = s_stVideo.ScrnEdges[1].nEdgeX;
	}
	if(ret_nSrcY){
		*ret_nSrcY = s_stVideo.ScrnEdges[1].nEdgeY;
	}
	if(ret_nWidth){
		*ret_nWidth = s_stVideo.ScrnEdges[1].nEdgeW;
	}
	if(ret_nHeight){
		*ret_nHeight = s_stVideo.ScrnEdges[1].nEdgeH;
	}
}

#define ALLIGNMENT_BY(x, y) {(x) = (x) - (x)%(y);}
#define MIN_SIZE_BY(x, y)   {(x) = ((x) < (y)) ?(y) :(x);}

void video_WbcAdjust(int x, int y, int w, int h)
{
	return 0;
	int  nKeepW = VO_ORIGIN_WIDTH  - TVSIZE_SCALAR*TVSIZE_STEPER;
	int  nKeepH = VO_ORIGIN_HEIGHT - TVSIZE_SCALAR*TVSIZE_STEPER;
	
	int  nVoDev = VO_CVBS_DEVICE;

	if(TVSIZE_SCALAR < x || x < 0) x = 0;
	if(TVSIZE_SCALAR < y || y < 0) y = 0;
	if(TVSIZE_SCALAR < w || w < 0) w = TVSIZE_SCALAR;
	if(TVSIZE_SCALAR < h || h < 0) h = TVSIZE_SCALAR;

	VO_CHN_ATTR_S stVoChnAttr;
    memset(&stVoChnAttr, 0, sizeof(stVoChnAttr));

	DVR_ASSERT(HI_MPI_VO_GetChnAttr(nVoDev, 0, &stVoChnAttr));

	stVoChnAttr.stRect.s32X = (x)*TVSIZE_STEPER;
	stVoChnAttr.stRect.s32Y = (y)*TVSIZE_STEPER;
	stVoChnAttr.stRect.u32Width  = nKeepW + (w-x)*TVSIZE_STEPER;
	stVoChnAttr.stRect.u32Height = nKeepH + (h-y)*TVSIZE_STEPER;
	
	ALLIGNMENT_BY(stVoChnAttr.stRect.s32X, 2);
	ALLIGNMENT_BY(stVoChnAttr.stRect.s32Y, 2);
	ALLIGNMENT_BY(stVoChnAttr.stRect.u32Width, 2);
	ALLIGNMENT_BY(stVoChnAttr.stRect.u32Height, 2);

	MIN_SIZE_BY(stVoChnAttr.stRect.u32Width, 16);
	MIN_SIZE_BY(stVoChnAttr.stRect.u32Height, 16);

    s_stVideo.ScrnEdges[1].nEdgeX = stVoChnAttr.stRect.s32X;
    s_stVideo.ScrnEdges[1].nEdgeY = stVoChnAttr.stRect.s32Y;
    s_stVideo.ScrnEdges[1].nEdgeW = stVoChnAttr.stRect.u32Width;
    s_stVideo.ScrnEdges[1].nEdgeH = stVoChnAttr.stRect.u32Height;

	DVR_ASSERT(HI_MPI_VO_SetAttrBegin(nVoDev));
    DVR_ASSERT(HI_MPI_VO_ChnHide(nVoDev, 0));
	DVR_ASSERT(HI_MPI_VO_SetChnAttr(nVoDev, 0, &stVoChnAttr));
	DVR_ASSERT(HI_MPI_VO_ChnShow(nVoDev, 0));
	DVR_ASSERT(HI_MPI_VO_SetAttrEnd(nVoDev));
}

void VIDEO_SetTvSize(int bSync)
{
	if(bSync) {
		if(-1 == s_stVideo.nCurDivi) {
			VIDEO_SwitchDefault();
		}
		else {
			VIDEO_Switch(s_stVideo.nCurDivi, s_stVideo.nCurPage);
		}
	}
}

void video_TvAdjust(int x, int y, int w, int h, int bSync)
{
	int  nKeepW = VO_ORIGIN_WIDTH  - TVSIZE_SCALAR*TVSIZE_STEPER;
	int  nKeepH = VO_ORIGIN_HEIGHT - TVSIZE_SCALAR*TVSIZE_STEPER;
	
	if(TVSIZE_SCALAR < x || x < 0) x = 0;
	if(TVSIZE_SCALAR < y || y < 0) y = 0;
	if(TVSIZE_SCALAR < w || w < 0) w = TVSIZE_SCALAR;
	if(TVSIZE_SCALAR < h || h < 0) h = TVSIZE_SCALAR;

	s_stVideo.ScrnEdges[1].nEdgeX = (x)*TVSIZE_STEPER;
	s_stVideo.ScrnEdges[1].nEdgeY = (y)*TVSIZE_STEPER;
	s_stVideo.ScrnEdges[1].nEdgeW = nKeepW + (w-x)*TVSIZE_STEPER;
	s_stVideo.ScrnEdges[1].nEdgeH = nKeepH + (h-y)*TVSIZE_STEPER;

	ALLIGNMENT_BY(s_stVideo.ScrnEdges[1].nEdgeX, 2);
	ALLIGNMENT_BY(s_stVideo.ScrnEdges[1].nEdgeY, 2);
	ALLIGNMENT_BY(s_stVideo.ScrnEdges[1].nEdgeW, 2);
	ALLIGNMENT_BY(s_stVideo.ScrnEdges[1].nEdgeH, 2);

	MIN_SIZE_BY(s_stVideo.ScrnEdges[1].nEdgeW, 16);
	MIN_SIZE_BY(s_stVideo.ScrnEdges[1].nEdgeH, 16);

	VIDEO_SetTvSize(bSync);
}
