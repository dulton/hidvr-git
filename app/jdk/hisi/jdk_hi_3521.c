

#include "jn_sdk/sdk_pri_api.h"
#include "hddvr_code.h"

#include "bsp/gpio_drv.h"

#if defined(SDK_PLATFORM_HI3515)
#define VO_VGA_DEVICE  (0)
#define VO_CVBS_DEVICE (2)

#elif defined(SDK_PLATFORM_HI3520)
#define VO_VGA_DEVICE  (0)
#define VO_CVBS_DEVICE (2)

#elif defined(SDK_PLATFORM_HI3521)
#define VO_VGA_DEVICE  (0)
#define VO_CVBS_DEVICE (2)

#elif defined(SDK_PLATFORM_HI3531)
#define VO_VGA_DEVICE  (1)
#define VO_CVBS_DEVICE (2)

#else
#error "null vo device"
#endif

#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
#define VO_BG_COLOR (0xf0f0f0) // 24 bits black
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
#define VO_BG_COLOR (0x0)
#endif



static int  nViDevNum = VIU_MAX_DEV_NUM;
static int  nViChnStp = 1;

#if defined(_EXT_HDDVR_LITE)
static JDK_UINT32 VIMAP_TBL_4[4] = { 0, 2, 4, 6, };
#else
static JDK_UINT32 VIMAP_TBL_4[4] = { 4, 8, 0,12, };
#endif

#if defined(_EXT_HDDVR_LITE)
static JDK_UINT32 VIMAP_TBL_8[8] = { 8, 9, 12, 13, 5, 4, 0, 1, };
#else
static JDK_UINT32 VIMAP_TBL_8[8] = { 12, 14, 0, 2, 8, 10, 4, 6,};
#endif

static JDK_UINT32 VIMAP_TBL_16[16] = { 14, 15, 2, 3, 10, 11, 6, 7, 4, 5, 8, 9, 0, 1, 12, 13, };
static JDK_UINT32 vpssImgSizeW[JDK_MAX_CHN];
static JDK_UINT32 vpssImgSizeH[JDK_MAX_CHN];

static JDK_UINT32 vimap_get_real_chn(JDK_UINT32 maxChn, JDK_UINT32 nChn)
{

	jdk_return_val_if_fail(nChn < JDK_MAX_CHN, -1);
	jdk_return_val_if_fail(maxChn <= JDK_MAX_CHN, -1);
	jdk_return_val_if_fail(nChn < maxChn, -1);

	if (maxChn > 16) {
		JDK_TRACE("invalid max-bnc-cam:%u" , maxChn);
		return -1;
	} else if (maxChn > 8) {
		return VIMAP_TBL_16[nChn];
	} else if (maxChn > 4) {
		return VIMAP_TBL_8[nChn];
	} else {
		return VIMAP_TBL_4[nChn];
	}
}

static inline JDK_UINT32 vimap_realchn2dev(JDK_UINT32 nViChn,JDK_VI_MUX_MODE muxMode)
{
	jdk_return_val_if_fail(JDK_IS_VALID_VI_MUX(muxMode),JDK_FALSE);
	JDK_UINT32 maxBncCam = JDK_MAX_BNC_CAM();
	if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3531) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3532)) {
		if (nViChn < 16) {
			return nViChn/2;
		}else {
			if (nViChn > 30)
				return 0;
			else
				return (nViChn - 16)/2;
		}
	} else if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3521) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3520A)
		|| JDK_SOC_CHIP_IS(JDK_HI_SOC_3520D) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3515A) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3515C)) {
		return (nViChn/4);
	} else
		return 0;
}

static inline JDK_UINT32 vimap_dev2chn(JDK_UINT32 nViDev,JDK_VI_MUX_MODE muxMode)
{
	JDK_UINT32 i , chn = -1;
	JDK_UINT32 realChn = (nViDev * 4);
	JDK_UINT32 bncCamNum = JDK_MAX_BNC_CAM();
	JDK_UINT32 *p_vimap = NULL;

	if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3531) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3532)) {
		if(muxMode == JDK_VI_MUX_X1){
			realChn = nViDev*2;
		}else{
			realChn = nViDev * muxMode;
		}
	} else {
		realChn = nViDev * 4;
	}
	
	for (i = 0; i < JDK_BNC_CAM(); i++) {
		if (bncCamNum == 4) {
			p_vimap = VIMAP_TBL_4;
		} else if (bncCamNum == 8) {
			p_vimap = VIMAP_TBL_8;
		} else if (bncCamNum == 16) {
			p_vimap = VIMAP_TBL_16;
		}
		if (p_vimap == NULL) {
			break;
		}
		if (p_vimap[i] == realChn) {
			chn = i;
			break;
		}
	}
	return chn;
}



static JDK_BOOL hi_3521_vimux_set_mask(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr)
{
	const JDK_UINT32 ViMaskTBL_3521[] = {
		0xFF000000, 0xFF0000, 0xFF00, 0xFF,
	};
	const JDK_UINT32 ViMaskTBL_3531[] = {
		0xFF000000, 0xFF0000, 0xFF00, 0xFF,
	};
	JDK_UINT32 vimask[4];
	if (JDK_SOC_BRAND_IS(JDK_SOC_BRAND_HISI)) {
		if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3521) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3520D) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3515A)) {
			memcpy(vimask, ViMaskTBL_3521, sizeof(ViMaskTBL_3521));
		} else if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3531)) {
			memcpy(vimask, ViMaskTBL_3531, sizeof(ViMaskTBL_3531));
		} else {
			return JDK_FALSE;
		}
	} else {
		return JDK_FALSE;
	}

    pstDevAttr->au32CompMask[0] = vimask[ViDev % JDK_ARRAY_SIZE(vimask)];

	return JDK_TRUE;
}


static VI_DEV_ATTR_S DEV_ATTR_BT656_720P_2MUX =
{
    /*接口模式*/
    VI_MODE_BT656,
    /*1、2、4路工作模式*/
    VI_WORK_MODE_2Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},
    /*逐行or隔行输入*/
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1}
};

static VI_DEV_ATTR_S DEV_ATTR_BT656_960H_2MUX =
{
    /*接口模式*/
    VI_MODE_BT656,
    /*1、2、4路工作模式*/
    VI_WORK_MODE_2Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},
    /*逐行or隔行输入*/
    VI_SCAN_INTERLACED,
    /*AdChnId*/
    {-1, -1, -1, -1}
};

