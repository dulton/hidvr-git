
#ifndef __SDK_MD_H__
#define __SDK_MD_H__

#include "sdk_api.h"

typedef struct SDK_MD_PARA_CH
{
	int vin;
	char name[32];
	int venc;
	uint32_t interleave;
	uint32_t mb_width;
	uint32_t mb_height;
	uint32_t** mb_threshold;
}SDK_MD_PARA_CH_t;

typedef struct SDK_MD_PARA
{
	int md_cnt;
	SDK_MD_PARA_CH_t** md_para;
}SDK_MD_PARA_t;

typedef struct SDK_MD_CALLBACK
{
}SDK_MD_CALLBACK_t;

typedef struct SDK_MD_REPORT
{
	int mb_hit; // motion marco block
	int mb_stats; // total detect maro block
	time_t timestamp;
}SDK_MD_REPORT_t;

extern SDK_API_t SDK_MD_init(const char* solution, SDK_MD_PARA_t* para, SDK_MD_CALLBACK_t* callback);
extern SDK_API_t SDK_MD_destroy();

// look up
extern SDK_API_t SDK_MD_lookup_byname(const char* name, int* ret_ch);

// report
extern SDK_API_t SDK_MD_get_report(int ch, SDK_MD_REPORT_t* ret_report);
extern SDK_API_t SDK_MD_clear_report(int ch);

#endif //__SDK_MD_H__

