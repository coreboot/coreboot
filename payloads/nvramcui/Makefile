unexport $(COREBOOT_EXPORTS)

LIBPAYLOAD_DIR=$(CURDIR)/libpayload
XCOMPILE=$(LIBPAYLOAD_DIR)/libpayload.xcompile
# build libpayload and put .config file in $(CURDIR) instead of ../libpayload
# to avoid pollute the libpayload source directory and possible conflicts
LPOPTS=obj="$(CURDIR)/build" DESTDIR="$(CURDIR)" DOTCONFIG="$(CURDIR)/.config"
CFLAGS += -Wall -Wvla -Werror -Os -ffreestanding -nostdinc -nostdlib

all: nvramcui.elf

$(LIBPAYLOAD_DIR):
	$(MAKE) -C ../libpayload $(LPOPTS) defconfig
	$(MAKE) -C ../libpayload $(LPOPTS)
	$(MAKE) -C ../libpayload $(LPOPTS) install

ifneq ($(strip $(wildcard libpayload)),)
include $(XCOMPILE)
LPGCC = CC="$(GCC_CC_x86_32)" "$(LIBPAYLOAD_DIR)/bin/lpgcc"
%.elf: %.c Makefile
	$(LPGCC) $(CFLAGS) -o $*.elf $*.c
else
# If libpayload is not found, first build libpayload,
# then do the make, this time it'll find libpayload
# and generate the nvramcui.elf target
%.elf: $(LIBPAYLOAD_DIR)
	$(MAKE) all
endif

clean:
	rm -rf build libpayload nvramcui.elf

distclean: clean
	rm -rf .config .config.old

.PHONY: all clean distclean
