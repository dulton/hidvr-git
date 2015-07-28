#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "bsp/gpio_i2c_drv.h"
#define XDBG_DEV "/dev/gpio_i2c"

static int i2c_port_dev = -1;

#define i2c_port_devcheck(x) ((x) >= 0)

static int  HDDVR_LowReadReg(GI2C_ARR_STRUCT * pGI2C)
{
    if (!i2c_port_devcheck(i2c_port_dev) || ioctl(i2c_port_dev, GPIO_I2C_ARR_RD, pGI2C) < 0) {
        printf("ioctl Read Debug Err !!! \n");
        return -1;
    }

  //  printf("Read  Chip:0x%08X Register:0x%08X Value:0x%08X\n", \
        pGI2C->Dev, pGI2C->Buf[0], pGI2C->Buf[1]);

}

static int  HDDVR_LowWriteReg(GI2C_ARR_STRUCT * pGI2C)
{
    if (!i2c_port_devcheck(i2c_port_dev) || ioctl(i2c_port_dev, GPIO_I2C_ARR_WR, pGI2C) < 0) {
        printf("ioctl Write Debug Err !!! \n");
        return -1;
    }

  //  printf("Write Chip:0x%08X Register:0x%08X Value:0x%08X\n", \
        pGI2C->Dev, pGI2C->Buf[0], pGI2C->Buf[1]);
}

int  HDDVR_i2c_port_init(void)
{
    int  tmpDev = open(XDBG_DEV, O_RDWR);

    if(i2c_port_devcheck(tmpDev)) {
	    i2c_port_dev = tmpDev;
	    return 0;
	}

    printf("%s Open Failed\n", XDBG_DEV);
    return -1;
}

void  HDDVR_i2c_port_exit(void)
{
    if(i2c_port_devcheck(i2c_port_dev)) {
        close(i2c_port_dev);
	}
}

unsigned int HDDVR_i2c_WriteByte(unsigned char ucChipAddr, unsigned char ucRegAddr, unsigned char ucRegValue)
{
    GI2C_ARR_STRUCT tmpStruct;

    tmpStruct.Grp   = 0;
    tmpStruct.Dev   = ucChipAddr;
    tmpStruct.RSize = 1;
    tmpStruct.DSize = 1;
	tmpStruct.Buf[0] = ucRegAddr;
	tmpStruct.Buf[1] = ucRegValue;

	return HDDVR_LowWriteReg(&tmpStruct);
}

unsigned char  HDDVR_i2c_ReadByte(unsigned char ucChipAddr, unsigned char ucRegAddr)
{
    GI2C_ARR_STRUCT tmpStruct;

    tmpStruct.Grp   = 0;
    tmpStruct.Dev   = ucChipAddr;
    tmpStruct.RSize = 1;
    tmpStruct.DSize = 1;
	tmpStruct.Buf[0] = ucRegAddr;

	HDDVR_LowReadReg(&tmpStruct);

	return tmpStruct.Buf[1];
}
