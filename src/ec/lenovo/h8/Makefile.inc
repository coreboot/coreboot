ifeq ($(CONFIG_EC_LENOVO_H8),y)

ifneq ($(filter y,$(CONFIG_H8_BEEP_ON_DEATH) $(CONFIG_H8_FLASH_LEDS_ON_DEATH)),)
romstage-y += panic.c
ramstage-y += panic.c
endif

ramstage-y += h8.c
smm-y += smm.c

endif
