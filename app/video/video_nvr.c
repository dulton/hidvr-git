#define NVR_MAX_WIDTH 1280
#define NVR_MAX_HEIGHT 720

#define VI_MUX_SIZW 704
#define VI_MUX_SIZH (VIDEO_IS_PAL() ? 576 : 480)

static int  video_ParamInit(void)
{
    return 0;
}

static int  video_ParamExit(void)
{
    return 0;
}

void video_send_logo(int nChn)
{
	int i = 0;
	for(i = 0; i < VO_DEV_CNT; ++i){
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
		HI_MPI_VO_ClearChnBuffer(VO_DEV_ID[i], nChn, HI_TRUE);
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
#endif
		HI_MPI_VO_SENDFRAME(VO_DEV_ID[i], nChn, VIDEO_GetUserPic(nChn));
	}
}

static inline void VI_D1_ATTR_CONF(VI_CHN_ATTR_S* pAttr)
{
}

static inline void VI_CIF_ATTR_CONF(VI_CHN_ATTR_S* pAttr)
{
}


static void video_DisplayD1(int nVoDev, int nChn, unsigned char bDisplay)
{
	// dont try to change input the original setting of d1 channel
	if(bDisplay){
		DVR_ASSERT(HI_MPI_VO_SetChnField(nVoDev, nChn, VO_FIELD_BOTH));
		NVRPC_switch_full(nChn);
	}else{
		DVR_ASSERT(HI_MPI_VO_SetChnField(nVoDev, nChn, VO_FIELD_BOTTOM));
		NVRPC_switch_full(-1);
	}

}




static unsigned int video_GetLoss(void)
{
	return NVRPC_get_connection();
}

#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
static void video_InputDestroy()
{
//	VOC_destory(NULL);
	NVRPC_destory();
	AVENCBUF_Destroy();
}
static void video_Division(int const nVoDev, int const nScrWidth, int nScrHeight, int nDiv, int nPage, unsigned char bD1)
{
	int i = 0;
	int nGrid = 1;
	VO_CHN_ATTR_S stVoChnAttr;

	int nLocate = 0;
	int nChWidth = 0;
	int nChHeight = 0;
	int const nOffsetX = (1280 - nScrWidth) / 2;
	int const nOffsetY = (724 - nScrHeight) / 2;
	int nBlankW = 0;
	int nBlankH = 0;

	if(-1 == nDiv){
		nDiv = VIDEO_GetMaxDiv();
	}
	switch(nDiv)
	{
	case 1: nGrid = 1; break;
	case 4: nGrid = 2; break;
	case 9: nGrid = 3; break;
	case 16: nGrid = 4; break;
	case 25: nGrid = 5; break;
	default:
		return;
	}

	nChWidth = nScrWidth / nGrid;
	nChHeight = nScrHeight / nGrid;
	nChWidth &= ~(2-1); // 2 alignment
	nChHeight &= ~(2-1); // 2 alignment
	nBlankW = nScrWidth % nChWidth;
	nBlankH = nScrHeight % nChHeight;
	for(i = 0, nLocate = 0; i < VIDEO_GetMaxDiv(); ++i){
		stVoChnAttr.stRect.s32X = nOffsetX + (((nLocate % nDiv) % nGrid) * nChWidth) & ~(2-1); // 2 alignment
		stVoChnAttr.stRect.s32Y = nOffsetY + (((nLocate % nDiv) / nGrid) * nChHeight) & ~(2-1); // 2 alignment
//		VIDEO_TRACE("vo(%d,%d) (x,y)=%d,%d, [w,h]=%d,%d", nVoDev, i, stVoChnAttr.stRect.s32X, stVoChnAttr.stRect.s32Y, stVoChnAttr.stRect.u32Width, stVoChnAttr.stRect.u32Height);
		if((nGrid - 1) == (nLocate % nGrid)){
			// the right side
			stVoChnAttr.stRect.u32Width = nChWidth + nBlankW;
		}else{
			// for osd bar
			stVoChnAttr.stRect.u32Width = nChWidth - 2;
		}
		if((nGrid - 1) == (nLocate / nGrid)){
			stVoChnAttr.stRect.u32Height = nChHeight + nBlankH;
		}else{
			// for osd bar
			stVoChnAttr.stRect.u32Height = nChHeight - 2;
		}
		stVoChnAttr.u32Priority = 1;
		stVoChnAttr.bZoomEnable = HI_TRUE;
		stVoChnAttr.bDeflicker = (0 == nVoDev) ? HI_FALSE : HI_TRUE;
		DVR_ASSERT(HI_MPI_VO_SetChnAttr(nVoDev, i, &stVoChnAttr));
		if(VO_CHN_IS_ONTOP(nDiv, nPage, i)){
			++nLocate;
			DVR_ASSERT(HI_MPI_VO_ChnShow(nVoDev, i));
		}else{
			DVR_ASSERT(HI_MPI_VO_ChnHide(nVoDev, i));
		}
	}
}

