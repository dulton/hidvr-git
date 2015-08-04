static int  RefSizW;
static int  RefSizH;

int  nViDevNum = VIU_MAX_DEV_NUM;
int  nViChnStp = 1;

static int nVideoModeAuto = 0;
static int nVideoModeCur[MAX_CAM_CH] = {
#if (MAX_CAM_CH == 4)
	0, 0, 0, 0,
#endif
#if (MAX_CAM_CH == 8)
	0, 0, 0, 0,
	0, 0, 0, 0,
#endif
#if (MAX_CAM_CH == 16)
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
#endif
};
static int nVideoModeLst[MAX_CAM_CH] = {
#if (MAX_CAM_CH == 4)
	-1, -1, -1, -1,
#endif
#if (MAX_CAM_CH == 8)
	-1, -1, -1, -1,
	-1, -1, -1, -1,
#endif
#if (MAX_CAM_CH == 16)
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
	-1, -1, -1, -1,
#endif
};

int  VideoModeSwitchChanged(int Chnl)
{
	if(Chnl < ARRAY_SIZE(nVideoModeCur)) {
		if(nVideoModeLst[Chnl] != nVideoModeCur[Chnl]) {
			nVideoModeLst[Chnl] = nVideoModeCur[Chnl];
			return 0;
		}
	}

	return -1;
}

int  VideoModeSwitchPrint(void)
{
	int Chnl = 0;
	for(Chnl = 0; Chnl < ARRAY_SIZE(nVideoModeCur); Chnl ++) {
		printf("%d :- Cur [%d], Lst [%d] -%s\n",
			Chnl,
			nVideoModeCur[Chnl], nVideoModeLst[Chnl],
			HDDVR_GetCHNLType(Chnl));
	}

	return -1;
}

void  VideoModeSwitchSetting(int Chnl, int V)
{
	if(Chnl < ARRAY_SIZE(nVideoModeCur)) {
		nVideoModeCur[Chnl] = V;
	}
}

void  VideoModeSwitchAutoSet(int Set)
{
	nVideoModeAuto = Set;
}

int  VideoModeSwitchAutoCheck(void)
{
	return nVideoModeAuto;
}

void VideoModeSwitchAuto(void)
{
	int tmpVideoStds[MAX_CAM_CH];
	int tmpVideoLoss[MAX_CAM_CH];
	int Chnl;

	for(Chnl = 0; Chnl < ARRAY_SIZE(tmpVideoStds); Chnl ++) {
		tmpVideoStds[Chnl] = HDDVR_InputFormatCheck(Chnl);
		tmpVideoLoss[Chnl] = HDDVR_VideoLossChnl(Chnl);
	}

	for(Chnl = 0; Chnl < ARRAY_SIZE(tmpVideoStds); Chnl += 2) {
		int ii;

		if((0 == tmpVideoLoss[Chnl])
		|| (0 == tmpVideoLoss[Chnl+1])) { //At least One Channel is Detected;
			continue;
		}

		for(ii = 0; ii < 2; ii ++) {
			int tmpStdCheck = HDVIDEO_UNKNOWN;

			switch(tmpVideoStds[Chnl+ii]) {
			case HDVIDEO_SD720H25FPS: case HDVIDEO_SD720H30FPS:
			case HDVIDEO_SD960H25FPS: case HDVIDEO_SD960H30FPS:
			case HDVIDEO_SD960H:
				tmpStdCheck = VIDEO_MODE_960H;
				break;
			case HDVIDEO_HD720P25FPS: case HDVIDEO_HD720P30FPS:
			case HDVIDEO_HD720P:
				tmpStdCheck = VIDEO_MODE_720P;
				break;
			case HDVIDEO_HD720P50FPS: case HDVIDEO_HD720P60FPS:
			case HDVIDEO_HD1080P25FPS: case HDVIDEO_HD1080P30FPS:
			case HDVIDEO_HD1080P:
			default:
			case HDVIDEO_UNKNOWN:
				break;
			}

			if(tmpStdCheck != HDVIDEO_UNKNOWN) {
				VideoModeSwitchSetting(Chnl, tmpStdCheck);
				VideoModeSwitchSetting(Chnl+1, tmpStdCheck);
				break;
			}
		}
	}

	return;
}

static int  video_ParamInit(void)
{
    RefSizW = VRES_SIZE_W_HD720P;
    RefSizH = VRES_SIZE_H_HD720P;

    return 0;
}

static int  video_ParamExit(void)
{
    return 0;
}

static void video_DisplayD1(int nVoDev, int nChn, unsigned char bDisplay)
{
	static int s_nLstViChn = -1;

	int tmpVpss = VIDEO_GetMaxDiv() + MAX_CAM_CH;

	if(bDisplay) {
		int tmpRealChnl;

		tmpRealChnl = VIMAP_Get(nChn);
		if(tmpRealChnl < 0) {
			return -1;
		}

		{
			VPSS_GRP_ATTR_S tmpVpssAttr;
			if(0 == HI_MPI_VPSS_GetGrpAttr(tmpVpss, &tmpVpssAttr)) {
				(HI_MPI_Sys_UnBindByForce(HI_ID_VPSS, tmpVpss, 0));
				VPSS_ViGroupSettingExit(tmpVpss);
			}
		}

		{
			VI_CHN_ATTR_S tmpViChnAttr;
			if(0 == HI_MPI_VI_GetChnAttr(tmpRealChnl*nViChnStp, &tmpViChnAttr)) {
				int tmpSizW = tmpViChnAttr.stDestSize.u32Width;
				int tmpSizH = tmpViChnAttr.stDestSize.u32Height;
				VPSS_ViGroupSettingInit(tmpVpss,
					TRUE,
					tmpSizW,
					tmpSizH,
					NULL,
					PIXEL_FORMAT_YUV_SEMIPLANAR_420,
					(tmpSizW <= VRES_SIZE_W_EF) ? TRUE : FALSE,
					TRUE,
					FALSE);
				
			}
		}

		s_nLstViChn = nChn;
		{
			(HI_MPI_Sys_BindByForce(HI_ID_VPSS, tmpVpss, 0, HI_ID_VIU, 0, tmpRealChnl*nViChnStp));
		}
	}else {
		if(s_nLstViChn != -1) {
			(HI_MPI_Sys_UnBindByForce(HI_ID_VPSS, tmpVpss, 0));
			VPSS_ViGroupSettingExit(tmpVpss);
		}

		s_nLstViChn = -1;
	}
}

void VI_MUX_SetMask(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr)
{
    const unsigned long ViMaskTBL[] = {
#if   defined(SDK_PLATFORM_HI3521)
        0xFF000000, 0xFF0000, 0xFF00, 0xFF,
#elif defined(SDK_PLATFORM_HI3531)
        0xFF000000, 0xFF0000, 0xFF00, 0xFF,
#endif
    };

    pstDevAttr->au32CompMask[0] = ViMaskTBL[ViDev % ARRAY_SIZE(ViMaskTBL)];
}

