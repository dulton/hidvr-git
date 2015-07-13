#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <math.h>
#include <errno.h>

#define VIDEO_DEV "/dev/video"

typedef struct video_std
{
    char std[8]; // in "pal" "ntsc" "auto"
	char siz[8]; // "720H" "960H" "FLD1" "HFD1" "FLEF" "HFEF" "720P" "1080P"
}video_std_t;

#define VIDEO_DRV_IOC_MAGIC            'V'
#define VIDEO_DRV_SET_STANDARD _IOW(VIDEO_DRV_IOC_MAGIC, 4, video_std_t)

#define VIDEO_IS_PAL() (1)
#define VIDEO_IS_960() (0)

static int video_StandardInit(void)
{
	// init adc standard pal / ntsc
	int fd = open(VIDEO_DEV, O_RDWR);
	if(fd > 0){
		video_std_t stStd;
		strcpy(stStd.std, (VIDEO_IS_PAL() ? "pal" : "ntsc"));
		strcpy(stStd.siz, (VIDEO_IS_960() ? "960h" : "720h"));
		ioctl(fd, VIDEO_DRV_SET_STANDARD, &stStd);
		close(fd);
		return 0;
	}
	return -1;
}


int  main(void)
{
	return video_StandardInit();
}
