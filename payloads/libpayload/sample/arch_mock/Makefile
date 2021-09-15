# SPDX-License-Identifier: GPL-2.0-only

# Sample libpayload Makefile for ARCH_MOCK
# ARCH_MOCK is not intended to be used with xcompile
include ../../.config

ifneq ($(CONFIG_LP_ARCH_MOCK),y)
$(error This example supports ARCH_MOCK only.)
endif

CC := gcc
AS := as
OBJCOPY := objcopy
LIBPAYLOAD_DIR := ../../install/libpayload
CFLAGS := -fno-builtin -Wall -Werror -Os \
	-include $(LIBPAYLOAD_DIR)/include/kconfig.h \
	-include $(LIBPAYLOAD_DIR)/include/compiler.h \
	-I $(LIBPAYLOAD_DIR)/include \
	-I $(LIBPAYLOAD_DIR)/include/mock \
	-ffunction-sections \
	-fdata-sections -g3
LDFLAGS := -Wl,--gc-sections
TARGET := hello
OBJS := $(TARGET).o
OBJS-mock := $(TARGET)_mocks.o
LIBPAYLOAD-local := libpayload.a
mocks := console_write

all: $(TARGET).elf

$(TARGET).elf: $(OBJS) $(OBJS-mock) $(LIBPAYLOAD-local)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBPAYLOAD-local) \
		-Wl,--exclude-libs,ALL -lc $(OBJS-mock)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.S.o: %.S
	$(AS) --32 -o $@ $<

# Copy libpayload and weaken all mocked symbols
$(LIBPAYLOAD-local): $(LIBPAYLOAD_DIR)/lib/libpayload.a
	$(OBJCOPY) $(foreach mock,$(mocks),--weaken-symbol=$(mock)) $< $@

clean:
	rm -f $(TARGET).elf *.o $(LIBPAYLOAD-local)

distclean: clean
