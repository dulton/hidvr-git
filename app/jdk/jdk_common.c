#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jdk_common.h"
#include "conf.h"

/* JDK_CHN_INPUT_MODE */
char *g_JdkChnInputModeString[] = 
{
	"VI-NULL", "BNC-D1", "BNC-720H", "BNC-960H", "BNC-720P","BNC-Half1080P", "BNC-1080P", "NET-720P", "NET-960P", "NET-1080P" 
};


JDK_BOOL JDK_inmode_get_wh(JDK_CHN_INPUT_MODE resol, JDK_UINT32 *width, JDK_UINT32 *height)
{
	JDK_UINT32 r_wh[][2] = {
			{0, 0},
			{704, 0},
			{720, 0},
			{960, 0},
			{1280, 720},
			{960,1080},
			{1920, 1080},
			{1280, 720},
			{1280, 960},
			{1920, 1080},
		};

	jdk_return_val_if_fail((width != NULL) && (height != NULL), JDK_FALSE);
	jdk_return_val_if_fail(resol < JDK_INPUT_NET_END, JDK_FALSE);
	
	if (resol > JDK_ARRAY_SIZE(r_wh)) {
		return JDK_FALSE;
	}
	
	*width = r_wh[resol][0];
	*height = r_wh[resol][1];

	if (r_wh[resol][1] == 0) {
		if (r_wh[resol][0] == 0){
			return JDK_FALSE;
		}
		if (JDK_STANDARD_IS_PAL()) {
			*height = 576;
		} else {
			*height = 480;
		}
	}

	return JDK_TRUE;
}

JDK_BOOL JDK_resol_get_wh(JDK_RESOLUTION_TYPE resol, JDK_UINT32 *width, JDK_UINT32 *height)
{
	jdk_return_val_if_fail((width != NULL) && (height != NULL), JDK_FALSE);
	jdk_return_val_if_fail(resol <= JDK_RESOL_1080P, JDK_FALSE);
	
	*width = 0;
	*height = 0;
	
	switch(resol) {
		case JDK_RESOL_QCIF:
			*width = 176;
			if (JDK_STANDARD_IS_PAL()) {
				*height = 144;
			} else {
				*height = 120;
			}
			break;
		case JDK_RESOL_QVGA:
			*width = 320;
			*height = 240;
			break;
		case JDK_RESOL_CIF:
			*width = 352;
			if (JDK_STANDARD_IS_PAL()) {
				*height = 288;
			} else {
				*height = 240;
			}
			break;
		case JDK_RESOL_HALF_D1:
			*width = 704;			
			if (JDK_STANDARD_IS_PAL()) {
				*height = 288;
			} else {
				*height = 240;
			}
			break;
		case JDK_RESOL_360P:
			*width = 640;
			*height = 360;
			break;
		case JDK_RESOL_VGA:
			*width = 640;
			*height = 480;
			break;
		case JDK_RESOL_D1:
			*width = 704;
			if (JDK_STANDARD_IS_PAL()) {
				*height = 576;
			} else {
				*height = 480;
			}
			break;
		case JDK_RESOL_960H:
			*width = 960;			
			if (JDK_STANDARD_IS_PAL()) {
				*height = 576;
			} else {
				*height = 480;
			}
			break;
		case JDK_RESOL_720P:
			*width = 1280;
			*height = 720;
			break;
		case JDK_RESOL_960P:
			*width = 1280;
			*height = 960;
			break;
		case JDK_RESOL_HFHD:
			*width = 960;
			*height = 1080;
			break;
		case JDK_RESOL_1080P:
			*width = 1920;
			*height = 1080;
			break;
	}

	return JDK_TRUE;
}


JDK_BOOL JDK_vga_resol_get_wh(JDK_VGA_RESOLUTION_TYPE resol, JDK_UINT32 *width, JDK_UINT32 *height)
{
	JDK_UINT32 r_wh[][2] = {
			{800, 600},
			{1024, 768},
			{1280, 720},
			{1366, 768},
			{1440, 900},
			{1920, 1080},
			{1920, 1080},
		};

	jdk_return_val_if_fail((width != NULL) && (height != NULL), JDK_FALSE);
	jdk_return_val_if_fail(resol < JDK_VGA_RESOLUTION_CNT, JDK_FALSE);
	
	if (resol > JDK_ARRAY_SIZE(r_wh)) {
		return JDK_FALSE;
	}
	
	*width = r_wh[resol][0];
	*height = r_wh[resol][1];

	return JDK_TRUE;
}