#if 0
VI_DEV_ATTR_S DEV_ATTR_7441_INTERLEAVED_720P =
/* 典型时序3:7441 BT1120 720P@60fps典型时序 (对接时序: 时序)*/
{
    /*接口模式*/
    VI_MODE_BT1120_INTERLEAVED,
    /*1、2、4路工作模式*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},
    /*逐行or隔行输入*/
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, 仅支持YUV格式*/
    VI_INPUT_DATA_UVUV,
     
    /*同步信息，对应reg手册的如下配置, --bt1120时序无效*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_NORM_PULSE,VI_VSYNC_VALID_NEG_HIGH,
    
    /*timing信息，对应reg手册的如下配置*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            VRES_SIZE_W_HD720P,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            VRES_SIZE_H_HD720P,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/ 
     0,            0,            0}
    }
};

VI_DEV_ATTR_S DEV_ATTR_7441_INTERLEAVED_1080P =
/* 典型时序3:7441 BT1120 720P@60fps典型时序 (对接时序: 时序)*/
{
    /*接口模式*/
    VI_MODE_BT1120_INTERLEAVED,
    /*1、2、4路工作模式*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},
    /*逐行or隔行输入*/
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, 仅支持YUV格式*/
    VI_INPUT_DATA_UVUV,
     
    /*同步信息，对应reg手册的如下配置, --bt1120时序无效*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_NORM_PULSE,VI_VSYNC_VALID_NEG_HIGH,
    
    /*timing信息，对应reg手册的如下配置*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            VRES_SIZE_W_HD1080P,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            VRES_SIZE_H_HD1080P,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/ 
     0,            0,            0}
    }
};
#endif

JDK_BOOL hi_3521_vpss_group_init(
    int  nVpssGrp,
    int  IsPreView,
    int  SizW, int  SizH,
    VPSS_CROP_INFO_S * Crop,
    PIXEL_FORMAT_E PixelFmt,
    int  NeedDie, int NeedIE, int NeedSp)
{
    {  //Greate VPSS Group;
        VPSS_GRP_ATTR_S stGrpAttr;

        memset(&stGrpAttr, 0, sizeof(stGrpAttr));

        stGrpAttr.u32MaxW   = SizW;
        stGrpAttr.u32MaxH   = SizH;
        stGrpAttr.bDrEn     = HI_FALSE;
        stGrpAttr.bDbEn     = HI_FALSE;
        stGrpAttr.bIeEn     = NeedIE ?HI_TRUE :HI_FALSE;
        stGrpAttr.bNrEn     = NeedIE ?HI_TRUE :HI_FALSE;
        stGrpAttr.bHistEn   = HI_FALSE;
        stGrpAttr.enDieMode = NeedDie
                            ? VPSS_DIE_MODE_AUTO
                            : VPSS_DIE_MODE_NODIE;
        stGrpAttr.enPixFmt  = PixelFmt;

		JDK_TRACE("vpss %d size (%d, %d) pixFmt:%d", nVpssGrp, SizW, SizH, PixelFmt);
        jdk_hi_return_val_if_fail(HI_MPI_VPSS_CreateGrp(nVpssGrp, &stGrpAttr), JDK_FALSE);
    }

    if(Crop) {
        jdk_hi_return_val_if_fail( HI_MPI_VPSS_SetCropCfg(nVpssGrp, Crop), JDK_FALSE);
    }

    {  //Set VPSS Param
        VPSS_GRP_PARAM_S stVpssParam;

        jdk_hi_return_val_if_fail( HI_MPI_VPSS_GetGrpParam(nVpssGrp, &stVpssParam), JDK_FALSE);
        stVpssParam.u32MotionThresh = 0;
        jdk_hi_return_val_if_fail( HI_MPI_VPSS_SetGrpParam(nVpssGrp, &stVpssParam), JDK_FALSE);
    }

    {  //Enable All VPSS Channels
        int ii;
        VPSS_CHN_ATTR_S stChnAttr;

        memset(&stChnAttr, 0, sizeof(stChnAttr));

        for(ii = 0; ii < VPSS_MAX_CHN_NUM; ii ++) {

            stChnAttr.bSpEn    = HI_FALSE;
            stChnAttr.bFrameEn =
               (ii == VPSS_PRE0_CHN) ? (NeedSp ?HI_TRUE :HI_FALSE) : HI_FALSE;
            stChnAttr.stFrame.u32Color[VPSS_FRAME_WORK_LEFT]   = 0xAAAAAA;
            stChnAttr.stFrame.u32Color[VPSS_FRAME_WORK_RIGHT]  = 0xAAAAAA;
            stChnAttr.stFrame.u32Color[VPSS_FRAME_WORK_BOTTOM] = 0xAAAAAA;
            stChnAttr.stFrame.u32Color[VPSS_FRAME_WORK_TOP]    = 0xAAAAAA;
            stChnAttr.stFrame.u32Width[VPSS_FRAME_WORK_LEFT]   = 1 << 1;
            stChnAttr.stFrame.u32Width[VPSS_FRAME_WORK_RIGHT]  = 1 << 1;
            stChnAttr.stFrame.u32Width[VPSS_FRAME_WORK_TOP]    = 1 << 1;
            stChnAttr.stFrame.u32Width[VPSS_FRAME_WORK_BOTTOM] = 1 << 1;

            jdk_hi_return_val_if_fail( HI_MPI_VPSS_SetChnAttr(nVpssGrp, ii, &stChnAttr), JDK_FALSE);
            if(IsPreView) {
				/*
                if(VPSS_PRE1_CHN == ii) 
				{
                    VPSS_CHN_MODE_S tmpChnMode;

                    jdk_hi_return_val_if_fail( HI_MPI_VPSS_GetChnMode(nVpssGrp, ii, &tmpChnMode), JDK_FALSE);
                    tmpChnMode.enChnMode = VPSS_CHN_MODE_USER;
                    tmpChnMode.u32Width  = SizW;
                    tmpChnMode.u32Height = SizH;
                    tmpChnMode.bDouble   = HI_FALSE;
                    tmpChnMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
                    jdk_hi_return_val_if_fail( HI_MPI_VPSS_SetChnMode(nVpssGrp, ii, &tmpChnMode), JDK_FALSE);
                }
                */
            }

            jdk_hi_return_val_if_fail( HI_MPI_VPSS_EnableChn(nVpssGrp, ii), JDK_FALSE);
			if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3521) && 
				(!JDK_SOC_CHIP_IS(JDK_HI_SOC_3515A) && !JDK_SOC_CHIP_IS(JDK_HI_SOC_3520D))) {
	            if(NeedIE) {
	                VPSS_CHN_NR_PARAM_S tmpNrParam;
	                jdk_hi_return_val_if_fail( HI_MPI_VPSS_GetChnNrParam(nVpssGrp, ii, &tmpNrParam), JDK_FALSE);
	                tmpNrParam.u32SfStrength = 24;
	                tmpNrParam.u32TfStrength = 6;
	                jdk_hi_return_val_if_fail( HI_MPI_VPSS_SetChnNrParam(nVpssGrp, ii, &tmpNrParam), JDK_FALSE);
				}
			}
        }
    }

    if(NeedIE) {
        VPSS_GRP_PARAM_S tmpGrpParam;

        jdk_hi_return_val_if_fail( HI_MPI_VPSS_GetGrpParam(nVpssGrp, &tmpGrpParam), JDK_FALSE);
		if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3521)) {
	        tmpGrpParam.u32Contrast   = tmpGrpParam.u32Contrast;
	        tmpGrpParam.u32IeStrength = 0;
	        tmpGrpParam.u32IeSharp    = 16;
	        tmpGrpParam.u32SfStrength = 24;
	        tmpGrpParam.u32TfStrength = 6;
		} else if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3515A) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3520D)) {
	        tmpGrpParam.u32Contrast   = 70;
	        tmpGrpParam.u32IeStrength = 0;
	        tmpGrpParam.u32IeSharp    = 16;
	        tmpGrpParam.u32SfStrength = 24;
	        tmpGrpParam.u32TfStrength = 6;
		}
		if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3531)) {
	        tmpGrpParam.u32Contrast   = tmpGrpParam.u32Contrast;
	        tmpGrpParam.u32IeStrength = 0;
	        tmpGrpParam.u32IeSharp    = tmpGrpParam.u32IeSharp;
	        tmpGrpParam.u32SfStrength = tmpGrpParam.u32SfStrength;
	        tmpGrpParam.u32TfStrength = tmpGrpParam.u32TfStrength;
		}
        jdk_hi_return_val_if_fail( HI_MPI_VPSS_SetGrpParam(nVpssGrp, &tmpGrpParam), JDK_FALSE);
    }

    jdk_hi_return_val_if_fail( HI_MPI_VPSS_StartGrp(nVpssGrp), JDK_FALSE);

	return JDK_TRUE;
}

JDK_BOOL hi_3521_vpss_group_deinit(int  nVpssGrp)
{
	VPSS_GRP_ATTR_S stGrpAttr;
	int ii;
	if (HI_MPI_VPSS_GetGrpAttr(nVpssGrp, &stGrpAttr) == HI_SUCCESS) {
    	jdk_hi_return_val_if_fail( HI_MPI_VPSS_StopGrp(nVpssGrp), JDK_FALSE);

		//Diable All VPSS Channels
        for(ii = 0; ii < VPSS_MAX_CHN_NUM; ii ++) {
            jdk_hi_return_val_if_fail( HI_MPI_VPSS_DisableChn(nVpssGrp, ii), JDK_FALSE);
        }

    	jdk_hi_return_val_if_fail( HI_MPI_VPSS_DestroyGrp(nVpssGrp), JDK_FALSE);
	}

	return JDK_TRUE;
}



static JDK_BOOL hi_3521_sys_init(JDK_STANDARD_MODE standard)
{

	return JDK_TRUE;
}

static JDK_POINTER hi_3521_sys_open_user_frame(const char* szPath, JDK_UINT32 nWidth, JDK_UINT32 nHeight)
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
	jdk_return_val_if_fail(VB_INVALID_HANDLE != VbBlk, NULL);
	u32PhyAddr = HI_MPI_VB_Handle2PhysAddr(VbBlk);
	jdk_return_val_if_fail(u32PhyAddr != 0, JDK_FALSE);
	pVirAddr = (unsigned char*)(HI_MPI_SYS_Mmap(u32PhyAddr, nSize));
	jdk_return_val_if_fail(pVirAddr != NULL, JDK_FALSE);
	pstUserFrame->u32PoolId = HI_MPI_VB_Handle2PoolId(VbBlk);
	jdk_return_val_if_fail(VB_INVALID_POOLID != pstUserFrame->u32PoolId, JDK_FALSE);
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
	jdk_return_val_if_fail(fp != NULL, JDK_FALSE);

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

static JDK_BOOL hi_3521_sys_close_user_frame(JDK_POINTER handler)
{
	return JDK_FALSE;
}


static JDK_BOOL hi_3521_sys_deinit()
{
	return JDK_FALSE;
}

JDK_BOOL hi_3521_vo_wbc_init(JDK_UINT32 w, JDK_UINT32 h, JDK_UINT32 frmRt, JDK_UINT32 pixFormat)
{
	VO_WBC_ATTR_S stWbcAttr;

	memset(&stWbcAttr, 0, sizeof(stWbcAttr));
	stWbcAttr.stTargetSize.u32Width  = w;
	stWbcAttr.stTargetSize.u32Height = h;
	stWbcAttr.u32FrameRate			 = frmRt;
	stWbcAttr.enPixelFormat 		 = pixFormat;	
	if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3520D) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3515A) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3515C)) {
		stWbcAttr.enDataSource	= VO_WBC_DATASOURCE_MIXER;
	}
	else if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3515)) {
		stWbcAttr.enDataSource	= VO_WBC_DATASOURCE_VIDEO;
	}
	jdk_hi_return_val_if_fail(HI_MPI_VO_SetWbcAttr(VO_VGA_DEVICE, &stWbcAttr), JDK_FALSE);
	jdk_hi_return_val_if_fail(HI_MPI_VO_SetWbcMode(VO_VGA_DEVICE, VO_WBC_MODE_PROG_TO_INTL), JDK_FALSE);
#if  defined(_HI3520D_RAM_LITE)
	jdk_hi_return_val_if_fail(HI_MPI_VO_SetWbcDepth(VO_VGA_DEVICE, 1), JDK_FALSE);
#endif
	jdk_hi_return_val_if_fail(HI_MPI_VO_EnableWbc(VO_VGA_DEVICE), JDK_FALSE);
	{
		MPP_CHN_S stSrcChn, stDesChn;

		stSrcChn.enModId   = HI_ID_VOU;
		stSrcChn.s32DevId  = VO_VGA_DEVICE;
		stSrcChn.s32ChnId  = 0;
		stDesChn.enModId   = HI_ID_VOU;
		stDesChn.s32DevId  = VO_CVBS_DEVICE;
		stDesChn.s32ChnId  = 0;
		jdk_hi_return_val_if_fail(HI_MPI_SYS_Bind(&stSrcChn, &stDesChn), JDK_FALSE);
	}

	return JDK_TRUE;
}

JDK_BOOL hi_3521_vo_wbc_deinit(JDK_MONITOR_TYPE monitor)
{
	VO_WBC_ATTR_S stWbcAttr;

	MPP_CHN_S stSrcChn,stDesChn;

	stDesChn.enModId  = HI_ID_VOU;
	stDesChn.s32DevId = VO_CVBS_DEVICE;
	stDesChn.s32ChnId = 0;	

	if(HI_SUCCESS == HI_MPI_SYS_GetBindbyDest(&stDesChn, &stSrcChn)) {
		if (stSrcChn.enModId == HI_ID_VOU && stSrcChn.s32DevId == VO_VGA_DEVICE) {
			jdk_hi_return_val_if_fail(HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn), JDK_FALSE);
			JDK_DEBUG("unbind vga wbc vga->cvbs(0)");
		}
	}

	memset(&stWbcAttr, 0, sizeof(stWbcAttr));
	if (HI_SUCCESS == HI_MPI_VO_GetWbcAttr(VO_VGA_DEVICE, &stWbcAttr)) 
	{
		/* disable wbc */ 
		jdk_hi_return_val_if_fail(HI_MPI_VO_DisableWbc (VO_VGA_DEVICE), JDK_FALSE);
	}
	return JDK_TRUE;
}



JDK_BOOL hi_3521_vo_deinit(JDK_MONITOR_TYPE monitor)
{	
	JDK_UINT32 voDev;

	voDev = VO_VGA_DEVICE;
	if (monitor == JDK_MONITOR_CVBS) {
		voDev = VO_CVBS_DEVICE;
	} else if (monitor == JDK_MONITOR_VGA || monitor == JDK_MONITOR_HDMI){
		voDev = VO_VGA_DEVICE;
	} else {
		JDK_TRACE("invalid monitor type: %x", monitor);
		return JDK_FALSE;
	}

	// disable wbc
	if (voDev == VO_VGA_DEVICE) {
		jdk_return_val_if_fail(hi_3521_vo_wbc_deinit(monitor), JDK_FALSE);
	}
	
	jdk_hi_return_val_if_fail(HI_MPI_VO_DisableVideoLayer(voDev), JDK_FALSE);
	jdk_hi_return_val_if_fail( HI_MPI_VO_Disable(voDev), JDK_FALSE);
	if (JDK_HI_SOC_SUPPORTED_HDMI() && (voDev == VO_VGA_DEVICE)) {
		jdk_hi_return_val_if_fail( HI_VO_HdmiStop(), JDK_FALSE);
	}

	return JDK_TRUE;
}

