ifeq ($(CONFIG_EC_LENOVO_H8),y)

ifneq ($(filter y,$(CONFIG_H8_BEEP_ON_DEATH) $(CONFIG_H8_FLASH_LEDS_ON_DEATH)),)
romstage-y += panic.c
ramstage-y += panic.c
endif

ramstage-y += h8.c
ramstage-y += bluetooth.c
ramstage-y += wwan.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += ssdt.c
smm-y += smm.c

endif
