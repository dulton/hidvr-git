#ifndef __QRCODE_H__
#define __QRCODE_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <regex.h>
#include "dvr_command.h"

int JN_QRCode_init();
int JN_make_QRCode( QRCode_INFO QR_info );
void JN_delete_QRCode();

#ifdef __cplusplus
};
#endif
#endif
