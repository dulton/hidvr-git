

#ifndef __JA_TYPES_H__
#define __JA_TYPES_H__
	
#ifdef __cplusplus
extern "C" {
#endif

typedef char   JA_CHAR;
typedef int   JA_BOOL;

typedef float   JA_FLOAT;
typedef double  JA_DOUBLE;

typedef signed char JA_INT8;
typedef unsigned char JA_UINT8;
typedef signed short JA_INT16;
typedef unsigned short JA_UINT16;
typedef signed int JA_INT32;
typedef unsigned int JA_UINT32;
#ifndef WIN32
typedef signed long long JA_INT64;
typedef unsigned long long JA_UINT64;
#else
typedef __int64 JA_INT64;
typedef __int64 JA_UINT64;
#endif

typedef void *JA_POINTER;

/* Define min and max constants for the fixed size numerical types */
#define JA_MININT8	((JA_INT8)  0x80)
#define JA_MAXINT8	((JA_INT8)  0x7f)
#define JA_MAXUINT8	((JA_UINT8) 0xff)

#define JA_MININT16	((JA_INT16)  0x8000)
#define JA_MAXINT16	((JA_INT16)  0x7fff)
#define JA_MAXUINT16	((JA_UINT16) 0xffff)

#define JA_MININT32	((JA_INT32)  0x80000000)
#define JA_MAXINT32	((JA_INT32)  0x7fffffff)
#define JA_MAXUINT32	((JA_UINT32) 0xffffffff)

#define JA_INT64_CONSTANT(val)	(val##LL)
#define JA_INT64_MODIFIER "ll"
#define JA_INT64_FORMAT "lli"
#define JA_UINT64_FORMAT "llu"

#define JA_MININT64	JA_INT64_CONSTANT(0x8000000000000000)
#define JA_MAXINT64	JA_INT64_CONSTANT(0x7fffffffffffffff)
#define JA_MAXUINT64	JA_INT64_CONSTANT(0xffffffffffffffffU)

#define JA_POINTER_TO_INT(p)	((JA_INT32)   (p))
#define JA_POINTER_TO_UINT(p)	((JA_UINT32)  (p))

#define JA_INT_TO_POINTER(i)	((JA_POINTER)  (i))
#define JA_UINT_TO_POINTER(u)	((JA_POINTER)  (u))


enum {
	JA_FALSE = 0,
	JA_TRUE = 1,
};


/*
* event & error-code
*/

enum {
   	JA_EVENT_ALL = -1,
	JA_EVENT_NULL = 0,
	JA_EVENT_IDLE,
	JA_EVENT_TIMER,
	JA_EVENT_ERROR_OCCUR,
	JA_EVENT_CONNECT_OK,
	JA_EVENT_DISCONNECT_OK,
	JA_EVENT_LOGIN_OK,
	JA_EVENT_LOGOUT_OK,
	JA_EVENT_AUTH_REQUIRE,
	JA_EVENT_REQ_PAUSE,
	JA_EVENT_REQ_PLAY,
	JA_EVENT_DESTROYED,
	JA_EVENT_PLAY,
	JA_EVENT_DATA_RECEIVED,
	JA_EVENT_RTCP_SENDER_REPORT,
	JA_EVENT_RTCP_RECEIVER_REPORT,
	JA_EVENT_MD,
	JA_EVENT_MD_EX,
	JA_EVENT_VIDEO_LOSS,
	JA_EVENT_SENSOR_IN,
	JA_EVENT_ALARM_OUT,
	JA_EVENT_TOO_BLURRY,			
	JA_EVENT_TOO_DARK,				
	JA_EVENT_TIME_CHANGED,
	JA_EVENT_NET_CONF_CHANGED,
	JA_EVENT_TOO_BRIGHT,			
	JA_EVENT_SCENE_CHANGE,				
	JA_EVENT_PROFILE_CHANGED,			
	JA_EVENT_VENC_CONF_CHANGED,			
	JA_EVENT_VSOURCE_CONF_CHANGED,		
	JA_EVENT_VOUT_CONF_CHANGED,		
	JA_EVENT_AENC_CONF_CHANGED,			
	JA_EVENT_ASOURCE_CONF_CHANGED,		
	JA_EVENT_AOUT_CONF_CHANGED,
	JA_EVENT_PTZ_CONF_CHANGED,

	JA_EVENT_NR
}nJA_EVENT_TYPE;


typedef enum{
	JA_ERROR_OK = 0,
	JA_ERROR_FAULT,
	JA_ERROR_INVALID_ARG,
	JA_ERROR_CONNECT_FAILED,
	JA_ERROR_SOCK_ERROR,
	JA_ERROR_AUTH_FAILED,
	JA_ERROR_SERVER_NOT_FOUND,
	JA_ERROR_LOST_HEARTBREAK,
	JA_ERROR_PEER_ABORT_SERVER,
	JA_ERROR_HTTP_PARSE_FAIL,
	JA_ERROR_XML_PARSE_FAIL,
	JA_ERROR_JSON_PARSE_FAIL,
	JA_ERROR_NO_MEM,
	JA_ERROR_RES_BUSY,

	JA_ERROR_NR
}nJA_ERROR_TYPE;



#if !(defined (JA_STMT_START) && defined (JA_STMT_END))
#  if defined (__GNUC__) && !defined (__STRICT_ANSI__) && !defined (__cplusplus)
#    define JA_STMT_START	(void) __extension__ (
#    define JA_STMT_END		)
#  else
#    if (defined (sun) || defined (__sun__))
#      define JA_STMT_START	if (1)
#      define JA_STMT_END	else (void)0
#    else
#      define JA_STMT_START	do
#      define JA_STMT_END	while (0)
#    endif
#  endif
#endif

#define JA_EXIT(n)	JA_STMT_START {exit(n); }JA_STMT_END

#ifdef __cplusplus
}
#endif
#endif /* __JA_TYPES_H__ */




