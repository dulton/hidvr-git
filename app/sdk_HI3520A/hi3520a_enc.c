
#include "hi3520a.h"
#include "sdk/sdk_api.h"
#include "sdk_debug.h"

#define HI_VENC_CH_BACKLOG_REF (16)
#define HI_VENC_STREAM_BACKLOG_REF (2)
#define HI_AENC_CH_BACKLOG_REF (16)

#define HI_VENC_JPEG_MIN_WIDTH (64)
#define HI_VENC_JPEG_MAX_WIDTH (8192)
#define HI_VENC_JPEG_MIN_HEIGHT (64)
#define HI_VENC_JPEG_MAX_HEIGHT (8192)

#define HI_VENC_OVERLAY_BACKLOG_REF (OVERLAY_MAX_NUM)
#define HI_VENC_OVERLAY_CANVAS_STOCK_REF (HI_VENC_CH_BACKLOG_REF * HI_VENC_STREAM_BACKLOG_REF * HI_VENC_OVERLAY_BACKLOG_REF)
#define HI_VENC_OVERLAY_HANDLE_OFFSET (1024 - HI_VENC_OVERLAY_CANVAS_STOCK_REF) // 0 - 1024

#define __HI_AOUT_HDMI_DEV (3)
#define __HI_AOUT_HDMI_CHN (0)
#define __HI_AIN_CH (1)
#define __HI_VENC_CH(_vin, _stream) ((_vin)* HI_VENC_STREAM_BACKLOG_REF + (_stream))

#define ENC_H264_STREAM_ATTR_MAGIC (0xf0f0f0f0)

typedef struct SDK_ENC_VIDEO_OVERLAY_ATTR
{
	SDK_ENC_VIDEO_OVERLAY_CANVAS_t* canvas;
	char name[32];
	int x, y;
	size_t width, height;
	RGN_HANDLE region_handle; 
}SDK_ENC_VIDEO_OVERLAY_ATTR_t;

typedef struct SDK_ENC_VIDEO_OVERLAY_ATTR_SET
{
	SDK_ENC_VIDEO_OVERLAY_ATTR_t attr[HI_VENC_OVERLAY_BACKLOG_REF];
}SDK_ENC_VIDEO_OVERLAY_ATTR_SET_t;

typedef struct SDK_ENC_ATTR
{
	SDK_ENC_H264_STREAM_ATTR_t video_stream_attr[HI_VENC_CH_BACKLOG_REF][HI_VENC_STREAM_BACKLOG_REF];
	uint8_t frame_ref_counter[HI_VENC_CH_BACKLOG_REF][HI_VENC_STREAM_BACKLOG_REF];	
	SDK_ENC_G711A_STREAM_ATTR_t audio_stream_attr[HI_AENC_CH_BACKLOG_REF];
	
	SDK_ENC_VIDEO_OVERLAY_ATTR_SET_t video_overlay_set[HI_VENC_CH_BACKLOG_REF][HI_VENC_STREAM_BACKLOG_REF];
	SDK_ENC_VIDEO_OVERLAY_CANVAS_t canvas_stock[HI_VENC_OVERLAY_CANVAS_STOCK_REF];

	bool loop_trigger;
	pthread_t loop_tid;

	int audio_msk;
	bool send_trigger;
	pthread_t send_tid;

	pthread_mutex_t snapshot_mutex;
}SDK_ENC_ATTR_t;

typedef struct SDK_ENC
{
	SDK_ENC_API_t api;
	SDK_ENC_ATTR_t attr;
}SDK_ENC_t;
static SDK_ENC_t _sdk_enc;
extern SDK_ENC_API_t* sdk_enc = NULL;

