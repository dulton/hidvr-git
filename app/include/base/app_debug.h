
#include <string.h>
#include <errno.h>

#ifndef APP_DEBUG_H_
#define APP_DEBUG_H_
#ifdef __cplusplus
extern "C" {
#endif

#define APP_DEBUG (1)

#if APP_DEBUG
#ifdef WIN32
#define APP_TRACE(fmt, ...) \
	do{\
		const char* bname = basename_c(strdupa_c(__FILE__));\
		int const syntax_fg = ((bname[0] >> 8) + (bname[0] & 0xff)) % 8;\
		printf("\033[1;%dm[%12s:%4d]\033[0m ", 30 + syntax_fg, bname, __LINE__);\
		printf(fmt, __VA_ARGS__);\
		printf("\r\n");\
	}while(0)

#define APP_ASSERT(exp, fmt, ...) \
	do{\
		if(!(exp)){\
			const char* bname = basename_c(strdupa_c(__FILE__));\
			int const syntax_fg = bname[0] % 8;\
			printf("\033[1;%dm[%12s:%4d]\033[0m assert(\"%s\") ", 30 + syntax_fg, bname, __LINE__, #exp);\
			printf(fmt, __VA_ARGS__);\
			printf("\r\n");\
			exit(1);\
		}\
	}while(0)
#else
#define APP_TRACE(fmt, arg...) \
	do{\
		const char* bname = basename(strdupa(__FILE__));\
		int const syntax_fg = ((bname[0] >> 8) + (bname[0] & 0xff)) % 8;\
		printf("\033[1;%dm[%12s:%4d]\033[0m ", 30 + syntax_fg, bname, __LINE__);\
		printf(fmt, ##arg);\
		printf("\r\n");\
	}while(0)

#define APP_ASSERT(exp, fmt, arg...) \
	do{\
		if(!(exp)){\
			const char* bname = basename(strdupa(__FILE__));\
			int const syntax_fg = bname[0] % 8;\
			printf("\033[1;%dm[%12s:%4d]\033[0m assert(\"%s\") ", 30 + syntax_fg, bname, __LINE__, #exp);\
			printf(fmt, ##arg);\
			printf("\r\n");\
			exit(1);\
		}\
	}while(0)

#endif
#else
#define APP_TRACE(fmt...)
#define APP_ASSERT(exp, fmt...)
#endif

#define OS_USER_EXEC(__name, __exp) do{ if(0 == strcmp(__name, OS_USER)){ __exp;} }while(0)
#define FRANK_EXEC(__exp) do{ OS_USER_EXEC("Frank", __exp); }while(0)

#ifdef __cplusplus
}
#endif
#endif //APP_DEBUG_H_