static JDK_BOOL hi_3521_vo_open_chn(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn, lpJDK_VO_CHN_ARG arg);

JDK_BOOL hi_3521_vo_init(JDK_MONITOR_TYPE monitor, JDK_STANDARD_MODE standard,
	JDK_VGA_RESOLUTION_TYPE dispResol,
	JDK_UINT32 dispOffsetX, JDK_UINT32 dispOffsetY,
	JDK_UINT32 imgW, JDK_UINT32 imgH)
{
	JDK_UINT32 voDev;
	JDK_UINT32 dispW, dispH;
	JDK_UINT32 nMaxFps = JDK_STANDARD_IS_PAL() ? 25 :30;

	VO_VIDEO_LAYER_ATTR_S videoLayerAttr;
	VO_PUB_ATTR_S voPubAttr;

	if (JDK_NET_CAM() > 0) {
		nMaxFps = 30;
	}

	voDev = VO_VGA_DEVICE;
	if (monitor == JDK_MONITOR_CVBS) {
		voDev = VO_CVBS_DEVICE;
		dispW = 704;
		dispH = (standard == JDK_STANDARD_PAL) ? 576 : 480;
		
		if (JDK_CVBS_MONITOR_MODE() != JDK_SD_MODE_DATA_BY_HD_WBC) {
			JDK_TRACE("ERROR:: cvbs monitor only support for WBC Mode Now!!!!!");
			return JDK_FALSE;
		}
	} else if (monitor == JDK_MONITOR_VGA || monitor == JDK_MONITOR_HDMI){
		voDev = VO_VGA_DEVICE;
		jdk_return_val_if_fail(JDK_vga_resol_get_wh(dispResol, &dispW, &dispH), JDK_FALSE);
	} else {
		JDK_TRACE("invalid monitor type: %x", monitor);
		return JDK_FALSE;
	}


	if (imgW == 0 || imgH == 0) {
		imgW = dispW;
		imgH = dispH;
	}
	
	// public attr init
	memset(&voPubAttr,  0, sizeof(voPubAttr));
	if (voDev == VO_VGA_DEVICE) {
		voPubAttr.u32BgColor = VO_BG_COLOR;
		voPubAttr.enIntfType = VO_INTF_VGA;
		if (JDK_HI_SOC_SUPPORTED_HDMI()) {
			voPubAttr.enIntfType = VO_INTF_VGA | VO_INTF_HDMI;
		} else {
			if (dispResol == JDK_VGA_1920x1080P50Hz || dispResol == JDK_VGA_1920x1080P60Hz) {
				JDK_TRACE("invalid resolution: %d", dispResol);
				return JDK_FALSE;
			}
		}

		switch(dispResol) {
			default:
			case JDK_VGA_800x600:	voPubAttr.enIntfSync = VO_OUTPUT_800x600_60;   break;
			case JDK_VGA_1024x768:	voPubAttr.enIntfSync = VO_OUTPUT_1024x768_60;  break;
#if defined(_JA7216NC)
			case JDK_VGA_1280x1024: voPubAttr.enIntfSync = VO_OUTPUT_1024x768_60; break;
#else
			case JDK_VGA_1280x1024: voPubAttr.enIntfSync = VO_OUTPUT_1280x1024_60;  break;
#endif
			case JDK_VGA_1366x768:	voPubAttr.enIntfSync = VO_OUTPUT_1366x768_60;  break;
			case JDK_VGA_1440x900:	voPubAttr.enIntfSync = VO_OUTPUT_1440x900_60;  break;
			case JDK_VGA_1920x1080P50Hz:   voPubAttr.enIntfSync = VO_OUTPUT_1080P50; break;
			case JDK_VGA_1920x1080P60Hz:   voPubAttr.enIntfSync = VO_OUTPUT_1080P60; break;
		}
	}
	else
	{
		voPubAttr.u32BgColor = VO_BG_COLOR;
		voPubAttr.enIntfType = VO_INTF_CVBS;
		voPubAttr.enIntfSync = JDK_STANDARD_IS_PAL() ? VO_OUTPUT_PAL : VO_OUTPUT_NTSC;
	}

	if(voPubAttr.enIntfType & VO_INTF_HDMI) {
		jdk_hi_return_val_if_fail( HI_SYS_SetMemConf(HI_ID_VOU, voDev, 0, (voDev == VO_VGA_DEVICE) ?MMZ_ZONE_NAME1 :MMZ_ZONE_NAME0), JDK_FALSE);
	}
	
	jdk_hi_return_val_if_fail( HI_MPI_VO_Disable(voDev), JDK_FALSE);
	jdk_hi_return_val_if_fail( HI_MPI_VO_SetPubAttr(voDev, &voPubAttr), JDK_FALSE);
#if  defined(_HI3520D_RAM_LITE)
	jdk_hi_return_val_if_fail( HI_MPI_VO_SetDispBufLen(voDev, (voDev == VO_VGA_DEVICE) ?4 :3), JDK_FALSE);
#endif
	jdk_hi_return_val_if_fail( HI_MPI_VO_Enable(voDev), JDK_FALSE);
	
	if(voPubAttr.enIntfType & VO_INTF_HDMI) {
		jdk_hi_return_val_if_fail( HI_VO_HdmiStart(voPubAttr.enIntfSync), JDK_FALSE);
	}

	// video layor init
	memset(&videoLayerAttr,  0, sizeof(videoLayerAttr));

	videoLayerAttr.stImageSize.u32Width  = JDK_MIN2(imgW, dispW);
	videoLayerAttr.stImageSize.u32Height = JDK_MIN2(imgH, dispH);
	videoLayerAttr.stDispRect.s32X = dispOffsetX;
	videoLayerAttr.stDispRect.s32Y = dispOffsetY;
	videoLayerAttr.stDispRect.u32Width	= dispW;
	videoLayerAttr.stDispRect.u32Height = dispH;
	//videoLayerAttr.u32DispFrmRt = 60;

#if  defined(HD_DISPLAY_SIZE_OPTIMISED)
	videoLayerAttr.stImageSize.u32Width  = videoLayerAttr.stDispRect.u32Width;
	videoLayerAttr.stImageSize.u32Height = videoLayerAttr.stDispRect.u32Height;
	if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3520D) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3531)) {
//#if  defined(_EXT_HDDVR_LITE) && !defined(_HVR)
#if  defined(_EXT_HDDVR_LITE)
		if(videoLayerAttr.stImageSize.u32Width > 1440) {
			videoLayerAttr.stImageSize.u32Width = 1440;
			videoLayerAttr.stImageSize.u32Height = 1080;
			s_jdkCtx->voScreenRect[0].width = videoLayerAttr.stImageSize.u32Width;
			s_jdkCtx->voScreenRect[0].height = videoLayerAttr.stImageSize.u32Height;
		}
#endif
	}
#endif

	videoLayerAttr.u32DispFrmRt = nMaxFps;
	videoLayerAttr.enPixFormat	= PIXEL_FORMAT_YUV_SEMIPLANAR_420;

	jdk_hi_return_val_if_fail( HI_MPI_VO_DisableVideoLayer(voDev), JDK_FALSE);

	JDK_TRACE("vodev:%u, offset(%u, %u) imgW:%u imgH:%u dispW:%u dispH:%u", voDev, dispOffsetX, dispOffsetY, imgW, imgH, dispW, dispH);
	jdk_hi_return_val_if_fail( HI_MPI_VO_SetVideoLayerAttr(voDev, &videoLayerAttr), JDK_FALSE);
	jdk_hi_return_val_if_fail( HI_MPI_VO_EnableVideoLayer(voDev), JDK_FALSE);

	if ((voDev == VO_CVBS_DEVICE) && (JDK_CVBS_MONITOR_MODE() == JDK_SD_MODE_DATA_BY_HD_WBC)) {
		// create a vo chn for cvbs vo device
		stJDK_VO_CHN_ARG voChnArg;

		memset(&voChnArg, 0, sizeof(voChnArg));
		voChnArg.rect.x = dispOffsetX;
		voChnArg.rect.y = dispOffsetY;
		voChnArg.rect.width = imgW;
		voChnArg.rect.height = imgH;
		jdk_return_val_if_fail(hi_3521_vo_open_chn(monitor, 0, &voChnArg), JDK_FALSE);

		// enable WBC
		jdk_return_val_if_fail(hi_3521_vo_wbc_init(imgW, imgH, videoLayerAttr.u32DispFrmRt, videoLayerAttr.enPixFormat), JDK_FALSE);
	}
	
	return JDK_TRUE;
}


JDK_BOOL hi_3521_vo_open_chn(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn, lpJDK_VO_CHN_ARG arg)
{
//	HI_S32 tmpVpssChn = VPSS_PRE0_CHN;
	VO_CHN_ATTR_S stVoChnAttr;
	JDK_UINT32 voDev;

	voDev = VO_VGA_DEVICE;
	if (monitor == JDK_MONITOR_CVBS) {
		voDev = VO_CVBS_DEVICE;
//#if defined(FORCE_NORMAL_CVBS_OUTPUT)
//		tmpVpssChn = VPSS_PRE1_CHN;
//#endif
	} else if (monitor == JDK_MONITOR_VGA || monitor == JDK_MONITOR_HDMI){
		voDev = VO_VGA_DEVICE;
	} else {
		JDK_TRACE("invalid monitor type: %x", monitor);
		return JDK_FALSE;
	}
	
	memset(&stVoChnAttr, 0, sizeof(stVoChnAttr));
	stVoChnAttr.stRect.s32X 	  = arg->rect.x;
	stVoChnAttr.stRect.s32Y 	  = arg->rect.y;
	stVoChnAttr.stRect.u32Width   = arg->rect.width;
	stVoChnAttr.stRect.u32Height  = arg->rect.height;
	stVoChnAttr.u32Priority 	  = 0;
	stVoChnAttr.bDeflicker		  = (voDev == VO_CVBS_DEVICE) ? HI_TRUE : HI_FALSE;
	if ((voDev == VO_VGA_DEVICE) && (JDK_CVBS_MONITOR_MODE() == JDK_SD_MODE_DATA_BY_PAINT)) {
		stVoChnAttr.bDeflicker		  = HI_FALSE;
	} else {
		stVoChnAttr.bDeflicker		  = HI_TRUE;
	}

	jdk_hi_return_val_if_fail( HI_MPI_VO_SetChnAttr(voDev, chn, &stVoChnAttr), JDK_FALSE);
//#if   !defined(FORCE_NORMAL_CVBS_OUTPUT)
	if(voDev == VO_VGA_DEVICE)
//#endif
	{
		jdk_hi_return_val_if_fail(HI_MPI_VO_SetChnFrameRate(voDev, chn, 30), JDK_FALSE);
	}
	jdk_hi_return_val_if_fail( HI_MPI_VO_ChnShow(voDev, chn), JDK_FALSE);
	
	jdk_hi_return_val_if_fail( HI_MPI_VO_EnableChn(voDev, chn), JDK_FALSE);

	/*
	jdk_hi_return_val_if_fail( HI_MPI_Sys_BindByForce(HI_ID_VOU, voDev, chn,
							HI_ID_VPSS, chn, tmpVpssChn), JDK_FALSE);
	jdk_hi_return_val_if_fail( HI_MPI_Sys_BindByForce(HI_ID_VOU, voDev, chn,
							HI_ID_VPSS, chn + JDK_ACTIVE_CAM(), tmpVpssChn), JDK_FALSE);
	*/
	JDK_DEBUG("vo dev:%x chn %d open", voDev, chn);

	return JDK_TRUE;
}

