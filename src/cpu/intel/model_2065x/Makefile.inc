ramstage-y += model_2065x_init.c
subdirs-y += ../../x86/name
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/tsc
subdirs-y += ../../intel/turbo
subdirs-y += ../../intel/microcode
subdirs-y += ../../x86/smm
subdirs-y += ../smm/gen1
subdirs-y += ../common

ramstage-y += tsc_freq.c
romstage-y += tsc_freq.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += tsc_freq.c

ramstage-y += acpi.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_2065x/microcode.bin

cpu_incs-y += $(src)/cpu/intel/car/non-evict/cache_as_ram.S
postcar-y += ../car/non-evict/exit_car.S

romstage-y += ../car/romstage.c
