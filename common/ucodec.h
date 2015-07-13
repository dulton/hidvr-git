
#ifndef __UCODEC_H__
#define __UCODEC_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define DVR_DEVID_SHARE "/tmp/DEVID"

extern int UCODEC_write(const char* ucode, ssize_t sz);
extern int UCODEC_read(char* ucode);
extern int UCODEC_check();
extern char* UCODEC_get_id();
#ifdef __cplusplus
};
#endif


#endif //__UCODEC_H__
