#ifndef __SDK_PRI_API_H__
#define __SDK_PRI_API_H__

#include "../../conf.h"

#if 1
#define SDK_DBG_COLOR "\033[0;36m"
#define HIS_ASSERT(expression) \
	do{\
		unsigned int const __ret = (expression);\
		if(HI_SUCCESS != __ret){\
			printf(SDK_DBG_COLOR"%s->%d %s failed->0x%08x\033[m\r\n",__FILE__,__LINE__,\
			#expression,__ret);\
			exit(1);\
		}\
	}while(0)
#define HIS_RETURN(expression, ret) \
	do{\
		unsigned int const __ret = (expression);\
		if(HI_SUCCESS != __ret){\
			printf(SDK_DBG_COLOR"%s->%d %s failed->0x%08x\033[m\r\n",__FILE__,__LINE__,\
			#expression,__ret);\
			return(ret);\
		}\
	}while(0)
#define HIS_ROUTIN(expression, rout) \
	do{\
		unsigned int const __ret = (expression);\
		if(HI_SUCCESS != __ret){\
			printf(SDK_DBG_COLOR"%s->%d %s failed->0x%08x\033[m\r\n",__FILE__,__LINE__,\
			#expression,__ret);\
			goto rout;\
		}\
	}while(0)

#define SDK_ASSERT(expression) \
	do{\
		unsigned int const __ret = (expression);\
		if(!__ret){\
			printf(SDK_DBG_COLOR"%s->%d %s failed->0x%08x\033[m\r\n",__FILE__,__LINE__,\
			#expression,__ret);\
			exit(1);\
		}\
	}while(0)
#define SDK_RETURN(expression, ret) \
	do{\
		unsigned int const __ret = (expression);\
		if(!__ret){\
			printf(SDK_DBG_COLOR"%s->%d %s failed->0x%08x\033[m\r\n",__FILE__,__LINE__,\
			#expression,__ret);\
			return(ret);\
		}\
	}while(0)
#define SDK_ROUTIN(expression, rout) \
	do{\
		unsigned int const __ret = (expression);\
		if(!__ret){\
			printf(SDK_DBG_COLOR"%s->%d %s failed->0x%08x\033[m\r\n",__FILE__,__LINE__,\
			#expression,__ret);\
			goto rout;\
		}\
	}while(0)

#else
#define HIS_ASSERT(expression)       do{}while(0)
#define HIS_RETURN(expression, ret)  do{}while(0)
#define HIS_ROUTIN(expression, rout) do{}while(0)
#define SDK_ASSERT(expression)       do{}while(0)
#define SDK_RETURN(expression, ret)  do{}while(0)
#define SDK_ROUTIN(expression, rout) do{}while(0)
#endif

#define  ARRAY_SIZE(x) (sizeof((x))/sizeof((x)[0]))

#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)

#include <hifb.h>
#include <hi_common.h>
#include <hi_comm_adec.h>
#include <hi_comm_aenc.h>
#include <hi_comm_ai.h>
#include <hi_comm_aio.h>
#include <hi_comm_ao.h>
#include <hi_comm_avenc.h>
#include <hi_comm_dwm.h>
#include <hi_comm_group.h>
#include <hi_comm_md.h>
#include <hi_comm_pciv.h>
#include <hi_comm_sys.h>
#include <hi_comm_vb.h>
#include <hi_comm_vd.h>
#include <hi_comm_vdec.h>
#include <hi_comm_venc.h>
#include <hi_comm_vi.h>
#include <hi_comm_video.h>
#include <hi_comm_vo.h>
#include <hi_comm_vpp.h>
#include <hi_debug.h>
#include <hi_defines.h>
#include <hi_errno.h>
#include <hi_math.h>
#include <hi_tde_api.h>
#include <hi_tde_errcode.h>
#include <hi_tde_type.h>
#include <hi_type.h>
#include <loadbmp.h>
#include <mkp_vd.h>
#include <mpi_adec.h>
#include <mpi_aenc.h>
#include <mpi_ai.h>
#include <mpi_ao.h>
#include <mpi_avenc.h>
#include <mpi_md.h>
#include <mpi_pciv.h>
#include <mpi_sys.h>
#include <mpi_vb.h>
#include <mpi_vdec.h>
#include <mpi_venc.h>
#include <mpi_vi.h>
#include <mpi_vo.h>
#include <mpi_vpp.h>

#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)

#include <hi_comm_adec.h>
#include <hi_comm_aenc.h>
#include <hi_comm_ai.h>
#include <hi_comm_aio.h>
#include <hi_comm_ao.h>
#include <hi_comm_hdmi.h>
#include <hi_comm_ive.h>
#include <hi_comm_pciv.h>
#include <hi_comm_rc.h>
#include <hi_comm_region.h>
#include <hi_comm_sys.h>
#include <hi_comm_vb.h>
#include <hi_comm_vda.h>
#include <hi_comm_vdec.h>
#include <hi_comm_venc.h>
#include <hi_comm_vi.h>
#include <hi_comm_video.h>
#include <hi_comm_vo.h>
#include <hi_comm_vpss.h>
#include <hi_common.h>
#include <hi_debug.h>
#include <hi_defines.h>
#include <hi_errno.h>
#include <hi_math.h>
#include <hi_tde_api.h>
#include <hi_tde_errcode.h>
#include <hi_tde_type.h>
#include <hi_type.h>
#include <hifb.h>
#include <list.h>
#include <mpi_adec.h>
#include <mpi_aenc.h>
#include <mpi_ai.h>
#include <mpi_ao.h>
#include <mpi_hdmi.h>
#include <mpi_ive.h>
#include <mpi_pciv.h>
#include <mpi_region.h>
#include <mpi_sys.h>
#include <mpi_vb.h>
#include <mpi_vda.h>
#include <mpi_vdec.h>
#include <mpi_venc.h>
#include <mpi_vi.h>
#include <mpi_vo.h>
#include <mpi_vpss.h>

#endif

#include "sdk_fix.h"

#endif// __SDK_PRI_API_H__
