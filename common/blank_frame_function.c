/*
 * blank_frame.c
 *
 *  Created on: 2012-3-2
 *      Author: root
 */


#include <string.h>
#include <stdio.h>

#include "blank_frame_function.h"

const char* file_path[BLANK_FRAME_SOLUTION_CNT] =
{
	BLANK_FRAME_FILE_PREFIX"352x288.100frames.264",
	BLANK_FRAME_FILE_PREFIX"352x240.100frames.264",
	BLANK_FRAME_FILE_PREFIX"704x576.100frames.264",
	BLANK_FRAME_FILE_PREFIX"704x480.100frames.264",
};

int file_size[BLANK_FRAME_SOLUTION_CNT][BLANK_FRAME_MAX_COUNT] =
{
	{352, 11,12,12,12,  12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,},
	{303, 11,12,12,12,  12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,},
	{1243,12,12,12,12,1803,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,},
	{1046,12,12,12,12,1506,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,},
};

BLANK_FRAME_CTX blank_frame_ctx[BLANK_FRAME_SOLUTION_CNT];

void BLANK_FRAME_init()
{
	memset(&blank_frame_ctx, 0, sizeof(BLANK_FRAME_CTX) * BLANK_FRAME_SOLUTION_CNT);
	int i;
	for (i = 0; i < BLANK_FRAME_SOLUTION_CNT; i++)
	{
		blank_frame_ctx[i].fp = fopen(file_path[i], "rb");
		blank_frame_ctx[i].sizes = file_size[i];
	}
}

void BLANK_FRAME_destory()
{
	int i;
	for (i = 0; i < BLANK_FRAME_SOLUTION_CNT; i++)
	{
		fclose(blank_frame_ctx[i].fp);
	}
	memset(&blank_frame_ctx, 0, sizeof(BLANK_FRAME_CTX) * BLANK_FRAME_SOLUTION_CNT);
}


void BLANK_FRAME_reset(BLANK_FRAME_SOLUTION _solu)
{
	blank_frame_ctx[_solu].frame_index = 0;
	fseek(blank_frame_ctx[_solu].fp, 0, SEEK_SET);
}

int BLANK_FRAME_get(BLANK_FRAME_SOLUTION _solu, void* _buf)
{
	int ret = fread(_buf, 1, blank_frame_ctx[_solu].sizes[blank_frame_ctx[_solu].frame_index], blank_frame_ctx[_solu].fp);
	blank_frame_ctx[_solu].frame_index ++;
	if (blank_frame_ctx[_solu].frame_index >= BLANK_FRAME_MAX_COUNT)
	{
		blank_frame_ctx[_solu].frame_index = 0;
		fseek(blank_frame_ctx[_solu].fp, 0, SEEK_SET);
	}
	return ret;
}
