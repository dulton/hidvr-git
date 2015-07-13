
#ifndef __HTTPD_H__
#define __HTTPD_H__

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>

#include "http_common.h"
#include "spook.h"
#include "aval.h"

typedef enum
{
	HTTPD_METHOD_GET,
	HTTPD_METHOD_POST,
	HTTPD_METHOD_CNT,
}HTTPD_METHOD;
extern char* HTTPD_METHOD_STRING[];

typedef enum
{
	HTTPD_HEAD_KEY_CONNECTION,
	HTTPD_HEAD_KEY_IF_MODIFIED_SINCE,
	HTTPD_HEAD_KEY_KEEP_ALIVE,
	HTTPD_HEAD_KEY_CONTENT_LENGTH,
	HTTPD_HEAD_KEY_CONTENT_TYPE,
	HTTPD_HEAD_KEY_CONTENT_DISPOSITION,
	HTTPD_HEAD_KEY_CNT,
}HTTPD_HEAD_KEY;
extern char* HTTPD_HEAD_KEY_STRING[];

typedef enum{
	HTTPD_STATUS_CODE_304_NOT_MODIFIED,
	HTTPD_STATUS_CODE_400_BAD_REQUEST,
	HTTPD_STATUS_CODE_403_FORBIDDEN,
	HTTPD_STATUS_CODE_404_NOT_FOUND,
	HTTPD_STATUS_CODE_501_NOT_IMPLEMENTED,
	HTTPD_STATUS_CODE_503_SERVICE_UNAVAILABLE,
	HTTPD_STATUS_CODE_CNT,
}HTTPD_STATUS_CODE;
extern char* HTTPD_STATUS_CODE_STRING[];


#define HTTPD_SESSION_REQUEST_BUF_SZ (8 * 1024)
#define HTTPD_SESSION_REQUEST_LINE_SZ (8)


typedef struct HTTPD_SESSION
{
	char request_buf[HTTPD_SESSION_REQUEST_BUF_SZ];
	int request_sz;
	
	int sock;
	bool* trigger;
	
	int keep_alive;
//	int method;
//	AVal path_info;
//	AVal path_ext;

	// request line
	HTTP_REQUEST_LINE_t request_line;
}HTTPD_SESSION_t;

extern int HTTPD_init(const char* file_folder);

extern SPOOK_SESSION_PROBE_t HTTPD_probe2(const void* msg, ssize_t msg_sz);
extern SPOOK_SESSION_LOOP_t HTTPD_loop2(bool* trigger, int sock, time_t* read_pts);

typedef int (*CGI_HANDLER)(HTTPD_SESSION_t *session);

extern int HTTPD_add_cgi(const char* name, CGI_HANDLER handler);
extern int HTTPD_remove_cgi(const char* name);


#endif //__HTTPD_H__

