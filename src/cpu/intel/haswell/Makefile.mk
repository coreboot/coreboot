## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += ../car/non-evict/cache_as_ram.S
bootblock-y += ../car/bootblock.c
bootblock-y += ../../x86/early_reset.S

romstage-y += romstage.c
romstage-y += ../car/romstage.c

postcar-y += ../car/non-evict/exit_car.S

ramstage-y += acpi.c
ramstage-y += haswell_init.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smmrelocate.c

smm-y += finalize.c

subdirs-y += ../microcode
subdirs-y += ../turbo

ifeq ($(CONFIG_INTEL_LYNXPOINT_LP),y)
# Haswell ULT/ULX
cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-45-*)
ifeq ($(CONFIG_SOC_INTEL_BROADWELL),y)
# Broadwell ULT/ULX
cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-3d-*)
endif
else
# Haswell Trad
cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-3c-*)
# Crystal Well (Trad)
cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-46-*)
# Broadwell Trad
cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-47-*)
endif
