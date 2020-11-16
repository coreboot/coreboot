##
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
## 3. The name of the author may not be used to endorse or promote products
##    derived from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
## ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
## FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
## DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
## OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
## LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
## OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
## SUCH DAMAGE.
##

# Sample libpayload Makefile.
include ../.config
include ../build/xcompile

ARCH-$(CONFIG_LP_ARCH_ARM)   := arm
ARCH-$(CONFIG_LP_ARCH_X86)   := x86_32
ARCH-$(CONFIG_LP_ARCH_ARM64) := arm64

CC := $(CC_$(ARCH-y))
AS := $(AS_$(ARCH-y))
LIBPAYLOAD_DIR := ../install/libpayload
XCC := CC="$(CC)" $(LIBPAYLOAD_DIR)/bin/lpgcc
XAS := AS="$(AS)" $(LIBPAYLOAD_DIR)/bin/lpas
CFLAGS := -fno-builtin -Wall -Werror -Os
TARGET := hello
OBJS := $(TARGET).o

all: $(TARGET).elf

$(TARGET).elf: $(OBJS)
	$(XCC) -o $@ $(OBJS)

%.o: %.c
	$(XCC) $(CFLAGS) -c -o $@ $<

%.S.o: %.S
	$(XAS) --32 -o $@ $<

clean:
	rm -f $(TARGET).elf *.o

distclean: clean
