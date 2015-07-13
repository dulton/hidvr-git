/*
 * FLV muxer
 * Copyright (c) 2003 The FFmpeg Project
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include "flv.h"
#include "string.h"
#include <math.h>
#include <stdlib.h>

extern int avlib_ff_avc_parse_nal_units_buf(const uint8_t *buf_in, uint8_t **buf, int *size);

void bufio_put_amf_string(BufIO *pb, const char *str)
{
    int len = strlen(str);
    bufio_put_be16(pb, len);
    bufio_put_buffer(pb, (unsigned char *)str, len);
}

int64_t lhav_dbl2int(double d){
    int e;
    if     ( !d) return 0;
    else if(d-d) return 0x7FF0000000000000LL + ((int64_t)(d<0)<<63) + (d!=d);
    d= frexp(d, &e);
    return (int64_t)(d<0)<<63 | (e+1022LL)<<52 | (int64_t)((fabs(d)-0.5)*(1LL<<53));
}

void bufio_put_amf_double(BufIO *pb, double d)
{
    bufio_put_byte(pb, AMF_DATA_TYPE_NUMBER);
    bufio_put_be64(pb, lhav_dbl2int(d));
}

void bufio_put_amf_bool(BufIO *pb, int b) {
    bufio_put_byte(pb, AMF_DATA_TYPE_BOOL);
    bufio_put_byte(pb, !!b);
}

int avlib_flv_write_header(BufIO *pb,int width,int height,int bit_rate,double framerate, double duration)
{
//	int i;
    //double framerate = 0.0;
    int metadata_size_pos, data_size;


    bufio_put_tag(pb,"FLV");
    bufio_put_byte(pb,1);
    bufio_put_byte(pb, FLV_HEADER_FLAG_HASVIDEO);
    bufio_put_be32(pb,9);
    bufio_put_be32(pb,0);

    /* write meta_tag */
    bufio_put_byte(pb, 18);         // tag type META
    metadata_size_pos= bufio_url_ftell(pb);
	//printf("Actlen:%d,ptr:%x,buffer:%x\n",pb->actlen,pb->buf_ptr,pb->buffer);
    bufio_put_be24(pb, 0);          // size of data part (sum of all parts below)
    bufio_put_be24(pb, 0);          // time stamp
    bufio_put_be32(pb, 0);          // reserved
    /* now data of data_size size */

    /* first event name as a string */
    bufio_put_byte(pb, AMF_DATA_TYPE_STRING);
    bufio_put_amf_string(pb, "onMetaData"); // 12 bytes

    /* mixed array (hash) with size and string/type/data tuples */
    bufio_put_byte(pb, AMF_DATA_TYPE_MIXEDARRAY);
    bufio_put_be32(pb, 5 + 2); // +2 for duration and file size

    bufio_put_amf_string(pb, "duration");
    //flv->duration_offset= bufio_url_ftell(pb);
    bufio_put_amf_double(pb, duration); // fill in the guessed duration, it'll be corrected later if incorrect

    bufio_put_amf_string(pb, "width");
    bufio_put_amf_double(pb, width);

    bufio_put_amf_string(pb, "height");
    bufio_put_amf_double(pb, height);

    bufio_put_amf_string(pb, "videodatarate");
    bufio_put_amf_double(pb, bit_rate / 1024.0);

    bufio_put_amf_string(pb, "framerate");
    bufio_put_amf_double(pb, framerate);

    bufio_put_amf_string(pb, "videocodecid");
    bufio_put_amf_double(pb, 7);

    bufio_put_amf_string(pb, "filesize");
    //flv->filesize_offset= bufio_url_ftell(pb);
    bufio_put_amf_double(pb, 0); // delayed write

    bufio_put_amf_string(pb, "");
    bufio_put_byte(pb, AMF_END_OF_OBJECT);

    /* write total size of tag */
    data_size= bufio_url_ftell(pb) - metadata_size_pos - 10;
    bufio_url_fseek(pb, metadata_size_pos, 2);
    bufio_put_be24(pb, data_size);
    bufio_url_fseek(pb, data_size + 10 - 3, 1);
    bufio_put_be32(pb, data_size + 11);


    unsigned char* avcbuf = NULL;
    int avcbuflen = 0;
    FLV_SOLUTION solution = FLV_SOLUTION_CNT;
	unsigned char avcbuf_352x288[24] ={0x01,0x42,0xE0,0x14,0xFF,0xE1,0x00,0x08,0x67,0x42,0xE0,0x14,0xDB,0x05,0x82,0x51,0x01,0x00,0x05,0x68,0xCE,0x30,0xA4,0x80};
	unsigned char avcbuf_352x240[24] ={0x01,0x42,0xE0,0x14,0xFF,0xE1,0x00,0x08,0x67,0x42,0xE0,0x14,0xDB,0x05,0x87,0xC4,0x01,0x00,0x05,0x68,0xCE,0x30,0xA4,0x80};
	unsigned char avcbuf_704x576[25] ={0x01,0x42,0xE0,0x1E,0xFF,0xE1,0x00,0x09,0x67,0x42,0xE0,0x1E,0xDB,0x02,0xC0,0x49,0x10,0x01,0x00,0x05,0x68,0xCE,0x30,0xA4,0x80};
	unsigned char avcbuf_704x480[25] ={0x01,0x42,0xE0,0x1E,0xFF,0xE1,0x00,0x09,0x67,0x42,0xE0,0x1E,0xDB,0x02,0xC0,0xF4,0x40,0x01,0x00,0x05,0x68,0xCE,0x30,0xA4,0x80};
	unsigned char avcbuf_640x352[25] ={0x01,0x42,0xE0,0x1E,0xFF,0xE1,0x00,0x09,0x67,0x42,0xE0,0x1E,0xDA,0x02,0x80,0xB4,0x40,0x01,0x00,0x05,0x68,0xCE,0x30,0xA4,0x80};
	unsigned char avcbuf_1280x720[25]={0x01,0x42,0xE0,0x1F,0xFF,0xE1,0x00,0x09,0x67,0x42,0xE0,0x1F,0xDA,0x01,0x40,0x16,0xC4,0x01,0x00,0x05,0x68,0xCE,0x30,0xA4,0x80};
    if(width == 352 && height == 288)
    {
    	solution = FLV_SOLUTION_352X288;
    	avcbuf = avcbuf_352x288;
    	avcbuflen = sizeof(avcbuf_352x288);
    }
    else if(width == 352 && height == 240)
    {
    	solution = FLV_SOLUTION_352X240;
    	avcbuf = avcbuf_352x240;
    	avcbuflen = sizeof(avcbuf_352x240);
    }
    else if(width == 704 && height == 576)
    {
    	solution = FLV_SOLUTION_704X576;
    	avcbuf = avcbuf_704x576;
    	avcbuflen = sizeof(avcbuf_704x576);
    }
    else if(width == 704 && height == 480)
    {
    	solution = FLV_SOLUTION_704X480;
    	avcbuf = avcbuf_704x480;
    	avcbuflen = sizeof(avcbuf_704x480);
    }
    else if(width == 640 && height == 352)
    {
    	solution = FLV_SOLUTION_640X352;
    	avcbuf = avcbuf_640x352;
    	avcbuflen = sizeof(avcbuf_640x352);
    }
    else if(width == 1280 && height == 720)
    {
    	solution = FLV_SOLUTION_1280X480;
    	avcbuf = avcbuf_1280x720;
    	avcbuflen = sizeof(avcbuf_1280x720);
    }
    else
    {
    	solution = FLV_SOLUTION_352X288;
    	avcbuf = avcbuf_352x288;
    	avcbuflen = sizeof(avcbuf_352x288);
    }

    int64_t pos;
    bufio_put_byte(pb, FLV_TAG_TYPE_VIDEO);
    bufio_put_be24(pb, 0); // size patched later
    bufio_put_be24(pb, 0); // ts
    bufio_put_byte(pb, 0); // ts ext
    bufio_put_be24(pb, 0); // streamid
    pos = bufio_url_ftell(pb);
    bufio_put_byte(pb, 7 | FLV_FRAME_KEY); // flags
    bufio_put_byte(pb, 0); // AVC sequence header
    bufio_put_be24(pb, 0); // composition time
    bufio_put_buffer(pb, avcbuf, avcbuflen);
    data_size = bufio_url_ftell(pb) - pos;
    bufio_url_fseek(pb, -data_size - 10, 1);
    bufio_put_be24(pb, data_size);
    bufio_url_fseek(pb, data_size + 10 - 3, 1);
    bufio_put_be32(pb, data_size + 11); // previous tag size

    return 0;

}

