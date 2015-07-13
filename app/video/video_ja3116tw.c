static int  RefSizW;
static int  RefSizH;

static int  video_ParamInit(void)
{
    RefSizW = VIDEO_IS_960() ?VRES_SIZE_W_EF :VRES_SIZE_W_SD;
    RefSizH = VIDEO_IS_PAL() ?VRES_SIZE_H_PL :VRES_SIZE_H_NT;

    return 0;
}

static int  video_ParamExit(void)
{
    return 0;
}

static void video_DisplayD1(int nVoDev, int nChn, unsigned char bDisplay)
{
	static int s_nLstViChn = -1;
	VI_CHN_ATTR_S tmpChnAttr;

	int tmpVpss = VIDEO_GetMaxDiv() + MAX_CAM_CH;

	if(bDisplay) {
		if(s_nLstViChn == -1) {
#if (defined(_HI3515A_EXT) || defined(_HI3520D_EXT)) //Vpss Image Enhanment Enable Only On Hi3515A/C & Hi3520D Platform
			VPSS_ViGroupSettingInit(tmpVpss, RefSizW, RefSizH, TRUE, NULL, TRUE);
#else
			VPSS_ViGroupSettingInit(tmpVpss, RefSizW, RefSizH, TRUE, NULL, FALSE);
#endif
			DVR_ASSERT(HI_MPI_VPSS_DisableBackupFrame(tmpVpss));
		}

		if(s_nLstViChn >= MAX_D1_CNT) {
			DVR_ASSERT(HI_MPI_VI_GetChnAttr(s_nLstViChn, &tmpChnAttr));
			tmpChnAttr.stDestSize.u32Width  = RefSizW/2;
			tmpChnAttr.stDestSize.u32Height = RefSizH/2;
			tmpChnAttr.enCapSel             = VI_CAPSEL_BOTTOM;
			DVR_ASSERT(HI_MPI_VI_SetChnAttr(s_nLstViChn, &tmpChnAttr));
		}
		s_nLstViChn = nChn;
		if(s_nLstViChn >= MAX_D1_CNT) {
			DVR_ASSERT(HI_MPI_VI_GetChnAttr(s_nLstViChn, &tmpChnAttr));
			tmpChnAttr.stDestSize.u32Width  = RefSizW;
			tmpChnAttr.stDestSize.u32Height = RefSizH;
			tmpChnAttr.enCapSel             = VI_CAPSEL_BOTH;
			DVR_ASSERT(HI_MPI_VI_SetChnAttr(s_nLstViChn, &tmpChnAttr));
		}

		{
			VPSS_CROP_INFO_S CropInfo;
			DVR_ASSERT(HI_MPI_VPSS_GetCropCfg(s_nLstViChn+MAX_CAM_CH, &CropInfo));
			if(CropInfo.stCropRect.u32Width < 32) {
				CropInfo.stCropRect.u32Width = 32;
			}
			if(CropInfo.stCropRect.u32Height < 32) {
				CropInfo.stCropRect.u32Height = 32;
			}
			DVR_ASSERT(HI_MPI_VPSS_SetCropCfg(tmpVpss, &CropInfo));
		}
		(HI_MPI_Sys_BindByForce(HI_ID_VPSS, tmpVpss, 0,
								HI_ID_VIU, 0, s_nLstViChn));
	}else {
		if(s_nLstViChn >= MAX_D1_CNT) {
			DVR_ASSERT(HI_MPI_VI_GetChnAttr(s_nLstViChn, &tmpChnAttr));
			tmpChnAttr.stDestSize.u32Width  = RefSizW/2;
			tmpChnAttr.stDestSize.u32Height = RefSizH/2;
			tmpChnAttr.enCapSel             = VI_CAPSEL_BOTTOM;
			DVR_ASSERT(HI_MPI_VI_SetChnAttr(s_nLstViChn, &tmpChnAttr));
		}

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
#if   defined(_HI3515A_EXT) || defined(_HI3520D_EXT)
        0xFF000000, 0xFF0000, 0xFF000000, 0xFF0000,
#else
        0xFF, 0xFF00, 0xFF0000, 0xFF000000,
#endif
#elif defined(SDK_PLATFORM_HI3531)
        0xFF000000, 0xFF00, 0xFF0000, 0xFF,
#endif
    };

    pstDevAttr->au32CompMask[0] = ViMaskTBL[ViDev % ARRAY_SIZE(ViMaskTBL)];
}

