
//arm-hismall-linux-gcc -DLXC_RAW -I../common raw.c -o raw

#ifdef LXC_RAW
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "conf.h"
#include "rec_file_format.h"


int main(int argc, char** argv)
{
	int chn = 0;
	int begin_folder = 0;
	int begin_file = 0;
	int count = 0;
	char* output = NULL;
	char* size_list = NULL;

	if(argc != 7)
	{
		printf("usage: %s chn begin_folder begin_file count output size_list\n", argv[0]);
		printf("example: %s 0 0 0 1 output.264 size_list.txt\n", argv[0]);
		return -1;
	}
	else
	{
		chn = atoi(argv[1]);
		begin_folder = atoi(argv[2]);
		begin_file = atoi(argv[3]);
		count = atoi(argv[4]);
		output = argv[5];
		size_list = argv[6];
	}

	printf("chn=%d, begin_folder=%d, begin_file=%d, count=%d, output=%s, slist=%s\n",
			chn, begin_folder, begin_file, count, output, size_list);

	FILE* output_fp = fopen(output, "w+");
	if(output_fp == NULL)
	{
		perror("%s is error");
		return -1;
	}

	FILE* size_fp = fopen(size_list, "w+");
	if(size_fp == NULL)
	{
		perror("%s is error");
		return -1;
	}

	char file_name[128];
	int curr_folder = begin_folder;
	int curr_file = begin_file;
	int curr_count = 0;
	FILE* fp = NULL;
	FileHeader file_hdr;
	Frame_Head_t frame_hdr;
	unsigned char frame_buf[256*1024];
	int ret = 0;
	while(1)
	{
		sprintf(file_name, "/root/rec/a2/dir%05d/file%04d.dat", curr_folder, curr_file);
		fp = fopen(file_name, "rb");
		if(fp == NULL)
		{
			break;
		}
		printf("open file:%s ok\n", file_name);

		ret = fread(&file_hdr, 1, sizeof(file_hdr), fp);
		if(ret != sizeof(file_hdr))
		{
			printf("sizeof(file_hdr) error, ret=%d\n", ret);
			fclose(fp);
			break;
		}


		if(FILE_HEADER_MAGIC != file_hdr.nMagic || FILE_TAIL_MAGIC != file_hdr.nMagic2)
		{
			printf("FILE_HEADER_MAGIC error");
			fclose(fp);
			break;
		}

		while(1)
		{
			ret = fread(&frame_hdr, 1, sizeof(frame_hdr), fp);
			if(ret != sizeof(frame_hdr))
			{
				printf("sizeof(file_hdr) error, ret=%d\n", ret);
				fclose(fp);
				break;
			}
			if(frame_hdr.magic != FRAME_HEAD_MAGIC || frame_hdr.magic2 != FRAME_TAIL_MAGIC)
			{
				printf("frame_hdr.magic error\n");
				fclose(fp);
				break;
			}
			if(0 == frame_hdr.nSize){
				printf("frame_hdr.nSize is 0\n");
				fclose(fp);
				break;
			}

			if(frame_hdr.channel == chn && AVENC_AUDIO != frame_hdr.frame_type)
			{
				ret = fread(frame_buf, 1, frame_hdr.nSize, fp);
				if(ret != frame_hdr.nSize)
				{
					printf("fread frame_hdr.nSize error\n");
					fclose(fp);
					break;
				}

				ret = fwrite(frame_buf, 1, frame_hdr.nSize, output_fp);
				if(ret != frame_hdr.nSize)
				{
					fclose(fp);
					break;
				}
				sprintf(frame_buf, "%d\r\n", frame_hdr.nSize);
				ret = fwrite(frame_buf, 1, strlen(frame_buf), size_fp);
				if(ret != strlen(frame_buf))
				{
					fclose(fp);
					break;
				}
			}
			else
			{
				fseek(fp, frame_hdr.nSize, SEEK_CUR);
				continue;
			}
		}

		fclose(fp);

		curr_file ++;
		if(curr_file > 127)
		{
			curr_file = 0;
			curr_folder ++;
		}

		curr_count ++;
		if(curr_count >= count)
		{
			break;
		}
	}

	fclose(output_fp);
	fclose(size_fp);
	return 0;
}
#endif
