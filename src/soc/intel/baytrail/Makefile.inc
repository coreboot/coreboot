subdirs-y += microcode
subdirs-y += romstage
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/intel/microcode

ramstage-y += memmap.c
romstage-y += memmap.c
ramstage-y += tsc_freq.c
romstage-y += tsc_freq.c
ramstage-$(CONFIG_CACHE_MRC_SETTINGS) += nvm.c
ramstage-$(CONFIG_CACHE_MRC_SETTINGS) += mrc_cache.c
romstage-$(CONFIG_CACHE_MRC_SETTINGS) += mrc_cache.c
ramstage-y += spi.c
ramstage-y += chip.c
ramstage-y += iosf.c
romstage-y += iosf.c
ramstage-y += northcluster.c
ramstage-y += ramstage.c
ramstage-y += gpio.c

# Remove as ramstage gets fleshed out
ramstage-y += placeholders.c

INCLUDES += -Isrc/soc/intel/baytrail/

# Run an intermediate step when producing coreboot.rom
# that adds additional components to the final firmware
# image outside of CBFS
INTERMEDIATE:=baytrail_add_me

baytrail_add_me: $(obj)/coreboot.pre $(IFDTOOL)
	printf "    DD         Adding Intel Firmware Descriptor\n"
	dd if=3rdparty/mainboard/$(MAINBOARDDIR)/descriptor.bin \
		of=$(obj)/coreboot.pre conv=notrunc >/dev/null 2>&1
	printf "    IFDTOOL    me.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		-i ME:3rdparty/mainboard/$(MAINBOARDDIR)/me.bin \
		$(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
ifeq ($(CONFIG_LOCK_MANAGEMENT_ENGINE),y)
	printf "    IFDTOOL    Locking Management Engine\n"
	$(objutil)/ifdtool/ifdtool -l $(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
else
	printf "    IFDTOOL    Unlocking Management Engine\n"
	$(objutil)/ifdtool/ifdtool -u $(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
endif

# Add memory reference code blob.
cbfs-files-$(CONFIG_HAVE_MRC) += mrc.bin
mrc.bin-file := $(call strip_quotes,$(CONFIG_MRC_FILE))
mrc.bin-position := $(CONFIG_MRC_BIN_ADDRESS)
mrc.bin-type := 0xab