void VPSS_ViGroupSettingInit(
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

        DVR_ASSERT(HI_MPI_VPSS_CreateGrp(nVpssGrp, &stGrpAttr));
    }

    if(Crop) {
        DVR_ASSERT(HI_MPI_VPSS_SetCropCfg(nVpssGrp, Crop));
    }

    {  //Set VPSS Param
        VPSS_GRP_PARAM_S stVpssParam;

        DVR_ASSERT(HI_MPI_VPSS_GetGrpParam(nVpssGrp, &stVpssParam));
        stVpssParam.u32MotionThresh = 0;
        DVR_ASSERT(HI_MPI_VPSS_SetGrpParam(nVpssGrp, &stVpssParam));
    }

    {  //Enable All VPSS Channels
        int ii;
        VPSS_CHN_ATTR_S stChnAttr;

        memset(&stChnAttr, 0, sizeof(stChnAttr));

        for(ii = 0; ii < VPSS_MAX_CHN_NUM; ii ++) {

            stChnAttr.bSpEn    = HI_FALSE;
            stChnAttr.bFrameEn =
               (ii == VPSS_PRE0_CHN)
                ?(NeedSp ?HI_TRUE :HI_FALSE)
                :HI_FALSE;
            stChnAttr.stFrame.u32Color[VPSS_FRAME_WORK_LEFT]   = 0xAAAAAA;
            stChnAttr.stFrame.u32Color[VPSS_FRAME_WORK_RIGHT]  = 0xAAAAAA;
            stChnAttr.stFrame.u32Color[VPSS_FRAME_WORK_BOTTOM] = 0xAAAAAA;
            stChnAttr.stFrame.u32Color[VPSS_FRAME_WORK_TOP]    = 0xAAAAAA;
            stChnAttr.stFrame.u32Width[VPSS_FRAME_WORK_LEFT]   = 1 << 1;
            stChnAttr.stFrame.u32Width[VPSS_FRAME_WORK_RIGHT]  = 1 << 1;
            stChnAttr.stFrame.u32Width[VPSS_FRAME_WORK_TOP]    = 1 << 1;
            stChnAttr.stFrame.u32Width[VPSS_FRAME_WORK_BOTTOM] = 1 << 1;

            DVR_ASSERT(HI_MPI_VPSS_SetChnAttr(nVpssGrp, ii, &stChnAttr));
            if(IsPreView) {
                if(VPSS_PRE1_CHN == ii) {
                    VPSS_CHN_MODE_S tmpChnMode;

                    DVR_ASSERT(HI_MPI_VPSS_GetChnMode(nVpssGrp, ii, &tmpChnMode));
                    tmpChnMode.enChnMode = VPSS_CHN_MODE_USER;
                    tmpChnMode.u32Width  = RefSizW;
                    tmpChnMode.u32Height = RefSizH;
                    tmpChnMode.bDouble   = HI_FALSE;
                    tmpChnMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
                    DVR_ASSERT(HI_MPI_VPSS_SetChnMode(nVpssGrp, ii, &tmpChnMode));
                }
            }

            DVR_ASSERT(HI_MPI_VPSS_EnableChn(nVpssGrp, ii));
#if   defined(SDK_PLATFORM_HI3521) && (!defined(_HI3520D_EXT) && !defined(_HI3515A_EXT))
            if(NeedIE) {
                VPSS_CHN_NR_PARAM_S tmpNrParam;
                DVR_ASSERT(HI_MPI_VPSS_GetChnNrParam(nVpssGrp, ii, &tmpNrParam));
                tmpNrParam.u32SfStrength = 24;
                tmpNrParam.u32TfStrength = 6;
                DVR_ASSERT(HI_MPI_VPSS_SetChnNrParam(nVpssGrp, ii, &tmpNrParam));
			}
#endif
        }
    }

    if(NeedIE) {
        VPSS_GRP_PARAM_S tmpGrpParam;

        DVR_ASSERT(HI_MPI_VPSS_GetGrpParam(nVpssGrp, &tmpGrpParam));
#if   defined(SDK_PLATFORM_HI3521)
#if   defined(_HI3520D_EXT) || defined(_HI3515A_EXT)
        tmpGrpParam.u32Contrast   = 70;
        tmpGrpParam.u32IeStrength = 0;
        tmpGrpParam.u32IeSharp    = 16;
        tmpGrpParam.u32SfStrength = 24;
        tmpGrpParam.u32TfStrength = 6;
#else
        tmpGrpParam.u32Contrast   = tmpGrpParam.u32Contrast;
        tmpGrpParam.u32IeStrength = 0;
        tmpGrpParam.u32IeSharp    = 16;
        tmpGrpParam.u32SfStrength = 24;
        tmpGrpParam.u32TfStrength = 6;
#endif
#endif
#if   defined(SDK_PLATFORM_HI3531)
        tmpGrpParam.u32Contrast   = tmpGrpParam.u32Contrast;
        tmpGrpParam.u32IeStrength = 0;
        tmpGrpParam.u32IeSharp    = tmpGrpParam.u32IeSharp;
        tmpGrpParam.u32SfStrength = tmpGrpParam.u32SfStrength;
        tmpGrpParam.u32TfStrength = tmpGrpParam.u32TfStrength;
#endif
        DVR_ASSERT(HI_MPI_VPSS_SetGrpParam(nVpssGrp, &tmpGrpParam));
    }

    DVR_ASSERT(HI_MPI_VPSS_StartGrp(nVpssGrp));
}

void VPSS_ViGroupSettingExit(int  nVpssGrp)
{
    (HI_MPI_VPSS_StopGrp(nVpssGrp));

    {  //Diable All VPSS Channels
        int ii;
        for(ii = 0; ii < VPSS_MAX_CHN_NUM; ii ++) {
            (HI_MPI_VPSS_DisableChn(nVpssGrp, ii));
        }
    }

    (HI_MPI_VPSS_DestroyGrp(nVpssGrp));
}

VI_DEV_ATTR_S DEV_ATTR_BT656_720P_2MUX =
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

VI_DEV_ATTR_S DEV_ATTR_BT656_960H_2MUX =
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

int ViChnFRLimitFlg[MAX_CAM_CH];

void ViChnFpsInit(void)
{
	memset(ViChnFRLimitFlg, 0, sizeof(ViChnFRLimitFlg));
}

void ViChnFpsLimit(int Chnl, int Set)
{
	if((Chnl >= 0) && (Chnl < MAX_CAM_CH)) {
		ViChnFRLimitFlg[Chnl] = Set;
	}
}

void ViChnFpsLimitAll(int Set)
{
	int ii;
	for(ii = 0; ii < MAX_CAM_CH; ii ++) {
		ViChnFpsLimit(ii, Set);
	}
}

