
#ifndef __HI3515_H__
#define __HI3515_H__
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

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_common.h"
#include "hi_comm_vb.h"
#include "hi_comm_sys.h"
#include "hi_comm_video.h"
#include "hi_comm_vi.h"
#include "hi_comm_vo.h"
#include "hi_comm_aenc.h"
#include "hi_comm_venc.h"
#include "hi_comm_aio.h"
#include "hi_comm_vpp.h"
#include "hi_comm_venc.h"
#include "hi_comm_avenc.h"
#include "hi_comm_md.h"

#include "mpi_sys.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_ai.h"
#include "mpi_vb.h"
#include "mpi_aenc.h"
#include "mpi_vpp.h"
#include "mpi_venc.h"
#include "mpi_avenc.h"
#include "mpi_md.h"

#include "hifb.h"

#define HI3515_DEBUG (1)
#if HI3515_DEBUG
#define HI3515_TRACE_ATTR "40;1;32"
#define HI3515_TRACE(fmt...) \
	do{\
		char* const bname = strdupa(__FILE__);\
		printf("\033["HI3515_TRACE_ATTR"mHI3515->[%s: %d] ", basename(bname), __LINE__);\
		printf(fmt);\
		printf("\033[0m\r\n");\
	}while(0)

#define HI3515_ASSERT(exp, fmt...) \
	do{\
		if(!(exp)){\
			char* const bname = strdupa(__FILE__);\
			printf("\033["HI3515_TRACE_ATTR"m");\
			printf("Assertion( %s ) @ [%s: %d] ", #exp, basename(bname), __LINE__);\
			printf("\033[0m\r\n");\
			exit(1);\
		}\
	}while(0)

#define HI3515_CHECK(exp, fmt...) \
	do{\
		uint32_t ret = exp;\
		if(HI_SUCCESS != ret){\
			char* const bname = strdupa(__FILE__);\
			printf("\033["HI3515_TRACE_ATTR"m");\
			printf("%s @ [%s: %d] err: 0x%08x <%s>", #exp, basename(bname), __LINE__, ret, HI3515_strerror(ret));\
			printf("\033[0m\r\n");\
			exit(1);\
		}\
	}while(0)

#else
#define HI3515_TRACE(fmt...)
#define HI3515_ASSERT(exp, fmt...)
#define HI3515_CHECK(exp, fmt...)
#endif

#define HI3515_MAX_IMG_WIDTH (720)
#define HI3515_MAX_IMG_HEIGHT (576)

extern const char* HI3515_strerror(const uint32_t errno);

#ifdef __cplusplus
};
#endif
#endif //__HI3515_H__

