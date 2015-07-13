
#include "sdk_def.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <poll.h>

#ifndef SDK_ENC_H_
#define SDK_ENC_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDK_ENC_BUF_ATTR
{
	// public attr
#define SDK_ENC_BUF_DATA_MAGIC (0xff00a0a0)
	uint32_t magic; // must be zero
	// video
#define SDK_ENC_BUF_DATA_H264 (0x00000000)
#define SDK_ENC_BUF_DATA_JPEG (0x00000001)
	// audio
#define SDK_ENC_BUF_DATA_PCM (0x80000000)
#define SDK_ENC_BUF_DATA_G711A (0x80000001)
#define SDK_ENC_BUF_DATA_G711U (0x80000002)
	uint32_t type;
	uint64_t timestamp_us; // the timestamp of soc engine, unit: us
	uint64_t time_us; // the timestamp of system clock, unit: us
	ssize_t data_sz;
	union
	{
		// TRUE == video_audio
		struct
		{
			uint32_t keyframe; // TRUE / FALSE
			uint32_t ref_counter; // ref frame counter
			uint32_t fps;
			uint32_t width;
			uint32_t height;
		}h264;
		// FALSE == video_audio
		struct
		{
			uint32_t sample_rate;
			uint32_t sample_width;
			uint32_t packet;
			float compress; // if g711a.u == 2
		}pcm, g711a, g711u;
	};
}SDK_ENC_BUF_ATTR_t;

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

typedef enum SDK_ENC_H264_MODE
{
	SDK_ENC_H264_MODE_AUTO = 0,
	SDK_ENC_H264_MODE_VBR,
	SDK_ENC_H264_MODE_CBR,
	SDK_ENC_H264_MODE_ABR,
	SDK_ENC_H264_MODE_FIXQP,
}SDK_ENC_H264_MODE_t;

typedef enum SDK_ENC_H264_PROFILE
{
	SDK_ENC_H264_PROFILE_DEFAULT = 0,
	SDK_ENC_H264_PROFILE_BASELINE,
	SDK_ENC_H264_PROFILE_MAIN,
}SDK_ENC_H264_PROFILE_t;

typedef enum SDK_ENC_H264_REF_MODE
{
	SDK_ENC_H264_REF_MODE_DEFAULT = 0,
	SDK_ENC_H264_REF_MODE_1X = SDK_ENC_H264_REF_MODE_DEFAULT,
	SDK_ENC_H264_REF_MODE_2X,
	SDK_ENC_H264_REF_MODE_4X,
}SDK_ENC_H264_REF_MODE_t;

typedef struct SDK_ENC_H264_STREAM_ATTR
{
	uint32_t magic; // ignore when init
	char name[16]; // ignore when init
	int vin; // ignore when init
	int stream; // ignore when init
	uint32_t vin_fps;
	size_t  width, height;
	uint32_t fps, gop;
	SDK_ENC_H264_PROFILE_t profile;
	SDK_ENC_H264_MODE_t mode;
	SDK_ENC_H264_REF_MODE_t ref_mode;
	uint32_t bps;
	uint32_t quality;
	size_t frame_limit;
	int buf_id;
	bool start;
	
}SDK_ENC_H264_STREAM_ATTR_t;

typedef enum SDK_ENC_AUDIO_BITWIDTH
{
	SDK_ENC_AUDIO_BITWIDTH_DEFAULT = 0,
	SDK_ENC_AUDIO_BITWIDTH_8BITS,
	SDK_ENC_AUDIO_BITWIDTH_16BITS,
}SDK_ENC_AUDIO_BITWIDTH_t;

typedef enum SDK_ENC_AUDIO_SAMPLE_RATE
{
	SDK_ENC_AUDIO_SAMPLE_RATE_DEFAULT = 0,
	SDK_ENC_AUDIO_SAMPLE_RATE_8KBPS,
	SDK_ENC_AUDIO_SAMPLE_RATE_16KBPS,
}SDK_ENC_AUDIO_SAMPLE_RATE_t;

typedef struct SDK_ENC_G711A_STREAM_ATTR
{
	uint32_t magic; // ignore when init
	int ain; // ignore when init
	int vin_ref; // ignore when init
	SDK_ENC_AUDIO_SAMPLE_RATE_t sample_rate;
	SDK_ENC_AUDIO_BITWIDTH_t bit_width;
	size_t packet_size;
	bool start;
	
}SDK_ENC_G711A_STREAM_ATTR_t;

typedef enum SDK_ENC_SNAPSHOT_QUALITY
{
	SDK_ENC_SNAPSHOT_QUALITY_HIGHEST = 0,
	SDK_ENC_SNAPSHOT_QUALITY_HIGH,
	SDK_ENC_SNAPSHOT_QUALITY_MEDIUM,
	SDK_ENC_SNAPSHOT_QUALITY_LOW,
	SDK_ENC_SNAPSHOT_QUALITY_LOWEST,
}SDK_ENC_SNAPSHOT_QUALITY_t;

