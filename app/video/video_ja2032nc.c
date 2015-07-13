
#define VI_CIF_WIDTH (352)
#define VI_CIF_HEIGHT (VIDEO_IS_PAL() ? 288 : 240)
#define SPECIAL_D1_CH  36//VIDEO_GetMaxDiv() // 36 for d1 output

static inline void VI_D1_ATTR_CONF(VI_CHN_ATTR_S* pAttr)
{
	pAttr->stCapRect.u32Width = 704;
	pAttr->stCapRect.s32X = 0;
	pAttr->enCapSel = VI_CAPSEL_BOTH;
	pAttr->bDownScale = HI_FALSE;
}

static inline void VI_CIF_ATTR_CONF(VI_CHN_ATTR_S* pAttr)
{
	pAttr->stCapRect.u32Width = 704;
	pAttr->stCapRect.s32X = 0;
	pAttr->enCapSel = VI_CAPSEL_BOTTOM;
	pAttr->bDownScale = HI_TRUE;
}

static void video_DisplayD1(int nVoDev, int nChn, unsigned char bDisplay)
{
	static int s_nDispViDev = -1;
	static int s_nDispViChn = -1;
	VI_CHN_ATTR_S stViChnAttr;
	if(bDisplay){
		int i = 0;
		for(i = 0; i < VIDEO_GetMaxDiv(); ++i){
			DVR_ASSERT(HI_MPI_VO_ChnHide(nVoDev, i));
		}
	}
	if(bDisplay){
		static int s_nLastFull = -1; // prevent to bind twice
		if(-1 == s_nLastFull || nChn != s_nLastFull){
			int fd = open(VIDEO_DEV, O_RDWR);
//			VIDEO_TRACE("%s fd = %d,nChn = %d", __func__, fd,nChn);
			if(fd > 0){
				// set ad configuration
				video_preview_t stPreview = {0};
				stPreview.chn = nChn; // switch to this channel @ ad
				ioctl(fd, VIDEO_DRV_SET_LIVE_CHN, &stPreview);
				close(fd);
				fd = -1;
			}
		}
		// unbind last full channel first
		if(!(-1 == s_nDispViDev && -1 == s_nDispViChn)){
			DVR_ASSERT(HI_MPI_VI_UnBindOutput(s_nDispViDev, s_nDispViChn, nVoDev, SPECIAL_D1_CH));
		}
		// bind to d1 special live connect
		s_nDispViDev = nChn < 16 ? 1 : 3;
		s_nDispViChn = 0;
		DVR_ASSERT(HI_MPI_VO_ClearChnBuffer(nVoDev, SPECIAL_D1_CH, HI_TRUE));
		if(s_stVideo.nLossStat & (1<<nChn)){
			DVR_ASSERT(HI_MPI_VO_SENDFRAME(nVoDev, SPECIAL_D1_CH, VIDEO_GetUserPic(nChn)));
		}else{
			DVR_ASSERT(HI_MPI_VI_GetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
			VI_D1_ATTR_CONF(&stViChnAttr);
			DVR_ASSERT(HI_MPI_VI_SetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
			DVR_ASSERT(HI_MPI_VI_BindOutput(s_nDispViDev, s_nDispViChn, nVoDev, SPECIAL_D1_CH));
		}
		usleep(40000);
		DVR_ASSERT(HI_MPI_VO_ChnShow(nVoDev, SPECIAL_D1_CH));
	}else{
		if(!(-1 == s_nDispViDev && -1 == s_nDispViChn)){
			DVR_ASSERT(HI_MPI_VO_ChnHide(nVoDev, SPECIAL_D1_CH));
//			printf("unbind output (%d,%d) to (%d,%d)", s_nDispViDev, s_nDispViChn, nVoDev, SPECIAL_D1_CH);
			DVR_ASSERT(HI_MPI_VI_UnBindOutput(s_nDispViDev, s_nDispViChn, nVoDev, SPECIAL_D1_CH));
			DVR_ASSERT(HI_MPI_VI_GetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
			VI_CIF_ATTR_CONF(&stViChnAttr);
			DVR_ASSERT(HI_MPI_VI_SetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
			s_nDispViDev = -1;
			s_nDispViChn = -1;
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

static void video_InputInit()
{
	int i = 0;
	// init sdk
	{
		VI_PUB_ATTR_S stViDevAttr;
		VI_CHN_ATTR_S stViChnAttr;

		// vi0 17-32 ch cif input 1d1
            // vi1 [17-32] ch d1 live
		// vi2 01-16 ch cif input 1d1
            // vi3 [01-16] ch d1 live
		////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////
		// vi0 & vi2 must 16cif-big-frame
		stViDevAttr.enInputMode = VI_MODE_BT656;
		stViDevAttr.enWorkMode = VI_WORK_MODE_1D1;
		stViDevAttr.enViNorm = (VIDEO_IS_PAL() ? VIDEO_ENCODING_MODE_PAL : VIDEO_ENCODING_MODE_NTSC);
		// channel attribute
		stViChnAttr.stCapRect.s32X = 0;
		stViChnAttr.stCapRect.s32Y = 0;
		stViChnAttr.stCapRect.u32Width = VI_CIF_WIDTH;
		stViChnAttr.stCapRect.u32Height = 8 * VI_CIF_HEIGHT + 7 * 2; // half field height combo frame
		stViChnAttr.bChromaResample = HI_FALSE;
		stViChnAttr.enCapSel = VI_CAPSEL_BOTH;
		stViChnAttr.bDownScale = HI_FALSE;
		stViChnAttr.bHighPri = HI_FALSE;
		stViChnAttr.enViPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
            // vi0
		DVR_ASSERT(HI_MPI_VI_Disable(0));
		DVR_ASSERT(HI_MPI_VI_SetPubAttr(0, &stViDevAttr));
		DVR_ASSERT(HI_MPI_VI_Enable(0));
		DVR_ASSERT(HI_MPI_VI_SetChnAttr(0, 0, &stViChnAttr));
		DVR_ASSERT(HI_MPI_VI_EnableChn(0, 0));
		DVR_ASSERT(HI_MPI_VI_SetSrcFrameRate(0, 0, MAX_CIF_LIVE_FPS));
            // vi2
		DVR_ASSERT(HI_MPI_VI_Disable(2));
		DVR_ASSERT(HI_MPI_VI_SetPubAttr(2, &stViDevAttr));
		DVR_ASSERT(HI_MPI_VI_Enable(2));
		DVR_ASSERT(HI_MPI_VI_SetChnAttr(2, 0, &stViChnAttr));
		DVR_ASSERT(HI_MPI_VI_EnableChn(2, 0));
		DVR_ASSERT(HI_MPI_VI_SetSrcFrameRate(2, 0, MAX_CIF_LIVE_FPS));

		////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////
		// vi1 & vi3 must be D1
		stViChnAttr.enCapSel = VI_CAPSEL_BOTH;
		stViChnAttr.bDownScale = HI_FALSE;
		stViChnAttr.stCapRect.s32Y = 0;
		stViChnAttr.stCapRect.u32Height = VIDEO_IS_PAL() ? 288 : 240;
		stViChnAttr.bChromaResample = HI_FALSE;
		stViChnAttr.bHighPri = HI_FALSE;
		VI_CIF_ATTR_CONF(&stViChnAttr);

		// vi1
		DVR_ASSERT(HI_MPI_VI_Disable(1));
		DVR_ASSERT(HI_MPI_VI_SetPubAttr(1, &stViDevAttr));
		DVR_ASSERT(HI_MPI_VI_Enable(1));
		DVR_ASSERT(HI_MPI_VI_SetChnAttr(1, 0, &stViChnAttr));
		DVR_ASSERT(HI_MPI_VI_EnableChn(1, 0));
		DVR_ASSERT(HI_MPI_VI_SetSrcFrameRate(1, 0, MAX_D1_LIVE_FPS));
		// vi3
		DVR_ASSERT(HI_MPI_VI_Disable(3));
		DVR_ASSERT(HI_MPI_VI_SetPubAttr(3, &stViDevAttr));
		DVR_ASSERT(HI_MPI_VI_Enable(3));
		DVR_ASSERT(HI_MPI_VI_SetChnAttr(3, 0, &stViChnAttr));
		DVR_ASSERT(HI_MPI_VI_EnableChn(3, 0));
		DVR_ASSERT(HI_MPI_VI_SetSrcFrameRate(3, 0, MAX_D1_LIVE_FPS));
      }
}

static void video_InputDestroy()
{
	// vi0
	DVR_ASSERT(HI_MPI_VI_DisableChn(0, 0));
	DVR_ASSERT(HI_MPI_VI_Disable(0));
	// vi1
	DVR_ASSERT(HI_MPI_VI_DisableChn(1, 0));
	DVR_ASSERT(HI_MPI_VI_Disable(1));
	// vi2
	DVR_ASSERT(HI_MPI_VI_DisableChn(2, 0));
	DVR_ASSERT(HI_MPI_VI_Disable(2));
	// vi3
	DVR_ASSERT(HI_MPI_VI_DisableChn(3, 0));
	DVR_ASSERT(HI_MPI_VI_Disable(3));
}

static void video_Division(int nVoDev, int nScrWidth, int nScrHeight, int nDiv, int nPage, unsigned char bLive)
{
	int i = 0;
	int nGrid = 1;
	VO_CHN_ATTR_S stVoChnAttr;
	int nLocate = 0;
	int nChWidth = 0;
	int nChHeight = 0;
	int nOffsetX = (VO_ORIGIN_WIDTH - nScrWidth) / 2;
	int nOffsetY = (VO_ORIGIN_HEIGHT - nScrHeight) / 2;
	int nBlankW = 0;
	int nBlankH = 0;
	int nBeginCh = nDiv * nPage;
	int nEndCh = VIDEO_GetMaxDiv();

//	if(!bD1 && 1 == nDiv){
//		// full screen is ready all the time at this solution
//		return;
//	}

	switch(nDiv)
	{
	case 1: nGrid = 1; break;
	case 4: nGrid = 2; break;
	case 9: nGrid = 3; break;
	case 16: nGrid = 4; break;
	case 25: nGrid = 5; break;
    case 36: nGrid = 6; break;
	default:
		return;
	}

	if(nVoDev == VO_CVBS_DEVICE) {
		nOffsetX = s_stVideo.ScrnEdges[1].nEdgeX;
		nOffsetY = s_stVideo.ScrnEdges[1].nEdgeY;

		nScrWidth  = s_stVideo.ScrnEdges[1].nEdgeW;
		nScrHeight = s_stVideo.ScrnEdges[1].nEdgeH;
	}

	// full d1 for live only
	stVoChnAttr.stRect.s32X = nOffsetX;
	stVoChnAttr.stRect.s32Y = nOffsetY;
	stVoChnAttr.stRect.u32Width = nScrWidth;
	stVoChnAttr.stRect.u32Height = nScrHeight;
	stVoChnAttr.u32Priority = 1;
	stVoChnAttr.bZoomEnable = HI_TRUE;
	stVoChnAttr.bDeflicker = HI_FALSE;
	DVR_ASSERT(HI_MPI_VO_SetChnAttr(nVoDev, SPECIAL_D1_CH, &stVoChnAttr));
	if(1 == nDiv && bLive){
		DVR_ASSERT(HI_MPI_VO_ChnShow(nVoDev, SPECIAL_D1_CH));
	}else{
		DVR_ASSERT(HI_MPI_VO_ChnHide(nVoDev, SPECIAL_D1_CH));
	}

	nChWidth = nScrWidth / nGrid;
	nChHeight = nScrHeight / nGrid;
	nChWidth &= ~(2-1); // 2 alignment
	nChHeight &= ~(2-1); // 2 alignment
	nBlankW = nScrWidth % nChWidth;
	nBlankH = nScrHeight % nChHeight;

	for(i = 0, nLocate = 0; i < nEndCh; ++i){
		//int const nChn = i % nDiv;
		unsigned char bShow = FALSE;

		if(i >= nBeginCh && i < (nBeginCh + nDiv) && (nDiv > 1 || !bLive)){
			// need to show
			bShow = TRUE;
			// live not full division mode
			// playback all the mode
			stVoChnAttr.stRect.s32X = (nOffsetX + (((nLocate % nDiv) % nGrid) * nChWidth)) & ~(2-1); // 2 alignment
			stVoChnAttr.stRect.s32Y = (nOffsetY + (((nLocate % nDiv) / nGrid) * nChHeight)) & ~(2-1); // 2 alignment
			if((nGrid - 1) == (nLocate % nGrid)){
				// the right side
				stVoChnAttr.stRect.u32Width = nChWidth + nBlankW;
			}else{
				// for osd bar
				stVoChnAttr.stRect.u32Width = nChWidth - 2;
			}
			if((nGrid - 1) == (nLocate / nGrid)){
				// the bottom
				stVoChnAttr.stRect.u32Height = nChHeight + nBlankH;
			}else{
				// for osd bar
				stVoChnAttr.stRect.u32Height = nChHeight - 2;
			}
			stVoChnAttr.u32Priority = 0;
			stVoChnAttr.bZoomEnable = HI_TRUE;
			stVoChnAttr.bDeflicker = (0 == nVoDev) ? HI_FALSE : HI_TRUE;
			++nLocate;
		}else{
			bShow = FALSE;
			stVoChnAttr.stRect.s32X = 0;
			stVoChnAttr.stRect.s32Y = 0;
			stVoChnAttr.stRect.u32Width = VI_CIF_WIDTH;
			stVoChnAttr.stRect.u32Height = VI_CIF_HEIGHT;
			stVoChnAttr.u32Priority = 0;
			stVoChnAttr.bZoomEnable = HI_FALSE;
			stVoChnAttr.bDeflicker = HI_FALSE;
		}
		//VIDEO_TRACE("vo(%d,%d) (x,y)=%d,%d, [w,h]=%d,%d", nVoDev, i, stVoChnAttr.stRect.s32X, stVoChnAttr.stRect.s32Y, stVoChnAttr.stRect.u32Width, stVoChnAttr.stRect.u32Height);
		DVR_ASSERT(HI_MPI_VO_SetChnAttr(nVoDev, i, &stVoChnAttr));
		if(bShow){
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

		stVgaLayerAttr.stImageSize.u32Width  = VO_ORIGIN_WIDTH;
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
			video_Division(VO_DEV_ID[i], s_stVideo.nScrWidth[i], s_stVideo.nScrHeight[i], VIDEO_GetMaxDiv(), 0, FALSE);
			// enable vo channel
			VO_DISPLAY_FIELD_E enDisplayField;
			for(ii = 0; ii < VIDEO_GetMaxDiv()+1; ++ii){
				// 1 - 16 digital zoom
				if(ii < MAX_CAM_CH){
					// for vi0 1-16
					// digital zoom
					VO_ZOOM_ATTR_S stZoomAttr;

					stZoomAttr.enField = VIDEO_FIELD_FRAME;
					stZoomAttr.stZoomRect.u32Width = VI_CIF_WIDTH;
					stZoomAttr.stZoomRect.u32Height = VI_CIF_HEIGHT;
					stZoomAttr.stZoomRect.s32X = 0;
					stZoomAttr.stZoomRect.s32Y = (ii % 8) * (stZoomAttr.stZoomRect.u32Height + 2);
//					VIDEO_TRACE("ch=%d zoom [%d,%d,%d,%d]", ii, stZoomAttr.stZoomRect.s32X, stZoomAttr.stZoomRect.s32Y, stZoomAttr.stZoomRect.u32Width, stZoomAttr.stZoomRect.u32Height);
					DVR_ASSERT(HI_MPI_VO_SetZoomInWindow(VO_DEV_ID[i], ii, &stZoomAttr));
					if((ii / 8) == 0 || (ii / 8) == 2){
						enDisplayField = VO_FIELD_BOTTOM;
					}else{
						enDisplayField = VO_FIELD_TOP;
					}
				}else if(SPECIAL_D1_CH == ii){
					// for ch[01-32] d1 live
					enDisplayField = VO_FIELD_BOTH;
					//DVR_ASSERT(HI_MPI_VO_SetChnField(VO_DEV_ID[i], ii, VO_FIELD_BOTH));
					//DVR_ASSERT(HI_MPI_VO_EnableChn(VO_DEV_ID[i], ii));
				}else{
					//for left
					enDisplayField = VO_FIELD_BOTH;
				}
//				VIDEO_TRACE("vodev=%d,ch=%d enDisplayField[%d]",VO_DEV_ID[i],ii, enDisplayField);
				DVR_ASSERT(HI_MPI_VO_SetChnField(VO_DEV_ID[i], ii, enDisplayField));
				DVR_ASSERT(HI_MPI_VO_EnableChn(VO_DEV_ID[i], ii));
				if(ii < MAX_CAM_CH){
					video_BindOutput(VO_DEV_ID[i], ii, TRUE);
				}else if(ii < SPECIAL_D1_CH){
					DVR_ASSERT(HI_MPI_VO_SENDFRAME(VO_DEV_ID[i], ii, s_pstLogoPic));
				}
			}
		}
	}
}

static void video_OutputDestroy()
{
	int i = 0, ii = 0;
	for(i = 0; i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0]); ++i){
		// enable vo channel
		for(ii = MAX_CAM_CH; ii >= 0; --ii){
			video_BindOutput(VO_DEV_ID[i], ii, FALSE);
			DVR_ASSERT(HI_MPI_VO_DisableChn(VO_DEV_ID[i], ii));
		}
		// disable video layer
		DVR_ASSERT(HI_MPI_VO_DisableVideoLayer(VO_DEV_ID[i]));
	}
	video_ScreenExit();
}

int video_DigitalZoom(int nChn, int nRatioX, int nRatioY, int nRatioW, int nRatioH, unsigned char bLive)
{
	return -1;
}
static void video_ClearDigitalZoom(int nChn)
{

}
