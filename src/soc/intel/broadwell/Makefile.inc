ifeq ($(CONFIG_SOC_INTEL_BROADWELL),y)

subdirs-y += pch

bootblock-y += bootblock.c

romstage-y += early_init.c
romstage-y += raminit.c
romstage-y += report_platform.c
romstage-y += romstage.c
romstage-$(CONFIG_HAVE_SPD_IN_CBFS) += spd.c

ramstage-y += acpi.c
ramstage-y += finalize.c
ramstage-y += gma.c
ramstage-y += memmap.c
romstage-y += memmap.c
postcar-y += memmap.c
ramstage-y += minihd.c
ramstage-y += northbridge.c
ramstage-y += pei_data.c
romstage-y += pei_data.c
ramstage-$(CONFIG_HAVE_REFCODE_BLOB) += refcode.c

CPPFLAGS_common += -Isrc/soc/intel/broadwell/include

# If an MRC file is an ELF file determine the entry address and first loadable
# section offset in the file. Subtract the offset from the entry address to
# determine the final location.
mrcelfoffset = $(shell $(READELF_x86_32) -S -W $(CONFIG_MRC_FILE) | sed -e 's/\[ /[0/' | awk '$$3 ~ /PROGBITS/ { print "0x"$$5; exit }' )
mrcelfentry = $(shell $(READELF_x86_32) -h -W $(CONFIG_MRC_FILE) | grep 'Entry point address' | awk '{print $$NF }')

# Add memory reference code blob.
cbfs-files-$(CONFIG_HAVE_MRC) += mrc.bin
mrc.bin-file := $(call strip_quotes,$(CONFIG_MRC_FILE))
mrc.bin-position := $(if $(findstring elf,$(CONFIG_MRC_FILE)),$(shell printf "0x%x" $$(( $(mrcelfentry) - $(mrcelfoffset) )) ),$(CONFIG_MRC_BIN_ADDRESS))
mrc.bin-type := mrc

endif
