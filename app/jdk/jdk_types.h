

#ifndef __JDK_TYPES_H__
#define __JDK_TYPES_H__
	
#ifdef __cplusplus
extern "C" {
#endif

typedef char   JDK_CHAR;
typedef int   JDK_BOOL;

typedef float   JDK_FLOAT;
typedef double  JDK_DOUBLE;

typedef signed char JDK_INT8;
typedef unsigned char JDK_UINT8;
typedef signed short JDK_INT16;
typedef unsigned short JDK_UINT16;
typedef signed int JDK_INT32;
typedef unsigned int JDK_UINT32;
#ifndef WIN32
typedef signed long long JDK_INT64;
typedef unsigned long long JDK_UINT64;
#else
typedef __int64 JDK_INT64;
typedef __int64 JDK_UINT64;
#endif

typedef void *JDK_POINTER;

/* Define min and max constants for the fixed size numerical types */
#define JDK_MININT8	((JDK_INT8)  0x80)
#define JDK_MAXINT8	((JDK_INT8)  0x7f)
#define JDK_MAXUINT8	((JDK_UINT8) 0xff)

#define JDK_MININT16	((JDK_INT16)  0x8000)
#define JDK_MAXINT16	((JDK_INT16)  0x7fff)
#define JDK_MAXUINT16	((JDK_UINT16) 0xffff)

#define JDK_MININT32	((JDK_INT32)  0x80000000)
#define JDK_MAXINT32	((JDK_INT32)  0x7fffffff)
#define JDK_MAXUINT32	((JDK_UINT32) 0xffffffff)

#define JDK_INT64_CONSTANT(val)	(val##LL)
#define JDK_INT64_MODIFIER "ll"
#define JDK_INT64_FORMAT "lli"
#define JDK_UINT64_FORMAT "llu"

#define JDK_MININT64	JDK_INT64_CONSTANT(0x8000000000000000)
#define JDK_MAXINT64	JDK_INT64_CONSTANT(0x7fffffffffffffff)
#define JDK_MAXUINT64	JDK_INT64_CONSTANT(0xffffffffffffffffU)

#define JDK_POINTER_TO_INT(p)	((JDK_INT32)   (p))
#define JDK_POINTER_TO_UINT(p)	((JDK_UINT32)  (p))

#define JDK_INT_TO_POINTER(i)	((JDK_POINTER)  (i))
#define JDK_UINT_TO_POINTER(u)	((JDK_POINTER)  (u))


enum {
	JDK_FALSE = 0,
	JDK_TRUE = 1,
};

/*
*/



#ifdef __cplusplus
}
#endif
#endif /* __JDK_TYPES_H__ */