int FixViChnFpsByForce(int Chnl)
{
    int ViChn = Chnl*nViChnStp;
#if    defined(_EXT_HDDVR_LITE)
#if    (MAX_CAM_CH == 4)
    int MaxFpsLite = 18;
#elif  (MAX_CAM_CH == 8)
    int MaxFpsLite = 20;
#elif  (MAX_CAM_CH == 16)
    int MaxFpsLite = 15;
#endif
    int MaxHDSizW  = VRES_SIZE_W_HD720P;
    int MaxHDSizH  = VRES_SIZE_H_HD720P;
#else
	int MaxFpsLite = 15;
	int MaxHDSizW  = VRES_SIZE_W_HD1080P;
    int MaxHDSizH  = VRES_SIZE_H_HD1080P;
#endif

    int VencChn = Chnl*2;
    VENC_CHN_ATTR_S tmpVencChnAttr;

    VI_CHN_ATTR_S   tmpViChnAttr;

    int tmpRealChnl;

    tmpRealChnl = VIMAP_Get(Chnl);
    if(tmpRealChnl < 0) {
        return -1;
    }

	if(ViChnFRLimitFlg[Chnl]) {
		if((HI_SUCCESS == HI_MPI_VENC_GetChnAttr(VencChn, &tmpVencChnAttr))
		&&(HI_SUCCESS  == HI_MPI_VI_GetChnAttr(tmpRealChnl*nViChnStp, &tmpViChnAttr))) {
			if((tmpViChnAttr.s32FrameRate > MaxFpsLite)
			&& (tmpVencChnAttr.stVeAttr.stAttrH264e.u32PicWidth >= MaxHDSizW)) {
				tmpViChnAttr.s32FrameRate = MaxFpsLite;
				HI_MPI_VI_SetChnAttr(tmpRealChnl*nViChnStp, &tmpViChnAttr);
			}
		}
	}
	else {
        if(HI_SUCCESS  == HI_MPI_VI_GetChnAttr(tmpRealChnl*nViChnStp, &tmpViChnAttr)) {
			if(tmpViChnAttr.s32FrameRate != tmpViChnAttr.s32SrcFrameRate) {
				if ( tmpViChnAttr.s32SrcFrameRate <= 30) {
					tmpViChnAttr.s32FrameRate = tmpViChnAttr.s32SrcFrameRate;
					HI_MPI_VI_SetChnAttr(tmpRealChnl*nViChnStp, &tmpViChnAttr);
				}
			}
		}
	}

	return 0;
}

int FixVencPicByViChnInfo(int Chnl, int Stream)
{
    int VencChn = Chnl*2+Stream;
    int VencSrcFrmRate = 0;
    int VencTgtFrmRate = 0;

    int ViChn = Chnl*nViChnStp;
    int ViFrmRate = 0;

    VENC_CHN_ATTR_S tmpVencChnAttr;
    VI_CHN_ATTR_S   tmpViChnAttr;
	
    int tmpRealChnl;

    tmpRealChnl = VIMAP_Get(Chnl);
    if(tmpRealChnl < 0) {
        return -1;
    }

    if((HI_SUCCESS == HI_MPI_VENC_GetChnAttr(VencChn, &tmpVencChnAttr))
        &&(HI_SUCCESS  == HI_MPI_VI_GetChnAttr(tmpRealChnl*nViChnStp, &tmpViChnAttr))) {

        ViFrmRate   = tmpViChnAttr.s32FrameRate;
        switch(tmpVencChnAttr.stRcAttr.enRcMode) {
        case VENC_RC_MODE_H264CBR:
        case VENC_RC_MODE_H264CBRv2:
            VencSrcFrmRate = tmpVencChnAttr.stRcAttr.stAttrH264Cbr.u32ViFrmRate;
            VencTgtFrmRate = tmpVencChnAttr.stRcAttr.stAttrH264Cbr.fr32TargetFrmRate;
            break;
        case VENC_RC_MODE_H264VBR:
        case VENC_RC_MODE_H264VBRv2:
            VencSrcFrmRate = tmpVencChnAttr.stRcAttr.stAttrH264Vbr.u32ViFrmRate;
            VencTgtFrmRate = tmpVencChnAttr.stRcAttr.stAttrH264Vbr.fr32TargetFrmRate;
            break;
        case VENC_RC_MODE_H264ABR:
            VencSrcFrmRate = tmpVencChnAttr.stRcAttr.stAttrH264Abr.u32ViFrmRate;
            VencTgtFrmRate = tmpVencChnAttr.stRcAttr.stAttrH264Abr.fr32TargetFrmRate;
            break;
        case VENC_RC_MODE_H264FIXQP:
            VencSrcFrmRate = tmpVencChnAttr.stRcAttr.stAttrH264FixQp.u32ViFrmRate;
            VencTgtFrmRate = tmpVencChnAttr.stRcAttr.stAttrH264FixQp.fr32TargetFrmRate;
            break;
        default:
            break;
        }

        if((0 == VencSrcFrmRate) || (0 == VencTgtFrmRate)) {  //Invalid FrameRate Value
            return;
        }

        if(VencSrcFrmRate != ViFrmRate) {
            VencSrcFrmRate = ViFrmRate;

            switch(tmpVencChnAttr.stRcAttr.enRcMode) {
            case VENC_RC_MODE_H264CBR:
            case VENC_RC_MODE_H264CBRv2:
                tmpVencChnAttr.stRcAttr.stAttrH264Cbr.u32ViFrmRate = VencSrcFrmRate;
                if(VencSrcFrmRate < VencTgtFrmRate) {
                    VencTgtFrmRate = VencSrcFrmRate;

                    tmpVencChnAttr.stRcAttr.stAttrH264Cbr.u32Gop      = 2*VencTgtFrmRate;
                    tmpVencChnAttr.stRcAttr.stAttrH264Cbr.u32StatTime = 2;
                    tmpVencChnAttr.stRcAttr.stAttrH264Cbr.fr32TargetFrmRate = VencTgtFrmRate;
                }
                break;
            case VENC_RC_MODE_H264VBR:
            case VENC_RC_MODE_H264VBRv2:
                tmpVencChnAttr.stRcAttr.stAttrH264Vbr.u32ViFrmRate = VencSrcFrmRate;
                if(VencSrcFrmRate < VencTgtFrmRate) {
                    VencTgtFrmRate = VencSrcFrmRate;

                    tmpVencChnAttr.stRcAttr.stAttrH264Vbr.u32Gop            = 2*VencTgtFrmRate;
                    tmpVencChnAttr.stRcAttr.stAttrH264Vbr.u32StatTime       = 2;
                    tmpVencChnAttr.stRcAttr.stAttrH264Vbr.fr32TargetFrmRate = VencTgtFrmRate;
                }
                break;
            case VENC_RC_MODE_H264ABR:
                tmpVencChnAttr.stRcAttr.stAttrH264Abr.u32ViFrmRate = VencSrcFrmRate;
                if(VencSrcFrmRate < VencTgtFrmRate) {
                    VencTgtFrmRate = VencSrcFrmRate;

                    tmpVencChnAttr.stRcAttr.stAttrH264Abr.u32Gop            = 2*VencTgtFrmRate;
                    tmpVencChnAttr.stRcAttr.stAttrH264Abr.u32StatTime       = 2;
                    tmpVencChnAttr.stRcAttr.stAttrH264Abr.fr32TargetFrmRate = VencTgtFrmRate;
                }
                break;
            case VENC_RC_MODE_H264FIXQP:
                tmpVencChnAttr.stRcAttr.stAttrH264FixQp.u32ViFrmRate = VencSrcFrmRate;
                if(VencSrcFrmRate < VencTgtFrmRate) {
                    VencTgtFrmRate = VencSrcFrmRate;

                    tmpVencChnAttr.stRcAttr.stAttrH264FixQp.u32Gop            = 2*VencTgtFrmRate;
                    tmpVencChnAttr.stRcAttr.stAttrH264FixQp.fr32TargetFrmRate = VencTgtFrmRate;
                }
                break;
            default:
                break;
            }

            HI_MPI_VENC_SetChnAttr(VencChn, &tmpVencChnAttr);
        }
    }

    return 0;
}

