#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "gpio_i2c_drv.h"
#define XDBG_DEV "/dev/gpio_i2c"

int  PrintHelp()
{
    puts("HowTo Info.\n"
     "Read  Op: argv[0] r chip addr\n"
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
        pGI2C->Dev,
          (int)pGI2C->Buf[0] << 8
        | (int)pGI2C->Buf[1],
          (int)pGI2C->Buf[5] << 24
        | (int)pGI2C->Buf[4] << 16
        | (int)pGI2C->Buf[3] << 8
        | (int)pGI2C->Buf[2]);

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
        pGI2C->Dev,
          (int)pGI2C->Buf[0] << 8
        | (int)pGI2C->Buf[1],
          (int)pGI2C->Buf[5] << 24
        | (int)pGI2C->Buf[4] << 16
        | (int)pGI2C->Buf[3] << 8
        | (int)pGI2C->Buf[2]);

    close(tDev);
}

// argv[0] r chip addr [number]
// argv[0] w chip addr value
int  main(int argc, char *argv[])
{
    int  tmpArgc;
    int  tmpRwFlg;

    int  tmpGrp;
    int  tmpDev;
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
    if(!tmpRwFlg) {
        PrintHelp();    
        return -1;
    }

    tmpStruct.Grp   = 0;
    tmpStruct.RSize = 2;
    tmpStruct.DSize = 1;

    sscanf(argv[2], "%10X", &tmpStruct.Dev);
    sscanf(argv[3], "%10X", &tmpAddr);

    tmpStruct.Buf[0] = (tmpAddr >> 8);
    tmpStruct.Buf[1] = (tmpAddr);

    if(argc == 5) {
        sscanf(argv[4], "%10X", &tmpAddr);
        tmpStruct.Buf[2] = (tmpAddr & 0xFF);
        tmpStruct.Buf[5] = (tmpAddr >> 24);
        tmpStruct.Buf[4] = (tmpAddr >> 16);
        tmpStruct.Buf[3] = (tmpAddr >> 8);
        tmpStruct.Buf[2] = (tmpAddr);
    }

    if(tmpRwFlg == 1) {
        ReadReg(&tmpStruct);
    }
    else {
        WriteReg(&tmpStruct);
        ReadReg(&tmpStruct);
    }
}