JDK_BOOL hi_3521_vo_chn_is_open(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn)
{
	HI_S32 nRet = -1;
	JDK_UINT32 voDev;
	VO_QUERY_STATUS_S chnStatus;

	voDev = VO_VGA_DEVICE;
	if (monitor == JDK_MONITOR_CVBS) {
		voDev = VO_CVBS_DEVICE;
	} else if (monitor == JDK_MONITOR_VGA || monitor == JDK_MONITOR_HDMI){
		voDev = VO_VGA_DEVICE;
	} else {
		JDK_TRACE("invalid monitor type: %x", monitor);
		return JDK_FALSE;
	}

	memset(&chnStatus, 0, sizeof(chnStatus));
	nRet =  HI_MPI_VO_QueryChnStat(voDev, chn, &chnStatus);
	if (nRet == HI_SUCCESS) {
		return JDK_TRUE;
	}

	return JDK_FALSE;
}

JDK_BOOL hi_3521_vo_chn_detach_binding(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn)
{
	JDK_UINT32 voDev;

	voDev = VO_VGA_DEVICE;
	if (monitor == JDK_MONITOR_CVBS) {
		voDev = VO_CVBS_DEVICE;
	} else if (monitor == JDK_MONITOR_VGA || monitor == JDK_MONITOR_HDMI){
		voDev = VO_VGA_DEVICE;
	} else {
		JDK_TRACE("invalid monitor type: %x", monitor);
		return JDK_FALSE;
	}

	if (hi_3521_vo_chn_is_open(monitor, chn) == JDK_TRUE) {
		jdk_hi_return_val_if_fail( HI_MPI_Sys_UnBindByForce(HI_ID_VOU, voDev, chn), JDK_FALSE);
	}
	return JDK_TRUE;
}


JDK_BOOL hi_3521_vo_hide_chn(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn)
{
	JDK_UINT32 voDev;
	VO_QUERY_STATUS_S chnStatus;

	voDev = VO_VGA_DEVICE;
	if (monitor == JDK_MONITOR_CVBS) {
		voDev = VO_CVBS_DEVICE;
	} else if (monitor == JDK_MONITOR_VGA || monitor == JDK_MONITOR_HDMI){
		voDev = VO_VGA_DEVICE;
	} else {
		JDK_TRACE("invalid monitor type: %x", monitor);
		return JDK_FALSE;
	}

	jdk_hi_return_val_if_fail( HI_MPI_VO_ClearChnBuffer(voDev, chn, HI_TRUE), JDK_FALSE);
	jdk_hi_return_val_if_fail( HI_MPI_VO_ChnHide(voDev, chn), JDK_FALSE);
	
	JDK_DEBUG("vo dev:%x chn %d hide", voDev, chn);
	
	return JDK_TRUE;
}

JDK_BOOL hi_3521_vo_show_chn(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn)
{
	JDK_UINT32 voDev;
	VO_QUERY_STATUS_S chnStatus;

	voDev = VO_VGA_DEVICE;
	if (monitor == JDK_MONITOR_CVBS) {
		voDev = VO_CVBS_DEVICE;
	} else if (monitor == JDK_MONITOR_VGA || monitor == JDK_MONITOR_HDMI){
		voDev = VO_VGA_DEVICE;
	} else {
		JDK_TRACE("invalid monitor type: %x", monitor);
		return JDK_FALSE;
	}

	jdk_hi_return_val_if_fail( HI_MPI_VO_ClearChnBuffer(voDev, chn, HI_TRUE), JDK_FALSE);
	jdk_hi_return_val_if_fail( HI_MPI_VO_ChnShow(voDev, chn), JDK_FALSE);
	
	JDK_DEBUG("vo dev:%x chn %d show", voDev, chn);
	
	return JDK_TRUE;
}


JDK_BOOL hi_3521_vo_close_chn(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn)
{
	JDK_UINT32 voDev;
	VO_QUERY_STATUS_S chnStatus;

	voDev = VO_VGA_DEVICE;
	if (monitor == JDK_MONITOR_CVBS) {
		voDev = VO_CVBS_DEVICE;
	} else if (monitor == JDK_MONITOR_VGA || monitor == JDK_MONITOR_HDMI){
		voDev = VO_VGA_DEVICE;
	} else {
		JDK_TRACE("invalid monitor type: %x", monitor);
		return JDK_FALSE;
	}

	jdk_hi_return_val_if_fail( HI_MPI_VO_ClearChnBuffer(voDev, chn, HI_TRUE), JDK_FALSE);
	jdk_hi_return_val_if_fail( HI_MPI_VO_DisableChn(voDev, chn), JDK_FALSE);
	
	JDK_DEBUG("vo dev:%x chn %d close", voDev, chn);
	
	return JDK_TRUE;
}


JDK_BOOL hi_3521_vo_enable_user_pic(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn)
{
	JDK_UINT32 voDev;

	voDev = VO_VGA_DEVICE;
	if (monitor == JDK_MONITOR_CVBS) {
		voDev = VO_CVBS_DEVICE;
	} else if (monitor == JDK_MONITOR_VGA || monitor == JDK_MONITOR_HDMI){
		voDev = VO_VGA_DEVICE;
	} else {
		JDK_TRACE("invalid monitor type: %x", monitor);
		return JDK_FALSE;
	}

	return JDK_TRUE;
	
}


JDK_BOOL hi_3521_vo_show_user_pic(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn, JDK_POINTER picHandler)
{
	JDK_UINT32 voDev;

	voDev = VO_VGA_DEVICE;
	if (monitor == JDK_MONITOR_CVBS) {
		voDev = VO_CVBS_DEVICE;
	} else if (monitor == JDK_MONITOR_VGA || monitor == JDK_MONITOR_HDMI){
		voDev = VO_VGA_DEVICE;
	} else {
		JDK_TRACE("invalid monitor type: %x", monitor);
		return JDK_FALSE;
	}
	
	JDK_DEBUG("vo show user pic @chn:%d", chn);
	
	jdk_hi_return_val_if_fail( HI_MPI_VPSS_ResetGrp(chn + JDK_ACTIVE_CAM()), JDK_FALSE);
	jdk_hi_return_val_if_fail(HI_MPI_VO_ClearChnBuffer(voDev, chn, HI_TRUE), JDK_FALSE);
	jdk_hi_return_val_if_fail(HI_MPI_VPSS_UserSendFrame(chn + JDK_ACTIVE_CAM(), picHandler), JDK_FALSE);

	return JDK_TRUE;
}

JDK_BOOL hi_3521_vo_disable_user_pic(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn)
{
	return JDK_FALSE;
}


JDK_BOOL hi_3521_vo_set_color(JDK_UINT32 chn, JDK_FLOAT brightness, JDK_FLOAT contrast, JDK_FLOAT saturation, JDK_FLOAT sharpness, JDK_FLOAT hue)
{
	return JDK_TRUE;
}

JDK_BOOL hi_3521_vo_get_color(JDK_UINT32 chn, JDK_FLOAT *brightness, JDK_FLOAT *contrast, JDK_FLOAT *saturation, JDK_FLOAT *sharpness, JDK_FLOAT *hue)
{
	return JDK_TRUE;
}


static JDK_BOOL hi_3521_vi_init(JDK_STANDARD_MODE standard, JDK_UINT32 imgW, JDK_UINT32 imgH)
{
	int ii;
	int nViChnStp = 1; // ???

	// optimize ddr usage
    for(ii = 0; ii < JDK_ACTIVE_CAM(); ii++) {
        jdk_hi_return_val_if_fail( HI_SYS_SetMemConf(HI_ID_VPSS, ii, 0, (ii < JDK_ACTIVE_CAM()/4) ?MMZ_ZONE_NAME0 :MMZ_ZONE_NAME1), JDK_FALSE);
    }
    for(; ii < JDK_ACTIVE_CAM() * 2; ii++) {
        jdk_hi_return_val_if_fail( HI_SYS_SetMemConf(HI_ID_VPSS, ii, 0, (ii < (JDK_ACTIVE_CAM()*5/4 )) ?MMZ_ZONE_NAME0 :MMZ_ZONE_NAME1), JDK_FALSE);
    }

	// vi unit memory config
    for(ii = 0; ii < JDK_BNC_CAM(); ii++) {
        MPP_CHN_S stMppChnVI;

        stMppChnVI.enModId  = HI_ID_VIU;
        stMppChnVI.s32DevId = 0; //For VIU mode, this item must be set to zero
        stMppChnVI.s32ChnId = (JDK_INT32)vimap_get_real_chn(JDK_MAX_BNC_CAM(), ii);

        jdk_hi_return_val_if_fail( HI_MPI_SYS_SetMemConf(&stMppChnVI, (ii < (JDK_BNC_CAM()/4)) ? MMZ_ZONE_NAME0 : MMZ_ZONE_NAME1), JDK_FALSE);
    }


	// 9[9]/25[25] : zero channel
	#if 0
	if(JDK_ZERO_CAM() != 0) {
        for(ii = JDK_ACTIVE_CAM(); ii < JDK_TOTAL_CAM(); ii++) {
            jdk_hi_return_val_if_fail( HI_SYS_SetMemConf(HI_ID_VPSS, ii + JDK_ACTIVE_CAM(), 0, MMZ_ZONE_NAME1), JDK_FALSE);
            jdk_return_val_if_fail(hi_3521_vpss_group_init(ii + JDK_ACTIVE_CAM(), FALSE, imgW, imgH, NULL,
                PIXEL_FORMAT_YUV_SEMIPLANAR_420, FALSE, FALSE, TRUE), JDK_FALSE);
        }
    }
	#endif
	
    //For Single Channel Screen View
    jdk_hi_return_val_if_fail( HI_SYS_SetMemConf(HI_ID_VPSS, JDK_TOTAL_CAM() + JDK_ACTIVE_CAM(), 0, MMZ_ZONE_NAME1), JDK_FALSE);

	/*
	jdk_return_val_if_fail(hi_3521_vpss_group_init(JDK_TOTAL_CAM() + JDK_ACTIVE_CAM(),
		FALSE,
		1280, 720,
		NULL,
		PIXEL_FORMAT_YUV_SEMIPLANAR_420,
		FALSE,
		TRUE,
		TRUE), JDK_FALSE);
	if (JDK_ZERO_CAM() > 0) {
		jdk_return_val_if_fail(hi_3521_vpss_group_init(JDK_ACTIVE_CAM() + JDK_ACTIVE_CAM(),
			FALSE,
			1280, 720,
			NULL,
			PIXEL_FORMAT_YUV_SEMIPLANAR_420,
			FALSE,
			TRUE,
			TRUE), JDK_FALSE);
	}
	*/

	return JDK_TRUE;
}


