ifeq ($(CONFIG_SOC_INTEL_BAYTRAIL),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo

ramstage-y += memmap.c
romstage-y += memmap.c
ramstage-y += tsc_freq.c
romstage-y += tsc_freq.c
smm-y += tsc_freq.c
ramstage-y += spi.c
smm-y += spi.c
ramstage-y += chip.c
ramstage-y += gfx.c
ramstage-y += iosf.c
romstage-y += iosf.c
smm-y += iosf.c
ramstage-y += northcluster.c
ramstage-y += ramstage.c
ramstage-y += gpio.c
romstage-y += reset.c
ramstage-y += reset.c
ramstage-y += cpu.c
ramstage-y += pmutil.c
smm-y += pmutil.c
smm-y += smihandler.c
ramstage-y += smm.c
ramstage-y += ehci.c
ramstage-y += xhci.c
ramstage-y += southcluster.c
ramstage-$(CONFIG_HAVE_REFCODE_BLOB) += refcode.c
ramstage-y += sata.c
ramstage-y += acpi.c
ramstage-y += lpe.c
ramstage-y += scc.c
ramstage-y += emmc.c
ramstage-y += lpss.c
ramstage-y += pcie.c
ramstage-y += sd.c
ramstage-y += dptf.c
ramstage-y += perf_power.c
ramstage-y += stage_cache.c
romstage-y += stage_cache.c
ramstage-$(CONFIG_ELOG) += elog.c
ramstage-y += hda.c

# Remove as ramstage gets fleshed out
ramstage-y += placeholders.c

cpu_microcode_bins += 3rdparty/blobs/soc/intel/baytrail/microcode.bin

CPPFLAGS_common += -Isrc/soc/intel/baytrail/include

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
