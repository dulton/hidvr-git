/*
 * rec_file_format.h
 *
 *  Created on: 2011-9-27
 *      Author: root
 */

#ifndef REC_FILE_FORMAT_H_
#define REC_FILE_FORMAT_H_

#include "conf.h"
#include "avenc_types.h"

#define FILE_HEADER_MAGIC (0x206f756c) //"luo "
#define FRAME_HEAD_MAGIC (0x2075696c)  //"liu "
#define FILE_TAIL_MAGIC (0x6c756f20) //" oul"
#define FRAME_TAIL_MAGIC (0x6c697520)  //" uil"

typedef enum
{
	REC_TYPE_NONE = 0,
	REC_TYPE_TIMER = 1,
	REC_TYPE_MOTION = 2,
	REC_TYPE_SENSOR = 4,
	REC_TYPE_MANUAL = 8,
}REC_TYPE;

#pragma pack(4)

typedef struct FileHeader
{
	union{
		struct{
			unsigned int nMagic;
			time_t lBeginTime;
			time_t lEndTime;
			int nFirstIDROffset[MAX_REF_CH];
			time_t nFirstIDRGenTime[MAX_REF_CH];
			int nLastIDROffset[MAX_REF_CH];
			time_t nLastIDRGenTime[MAX_REF_CH];
			int nLastSessionID[MAX_REF_CH];
			int nLastSessionRnd[MAX_REF_CH];
			time_t nLastSessionUpdateTime[MAX_REF_CH];
			int nFrameCount[MAX_REF_CH];
			long long nFrameTotalSize[MAX_REF_CH];
		};
		struct
		{
			unsigned int reserved[2048-1];
			unsigned int nMagic2;
		};
	};
}FileHeader;

typedef struct
{
	union
	{
		struct
		{
			unsigned int magic; //头标记
			int session_rnd;    //段随机数
			int frame_width; //分辨率 宽
			int frame_height; //分辨率 高
			int frame_rate; //帧率 如25fps
			int audio_sample_rate; //音频采样率
			char audio_format[8]; //音频格式
			int audio_data_width; //音频数据宽度 8bit/16bit
			AVENC_FRAME_TYPE frame_type; //帧类型，I/P/音频等
			int session_id; //段ID
			int channel; //通道号
			REC_TYPE rec_type; //录像类型，如手动，定时等
			unsigned long long frame_index; //帧在段中的索引
			unsigned int nSize; //帧大小，不包括帧头
			unsigned long long u64TSP;
			unsigned long nGenTime;
			int ref_count; //跳帧参考
		};
		struct
		{
			unsigned int reserved[32-1];
			unsigned int magic2; //尾标记
		};
	};
}Frame_Head_t;

#pragma pack()

#define REC_FILE_PATH "/root/rec"

#endif /* REC_FILE_FORMAT_H_ */
