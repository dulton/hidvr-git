

#ifndef __DVR_DEBUG_H__
#define __DVR_DEBUG_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define DVR_DEBUG_COLOR "\033[0;36m"
#define DVR_NDEBUG
#ifdef DVR_NDEBUG
#define DVR_ASSERT(expression) \
	do{\
		unsigned int const __ret = (expression);\
		if(HI_SUCCESS != __ret){\
			printf(DVR_DEBUG_COLOR"%s->%d %s failed->0x%08x\033[m\r\n",__FILE__,__LINE__,\
			#expression,__ret);\
			exit(1);\
		}\
	}while(0)
#else
#define DVR_ASSERT(expression) do{unsigned int const __ret = (expression);}while(0)
#endif

#endif //__DVR_DEBUG_H__

