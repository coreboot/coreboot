ifeq ($(CONFIG_SOC_INTEL_BROADWELL),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/intel/common

subdirs-y += pch

bootblock-y += bootblock/cpu.c
bootblock-y += bootblock/systemagent.c
bootblock-y += ../../../cpu/intel/car/bootblock.c
bootblock-y += ../../../cpu/intel/car/non-evict/cache_as_ram.S
bootblock-y += ../../../cpu/x86/early_reset.S

ramstage-y += acpi.c
ramstage-y += cpu.c
ramstage-y += cpu_info.c
smm-y += cpu_info.c
ramstage-y += finalize.c
ramstage-y += gma.c
ramstage-y += memmap.c
romstage-y += memmap.c
postcar-y += memmap.c
ramstage-y += minihd.c
ramstage-y += pei_data.c
romstage-y += pei_data.c
ramstage-y += ramstage.c
ramstage-$(CONFIG_HAVE_REFCODE_BLOB) += refcode.c
ramstage-y += smmrelocate.c
ramstage-y += systemagent.c
bootblock-y += tsc_freq.c
ramstage-y += tsc_freq.c
romstage-y += tsc_freq.c
smm-y      += tsc_freq.c
postcar-y  += tsc_freq.c
verstage-y += tsc_freq.c

postcar-y += ../../../cpu/intel/car/non-evict/exit_car.S

cpu_microcode_bins += 3rdparty/blobs/soc/intel/broadwell/microcode.bin

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
