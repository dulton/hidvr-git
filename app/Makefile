
include ../plat.mk

#cmd define
RM := rm -rf
ECHO := @echo
LINK := ln -s
CP := cp -av
MKDIR := @mkdir -p
MV := mv -v
MKIMAGE := mkimage -A arm -O linux
MAKE := make
PRINTF := @printf "\033[32;1m\t%s\033[0m\n"

#toolchain define
CC    := $(PRJ_CROSS)gcc
CXX   := $(PRJ_CROSS)g++
AR    := $(PRJ_CROSS)ar
STRIP := $(PRJ_CROSS)strip

#dir define
SDK_DIR := $(CURDIR)/sdk_$(SDK_PLAT)
DEBUG_DIR := $(CURDIR)/debug
LIB_DIR := $(CURDIR)/lib
LIB3RD_DIR := $(CURDIR)/lib3rd_$(BIN_FMT)

#src/obj/dep/lib(app only) define
SRC_TMP := $(wildcard *.c)
SRC := $(foreach n,$(SRC_TMP), $(CURDIR)/$(n))

OBJ_TMP := $(patsubst %.c,%.o,$(SRC_TMP))
OBJ := $(foreach n,$(OBJ_TMP), $(DEBUG_DIR)/$(n))

DEP_TMP := $(patsubst %.c,%.d,$(SRC_TMP))
DEP := $(foreach n,$(DEP_TMP), $(DEBUG_DIR)/$(n))


LIB_NOT_USE := hichip
LIB_TMP := mediabuf spook jn_sdk generic sdk_$(SDK_PLAT) jdk
LIB := $(foreach n,$(LIB_TMP), $(LIB_DIR)/lib$(n).a)
LIB_INC := $(foreach n,$(LIB_TMP), -I$(CURDIR)/$(n) -I$(CURDIR)/include/$(n) )
#LIB_INC += -I$(PRJ_PATH)/include -I$(SDK_DIR)/include

LIB3RD_NOT_USE := uuid
LIB3RD_TMP0 += jcm juan
LIB3RD_TMP += h264parser
LIB3RD_TMP += gmodule-2.0 gobject-2.0 gthread-2.0 glib-2.0
LIB3RD_TMP += json
LIB3RD_TMP += base
LIB3RD0 := -L$(LIB3RD_DIR) -L$(CURDIR)/../common $(foreach n,$(LIB3RD_TMP0), -l$(n))
LIB3RD := $(foreach n,$(LIB3RD_TMP), $(LIB3RD_DIR)/lib$(n).a)

#incs/defs/libs(app only) define
INCS := -I$(CURDIR) \
		-I$(CURDIR)/app \
		-I$(CURDIR)/include \
		-I$(CURDIR)/include/base \
		-I$(CURDIR)/include/glib \
		-I$(CURDIR)/include/h264parser \
		-I$(CURDIR)/../common \
		-I$(SDK_DIR)/include \
		$(LIB_INC)
DEFS := -DLINUX=2 -DARM=$(SDK_PLAT) -DTRUE=1 -DFALSE=0 -D_GNU_SOURCE
#LIBS := -L$(SDK_DIR)/lib $(SDK_LIBS)
LIBS := -L$(SDK_DIR)/lib $(SDK_LIBS) -L$(CURDIR)/lib

#cflags/ldflags(app only) define
CFLAGS := -Wall -O0 -g3 $(SDK_FIX)
LDFLAGS := -L$(CURDIR)/../common -ljuan -lpthread -ldl -lm -lstdc++ $(LIBP2P) $(LIBS)

#special target define
.PHONY: clean $(LIB)

all: dvr_app

clean:
	rm -rf debug
	rm -rf lib
	rm -rf dvr_app

dvr_app: $(OBJ) $(LIB)
	$(PRINTF) "LD  $@";
	$(CC) $(OBJ) $(LIB3RD0) $(LIB) $(LIB3RD) $(LDFLAGS) -o $@

$(LIB):
	$(MAKE) -f ../Makefile.lib -C $(subst .a,,$(subst $(LIB_DIR)/lib,,$@))

#common target define
ifneq ($(MAKECMDGOALS), clean)
-include $(DEP)
endif

$(DEBUG_DIR)/%.d: %.c
	$(MKDIR) $(DEBUG_DIR)
	$(PRINTF) "DEP $@";$(CC) -MM -MF"$@" -MT"$(@:.d=.o)" $(INCS) $(DEFS) $<

$(DEBUG_DIR)/%.o: %.c
	$(MKDIR) $(DEBUG_DIR)
	$(PRINTF) "CC  $@";$(CC) -c $(CFLAGS) $(INCS) $(DEFS) -o "$@" $<