void video_ChnSpeedRate(int Chn, int nRate)
{
}

void video_VoSpeedRate(int nRate)
{
}

void video_ChnStopOutput(int Chn)
{
}

void video_ChnResumeOutput(int Chn)
{
}

void video_VoOutputOpa(int nEnable)
{
}

static void video_OutputInit()
{
	int i = 0, ii = 0;
	int const nMaxFps = MAX_D1_CNT ? MAX_D1_LIVE_FPS : MAX_CIF_LIVE_FPS;

	VO_VIDEO_LAYER_ATTR_S stVgaLayerAttr;
	VO_VIDEO_LAYER_ATTR_S stCvbsLayerAttr;

	// vga output
	{
		VGA_RESOLUTION const enRes = g_pstSysEnv->GetVGAResolution(g_pstSysEnv);

		stVgaLayerAttr.stImageSize.u32Width = 1280;
		stVgaLayerAttr.stImageSize.u32Height = 724;
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
		}

		stVgaLayerAttr.u32DispFrmRt = nMaxFps;
		stVgaLayerAttr.enPixFormat  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		stVgaLayerAttr.s32PiPChn    = VO_DEFAULT_CHN;
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
		stCvbsLayerAttr.s32PiPChn             = VO_DEFAULT_CHN;
	}
	{
		// configuration active
		const VO_VIDEO_LAYER_ATTR_S astVideoLayerAttr[] = {stVgaLayerAttr, stCvbsLayerAttr};

        video_ScreenInit();

		for(i = 0; i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0]); ++i){
			//DVR_ASSERT(HI_MPI_VO_SetDispBufLen(nDev, MAX_CAM_CH));
			DVR_ASSERT(HI_MPI_VO_SetVideoLayerAttr(VO_DEV_ID[i], &astVideoLayerAttr[i]));
			DVR_ASSERT(HI_MPI_VO_EnableVideoLayer(VO_DEV_ID[i]));
			// setup division
			video_Division(VO_DEV_ID[i], s_stVideo.nScrWidth[i], s_stVideo.nScrHeight[i], -1, 0, FALSE);
			// enable vo channel
			for(ii = 0; ii < VIDEO_GetMaxDiv(); ++ii){
				DVR_ASSERT(HI_MPI_VO_SetChnField(VO_DEV_ID[i], ii, VO_FIELD_BOTTOM));
				DVR_ASSERT(HI_MPI_VO_EnableChn(VO_DEV_ID[i], ii));
				if(ii < MAX_CAM_CH){
				}else{
					DVR_ASSERT(HI_MPI_VO_SENDFRAME(VO_DEV_ID[i], ii, s_pstLogoPic));
				}
			}
		}
	}
	s_stVideo.bOutput = TRUE;
}

static void video_InputInit()
{
	AVENCBUF_Init();
	NVRPC_init();
	//	VOC_init(NULL);
}

