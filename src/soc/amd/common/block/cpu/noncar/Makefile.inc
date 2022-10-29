## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_NONCAR),y)

bootblock-y += bootblock.c
bootblock-y += early_cache.c
bootblock-y += pre_c.S
bootblock-y += write_resume_eip.c
bootblock-$(CONFIG_TPM_MEASURED_BOOT) += bootblock_measure.c
romstage-y += memmap.c
ramstage-y += cpu.c
romstage-y += cpu.c
ramstage-y += memmap.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_NONCAR