void VPSS_ViGroupSettingInit(int  nVpssGrp, int  SizW, int  SizH, int  NeedDie, VPSS_CROP_INFO_S * Crop, int NeedIE)
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
        stGrpAttr.enPixFmt  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;

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
            || (ii == VPSS_PRE1_CHN)
                ?HI_TRUE
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
#if defined(FORCE_NORMAL_CVBS_OUTPUT)
            if(VPSS_PRE1_CHN == ii) {
                VPSS_CHN_MODE_S tmpChnMode;

                DVR_ASSERT(HI_MPI_VPSS_GetChnMode(nVpssGrp, ii, &tmpChnMode));
                tmpChnMode.enChnMode = VPSS_CHN_MODE_USER;
                tmpChnMode.u32Width  = SizW;
                tmpChnMode.u32Height = SizH;
                tmpChnMode.bDouble   = HI_FALSE;
                tmpChnMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
                DVR_ASSERT(HI_MPI_VPSS_SetChnMode(nVpssGrp, ii, &tmpChnMode));
            }
#endif
            DVR_ASSERT(HI_MPI_VPSS_EnableChn(nVpssGrp, ii));
#if   defined(SDK_PLATFORM_HI3521) && (!defined(_HI3520D_EXT) && !defined(_HI3515A_EXT))
            if(NeedIE) {
                VPSS_CHN_NR_PARAM_S tmpNrParam;

                DVR_ASSERT(HI_MPI_VPSS_GetChnNrParam(nVpssGrp, ii, &tmpNrParam));
                tmpNrParam.u32SfStrength = 40;
                tmpNrParam.u32TfStrength = 10;
                DVR_ASSERT(HI_MPI_VPSS_SetChnNrParam(nVpssGrp, ii, &tmpNrParam));
            }
#endif
        }
    }

    if(NeedIE) {
#if   defined(_HI3520D_EXT) || defined(_HI3515A_EXT)
        VPSS_GRP_PARAM_S tmpGrpParam;

        DVR_ASSERT(HI_MPI_VPSS_GetGrpParam(nVpssGrp, &tmpGrpParam));
        tmpGrpParam.u32Contrast   = 75;
        tmpGrpParam.u32IeStrength = 1;
        tmpGrpParam.u32IeSharp    = 16;
        tmpGrpParam.u32SfStrength = 64;
        tmpGrpParam.u32TfStrength = 16;
        DVR_ASSERT(HI_MPI_VPSS_SetGrpParam(nVpssGrp, &tmpGrpParam));
#endif
    }

    DVR_ASSERT(HI_MPI_VPSS_StartGrp(nVpssGrp));
}

void VPSS_ViGroupSettingExit(int  nVpssGrp)
{
    DVR_ASSERT(HI_MPI_VPSS_StopGrp(nVpssGrp));

    {  //Diable All VPSS Channels
        int ii;
        for(ii = 0; ii < VPSS_MAX_CHN_NUM; ii ++) {
            DVR_ASSERT(HI_MPI_VPSS_DisableChn(nVpssGrp, ii));
        }
    }

    DVR_ASSERT(HI_MPI_VPSS_DestroyGrp(nVpssGrp));
}

VI_DEV_ATTR_S DEV_ATTR_BT656D1_4MUX =
{
    /*接口模式*/
    VI_MODE_BT656,
    /*1、2、4路工作模式*/
    VI_WORK_MODE_4Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},
    /*逐行or隔行输入*/
    VI_SCAN_INTERLACED,
    /*AdChnId*/
    {-1, -1, -1, -1}
};
int  nViDevNum = VIU_MAX_DEV_NUM;

