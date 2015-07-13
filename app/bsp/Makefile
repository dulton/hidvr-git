include	$(PWD)/../../plat.mk

obj-m := hi3515.o gpio.o gpio_i2c.o irda.o decoder.o security.o rtc_pcf8563.o

ifeq "$(SDK_PLAT)" "HI3520"
obj-m += atp862x.o
endif

hi3515-y := hi3515_drv.o
gpio-y := gpio_drv.o \
	gpio_drv_keypad.o
gpio_i2c-y := gpio_i2c_drv.o
irda-y := irda_drv.o
decoder-y := decoder_drv.o
security-y := gi2c.o

.PHOHY: default clean

default:
	@make -C ./kernel M=$(PWD) modules
clean:
	@make -C ./kernel M=$(PWD) clean

