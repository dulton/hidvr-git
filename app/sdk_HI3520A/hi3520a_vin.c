
#include "sdk/sdk_api.h"
#include "hi3520a.h"
#include "sdk_debug.h"

#define HI3520A_VIN_CH_BACKLOG_REF (16)

typedef struct SDK_VIN_ATTR
{
}SDK_VIN_ATTR_t;

typedef struct SDK_VIN
{
	SDK_VIN_API_t api;
	SDK_VIN_ATTR_t attr;
}SDK_VIN_t;
static SDK_VIN_t _sdk_vin;
extern SDK_VIN_API_t* sdk_vin = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct BIT_MAP_FILE_HEADER
{
	char type[2]; // "BM" (0x4d42)
    uint32_t file_size; //文件大小（字节）
    uint32_t reserved_zero; //保留，值为0
    uint32_t off_bits; //数据区相对于文件头的偏移量（字节）
	uint32_t info_size;
	uint32_t width;
	uint32_t height;
	uint16_t planes; // 0 - 1
	uint16_t bit_count; // 0 - 1
	uint32_t compression; // 0 - 1
	uint32_t size_image; // 0 - 1
	uint32_t xpels_per_meter;
	uint32_t ypels_per_meter;
	uint32_t clr_used;
	uint32_t clr_important;
}__attribute__((packed)) BIT_MAP_FILE_HEADER_t; //

static int vin_capture(int vin, FILE* bitmap_stream)
{
	int ret = 0;
	int api_ret = SDK_FAILURE;
	if(vin < HI3520A_VIN_CH_BACKLOG_REF){
		int frame_depth = 0;
		VIDEO_FRAME_INFO_S video_frame_info;
		

		SOC_CHECK(HI_MPI_VI_GetFrameDepth(vin, &frame_depth));
		if(0 == frame_depth){
			HI_MPI_VI_SetFrameDepth(vin, 1);
		}

		if(HI_SUCCESS == HI_MPI_VI_GetFrame(vin, &video_frame_info)){
			if(PIXEL_FORMAT_YUV_SEMIPLANAR_420 == video_frame_info.stVFrame.enPixelFormat){
				int i = 0, ii = 0;
				int const width = video_frame_info.stVFrame.u32Width;
				int const height = video_frame_info.stVFrame.u32Height;
				size_t const stride = video_frame_info.stVFrame.u32Stride[0];
				size_t const yuv420_size = stride * height * 3 / 2;
				const uint8_t* yuv420_data = HI_MPI_SYS_Mmap(video_frame_info.stVFrame.u32PhyAddr[0], yuv420_size);
				// allocate the bitmap data
				size_t bitmap24_size = width * height * 3;
				uint8_t* bitmap24_data = alloca(bitmap24_size);
				uint8_t* bitmap24_pixel = bitmap24_data;

				SOC_TRACE("Frame(%d) [%dx%d] stride %d", vin, width, height, stride);
				
				if(yuv420_data){
					int y, u, v, yy, vr, ug, vg, ub;
					int r, g, b;
					const uint8_t *py = (uint8_t*)(yuv420_data);
					const uint8_t *puv = (uint8_t*)(py + width * height);

					// yuv420 to rgb888
					for(i = 0; i < height; ++i){
						for(ii = 0; ii < width; ++ii){
							y = py[0];
							yy = y * 256;
							
							u = puv[1] - 128;
							ug = 88 * u;
							ub = 454 * u;

							v = puv[0] - 128;
							vg = 183 * v;
							vr = 359 * v;

							///////////////////////////////////
							// convert
							r = (yy + vr) >> 8;
							g = (yy - ug - vg) >> 8;
							b = (yy + ub) >> 8;

							if(r < 0){
								r = 0;
							}
							if(r > 255){
								r = 255;
							}
							if(g < 0){
								g = 0;
							}
							if(g > 255){
								g = 255;
							}
							if(b < 0){
								b = 0;
							}
							if(b > 255){
								b = 255;
							}

							*bitmap24_pixel++ = (uint8_t)b;
							*bitmap24_pixel++ = (uint8_t)g;
							*bitmap24_pixel++ = (uint8_t)r;

							//SOC_TRACE("RGB[%3d,%3d,%3d] @ (%3d,%3d)", r, g, b, ii, i);

							///////////////////////////////////
							++py;
							if(0 != (ii % 2)){
								// even
								puv += 2;
							}
						}

						if(0 != (i % 2)){
							// try twice
							puv -= width;
						}
					}
					SOC_CHECK(HI_MPI_SYS_Munmap(yuv420_data, yuv420_size));

					if(0 == fseek(bitmap_stream, 0, SEEK_SET)){
						

						BIT_MAP_FILE_HEADER_t bmp_header;
						memset(&bmp_header, 0, sizeof(bmp_header));

						bmp_header.type[0] = 'B';
						bmp_header.type[1] = 'M';
						bmp_header.file_size = sizeof(bmp_header) + bitmap24_size;
						bmp_header.reserved_zero = 0;
						bmp_header.off_bits = sizeof(bmp_header);
						bmp_header.info_size = 40;
						bmp_header.width = width;
						bmp_header.height = height;
						bmp_header.planes = 1;
						bmp_header.bit_count = 24;
						bmp_header.compression = 0;
						bmp_header.size_image = bitmap24_size;
						bmp_header.xpels_per_meter = 0;
						bmp_header.ypels_per_meter = 0;
						bmp_header.clr_used = 0;
						bmp_header.clr_important = 0;

						fwrite(&bmp_header, 1, sizeof(bmp_header), bitmap_stream);
						for(i = 0; i < height; ++i){
							void* bitmap_offset = bitmap24_data + (height - i - 1) * width * 3;
							fwrite(bitmap_offset, 1, width * 3, bitmap_stream);
						}
						
						api_ret = SDK_SUCCESS;
					}
				}
			}
			SOC_CHECK(HI_MPI_VI_ReleaseFrame(vin, &video_frame_info));
		}
		SOC_CHECK(HI_MPI_VI_SetFrameDepth(vin, frame_depth));
	}
	return api_ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static SDK_VIN_t _sdk_vin =
{
	// init the interfaces
	.api =
	{
		.capture = vin_capture,
	},
};


int SDK_init_vin(SDK_VIN_HW_SPEC_t hw_spec)
{
	if(!sdk_vin){
		// set handler pointer
		sdk_vin = (SDK_VIN_API_t*)(&_sdk_vin);

		// FIXME:
		//FILE* fid = fopen("./yuv420.bmp", "w+b");
		//sdk_vin->capture(0, fid);
		//fclose(fid);
		

		return 0;
	}
	return -1;
}

int SDK_destroy_vin()
{
	if(sdk_vin){


		// clear handler pointer
		sdk_vin = NULL;
		return 0L;
	}
	return -1;
}