int ViChnDisable(int Chnl)
{
    int tmpRealChnl;

    tmpRealChnl = VIMAP_Get(Chnl);
    if(tmpRealChnl < 0) {
        return -1;
    }

    HI_MPI_Sys_UnBindByForce(HI_ID_VPSS, Chnl, 0);
    HI_MPI_VI_DisableChn(tmpRealChnl*nViChnStp);

    return 0;
}

int ViChnSetting(int Chnl, HD_INPUT_FORMAT Frmt)
{
    int tmpRefSizW = RefSizW;
    int tmpRefSizH = RefSizH;
    int tmpFps     = 0;
    int tmpSiz     = 0;

    int tmpRealChnl;
	int ViDev = 0;

    tmpRealChnl = VIMAP_Get(Chnl);
    if(tmpRealChnl < 0) {
        return -1;
    }
#if   defined(SDK_PLATFORM_HI3521)
    ViDev = tmpRealChnl / 4;
#endif
#if   defined(SDK_PLATFORM_HI3531)
    ViDev = tmpRealChnl / 2;


#endif


	if(0 != HDDVR_GetFormatInfo(Frmt, &tmpRefSizW, &tmpRefSizH, &tmpFps)) {
		HDDVR_GetFormatInfo(HDVIDEO_HD720P25FPS, &tmpRefSizW, &tmpRefSizH, &tmpFps);
	}

	if(0)
    {
        VI_DEV_ATTR_S tmpViDevAttr;

        HI_MPI_VI_DisableDev(tmpRealChnl);

        if(tmpSiz == 0) {
            memcpy(&tmpViDevAttr, &DEV_ATTR_7441_INTERLEAVED_720P, sizeof(tmpViDevAttr));
            VI_MUX_SetMask(tmpRealChnl, &tmpViDevAttr);
        }
        if(tmpSiz == 1) {
            memcpy(&tmpViDevAttr, &DEV_ATTR_7441_INTERLEAVED_1080P, sizeof(tmpViDevAttr));
            VI_MUX_SetMask(tmpRealChnl, &tmpViDevAttr);
        }

        DVR_ASSERT(HI_MPI_VI_SetDevAttr(tmpRealChnl, &tmpViDevAttr));
        DVR_ASSERT(HI_MPI_VI_EnableDev(tmpRealChnl));
	}

#if   defined(_HI3520D_EXT)
	if((2 == tmpRealChnl) || (6 == tmpRealChnl)) {
	    VI_CHN_BIND_ATTR_S stChnBindAttr;

	    DVR_ASSERT(HI_MPI_VI_ChnUnBind(tmpRealChnl));

	    if(2 == tmpRealChnl) {
	        stChnBindAttr.ViDev = 0;
	        stChnBindAttr.ViWay = 1;
	    }

	    if(6 == tmpRealChnl) {
	        stChnBindAttr.ViDev = 1;
	        stChnBindAttr.ViWay = 1;
	    }

	    DVR_ASSERT(HI_MPI_VI_ChnBind(tmpRealChnl, &stChnBindAttr));
	}
#endif

    {
        VI_CHN_ATTR_S tmpChnAttr;
        PIXEL_FORMAT_E tmpPicFmt;

        tmpPicFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
#if   defined(_HI3520D_EXT)
		if(((2 == tmpRealChnl) || (6 == tmpRealChnl)) && (tmpRefSizW >= VRES_SIZE_W_HD720P)) {
			tmpPicFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
		}
#else
        if((0 != (tmpRealChnl%2)) && (tmpRefSizW >= VRES_SIZE_W_HD720P)) {
            tmpPicFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
        }
#endif
        tmpChnAttr.stCapRect.s32X       = 0;
        tmpChnAttr.stCapRect.s32Y       = 0;
        tmpChnAttr.stCapRect.u32Width   = tmpRefSizW;
        tmpChnAttr.stCapRect.u32Height  = tmpRefSizH;
        tmpChnAttr.enCapSel             = VI_CAPSEL_BOTH;
        tmpChnAttr.stDestSize.u32Width  = tmpRefSizW;
        tmpChnAttr.stDestSize.u32Height = tmpRefSizH;
        tmpChnAttr.enPixFormat          = tmpPicFmt;
        tmpChnAttr.bMirror              = HI_FALSE;
        tmpChnAttr.bFlip                = HI_FALSE;

        tmpChnAttr.bChromaResample      = HI_FALSE;
        tmpChnAttr.s32SrcFrameRate      = tmpFps;
        tmpChnAttr.s32FrameRate         = (tmpFps > 30) ? (tmpFps /2) : tmpFps;

        DVR_ASSERT(HI_MPI_VI_SetChnAttr(tmpRealChnl*nViChnStp, &tmpChnAttr));
		if(tmpPicFmt == PIXEL_FORMAT_YUV_SEMIPLANAR_422) {
			DVR_ASSERT(HI_MPI_VI_EnableChn422to420(tmpRealChnl));
		}
        DVR_ASSERT(HI_MPI_VI_EnableChn(tmpRealChnl*nViChnStp));
        HI_MPI_Sys_BindByForce(HI_ID_VPSS, Chnl, 0,
                                HI_ID_VIU, 0, tmpRealChnl*nViChnStp);
    }

    return 0;
}

static int tmpInputStat[MAX_CAM_CH] =
#if (MAX_CAM_CH == 4)
		{ HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, };
#endif
#if (MAX_CAM_CH == 8)
		{ HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN,
		  HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, };
#endif
#if (MAX_CAM_CH == 16)
		{ HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN,
		  HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN,
		  HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN,
		  HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, HDVIDEO_UNKNOWN, };
#endif

