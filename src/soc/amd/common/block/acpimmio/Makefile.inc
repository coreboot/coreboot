ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO),y)

all-y += mmio_util.c
smm-y += mmio_util.c

bootblock-y += biosram.c
verstage_x86-y += biosram.c
romstage-y += biosram.c
postcar-y += biosram.c
ramstage-y += biosram.c
smm-y += biosram.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO
