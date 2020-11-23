ramstage-y += haswell_init.c

romstage-y += romstage.c
romstage-y += ../car/romstage.c

ramstage-y += acpi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smmrelocate.c

smm-y += finalize.c

bootblock-y += ../car/non-evict/cache_as_ram.S
bootblock-y += ../car/bootblock.c
bootblock-y += ../../x86/early_reset.S
bootblock-y += bootblock.c

postcar-y += ../car/non-evict/exit_car.S

subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm
subdirs-y += ../microcode
subdirs-y += ../turbo

cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-3c-*)
cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-45-*)

ifeq ($(CONFIG_SOC_INTEL_BROADWELL),y)
cpu_microcode_bins += 3rdparty/blobs/soc/intel/broadwell/microcode.bin
endif
