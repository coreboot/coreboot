ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO),y)

all-y += mmio_util.c
smm-y += mmio_util.c

all-y += biosram.c
smm-y += biosram.c

bootblock-y += print_reset_status.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO
