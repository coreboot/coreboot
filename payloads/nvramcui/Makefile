unexport $(COREBOOT_EXPORTS)

OBJS = $(obj)/nvramcui.o
TARGET = $(obj)/nvramcui.elf

all: real-all

include ../libpayload/Makefile.payload

real-all: $(TARGET)

.PHONY: all real-all
