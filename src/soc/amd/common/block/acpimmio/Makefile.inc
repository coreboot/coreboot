ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO),y)

bootblock-y += mmio_util.c
verstage-y += mmio_util.c
romstage-y += mmio_util.c
postcar-y += mmio_util.c
ramstage-y += mmio_util.c
smm-y += mmio_util.c

bootblock-y += biosram.c
verstage_x86-y += biosram.c
romstage-y += biosram.c
postcar-y += biosram.c
ramstage-y += biosram.c
smm-y += biosram.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO
