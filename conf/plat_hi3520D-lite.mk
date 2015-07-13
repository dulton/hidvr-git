PRJ_CROSS       := /src/arm-hisiv100/bin/arm-hisiv100-linux-uclibcgnueabi-
BIN_FMT         := eabi
SDK_PLAT        := HI3520D
SDK_LIBS        := -lmpi -lVoiceEngine -laec -lanr -lhdmi -lresampler -ljpeg -ltde -lvqev2
SDK_FIX         := -march=armv7-a -mcpu=cortex-a9
LOAD_ADDRESS    = 0x81000000
ENTRY_POINT     = 0x81000000
SUB_ROOTFS_PATH := rootfs_A9
SQUASHFS_VER    = 4.2
SQUASHFS_OPT    = -b 1048576
PLAT_SPEC       = lite
