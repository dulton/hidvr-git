
#ifndef __MEDIABUF_DEBUG_H__
#define __MEDIABUF_DEBUG_H__

#define MEDIABUF_DEBUG (1)
#if MEDIABUF_DEBUG
#define MEDIABUF_SYNTAX "1;33"
#define MEDIABUF_TRACE(fmt, arg...) \
	do{\
		const char* bname = basename(strdupa(__FILE__));\
		printf("\033["MEDIABUF_SYNTAX"m[%12s:%4d]\033[0m ", bname, __LINE__);\
		printf(fmt, ##arg);\
		printf("\r\n");\
	}while(0)
		
#define MEDIABUF_ASSERT(exp, fmt, arg...) \
	do{\
		if(!(exp)){\
			const char* bname = basename(strdupa(__FILE__));\
			printf("\033["MEDIABUF_SYNTAX"m[%12s:%4d]\033[0m assert(\"%s\") ", bname, __LINE__, #exp);\
			printf(fmt, ##arg);\
			printf("\r\n");\
			exit(1);\
		}\
	}while(0)

#else
#define MEDIABUF_TRACE(fmt...)
#define MEDIABUF_ASSERT(exp, fmt...)
#endif

#endif //__MEDIABUF_DEBUG_H__

