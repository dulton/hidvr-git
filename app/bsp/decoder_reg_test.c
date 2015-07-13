#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "decoder_drv.h"

#define XDBG_DEV "/dev/audio"

int  PrintHelp()
{
    puts("HowTo Info.\n"
     "Read  Op: argv[0] r chip addr [number]\n"
     "Write Op: argv[0] w chip addr value\n"
    );
}

int  ReadReg(int Chip, int Addr, int Value)
{
    int  i;
    int  tmpAddr;
    audio_reg_xdbg_t reg_dbg;

    int  tDev = open(XDBG_DEV, O_RDWR);
    if (tDev < 0) {
        printf("%s Open Failed\n", XDBG_DEV);
        return -1;
    }

    printf("CHIP, ADDR, VALUE\n");

    for(i = 0, tmpAddr = Addr; i < Value; i ++, tmpAddr += 4) {
        reg_dbg.chip   = Chip;
        reg_dbg.rw_flg = 1;    //Read Operation Flag
        reg_dbg.addr   = tmpAddr;
        reg_dbg.value  = 0;

        if (ioctl(tDev, AUDIO_DRV_REG_XDBG, &reg_dbg) < 0) {
            printf("ioctl Read Debug Err !!! \n");
            close(tDev);
            return -1;
        }

		printf("%02d, 0x%08X, 0x%08X\n",
            reg_dbg.chip, reg_dbg.addr, reg_dbg.value);
    }

    close(tDev);
}

int  WriteReg(int Chip, int Addr, int  Value)
{
    int  i,j;

    int  tDev = open(XDBG_DEV, O_RDWR);
    if (tDev < 0) {
        printf("%s Open Failed\n", XDBG_DEV);
        return -1;
    }

    audio_reg_xdbg_t reg_dbg;

    reg_dbg.chip   = Chip;
    reg_dbg.rw_flg = 0;  //Write Operation Flag
    reg_dbg.addr   = Addr;
    reg_dbg.value  = Value;

    if (ioctl(tDev, AUDIO_DRV_REG_XDBG, &reg_dbg) < 0) {
        printf("ioctl Write Debug Err !!! \n");
        close(tDev);
        return -1;
    }

    printf("Write Value:0x%08X To Chip.%02d, At Addr:0x%08X Succeed\n",
        reg_dbg.value, reg_dbg.chip, reg_dbg.addr);

    close(tDev);
}


// argv[0] r chip addr [number]
// argv[0] w chip addr value
int  main(int argc, char *argv[])
{
    int  tmpArgc;
    int  tmpRwFlg;

    int  tmpChip = 0;
    int  tmpAddr = 0;
    int  tmpVal  = 1;

    if(argc < 4) {
        PrintHelp();
        return -1;
    }

    tmpRwFlg = 0;
    if(!strcmp(argv[1], "r")) {
        tmpRwFlg = 1;
    }
    if(!strcmp(argv[1], "w") && argc ==5 ) {
        tmpRwFlg = 2;
    }
    if(!tmpRwFlg) {
        PrintHelp();
        return -1;
    }
    
    sscanf(argv[2], "%10X", &tmpChip);
    sscanf(argv[3], "%10X", &tmpAddr);

    if(argc == 5) {
        sscanf(argv[4], "%10X", &tmpVal);
    }

    printf("tmpChip:0x%08X tmpAddr:0x%08X tmpVal:0x%08X\n", \
        tmpChip, tmpAddr, tmpVal);

    if(tmpRwFlg == 1) {
        ReadReg(tmpChip, tmpAddr, tmpVal);
    }
    else {
        WriteReg(tmpChip, tmpAddr, tmpVal);
    }
}
