ramstage-y += model_2065x_init.c
subdirs-y += ../../x86/name
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../intel/turbo
subdirs-y += ../../intel/microcode
subdirs-y += ../smm/gen1

ramstage-y += acpi.c

smm-y += finalize.c

cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-25-*)

bootblock-y += ../car/non-evict/cache_as_ram.S
bootblock-y += ../car/bootblock.c
bootblock-y += ../../x86/early_reset.S

postcar-y += ../car/non-evict/exit_car.S

romstage-y += ../car/romstage.c
