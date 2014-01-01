include ../../.config

ARCHDIR-$(CONFIG_ARCH_ARMV7)	:= armv7
ARCHDIR-$(CONFIG_ARCH_X86)	:= x86

# Only Intel chipsets supported, currently.
OBJ-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON)	+= uio_usbdebug_intel.o

PROGRAM		:= uio_usbdebug

CB_SRC		:= $(shell realpath ../../src)
CB_SOURCES	:= drivers/usb/pci_ehci.c drivers/usb/ehci_debug.c
CB_INCLUDES	:= drivers/usb/ehci.h drivers/usb/ehci_debug.h \
		   drivers/usb/usb_ch9.h
OBJECTS		:= uio_usbdebug.o \
		   console/printk.o \
		   device/device_util.o device/pci_device.o device/pci_ops.o \
		   lib/cbmem.o \
		   $(OBJ-y) \
		   $(patsubst %.c,%.o,$(CB_SOURCES))

CONFIG_H	:= ../../build/config.h

CFLAGS		+= -m32 -g \
		   -Wall -Wextra -Werror \
		   -Wno-unused-parameter -Wno-error=sign-compare
CPPFLAGS	+= -I../../src/include/ -I../../src/arch/$(ARCHDIR-y)/include/ \
		   -include$(CONFIG_H)

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS) $(OBJ-y)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS)

$(CB_SOURCES) $(CB_INCLUDES):
	@mkdir -p $(dir $@)
	@ln -sf $(CB_SRC)/$@ $@

$(OBJECTS): $(CONFIG_H) $(CB_INCLUDES)

clean:
	-@rm -rf $(CB_SOURCES) $(CB_INCLUDES) $(OBJECTS) $(PROGRAM)

.PHONY: all clean
