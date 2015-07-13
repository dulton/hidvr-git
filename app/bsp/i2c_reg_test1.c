#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "gpio_i2c_drv.h"
#define XDBG_DEV "/dev/gpio_i2c"

//Working Mode
//Addr Size: 1/2/4 Byte LE
//Reg  Size: 1/2/4 Byte LE

int  PrintHelp()
{
    puts("HowTo Info.\n"
     "Read  Op: argv[0] r chip addr [num]\n"
     "Write Op: argv[0] w chip addr value\n"
    );
}

int  ReadReg(GI2C_ARR_STRUCT * pGI2C)
{
    int  tDev = open(XDBG_DEV, O_RDWR);
    if (tDev < 0) {
        printf("%s Open Failed\n", XDBG_DEV);
        return -1;
    }

    if (ioctl(tDev, GPIO_I2C_ARR_RD, pGI2C) < 0) {
        printf("ioctl Read Debug Err !!! \n");
        close(tDev);
        return -1;
    }

    printf("Read  Chip:0x%08X Register:0x%08X Value:0x%08X\n", \
        pGI2C->Dev, pGI2C->Buf[0], pGI2C->Buf[1]);

    close(tDev);
}

int  WriteReg(GI2C_ARR_STRUCT * pGI2C)
{
    int  tDev = open(XDBG_DEV, O_RDWR);
    if (tDev < 0) {
        printf("%s Open Failed\n", XDBG_DEV);
        return -1;
    }

    if (ioctl(tDev, GPIO_I2C_ARR_WR, pGI2C) < 0) {
        printf("ioctl Write Debug Err !!! \n");
        close(tDev);
        return -1;
    }

    printf("Write Chip:0x%08X Register:0x%08X Value:0x%08X\n", \
        pGI2C->Dev, pGI2C->Buf[0], pGI2C->Buf[1]);

    close(tDev);
}

// argv[0] r chip addr [num]
// argv[0] w chip addr value
int  main(int argc, char *argv[])
{
    int  tmpRwFlg;
    int  tmpAddr;

    GI2C_ARR_STRUCT tmpStruct;

    if(argc < 3) {
        PrintHelp();
        return -1;
    }

    memset(&tmpStruct, 0, sizeof(tmpStruct));

    tmpRwFlg = 0;
    if(!strcmp(argv[1], "r") && argc == 4 ) {
        tmpRwFlg = 1;
    }
    if(!strcmp(argv[1], "w") && argc == 5 ) {
        tmpRwFlg = 2;
    }
    if(!strcmp(argv[1], "r") && argc == 5 ) {
        tmpRwFlg = 3;
    }
    if(!tmpRwFlg) {
        PrintHelp();
        return -1;
    }

    tmpStruct.Grp   = 1;
    tmpStruct.RSize = 1;
    tmpStruct.DSize = 1;

    sscanf(argv[2], "%10X", &tmpStruct.Dev);
    sscanf(argv[3], "%10X", &tmpAddr);
    tmpStruct.Buf[0] = tmpAddr;

    if(tmpRwFlg == 1) {
        ReadReg(&tmpStruct);
    }
	else if(tmpRwFlg == 3) {
		sscanf(argv[4], "%10X", &tmpAddr);
		while(tmpAddr -- > 0) {
			ReadReg(&tmpStruct);
			tmpStruct.Buf[0] ++;
		}
	}
    else {
		sscanf(argv[4], "%10X", &tmpAddr);
		tmpStruct.Buf[1] = tmpAddr;

        WriteReg(&tmpStruct);
        ReadReg(&tmpStruct);
    }

	return 0;
}
