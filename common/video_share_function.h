/*
 * video_share_function.h
 *
 *  Created on: 2011-10-11
 *      Author: root
 */

#ifndef VIDEO_SHARE_FUNCTION_H_
#define VIDEO_SHARE_FUNCTION_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "conf.h"
#include "avenc_types.h"

#pragma pack(4)
typedef struct
{
	void* begin_ptr[MAX_CAM_CH];
	void* writting_ptr[MAX_CAM_CH];
	unsigned int seg_index[MAX_CAM_CH];
}VS_MANAGE;

#define VS_MAGIC_OK (12345)
#define VS_MAGIC_WRITTING (54321)
#define VS_MAGIC_END (88888)
typedef struct
{
	int magic;
	unsigned int seg_index;
	int len;
	unsigned long long utime;
	AVENC_FRAME_TYPE type;
	void* begin;
	void* writting;
}VS_FRAMEHEAD;
#pragma pack()

#define VS_SHM_BASE ((void*)0x60000000)
#ifdef _NVR
#define VS_CHANNEL_BUF_SIZE (1024*1024*3/2)
#else
#define VS_CHANNEL_BUF_SIZE (1024*256)
#endif

//common
extern void* VS_get_begin(int _index);
extern void* VS_get_writting(int _index);

//write
extern void VS_init();
extern void VS_destory();
extern void* VS_reset_chn(int _index);
extern void VS_set_writting(void* _writting, int _index);
extern void VS_add_seg_index(int _index);

//read
extern void VS_at();
extern void VS_dt();
extern void* VS_get_next(void* _last);
extern int VS_is_far(void* _current, int _index);
extern void* VS_get_frame(VS_FRAMEHEAD* _head, int _index, void* _last_addr);
extern unsigned int VS_get_seg_index(int _index);

#ifdef __cplusplus
}
#endif
#endif /* VIDEO_SHARE_FUNCTION_H_ */
