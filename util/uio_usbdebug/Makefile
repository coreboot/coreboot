# SPDX-License-Identifier: GPL-2.0-only

include ../../.config

ARCHDIR-$(CONFIG_ARCH_ARMV7)	:= armv7
ARCHDIR-$(CONFIG_ARCH_X86)	:= x86

# Only Intel chipsets supported, currently.
OBJ-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON)	+= uio_usbdebug_intel.o

PROGRAM		:= uio_usbdebug

CB_SRC		:= $(shell realpath ../../src)
CB_SOURCES	:= drivers/usb/ehci_debug.c
CB_INCLUDES	:= \
		   drivers/usb/ehci.h \
		   drivers/usb/ehci_debug.h \
		   drivers/usb/usb_ch9.h
INCLUDES	:= \
		   include/device/device.h
OBJECTS		:= \
		   uio_usbdebug.o \
		   drivers/usb/pci_ehci.o \
		   console/printk.o \
		   lib/cbmem.o \
		   $(OBJ-y) \
		   $(patsubst %.c,%.o,$(CB_SOURCES))

KCONFIG_H	:= ../../src/include/kconfig.h

CFLAGS		+= \
		   -m32 -g \
		   -Wall -Wextra -Werror \
		   -Wno-unused-parameter -Wno-error=sign-compare
CPPFLAGS	+= \
		   -Iinclude/ \
		   -I../../src/include/ -I../../src/arch/$(ARCHDIR-y)/include/ \
		   -I../../build/ -include$(KCONFIG_H)

LIBS		:= -lpci -lz

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

$(CB_SOURCES) $(CB_INCLUDES):
	@mkdir -p $(dir $@)
	@ln -sf $(CB_SRC)/$@ $@

$(OBJECTS): $(CONFIG_H) $(CB_INCLUDES) $(INCLUDES)

clean:
	-@rm -rf $(CB_SOURCES) $(CB_INCLUDES) $(OBJECTS) $(PROGRAM)

.PHONY: all clean