static inline uint64_t get_time_us()
{
	uint64_t time_us = 0;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	time_us = tv.tv_sec;
	time_us <<= 32;
	time_us += tv.tv_usec;
	return time_us;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

static int do_buffer_request(int buf_id, ssize_t data_sz, uint32_t keyflag)
{
	if(sdk_enc->do_buffer_request){
		return sdk_enc->do_buffer_request(buf_id, data_sz, keyflag);
	}
	return -1;
}
static int do_buffer_attach(int buf_id, const void* item, ssize_t item_sz)
{
	if(sdk_enc->do_buffer_attach){
		return sdk_enc->do_buffer_attach(buf_id, item, item_sz);
	}
	return -1;
}
static int do_buffer_commit(int buf_id)
{
	if(sdk_enc->do_buffer_commit){
		return sdk_enc->do_buffer_commit(buf_id);
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

static int bitrate_regulate(int const enc_width, int const enc_height, int const requested_bps)
{
	int const enc_size = enc_width * enc_height;
	int max_bps = 0;
	if(enc_size <= (180 * 144)){
		max_bps = 256; // kbps
	}else if(enc_size <= (360 * 144)){
		max_bps = 384;
	}else if(enc_size <= (480 * 144)){
		max_bps = 480;
	}else if(enc_size <= (360 * 288)){
		max_bps = 512; // kbps
	}else if(enc_size <= (480 * 288)){
		max_bps = 640;
	}else if(enc_size <= (720 * 288)){
		max_bps = 768;
	}else if(enc_size <= (960 * 288)){
		max_bps = 1024;
	}else if(enc_size <= (720 * 576)){
		max_bps = 1536; // kbps
	}else if(enc_size <= (960 * 576)){
		max_bps = 2048; // kbps
	}
	if(requested_bps < max_bps){
		return requested_bps;
	}
	return max_bps;
}

static int enc_lookup_stream_byname(const char* name, int* ret_vin, int* ret_stream)
{
	int i = 0, ii = 0;
	for(i = 0; i < HI_VENC_CH_BACKLOG_REF; ++i){
		for(ii = 0; ii < HI_VENC_STREAM_BACKLOG_REF; ++ii){
			SDK_ENC_H264_STREAM_ATTR_t* stream_attr = &_sdk_enc.attr.video_stream_attr[i][ii];
			if(ENC_H264_STREAM_ATTR_MAGIC == stream_attr->magic){
				// lookup which is init
				if(0 == strcasecmp(stream_attr->name, name)
					&& strlen(stream_attr->name) == strlen(name)){

					if(ret_vin){
						*ret_vin = i;
					}
					if(ret_stream){
						*ret_stream = ii;
					}
					return SDK_SUCCESS;
				}
			}
		}
	}
	return SDK_FAILURE;
}

static int enc_create_h264_stream(const char* name, int vin, int stream, SDK_ENC_H264_STREAM_ATTR_t* stream_attr)
{
	if(stream_attr && vin < HI_VENC_CH_BACKLOG_REF && stream < HI_VENC_STREAM_BACKLOG_REF){
		SDK_ENC_H264_STREAM_ATTR_t* this_stream_attr = &_sdk_enc.attr.video_stream_attr[vin][stream];
		if(0 == this_stream_attr->magic){
			int const venc_group = vin * HI_VENC_STREAM_BACKLOG_REF + stream;
			int const venc_ch = venc_group;
			int const vpss_group = vin;
			int const vpss_ch = (0 == stream) ? VPSS_BSTR_CHN : VPSS_LSTR_CHN;
			int bps_regular = 0;

			// hisilicon structure
			//VPSS_CHN_MODE_S vpss_chn_mode;
			//VPSS_CHN_ATTR_S vpss_chn_attr;
			MPP_CHN_S mpp_chn_vpss;
			MPP_CHN_S mpp_chn_venc;
			VENC_CHN_ATTR_S venc_chn_attr;
			VENC_ATTR_H264_REF_MODE_E venc_attr_h264_ref_mode;
			VENC_ATTR_S* const p_venc_attr = &venc_chn_attr.stVeAttr; // the attribute of video encoder
			VENC_RC_ATTR_S* const p_venc_rc_attr = &venc_chn_attr.stRcAttr; // the attribute of rate  ctrl
			VENC_ATTR_H264_S* const p_venc_attr_h264 = &p_venc_attr->stAttrH264e;
			VENC_ATTR_H264_CBR_S* const p_h264_cbr = &p_venc_rc_attr->stAttrH264Cbr;
			VENC_ATTR_H264_VBR_S* const p_h264_vbr = &p_venc_rc_attr->stAttrH264Vbr;
			VENC_ATTR_H264_FIXQP_S* const p_h264_fixqp = &p_venc_rc_attr->stAttrH264FixQp;
			VENC_ATTR_H264_ABR_S* const p_h264_abr = &p_venc_rc_attr->stAttrH264Abr;
			
			// only magic is null could be init
			// init this stream attribute;
			memcpy(this_stream_attr, stream_attr, sizeof(SDK_ENC_H264_STREAM_ATTR_t));
			strncpy(this_stream_attr->name, name, sizeof(this_stream_attr->name));
			this_stream_attr->magic = ENC_H264_STREAM_ATTR_MAGIC;
			this_stream_attr->vin = vin;
			this_stream_attr->stream = stream;
			this_stream_attr->start = false; // very important, declare the encode status

			// Greate Venc Group
			SOC_CHECK(HI_MPI_VENC_CreateGroup(venc_group));

			// Create Venc Channel
			memset(&venc_chn_attr, 0, sizeof(venc_chn_attr));
			p_venc_attr->enType = PT_H264; // must be h264 for this interface
			p_venc_attr_h264->u32MaxPicWidth = stream_attr->width;
			p_venc_attr_h264->u32MaxPicHeight = stream_attr->height;
			p_venc_attr_h264->u32PicWidth = stream_attr->width; // the picture width
			p_venc_attr_h264->u32PicHeight = stream_attr->height;// the picture height
			//p_venc_attr_h264->u32BufSize  = p_venc_attr_h264->u32MaxPicWidth * p_venc_attr_h264->u32MaxPicHeight * 3 / 2; // stream buffer size
			p_venc_attr_h264->u32BufSize  = p_venc_attr_h264->u32MaxPicWidth * p_venc_attr_h264->u32MaxPicHeight*3/4; // stream buffer size
			if(0 < (p_venc_attr_h264->u32BufSize%64)) {
				p_venc_attr_h264->u32BufSize += 64 - (p_venc_attr_h264->u32BufSize%64);
			}
			switch(stream_attr->profile){
				default:
				case SDK_ENC_H264_PROFILE_BASELINE:
					p_venc_attr_h264->u32Profile = 0;
					break;
				
				case SDK_ENC_H264_PROFILE_DEFAULT:
				case SDK_ENC_H264_PROFILE_MAIN:
					p_venc_attr_h264->u32Profile = 1;
					break;
			}
			p_venc_attr_h264->bByFrame = HI_TRUE;// get stream mode is slice mode or frame mode
			p_venc_attr_h264->bField = HI_FALSE;  // surpport frame code only for hi3516, bfield = HI_FALSE
			p_venc_attr_h264->bMainStream = HI_TRUE; // surpport main stream only for hi3516, bMainStream = HI_TRUE
			p_venc_attr_h264->u32Priority = 0; // channels precedence level. invalidate for hi3516
			p_venc_attr_h264->bVIField = HI_FALSE; // the sign of the VI picture is field or frame. Invalidate for hi3516

			switch(stream_attr->profile){
				case SDK_ENC_H264_PROFILE_BASELINE:
				{
					p_venc_attr_h264->u32Profile = 0;
					break;
				}
				case SDK_ENC_H264_PROFILE_DEFAULT:
				case SDK_ENC_H264_PROFILE_MAIN:
				{
					p_venc_attr_h264->u32Profile = 1;
					break;
				}
			}

			bps_regular = bitrate_regulate(this_stream_attr->width, this_stream_attr->height, this_stream_attr->bps);
			switch(stream_attr->mode){
				case SDK_ENC_H264_MODE_VBR:
				case SDK_ENC_H264_MODE_AUTO:
				{
					venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBRv2;
					p_h264_vbr->u32Gop = stream_attr->gop;
					p_h264_vbr->u32StatTime = (stream_attr->gop + stream_attr->fps - 1) / stream_attr->fps;
					p_h264_vbr->u32ViFrmRate = stream_attr->vin_fps;
					p_h264_vbr->fr32TargetFrmRate = (typeof(p_h264_vbr->fr32TargetFrmRate))stream_attr->fps;
					p_h264_vbr->u32MaxBitRate = bps_regular * 4 /3;
					p_h264_vbr->u32MinQp = 24;
					p_h264_vbr->u32MaxQp = 32;
					break;
				}

				case SDK_ENC_H264_MODE_CBR:
				case SDK_ENC_H264_MODE_ABR:
				{
					p_venc_rc_attr->enRcMode = VENC_RC_MODE_H264CBRv2;
					p_h264_cbr->u32Gop = stream_attr->gop;
					p_h264_cbr->u32StatTime = (stream_attr->gop + stream_attr->fps - 1) / stream_attr->fps;
					p_h264_cbr->u32ViFrmRate = stream_attr->vin_fps;
					p_h264_cbr->fr32TargetFrmRate = (typeof(p_h264_vbr->fr32TargetFrmRate))stream_attr->fps;
					p_h264_cbr->u32BitRate = bps_regular;
					p_h264_cbr->u32FluctuateLevel = 0;
					break;
				}
				case SDK_ENC_H264_MODE_FIXQP:
				{
					p_venc_rc_attr->enRcMode = VENC_RC_MODE_H264FIXQP;
					p_h264_fixqp->u32Gop = stream_attr->gop;
					p_h264_fixqp->u32ViFrmRate = stream_attr->vin_fps;
					p_h264_fixqp->fr32TargetFrmRate = (typeof(p_h264_vbr->fr32TargetFrmRate))stream_attr->fps;
					p_h264_fixqp->u32IQp = 20;
					p_h264_fixqp->u32PQp = 23;
					break;
				}
				default:
				{
					SOC_TRACE("H264 encode mode %d not supported!", stream_attr->mode);
					break;
				}
			}

			SOC_CHECK(HI_MPI_VENC_CreateChn(venc_ch, &venc_chn_attr));
			SOC_CHECK(HI_MPI_VENC_RegisterChn(venc_ch, venc_group));

			memset(&mpp_chn_vpss, 0, sizeof(mpp_chn_vpss));
			memset(&mpp_chn_venc, 0, sizeof(mpp_chn_venc));
			// binding venc to vpss
			mpp_chn_vpss.enModId = HI_ID_VPSS;
			mpp_chn_vpss.s32DevId = vpss_group;
			mpp_chn_vpss.s32ChnId = vpss_ch;
			mpp_chn_venc.enModId = HI_ID_GROUP;
			mpp_chn_venc.s32DevId = venc_group;
			mpp_chn_venc.s32ChnId = venc_ch;
			SOC_CHECK(HI_MPI_SYS_Bind(&mpp_chn_vpss, &mpp_chn_venc));

			// set h264 reference mode
			SOC_CHECK(HI_MPI_VENC_GetH264eRefMode(venc_ch, &venc_attr_h264_ref_mode));
			switch(stream_attr->ref_mode) {
			case SDK_ENC_H264_REF_MODE_1X:
				venc_attr_h264_ref_mode = H264E_REF_MODE_1X;
				break;
			case SDK_ENC_H264_REF_MODE_2X:
				venc_attr_h264_ref_mode = H264E_REF_MODE_2X;
				break;
			case SDK_ENC_H264_REF_MODE_4X:
				venc_attr_h264_ref_mode = H264E_REF_MODE_4X;
				break;
			}
			SOC_CHECK(HI_MPI_VENC_SetH264eRefMode(venc_ch, venc_attr_h264_ref_mode));

			// create a new video encode engine
			return SDK_SUCCESS;
		}
	}
	return SDK_FAILURE;
}

static int enc_release_h264_stream(int vin, int stream)
{
	SDK_ENC_H264_STREAM_ATTR_t* stream_attr = &_sdk_enc.attr.video_stream_attr[vin][stream];
	if(stream_attr && vin < HI_VENC_CH_BACKLOG_REF && stream < HI_VENC_STREAM_BACKLOG_REF){
		if(ENC_H264_STREAM_ATTR_MAGIC == stream_attr->magic){
			int const venc_ch = __HI_VENC_CH(vin, stream);
			int const venc_group = venc_ch;
			int const vpss_group = vin;
			int const vpss_ch = (venc_ch % HI_VENC_STREAM_BACKLOG_REF); 
			
			MPP_CHN_S mpp_chn_vpss;
			MPP_CHN_S mpp_chn_venc;

			memset(&mpp_chn_vpss, 0, sizeof(mpp_chn_vpss));
			memset(&mpp_chn_venc, 0, sizeof(mpp_chn_venc));
			// unbind venc to vpss
			mpp_chn_vpss.enModId = HI_ID_VPSS;
			mpp_chn_vpss.s32DevId = vpss_group;
			mpp_chn_vpss.s32ChnId = vpss_ch;
			mpp_chn_venc.enModId = HI_ID_GROUP;
			mpp_chn_venc.s32DevId = venc_group;
			mpp_chn_venc.s32ChnId = venc_ch;
			SOC_TRACE("Stoping recv venc %d", venc_ch);
			SOC_CHECK(HI_MPI_VENC_StopRecvPic(venc_ch));
			SOC_CHECK(HI_MPI_SYS_UnBind(&mpp_chn_vpss, &mpp_chn_venc));
			SOC_CHECK(HI_MPI_VENC_UnRegisterChn(venc_ch));
			SOC_CHECK(HI_MPI_VENC_DestroyChn(venc_ch));
			SOC_CHECK(HI_MPI_VENC_DestroyGroup(venc_group));

			// clear the magic
			stream_attr->magic = 0;
			return SDK_SUCCESS;
		}
	}
	return SDK_FAILURE;
}

static int enc_create_g711a_stream(int ain, int vin_ref, SDK_ENC_G711A_STREAM_ATTR_t* stream_attr)
{
	if(ain < HI_AENC_CH_BACKLOG_REF){
		if(vin_ref < HI_VENC_CH_BACKLOG_REF){
			int const aenc_ch = ain;
			SDK_ENC_G711A_STREAM_ATTR_t* const this_stream_attr = &_sdk_enc.attr.audio_stream_attr[ain];
		
			if(0 == this_stream_attr->magic){
				AENC_ATTR_G711_S aenc_attr_g711 = {0};
				AENC_CHN_ATTR_S aenc_ch_attr = {.enType = PT_G711A, .pValue = &aenc_attr_g711,};
				MPP_CHN_S mpp_ch_ai, mpp_ch_aenc;

				aenc_ch_attr.enType = PT_G711A;
				aenc_ch_attr.pValue = &aenc_attr_g711;
				//aenc_ch_attr.u32BufSize = 8;
				//aenc_ch_attr.u32BufSize = MAX_AUDIO_FRAME_NUM; // ver 050 faq 1.6.3
				aenc_ch_attr.u32BufSize = MAX_AUDIO_FRAME_NUM;

				memcpy(this_stream_attr, stream_attr, sizeof(SDK_ENC_G711A_STREAM_ATTR_t));
				this_stream_attr->magic = ENC_H264_STREAM_ATTR_MAGIC; // mark the magic
				this_stream_attr->ain = ain;
				this_stream_attr->vin_ref = vin_ref;
				// FIXME: remember to check the sample rate and bitwidth
				this_stream_attr->sample_rate = SDK_ENC_AUDIO_SAMPLE_RATE_8KBPS;
				this_stream_attr->bit_width = SDK_ENC_AUDIO_BITWIDTH_8BITS;
				this_stream_attr->packet_size = 320;

				// sdk operate
				// enable vin
				SOC_CHECK(HI_MPI_AI_EnableChn(__HI_AIN_CH, ain));
				// create aenc chn
				SOC_CHECK(HI_MPI_AENC_CreateChn(aenc_ch, &aenc_ch_attr));

#if 0 //enable resample
				// bind AENC to AI channel
				mpp_ch_ai.enModId  = HI_ID_AI;
				mpp_ch_ai.s32DevId = __HI_AIN_CH;
				mpp_ch_ai.s32ChnId = ain;

				mpp_ch_aenc.enModId  = HI_ID_AENC;
				mpp_ch_aenc.s32DevId = 0;
				mpp_ch_aenc.s32ChnId = aenc_ch;

				SOC_CHECK(HI_MPI_SYS_Bind(&mpp_ch_ai, &mpp_ch_aenc));
#endif

				_sdk_enc.attr.audio_msk |= 1<<ain;
				return SDK_SUCCESS;	
			}
		}
	}
	return SDK_FAILURE;
}

static int enc_release_g711a_stream(int ain)
{
	if(ain < HI_AENC_CH_BACKLOG_REF){
		int const aenc_ch = ain;
		SDK_ENC_G711A_STREAM_ATTR_t* const this_stream_attr = &_sdk_enc.attr.audio_stream_attr[ain];
		if(ENC_H264_STREAM_ATTR_MAGIC == this_stream_attr->magic){
#if 0//enable resample
			// unbind AENC
			MPP_CHN_S mpp_ch_ai, mpp_ch_aenc;

			mpp_ch_ai.enModId  = HI_ID_AI;
			mpp_ch_ai.s32DevId = __HI_AIN_CH;
			mpp_ch_ai.s32ChnId = ain;

			mpp_ch_aenc.enModId  = HI_ID_AENC;
			mpp_ch_aenc.s32DevId = 0;
			mpp_ch_aenc.s32ChnId = aenc_ch;

			SOC_CHECK(HI_MPI_SYS_UnBind(&mpp_ch_ai, &mpp_ch_aenc));
#endif
			// destroy aenc chn
			SOC_CHECK(HI_MPI_AENC_DestroyChn(aenc_ch));
			// disable vin
			SOC_CHECK(HI_MPI_AI_DisableChn(__HI_AIN_CH, ain));

			// clear flag
			this_stream_attr->magic = 0;
			_sdk_enc.attr.audio_msk &= ~(1<<ain);
			return SDK_SUCCESS;
		}
	}
	return SDK_FAILURE;
}



static int enc_start_h264_stream(int vin, int stream)
{
	SDK_ENC_H264_STREAM_ATTR_t* const stream_attr = &_sdk_enc.attr.video_stream_attr[vin][stream];
	if(!stream_attr->start){
		SOC_CHECK(HI_MPI_VENC_StartRecvPic(__HI_VENC_CH(vin, stream)));
		stream_attr->start = true;
		return SDK_SUCCESS;
	}
	return SDK_FAILURE;
}

static int enc_stop_h264_stream(int vin, int stream)
{
	SDK_ENC_H264_STREAM_ATTR_t* const stream_attr = &_sdk_enc.attr.video_stream_attr[vin][stream];
	if(stream_attr->start){
		SOC_CHECK(HI_MPI_VENC_StopRecvPic(__HI_VENC_CH(vin, stream)));
		stream_attr->start = false;
		return SDK_SUCCESS;
	}
	return SDK_FAILURE;
}

static int enc_request_h264_keyframe(int vin, int stream)
{
	SOC_CHECK(HI_MPI_VENC_RequestIDR(__HI_VENC_CH(vin, stream)));
	return SDK_SUCCESS;
}

static inline ssize_t _stream_size(VENC_STREAM_S venc_stream)
{
	int i = 0;
	ssize_t stream_size = 0;
	for(i = 0; i < venc_stream.u32PackCount; ++i){
		stream_size += venc_stream.pstPack[i].u32Len[0];
		stream_size += venc_stream.pstPack[i].u32Len[1];
	}
	return stream_size;
}

static int enc_video_proc(int vin, int stream)
{
#define HI3520A_MAX_VENC_PACK (4)
	
	SDK_ENC_H264_STREAM_ATTR_t* const venc_ch_attr = &(_sdk_enc.attr.video_stream_attr[vin][stream]);
	uint8_t* const ref_counter = &(_sdk_enc.attr.frame_ref_counter[vin][stream]);
	int const venc_ch = vin * HI_VENC_STREAM_BACKLOG_REF + stream;

	if(ENC_H264_STREAM_ATTR_MAGIC == venc_ch_attr->magic){
		VENC_CHN_STAT_S venc_chn_stat;

		venc_chn_stat.u32CurPacks = HI3520A_MAX_VENC_PACK + 1;
		if(HI_SUCCESS == HI_MPI_VENC_Query(venc_ch, &venc_chn_stat)){
			
			if(venc_chn_stat.u32CurPacks > 0){
				SDK_ENC_BUF_ATTR_t attr;
				VENC_PACK_S venc_pack[HI3520A_MAX_VENC_PACK];
				VENC_STREAM_S venc_stream;
				bool is_keyframe = false;
				
				// get media stream
				memset(&venc_stream, 0, sizeof(venc_stream));
				venc_stream.u32PackCount = HI3520A_MAX_VENC_PACK;
				venc_stream.pstPack = venc_pack;
				SOC_CHECK(HI_MPI_VENC_GetStream(venc_ch, &venc_stream, HI_TRUE));
				is_keyframe = (HI3520A_MAX_VENC_PACK == venc_stream.u32PackCount);

				if(is_keyframe){
					*ref_counter = 0;
				}else{
					// h264 enc is x4 reference
					switch(venc_ch_attr->ref_mode) {
					case SDK_ENC_H264_REF_MODE_1X:
						*ref_counter += 1;
						*ref_counter %= 1;
						break;
					case SDK_ENC_H264_REF_MODE_2X:
						*ref_counter += 1;
						*ref_counter %= 2;
						break;
					case SDK_ENC_H264_REF_MODE_4X:
					default:
						*ref_counter += 1;
						*ref_counter %= 4;
						break;
					}
				}

				if(1){//if(0 == venc_ch_attr->ref_counter || 2 == venc_ch_attr->ref_counter){
					// buffering strream
					attr.magic = SDK_ENC_BUF_DATA_MAGIC;
					attr.type = SDK_ENC_BUF_DATA_H264;
					attr.timestamp_us = venc_stream.pstPack->u64PTS; // get the first nalu pts
					attr.time_us = get_time_us();
					attr.data_sz = _stream_size(venc_stream);
					attr.h264.keyframe = is_keyframe;
					attr.h264.ref_counter = *ref_counter;
					attr.h264.fps = venc_ch_attr->fps;
					attr.h264.width = venc_ch_attr->width;
					attr.h264.height = venc_ch_attr->height;

					if(0 == do_buffer_request(venc_ch_attr->buf_id, attr.data_sz + sizeof(attr), attr.h264.keyframe)){	
						int i = 0;
						// buffer in the attribute
						do_buffer_attach(venc_ch_attr->buf_id, &attr, sizeof(attr));
						// buffer in the payload
						for(i = 0; i < venc_stream.u32PackCount; ++i){
							do_buffer_attach(venc_ch_attr->buf_id, venc_stream.pstPack[i].pu8Addr[0], venc_stream.pstPack[i].u32Len[0]);
							if(venc_stream.pstPack[i].u32Len[1] > 0){
								do_buffer_attach(venc_ch_attr->buf_id, venc_stream.pstPack[i].pu8Addr[1], venc_stream.pstPack[i].u32Len[1]);
							}
						}
						do_buffer_commit(venc_ch_attr->buf_id);
					}
				}
					
				SOC_CHECK(HI_MPI_VENC_ReleaseStream(venc_ch, &venc_stream));
				return SDK_SUCCESS;
			}
		}
	}

	return SDK_FAILURE;
}

static int enc_audio_proc(int ain)
{
	int i = 0;
	SDK_ENC_G711A_STREAM_ATTR_t* const audio_stream_attr = &_sdk_enc.attr.audio_stream_attr[ain];
	if(ENC_H264_STREAM_ATTR_MAGIC == audio_stream_attr->magic){
		AUDIO_STREAM_S audio_stream = {0};
		if(HI_SUCCESS == HI_MPI_AENC_GetStream(ain, &audio_stream, HI_IO_NOBLOCK)){
			SDK_ENC_BUF_ATTR_t attr;
			attr.magic = SDK_ENC_BUF_DATA_MAGIC;
			attr.type = SDK_ENC_BUF_DATA_G711A;
			attr.timestamp_us = audio_stream.u64TimeStamp;; // get the first nalu pts
			attr.time_us = get_time_us();
			attr.data_sz = audio_stream.u32Len - 4; // remove the heading 4bytes of hisilicon g711a
			attr.g711a.sample_rate = 8000; // FIXME:
			attr.g711a.sample_width = 16; // FIXME:
			attr.g711a.packet = attr.data_sz;
			attr.g711a.compress = 2;
	
			// relative to the video stream channel
			// store the audio frame to every relevant video stream
			for(i = 0; i < HI_VENC_STREAM_BACKLOG_REF; ++i){
				SDK_ENC_H264_STREAM_ATTR_t* const video_stream_attr = &_sdk_enc.attr.video_stream_attr[audio_stream_attr->vin_ref][i];
				if(0 == do_buffer_request(video_stream_attr->buf_id, sizeof(SDK_ENC_BUF_ATTR_t) + attr.data_sz, false)){
					do_buffer_attach(video_stream_attr->buf_id, &attr, sizeof(attr));
					//SOC_TRACE("audio len = %d", audio_stream.u32Len);
					// if the g711a packet len is 320
					// the former 4bytes == 0x00a00100
					// if the g711a packet len is 480
					// the former 4bytes == 0x00f00100
					//uint32_t* head = audio_stream.pStream;
					//SOC_TRACE("%08x", *head);
					do_buffer_attach(video_stream_attr->buf_id, audio_stream.pStream + 4, audio_stream.u32Len - 4);
					do_buffer_commit(video_stream_attr->buf_id);			
				}
			}
			SOC_CHECK(HI_MPI_AENC_ReleaseStream(ain, &audio_stream));
			return 0;
		}
	}
	return -1;
}


static void* enc_loop(void* arg)
{
	int i = 0, ii = 0;
	
	while(_sdk_enc.attr.loop_trigger){
		// main stream first
		for(i = 0; i < HI_VENC_STREAM_BACKLOG_REF; ++i){
			for(ii = 0; ii < HI_VENC_CH_BACKLOG_REF; ++ii){
				while(0 == enc_video_proc(ii, i));
			}
		}
		// audio stream
		for(i = 0; i < HI_AENC_CH_BACKLOG_REF; ++i){
			while(0 == enc_audio_proc(i));
		}
		usleep(30000);
	}
	pthread_exit(NULL);
}

static void* send_frame(void *arg)
{
	int i;
	int ret = 0;
	AUDIO_FRAME_S stFrame;
	AI_CHN_PARAM_S stAiChnPara;
	
	for(i = 0; i < HI_AENC_CH_BACKLOG_REF; ++i)
	{
		if(HI_SUCCESS == HI_MPI_AI_GetChnParam(__HI_AIN_CH, i, &stAiChnPara))
		{
			stAiChnPara.u32UsrFrmDepth = 30;
			HI_MPI_AI_SetChnParam(__HI_AIN_CH, i, &stAiChnPara);
		}
	}
	
	while(_sdk_enc.attr.send_trigger && _sdk_enc.attr.audio_msk != 0)
	{
		for(i = 0; i< HI_AENC_CH_BACKLOG_REF; ++i)
		{
			if(!(_sdk_enc.attr.audio_msk & (1<<i)))
			{
				continue;
			}
			
			if(HI_SUCCESS == HI_MPI_AI_GetFrame(__HI_AIN_CH, i, &stFrame, NULL, HI_FALSE))
			{
				if(i == AUDIO_LiveHdmiGet())
				{
					ret = HI_MPI_AO_SendFrame(__HI_AOUT_HDMI_DEV, __HI_AOUT_HDMI_CHN, &stFrame, HI_FALSE);
				}

				ret = HI_MPI_AENC_SendFrame(i, &stFrame, NULL);

				ret = HI_MPI_AI_ReleaseFrame(__HI_AIN_CH, i, &stFrame, NULL);
			}

			if(!_sdk_enc.attr.send_trigger)
			{
				break;
			}
		}
		usleep(10000);

	}
	
	pthread_exit(NULL);
}

static int enc_start()
{
	int ret = 0;
	if(!_sdk_enc.attr.send_tid && !_sdk_enc.attr.loop_tid)
	{
		_sdk_enc.attr.send_trigger = true;
		ret = pthread_create(&_sdk_enc.attr.send_tid, NULL, send_frame, NULL);
		SOC_ASSERT(0 == ret, "send audio frame loop create thread failed!");

		_sdk_enc.attr.loop_trigger = true;
		ret = pthread_create(&_sdk_enc.attr.loop_tid, NULL, enc_loop, NULL);
		SOC_ASSERT(0 == ret, "AV encode do loop create thread failed!");
		
		return SDK_SUCCESS;
	}

	return SDK_FAILURE;
}

static int enc_stop()
{
	if(_sdk_enc.attr.send_tid)
	{
		_sdk_enc.attr.send_trigger = false;
		pthread_join(_sdk_enc.attr.send_tid, NULL);
		_sdk_enc.attr.send_tid = (pthread_t)NULL;
	}
	
	if(_sdk_enc.attr.loop_tid){
		_sdk_enc.attr.loop_trigger = false;
		pthread_join(_sdk_enc.attr.loop_tid, NULL);
		_sdk_enc.attr.loop_tid = (pthread_t)NULL;
	}

	if(_sdk_enc.attr.send_tid || _sdk_enc.attr.loop_tid)
	{
		return SDK_FAILURE;
	}
	else
	{
		return SDK_SUCCESS;
	}
}

static int enc_snapshot(int vin, SDK_ENC_SNAPSHOT_QUALITY_t quality, size_t width, size_t height, FILE* stream)
{
	if(vin < HI_VENC_CH_BACKLOG_REF){
		SDK_ENC_H264_STREAM_ATTR_t* const video_stream_attr = &_sdk_enc.attr.video_stream_attr[vin][0];
		int const venc_grp_jpeg = VENC_MAX_GRP_NUM - 1; // the last venc group
		int const venc_chn_jpeg = VENC_MAX_CHN_NUM - 1; // the last venc channel
		int const vpss_group = vin;
		int const vpss_ch = VPSS_BYPASS_CHN; 
		VENC_CHN_ATTR_S venc_chn_attr;
		VENC_ATTR_JPEG_S* const p_venc_attr_jpeg = &venc_chn_attr.stVeAttr.stAttrJpeg;
		MPP_CHN_S mpp_chn_vpss;
		MPP_CHN_S mpp_chn_venc;

		int const width_min = HI_VENC_JPEG_MIN_WIDTH;
		int const width_max = video_stream_attr->width;
		int const height_min = HI_VENC_JPEG_MIN_HEIGHT;
		int const height_max = video_stream_attr->height;

		pthread_mutex_lock(&_sdk_enc.attr.snapshot_mutex);

		// default size
		if(SDK_ENC_SNAPSHOT_DEFAULT_SIZE == width || SDK_ENC_SNAPSHOT_DEFAULT_SIZE == height){
			width = video_stream_attr->width / 2;
			height = video_stream_attr->height / 2;
		}else{
			if(SDK_ENC_SNAPSHOT_MAX_SIZE == width){
				width = width_max;
			}
			if(SDK_ENC_SNAPSHOT_MAX_SIZE == height){
				height = height_max;
			}
			if(SDK_ENC_SNAPSHOT_MIN_SIZE == width){
				width = width_min;
			}
			if(SDK_ENC_SNAPSHOT_MIN_SIZE == height){
				height = height_min;
			}
		}

		// check the hisilicon size limited
		if(width < width_min){
			width = width_min;
		}
		if(width > width_max){
			width = width_max;
		}
		if(height < height_min){
			height = height_min;
		}
		if(height > height_max){
			height = height_max;
		}

		// create venc group and channel
		venc_chn_attr.stVeAttr.enType = PT_JPEG;
		p_venc_attr_jpeg->u32MaxPicWidth  = width;
		p_venc_attr_jpeg->u32MaxPicHeight = height;
		p_venc_attr_jpeg->u32PicWidth  = width;
		p_venc_attr_jpeg->u32PicHeight = height;
		p_venc_attr_jpeg->u32BufSize = width * height * 3 / 2;
		p_venc_attr_jpeg->bByFrame = HI_TRUE; // get stream mode is field mode	or frame mode
		p_venc_attr_jpeg->bVIField = HI_FALSE; // the sign of the VI picture is field or frame
		p_venc_attr_jpeg->u32Priority = 1; // channels precedence level
		SOC_CHECK(HI_MPI_VENC_CreateGroup(venc_grp_jpeg));
		SOC_CHECK(HI_MPI_VENC_CreateChn(venc_chn_jpeg, &venc_chn_attr));
		SOC_CHECK(HI_MPI_VENC_RegisterChn(venc_grp_jpeg, venc_chn_jpeg));

		// start to receive the picture from vi unit
		mpp_chn_vpss.enModId = HI_ID_VPSS;
		mpp_chn_vpss.s32DevId = vpss_group;
		mpp_chn_vpss.s32ChnId = vpss_ch; // attention here
		mpp_chn_venc.enModId = HI_ID_GROUP;
		mpp_chn_venc.s32DevId = venc_grp_jpeg;
		mpp_chn_venc.s32ChnId = venc_chn_jpeg;
		SOC_CHECK(HI_MPI_SYS_Bind(&mpp_chn_vpss, &mpp_chn_venc));
		SOC_CHECK(HI_MPI_VENC_StartRecvPic(venc_chn_jpeg));

		do
		{
			int i = 0;
			int ret = 0;
			struct timeval select_timeo = { .tv_sec  = 2, .tv_usec = 0, };
			fd_set rfd_set;
			VENC_CHN_STAT_S venc_chn_stat;
			HI_S32 const venc_jpeg_fd = HI_MPI_VENC_GetFd(venc_chn_jpeg);

			FD_ZERO(&rfd_set);
			FD_SET(venc_jpeg_fd, &rfd_set);
			ret = select(venc_jpeg_fd + 1, &rfd_set, NULL, NULL, &select_timeo);
			if(ret < 0){
				SOC_TRACE("Snapshot select failed!");
			}else if (0 == ret){
				SOC_TRACE("Snapshot select timeout!");
			}else{
				if(FD_ISSET(venc_jpeg_fd, &rfd_set)){
					SOC_CHECK(HI_MPI_VENC_Query(venc_chn_jpeg, &venc_chn_stat));
					// here you must keep the u32CurPacks not zero
					if(venc_chn_stat.u32CurPacks > 0){
						VENC_STREAM_S venc_stream;
						venc_stream.u32PackCount = venc_chn_stat.u32CurPacks;
						venc_stream.pstPack = (VENC_PACK_S*)alloca(sizeof(VENC_PACK_S) * venc_stream.u32PackCount);
						SOC_CHECK(HI_MPI_VENC_GetStream(venc_chn_jpeg, &venc_stream, HI_IO_NOBLOCK));
						if(stream){
							for (i = 0; i < venc_stream.u32PackCount; ++i){
								fwrite(venc_stream.pstPack[i].pu8Addr[0], 1, venc_stream.pstPack[i].u32Len[0], stream);
								if(venc_stream.pstPack[i].u32Len[1]){
									fwrite(venc_stream.pstPack[i].pu8Addr[1], 1, venc_stream.pstPack[i].u32Len[1], stream);
								}
							}
							fflush(stream);
						}
						SOC_CHECK(HI_MPI_VENC_ReleaseStream(venc_chn_jpeg, &venc_stream));
					}
				}
			}
		}while(0);

		// destruct
		SOC_CHECK(HI_MPI_VENC_StopRecvPic(venc_chn_jpeg));
		SOC_CHECK(HI_MPI_SYS_UnBind(&mpp_chn_vpss, &mpp_chn_venc));
		SOC_CHECK(HI_MPI_VENC_UnRegisterChn(venc_chn_jpeg));
		SOC_CHECK(HI_MPI_VENC_DestroyChn(venc_chn_jpeg));
		SOC_CHECK(HI_MPI_VENC_DestroyGroup(venc_grp_jpeg));

		pthread_mutex_unlock(&_sdk_enc.attr.snapshot_mutex);
		return SDK_SUCCESS;
	}
	return SDK_FAILURE;
}

static inline uint16_t overlay_pixel_argb4444(SDK_ENC_VIDEO_OVERLAY_PIXEL_t pixel)
{
	return ((pixel.alpha>>4)<<12)|((pixel.red>>4)<<8)|((pixel.green>>4)<<4)|((pixel.blue>>4)<<0);
}

static inline SDK_ENC_VIDEO_OVERLAY_PIXEL_t overlay_pixel_argb8888(uint16_t pixel)
{
	SDK_ENC_VIDEO_OVERLAY_PIXEL_t pixel_8888;
	pixel_8888.alpha = ((pixel>>12)<<4) & 0xff;
	pixel_8888.red = ((pixel>>8)<<4) & 0xff;
	pixel_8888.green = ((pixel>>4)<<4) & 0xff;
	pixel_8888.blue = ((pixel>>0)<<4) & 0xff;
	return pixel_8888;
}


static int overlay_canvas_put_pixel(struct SDK_ENC_VIDEO_OVERLAY_CANVAS* canvas, int x, int y, SDK_ENC_VIDEO_OVERLAY_PIXEL_t pixel)
{
	if(canvas){
		if(x < canvas->width && y < canvas->height){
			uint16_t* const pixels = (uint16_t*)canvas->pixels;
			*(pixels + y * canvas->width + x) = overlay_pixel_argb4444(pixel);
			return SDK_SUCCESS;
		}
	}
	return SDK_FAILURE;
}

static int overlay_canvas_get_pixel(struct SDK_ENC_VIDEO_OVERLAY_CANVAS* canvas, int x, int y, SDK_ENC_VIDEO_OVERLAY_PIXEL_t* ret_pixel)
{
	if(canvas){
		if(x < canvas->width && y < canvas->height){
			uint16_t* const pixels = (uint16_t*)canvas->pixels;
			if(ret_pixel){
				*ret_pixel = overlay_pixel_argb8888(*(pixels + y * canvas->width + x));
				return SDK_SUCCESS;
			}
		}
	}
	return SDK_FAILURE;
}

static int overlay_canvas_match_pixel(struct SDK_ENC_VIDEO_OVERLAY_CANVAS* canvas, SDK_ENC_VIDEO_OVERLAY_PIXEL_t pixel1, SDK_ENC_VIDEO_OVERLAY_PIXEL_t pixel2)
{
	return overlay_pixel_argb4444(pixel1) == overlay_pixel_argb4444(pixel2);
}

static int overlay_canvas_put_rect(struct SDK_ENC_VIDEO_OVERLAY_CANVAS* canvas, int x, int y, size_t width, size_t height,SDK_ENC_VIDEO_OVERLAY_PIXEL_t pixel)
{
	if(canvas){
		if(x < canvas->width && y < canvas->height){
			int i, ii;
			uint16_t* const pixels = (uint16_t*)(canvas->pixels);
			uint16_t const pixel_4444 = overlay_pixel_argb4444(pixel);
			
			if(x + width >= canvas->width){
				width = canvas->width - x;
			}
			if(y + height >= canvas->height){
				height = canvas->height - y;
			}
			
			for(i = 0; i < height; ++i){
				uint16_t* pixel_pos = pixels + i * canvas->width;
				if(0 == i || height - 1 == i){
					// put one line
					for(ii = 0; ii < width; ++ii){
						*pixel_pos++ = pixel_4444;
					}
				}else{
					// put 2 dots
					pixel_pos[0] = pixel_pos[width - 1] = pixel_4444;
				}
			}
			return SDK_SUCCESS;
		}
	}
	return SDK_FAILURE;
}

static int overlay_canvas_fill_rect(struct SDK_ENC_VIDEO_OVERLAY_CANVAS* canvas, int x, int y, size_t width, size_t height, SDK_ENC_VIDEO_OVERLAY_PIXEL_t pixel)
{
	if(canvas){
		if(!width){
			width = canvas->width;
		}
		if(!height){
			height = canvas->height;
		}
		
		if(x < canvas->width && y < canvas->height){
			int i, ii;
			uint16_t* const pixels = (uint16_t*)(canvas->pixels);
			uint16_t const pixel_4444 = overlay_pixel_argb4444(pixel);
			
			if(x + width >= canvas->width){
				width = canvas->width - x;
			}
			if(y + height >= canvas->height){
				height = canvas->height - y;
			}
			
			for(i = 0; i < height; ++i){
				uint16_t* pixel_pos = pixels + i * canvas->width;
				for(ii = 0; ii < width; ++ii){
					*pixel_pos++ = pixel_4444;
				}
			}
			return SDK_SUCCESS;
		}
	}
	return SDK_FAILURE;
}

static int overlay_canvas_erase_rect(struct SDK_ENC_VIDEO_OVERLAY_CANVAS* canvas, int x, int y, size_t width, size_t height)
{
	SDK_ENC_VIDEO_OVERLAY_PIXEL_t erase_pixel;
	erase_pixel.alpha = 0;
	erase_pixel.red = 0;
	erase_pixel.green = 0;
	erase_pixel.blue = 0;
	return canvas->fill_rect(canvas, x, y, width, height, erase_pixel);
}


static SDK_ENC_VIDEO_OVERLAY_CANVAS_t* enc_create_overlay_canvas(size_t width, size_t height)
{
	int i = 0;
	if(width > 0 && height > 0){
		SDK_ENC_VIDEO_OVERLAY_CANVAS_t* const canvas_stock = _sdk_enc.attr.canvas_stock;
		for(i = 0; i < HI_VENC_OVERLAY_CANVAS_STOCK_REF; ++i){
			SDK_ENC_VIDEO_OVERLAY_CANVAS_t* const canvas = canvas_stock + i;
			if(!canvas->pixels){
				// has not been allocated
				SDK_ENC_VIDEO_OVERLAY_CANVAS_t* const canvas = &canvas_stock[i];

				canvas->width = SDK_ALIGNED_BIG_ENDIAN(width, 2); // aligned to 2 pixel
				canvas->height = SDK_ALIGNED_BIG_ENDIAN(height, 2);
				// hisilicon use argb444 format
				canvas->pixel_format.rmask = 0x0f00;
				canvas->pixel_format.gmask = 0x00f0;
				canvas->pixel_format.bmask = 0x000f;
				canvas->pixel_format.amask = 0xf000;
				// frame buffer
				canvas->pixels = calloc(canvas->width * canvas->height * sizeof(uint16_t), 1);
				// interfaces
				canvas->put_pixel = overlay_canvas_put_pixel;
				canvas->get_pixel = overlay_canvas_get_pixel;
				canvas->match_pixel = overlay_canvas_match_pixel;
				canvas->put_rect = overlay_canvas_put_rect;
				canvas->fill_rect = overlay_canvas_fill_rect;
				canvas->erase_rect = overlay_canvas_erase_rect;
				return canvas;
			}
		}
	}
	return NULL;
}

static void enc_release_overlay_canvas(SDK_ENC_VIDEO_OVERLAY_CANVAS_t* canvas)
{
	if(canvas){
		if(canvas->pixels){
			free(canvas->pixels);
			canvas->pixels = NULL; // very important
		}
		// baccaus the canvas is created from the stock
		// so it's needless to be free
	}
}

static SDK_ENC_VIDEO_OVERLAY_ATTR_t* enc_lookup_overlay_byname(int vin, int stream, const char* name)
{
	int i = 0;
	if(vin < HI_VENC_CH_BACKLOG_REF
		&& stream < HI_VENC_STREAM_BACKLOG_REF){
		SDK_ENC_VIDEO_OVERLAY_ATTR_SET_t* const overlay_set = &_sdk_enc.attr.video_overlay_set[vin][stream];
		// check name override
		for(i = 0; i < HI_VENC_OVERLAY_BACKLOG_REF; ++i){
			SDK_ENC_VIDEO_OVERLAY_ATTR_t* const overlay = &overlay_set->attr[i];
			//SDK_TRACE("Looking up \"%s\"/\"%s\"", name, overlay->name);
			if(overlay->canvas && 0 == strcmp(overlay->name, name)){
				// what's my target
				return overlay;
			}
		}
	}
	return NULL;
}
	

static int enc_create_overlay(int vin, int stream, const char* overlay_name,
		int x, int y, size_t width, size_t height, SDK_ENC_VIDEO_OVERLAY_CANVAS_t* const canvas)
{
	int i = 0;
	if(NULL != canvas
		&& vin < HI_VENC_CH_BACKLOG_REF
		&& stream < HI_VENC_STREAM_BACKLOG_REF){
		SDK_ENC_VIDEO_OVERLAY_ATTR_SET_t* const overlay_set = &_sdk_enc.attr.video_overlay_set[vin][stream];
		// check name override
		if(NULL != enc_lookup_overlay_byname(vin, stream, overlay_name)){
			SOC_TRACE("Overlay name %s override", overlay_name);
			return SDK_FAILURE;
		}
		
		// create
		if(!width){
			width = canvas->width;
		}
		if(!height){
			height = canvas->height;
		}

		// alignment
		x = SDK_ALIGNED_BIG_ENDIAN(x, 4);
		y = SDK_ALIGNED_BIG_ENDIAN(y, 4);
		width = SDK_ALIGNED_BIG_ENDIAN(width, 2);
		height = SDK_ALIGNED_BIG_ENDIAN(height, 2);
		
		for(i = 0; i < HI_VENC_OVERLAY_BACKLOG_REF; ++i){
			SDK_ENC_VIDEO_OVERLAY_ATTR_t* const overlay = &overlay_set->attr[i];
			if(!overlay->canvas){
				overlay->canvas = canvas;
				snprintf(overlay->name, sizeof(overlay->name), "%s", overlay_name);
				overlay->x = x;
				overlay->y = y;
				overlay->width = width;
				overlay->height = height;

				if(1){//if(overlay->region_handle >= 0){
					RGN_HANDLE const region_handle = overlay->region_handle;
					RGN_ATTR_S region_attr;
					RGN_CHN_ATTR_S region_ch_ttr;
					MPP_CHN_S mpp_chn_venc;
					BITMAP_S bitmap;

					memset(&region_attr, 0, sizeof(region_attr));
					region_attr.enType = OVERLAY_RGN;
					region_attr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_4444;
					
					region_attr.unAttr.stOverlay.stSize.u32Width = overlay->width;
					region_attr.unAttr.stOverlay.stSize.u32Height = overlay->height;
					region_attr.unAttr.stOverlay.u32BgColor = 0;
					SOC_CHECK(HI_MPI_RGN_Create(region_handle, &region_attr));

					memset(&mpp_chn_venc, 0, sizeof(mpp_chn_venc));
				    mpp_chn_venc.enModId = HI_ID_GROUP;
			        mpp_chn_venc.s32DevId = __HI_VENC_CH(vin, stream);
			        mpp_chn_venc.s32ChnId = 0;

					memset(&region_ch_ttr,0,sizeof(region_ch_ttr));
			        region_ch_ttr.bShow = HI_FALSE;
			        region_ch_ttr.enType = OVERLAY_RGN;
			        region_ch_ttr.unChnAttr.stOverlayChn.stPoint.s32X = overlay->x;
			        region_ch_ttr.unChnAttr.stOverlayChn.stPoint.s32Y = overlay->y;
			        region_ch_ttr.unChnAttr.stOverlayChn.u32BgAlpha = 0;
			        region_ch_ttr.unChnAttr.stOverlayChn.u32FgAlpha = 64;
			        region_ch_ttr.unChnAttr.stOverlayChn.u32Layer = 0;

			        region_ch_ttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = HI_FALSE;
			        region_ch_ttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 0;
					SOC_CHECK(HI_MPI_RGN_AttachToChn(region_handle, &mpp_chn_venc, &region_ch_ttr));

//					SOC_TRACE("Create overlay(%d,%d) @ %d [%dx%d] named \"%s\"",
//						vin, stream, overlay->region_handle, overlay->width, overlay->height, overlay->name);

				}
				return SDK_SUCCESS;
			}
		}
	}
	return SDK_FAILURE;
}

static int enc_release_overlay(int vin, int stream, const char* overlay_name)
{
	if(vin < HI_VENC_CH_BACKLOG_REF
		&& stream < HI_VENC_STREAM_BACKLOG_REF){
		SDK_ENC_VIDEO_OVERLAY_ATTR_t* const overlay = enc_lookup_overlay_byname(vin, stream, overlay_name);
		if(NULL != overlay){
			MPP_CHN_S mpp_chn_venc;

			// only clear the canvas is ok
			// about the canvas release is not my business
			overlay->canvas = NULL;
			memset(&mpp_chn_venc, 0, sizeof(mpp_chn_venc));
			mpp_chn_venc.enModId = HI_ID_GROUP;
			mpp_chn_venc.s32DevId = __HI_VENC_CH(vin, stream);
			mpp_chn_venc.s32ChnId = 0;
			SOC_CHECK(HI_MPI_RGN_DetachFrmChn(overlay->region_handle, &mpp_chn_venc));
			SOC_CHECK(HI_MPI_RGN_Destroy(overlay->region_handle));
			return SDK_SUCCESS;
		}
	}
	return SDK_FAILURE;
}

static SDK_ENC_VIDEO_OVERLAY_CANVAS_t* enc_get_overlay_canvas(int vin, int stream, const char* overlay_name)
{
	int i = 0;
	SDK_ENC_VIDEO_OVERLAY_ATTR_t* overlay = enc_lookup_overlay_byname(vin, stream, overlay_name);
	if(overlay){
		return overlay->canvas;
	}
	return NULL;
}

static int enc_show_overlay(int vin, int stream, const char* overlay_name, bool show)
{
	if(vin < HI_VENC_CH_BACKLOG_REF
		&& stream < HI_VENC_STREAM_BACKLOG_REF){
		SDK_ENC_VIDEO_OVERLAY_ATTR_t* const overlay = enc_lookup_overlay_byname(vin, stream, overlay_name);
		if(NULL != overlay){
			RGN_HANDLE const region_handle = overlay->region_handle;
			MPP_CHN_S mpp_ch_venc;
			RGN_CHN_ATTR_S region_ch_attr;

		    mpp_ch_venc.enModId = HI_ID_GROUP;
		    mpp_ch_venc.s32DevId = __HI_VENC_CH(vin, stream);
		    mpp_ch_venc.s32ChnId = 0;
		    SOC_CHECK(HI_MPI_RGN_GetDisplayAttr(region_handle, &mpp_ch_venc, &region_ch_attr));
		    region_ch_attr.bShow = show ? HI_TRUE : HI_FALSE;
		    SOC_CHECK(HI_MPI_RGN_SetDisplayAttr(region_handle, &mpp_ch_venc, &region_ch_attr));
			return SDK_SUCCESS;
		}
	}
	return SDK_FAILURE;
}

static int enc_update_overlay(int vin, int stream, const char* overlay_name)
{
	if(vin < HI_VENC_CH_BACKLOG_REF
		&& stream < HI_VENC_STREAM_BACKLOG_REF){
		SDK_ENC_VIDEO_OVERLAY_ATTR_t* const overlay = enc_lookup_overlay_byname(vin, stream, overlay_name);
		if(NULL != overlay){
			RGN_HANDLE const region_handle = overlay->region_handle;
			BITMAP_S overlay_bitmap;
			overlay_bitmap.enPixelFormat = PIXEL_FORMAT_RGB_4444;
			overlay_bitmap.u32Width = overlay->canvas->width;
			overlay_bitmap.u32Height = overlay->canvas->height;
			overlay_bitmap.pData = overlay->canvas->pixels;
			SOC_CHECK(HI_MPI_RGN_SetBitMap(region_handle, &overlay_bitmap));
			return SDK_SUCCESS;
		}
	}
	return SDK_FAILURE;
}

static SDK_ENC_t _sdk_enc =
{
	// init the interfaces
	.api = {
		.lookup_stream_byname = enc_lookup_stream_byname,
		.create_h264_stream = enc_create_h264_stream,
		.release_h264_stream = enc_release_h264_stream,
		.start_h264_stream = enc_start_h264_stream,
		.stop_h264_stream = enc_stop_h264_stream,
		.request_h264_keyframe = enc_request_h264_keyframe,
		.create_g711a_stream = enc_create_g711a_stream,
		.release_g711a_stream = enc_release_g711a_stream,
		.start = enc_start,
		.stop = enc_stop,
		.snapshot = enc_snapshot,
		// overlay
		.create_overlay_canvas = enc_create_overlay_canvas,
		.release_overlay_canvas = enc_release_overlay_canvas,
		.create_overlay = enc_create_overlay,
		.release_overlay = enc_release_overlay,
		.get_overlay_canvas = enc_get_overlay_canvas,
		.show_overlay = enc_show_overlay,
		.update_overlay = enc_update_overlay,
	},
};


int SDK_init_enc()
{
	int i = 0, ii = 0, iii = 0;
	// only 'sdk_enc' pointer is NULL could be init
	if(NULL == sdk_enc){
		// set handler pointer
		sdk_enc = (SDK_ENC_API_t*)(&_sdk_enc);
		
		// clear the buffering callback
		sdk_enc->do_buffer_request = NULL;
		sdk_enc->do_buffer_attach = NULL;
		sdk_enc->do_buffer_commit = NULL;
		
		// init the internal attribute value
		// clear the stream attrubutes
		// clear the frame counter
		for(i = 0; i < HI_VENC_CH_BACKLOG_REF; ++i){
			for(ii = 0; ii < HI_VENC_STREAM_BACKLOG_REF; ++ii){
				SDK_ENC_H264_STREAM_ATTR_t* const stream_attr = &_sdk_enc.attr.video_stream_attr[i][ii];
				uint8_t* const frame_ref_counter = &_sdk_enc.attr.frame_ref_counter[i][ii];

				stream_attr->magic = 0;
				*frame_ref_counter = 0;
			}
		}
		// init the overlay set handl
		for(i = 0; i < HI_VENC_CH_BACKLOG_REF; ++i){
			for(ii = 0; ii < HI_VENC_STREAM_BACKLOG_REF; ++ii){
				SDK_ENC_VIDEO_OVERLAY_ATTR_SET_t* const overlay_set = &_sdk_enc.attr.video_overlay_set[i][ii];
				for(iii = 0; iii < HI_VENC_OVERLAY_BACKLOG_REF; ++iii){
					SDK_ENC_VIDEO_OVERLAY_ATTR_t* const overlay = &overlay_set->attr[iii];

					overlay->canvas = NULL;
					memset(overlay->name, 0, sizeof(overlay->name));
					overlay->x = 0;
					overlay->y = 0;
					overlay->width = 0;
					overlay->height = 0;
					// very important, pre-alloc the handle number
					overlay->region_handle = HI_VENC_OVERLAY_HANDLE_OFFSET;
					overlay->region_handle += i * HI_VENC_STREAM_BACKLOG_REF * HI_VENC_OVERLAY_BACKLOG_REF;
					overlay->region_handle += ii * HI_VENC_OVERLAY_BACKLOG_REF;
					overlay->region_handle += iii;
				}
			}

		}
		
		// init the snapshot mutex
		pthread_mutex_init(&_sdk_enc.attr.snapshot_mutex, NULL);
		// success to init
		return SDK_SUCCESS;
	}
	return SDK_FAILURE;
}

int SDK_destroy_enc()
{
	if(sdk_enc){
		int i = 0, ii = 0;
		// destroy the snapshot mutex
		pthread_mutex_destroy(&_sdk_enc.attr.snapshot_mutex);
		// stop encode firstly
		sdk_enc->stop();
		// release the canvas stock
		for(i = 0; i < HI_VENC_OVERLAY_CANVAS_STOCK_REF; ++i){
			sdk_enc->release_overlay_canvas(_sdk_enc.attr.canvas_stock + i);
		}
		
		// release the audio encode
		for(i = 0; i < HI_AENC_CH_BACKLOG_REF; ++i){
			sdk_enc->release_g711a_stream(i);
		}
		// release the video encode
		for(i = 0; i < HI_VENC_CH_BACKLOG_REF; ++i){
			for(ii = 0; ii < HI_VENC_STREAM_BACKLOG_REF; ++ii){
				// destroy sub stream firstly
				sdk_enc->release_h264_stream(i, ii);
			}
		}
		// clear handler pointer
		sdk_enc = NULL;
		// success to destroy
		return SDK_SUCCESS;
	}
	return SDK_FAILURE;
}