JDK_BOOL hi_3521_vi_chn_deinit(int Chnl)
{
    jdk_hi_return_val_if_fail( HI_MPI_Sys_UnBindByForce(HI_ID_VPSS, Chnl + JDK_ACTIVE_CAM(), 0), JDK_FALSE);
	if (JDK_CHN_STATE(Chnl) == JDK_CHN_STATE_BNC)  // wenson, debug
	{
		int tmpRealChnl;
		
		tmpRealChnl = (JDK_INT32)vimap_get_real_chn(JDK_MAX_BNC_CAM(), Chnl);
		if(tmpRealChnl < 0) {
			return JDK_FALSE;
		}
		
	    jdk_hi_return_val_if_fail( HI_MPI_Sys_UnBindByForce(HI_ID_VPSS, Chnl, 0), JDK_FALSE);
	    jdk_hi_return_val_if_fail( HI_MPI_VI_DisableChn(tmpRealChnl*nViChnStp), JDK_FALSE);
	}

    return JDK_TRUE;
}

JDK_BOOL hi_3521_vi_chn_init(int Chnl, lpJDK_VIN_ARG arg, HD_INPUT_FORMAT Frmt)
{
    int tmpRefSizW = 0;
    int tmpRefSizH = 0;
    int tmpFps     = 0;
    int tmpSiz     = 0;

    int tmpRealChnl = 0;

    tmpRealChnl = (JDK_INT32)vimap_get_real_chn(JDK_MAX_BNC_CAM(), Chnl);
    if(tmpRealChnl < 0) {
        return JDK_FALSE;
    }

	if(0 != HDDVR_GetFormatInfo(Frmt, &tmpRefSizW, &tmpRefSizH, &tmpFps)) {
		HDDVR_GetFormatInfo(HDVIDEO_HD720P25FPS, &tmpRefSizW, &tmpRefSizH, &tmpFps);
	}
	jdk_return_val_if_fail(JDK_vi_resize_in_cut_mode(arg->cutMode, &tmpRefSizW, &tmpRefSizH), JDK_FALSE);


	if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3520D))
	{
		if((2 == tmpRealChnl) || (6 == tmpRealChnl)) {
		    VI_CHN_BIND_ATTR_S stChnBindAttr;

		    jdk_hi_return_val_if_fail( HI_MPI_VI_ChnUnBind(tmpRealChnl), JDK_FALSE);

		    if(2 == tmpRealChnl) {
		        stChnBindAttr.ViDev = 0;
		        stChnBindAttr.ViWay = 1;
		    }

		    if(6 == tmpRealChnl) {
		        stChnBindAttr.ViDev = 1;
		        stChnBindAttr.ViWay = 1;
		    }

		    jdk_hi_return_val_if_fail( HI_MPI_VI_ChnBind(tmpRealChnl, &stChnBindAttr), JDK_FALSE);
		}
	}

    {
        VI_CHN_ATTR_S tmpChnAttr;
        PIXEL_FORMAT_E tmpPicFmt;

        tmpPicFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		
		if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3520D)) {
#ifdef _EXT_HDDVR_LITE
			//if(((2 == tmpRealChnl) || (6 == tmpRealChnl)) && (tmpRefSizW >= JDK_VRES_SIZE_W_HD720P)) {
			if( tmpRefSizW >= JDK_VRES_SIZE_W_HD720P ) {
				tmpPicFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
			}
#endif
		} else if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3521)){
			if((0 != ((tmpRealChnl/4)%2)) && (tmpRefSizW >= JDK_VRES_SIZE_W_HD720P)) {
	            tmpPicFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
	        }
		} else if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3531)){
#ifdef _EXT_HDDVR_LITE
			 if((0 != ((tmpRealChnl)%2)) && (tmpRefSizW >= JDK_VRES_SIZE_W_HD720P)) {
	            tmpPicFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
			 }
#else
	        if((0 != ((tmpRealChnl/2)%2)) && (tmpRefSizW >= JDK_VRES_SIZE_W_HD720P)) {
	            tmpPicFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
	        }
#endif
		}

        tmpChnAttr.stCapRect.s32X       = 0;
        tmpChnAttr.stCapRect.s32Y       = 0;
        tmpChnAttr.stCapRect.u32Width   = tmpRefSizW;
        tmpChnAttr.stCapRect.u32Height  = tmpRefSizH;
        tmpChnAttr.enCapSel             = VI_CAPSEL_BOTH;

		if(Frmt == HDVIDEO_SD960H25FPS)
		{
			tmpChnAttr.stDestSize.u32Width  = tmpRefSizW >> 1;
			tmpChnAttr.stDestSize.u32Height = tmpRefSizH;
		}
		else
		{
			tmpChnAttr.stDestSize.u32Width  = tmpRefSizW;
			tmpChnAttr.stDestSize.u32Height = tmpRefSizH;

		}
        tmpChnAttr.enPixFormat          = tmpPicFmt;
        tmpChnAttr.bMirror              = HI_FALSE;
        tmpChnAttr.bFlip                = HI_FALSE;

        tmpChnAttr.bChromaResample      = HI_FALSE;
        tmpChnAttr.s32SrcFrameRate      = tmpFps;
        tmpChnAttr.s32FrameRate         = (tmpFps > 30) ? (tmpFps /2) : tmpFps;;

		JDK_TRACE("vi chn(%u, %u) size: %d x %d init", Chnl, tmpRealChnl, tmpRefSizW, tmpRefSizH);

		if(Frmt == HDVIDEO_SD960H25FPS){
			HI_MPI_VI_SetChnScanMode(tmpRealChnl, VI_SCAN_INTERLACED);
		}
		else{
			HI_MPI_VI_SetChnScanMode(tmpRealChnl, VI_SCAN_PROGRESSIVE);
		}

        jdk_hi_return_val_if_fail( HI_MPI_VI_SetChnAttr(tmpRealChnl*nViChnStp, &tmpChnAttr), JDK_FALSE);
		if(tmpPicFmt == PIXEL_FORMAT_YUV_SEMIPLANAR_422) {
			jdk_hi_return_val_if_fail( HI_MPI_VI_EnableChn422to420(tmpRealChnl), JDK_FALSE);
		}
        jdk_hi_return_val_if_fail( HI_MPI_VI_EnableChn(tmpRealChnl*nViChnStp), JDK_FALSE);

    }

    return JDK_TRUE;
}


static JDK_BOOL hi_3521_vi_dev_deinit(JDK_UINT32 viDev, JDK_VI_MUX_MODE muxMode)
{
	JDK_UINT32 Chnl = vimap_dev2chn(viDev,muxMode);
	JDK_INT32 n;

	jdk_return_val_if_fail(Chnl != (-1), JDK_FALSE);
	jdk_return_val_if_fail(JDK_IS_VALID_VI_MUX(muxMode), JDK_FALSE);

	for (n = 0; n < muxMode; n++) {
		jdk_return_val_if_fail(hi_3521_vi_chn_deinit(Chnl + n), JDK_FALSE);
		hi_3521_vpss_group_deinit(Chnl + n);
	}

	jdk_hi_return_val_if_fail( HI_MPI_VI_DisableDev(viDev), JDK_FALSE);

	return JDK_TRUE;
}


static JDK_BOOL hi_3521_vi_dev_init(JDK_UINT32 viDev, lpJDK_VIN_ARG arg, JDK_UINT32 viResol)
{	
#if 0
	JDK_UINT32 viChn = 0;
	JDK_UINT32 viResol = 0;

	viChn = vimap_dev2chn(viDev);
	jdk_return_val_if_fail(viChn != (-1), JDK_FALSE);
	
	//jdk_return_val_if_fail(hi_3521_vi_dev_deinit(viDev, arg->muxMode), JDK_FALSE);

	switch(JDK_CHN_IN_MODE(viChn)) {
		case JDK_INPUT_BNC_960H:
			viResol = JDK_STANDARD_IS_PAL() ?HDVIDEO_SD960H25FPS :HDVIDEO_SD960H30FPS;
			break;
		case JDK_INPUT_BNC_720P:
			viResol = JDK_STANDARD_IS_PAL() ?HDVIDEO_HD720P25FPS :HDVIDEO_HD720P30FPS;
			break;
		case JDK_INPUT_MODE_NULL:
			JDK_TRACE("%u null mode", viChn);
			break;
		default: 
			JDK_TRACE("%u unknown inmode:%u", viChn, JDK_CHN_IN_MODE(viChn));
			return JDK_FALSE;
	}
#endif
	hi_3521_vi_dev_deinit(viDev, arg->muxMode);

    {
        VI_DEV_ATTR_S tmpViDevAttr;

		if(viResol < HDVIDEO_HD720P25FPS) {
			memcpy(&tmpViDevAttr, &DEV_ATTR_BT656_960H_2MUX, sizeof(tmpViDevAttr));
		}
		else {
			memcpy(&tmpViDevAttr, &DEV_ATTR_BT656_720P_2MUX, sizeof(tmpViDevAttr));
		}

		switch(arg->muxMode)
		{
		case JDK_VI_MUX_X1:
			tmpViDevAttr.enWorkMode = VI_WORK_MODE_1Multiplex;
			break;
		case JDK_VI_MUX_X2:
			tmpViDevAttr.enWorkMode = VI_WORK_MODE_2Multiplex;
			break;
		case JDK_VI_MUX_X4:
			tmpViDevAttr.enWorkMode = VI_WORK_MODE_4Multiplex;
			break;
		default:
			tmpViDevAttr.enWorkMode = VI_WORK_MODE_2Multiplex;
			break;
		}

		jdk_return_val_if_fail(hi_3521_vimux_set_mask(viDev, &tmpViDevAttr) == JDK_TRUE, JDK_FALSE);

		if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3515A) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3520D)) {
	        if(GPIODRV_GetVerFix() == GPIO_PROBE_VER1) {
	            tmpViDevAttr.bDataRev = HI_TRUE;
	        }
    	}

      	jdk_hi_return_val_if_fail( HI_MPI_VI_SetDevAttr(viDev, &tmpViDevAttr), JDK_FALSE);

		if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3515A) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3520D)){
			if(viResol < HDVIDEO_HD720P25FPS) {
				jdk_hi_return_val_if_fail( HI_MPI_VI_SetPortMode(viDev, VI_PORTMODE_D1), JDK_FALSE);
			}
			else {
				jdk_hi_return_val_if_fail( HI_MPI_VI_SetPortMode(viDev, VI_PORTMODE_720P), JDK_FALSE);
			}
		}

        jdk_hi_return_val_if_fail( HI_MPI_VI_EnableDev(viDev), JDK_FALSE);
    }

	JDK_DEBUG("vi dev: %d init done", viDev);
	
	return JDK_TRUE;
}

