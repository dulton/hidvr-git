#include <stdio.h>
#include <stdlib.h>
#include "bufio.h"
#include <string.h>

int init_buffer(BufIO *s,int size)
{
    if(s->buffer) {
        free(s->buffer);
    }
    //printf("size:%d\n",size);
    memset(s,0,sizeof(BufIO));
    s->buffer=(unsigned char *)malloc(size);
    if (!s->buffer) {
        printf("init buffer fail!\n");
        return -1;
    }
    s->buf_ptr=s->buffer;
    s->buffer_size=size;
    s->buf_end=s->buffer+size;
	return size;
}

void flush_buffer(BufIO *s)
{
}

void bufio_put_byte(BufIO *s, int b)
{
    *(s->buf_ptr)++ = b;
    if (s->buf_ptr >= s->buf_end)
        flush_buffer(s);
    if (s->buf_ptr - s->buffer > s->actlen)
		s->actlen=s->buf_ptr-s->buffer;
}

void bufio_put_buffer(BufIO *s, const unsigned char *buf, int size)
{
    int len;
	if(s->buf_end - s->buf_ptr< size)
		len=s->buf_end-s->buf_ptr;
	else
		len=size;
    memcpy(s->buf_ptr, buf, len);
    s->buf_ptr += len;

    if (s->buf_ptr >= s->buf_end)
        flush_buffer(s);

    if (s->buf_ptr - s->buffer > s->actlen)
		s->actlen=s->buf_ptr-s->buffer;
}

void bufio_put_flush_packet(BufIO *s)
{
    flush_buffer(s);
    //s->must_flush = 0;
}

int64_t bufio_url_fseek(BufIO *s, int64_t offset, int whence)
{
	if(whence==1)
		s->buf_ptr+=offset;
	if(whence==2)
		s->buf_ptr=s->buffer+offset;
	
    return s->buf_ptr-s->buffer;
}

void bufio_url_fskip(BufIO *s, int64_t offset)
{
    bufio_url_fseek(s, offset, 1);
}

int64_t bufio_url_ftell(BufIO *s)
{
    return bufio_url_fseek(s, 0, 1);
}

int64_t bufio_url_fsize(BufIO *s)
{
    return s->actlen;
}

int bufio_url_feof(BufIO *s)
{
    if(!s)
        return 0;
    if(s->buf_ptr>=s->buf_end)
		return 1;
	else
		return 0;
}

int bufio_url_ferror(BufIO *s)
{
    if(!s)
        return 0;
    //return s->error;
}

void bufio_put_le32(BufIO *s, unsigned int val)
{
    bufio_put_byte(s, val);
    bufio_put_byte(s, val >> 8);
    bufio_put_byte(s, val >> 16);
    bufio_put_byte(s, val >> 24);
}

void bufio_put_be32(BufIO *s, unsigned int val)
{
    bufio_put_byte(s, val >> 24);
    bufio_put_byte(s, val >> 16);
    bufio_put_byte(s, val >> 8);
    bufio_put_byte(s, val);
}

void bufio_put_strz(BufIO *s, const char *str)
{
    if (str)
        bufio_put_buffer(s, (const unsigned char *) str, strlen(str) + 1);
    else
        bufio_put_byte(s, 0);
}

void bufio_put_le64(BufIO *s, uint64_t val)
{
    bufio_put_le32(s, (uint32_t)(val & 0xffffffff));
    bufio_put_le32(s, (uint32_t)(val >> 32));
}

void bufio_put_be64(BufIO *s, uint64_t val)
{
    bufio_put_be32(s, (uint32_t)(val >> 32));
    bufio_put_be32(s, (uint32_t)(val & 0xffffffff));
}

void bufio_put_le16(BufIO *s, unsigned int val)
{
    bufio_put_byte(s, val);
    bufio_put_byte(s, val >> 8);
}

void bufio_put_be16(BufIO *s, unsigned int val)
{
    //printf("test1\n");
    bufio_put_byte(s, val >> 8);
    bufio_put_byte(s, val);
}

void bufio_put_le24(BufIO *s, unsigned int val)
{
    bufio_put_le16(s, val & 0xffff);
    bufio_put_byte(s, val >> 16);
}

void bufio_put_be24(BufIO *s, unsigned int val)
{
    bufio_put_be16(s, val >> 8);
    bufio_put_byte(s, val);
}

void bufio_put_tag(BufIO *s, const char *tag)
{
    while (*tag) {
        bufio_put_byte(s, *tag++);
    }
}

