#ifndef __CROSS_TYPE_H__
#define __CROSS_TYPE_H__

#include "stdinc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(WIN32) && !defined(__cplusplus)

typedef enum {
	false,
	true
}bool;

typedef unsigned char uint8_t;
typedef INT64 int64_t;
typedef UINT64 uint64_t;
typedef unsigned int uint32_t;
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
#else

#include <stdbool.h>
#include <stdint.h>

#endif

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#ifdef __cplusplus
}
#endif
#endif