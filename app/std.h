/*
 * std.h
 *
 *  Created on: 2010-11-13
 *      Author: root
 */

#ifndef STD_H_
#define STD_H_
//
////system
#include <stdio.h>
#include <unistd.h>
#include <time.h>
//#include <sys/time.h>
//#include <string.h>
#include <stdlib.h>
//#include <errno.h>
//#include <pthread.h>
#include <assert.h>



#define ENABLE_DEBUG
#define ENABLE_INFO
#define ENABLE_ERROR
//#define NDEBUG


extern struct tm xxxt;
extern struct tm get_now();

#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"

#ifdef ENABLE_DEBUG
#define TRACE_DEBUG(fmt, args...) xxxt=get_now();printf(GREEN"DEBUG:%5u:[%s:%d]@%02d:%02d:%02d  "fmt"\n"NONE, getpid(),  __FUNCTION__, __LINE__, xxxt.tm_hour, xxxt.tm_min, xxxt.tm_sec, ##args)
#else
#define TRACE_DEBUG(fmt, args...)
#endif

#ifdef ENABLE_INFO
#define TRACE_INFO(fmt, args...)  xxxt=get_now();printf(YELLOW"INFO :%5u:[%s:%d]@%02d:%02d:%02d  "fmt"\n"NONE, getpid(),  __FUNCTION__, __LINE__, xxxt.tm_hour, xxxt.tm_min, xxxt.tm_sec, ##args)
#else
#define TRACE_INFO(fmt, args...)
#endif

#ifdef ENABLE_ERROR
#define TRACE_ERROR(fmt, args...) xxxt=get_now();printf(  RED"ERROR:%5u:[%s:%d]@%02d:%02d:%02d  "fmt"\n"NONE, getpid(),  __FUNCTION__, __LINE__, xxxt.tm_hour, xxxt.tm_min, xxxt.tm_sec, ##args)
#else
#define TRACE_ERROR(fmt, args...)
#endif


#define ASSERT(express, fmt, args...) assert(((express) ? 1 : (fprintf(stderr, fmt"\t", ##args),0)))

//#define MALLOC(size) malloc(size);TRACE_DEBUG("malloc(%d) %s:%d", size, __FUNCTION__, __LINE__)
#define MALLOC(size) malloc(size)
#endif /* STD_H_ */
