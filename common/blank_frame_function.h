/*
 * blank_frame.h
 *
 *  Created on: 2012-3-2
 *      Author: root
 */

#ifndef BLANK_FRAME_H_
#define BLANK_FRAME_H_


typedef struct
{
	int frame_index;
	FILE* fp;
	int* sizes;
}BLANK_FRAME_CTX;

typedef enum
{
	BLANK_FRAME_SOLUTION_352X288,
	BLANK_FRAME_SOLUTION_352X240,
	BLANK_FRAME_SOLUTION_704X576,
	BLANK_FRAME_SOLUTION_704X480,
	BLANK_FRAME_SOLUTION_CNT,
}BLANK_FRAME_SOLUTION;

#define BLANK_FRAME_MAX_COUNT (100)
#define BLANK_FRAME_MAX_FRAME_SIZE (2*1024)
#define BLANK_FRAME_FILE_PREFIX "/root/dvr_web/www/"

void BLANK_FRAME_init();
void BLANK_FRAME_destory();
void BLANK_FRAME_reset(BLANK_FRAME_SOLUTION _solu);
int BLANK_FRAME_get(BLANK_FRAME_SOLUTION _solu, void* _buf);

#endif /* BLANK_FRAME_H_ */
