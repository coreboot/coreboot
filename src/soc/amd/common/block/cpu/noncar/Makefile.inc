ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_NONCAR),y)

bootblock-y += pre_c.S
bootblock-y += write_resume_eip.c
romstage-y += memmap.c
ramstage-y += cpu.c
ramstage-y += memmap.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_NONCAR
