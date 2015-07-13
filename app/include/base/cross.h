#ifndef __CROSS_HEAD_FILE__
#define __CROSS_HEAD_FILE__

#include "cross_type.h"
#include "cross_thread.h"
#include "cross_lock.h"
#include "cross_ipc.h"

#ifdef WIN32
#define _COS_WIN
#endif

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C    extern "C"
#else
#define EXTERN_C    extern
#endif
#endif

#if defined(WIN32) && !defined(__cplusplus)
#define inline __inline
#endif

#ifdef WIN32
#define STRDUP _strdup
#define SNPRINTF _sntprintf
#define STRCASECMP _stricmp
#define RANDOM rand
typedef HANDLE pthread_t;

// last errno
#define GetLastError()	GetLastError()
// millisecond	
typedef uint64_t MillisecondTimer_t;
typedef uint64_t MilliSecond_t;
////mtimer and mtimer_tmp is type of MillisecondTimer_t,lasttime is type of MilliSecond_t which is duration time between start and stop
#define MilliTimerStart(mtimer)	mtimer=GetTickCount64()	
#define MilliTimerAdd(mtimer,val) mtimer+=(val)
#define MilliTimerMinus(mtimer,val) mtimer-=(val)
#define MilliTimerSet(mtimer,sec,usec) mtimer=(sec)*1000+((usec)/1000)
#define MilliTimerGet(mtimer,sec,usec) \
	do{\
		sec = mtimer/1000;\
		usec = (mtimer%1000)*1000;\
	}while(0)
#define MilliTimerIsClear(mtimer) ((mtimer == 0) ? TRUE : FALSE)
#define MilliTimerStop(mtimer,mtimer_tmp,lasttime) \
	do{\
		mtimer_tmp = GetTickCount();\
		lasttime=mtimer_tmp-mtimer;\
	}while(0)

#else // _COS_WIN
#define STRDUP strdup
#define SNPRINTF snprintf
#define STRCASECMP strcasecmp
#define RANDOM random

// last errno
#define GetLastError()	errno
// millisecond	
typedef struct timeval MillisecondTimer_t;
typedef uint64_t MilliSecond_t;
////mtimer and mtimer_tmp is type of MillisecondTimer_t,lasttime is type of MilliSecond_t which is duration time between start and stop
#define MilliTimerStart(mtimer)	gettimeofday(&mtimer,NULL)
#define MilliTimerSet(mtimer,sec,usec) \
	do{\
		mtimer.tv_sec = (sec);\
		mtimer.tv_usec = (usec);\
	}while(0)
#define MilliTimerGet(mtimer,sec,usec) \
		do{\
			sec = mtimer.tv_sec;\
			usec = mtimer.tv_usec;\
		}while(0)
#define MilliTimerIsClear(mtimer) ((mtimer.tv_sec == 0 && mtimer.tv_usec == 0) ? TRUE : FALSE)
#define MilliTimerAdd(mtimer,val) \
	do{\
		mtimer.tv_sec+=(val)/1000;\
		mtimer.tv_usec+=((val)%1000)*1000;\
		if(mtimer.tv_usec >= 1000000){\
			mtimer.tv_usec -= 1000000;\
			mtimer.tv_sec++;\
		}\
	}while(0)
#define MilliTimerMinus(mtimer,val) \
	do{\
		mtimer.tv_sec-=(val)/1000;\
		if(mtimer.tv_usec < (((val)%1000)*1000)){\
			mtimer.tv_usec = mtimer.tv_usec + 1000000 - ((val)%1000)*1000;\
			mtimer.tv_sec--;\
		}else{\
			mtimer.tv_usec -= ((val)%1000)*1000;\
		}\
	}while(0)
#define MilliTimerStop(mtimer,mtimer_tmp,lasttime) \
	do{\
		gettimeofday(&mtimer_tmp,NULL);\
		lasttime=(mtimer_tmp.tv_sec-mtimer.tv_sec)*1000+(mtimer_tmp.tv_usec - mtimer.tv_usec)/1000;\
	}while(0)

#endif //!defined(_COS_WIN)

EXTERN_C int sleep_ms_c(unsigned long ms);

EXTERN_C int sleep_s_c(unsigned long s);

EXTERN_C struct tm * localtime_c(time_t * tt,struct tm *t);

EXTERN_C struct tm * gmtime_c(time_t * tt,struct tm * t);

EXTERN_C char * ctime_c(time_t * t,char * str);

EXTERN_C char * basename_c(char *path);

EXTERN_C char * strdupa_c(char * s);

EXTERN_C void bcopy_c(void * src, void * dest,size_t n);

#endif