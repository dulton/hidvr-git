

#ifndef __STIME_H__
#define __STIME_H__

#include <sys/time.h>
#include <time.h>
#include <stdio.h>

//Statistics Time
#define STIME_USE
#ifdef STIME_USE
#define STIME_TRACE(fmt...) 	do{printf("\033[0;32;31m[%s] ", __FUNCTION__);printf(fmt);printf("\033[m\r\n");}while(0)
#define STIME_BEGIN() \
	struct timeval tv_begin = {0}, tv_end = {0};\
	static unsigned int shortest = 0, longest = 0;\
	static unsigned int run_times = 1;\
	static unsigned int run_sum = 0;\
	do{\
		gettimeofday(&tv_begin , NULL);\
	}while(0)
	
#define STIME_END() \
	gettimeofday(&tv_end, NULL);\
	do{\
		struct timeval tv_sub = {0};\
		timersub(&tv_end, &tv_begin, &tv_sub);\
		{\
			int const current_time = tv_sub.tv_sec * 1000 + tv_sub.tv_usec / 1000;\
			if(current_time > longest){longest = current_time;}\
			if(0 == shortest || current_time < shortest){shortest = current_time;}\
			run_sum += current_time;\
			STIME_TRACE("take time: %dms/[%d,%d]ms average %dms", current_time, shortest, longest, run_sum/(run_times++));\
		}\
	}while(0)
#else
#define STIME_BEGIN()
#define STIME_END()
#endif

#endif //__STIME_H__