static void video_InputAutoFit(int Chnl)
{
	int tmpEEE;

    int tmpRealChnl;

    tmpRealChnl = VIMAP_Get(Chnl);
    if(tmpRealChnl < 0) {
		return;
    }

	if(Chnl >= MAX_CAM_CH) {
		return;
	}

	if(0 != (Chnl % 2)) { //Only Check Corresponding Even Channel
		return;
	}

	if(0 != VideoModeSwitchChanged(Chnl)) {
		return;
	}

	switch(nVideoModeLst[Chnl]) {
	case VIDEO_MODE_960H:
		tmpEEE = VIDEO_IS_PAL() ?HDVIDEO_SD960H25FPS :HDVIDEO_SD960H30FPS;
		break;

	case VIDEO_MODE_720P:
		tmpEEE = VIDEO_IS_PAL() ?HDVIDEO_HD720P25FPS :HDVIDEO_HD720P30FPS;
		break;

	default: return;
	}
	printf("\nCHECKING : %s\n", __FUNCTION__);
    {
		//Disable ViChn
#if   defined(SDK_PLATFORM_HI3521)
		int ViDev = tmpRealChnl / 4;
#endif
#if   defined(SDK_PLATFORM_HI3531)
		int ViDev = tmpRealChnl / 2;
#endif
		ViChnDisable(Chnl);
		ViChnDisable(Chnl+1);
		VPSS_ViGroupSettingExit(Chnl);
		VPSS_ViGroupSettingExit(Chnl+1);

		DVR_ASSERT(HI_MPI_VI_DisableDev(ViDev));

        {
            VI_DEV_ATTR_S tmpViDevAttr;

			if(tmpEEE < HDVIDEO_HD720P25FPS) {
				memcpy(&tmpViDevAttr, &DEV_ATTR_BT656_960H_2MUX, sizeof(tmpViDevAttr));
				VI_MUX_SetMask(ViDev, &tmpViDevAttr);
			}
			else {
				memcpy(&tmpViDevAttr, &DEV_ATTR_BT656_720P_2MUX, sizeof(tmpViDevAttr));
				VI_MUX_SetMask(ViDev, &tmpViDevAttr);
			}
#if   defined(_HI3515A_EXT) || defined(_HI3520D_EXT)
            if(GPIODRV_GetVerFix() == GPIO_PROBE_VER1) {
                tmpViDevAttr.bDataRev = HI_TRUE;
            }
#endif
            DVR_ASSERT(HI_MPI_VI_SetDevAttr(ViDev, &tmpViDevAttr));

#if   defined(_HI3515A_EXT) || defined(_HI3520D_EXT)
			if(tmpEEE < HDVIDEO_HD720P25FPS) {
				DVR_ASSERT(HI_MPI_VI_SetPortMode(ViDev, VI_PORTMODE_D1));
			}
			else {
				DVR_ASSERT(HI_MPI_VI_SetPortMode(ViDev, VI_PORTMODE_720P));
			}
#endif
            DVR_ASSERT(HI_MPI_VI_EnableDev(ViDev));
        }

		HDDVR_InputFormatSetting(Chnl, tmpEEE);
		(HI_MPI_Sys_UnBindByForce(HI_ID_VPSS, VIDEO_GetMaxDiv() + MAX_CAM_CH, 0));
		ViChnSetting(Chnl, tmpEEE);
		ViChnSetting(Chnl+1, tmpEEE);

		{
			int tmpRefSizW, tmpRefSizH;
			if(0 != HDDVR_GetFormatInfo(tmpEEE, &tmpRefSizW, &tmpRefSizH, NULL)) {
				HDDVR_GetFormatInfo(HDVIDEO_HD720P25FPS, &tmpRefSizW, &tmpRefSizH, NULL);
			}
			VPSS_ViGroupSettingInit(Chnl,
				TRUE,
				tmpRefSizW,
				tmpRefSizH,
				NULL,
				PIXEL_FORMAT_YUV_SEMIPLANAR_420,
				(tmpEEE < HDVIDEO_HD720P25FPS) ? TRUE : FALSE,
				TRUE,
				TRUE);
			VPSS_ViGroupSettingInit(Chnl+1,
				TRUE,
				tmpRefSizW,
				tmpRefSizH,
				NULL,
				PIXEL_FORMAT_YUV_SEMIPLANAR_420,
				(tmpEEE < HDVIDEO_HD720P25FPS) ? TRUE : FALSE,
				TRUE,
				TRUE);
		}
	}

	tmpInputStat[Chnl] = tmpEEE;
	tmpInputStat[Chnl+1] = tmpEEE;
}

static void video_InputInit()
{
    int ViDev, ViChn, ii;
    for(ii = 0; ii < MAX_CAM_CH*2; ii ++) {
        DVR_ASSERT(HI_SYS_SetMemConf(HI_ID_VPSS, ii, 0, (ii < MAX_CAM_CH/4) ?MMZ_ZONE_NAME0 :MMZ_ZONE_NAME1));
    }
	
    ViChnFpsInit();

	{ //Load Channel Mode Values;
		extern Sysenv* g_pstSysEnv;
		nVideoModeAuto = g_pstSysEnv->GetChannelModeAuto(g_pstSysEnv);
		for(ii = 0; ii < ARRAY_SIZE(nVideoModeCur); ii ++) {
			nVideoModeCur[ii] = g_pstSysEnv->GetChannelModes(g_pstSysEnv, ii);
		}
	}

    for(ViChn = 0; ViChn < MAX_CAM_CH; ViChn ++) {
        int  SizeVpssW = RefSizW;
        int  SizeVpssH = RefSizH;
        {  //Enable VIU Memory Space;
            MPP_CHN_S stMppChnVI;

            stMppChnVI.enModId  = HI_ID_VIU;
            stMppChnVI.s32DevId = 0; //For VIU mode, this item must be set to zero
            stMppChnVI.s32ChnId = VIMAP_Get(ViChn);

            DVR_ASSERT(HI_MPI_SYS_SetMemConf(&stMppChnVI, (ViChn < MAX_CAM_CH/2) ?MMZ_ZONE_NAME0 :MMZ_ZONE_NAME1));
        }
		video_InputAutoFit(ViChn);
        {  //Enable VPSS Binding To Vi
            VPSS_ViGroupSettingInit(ViChn + MAX_CAM_CH,
                FALSE,
                RefSizW,
                RefSizH,
				NULL,
                PIXEL_FORMAT_YUV_SEMIPLANAR_420,
                FALSE,
                FALSE,
                TRUE);
        }
    }
    if(MAX_CAM_CH < VIDEO_GetMaxDiv()) {
        for(ii = MAX_CAM_CH; ii < VIDEO_GetMaxDiv(); ii ++) {
            DVR_ASSERT(HI_SYS_SetMemConf(HI_ID_VPSS, ii + MAX_CAM_CH, 0, MMZ_ZONE_NAME1));
            VPSS_ViGroupSettingInit(ii + MAX_CAM_CH, FALSE, RefSizW, RefSizH, NULL,
                PIXEL_FORMAT_YUV_SEMIPLANAR_420, FALSE, FALSE, TRUE);
        }
    }
	for(ii = 0; ii < MAX_CAM_CH; ii ++) {
		(HI_MPI_Sys_BindByForce(HI_ID_VPSS, ii+MAX_CAM_CH, 0,
								HI_ID_VDEC, 0, ii));
	}
    { //For Single Channel Screen View
        DVR_ASSERT(HI_SYS_SetMemConf(HI_ID_VPSS, VIDEO_GetMaxDiv() + MAX_CAM_CH, 0, MMZ_ZONE_NAME0));
    }
}

