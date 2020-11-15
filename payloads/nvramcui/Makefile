unexport $(COREBOOT_EXPORTS)

ARCH = x86_32
OBJS = $(obj)/nvramcui.o
TARGET = $(obj)/nvramcui.elf

all: real-all

include ../libpayload/Makefile.payload

real-all: $(TARGET)

.PHONY: all real-all
