
#ifndef __SLOG_H__
#define __SLOG_H__

#include <stdio.h>
#include <stdarg.h>
#include "cross.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _DEBUG
#undef NODEBUG
#endif


#define LM_SOCK	"SOCK"
#define LM_ALOE	"ALOE"

typedef enum
{ 
	SL_CRIT=0,
	SL_ERROR,
	SL_WARNING,
	SL_INFO,
	SL_DEBUG,
	SL_DEBUG2,
	SL_ALL
} enSLOG_LEVEL;

typedef void (fSLOG_HOOK)(const char *module, int level, const char *fmt, va_list);
void slog_set_hook(fSLOG_HOOK *cb);
void slog_set_output(FILE *file);

#ifdef __GNUC__
void slog_printf(const char *format, ...) __attribute__ ((__format__ (__printf__, 1, 2)));
void slog_status(const char *format, ...) __attribute__ ((__format__ (__printf__, 1, 2)));
void slog(const char *module, int level, const char *format, ...) __attribute__ ((__format__ (__printf__, 3, 4)));
void slog_assert(int expression, const char *module, int level, const char *format, ...) __attribute__ ((__format__ (__printf__, 4, 5)));
#else
void slog_printf(const char *format, ...);
void slog_status(const char *format, ...);
void slog(const char *module, int level, const char *format, ...);
void slog_assert(int expression, const char *module, int level, const char *format, ...);
#endif
void slog_hex(int level, const unsigned char *data, unsigned long len);
void slog_hexstring(int level, const unsigned char *data, unsigned long len);
void slog_set_level(enSLOG_LEVEL lvl);
enSLOG_LEVEL slog_get_level(void);
void slog_set_level2(enSLOG_LEVEL lvl, int module);

#ifdef WIN32
#define __basename basename_c
#define __strdupa strdupa_c
#else
#define __basename basename
#define __strdupa strdupa
#endif

#ifdef WIN32
#define SLOG(module, level, fmt, ...) \
	do{\
		const char* bname = __basename(__strdupa(__FILE__));\
		slog(module, level, "[%12s:%4d] "fmt"\r\n", bname, __LINE__, __VA_ARGS__);\
	}while(0)

#define SLOG_DEBUG(module, fmt, ...) \
	do{\
		const char* bname = __basename(__strdupa(__FILE__));\
		slog(module, SL_DEBUG, "[%12s:%4d] "fmt"\r\n", bname, __LINE__, __VA_ARGS__);\
	}while(0)
	
#define SLOG_WARNING(module, fmt, ...) \
	do{\
		const char* bname = __basename(__strdupa(__FILE__));\
		slog(module, SL_WARNING, "[%12s:%4d] "fmt"\r\n", bname, __LINE__, __VA_ARGS__);\
	}while(0)
	
#define SLOG_ERROR(module, fmt, ...) \
	do{\
		const char* bname = __basename(__strdupa(__FILE__));\
		slog(module, SL_ERROR, "[%12s:%4d] "fmt"\r\n", bname, __LINE__, __VA_ARGS__);\
	}while(0)

#define SASERT(module, exp, fmt, ...) \
	do{\
		if(!(exp)){\
			const char* bname = __basename(__strdupa(__FILE__));\
			slog(module, SL_CRIT, "[%12s:%4d] "fmt"\r\n", bname, __LINE__, __VA_ARGS__);\
			exit(1);\
		}\
	}while(0)

#define VLOG(level, fmt, ...) \
	do{\
		const char* bname = __basename(__strdupa(__FILE__));\
		slog("VLOG", level, "[%12s:%4d] "fmt"\r\n", bname, __LINE__, __VA_ARGS__);\
	}while(0)
		
#else
#define SLOG(module, level, fmt, arg...) \
	do{\
		const char* bname = __basename(__strdupa(__FILE__));\
		slog(module, level, "[%12s:%4d] "fmt"\r\n", bname, __LINE__, ##arg);\
	}while(0)

#define SLOG_DEBUG(module, fmt, arg...) \
	do{\
		const char* bname = __basename(__strdupa(__FILE__));\
		slog(module, SL_DEBUG, "[%12s:%4d] "fmt"\r\n", bname, __LINE__, ##arg);\
	}while(0)
	
#define SLOG_WARNING(module, fmt, arg...) \
	do{\
		const char* bname = __basename(__strdupa(__FILE__));\
		slog(module, SL_WARNING, "[%12s:%4d] "fmt"\r\n", bname, __LINE__, ##arg);\
	}while(0)
	
#define SLOG_ERROR(module, fmt, arg...) \
	do{\
		const char* bname = __basename(__strdupa(__FILE__));\
		slog(module, SL_ERROR, "[%12s:%4d] "fmt"\r\n", bname, __LINE__, ##arg);\
	}while(0)

#define SASERT(module, exp, fmt, arg...) \
	do{\
		if(!(exp)){\
			const char* bname = __basename(__strdupa(__FILE__));\
			slog(module, SL_CRIT, "[%12s:%4d] "fmt"\r\n", bname, __LINE__, ##arg);\
			exit(1);\
		}\
	}while(0)

#define VLOG(level, fmt, arg...) \
	do{\
		const char* bname = __basename(__strdupa(__FILE__));\
		slog("VLOG", level, "[%12s:%4d] "fmt"\r\n", bname, __LINE__, ##arg);\
	}while(0)
		
#endif	

#ifdef __cplusplus
}
#endif

#endif