int avlib_flv_write_packet(BufIO *pb, unsigned char *buf,int size,int timestamp,int keyframe)
{
    unsigned ts;
    int flags, flags_size;
	uint8_t *data= NULL;

	flags_size= 5;
	bufio_put_byte(pb, FLV_TAG_TYPE_VIDEO);
	flags = 7;//enc->codec_tag;
	if (keyframe)
		flags |= FLV_FRAME_KEY;
	else
		flags |= FLV_FRAME_INTER;

    ts = timestamp;
    if (avlib_ff_avc_parse_nal_units_buf(buf, &data, &size) < 0)
        return -1;
    bufio_put_be24(pb,size + flags_size);
    bufio_put_be24(pb,ts);
    bufio_put_byte(pb,(ts >> 24) & 0x7F); // timestamps are 32bits _signed_
    bufio_put_be24(pb,0);
    bufio_put_byte(pb,flags);
    bufio_put_byte(pb,1); // AVC NALU
    bufio_put_be24(pb,0);

    bufio_put_buffer(pb, data ? data : buf, size);
	free(data);

    bufio_put_be32(pb,size+flags_size+11); // previous tag size
    return 0;
}

int avlib_flv_write_audio_packet(BufIO *pb, unsigned char *buf,int size, int timestamp, unsigned char conf)
{
	//audio 1
	bufio_put_byte(pb, FLV_TAG_TYPE_AUDIO);

	//data size 3
	bufio_put_be24(pb, size + 1);

	//timestamp 4
	bufio_put_be24(pb,timestamp);
	bufio_put_byte(pb,(timestamp >> 24) & 0x7F); // timestamps are 32bits _signed_

	//stream id 3
	bufio_put_be24(pb, 0);

	//format 1
	//lpcm 11025 16bit mono 0x36
	//lpcm 5500 16bit mono 0x32
	//lpcm 5500 8bit mono 0x30
	//alaw 5500 8bit mono 0x70
	bufio_put_byte(pb, conf);

	//data size
	bufio_put_buffer(pb, buf, size);

	// previous tag size 4
	bufio_put_be32(pb, size + 12);

	return 0;
}
