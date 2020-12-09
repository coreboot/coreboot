ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_NONCAR),y)

bootblock-y += pre_c.S
bootblock-y += write_resume_eip.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_NONCAR