static void video_InputInit()
{
    int ViDev, ViChn, ii;
    for(ii = 0; ii < MAX_CAM_CH*2; ii ++) {
        if(ii < MAX_CAM_CH/2) {
            DVR_ASSERT(HI_SYS_SetMemConf(HI_ID_VPSS, ii, 0, ii%2 ?MMZ_ZONE_NAME1 :MMZ_ZONE_NAME0));
        }
        else {
            DVR_ASSERT(HI_SYS_SetMemConf(HI_ID_VPSS, ii, 0, MMZ_ZONE_NAME1));
        }
    }

#if   defined(_HI3515A_EXT) || defined(_HI3520D_EXT)
    nViDevNum = MAX_CAM_CH/4;
#endif

#if   defined(SDK_PLATFORM_HI3521)
    for(ViDev = 0; ViDev < nViDevNum; ViDev +=1) {
#elif defined(SDK_PLATFORM_HI3531)
    for(ViDev = 0; ViDev < nViDevNum; ViDev +=2) {
#endif
        {  //Enable Vi Device;
            VI_DEV_ATTR_S tmpViDevAttr;

            memcpy(&tmpViDevAttr, &DEV_ATTR_BT656D1_4MUX, sizeof(tmpViDevAttr));
            VI_MUX_SetMask(ViDev, &tmpViDevAttr);
#if   defined(_HI3515A_EXT) || defined(_HI3520D_EXT)
            if(GPIODRV_GetVerFix() == GPIO_PROBE_VER1) {
                tmpViDevAttr.bDataRev = HI_TRUE;
            }
#endif

            DVR_ASSERT(HI_MPI_VI_SetDevAttr(ViDev, &tmpViDevAttr));
            DVR_ASSERT(HI_MPI_VI_EnableDev(ViDev));
        }
    }
    for(ViChn = 0; ViChn < MAX_CAM_CH; ViChn ++) {
        int  SizeVpssW = (ViChn < MAX_D1_CNT) ?RefSizW :RefSizW/2;
        int  SizeVpssH = (ViChn < MAX_D1_CNT) ?RefSizH :RefSizH/2;
        {  //Enable VIU Memory Space;
            MPP_CHN_S stMppChnVI;

            stMppChnVI.enModId  = HI_ID_VIU;
            stMppChnVI.s32DevId = 0; //For VIU mode, this item must be set to zero
            stMppChnVI.s32ChnId = ViChn;

            DVR_ASSERT(HI_MPI_SYS_SetMemConf(&stMppChnVI, (ViChn < MAX_CAM_CH/2) && (ViChn%2) ?MMZ_ZONE_NAME0 :MMZ_ZONE_NAME1));
        }
        {  //Enable VIU Channel Attribute;
            VI_CHN_ATTR_S tmpChnAttr;

            tmpChnAttr.stCapRect.s32X       = 0;
            tmpChnAttr.stCapRect.s32Y       = 0;
            tmpChnAttr.stCapRect.u32Width   = RefSizW;
            tmpChnAttr.stCapRect.u32Height  = RefSizH;
            tmpChnAttr.enCapSel             = 
                ((RefSizW == SizeVpssW) && ((RefSizH == SizeVpssH)))
                ?VI_CAPSEL_BOTH
                :VI_CAPSEL_BOTTOM;
            tmpChnAttr.stDestSize.u32Width  = SizeVpssW;
            tmpChnAttr.stDestSize.u32Height = SizeVpssH;
            tmpChnAttr.enPixFormat          = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
            tmpChnAttr.bMirror              = HI_FALSE;
            tmpChnAttr.bFlip                = HI_FALSE;

            tmpChnAttr.bChromaResample      = HI_FALSE;
            tmpChnAttr.s32SrcFrameRate      = VIDEO_IS_PAL() ?25 :30;
            tmpChnAttr.s32FrameRate         = VIDEO_IS_PAL() ?25 :30;

            DVR_ASSERT(HI_MPI_VI_SetChnAttr(ViChn, &tmpChnAttr));
            DVR_ASSERT(HI_MPI_VI_EnableChn(ViChn));
        }
        {  //Enable VPSS Binding To Vi
            int tmpSizW = VRES_SIZE_W_SD;
            int tmpSizH = VRES_SIZE_H_PL;

#if   defined(_EXT_EFFIO)
            tmpSizW = VRES_SIZE_W_EF;
#endif

#if   defined(_HI3515A_EXT) || defined(_HI3520D_EXT)
            if(ViChn >= MAX_D1_CNT) {
                tmpSizW /= 2;
                tmpSizH /= 2;
            }
#endif
            VPSS_ViGroupSettingInit(ViChn,
                tmpSizW,
                tmpSizH,
                (ViChn < MAX_D1_CNT) ?TRUE :FALSE,
                NULL,
                FALSE);
            DVR_ASSERT(HI_MPI_VPSS_DisableBackupFrame(ViChn));
            VPSS_ViGroupSettingInit(ViChn + MAX_CAM_CH,
                tmpSizW,
                tmpSizH,
                FALSE,
                NULL,
                FALSE);
#if   !defined(SDK_PLATFORM_HI3531)
			{
				VPSS_PRESCALE_INFO_S tmpPreScale;

				memset(&tmpPreScale, 0, sizeof(tmpPreScale));

				tmpPreScale.bPreScale = HI_TRUE;
				tmpPreScale.enCapSel  =
					((RefSizW == SizeVpssW) && ((RefSizH == SizeVpssH)))
					?VPSS_CAPSEL_BOTH
					:VPSS_CAPSEL_BOTTOM;
				tmpPreScale.stDestSize.u32Width  = SizeVpssW;
				tmpPreScale.stDestSize.u32Height = SizeVpssH;

				DVR_ASSERT(HI_MPI_VPSS_SetPreScale(ViChn, &tmpPreScale));
				DVR_ASSERT(HI_MPI_VPSS_SetPreScale(ViChn + MAX_CAM_CH, &tmpPreScale));
			}
#endif
        }
    }
    if(MAX_CAM_CH < VIDEO_GetMaxDiv()) {
        for(ii = MAX_CAM_CH; ii < VIDEO_GetMaxDiv(); ii ++) {
            DVR_ASSERT(HI_SYS_SetMemConf(HI_ID_VPSS, ii + MAX_CAM_CH, 0, MMZ_ZONE_NAME1));
            VPSS_ViGroupSettingInit(ii + MAX_CAM_CH, RefSizW, RefSizH, FALSE, NULL, FALSE);
        }
    }
	for(ii = 0; ii < MAX_CAM_CH; ii ++) {
		(HI_MPI_Sys_BindByForce(HI_ID_VPSS, ii, 0,
								HI_ID_VIU, 0, ii));
		(HI_MPI_Sys_BindByForce(HI_ID_VPSS, ii+MAX_CAM_CH, 0,
								HI_ID_VDEC, 0, ii));
	}
    { //For Single Channel Screen View
        DVR_ASSERT(HI_SYS_SetMemConf(HI_ID_VPSS, VIDEO_GetMaxDiv() + MAX_CAM_CH, 0, MMZ_ZONE_NAME1));
    }
}

static void video_InputDestroy()
{
    int ViDev, ViChn, ii;
    {
        VPSS_ViGroupSettingExit(VIDEO_GetMaxDiv() + MAX_CAM_CH);
    }
	for(ii = 0; ii < MAX_CAM_CH*2; ii ++) {
		HI_MPI_Vpss_UnBindVi(ii, ii%MAX_CAM_CH);
	}
    for(ViChn = 0; ViChn < MAX_CAM_CH; ViChn ++) {
        VPSS_ViGroupSettingExit(ViChn);
        VPSS_ViGroupSettingExit(ViChn+MAX_CAM_CH);

        DVR_ASSERT(HI_MPI_VI_DisableChn(ViChn));
    }
    if(MAX_CAM_CH < VIDEO_GetMaxDiv()) {
        for(ii = MAX_CAM_CH; ii < VIDEO_GetMaxDiv(); ii ++) {
            VPSS_ViGroupSettingExit(ii + MAX_CAM_CH);
        }
    }
#if   defined(SDK_PLATFORM_HI3521)
    for(ViDev = 0; ViDev < nViDevNum; ViDev +=1) {
#elif defined(SDK_PLATFORM_HI3531)
    for(ViDev = 0; ViDev < nViDevNum; ViDev +=2) {
#endif
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
#if defined(FORCE_NORMAL_CVBS_OUTPUT) || defined(_HI3515A_EXT) || defined(_HI3520D_EXT)
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
#if   !defined(FORCE_NORMAL_CVBS_OUTPUT)
		if(VO_DEV_ID[i] == VO_VGA_DEVICE)
#endif
		{
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

	for(i = 0; i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0]); ++i) {
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
#if   !defined(FORCE_NORMAL_CVBS_OUTPUT)
		if(VO_DEV_ID[i] == VO_VGA_DEVICE)
#endif
		{
			DVR_ASSERT(HI_MPI_VO_ChnPause(VO_DEV_ID[i], Chn));
		}
	}
}

void video_ChnResumeOutput(int Chn)
{
	int i;

	for(i = 0; i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0]); ++i) {
#if   !defined(FORCE_NORMAL_CVBS_OUTPUT)
		if(VO_DEV_ID[i] == VO_VGA_DEVICE)
#endif
		{
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
		case VGA_1280x1024:
			stVgaLayerAttr.stDispRect.u32Width  = 1280;
			stVgaLayerAttr.stDispRect.u32Height = 1024;
			break;
		case VGA_1366x768:
			stVgaLayerAttr.stDispRect.u32Width  = 1366;
			stVgaLayerAttr.stDispRect.u32Height = 768;
			break;
		case VGA_1440x900:
			stVgaLayerAttr.stDispRect.u32Width  = 1440;
			stVgaLayerAttr.stDispRect.u32Height = 900;
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
		s_stVideo.nScrWidth[0]  = stVgaLayerAttr.stDispRect.u32Width;
		s_stVideo.nScrHeight[0] = stVgaLayerAttr.stDispRect.u32Height;
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
					if(ii < MAX_CAM_CH){
						video_BindOutput(VO_DEV_ID[i], ii, TRUE);
					}else{
						DVR_ASSERT(HI_MPI_VO_SENDFRAME(VO_DEV_ID[i], ii, s_pstLogoPic));
					}
				}
            }
#if !defined(FORCE_NORMAL_CVBS_OUTPUT)
            if(VO_DEV_ID[i] == VO_CVBS_DEVICE) {
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
        if(VO_DEV_ID[i] == VO_CVBS_DEVICE) {
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


