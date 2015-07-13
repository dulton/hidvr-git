/*
 * video_share_function.c
 *
 *  Created on: 2011-10-11
 *      Author: root
 */
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "conf.h"
#include "video_share_function.h"

int VS_shm_id = -1;
VS_MANAGE* VS_manage = NULL;

#define VS_FTOK_PATH "//root//dvr_app//dvr_app"
#define VS_MAX_SIZE (VS_CHANNEL_BUF_SIZE * MAX_CAM_CH + sizeof(VS_MANAGE))

void VS_init()
{
	if(VS_shm_id == -1)
	{
		VS_shm_id = shmget(ftok(VS_FTOK_PATH, 0), VS_MAX_SIZE, IPC_CREAT | 0666);
		VS_manage = (VS_MANAGE*)shmat(VS_shm_id, VS_SHM_BASE, 0);
		memset(VS_manage, 0, sizeof(VS_MANAGE));

		int i;
		for(i = 0; i < MAX_CAM_CH; i++)
		{
			VS_manage->begin_ptr[i] = VS_manage + 1;
			VS_manage->begin_ptr[i] += VS_CHANNEL_BUF_SIZE * i;
			VS_manage->writting_ptr[i] = VS_manage->begin_ptr[i];
		}
	}
}

void VS_destory()
{
	shmdt(VS_manage);
	shmctl(VS_shm_id, IPC_RMID, 0);
	VS_shm_id = -1;
}

void VS_at()
{
	if(VS_shm_id == -1)
	{
		VS_shm_id = shmget(ftok(VS_FTOK_PATH, 0), VS_MAX_SIZE, IPC_CREAT | 0666);
//		perror("shmget\n");
		VS_manage = (VS_MANAGE*)shmat(VS_shm_id, VS_SHM_BASE, 0);
//		perror("shmat\n");
	}
}

void VS_dt()
{
	VS_shm_id = -1;
	shmdt(VS_manage);
//	perror("shmdt\n");
}

void* VS_get_begin(int _index)
{
	return VS_manage->begin_ptr[_index];
}

void* VS_get_next(void* _last)
{
	int align = ((unsigned int)_last) % 4;
	if(align != 0)
	{
		align = 4 - align;
	}
	return (_last + align);
}

int VS_is_far(void* _current, int _index)
{
	if(VS_manage->writting_ptr[_index] < _current)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void* VS_reset_chn(int _index)
{
	VS_manage->writting_ptr[_index] = VS_manage->begin_ptr[_index];
	return VS_manage->writting_ptr[_index];
}

void* VS_get_writting(int _index)
{
	return VS_manage->writting_ptr[_index];
}

void VS_set_writting(void* _writting, int _index)
{
	int align = ((unsigned int)_writting) % 4;
	if(align != 0)
	{
		VS_manage->writting_ptr[_index] = _writting + (4 - align);
	}
	else
	{
		VS_manage->writting_ptr[_index] = _writting;
	}
}

void VS_add_seg_index(int _index)
{
	VS_manage->seg_index[_index]++;
}

void* VS_get_frame(VS_FRAMEHEAD* _head, int _index, void* _last_addr)
{
	void* reading_ptr = _last_addr;

	if(reading_ptr == NULL)
	{
		goto get_i_frame_begin;
	}
	else
	{
		goto get_p_frame_begin;
	}


	VS_FRAMEHEAD head;
	int is_far;

get_i_frame_begin:
	reading_ptr = VS_get_begin(_index);

get_i_frame:
	head = *((VS_FRAMEHEAD*)reading_ptr);
//	CGI_TRACE("I vs_fh_ptr=%p, magic=%d, len=%d\n", reading_ptr, head.magic, head.len);
	if(head.magic == VS_MAGIC_OK)
	{
		*_head = head;
		reading_ptr += sizeof(VS_FRAMEHEAD) + head.len;
		return reading_ptr;
	}
	else
	{
		usleep(10);
		goto get_i_frame;
	}

get_p_frame_begin:
	reading_ptr = VS_get_next(reading_ptr);

get_p_frame:
	head = *((VS_FRAMEHEAD*)reading_ptr);
	is_far = VS_is_far(reading_ptr, _index);
//	CGI_TRACE("P vs_fh_ptr=%p, magic=%d, len=%d\n", reading_ptr, head.magic, head.len);
	if(head.magic == VS_MAGIC_OK)
	{
		*_head = head;
		reading_ptr += sizeof(VS_FRAMEHEAD) + head.len;
		return reading_ptr;
	}
	else if(head.magic == VS_MAGIC_WRITTING) //todo:发现死循环在这个地方。
	{
		usleep(10);
		goto get_p_frame;
	}
	else if(head.magic == VS_MAGIC_END)
	{
		usleep(10);
		goto get_i_frame_begin;
	}
	else
	{
		usleep(10);
		goto get_i_frame_begin;
	}
}

unsigned int VS_get_seg_index(int _index)
{
	return VS_manage->seg_index[_index];
}