static void video_InputDestroy()
{
    int ViDev, ViChn, ii;
    {
        VPSS_ViGroupSettingExit(VIDEO_GetMaxDiv() + MAX_CAM_CH);
    }
	for(ii = 0; ii < MAX_CAM_CH*2; ii ++) {
		HI_MPI_Vpss_UnBindVi(ii, (ii%MAX_CAM_CH)*nViChnStp);
	}
    for(ViChn = 0; ViChn < MAX_CAM_CH; ViChn ++) {
        VPSS_ViGroupSettingExit(ViChn);
        VPSS_ViGroupSettingExit(ViChn+MAX_CAM_CH);

        DVR_ASSERT(HI_MPI_VI_DisableChn(ViChn*nViChnStp));
    }
    if(MAX_CAM_CH < VIDEO_GetMaxDiv()) {
        for(ii = MAX_CAM_CH; ii < VIDEO_GetMaxDiv(); ii ++) {
            VPSS_ViGroupSettingExit(ii + MAX_CAM_CH);
        }
    }
    for(ViDev = 0; ViDev < nViDevNum; ViDev +=1) {
        DVR_ASSERT(HI_MPI_VI_DisableDev(ViDev));
    }
}

static void video_Division(int const nVoDev, int const nScrWidth, int nScrHeight, int nDiv, int nPage, int bLive)
{
	int nLocate = 0;
	int nGrid   = 0;
	int ii;
    int tmpW    = 0;
    int tmpH    = 0;

    int nOffsetX = 0;
    int nOffsetY = 0;
    int nAbsSetW = nScrWidth;
    int nAbsSetH = nScrHeight;

	if(-1 == nDiv){
		nDiv = VIDEO_GetMaxDiv();
	}

	switch(nDiv) {
	case 1:  nGrid = 1; break;
	case 4:  nGrid = 2; break;
	case 9:  nGrid = 3; break;
	case 16: nGrid = 4; break;
	case 25: nGrid = 5; break;
	case 36: nGrid = 6; break;
	default:
		return;
	}

#if defined(FORCE_NORMAL_CVBS_OUTPUT)
    if(nVoDev == VO_CVBS_DEVICE) {
        nOffsetX = s_stVideo.ScrnEdges[1].nEdgeX;
        nOffsetY = s_stVideo.ScrnEdges[1].nEdgeY;

        nAbsSetW = s_stVideo.ScrnEdges[1].nEdgeW;
        nAbsSetH = s_stVideo.ScrnEdges[1].nEdgeH;
    }
#endif

    #define ALIGN_BACK(x, a) ((a) * (((x) / (a))))
    for (ii = 0; ii < VIDEO_GetMaxDiv() && ii < VO_MAX_CHN_NUM; ii++) {
        DVR_ASSERT(HI_MPI_VO_ChnHide(nVoDev, ii));
    }

    tmpW = ALIGN_BACK(nAbsSetW/nGrid,  2);
    tmpH = ALIGN_BACK(nAbsSetH/nGrid, 2);

    for (ii = 0, nLocate = 0; ii < VIDEO_GetMaxDiv() && ii < VO_MAX_CHN_NUM; ii++) {
        if(VO_CHN_IS_ONTOP(nDiv, nPage, ii)) {
            VO_CHN_ATTR_S stVoChnAttr;
            memset(&stVoChnAttr, 0, sizeof(stVoChnAttr));
            stVoChnAttr.stRect.s32X       = nOffsetX + tmpW * (nLocate%nGrid);
            stVoChnAttr.stRect.s32Y       = nOffsetY + tmpH * (nLocate/nGrid);
            stVoChnAttr.stRect.u32Width   = tmpW;
            stVoChnAttr.stRect.u32Height  = tmpH;
            stVoChnAttr.u32Priority       = 0;
            stVoChnAttr.bDeflicker        = (nVoDev == VO_CVBS_DEVICE) ?HI_TRUE :HI_FALSE;
#if defined(FORCE_NORMAL_CVBS_OUTPUT)
            stVoChnAttr.bDeflicker        = HI_FALSE;
#endif

            if(nLocate%nGrid == nGrid-1) {
                stVoChnAttr.stRect.u32Width = (nAbsSetW+nOffsetX)-stVoChnAttr.stRect.s32X;
            }
            if(nLocate/nGrid == nGrid-1) {
                stVoChnAttr.stRect.u32Height = (nAbsSetH+nOffsetY)-stVoChnAttr.stRect.s32Y;
            }

            DVR_ASSERT(HI_MPI_VO_SetChnAttr(nVoDev, ii, &stVoChnAttr));
            DVR_ASSERT(HI_MPI_VO_ChnShow(nVoDev, ii));
            nLocate ++;
        }
    }
}

void video_ChnSpeedRate(int Chn, int nRate)
{
	float XCNV[3] = {0.5, 1, 2};
	int nMaxFps = VIDEO_IS_PAL() ?25 :30;
	int i;

	for(i = 0; i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0]); ++i) {
		if(VO_DEV_ID[i] == VO_VGA_DEVICE) {
			HI_S32 hiRet = 0;
			if ((hiRet = HI_MPI_VO_SetChnFrameRate(VO_DEV_ID[i], Chn, nMaxFps*XCNV[nRate])) != HI_SUCCESS) {
				VIDEO_TRACE ("HI_MPI_VO_SetChnFrameRate failed @errcode: 0x%x", hiRet);
			}
		}
	}
}

void video_ChnFrameRate(int Chn, int nFrameRate)
{
	int i;
	if((nFrameRate < 0) || (nFrameRate > 60)) {
		return;
	}

	for(i = 0; (i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0])) && (i < MAX_CAM_CH); ++i) {
#if   !defined(FORCE_NORMAL_CVBS_OUTPUT)
		if(VO_DEV_ID[i] == VO_VGA_DEVICE)
#endif
		{
			DVR_ASSERT(HI_MPI_VO_SetChnFrameRate(VO_DEV_ID[i], Chn, nFrameRate));
		}
	}
}

void video_VoSpeedRate(int nRate)
{
	int i;
	for(i = 0; i < VIDEO_GetMaxDiv(); i ++) {
		video_ChnSpeedRate(i, nRate);
	}
}

void video_ChnStopOutput(int Chn)
{
	int i;

	for(i = 0; i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0]); ++i) {
		if(VO_DEV_ID[i] == VO_VGA_DEVICE) {
			DVR_ASSERT(HI_MPI_VO_ChnPause(VO_DEV_ID[i], Chn));
		}
	}
}

void video_ChnResumeOutput(int Chn)
{
	int i;

	for(i = 0; i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0]); ++i) {
		if(VO_DEV_ID[i] == VO_VGA_DEVICE) {
			DVR_ASSERT(HI_MPI_VO_ChnResume(VO_DEV_ID[i], Chn));
		}
	}
}