static JDK_BOOL hi_3521_bnc_vi_open(JDK_UINT32 chn, lpJDK_VIN_ARG arg)
{
	JDK_UINT32 viRealChn = 0;
	JDK_UINT32 viDev = 0;
	JDK_UINT32 fps, imgW, imgH, playImgW, playImgH;
	JDK_UINT32 viResol = 0;

	jdk_return_val_if_fail(JDK_IS_VALID_VI_MUX(arg->muxMode),JDK_FALSE);

	viRealChn = vimap_get_real_chn(JDK_MAX_BNC_CAM(), chn);
	viDev = vimap_realchn2dev(viRealChn,arg->muxMode);

	if (viRealChn == (-1) || viDev<0) {
		return JDK_FALSE;
	}
	
	// 1, init vi dev
	switch(arg->inMode) {
		case JDK_INPUT_BNC_D1:
		case JDK_INPUT_BNC_720H:
			viResol = JDK_STANDARD_IS_PAL() ?HDVIDEO_SD720H25FPS : HDVIDEO_SD720H30FPS;
			break;
		case JDK_INPUT_BNC_960H:
			viResol = JDK_STANDARD_IS_PAL() ?HDVIDEO_SD960H25FPS : HDVIDEO_SD960H30FPS;
			break;
		case JDK_INPUT_BNC_720P:
			viResol = JDK_STANDARD_IS_PAL() ?HDVIDEO_HD720P25FPS : HDVIDEO_HD720P30FPS;
			break;
		case JDK_INPUT_BNC_HFHD:
		case JDK_INPUT_BNC_1080P:
			viResol = JDK_STANDARD_IS_PAL() ?HDVIDEO_HD1080P25FPS : HDVIDEO_HD1080P30FPS;
			break;
		case JDK_INPUT_MODE_NULL:
			JDK_TRACE("%u null mode", chn);
			break;
		case JDK_INPUT_FORMAT_AUTO:
			viResol = HDDVR_InputFormatCheck(chn);
			if (viResol == HDVIDEO_UNKNOWN) {
				viResol = JDK_STANDARD_IS_PAL() ?HDVIDEO_SD960H25FPS :HDVIDEO_SD960H30FPS;
			}
			JDK_TRACE("%u] auto inmode check result: %d", chn, viResol);
			break;
		default: 
			JDK_TRACE("%u unknown inmode:%u", chn, arg->inMode);
			return JDK_FALSE;
	}

	jdk_return_val_if_fail(HDDVR_GetFormatInfo(viResol, &imgW, &imgH, &fps) == 0, JDK_FALSE);
	jdk_return_val_if_fail(JDK_vi_resize_in_cut_mode(arg->cutMode, &imgW, &imgH), JDK_FALSE);

	if (chn < JDK_MAX_PLAYBACK_CAM()) {
		jdk_return_val_if_fail(JDK_resol_get_wh(JDK_MAX_PLAYBACK_CAM_RESOL(), &playImgW, &playImgH), JDK_FALSE);
		if ((playImgH * playImgH) < (imgW * imgH)) {
			playImgW = imgW;
			playImgH = imgH;
		}
	} else {
		playImgW = imgW;
		playImgH = imgH;
	}

	//if ((chn % arg->muxMode) == 0) { /* make sure the vi-device of all channels in a group init only once */
	//	jdk_return_val_if_fail(hi_3521_vi_dev_init(viDev, arg, viResol), JDK_FALSE);
	//}
	if(JDK_is_all_sibling_chns_close(chn, arg->muxMode)){
		jdk_return_val_if_fail(hi_3521_vi_dev_init(viDev, arg, viResol), JDK_FALSE);
	}

	// 2, init vi chn
	jdk_return_val_if_fail(hi_3521_vi_chn_init(chn, arg, viResol), JDK_FALSE);

	// 3, init  vpss 
	// 3.1 live mode vpss
	jdk_return_val_if_fail(hi_3521_vpss_group_init(chn,
		TRUE,
		imgW, imgH,
		NULL,
		PIXEL_FORMAT_YUV_SEMIPLANAR_420,
		(arg->inMode < JDK_INPUT_BNC_720P) ? TRUE : FALSE,
		TRUE,
		TRUE), JDK_FALSE);

	// 3.2 playback mode vpss
	jdk_return_val_if_fail(hi_3521_vpss_group_init(chn + JDK_ACTIVE_CAM(),
		FALSE,
		playImgW, playImgH,
		NULL,
		PIXEL_FORMAT_YUV_SEMIPLANAR_420,
		FALSE,
		FALSE,
		TRUE), JDK_FALSE);

	// 4, bind vi -> vpss
	jdk_hi_return_val_if_fail( HI_MPI_Sys_BindByForce(HI_ID_VPSS, chn, 0,
							HI_ID_VIU, 0, viRealChn * nViChnStp), JDK_FALSE);
	
	// 5, bind vdec -> vpss
	jdk_hi_return_val_if_fail( HI_MPI_Sys_BindByForce(HI_ID_VPSS, chn+JDK_ACTIVE_CAM(), 0,
							HI_ID_VDEC, 0, chn), JDK_FALSE);
	//VideoChange_UpdateCover(chn, imgW, imgH);

	HDDVR_InputFormatSetting(chn, viResol);
	if(arg->muxMode > JDK_VI_MUX_X1 ){
		if((JCONF_BNC_CAM()%arg->muxMode != 0) && (JCONF_BNC_CAM() < MAX_CAM_CH) && (chn == JCONF_BNC_CAM()-1)){
			//鍒濆鍖栧悓涓�澶嶇敤涓嬬殑 net chn
			int ii;
			for(ii = 0; ii < ((int)arg->muxMode-1); ii++){
				HDDVR_InputFormatSetting(chn+ii, viResol);
			}
		}
	}

	vpssImgSizeW[chn] = playImgW;
	vpssImgSizeH[chn] = playImgH;

	JDK_DEBUG("bnc vi %d mode:%u (%u x %u) open", chn, viResol, imgW, imgH);

	return JDK_TRUE;
}

static JDK_BOOL hi_3521_net_vi_open(JDK_UINT32 chn, lpJDK_VIN_ARG arg)
{
	JDK_UINT32 imgW, imgH;
	JDK_UINT32 viResol = JDK_STANDARD_IS_PAL() ?HDVIDEO_SD960H25FPS : HDVIDEO_SD960H30FPS;
	
	jdk_return_val_if_fail(JDK_inmode_get_wh(arg->inMode, &imgW, &imgH), JDK_FALSE);
	
	// 1, init  vpss 
	jdk_return_val_if_fail(hi_3521_vpss_group_init(chn + JDK_ACTIVE_CAM(),
		FALSE,
		imgW, imgH,
		NULL,
		PIXEL_FORMAT_YUV_SEMIPLANAR_420,
		FALSE,
		FALSE,
		TRUE), JDK_FALSE);

	// 2, bind vdec->vpss
	jdk_hi_return_val_if_fail( HI_MPI_Sys_BindByForce(HI_ID_VPSS, chn+JDK_ACTIVE_CAM(), 0,
							HI_ID_VDEC, 0, chn), JDK_FALSE);

	// 3, bind vpss-> vo
	/*
	{
		HI_S32 tmpVpssChn = VPSS_PRE0_CHN;
		JDK_UINT32 voDev;
		JDK_UINT32 vo_chn = chn;
		
		voDev = VO_VGA_DEVICE;
		jdk_hi_return_val_if_fail( HI_MPI_Sys_BindByForce(HI_ID_VOU, voDev, vo_chn,
								HI_ID_VPSS, chn + JDK_ACTIVE_CAM(), tmpVpssChn), JDK_FALSE);
	}
	*/
	
	vpssImgSizeW[chn] = imgW;
	vpssImgSizeH[chn] = imgH;

	//设置该通道分辨率避免无设置对其他通道造成影响
#ifndef _EXT_HDDVR_LITE
	HDDVR_InputFormatSetting(chn, viResol);
#endif	
	JDK_DEBUG("net vi %d(%u x %u) open", chn, imgW, imgH);
	
	return JDK_TRUE;

}

static JDK_BOOL hi_3521_vi_close(JDK_UINT32 chn)
{
	JDK_UINT32 viRealChn = 0;
	JDK_INT32 viDev = 0;
	JDK_VI_MUX_MODE muxMode = JDK_get_mux_mode();

	viRealChn = vimap_get_real_chn(JDK_MAX_BNC_CAM(), chn);
	viDev = vimap_realchn2dev(viRealChn,muxMode);
	
	if (JDK_CHN_STATE(chn) == JDK_CHN_STATE_BNC) {
		jdk_return_val_if_fail(hi_3521_vi_chn_deinit(chn), JDK_FALSE);
		JDK_CHN_SET_STATE(chn, JDK_CHN_STATE_NULL);
		hi_3521_vpss_group_deinit(chn);
		hi_3521_vpss_group_deinit(chn + JDK_ACTIVE_CAM());
	} else if (JDK_CHN_STATE(chn) == JDK_CHN_STATE_NET) {
		JDK_CHN_SET_STATE(chn, JDK_CHN_STATE_NULL);
		jdk_return_val_if_fail(hi_3521_vi_chn_deinit(chn), JDK_FALSE);
		jdk_return_val_if_fail(hi_3521_vpss_group_deinit(chn + JDK_ACTIVE_CAM()), JDK_FALSE);
	}else if (JDK_CHN_STATE(chn) == JDK_CHN_STATE_NULL) {
		return JDK_TRUE;
	} else {
		JDK_TRACE("invalid chn state: %d", JDK_CHN_STATE(chn));
		return JDK_FALSE;
	}

	JDK_CHN_SET_STATE(chn, JDK_CHN_STATE_NULL);

	if (JDK_is_all_sibling_chns_close(chn, muxMode)) { /* make sure vi-device deinit only once for all channels in a group */
		jdk_hi_return_val_if_fail( HI_MPI_VI_DisableDev(viDev), JDK_FALSE);
	}

	memset(&s_jdkCtx->viArg[chn], 0, sizeof(stJDK_VIN_ARG));

	return JDK_TRUE;
}

