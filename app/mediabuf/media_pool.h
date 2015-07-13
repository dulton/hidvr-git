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


#include "media_entry.h"
#include "media_malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>

#ifndef MEDIA_POOL_H_
#define MEDIA_POOL_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _MEDIA_POOL_IN_CTRL
{
	size_t data_size;
	size_t in_size;
	void* ptr;
	lpRW_LOCK lock;
}stMEDIA_POOL_IN_CTRL, *lpMEDIA_POOL_IN_CTRL;

typedef struct _MEDIA_POOL_USER
{
	const void *pool;
	int entry_rd;
	uint32_t data_rd_id;
	time_t timestamp;
	const void* lock;
}stMEDIA_POOL_USER, *lpMEDIA_POOL_USER;

typedef void (*fMEDIA_POOL_ON_ENTRY_IN)(const char *name, const void *entry_data, size_t entry_size, bool key_flag);
typedef void (*fMEDIA_POOL_ON_ENTRY_OUT)(const char *name, const void *entry_data, size_t entry_size, bool key_flag);
typedef void (*fMEDIA_POOL_ON_ENTRY_CLEAR)(const char *name, const void *entry_data, size_t entry_size, bool key_flag);
typedef void (*fMEDIA_POOL_ON_USER_CHANGED)(const char *name, uint32_t user_number, uint32_t user_max);
typedef void (*fMEDIA_POOL_ON_POOL_EXHAUSTED)(const char *name);

typedef void *(*fMEDIA_CALLOC)(size_t nmemb, size_t size);
typedef void *(*fMEDIA_MALLOC)(size_t size);
typedef void *(*fMEDIA_REALLOC)(void *ptr, size_t size);
typedef void (*fMEDIA_FREE)(void *ptr);

typedef struct _MEDIA_POOL
{
	// some parameters
	char preferred_name[64];
	char alternate_name[64];

	// the full flag of this pool
	bool full_flag;
	bool dataflow_flag;
	uint32_t data_counter;

	uint32_t entry_available, entry_key_available;
	uint32_t entry_in_speed; // unit: us example fps = 25 -> in_speed = 1000'000 / 25 = 40000 us
	uint32_t user_available;
	
	// entry status
	int entry_key_interval_min, entry_key_interval_max;
	lpMEDIA_ENTRY *entries;
	int entry_in; // pointing to next input position, default 0
	int* entry_key_history; // all the key loaction in epointing to the lastest serval key entry s
	lpMEDIA_POOL_IN_CTRL entry_in_ctrl;
	uint32_t entry_counter;
	uint64_t timestamp;

	// user status
	stMEDIA_POOL_USER** users;
	lpRW_LOCK user_op_lock;

	// callback function
	fMEDIA_POOL_ON_ENTRY_IN on_entry_in;
	fMEDIA_POOL_ON_ENTRY_OUT on_entry_out;
	fMEDIA_POOL_ON_ENTRY_CLEAR on_entry_clear;
	fMEDIA_POOL_ON_USER_CHANGED on_user_changed;
	fMEDIA_POOL_ON_POOL_EXHAUSTED on_pool_exhausted;

	// memory alloc
	size_t mem_size;
	mspace mem_space;
	fMEDIA_CALLOC calloc;
	fMEDIA_MALLOC malloc;
	fMEDIA_REALLOC realloc;
	fMEDIA_FREE free;

	// debug
	time_t full_last_time;
	uint32_t discard_count;
	
}stMEDIA_POOL, *lpMEDIA_POOL;

extern lpMEDIA_POOL MEDIAPOOL_create(size_t mem_size, const char *preferred_name, const char *alternate_name, 
	uint32_t howmany_entry, uint32_t howmany_key, uint32_t user_available);
extern void MEDIAPOOL_release(lpMEDIA_POOL const pool);

extern void MEDIAPOOL_hock_entry_in(lpMEDIA_POOL const pool, fMEDIA_POOL_ON_ENTRY_IN on_entry_in);
extern void MEDIAPOOL_hock_entry_out(lpMEDIA_POOL const pool, fMEDIA_POOL_ON_ENTRY_OUT on_entry_out);
extern void MEDIAPOOL_hock_entry_clear(lpMEDIA_POOL const pool, fMEDIA_POOL_ON_ENTRY_CLEAR on_entry_clear);
extern void MEDIAPOOL_hock_user_changed(lpMEDIA_POOL const pool, fMEDIA_POOL_ON_USER_CHANGED on_user_changed);
extern void MEDIAPOOL_hock_pool_exhausted(lpMEDIA_POOL const pool, fMEDIA_POOL_ON_POOL_EXHAUSTED on_pool_exhausted);

extern int MEDIAPOOL_in(lpMEDIA_POOL const pool, const void *data, size_t data_size, bool key_flag, bool wait_block);
extern int MEDIAPOOL_in_request(lpMEDIA_POOL const pool, size_t data_size, bool key_flag, bool wait_block);
extern int MEDIAPOOL_in_append(lpMEDIA_POOL const pool, const void *data_piece, size_t data_piece_size);
extern int MEDIAPOOL_in_commit(lpMEDIA_POOL const pool);

extern int MEDIAPOOL_emtpy(lpMEDIA_POOL const pool);

extern lpMEDIA_POOL_USER MEDIAPOOL_user_attach(lpMEDIA_POOL const pool);
extern void MEDIABUF_user_detach(lpMEDIA_POOL_USER const user);
extern int MEDIABUF_user_sync(lpMEDIA_POOL_USER const user);

extern int MEDIAPOOL_out_lock(lpMEDIA_POOL_USER user);
extern void MEDIAPOOL_out_unlock(lpMEDIA_POOL_USER user);
extern int MEDIAPOOL_out(lpMEDIA_POOL_USER user, void **ret_data_ptr, void* ret_data, size_t *ret_size);

extern void MEDIAPOOL_rw_status(lpMEDIA_POOL const pool);

extern const char *MEDIAPOOL_preferred_name(lpMEDIA_POOL const pool);
extern const char *MEDIAPOOL_alternate_name(lpMEDIA_POOL const pool);

extern uint32_t MEDIAPOOL_in_speed(lpMEDIA_POOL const pool); // unit. us

#ifdef __cplusplus
};
#endif
#endif //MEDIA_POOL_H_

