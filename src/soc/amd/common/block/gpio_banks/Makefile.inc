ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_BANKED_GPIOS),y)

bootblock-y += gpio.c
verstage-y += gpio.c
romstage-y += gpio.c
postcar-y += gpio.c
ramstage-y += gpio.c
smm-y += gpio.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_BANKED_GPIOS
