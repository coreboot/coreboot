LIBPAYLOAD_DIR=$(CURDIR)/libpayload
XCOMPILE=$(LIBPAYLOAD_DIR)/libpayload.xcompile
# build libpayload and put .config file in $(CURDIR) instead of ../libpayload
# to avoid pollute the libpayload source directory and possible conflicts
LPOPTS=obj="$(CURDIR)/build" DESTDIR="$(CURDIR)" DOTCONFIG="$(CURDIR)/.config"

all: nvramcui.elf

libpayload:
	$(MAKE) -C ../libpayload $(LPOPTS) defconfig
	$(MAKE) -C ../libpayload $(LPOPTS)
	$(MAKE) -C ../libpayload $(LPOPTS) install

ifneq ($(strip $(wildcard libpayload)),)
include $(XCOMPILE)
LPGCC = CC="$(GCC_CC_x86_32)" "$(LIBPAYLOAD_DIR)/bin/lpgcc"
%.elf: %.c
	$(LPGCC) -o $*.elf $*.c
else
# If libpayload is not found, first build libpayload,
# then do the make, this time it'll find libpayload
# and generate the nvramcui.elf target
%.elf: libpayload
	$(MAKE) all
endif

.PHONY:
clean:
	rm -f nvramcui.elf

distclean: clean
	rm -rf build libpayload .config