JDK_BOOL hi_3521_vo_is_bind(JDK_UINT32 vo_chn, JDK_UINT32 vpssGrp)
{
	JDK_UINT32 voDev;
	MPP_CHN_S stSrcChn,stDesChn;

	voDev = VO_VGA_DEVICE;
	stDesChn.enModId  = HI_ID_VOU;
	stDesChn.s32DevId = voDev;
	stDesChn.s32ChnId = vo_chn;	

	if(HI_SUCCESS == HI_MPI_SYS_GetBindbyDest(&stDesChn, &stSrcChn)) {
		if (stSrcChn.enModId == HI_ID_VPSS){
			if (stSrcChn.s32DevId == vpssGrp) {
				JDK_DEBUG("vpss(%u)->vo(%u)", vpssGrp, vo_chn);
				return JDK_TRUE;
			}
		} else {
			JDK_DEBUG("vpss(%u)->vo(%u) has no binding", vpssGrp, vo_chn);
		}
	}
	return JDK_FALSE;
}


JDK_BOOL hi_3521_vi_bind_output(JDK_UINT32 vi_chn, JDK_UINT32 vo_chn)
{
	HI_S32 tmpVpssChn = VPSS_PRE0_CHN;
	JDK_UINT32 viRealChn = 0;
	JDK_UINT32 viDev = 0;
	JDK_UINT32 voDev;
	JDK_VI_MUX_MODE muxMode = JDK_get_mux_mode();

	voDev = VO_VGA_DEVICE;

	viRealChn = vimap_get_real_chn(JDK_MAX_BNC_CAM(), vi_chn);
	viDev = vimap_realchn2dev(viRealChn,muxMode);

	if (viRealChn == (-1)) {
		return JDK_FALSE;
	}

	//jdk_hi_return_val_if_fail( HI_MPI_Sys_BindByForce(HI_ID_VPSS, vo_chn, 0,
	//						HI_ID_VIU, 0, viRealChn * nViChnStp), JDK_FALSE);

	jdk_hi_return_val_if_fail( HI_MPI_Sys_BindByForce(HI_ID_VOU, voDev, vo_chn,
							HI_ID_VPSS, vi_chn, tmpVpssChn), JDK_FALSE);

	JDK_DEBUG("bind vi(%u)->vo(%u)", vi_chn, vo_chn);
	return JDK_TRUE;
}

JDK_BOOL hi_3521_vi_unbind_output(JDK_UINT32 vi_chn, JDK_UINT32 vo_chn)
{
	HI_S32 tmpVpssChn = VPSS_PRE0_CHN;
	JDK_UINT32 voDev;
	MPP_CHN_S stSrcChn,stDesChn;

	voDev = VO_VGA_DEVICE;


	stDesChn.enModId  = HI_ID_VOU;
	stDesChn.s32DevId = voDev;
	stDesChn.s32ChnId = vo_chn;	

	if(HI_SUCCESS == HI_MPI_SYS_GetBindbyDest(&stDesChn, &stSrcChn)) {
		if (stSrcChn.enModId == HI_ID_VIU) {
			jdk_hi_return_val_if_fail(HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn), JDK_FALSE);
			JDK_DEBUG("unbind vi(%u)->vo(%u)", vi_chn, vo_chn);
		}else if (stSrcChn.enModId == HI_ID_VPSS){
			if (stSrcChn.s32DevId < JDK_TOTAL_CAM()) {
				jdk_hi_return_val_if_fail(HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn), JDK_FALSE);
				JDK_DEBUG("unbind vi-vpss1(%u)->vo(%u)", vi_chn, vo_chn);
			}
		} else {
			JDK_DEBUG("unbind vi(%u)->vo(%u) fail for no binding of vi->vo", vi_chn, vo_chn);
		}
	}

	//jdk_hi_return_val_if_fail( HI_MPI_Sys_UnBindByForce(HI_ID_VOU, voDev, vo_chn), JDK_FALSE);
	
	//jdk_hi_return_val_if_fail( HI_MPI_Sys_BindByForce(HI_ID_VOU, voDev, vo_chn,
	//						HI_ID_VPSS, vi_chn + JDK_ACTIVE_CAM(), tmpVpssChn), JDK_FALSE);
	
	//JDK_DEBUG("unbind vi(%u)->vo(%u)", vi_chn, vo_chn);
	
	return JDK_TRUE;
}


static JDK_BOOL hi_3521_vi_deinit()
{
	JDK_UINT32 ii   = 0;

	for ( ii = 0; ii < JDK_TOTAL_CAM(); ii++) {
		hi_3521_vi_close(ii);
	}
	
	return JDK_FALSE;
}

static JDK_BOOL hi_3521_vi_open(JDK_UINT32 chn, lpJDK_VIN_ARG arg)
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
	jdk_return_val_if_fail(arg != NULL, JDK_FALSE);


	JDK_TRACE("vi open %u mode:%u state:%u", chn, arg->inMode, JDK_CHN_STATE(chn));
	if (JDK_CHN_IN_MODE(chn) == arg->inMode) {
		JDK_TRACE("chn in mode not changed!!!");
		return JDK_TRUE;
	}

	if (JDK_CHN_STATE(chn) != JDK_CHN_STATE_NULL) {
		jdk_return_val_if_fail(hi_3521_vi_close(chn), JDK_FALSE);
	}

	if ((arg->inMode >= JDK_INPUT_BNC_BASE && arg->inMode < JDK_INPUT_BNC_END) 
		|| arg->inMode == JDK_INPUT_FORMAT_AUTO) {
		jdk_return_val_if_fail(hi_3521_bnc_vi_open(chn, arg), JDK_FALSE);
		JDK_CHN_SET_STATE(chn, JDK_CHN_STATE_BNC);
	} else if (arg->inMode >= JDK_INPUT_NET_BASE && arg->inMode < JDK_INPUT_NET_END) {
		jdk_return_val_if_fail(hi_3521_net_vi_open(chn, arg), JDK_FALSE);
		JDK_CHN_SET_STATE(chn, JDK_CHN_STATE_NET);	
	} else {
		JDK_TRACE("chn in mode invalid:%d!!!", arg->inMode);
		return JDK_FALSE;
	}

	JDK_CHN_SET_IN_MODE(chn, arg->inMode);
	memcpy(&s_jdkCtx->viArg[chn], arg, sizeof(stJDK_VIN_ARG));

	return JDK_TRUE;
}

static JDK_BOOL hi_3521_vi_is_open(JDK_UINT32 chn)
{
	return JDK_TRUE;
}

static JDK_BOOL hi_3521_vi_get_size(JDK_UINT32 chn, lpJDK_SIZE size)
{
	JDK_UINT32 viRealChn = 0;
	VI_CHN_ATTR_S pstAttr;

	viRealChn = vimap_get_real_chn(JDK_MAX_BNC_CAM(), chn);

	jdk_return_val_if_fail(size != NULL, JDK_FALSE);
	jdk_hi_return_val_if_fail(HI_MPI_VI_GetChnAttr(viRealChn, &pstAttr), JDK_FALSE);
	size->width = pstAttr.stDestSize.u32Width;
	size->height = pstAttr.stDestSize.u32Height;
	
	return JDK_TRUE;
}

static JDK_BOOL hi_3521_bnc_vi_digital_zoom_init(JDK_UINT32 nChn)
{
	if (JDK_IS_BNC_CAM(nChn)) {
		HI_S32 tmpVpssChn = VPSS_PRE0_CHN;
		JDK_UINT32 voDev = VO_VGA_DEVICE;
		JDK_UINT32 viRealChn = 0;

		if (hi_3521_vo_is_bind(nChn, nChn) == JDK_FALSE) {
			return JDK_FALSE;
		}
		
		viRealChn = vimap_get_real_chn(JDK_MAX_BNC_CAM(), nChn);
		// create vpss2
		jdk_return_val_if_fail(hi_3521_vpss_group_init(JDK_TOTAL_CAM()+ JDK_ACTIVE_CAM(),
			FALSE,
			vpssImgSizeW[nChn], vpssImgSizeW[nChn],
			NULL,
			PIXEL_FORMAT_YUV_SEMIPLANAR_420,
			FALSE,
			FALSE,
			TRUE), JDK_FALSE);
		
		// bind vi->vpss2
		jdk_hi_return_val_if_fail( HI_MPI_Sys_BindByForce(HI_ID_VPSS, JDK_TOTAL_CAM()+ JDK_ACTIVE_CAM(), 0,
								HI_ID_VIU, 0, viRealChn * nViChnStp), JDK_FALSE);
		// unbind vi(vpss1)->vo
		jdk_return_val_if_fail(hi_3521_vi_unbind_output(nChn, nChn), JDK_FALSE);
	
		// bind vpss2->vo
		jdk_hi_return_val_if_fail( HI_MPI_Sys_BindByForce(HI_ID_VOU, voDev, nChn,
								HI_ID_VPSS, JDK_TOTAL_CAM()+ JDK_ACTIVE_CAM(), tmpVpssChn), JDK_FALSE);
	
		JDK_TRACE("vi->vpss1->vo changed to vi->vpss2->vo");
	}
	return JDK_TRUE;
}

static JDK_BOOL hi_3521_bnc_vi_digital_zoom_deinit(JDK_UINT32 nChn)
{
	if (JDK_IS_BNC_CAM(nChn)) {
		HI_S32 tmpVpssChn = VPSS_PRE0_CHN;
		JDK_UINT32 voDev = VO_VGA_DEVICE;
		JDK_UINT32 viRealChn = 0;
		
		viRealChn = vimap_get_real_chn(JDK_MAX_BNC_CAM(), nChn);
		// upbind vpss2->vo
		jdk_return_val_if_fail(hi_3521_vi_unbind_output(nChn, nChn), JDK_FALSE);

		// upbind vi->vpss2

		// destroy vpss2
		jdk_return_val_if_fail(hi_3521_vpss_group_deinit(JDK_TOTAL_CAM()+ JDK_ACTIVE_CAM()), JDK_FALSE);

		// rebind vi(vpss1)->vo
		hi_3521_vi_bind_output(nChn, nChn);
	
		JDK_TRACE("vi->vpss2->vo changed to vi->vpss1->vo");
	}
	return JDK_TRUE;
}

