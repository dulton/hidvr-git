

#ifndef	__AVENC_H__
#define	__AVENC_H__

#include "jn_sdk/sdk_pri_api.h"

#include "avenc_buf.h"

#define AVENC_REC_SPAN (30.0)

#define FRAME_MAGIC (AVENC_FRAME_MAGIC)

#define AVENC_LOCAL_GOP_TIME (AVENC_BUFFER_GOP_TIME)
#define AVENC_NETWORK_GOP_TIME (4)

#define AVENC_LOCAL_GOP(fr) (fr * AVENC_LOCAL_GOP_TIME)

//
// local stream ->      0 cif high quality
//                      1
//                      2
//                      3
//                      4
//                      5
//                      6
//                      7
// cell phone stream -> 8
// network stream ->    9 cif low quality
//                      10
//                      11
//                      12
//                      13
//                      14
//                      15
//                      16
// cell phone stream -> 17 reserved
//

// using isolate sub stream specially for network transmission
#define AVENC_ISOLATE_NEWORK_STREAM

#define IS_MAIN_VIDEO_STREAM(ch) (((ch)>=0)&&((ch)<MAX_CAM_CH))
#ifdef AVENC_ISOLATE_NEWORK_STREAM
#define IS_SUB_VIDEO_STREAM(ch) (((ch)>=(MAX_CAM_CH))&&((ch)<(MAX_CAM_CH*2)))
#define AVENC_VIDEO_MAX_CH (MAX_CAM_CH * 2)
#define MAX_VENC_GROUP (MAX_CAM_CH)
#define MAIN_CHN_NO(i) (i)
#define SUB_CHN_NO(i) (MAX_CAM_CH+i)
#else
#define IS_SUB_VIDEO_STREAM(ch) (((ch)>=0)&&((ch)<MAX_CAM_CH)) // network and local video stream is the same stream
#define AVENC_VIDEO_MAX_CH (MAX_CAM_CH + 1)
#define MAX_VENC_GROUP (MAX_CAM_CH+1)
#define MAIN_CHN_NO(i) (i)
#define SUB_CHN_NO(i) (i)
#endif  //end of AVENC_ISOLATE_NEWORK_STREAM

// using isolate jpeg snapshot channel
#define AVENC_SNAPSHOT_CH (MAX_CAM_CH*2)
#define IS_SNAPSHOT_VIDEO_STREAM(ch) (AVENC_SNAPSHOT_CH==(ch))

// audio stream count
#define AVENC_AUDIO_MAX_CH (MAX_CAM_CH > 16 ? 16 : MAX_CAM_CH)
#define IS_AUDIO_STREAM(ch) (((ch)>=AVENC_VIDEO_MAX_CH)&&((ch)<(AVENC_VIDEO_MAX_CH+AVENC_AUDIO_MAX_CH)))

#define AVENC_MAX_CH (AVENC_VIDEO_MAX_CH+AVENC_AUDIO_MAX_CH)
#define AVENC_NETWORK_STREAM_MODE (RC_MODE_CBR)
#define AVENC_NETWORK_STREAM_FRAMERATE (MAX_NET_ENC_FPS)
#define AVENC_NETWORK_STREAM_BITRATE (196)
#define AVENC_NETWORK_GOP(fr) (fr*AVENC_NETWORK_GOP_TIME)

#define VENC_MAX_PACK_CNT (4) // relatively to hisilcion SDK
#pragma pack(4)
typedef struct _FrameAttr
{
	unsigned int nMagic; // must be AVENC_FRAME_FLAG, but input 0 is ok
	int nChn; // channel of this frame belong to
	AVENC_FRAME_TYPE enType;
	unsigned int nSize;
	unsigned long long u64TSP; // generation time stamp
	unsigned long nGenTime; // generation time
	unsigned int nGenIndex;
	union
	{
		// entype == IDR / PSLICE
		struct
		{
			unsigned int nFrameRate;
			unsigned int nFrameWidth;
			unsigned int nFrameHeight;
			int nReCounter;
		};
		// entype == AUDIO
		struct
		{
			unsigned int nSampleRate;
			unsigned int nSampleWidth;
			char szEncMode[8];
		};
	};
	AVENC_DATA_TYPE data_type;
	union
	{
		struct
		{
			VENC_STREAM_S v;
			VENC_PACK_S v_packs[VENC_MAX_PACK_CNT];
		}video;
		struct
		{
			AUDIO_STREAM_S a;
		}audio;
		struct
		{
			void* p;
		}buffer;
	}data_handle;
}FrameAttr;
#pragma pack()

extern int AVENC_Init();
extern int AVENC_Destroy();
extern void AVENC_Reset();

extern void AVENC_reload_condition();
extern int AVENC_rec_start(int _type, int _channel);
extern int AVENC_rec_stop(int _channel);
extern int AVENC_running();
extern int AVENC_rec_type(int _channel);
extern int AVENC_has_type(int _value, int _type);

extern int AVENC_get_main_frame_rate(int _chn);
extern int AVENC_get_sub_frame_rate(int _chn);
extern int AVENC_set_main_width(int _chn, int _width);
extern int AVENC_set_sub_width(int _chn, int _width);
extern int AVENC_set_main_height(int _chn, int _height);
extern int AVENC_set_sub_height(int _chn, int _height);

extern int AVENC_SeekFirstIDR(int const nChn);
extern int AVENC_GetFrame(const int nChn, FrameAttr* ret_stAttr, void** ret_pAddr, unsigned char ret_u8Frame[AVENC_MAX_FRAME_SIZE], unsigned char bWait, long lWaitSec, long lWaitNanoSec);

extern int AVENC_PeekFrame(const int nChn, FrameAttr* ret_stAttr, void** ret_pAddr);

extern int AVENC_RequestIDR(int nChn);
extern int AVENC_RequestNetworkIDR(int const nChn);

extern int AVENC_SnapShot(int nChn, const char* pzFileName, int nWidth, int nHeight, int nQuality);


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
extern void AVENC_NetworkGetAttr(int const nChn, int* ret_nFrameRate, int* ret_nGOP, int* ret_nBitRate);
extern void AVENC_NetworkSetAttr(int const nChn, int nFrameRate, int nGOP, int nBitRate);
extern void AVENC_NetworkConnect(int const nChn);
extern void AVENC_NetworkDisconnect(int const nChn);
extern void AVENC_NetworkRefresh(int const nChn);
extern void AVENC_NetworkUpper(int const nChn);
extern void AVENC_NetworkLower(int const nChn);

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// cell phone encode interfaces
extern void AVENC_CellPhoneGetAttr(int* ret_nFrameRate, int* ret_nGOP, int* ret_nBitRate);
extern void AVENC_CellPhoneSetAttr(int nFrameRate, int nGOP, int nBitRate);
extern int AVENC_CellPhoneBegin(int nChn);
extern void AVENC_CellPhoneUpper();
extern void AVENC_CellPhoneLower();
extern void AVENC_CellPhoneRefresh();
extern int AVENC_CellPhoneEnd();

#endif	//__AVENC_H__


