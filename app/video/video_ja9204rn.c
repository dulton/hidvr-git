
#ifdef _JA7204
//#define HISILICON_VIDEO_LOSS_IMG_USE
#endif

static inline void VI_D1_ATTR_CONF(VI_CHN_ATTR_S* pAttr)
{
	pAttr->stCapRect.u32Width = 960;
	pAttr->stCapRect.s32X = 0;
	pAttr->enCapSel = VI_CAPSEL_BOTH;
	pAttr->bDownScale = HI_FALSE;
}

static inline void VI_CIF_ATTR_CONF(VI_CHN_ATTR_S* pAttr)
{
	pAttr->stCapRect.u32Width = 960;
	pAttr->stCapRect.s32X = 0;
	pAttr->enCapSel = VI_CAPSEL_BOTTOM;
	pAttr->bDownScale = HI_TRUE;
}

static void video_DisplayD1(int nVoDev, int nChn, unsigned char bDisplay)
{
	static int s_nDispViDev = -1;
	static int s_nDispViChn = -1;
	static int s_nDispVoChn = -1;
	VI_CHN_ATTR_S stViChnAttr;

	// input reset

	// dont try to change input the original setting of d1 channel
	if(bDisplay){
		if(!(-1 == s_nDispViDev && -1 == s_nDispViChn)){
			if(s_nDispVoChn >= MAX_D1_CNT){
				DVR_ASSERT(HI_MPI_VI_GetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
				VI_CIF_ATTR_CONF(&stViChnAttr);
				DVR_ASSERT(HI_MPI_VI_SetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
			}
			DVR_ASSERT(HI_MPI_VO_SetChnField(nVoDev, nChn, VO_FIELD_BOTTOM));
		}
		if(nChn >= MAX_D1_CNT){
			if(0 == VIMAP_Get(nChn, &s_nDispViDev, &s_nDispViChn)){
				DVR_ASSERT(HI_MPI_VI_GetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
				VI_D1_ATTR_CONF(&stViChnAttr);
				DVR_ASSERT(HI_MPI_VI_SetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
				s_nDispVoChn = nChn;
			}
		}
		DVR_ASSERT(HI_MPI_VO_SetChnField(nVoDev, nChn, VO_FIELD_BOTH));
	}else{
		if(!(-1 == s_nDispViDev && -1 == s_nDispViChn)){
			if(nChn >= MAX_D1_CNT){
				// reset field setting
				DVR_ASSERT(HI_MPI_VI_GetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
				VI_CIF_ATTR_CONF(&stViChnAttr);
				DVR_ASSERT(HI_MPI_VI_SetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
				s_nDispViDev = -1;
				s_nDispViChn = -1;
			}
			s_nDispVoChn = -1;
		}
		DVR_ASSERT(HI_MPI_VO_SetChnField(nVoDev, nChn, VO_FIELD_BOTTOM));
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

static void video_InputInit()
{
	int i = 0;
	// init sdk
	{
		int nDev = 0;
		int nChnCnt = 0;
		VI_PUB_ATTR_S stViDevAttr;
		VI_CHN_ATTR_S stViChnAttr;

		// vi0 1-4 ch cif input 4d1
		// vi2 5-8 ch d1 input 4d1

		// device attributes
		stViDevAttr.enInputMode = VI_MODE_BT656;
		stViDevAttr.enWorkMode = VI_WORK_MODE_4D1;
		stViDevAttr.enViNorm = (VIDEO_IS_PAL() ? VIDEO_ENCODING_MODE_PAL : VIDEO_ENCODING_MODE_NTSC);
		// channel attributes
		stViChnAttr.stCapRect.s32Y = 0;
		stViChnAttr.stCapRect.u32Height = (VIDEO_IS_PAL() ? 288 : 240);
		stViChnAttr.bHighPri = HI_FALSE;
		stViChnAttr.bChromaResample = HI_FALSE;
		stViChnAttr.enViPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		VI_CIF_ATTR_CONF(&stViChnAttr);

		////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////
		// vi0
		nDev = 0;
		DVR_ASSERT(HI_MPI_VI_Disable(nDev));
		DVR_ASSERT(HI_MPI_VI_SetPubAttr(nDev, &stViDevAttr));
		DVR_ASSERT(HI_MPI_VI_Enable(nDev));
		for(i = 0; i < 4; ++i){
			if(nChnCnt++ < MAX_D1_CNT){
				VI_D1_ATTR_CONF(&stViChnAttr);
			}else{
				VI_CIF_ATTR_CONF(&stViChnAttr);
			}
			DVR_ASSERT(HI_MPI_VI_SetChnAttr(nDev, i, &stViChnAttr));
			DVR_ASSERT(HI_MPI_VI_EnableChn(nDev, i));
			DVR_ASSERT(HI_MPI_VI_SetSrcFrameRate(nDev, i, MAX_CIF_LIVE_FPS));
			DVR_ASSERT(HI_MPI_VI_SetFrameRate(nDev, i, MAX_CIF_LIVE_FPS));
		}

#if (MAX_CAM_CH > 4)
		////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////
		// vi2
		nDev = 2;
		DVR_ASSERT(HI_MPI_VI_Disable(nDev));
		DVR_ASSERT(HI_MPI_VI_SetPubAttr(nDev, &stViDevAttr));
		DVR_ASSERT(HI_MPI_VI_Enable(nDev));
		for(i = 0; i < 4; ++i){
			if(nChnCnt++ < MAX_D1_CNT){
				VI_D1_ATTR_CONF(&stViChnAttr);
			}else{
				VI_CIF_ATTR_CONF(&stViChnAttr);
			}
			DVR_ASSERT(HI_MPI_VI_SetChnAttr(nDev, i, &stViChnAttr));
			DVR_ASSERT(HI_MPI_VI_EnableChn(nDev, i));
			DVR_ASSERT(HI_MPI_VI_SetSrcFrameRate(nDev, i, MAX_CIF_LIVE_FPS));
			DVR_ASSERT(HI_MPI_VI_SetFrameRate(nDev, i, MAX_CIF_LIVE_FPS));
		}
#endif
	}
}

static void video_InputDestroy()
{
	int nDev = 0;
	int i = 0;
#if (MAX_CAM_CH > 4)
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	// vi2
	nDev = 2;
	for(i = 0; i < 4; ++i){
		DVR_ASSERT(HI_MPI_VI_DisableChn(nDev, i));
	}
	DVR_ASSERT(HI_MPI_VI_Disable(nDev));
#endif
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	// vi0
	nDev = 0;
	for(i = 0; i < 4; ++i){
		DVR_ASSERT(HI_MPI_VI_DisableChn(nDev, i));
	}
	DVR_ASSERT(HI_MPI_VI_Disable(nDev));
}

static void video_Division(int nVoDev, int nScrWidth, int nScrHeight, int nDiv, int nPage, unsigned char bD1)
{
	int i = 0;
	int nGrid = 1;
	VO_CHN_ATTR_S stVoChnAttr;

	int nLocate = 0;
	int nChWidth = 0;
	int nChHeight = 0;
	int nOffsetX = ((nScrWidth > VO_ORIGIN_WIDTH ? 960 : VO_ORIGIN_WIDTH) - nScrWidth) / 2;
	int nOffsetY = (VO_ORIGIN_HEIGHT - nScrHeight) / 2;
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

	if(nVoDev == VO_CVBS_DEVICE) {
		nOffsetX = s_stVideo.ScrnEdges[1].nEdgeX;
		nOffsetY = s_stVideo.ScrnEdges[1].nEdgeY;

		nScrWidth  = s_stVideo.ScrnEdges[1].nEdgeW;
		nScrHeight = s_stVideo.ScrnEdges[1].nEdgeH;
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

static void video_OutputInit()
{
	int i = 0, ii = 0;
	int const nMaxFps = MAX_D1_CNT ? MAX_D1_LIVE_FPS : MAX_CIF_LIVE_FPS;

	VO_VIDEO_LAYER_ATTR_S stVgaLayerAttr;
	VO_VIDEO_LAYER_ATTR_S stCvbsLayerAttr;

	// vga output
	{
		VGA_RESOLUTION const enRes = g_pstSysEnv->GetVGAResolution(g_pstSysEnv);

		stVgaLayerAttr.stImageSize.u32Width = 960;
		stVgaLayerAttr.stImageSize.u32Height = VO_ORIGIN_HEIGHT;
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
					video_BindOutput(VO_DEV_ID[i], ii, TRUE);
				}else{
					DVR_ASSERT(HI_MPI_VO_SENDFRAME(VO_DEV_ID[i], ii, s_pstLogoPic));
				}
			}
		}
	}
	s_stVideo.bOutput = TRUE;
}

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

static int video_DigitalZoom(int nChn, int nRatioX, int nRatioY, int nRatioW, int nRatioH, unsigned char bLive	)
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

static void video_ClearDigitalZoom(int nChn)
{
	int i = 0;
	VO_ZOOM_RATIO_S stZoomRatio;
	VO_ZOOM_ATTR_S stZoomRect;
	for(i = 0; i < sizeof(VO_DEV_ID)/sizeof(VO_DEV_ID[0]); ++i){
		memset(&stZoomRatio, 0, sizeof(stZoomRatio));
		DVR_ASSERT(HI_MPI_VO_SetZoomInRatio(VO_DEV_ID[i], nChn, &stZoomRatio));
		DVR_ASSERT(HI_MPI_VO_GetZoomInWindow(VO_DEV_ID[i], nChn, &stZoomRect));
		memset(&stZoomRect.stZoomRect, 0, sizeof(stZoomRect.stZoomRect));
		DVR_ASSERT(HI_MPI_VO_SetZoomInWindow(VO_DEV_ID[i], nChn, &stZoomRect));
	}
	memset(&s_stVideo.astDigitalZoomRatio[nChn], 0, sizeof(s_stVideo.astDigitalZoomRatio[nChn]));
	s_stVideo.abDigitalZoom[nChn] = FALSE;
}


