ramstage-y += model_206ax_init.c
subdirs-y += ../../x86/name
subdirs-y += ../smm/gen1

subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../microcode
subdirs-y += ../turbo

ramstage-y += acpi.c

ramstage-y += common.c
romstage-y += common.c
smm-y += common.c

smm-y += finalize.c

cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-2a-*)
cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-3a-*)

bootblock-y += bootblock.c
bootblock-y += ../car/non-evict/cache_as_ram.S
bootblock-y += ../../x86/early_reset.S
bootblock-y += ../car/bootblock.c

postcar-y += ../car/non-evict/exit_car.S

romstage-y += ../car/romstage.c