#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
void VPSS_ViGroupSettingInit(int  nVpssGrp, int  SizW, int  SizH, int  NeedDie, VPSS_CROP_INFO_S * Crop)
{
    {  //Greate VPSS Group;
        VPSS_GRP_ATTR_S stGrpAttr;

        memset(&stGrpAttr, 0, sizeof(stGrpAttr));

        stGrpAttr.u32MaxW   = SizW;
        stGrpAttr.u32MaxH   = SizH;
        stGrpAttr.bDrEn     = HI_FALSE;
        stGrpAttr.bDbEn     = HI_FALSE;
        stGrpAttr.bIeEn     = HI_FALSE;
        stGrpAttr.bNrEn     = HI_FALSE;
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
            DVR_ASSERT(HI_MPI_VPSS_EnableChn(nVpssGrp, ii));
        }
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


static void video_InputInit()
{
    int ViDev, ViChn, ii;
    for(ii = 0; ii < MAX_CAM_CH*2; ii ++) {
        if(ii < MAX_CAM_CH) {
//            DVR_ASSERT(HI_SYS_SetMemConf(HI_ID_VPSS, ii, 0, ii%2 ?MMZ_ZONE_NAME1 :MMZ_ZONE_NAME0));
        }
        else {
            DVR_ASSERT(HI_SYS_SetMemConf(HI_ID_VPSS, ii, 0, MMZ_ZONE_NAME1));
        }
    }
#if   defined(SDK_PLATFORM_HI3521)
    for(ViDev = 0; ViDev < VIU_MAX_DEV_NUM; ViDev +=1) {
#elif defined(SDK_PLATFORM_HI3531)
    for(ViDev = 0; ViDev < VIU_MAX_DEV_NUM; ViDev +=2) {
#endif
    }
    for(ViChn = 0; ViChn < MAX_CAM_CH; ViChn ++) {
        int  SizeVpssW = NVR_MAX_WIDTH;
        int  SizeVpssH = NVR_MAX_HEIGHT;

        {  //Enable VPSS Binding To Vi
            VPSS_ViGroupSettingInit(ViChn + MAX_CAM_CH, SizeVpssW, SizeVpssH, FALSE, NULL);
        }
    }
    if(MAX_CAM_CH < VIDEO_GetMaxDiv()) {
        for(ii = MAX_CAM_CH; ii < VIDEO_GetMaxDiv(); ii ++) {
            DVR_ASSERT(HI_SYS_SetMemConf(HI_ID_VPSS, ii + MAX_CAM_CH, 0, MMZ_ZONE_NAME1));
            VPSS_ViGroupSettingInit(ii + MAX_CAM_CH, VI_MUX_SIZW, VI_MUX_SIZH, FALSE, NULL);
        }
    }

//    AVENCBUF_Init();
    NVRPC_init();
}

static void video_InputDestroy()
{
	int ViDev, ViChn, ii;
	for(ViChn = 0; ViChn < MAX_CAM_CH; ViChn ++) {
		VPSS_ViGroupSettingExit(ViChn+MAX_CAM_CH);
	}
	if(MAX_CAM_CH < VIDEO_GetMaxDiv()) {
		for(ii = MAX_CAM_CH; ii < VIDEO_GetMaxDiv(); ii ++) {
			VPSS_ViGroupSettingExit(ii + MAX_CAM_CH);
		}
	}
}

static void video_Division(int const nVoDev, int nScrWidth, int nScrHeight, int nDiv, int nPage, int bLive)
{
//	nScrWidth = VI_MUX_SIZW;
//	nScrHeight = VI_MUX_SIZH;

	int nLocate = 0;
	int nGrid   = 0;
	int ii;

	if(-1 == nDiv){
		nDiv = VIDEO_GetMaxDiv();
	}

	switch(nDiv) {
	case 1:  nGrid = 1; break;
	case 4:  nGrid = 2; break;
	case 9:  nGrid = 3; break;
	case 16: nGrid = 4; break;
	case 25: nGrid = 5; break;
	default:
		return;
	}

    #define ALIGN_BACK(x, a) ((a) * (((x) / (a))))
    for (ii = 0; ii < VIDEO_GetMaxDiv(); ii++) {
        DVR_ASSERT(HI_MPI_VO_ChnHide(nVoDev, ii));
    }
    for (ii = 0, nLocate = 0; ii < VIDEO_GetMaxDiv(); ii++) {
        if(VO_CHN_IS_ONTOP(nDiv, nPage, ii)) {
            VO_CHN_ATTR_S stVoChnAttr;
            memset(&stVoChnAttr, 0, sizeof(stVoChnAttr));
            stVoChnAttr.stRect.s32X       = ALIGN_BACK((nScrWidth/nGrid)  * (nLocate%nGrid), 2);
            stVoChnAttr.stRect.s32Y       = ALIGN_BACK((nScrHeight/nGrid) * (nLocate/nGrid), 2);
            stVoChnAttr.stRect.u32Width   = ALIGN_BACK(nScrWidth/nGrid,  2)-2;
            stVoChnAttr.stRect.u32Height  = ALIGN_BACK(nScrHeight/nGrid, 2)-2;
            stVoChnAttr.u32Priority       = 0;
            stVoChnAttr.bDeflicker        = (nVoDev == VO_CVBS_DEVICE) ?HI_TRUE :HI_FALSE;

            if(nLocate%nGrid == nGrid-1) {
                stVoChnAttr.stRect.u32Width = (nScrWidth)-stVoChnAttr.stRect.s32X;
            }
            if(nLocate/nGrid == nGrid-1) {
                stVoChnAttr.stRect.u32Height = (nScrHeight)-stVoChnAttr.stRect.s32Y;
            }

            DVR_ASSERT(HI_MPI_VO_SetChnAttr(nVoDev, ii, &stVoChnAttr));
            DVR_ASSERT(HI_MPI_VO_ChnShow(nVoDev, ii));
            nLocate ++;
        }
    }
}

static void video_OutputInit()
{
	int i = 0, ii = 0;
	int const nMaxFps = 30;

	VO_VIDEO_LAYER_ATTR_S stVgaLayerAttr;
	VO_VIDEO_LAYER_ATTR_S stCvbsLayerAttr;

    memset(&stVgaLayerAttr,  0, sizeof(stVgaLayerAttr));
    memset(&stCvbsLayerAttr, 0, sizeof(stCvbsLayerAttr));
	// vga output
	{
		VGA_RESOLUTION const enRes = g_pstSysEnv->GetVGAResolution(g_pstSysEnv);

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
		stVgaLayerAttr.stImageSize.u32Width  = stVgaLayerAttr.stDispRect.u32Width;
		stVgaLayerAttr.stImageSize.u32Height = stVgaLayerAttr.stDispRect.u32Height;

		stVgaLayerAttr.u32DispFrmRt = nMaxFps;
		stVgaLayerAttr.enPixFormat  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;

		s_stVideo.nScrWidth[0] = stVgaLayerAttr.stDispRect.u32Width;
		s_stVideo.nScrHeight[0] = stVgaLayerAttr.stDispRect.u32Height;
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

            if(VO_DEV_ID[i] == VO_VGA_DEVICE) {
                // setup division
                s_stVideo.nCurDivi = -1;
                s_stVideo.nCurPage =  0;
                video_Division(VO_DEV_ID[i], s_stVideo.nScrWidth[i], s_stVideo.nScrHeight[i],
                    s_stVideo.nCurDivi, s_stVideo.nCurPage, TRUE);
				for(ii = 0; ii < VIDEO_GetMaxDiv(); ++ii){
					DVR_ASSERT(HI_MPI_VO_EnableChn(VO_DEV_ID[i], ii));
					if(ii < MAX_CAM_CH){
						video_BindOutput(VO_DEV_ID[i], ii, FALSE);
					}else{
						DVR_ASSERT(HI_MPI_VO_SENDFRAME(VO_DEV_ID[i], ii, s_pstLogoPic));
					}
				}
            }
            if(VO_DEV_ID[i] == VO_CVBS_DEVICE) {
                video_Division(VO_DEV_ID[i],
                            astVideoLayerAttr[i].stImageSize.u32Width,
                            astVideoLayerAttr[i].stImageSize.u32Height,
                            1, 0,
                            TRUE);
                DVR_ASSERT(HI_MPI_VO_EnableChn(VO_DEV_ID[i], 0));
                {
                    VO_WBC_ATTR_S stWbcAttr;
                    memset(&stWbcAttr, 0, sizeof(stWbcAttr));
                    stWbcAttr.stTargetSize.u32Width  = astVideoLayerAttr[i].stImageSize.u32Width;
                    stWbcAttr.stTargetSize.u32Height = astVideoLayerAttr[i].stImageSize.u32Height;
                    stWbcAttr.u32FrameRate           = astVideoLayerAttr[i].u32DispFrmRt/2;
                    stWbcAttr.enPixelFormat          = astVideoLayerAttr[i].enPixFormat;
#if   defined(_HI3515A_EXT) || defined(_HI3520D_EXT)
                    stWbcAttr.enDataSource           = VO_WBC_DATASOURCE_VIDEO;
#endif
                    DVR_ASSERT(HI_MPI_VO_SetWbcAttr(VO_VGA_DEVICE, &stWbcAttr));
                    DVR_ASSERT(HI_MPI_VO_SetWbcMode(VO_VGA_DEVICE, VO_WBC_MODE_PROG_TO_INTL));
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
		}
	}
	s_stVideo.bOutput = TRUE;
}

#endif

static void video_OutputDestroy()
{
	int i = 0, ii = 0;
	for(i = 0; i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0]); ++i){
		// enable vo channel
		for(ii = 0; ii < VIDEO_GetMaxDiv(); ++ii){
			DVR_ASSERT(HI_MPI_VO_DisableChn(VO_DEV_ID[i], ii));
		}
		// disable video layer
		DVR_ASSERT(HI_MPI_VO_DisableVideoLayer(VO_DEV_ID[i]));
	}
	video_ScreenExit();
	s_stVideo.bOutput = FALSE;
}
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
static int video_DigitalZoom(int nChn, int nRatioX, int nRatioY, int nRatioW, int nRatioH, unsigned char bLive)
{
	static unsigned char s_bZoom[MAX_CAM_CH];
	if(nChn < MAX_CAM_CH){
		int i = 0;
		VO_ZOOM_RATIO_S stZoomRatio = {0};
		if(!s_bZoom[nChn]){
			if(0 == nRatioX && 0 == nRatioY && 1000 == nRatioW && 1000 == nRatioH){
				// needless to zoom
				return 0;
			}
			stZoomRatio.u32XRatio = nRatioX;
			stZoomRatio.u32YRatio = nRatioY;
			stZoomRatio.u32WRatio = nRatioW;
			stZoomRatio.u32HRatio = nRatioH;
			for(i = 0; i < sizeof(VO_DEV_ID)/sizeof(VO_DEV_ID[0]); ++i){
				DVR_ASSERT(HI_MPI_VO_SetZoomInRatio(VO_DEV_ID[i], nChn, &stZoomRatio));
			}
			s_bZoom[nChn] = TRUE;
		}else{
			// all zeroes is recover
			memset(&stZoomRatio, 0, sizeof(stZoomRatio));
			for(i = 0; i < sizeof(VO_DEV_ID)/sizeof(VO_DEV_ID[0]); ++i){
				DVR_ASSERT(HI_MPI_VO_SetZoomInRatio(VO_DEV_ID[i], nChn, &stZoomRatio));
			}
			s_bZoom[nChn] = FALSE;
		}
		return 0;
	}
	return -1;
}
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
static int video_DigitalZoom(int nChn, int nRatioX, int nRatioY, int nRatioW, int nRatioH, unsigned char bLive)
{
#define RATIO_CALAC(r, m) ((m)/((typeof(r))(((r) * (m) /1000) /16) *16))
	static unsigned char s_bZoom[MAX_CAM_CH];
	if(nChn < MAX_CAM_CH) {
		if(0 == nRatioX && 0 == nRatioY && 1000 == nRatioW && 1000 == nRatioH){
			return 0;
		}
		if(RATIO_CALAC(nRatioW, s_stVideo.nScrWidth[0]) >= 16
		|| RATIO_CALAC(nRatioH, s_stVideo.nScrHeight[0]) >= 16){
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
			s_bZoom[nChn] = TRUE;
		}else{
			CropInfo.stCropRect.u32Width  = 32;
			CropInfo.stCropRect.u32Height = 32;
 			CropInfo.bEnable              = HI_FALSE;
			DVR_ASSERT(HI_MPI_VPSS_SetCropCfg(nChn+MAX_CAM_CH, &CropInfo));
			s_bZoom[nChn] = FALSE;
		}
		return 0;
	}
	return -1;
}
#endif
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
