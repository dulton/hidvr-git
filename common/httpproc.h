#ifndef __HTTPPROC_H__
#define __HTTPPROC_H__

#include <stdio.h>
#include <strings.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

int avlib_httpclientproc(char *rheader,char *requestcmd);
int avlib_writehttpheader(int code,char * contenttype,int contentsize,char *rheader);

#ifdef __cplusplus
}
#endif

#endif //#ifndef __HTTPPROC_H__
