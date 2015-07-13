
#ifndef HI3520A_H_
#define HI3520A_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include "hi_type.h"

#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_vi.h"
#include "hi_comm_vo.h"
#include "hi_comm_venc.h"
#include "hi_comm_vpss.h"
#include "hi_comm_vdec.h"
#include "hi_comm_vda.h"
#include "hi_comm_region.h"
#include "hi_comm_adec.h"
#include "hi_comm_aenc.h"
#include "hi_comm_ai.h"

struct HI3520A_ERR_MAP
{
	uint32_t errno;
	const char* errstr;
};

static struct HI3520A_ERR_MAP _hi3520a_err_map[] =
{
	// ai
	{0xA0158001, "HI_ERR_AI_INVALID_DEVID",},
	{0xA0158002, "HI_ERR_AI_INVALID_CHNID",},
	{0xA0158003, "HI_ERR_AI_ILLEGAL_PARAM",},
	{0xA015800B, "HI_ERR_AI_NOT_ENABLED",},
	{0xA0158006, "HI_ERR_AI_NULL_PTR",},
	{0xA0158007, "HI_ERR_AI_NOT_CONFIG",},
	{0xA0158008, "HI_ERR_AI_NOT_SUPPORT",},
	{0xA0158009, "HI_ERR_AI_NOT_PERM",},
	{0xA015800B, "HI_ERR_AI_NOT_ENABLED",},
	{0xA015800C, "HI_ERR_AI_NOMEM",},
	{0xA015800D, "HI_ERR_AI_NOBUF",},
	{0xA015800E, "HI_ERR_AI_BUF_EMPTY",},
	{0xA015800F, "HI_ERR_AI_BUF_FULL",},
	{0xA0158010, "HI_ERR_AI_SYS_NOTREADY",},
	{0xA0158012, "HI_ERR_AI_BUSY",},

	// aenc
	{0xA0178001, "HI_ERR_AENC_INVALID_DEVID",},
	{0xA0178002, "HI_ERR_AENC_INVALID_CHNID",},
	{0xA0178003, "HI_ERR_AENC_ILLEGAL_PARAM",},
	{0xA0178004, "HI_ERR_AENC_EXIST",},
	{0xA0178005, "HI_ERR_AENC_UNEXIST",},
	{0xA0178006, "HI_ERR_AENC_NULL_PTR",},
	{0xA0178007, "HI_ERR_AENC_NOT_CONFIG",},
	{0xA0178008, "HI_ERR_AENC_NOT_SUPPORT",},
	{0xA0178009, "HI_ERR_AENC_NOT_PERM",},
	{0xA017800C, "HI_ERR_AENC_NOMEM",},
	{0xA017800D, "HI_ERR_AENC_NOBUF",},
	{0xA017800E, "HI_ERR_AENC_BUF_EMPTY",},
	{0xA017800F, "HI_ERR_AENC_BUF_FULL",},
	{0xA0178010, "HI_ERR_AENC_SYS_NOTREADY",},
	{0xA0178040, "HI_ERR_AENC_ENCODER_ERR",},

	// adec
	{0xA0188001, "HI_ERR_ADEC_INVALID_DEVID",},
	{0xA0188002, "HI_ERR_ADEC_INVALID_CHNID",},
	{0xA0188003, "HI_ERR_ADEC_ILLEGAL_PARAM",},
	{0xA0188004, "HI_ERR_ADEC_EXIST",},
	{0xA0188005, "HI_ERR_ADEC_UNEXIST",},
	{0xA0188006, "HI_ERR_ADEC_NULL_PTR",},
	{0xA0188007, "HI_ERR_ADEC_NOT_CONFIG",},
	{0xA0188008, "HI_ERR_ADEC_NOT_SUPPORT",},
	{0xA0188009, "HI_ERR_ADEC_NOT_PERM",},
	{0xA018800C, "HI_ERR_ADEC_NOMEM",},
	{0xA018800D, "HI_ERR_ADEC_NOBUF",},
	{0xA018800E, "HI_ERR_ADEC_BUF_EMPTY",},
	{0xA018800F, "HI_ERR_ADEC_BUF_FULL",},
	{0xA0188010, "HI_ERR_ADEC_SYS_NOTREADY",},
	{0xA0188040, "HI_ERR_ADEC_DECODER_ERR",},
	
	// venc
	{0xA0078001, "HI_ERR_VENC_INVALID_DEVID",},
	{0xA0078002, "HI_ERR_VENC_INVALID_CHNID",},
	{0xA0078003, "HI_ERR_VENC_ILLEGAL_PARAM",},
	{0xA0078004, "HI_ERR_VENC_EXIST",},
	{0xA0078005, "HI_ERR_VENC_UNEXIST",},
	{0xA0078006, "HI_ERR_VENC_NULL_PTR",},
	{0xA0078007, "HI_ERR_VENC_NOT_CONFIG",},
	{0xA0078008, "HI_ERR_VENC_NOT_SUPPORT",},
	{0xA0078009, "HI_ERR_VENC_NOT_PERM",},
	{0xA007800C, "HI_ERR_VENC_NOMEM",},
	{0xA007800D, "HI_ERR_VENC_NOBUF",},
	{0xA007800E, "HI_ERR_VENC_BUF_EMPTY",},
	{0xA007800F, "HI_ERR_VENC_BUF_FULL",},
	{0xA0078010, "HI_ERR_VENC_SYS_NOTREADY",},

	// region
	{0xA0128001, "HI_ERR_RGN_INVALID_DEVID",},
	{0xA0128002, "HI_ERR_RGN_INVALID_CHNID",},
	{0xA0128003, "HI_ERR_RGN_ILLEGAL_PARAM",},
	{0xA0128004, "HI_ERR_RGN_EXIST",},
	{0xA0128005, "HI_ERR_RGN_UNEXIST",},
	{0xA0128006, "HI_ERR_RGN_NULL_PTR",},
	{0xA0128007, "HI_ERR_RGN_NOT_CONFIG",},
	{0xA0128008, "HI_ERR_RGN_NOT_SUPPORT",},
	{0xA0128009, "HI_ERR_RGN_NOT_PERM",},
	{0xA012800C, "HI_ERR_RGN_NOMEM",},
	{0xA012800D, "HI_ERR_RGN_NOBUF",},
	{0xA012800E, "HI_ERR_RGN_BUF_EMPTY",},
	{0xA012800F, "HI_ERR_RGN_BUF_FULL",},
	{0xA0128010, "HI_ERR_RGN_NOTREADY",},
	{0xA0128011, "HI_ERR_RGN_BADADDR",},
	{0xA0128012, "HI_ERR_RGN_BUSY",},

	
};

const char* SOC_strerror(uint32_t const errno)
{
	int i = 0;
	for(i = 0; i < (int)(sizeof(_hi3520a_err_map) / sizeof(_hi3520a_err_map[0])); ++i){
		if(errno == _hi3520a_err_map[i].errno){
			return _hi3520a_err_map[i].errstr;
		}
	}
	return "UNKNOWN ERROR!";
}


#endif