#define SDK_ENC_SNAPSHOT_DEFAULT_SIZE (0)
#define SDK_ENC_SNAPSHOT_MAX_SIZE (-1)
#define SDK_ENC_SNAPSHOT_MIN_SIZE (-2)

typedef union SDK_ENC_VIDEO_OVERLAY_PIXEL
{
	uint32_t argb8888;
	struct {
		uint8_t blue, green, red, alpha;
	};
}SDK_ENC_VIDEO_OVERLAY_PIXEL_t;

#define SDK_ENC_VIDEO_OVERLAY_PIXEL_RGB(_pixel, _a, _r, _g, _b) \
	do{\
		_pixel.alpha = (_a), _pixel.red = (_r), _pixel.green = (_g), _pixel.blue = (_b);\
	}while(0)

typedef struct SDK_ENC_VIDEO_OVERLAY_PIXEL_FORMAT
{
	uint32_t rmask, gmask, bmask, amask;
}SDK_ENC_VIDEO_OVERLAY_PIXEL_FORMAT_t;

typedef struct SDK_ENC_VIDEO_OVERLAY_CANVAS
{
	size_t width, height;
	SDK_ENC_VIDEO_OVERLAY_PIXEL_FORMAT_t pixel_format;
	void* pixels;

	int (*put_pixel)(struct SDK_ENC_VIDEO_OVERLAY_CANVAS* canvas, int x, int y,
		SDK_ENC_VIDEO_OVERLAY_PIXEL_t pixel);
	int (*get_pixel)(struct SDK_ENC_VIDEO_OVERLAY_CANVAS* canvas, int x, int y,
		SDK_ENC_VIDEO_OVERLAY_PIXEL_t* ret_pixel);
	bool (*match_pixel)(struct SDK_ENC_VIDEO_OVERLAY_CANVAS* canvas, SDK_ENC_VIDEO_OVERLAY_PIXEL_t pixel1, SDK_ENC_VIDEO_OVERLAY_PIXEL_t pixel2);
	int (*put_rect)(struct SDK_ENC_VIDEO_OVERLAY_CANVAS* canvas, int x, int y,
		size_t width, size_t height,SDK_ENC_VIDEO_OVERLAY_PIXEL_t pixel);
	int (*fill_rect)(struct SDK_ENC_VIDEO_OVERLAY_CANVAS* canvas, int x, int y,
		size_t width, size_t height, SDK_ENC_VIDEO_OVERLAY_PIXEL_t pixel);
	int (*erase_rect)(struct SDK_ENC_VIDEO_OVERLAY_CANVAS* canvas, int x, int y, size_t width, size_t height);
	
}SDK_ENC_VIDEO_OVERLAY_CANVAS_t;


typedef struct SDK_ENC_API
{
	// enc buffering callback
	int (*do_buffer_request)(int buf_id, size_t data_sz, bool keyflag);
	int (*do_buffer_attach)(int buf_id, const void* item, ssize_t item_sz);
	int (*do_buffer_commit)(int buf_id);
	//
	int (*lookup_stream_byname)(const char* name, int* ret_vin, int* ret_stream);
	int (*create_h264_stream)(const char* name, int vin, int stream, SDK_ENC_H264_STREAM_ATTR_t* stream_attr);
	int (*release_h264_stream)(int vin, int stream);
	
	int (*start_h264_stream)(int vin, int stream);
	int (*stop_h264_stream)(int vin, int stream);
	
	int (*request_h264_keyframe)(int vin, int stream);

	int (*create_g711a_stream)(int ain, int vin_ref, SDK_ENC_G711A_STREAM_ATTR_t* stream_attr);
	int (*release_g711a_stream)(int ain);
	
	int (*start)(void);
	int (*stop)(void);

	int (*snapshot)(int vin, SDK_ENC_SNAPSHOT_QUALITY_t quality, size_t width, size_t height, FILE* stream);

	// overlay
	SDK_ENC_VIDEO_OVERLAY_CANVAS_t* (*create_overlay_canvas)(size_t width, size_t height);
	void (*release_overlay_canvas)(SDK_ENC_VIDEO_OVERLAY_CANVAS_t* canvas);

	int (*create_overlay)(int vin, int stream, const char* overlay_name,
		int x, int y, size_t width, size_t height, SDK_ENC_VIDEO_OVERLAY_CANVAS_t* const canvas);
	int (*release_overlay)(int vin, int stream, const char* overlay_name);

	SDK_ENC_VIDEO_OVERLAY_CANVAS_t* (*get_overlay_canvas)(int vin, int stream, const char* overlay_name);

	int (*show_overlay)(int vin, int stream, const char* overlay_name, bool show);
	int (*update_overlay)(int vin, int stream, const char* overlay_name);

}SDK_ENC_API_t;

// could be used after 'SDK_init_enc' success to call
__SDK_API SDK_ENC_API_t* sdk_enc;

__SDK_API int SDK_init_enc();
__SDK_API int SDK_destroy_enc();

#ifdef __cplusplus
};
#endif
#endif //SDK_ENC_H_

