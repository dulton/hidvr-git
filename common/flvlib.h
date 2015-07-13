#ifndef __FLVLIB_HEADER_FILE__
#define __FLVLIB_HEADER_FILE__
#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************/
/*avlib_flv_write_header 生成FLV文件头，调用函数后，读取pb->buffer,pb->actlen为大小*/
/*BufIO *pb  输入缓冲结构                                         */
/*width,height 创建FLV的视频流的宽和高*/
/*bit_rate 创建FLV的视频流的码流*/
/*Framerate 创建FLV的视频流的帧率*/
/******************************************************************/
int avlib_flv_write_header(BufIO *pb,int width,int height,int bit_rate,double framerate,double duration);

/******************************************************************/
/*avlib_flv_write_packet 生成FLV视频流的帧，调用函数后，读取pb->buffer,pb->actlen为大小*/
/*BufIO *pb  输入缓冲结构                                         */
/*unsigned char *buf H.264的裸数据帧*/
/*int size H.264裸数据帧的大小*/
/*int timestamp 时间戳（毫秒），累加的值，例如第一帧为0，第二帧为40，第三帧为80..........*/
/*int keyframe 是否为关键帧，1-I frame,0-P-frame*/
/******************************************************************/
int avlib_flv_write_packet(BufIO *pb, unsigned char *buf,int size,int timestamp,int keyframe);

/******************************************************************/
/*avlib_flv_write_audio_packet 生成FLV音频流的帧，调用函数后，读取pb->buffer,pb->actlen为大小*/
/*BufIO *pb  输入缓冲结构                                         */
/*unsigned char *buf 11025 16bit mono lpcm裸数据帧*/
/*int size 裸数据帧的大小*/
/*int timestamp 时间戳（毫秒），累加的值，例如第一帧为0，第二帧为40，第三帧为80..........*/
/******************************************************************/
int avlib_flv_write_audio_packet(BufIO *pb, unsigned char *buf,int size, int timestamp, unsigned char conf);

/******************************************************************/
/*avlib_writehttpheader 生成HTTP头，通过rheader返回*/
/*int code http返回代码，例如200为OK，400为BAD REQUEST*/
/*char *contenttype 返回内容类型，例如"image/jpeg"*/
/*int contentsize 返回内容的大小，假如是视频流的话，应该是0xffffffff*/
/******************************************************************/
int avlib_writehttpheader(int code,char * contenttype,int contentsize,char *rheader);

/******************************************************************/
/*avlib_httpclientproc 处理HTTP头，通过rheader返回请求URL，例如客户端请求http://192.168.0.1/1.flv那么返回/1.flv*/
/******************************************************************/
int avlib_httpclientproc(char *rheader,char *requestcmd);

#ifdef __cplusplus
}
#endif
#endif