void video_VoOutputOpa(int nEnable)
{
	int i;

	for(i = 0; i < VIDEO_GetMaxDiv(); i ++) {
		if(nEnable) {
			video_ChnResumeOutput(i);
		}
		else {
			video_ChnStopOutput(i);
		}
	}
}

static void video_OutputInit()
{
	int i = 0, ii = 0;
	int nMaxFps = VIDEO_IS_PAL() ?25 :30;

	VO_VIDEO_LAYER_ATTR_S stVgaLayerAttr;
	VO_VIDEO_LAYER_ATTR_S stCvbsLayerAttr;

    memset(&stVgaLayerAttr,  0, sizeof(stVgaLayerAttr));
    memset(&stCvbsLayerAttr, 0, sizeof(stCvbsLayerAttr));
	// vga output
	{
		VGA_RESOLUTION const enRes = g_pstSysEnv->GetVGAResolution(g_pstSysEnv);

		stVgaLayerAttr.stImageSize.u32Width  = RefSizW;
		stVgaLayerAttr.stImageSize.u32Height = RefSizH;
		stVgaLayerAttr.stDispRect.s32X = 0;
		stVgaLayerAttr.stDispRect.s32Y = 0;
		switch(enRes) {
		case VGA_800x600:
			stVgaLayerAttr.stDispRect.u32Width  = 800;
			stVgaLayerAttr.stDispRect.u32Height = 600;
			break;
		default:
		case VGA_1024x768:
			stVgaLayerAttr.stDispRect.u32Width  = 1024;
			stVgaLayerAttr.stDispRect.u32Height = 768;
			break;
		case VGA_1280x720:
			stVgaLayerAttr.stDispRect.u32Width  = 1280;
			stVgaLayerAttr.stDispRect.u32Height = 720;
			break;
		case VGA_1920x1080P50Hz:
		case VGA_1920x1080P60Hz:
			stVgaLayerAttr.stDispRect.u32Width  = 1920;
			stVgaLayerAttr.stDispRect.u32Height = 1080;
			break;
		}

		if(stVgaLayerAttr.stImageSize.u32Width > stVgaLayerAttr.stDispRect.u32Width) {
			stVgaLayerAttr.stImageSize.u32Width = stVgaLayerAttr.stDispRect.u32Width;
			s_stVideo.nScrWidth[0] = stVgaLayerAttr.stDispRect.u32Width;
		}
		if(stVgaLayerAttr.stImageSize.u32Height > stVgaLayerAttr.stDispRect.u32Height) {
			stVgaLayerAttr.stImageSize.u32Height = stVgaLayerAttr.stDispRect.u32Height;
			s_stVideo.nScrHeight[0] = stVgaLayerAttr.stDispRect.u32Height;
		}

#if  defined(HD_DISPLAY_SIZE_OPTIMISED)
		stVgaLayerAttr.stImageSize.u32Width  = stVgaLayerAttr.stDispRect.u32Width;
		stVgaLayerAttr.stImageSize.u32Height = stVgaLayerAttr.stDispRect.u32Height;
#if  defined(_EXT_HDDVR_LITE) && (defined(_HI3520D_EXT) || defined(SDK_PLATFORM_HI3531))
		if(stVgaLayerAttr.stImageSize.u32Width > 1440) {
			stVgaLayerAttr.stImageSize.u32Width = 1440;
			stVgaLayerAttr.stImageSize.u32Height = 900;
		}
#endif
		s_stVideo.nScrWidth[0]  = stVgaLayerAttr.stImageSize.u32Width;
		s_stVideo.nScrHeight[0] = stVgaLayerAttr.stImageSize.u32Height;
#endif

		stVgaLayerAttr.u32DispFrmRt = nMaxFps;
		stVgaLayerAttr.enPixFormat  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	}
	// cvbs
	{
		stCvbsLayerAttr.stImageSize.u32Width  = VO_ORIGIN_WIDTH;
		stCvbsLayerAttr.stImageSize.u32Height = VO_ORIGIN_HEIGHT;
		stCvbsLayerAttr.stDispRect.s32X       = 0;
		stCvbsLayerAttr.stDispRect.s32Y       = 0;
		stCvbsLayerAttr.stDispRect.u32Width   = VO_ORIGIN_WIDTH;
		stCvbsLayerAttr.stDispRect.u32Height  = VO_ORIGIN_HEIGHT;
		stCvbsLayerAttr.u32DispFrmRt          = nMaxFps;
		stCvbsLayerAttr.enPixFormat           = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	}
	{
		// configuration active
		const VO_VIDEO_LAYER_ATTR_S astVideoLayerAttr[] = {stVgaLayerAttr, stCvbsLayerAttr};

        video_ScreenInit();

		for(i = 0; i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0]); ++i){

            DVR_ASSERT(HI_MPI_VO_DisableVideoLayer(VO_DEV_ID[i]));

			//DVR_ASSERT(HI_MPI_VO_SetDispBufLen(nDev, MAX_CAM_CH));
			DVR_ASSERT(HI_MPI_VO_SetVideoLayerAttr(VO_DEV_ID[i], &astVideoLayerAttr[i]));
			DVR_ASSERT(HI_MPI_VO_EnableVideoLayer(VO_DEV_ID[i]));
#if !defined(FORCE_NORMAL_CVBS_OUTPUT)
            if(VO_DEV_ID[i] == VO_VGA_DEVICE)
#endif
            {
                // setup division
                s_stVideo.nCurDivi = -1;
                s_stVideo.nCurPage =  0;
                video_Division(VO_DEV_ID[i], s_stVideo.nScrWidth[i], s_stVideo.nScrHeight[i],
                    s_stVideo.nCurDivi, s_stVideo.nCurPage, TRUE);
				for(ii = 0; ii < VIDEO_GetMaxDiv(); ++ii){
					DVR_ASSERT(HI_MPI_VO_EnableChn(VO_DEV_ID[i], ii));
					video_ChnSpeedRate(ii, VO_SPEED_RATE_X1_0); //Sync Speed for Live Output
					if(ii < MAX_CAM_CH){
						video_BindOutput(VO_DEV_ID[i], ii, TRUE);
					}else{
						DVR_ASSERT(HI_MPI_VO_SENDFRAME(VO_DEV_ID[i], ii, s_pstLogoPic));
					}
				}
            }
#if !defined(FORCE_NORMAL_CVBS_OUTPUT)
            if(0) {
                video_Division(VO_DEV_ID[i],
                            astVideoLayerAttr[i].stImageSize.u32Width,
                            astVideoLayerAttr[i].stImageSize.u32Height,
                            1, 0,
                            TRUE);
                DVR_ASSERT(HI_MPI_VO_EnableChn(VO_DEV_ID[i], 0));
                DVR_ASSERT(HI_MPI_VO_SetChnFrameRate(VO_DEV_ID[i], 0, nMaxFps));
                {
                    VO_WBC_ATTR_S stWbcAttr;
                    memset(&stWbcAttr, 0, sizeof(stWbcAttr));
                    stWbcAttr.stTargetSize.u32Width  = astVideoLayerAttr[i].stImageSize.u32Width;
                    stWbcAttr.stTargetSize.u32Height = astVideoLayerAttr[i].stImageSize.u32Height;
                    stWbcAttr.u32FrameRate           = astVideoLayerAttr[i].u32DispFrmRt/2;
                    stWbcAttr.enPixelFormat          = astVideoLayerAttr[i].enPixFormat;
#if (defined(_HI3515A_EXT) || defined(_HI3520D_EXT))
                    stWbcAttr.enDataSource           = VO_WBC_DATASOURCE_VIDEO;
#endif
                    DVR_ASSERT(HI_MPI_VO_SetWbcAttr(VO_VGA_DEVICE, &stWbcAttr));
                    DVR_ASSERT(HI_MPI_VO_SetWbcMode(VO_VGA_DEVICE, VO_WBC_MODE_PROG_TO_INTL));
#if  defined(_HI3520D_RAM_LITE)
                    DVR_ASSERT(HI_MPI_VO_SetWbcDepth(VO_VGA_DEVICE, 1));
#endif
                    DVR_ASSERT(HI_MPI_VO_EnableWbc(VO_VGA_DEVICE));
                    {
                        MPP_CHN_S stSrcChn, stDesChn;

                        stSrcChn.enModId   = HI_ID_VOU;
                        stSrcChn.s32DevId  = VO_VGA_DEVICE;
                        stSrcChn.s32ChnId  = 0;
                        stDesChn.enModId   = HI_ID_VOU;
                        stDesChn.s32DevId  = VO_CVBS_DEVICE;
                        stDesChn.s32ChnId  = 0;
                        DVR_ASSERT(HI_MPI_SYS_Bind(&stSrcChn, &stDesChn));
                    }
                }
            }
#endif
		}
	}
	s_stVideo.bOutput = TRUE;
}

