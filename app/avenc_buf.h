


#ifndef __AVENC_BUF_H__
#define __AVENC_BUF_H__

#include <time.h>
#include "conf.h"
#include "avenc_types.h"

#define AVENC_FRAME_MAGIC (0xf5a3ef15)
#ifdef _NVR
#	define AVENC_MAX_FRAME_SIZE (512*1024)
#elif  defined(_EXT_HDDVR)
#	define AVENC_MAX_FRAME_SIZE (1024*1024)
#else
#	if (MAX_D1_CNT > 0)
#		define AVENC_MAX_FRAME_SIZE (256*1024)
#	else
#		if (MAX_CAM_CH >= 16)
#			define AVENC_MAX_FRAME_SIZE (128*1024)
#		else
#			define AVENC_MAX_FRAME_SIZE (64*1024)
#		endif
#	endif
#endif

#define AVENC_BUFFER_GOP_TIME (AVENC_GOP_TIME)
#define AVENC_BUFFER_KEEY_TIME (AVENC_BUFFER_GOP_TIME > 2 ? AVENC_BUFFER_GOP_TIME : 2) // how many frames left in buf

typedef struct _AVFrameMark
{
	unsigned int nMagic; // must be AVENC_FRAME_FLAG, but input 0 is ok
	int nChn; // channel of this frame belong to
	AVENC_FRAME_TYPE enType;
	unsigned int nIndex; // likely to frame count
	unsigned int nSize;
	unsigned long long u64TSP; // generation time stamp
	void* pAddr; // frame data address
	time_t nGenTime; // generation time
	unsigned int nGenIndex; // generation index
}AVFrameMark;


//
// basic status print
//
// RETURN void
//
extern void AVENCBUF_Print();

//
// init/destroy avnec buffer control
//
// RETURN success 0 failed -1
//
extern int AVENCBUF_Init();
extern int AVENCBUF_Destroy();

//
// reset
//
extern void AVENCBUF_Reset(const int nChn);
extern void AVENCBUF_ResetAll();

//
// report how many times the buffer of 'nChn' has overwrited
//
// RETURN the times of how many buffer has overwrited
//
extern unsigned int AVENCBUF_ReportOverWrite(const int nChn);

//
// peek the frame of current location in 'nChn'buffer
//
// RETURN success 0 then you can get frame's parameter from 'ret_stMark'
//        failed -1 maybey buffer is null
//
extern int AVENCBUF_PeekFrame(const int nChn, AVFrameMark* ret_stMark);

//
// insert a frame to avenc buffer of 'nChn' channel with stream mode
//
// RETURN success 0 then the arguement 'ret_stMark' will update, you can get frame's parameter
//        failed -1
//
extern int AVENCBUF_InStreamRequest(AVFrameMark* pstFrMark);
extern void AVENCBUF_InStream(const unsigned char au8Stream[AVENC_MAX_FRAME_SIZE], int nSize);
extern void AVENCBUF_InStreamCommit();

//
// insert a frame to avenc buffer of 'nChn' channel
//
// RETURN success 0 then the arguement 'ret_stMark' will update, you can get frame's parameter
//        failed -1
//
extern int AVENCBUF_InFrame(AVFrameMark* pstMark, const unsigned char au8Frame[AVENC_MAX_FRAME_SIZE]);

//
// get a frame from avenc buffer of 'nChn' channel
//
// RETURN success 0 failed -1
//
extern int AVENCBUF_OutFrame(const int nChn, AVFrameMark* ret_stMark, unsigned char ret_au8Frame[AVENC_MAX_FRAME_SIZE], unsigned char bWait, long lWaitSec, long lWaitNanoSec);


//
// check whether the frame of current location is an IDR
//
// RETURN true 1 false 0
//
extern unsigned char AVENCBUF_CheckIDR(const int nChn);
//
// not skip current frame if it's an IDR to seek the first IDR location
//
// RETURN success 0 failed -1
//
extern int AVENCBUF_SeekFirstIDR(const int nChn);
//
// skip current frame even it's an IDR to seek the first IDR location
//
// RETURN success 0 failed -1
//
extern int AVENCBUF_SeekNextIDR(const int nChn);

//
// get the frames entries int 'nChn' avnec buffer
//
// RETURN if 0 means this buffer is null
//
extern unsigned int AVENCBUF_GetEntries(const int nChn);

#endif //__AVENC_BUF_H__