static unsigned char s_bZoom[JDK_MAX_CHN];
static JDK_BOOL hi_3521_vi_set_digital_zoom(JDK_UINT32 nChn, JDK_UINT32 nRatioX, JDK_UINT32 nRatioY, JDK_UINT32 nRatioW, JDK_UINT32 nRatioH)
{
#define RATIO_CALAC(r, m) ((m)/((typeof(r))(((r) * (m) /1000) /16) *16))
	if(nChn < JDK_ACTIVE_CAM()) {
		if(0 == nRatioX && 0 == nRatioY && 1000 == nRatioW && 1000 == nRatioH){
			return JDK_TRUE;
		}
		if(RATIO_CALAC(nRatioW, vpssImgSizeW[nChn]) >= 16
		|| RATIO_CALAC(nRatioH, vpssImgSizeH[nChn]) >= 16){
			return JDK_TRUE;
		}

		VPSS_CROP_INFO_S CropInfo;
		memset(&CropInfo, 0, sizeof(CropInfo));

		if(!s_bZoom[nChn]) {
			if(jdk_is_live_mode()) {
				// no zoom in for vloss channel
				if (hi_3521_bnc_vi_digital_zoom_init(nChn) == JDK_FALSE) {
					return JDK_FALSE;
				}
			}
			
			CropInfo.enCropCoordinate = VPSS_CROP_RITIO_COOR;
			CropInfo.enCapSel		  = VPSS_CAPSEL_BOTH;
			CropInfo.stCropRect.s32X  = nRatioX;
			CropInfo.stCropRect.s32Y  = nRatioY;
			CropInfo.stCropRect.u32Width  = nRatioW < 32 ?32 :nRatioW;
			CropInfo.stCropRect.u32Height = nRatioH < 32 ?32 :nRatioH;

			CropInfo.bEnable		  = HI_TRUE;
			jdk_hi_return_val_if_fail(HI_MPI_VPSS_SetCropCfg(nChn+JDK_ACTIVE_CAM(), &CropInfo), JDK_FALSE);
			if(JDK_IS_BNC_CAM(nChn) && jdk_is_live_mode() && (VO_DIVISION_X1 == JDK_VL_CUR_DIVI()) && (nChn == JDK_VL_CUR_PAGE())) {
				jdk_hi_return_val_if_fail(HI_MPI_VPSS_SetCropCfg(JDK_TOTAL_CAM()+ JDK_ACTIVE_CAM(), &CropInfo), JDK_FALSE);
			}
			s_bZoom[nChn] = TRUE;
		}else{
			CropInfo.stCropRect.u32Width  = 32;
			CropInfo.stCropRect.u32Height = 32;
			CropInfo.bEnable			  = HI_FALSE;
			jdk_hi_return_val_if_fail(HI_MPI_VPSS_SetCropCfg(nChn+JDK_ACTIVE_CAM(), &CropInfo), JDK_FALSE);
			if(JDK_IS_BNC_CAM(nChn) && jdk_is_live_mode() && (VO_DIVISION_X1 == JDK_VL_CUR_DIVI()) && (nChn == JDK_VL_CUR_PAGE())) {
				jdk_hi_return_val_if_fail(HI_MPI_VPSS_SetCropCfg(JDK_TOTAL_CAM() + JDK_ACTIVE_CAM(), &CropInfo), JDK_FALSE);
				hi_3521_bnc_vi_digital_zoom_deinit(nChn);
			}
			s_bZoom[nChn] = FALSE;
		}
		return JDK_TRUE;
	}

	return JDK_FALSE;
}


static JDK_BOOL hi_3521_vi_clear_digital_zoom(JDK_UINT32 nChn)
{
	if(nChn < JDK_ACTIVE_CAM()) {
		if (s_bZoom[nChn] == JDK_TRUE) {
			VPSS_CROP_INFO_S CropInfo;
			memset(&CropInfo, 0, sizeof(CropInfo));

			CropInfo.stCropRect.u32Width  = 32;
			CropInfo.stCropRect.u32Height = 32;
			CropInfo.bEnable              = HI_FALSE;
			jdk_hi_return_val_if_fail(HI_MPI_VPSS_SetCropCfg(nChn+JDK_ACTIVE_CAM(), &CropInfo), JDK_FALSE);
			if(jdk_is_live_mode()) {
				hi_3521_bnc_vi_digital_zoom_deinit(nChn);
			}
			//memset(&s_stVideo.astDigitalZoomRatio[nChn], 0, sizeof(s_stVideo.astDigitalZoomRatio[nChn]));
			//s_stVideo.abDigitalZoom[nChn] = FALSE;
			s_bZoom[nChn] = FALSE;
		}
	}
}


static JDK_BOOL hi_3521_vi_bind_vo(JDK_UINT32 vi_chn, JDK_UINT32 vo_chn)
{
	return JDK_FALSE;
}

static JDK_BOOL hi_3521_vi_bind_venc(JDK_UINT32 vi_chn, JDK_UINT32 venc_chn)
{
	return JDK_FALSE;
}

static JDK_BOOL hi_3521_vi_unbind_vo(JDK_UINT32 vi_chn)
{
	return JDK_FALSE;
}

static JDK_BOOL hi_3521_vi_unbind_venc(JDK_UINT32 vi_chn)
{
	return JDK_FALSE;
}

JDK_BOOL hi_3521_vi_set_color(JDK_UINT32 chn, JDK_FLOAT brightness, JDK_FLOAT contrast, JDK_FLOAT saturation, JDK_FLOAT sharpness, JDK_FLOAT hue)
{
	return JDK_FALSE;
}

JDK_BOOL hi_3521_vi_get_color(JDK_UINT32 chn, JDK_FLOAT *brightness, JDK_FLOAT *contrast, JDK_FLOAT *saturation, JDK_FLOAT *sharpness, JDK_FLOAT *hue)
{
	return JDK_FALSE;
}

JDK_BOOL hi_3521_vdec_init()
{
	return JDK_TRUE;
}

JDK_BOOL hi_3521_vdec_deinit()
{
	return JDK_TRUE;
}

JDK_BOOL hi_3521_vdec_bind_vo(JDK_UINT32 dec_chn, JDK_MONITOR_TYPE monitor, JDK_UINT32 vo_chn)
{
	HI_S32 tmpVpssChn = VPSS_PRE0_CHN;
	JDK_UINT32 voDev;

	voDev = VO_VGA_DEVICE;
	if (monitor == JDK_MONITOR_CVBS) {
		voDev = VO_CVBS_DEVICE;
	} else if (monitor == JDK_MONITOR_VGA || monitor == JDK_MONITOR_HDMI){
		voDev = VO_VGA_DEVICE;
	} else {
		JDK_TRACE("invalid monitor type: %x", monitor);
		return JDK_FALSE;
	}

	if (hi_3521_vo_is_bind(vo_chn, dec_chn + JDK_ACTIVE_CAM()) == JDK_TRUE) {
		JDK_DEBUG("vdec (%u)->vo(%u) has binding now", dec_chn, vo_chn);
		return JDK_TRUE;
	}

	
	jdk_hi_return_val_if_fail( HI_MPI_VPSS_ResetGrp(dec_chn + JDK_ACTIVE_CAM()), JDK_FALSE);

	jdk_hi_return_val_if_fail( HI_MPI_Sys_BindByForce(HI_ID_VOU, voDev, vo_chn,
							HI_ID_VPSS, dec_chn + JDK_ACTIVE_CAM(), tmpVpssChn), JDK_FALSE);
	
	
	JDK_DEBUG("bind vdec (%u)->vo(%u)", dec_chn, vo_chn);
	
	return JDK_TRUE;
}


JDK_BOOL hi_3521_vdec_unbind_vo(JDK_UINT32 dec_chn, JDK_MONITOR_TYPE monitor, JDK_UINT32 vo_chn)
{
	HI_S32 tmpVpssChn = VPSS_PRE0_CHN;
	JDK_UINT32 voDev;

	voDev = VO_VGA_DEVICE;
	if (monitor == JDK_MONITOR_CVBS) {
		voDev = VO_CVBS_DEVICE;
	} else if (monitor == JDK_MONITOR_VGA || monitor == JDK_MONITOR_HDMI){
		voDev = VO_VGA_DEVICE;
	} else {
		JDK_TRACE("invalid monitor type: %x", monitor);
		return JDK_FALSE;
	}

	jdk_hi_return_val_if_fail( HI_MPI_Sys_UnBindByForce(HI_ID_VOU, voDev, vo_chn), JDK_FALSE);
	
	
	JDK_DEBUG("unbind vdec (%u)->vo(%u)", dec_chn, vo_chn);
	
	return JDK_TRUE;
}

JDK_BOOL hi_3521_vdec_open_chn(JDK_UINT32 chn, lpJDK_VDEC_ARG arg)
{
}

JDK_BOOL hi_3521_vdec_close_chn(JDK_UINT32 chn)
{
}


stJDK_VI_IFACE g_jdkHi3521ViIface = {
	hi_3521_vi_init,
	hi_3521_vi_deinit,
	hi_3521_vi_open,
	hi_3521_vi_is_open,
	hi_3521_vi_close,
	hi_3521_vi_get_size,
	hi_3521_vi_bind_output,
	hi_3521_vi_unbind_output,
	hi_3521_vi_set_digital_zoom,
	hi_3521_vi_clear_digital_zoom,
};

stJDK_VO_IFACE g_jdkHi3521VoIface = {
	hi_3521_vo_init,
	hi_3521_vo_deinit,
	hi_3521_vo_open_chn,
	hi_3521_vo_chn_is_open,
	hi_3521_vo_close_chn,
	hi_3521_vo_hide_chn,
	hi_3521_vo_show_chn,
	hi_3521_vo_chn_detach_binding,
	hi_3521_vo_enable_user_pic,
	hi_3521_vo_show_user_pic,
	hi_3521_vo_disable_user_pic,
};

stJDK_SYS_IFACE g_jdkHi3521SysIface = {
	hi_3521_sys_init,
	hi_3521_sys_deinit,
	hi_3521_sys_open_user_frame,
	hi_3521_sys_close_user_frame,
};


stJDK_VENC_IFACE g_jdkHi3521VencIface = {
	NULL,
	NULL,
};

stJDK_VDEC_IFACE g_jdkHi3521VdecIface = {
	hi_3521_vdec_init,
	hi_3521_vdec_deinit,
	hi_3521_vdec_bind_vo,
	hi_3521_vdec_unbind_vo,
	NULL,
	NULL,
};