static void video_OutputDestroy()
{
	int i = 0, ii = 0;
	for(i = sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0])-1; i >= 0 ; --i){
#if !defined(FORCE_NORMAL_CVBS_OUTPUT)
        if(VO_DEV_ID[i] == VO_VGA_DEVICE)
#endif
        {
            // enable vo channel
            for(ii = 0; ii < VIDEO_GetMaxDiv(); ++ii){
                DVR_ASSERT(HI_MPI_VO_DisableChn(VO_DEV_ID[i], ii));
            }
        }
#if !defined(FORCE_NORMAL_CVBS_OUTPUT)
        if(0) {
            {
                MPP_CHN_S stSrcChn, stDesChn;

                stSrcChn.enModId   = HI_ID_VOU;
                stSrcChn.s32DevId  = VO_VGA_DEVICE;
                stSrcChn.s32ChnId  = 0;
                stDesChn.enModId   = HI_ID_VOU;
                stDesChn.s32DevId  = VO_CVBS_DEVICE;
                stDesChn.s32ChnId  = 0;
                DVR_ASSERT(HI_MPI_SYS_UnBind(&stSrcChn, &stDesChn));
            }
            DVR_ASSERT(HI_MPI_VO_DisableWbc(VO_VGA_DEVICE));
            DVR_ASSERT(HI_MPI_VO_DisableChn(VO_DEV_ID[i], 0));
        }
#endif
		// disable video layer
		DVR_ASSERT(HI_MPI_VO_DisableVideoLayer(VO_DEV_ID[i]));
	}
	video_ScreenExit();
	s_stVideo.bOutput = FALSE;
}

static int video_DigitalZoom(int nChn, int nRatioX, int nRatioY, int nRatioW, int nRatioH, unsigned char bLive)
{
#define RATIO_CALAC(r, m) ((m)/((typeof(r))(((r) * (m) /1000) /16) *16))
	static unsigned char s_bZoom[MAX_CAM_CH];
	if(nChn < MAX_CAM_CH) {
		if(0 == nRatioX && 0 == nRatioY && 1000 == nRatioW && 1000 == nRatioH){
			return 0;
		}
		if(RATIO_CALAC(nRatioW, RefSizW) >= 16
		|| RATIO_CALAC(nRatioH, RefSizH) >= 16){
			return 0;
		}

		VPSS_CROP_INFO_S CropInfo;
		memset(&CropInfo, 0, sizeof(CropInfo));

		if(!s_bZoom[nChn]) {
			CropInfo.enCropCoordinate = VPSS_CROP_RITIO_COOR;
			CropInfo.enCapSel         = VPSS_CAPSEL_BOTH;
			CropInfo.stCropRect.s32X  = nRatioX;
			CropInfo.stCropRect.s32Y  = nRatioY;
			CropInfo.stCropRect.u32Width  = nRatioW < 32 ?32 :nRatioW;
			CropInfo.stCropRect.u32Height = nRatioH < 32 ?32 :nRatioH;

			CropInfo.bEnable          = HI_TRUE;
			DVR_ASSERT(HI_MPI_VPSS_SetCropCfg(nChn+MAX_CAM_CH, &CropInfo));
			if(bLive && 1 == s_stVideo.nCurDivi && nChn == s_stVideo.nCurPage) {
				DVR_ASSERT(HI_MPI_VPSS_SetCropCfg(VIDEO_GetMaxDiv() + MAX_CAM_CH, &CropInfo));
			}
			s_bZoom[nChn] = TRUE;
		}else{
			CropInfo.stCropRect.u32Width  = 32;
			CropInfo.stCropRect.u32Height = 32;
 			CropInfo.bEnable              = HI_FALSE;
			DVR_ASSERT(HI_MPI_VPSS_SetCropCfg(nChn+MAX_CAM_CH, &CropInfo));
			if(bLive && 1 == s_stVideo.nCurDivi && nChn == s_stVideo.nCurPage) {
				DVR_ASSERT(HI_MPI_VPSS_SetCropCfg(VIDEO_GetMaxDiv() + MAX_CAM_CH, &CropInfo));
			}
			s_bZoom[nChn] = FALSE;
		}
		return 0;
	}
	return -1;
}

static void video_ClearDigitalZoom(int nChn)
{
	if(nChn < MAX_CAM_CH) {
		VPSS_CROP_INFO_S CropInfo;
		memset(&CropInfo, 0, sizeof(CropInfo));

		CropInfo.stCropRect.u32Width  = 32;
		CropInfo.stCropRect.u32Height = 32;
		CropInfo.bEnable              = HI_FALSE;
		DVR_ASSERT(HI_MPI_VPSS_SetCropCfg(nChn+MAX_CAM_CH, &CropInfo));

		memset(&s_stVideo.astDigitalZoomRatio[nChn], 0, sizeof(s_stVideo.astDigitalZoomRatio[nChn]));
		s_stVideo.abDigitalZoom[nChn] = FALSE;
	}
}


