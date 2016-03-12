ifeq ($(CONFIG_DRIVER_XPOWERS_AXP209),y)

bootblock-$(CONFIG_DRIVER_XPOWERS_AXP209_BOOTBLOCK) += axp209.c
romstage-y += axp209.c
ramstage-y += axp209.c

endif
