
#ifndef __SPOOK_DEBUG_H__
#define __SPOOK_DEBUG_H__

#define SPOOK_DEBUG (1)
#if SPOOK_DEBUG
#define SPOOK_SYNTAX "37;1;32"
#define SPOOK_TRACE(fmt, arg...) \
	do{\
		const char* bname = basename(strdupa(__FILE__));\
		printf("\033["SPOOK_SYNTAX"m[%12s:%4d]\033[0m ", bname, __LINE__);\
		printf(fmt, ##arg);\
		printf("\r\n");\
	}while(0)

#define SPOOK_ASSERT(exp, fmt, arg...) \
	do{\
		if(!(exp)){\
			const char* bname = basename(strdupa(__FILE__));\
			printf("\033["SPOOK_SYNTAX"m[%12s:%4d]\033[0m assert(\"%s\") ", bname, __LINE__, #exp);\
			printf(fmt, ##arg);\
			printf("\r\n");\
			exit(1);\
		}\
	}while(0)

#else
#define SPOOK_TRACE(fmt...)
#define SPOOK_ASSERT(exp, fmt...)
#endif

#endif //__SPOOK_DEBUG_H__

