/*
*	IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
*
*	By downloading, copying, installing or using the software you agree to this license.
*	If you do not agree to this license, do not download, install,
*	Copy or use the software.
*
*	Copyright (C) 2012, JUAN, Co, All Rights Reserved.
*
*	Project Name:MediaBuf
*	File Name:
*
*	Writed by Frank Law at 2013 - 05 - 09 JUAN,Guangzhou,Guangdong,China
*/

#include "rwlock.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "rwlock.h"
#include <stdint.h>
#include <stdbool.h>

#ifndef MEDIA_ENTRY_H_
#define MEDIA_ENTRY_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _MEDIA_ENTRY
{
#define MEDIA_ENTRY_MAGIC (('L'<<24)|('A'<<16)|('W'<<8)|('L'<<0))
	uint32_t magic;
	// key entry flag
	bool key_flag;
	// payload data
	ssize_t data_size;
	void* data;
	// next pointing
	int next;
	// lock
	lpRW_LOCK rwlock;
}stMEDIA_ENTRY, *lpMEDIA_ENTRY;

extern void MEDIAENTRY_clear(lpMEDIA_ENTRY const entry, int next);

extern lpMEDIA_ENTRY MEDIAENTRY_create();
extern void MEDIAENTRY_destroy(lpMEDIA_ENTRY entry);

#ifdef __cplusplus
};
#endif
#endif //MEDIA_ENTRY_H_