JDK_BOOL JDK_vi_resize_in_cut_mode(JDK_VI_CUT_MODE cutMode, JDK_INT32 *width, JDK_INT32 *height)
{
	if (cutMode == JDK_VI_CUT_MODE_NONE) 
		return JDK_TRUE;
	if (width == NULL || height == NULL)
		return JDK_FALSE;
	switch (cutMode){
		case JDK_VI_CUT_MODE_1_2:
			*width /= 2;
			break;
		case JDK_VI_CUT_MODE_1_4:
			*width /= 2;
			*height /= 2;
			break;
		case JDK_VI_CUT_MODE_1_8:
			*width /= 4;
			*height /= 2;
			break;
		case JDK_VI_CUT_MODE_1_9:
			*width /= 3;
			*height /= 3;
			break;
		case JDK_VI_CUT_MODE_1_16:
			*width /= 4;
			*height /= 4;
			break;
		default:
			return JDK_FALSE;
	}
	JDK_DEBUG("resize vi cutmode:%d to (%d x %d)", cutMode, *width, *height);
	return JDK_TRUE;
}

JDK_BOOL JDK_GET_SIBLING_CHN(JDK_UINT32 thiz_chn, JDK_VI_MUX_MODE muxMode,  JDK_UINT32 *sibling_chns, JDK_UINT32 *n_sibling_chns)
{
	JDK_INT32 i, n, leftVal, rightVal = 0;
	jdk_return_val_if_fail((sibling_chns != NULL) && (n_sibling_chns != NULL), JDK_FALSE);
	jdk_return_val_if_fail((*n_sibling_chns) > 0, JDK_FALSE);
	jdk_return_val_if_fail(thiz_chn < JDK_MAX_CHN, JDK_FALSE);
	jdk_return_val_if_fail(JDK_IS_VALID_VI_MUX(muxMode), JDK_FALSE);
	
	*n_sibling_chns = muxMode;
	sibling_chns[0] = thiz_chn;
	rightVal = (thiz_chn + muxMode)/muxMode * muxMode;
	leftVal = rightVal - muxMode;
	//printf("[%u] sibling chns(%u): ", thiz_chn, *n_sibling_chns);
	for (i = 0, n = leftVal; n < rightVal; n++, i++) {
		sibling_chns[i] = n;
		//printf("%d,", n);
	}
	//printf("\r\n");
	return JDK_TRUE;
}

JDK_BOOL JDK_is_all_sibling_chns_close_2MUX(JDK_UINT32 chn, JDK_INT32 viDev)
{
	int ii;
	JDK_UINT32 tmpChnl;
	JDK_UINT32 openFlag = 0;

	tmpChnl = viDev * 2;

	if ( chn%2 == 1)
	{
		tmpChnl -= 1;
	}
	else
	{
		tmpChnl += 1;
	}

	if (JDK_CHN_STATE(tmpChnl) == JDK_CHN_STATE_BNC)
	{
		return JDK_FALSE;
	}
	else
	{
		return JDK_TRUE;
	}
}

JDK_BOOL JDK_is_all_sibling_chns_close(JDK_UINT32 thiz_chn, JDK_VI_MUX_MODE muxMode)
{
	JDK_INT32 n;
	JDK_UINT32 siblingChns[JDK_MAX_CHN];
	JDK_UINT32 nChn = JDK_MAX_CHN;
	JDK_UINT32 openFlag = 0;

	jdk_return_val_if_fail(JDK_GET_SIBLING_CHN(thiz_chn, muxMode, siblingChns, &nChn), JDK_FALSE);
	for (n = 0; n < nChn; n++) {
		//if (siblingChns[n] != thiz_chn) {
			if (JDK_CHN_STATE(siblingChns[n]) == JDK_CHN_STATE_BNC) {
				openFlag++;
			}
		//}
	}
	//printf("Open Chn Flag is %u\r\n", openFlag);
	if (openFlag == 0)
		return JDK_TRUE;
	else
		return JDK_FALSE;
}

JDK_VI_MUX_MODE JDK_get_mux_mode()
{
#ifdef BNC_VI_MUX_X1
	return JDK_VI_MUX_X1;
#elif defined BNC_VI_MUX_X4	
	return JDK_VI_MUX_X4;
#else
	return JDK_VI_MUX_X2;
#endif
}
