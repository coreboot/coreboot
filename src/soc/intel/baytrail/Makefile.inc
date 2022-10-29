## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_BAYTRAIL),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/intel/common

all-y += tsc_freq.c

bootblock-y += ../../../cpu/intel/car/non-evict/cache_as_ram.S
bootblock-y += ../../../cpu/intel/car/bootblock.c
bootblock-y += ../../../cpu/x86/early_reset.S
bootblock-y += bootblock/bootblock.c

romstage-y += iosf.c
romstage-y += memmap.c
romstage-y += pmutil.c

postcar-y += iosf.c
postcar-y += memmap.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += dptf.c
ramstage-y += ehci.c
ramstage-y += emmc.c
ramstage-y += fadt.c
ramstage-y += gfx.c
ramstage-y += gpio.c
ramstage-y += hda.c
ramstage-y += iosf.c
ramstage-y += lpe.c
ramstage-y += lpss.c
ramstage-y += memmap.c
ramstage-y += northcluster.c
ramstage-y += pcie.c
ramstage-y += perf_power.c
ramstage-y += pmutil.c
ramstage-y += ramstage.c
ramstage-y += sata.c
ramstage-y += scc.c
ramstage-y += sd.c
ramstage-y += smm.c
ramstage-y += southcluster.c
ramstage-y += xhci.c
ramstage-$(CONFIG_ELOG) += elog.c
ramstage-$(CONFIG_VGA_ROM_RUN) += int15.c

ifeq ($(CONFIG_HAVE_REFCODE_BLOB),y)
ramstage-y += refcode.c
else
ramstage-y += modphy_table.c refcode_native.c
endif

smm-y += iosf.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += tsc_freq.c

# Remove as ramstage gets fleshed out
ramstage-y += placeholders.c

postcar-y += ../../../cpu/intel/car/non-evict/exit_car.S

cpu_microcode_bins += 3rdparty/blobs/soc/intel/baytrail/microcode.bin \
	3rdparty/intel-microcode/intel-ucode/06-37-09

CPPFLAGS_common += -Isrc/soc/intel/baytrail/include

ifeq ($(CONFIG_HAVE_MRC),y)

# Bay Trail MRC is an ELF file. Determine the entry address and first loadable
# section offset in the file. Subtract the offset from the entry address to
# determine the final location.
mrcelfoffset = $(shell $(READELF_x86_32) -S -W $(CONFIG_MRC_FILE) | sed -e 's/\[ /[0/' | awk '$$3 ~ /PROGBITS/ { print "0x"$$5; exit }' )
mrcelfentry = $(shell $(READELF_x86_32) -h -W $(CONFIG_MRC_FILE) | grep 'Entry point address' | awk '{print $$NF }')

# Add memory reference code blob.
cbfs-files-y += mrc.bin
mrc.bin-file := $(call strip_quotes,$(CONFIG_MRC_FILE))
mrc.bin-position := $(shell printf "0x%x" $$(( $(mrcelfentry) - $(mrcelfoffset) )) )
mrc.bin-type := mrc

endif
endif
