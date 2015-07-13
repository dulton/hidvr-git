PRJ_CROSS       := /src/arm-uclibc/bin/arm-hisi-linux-
BIN_FMT         := oabi
SDK_PLAT        := HI3515
SDK_LIBS        := -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lresampler -lloadbmp -ltde
SDK_FIX         := -march=armv5te
LOAD_ADDRESS    = 0xc1500000
ENTRY_POINT     = 0xc1500000
SUB_ROOTFS_PATH := rootfs_15
SQUASHFS_VER    = 4.2
SQUASHFS_OPT    =
PLAT_SPEC       = lite
