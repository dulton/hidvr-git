include ../plat.mk
include ../Makefile.param

#sample
#DEFS +=
#LIBS += 
#INCS += 
#SRCS += 
#OBJS += 
#TARGET += 


DEFS += -DLINUX=2 \
		-D__ARM__ \
		-DARCH=ARM \
		-DARM=$(SDK_PLAT) \

LIBS += 

INCS += 

SRCS := $(wildcard *.c)

OBJS := $(patsubst %.c,%.o, $(SRCS))

TARGET := libjuan.a

CFLAGS += $(INCS) $(DEFS)
LDFLAGS += $(LIBS)

all:$(TARGET)

%.o:%.c
	$(CC) $(CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
sinclude $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(AR) $@ $(OBJS)


.PHONY: clean
clean:
	$(RM) *.o *.d $(TARGET)
