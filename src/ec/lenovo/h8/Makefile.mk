## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_EC_LENOVO_H8),y)

ramstage-y += sense.c
verstage-y += sense.c
romstage-y += sense.c
bootblock-y += sense.c
postcar-y += sense.c
smm-y += sense.c

ramstage-$(CONFIG_VBOOT) += vboot.c
verstage-$(CONFIG_VBOOT) += vboot.c
romstage-$(CONFIG_VBOOT) += vboot.c
bootblock-$(CONFIG_VBOOT) += vboot.c
postcar-$(CONFIG_VBOOT) += vboot.c

ifneq ($(filter y,$(CONFIG_H8_BEEP_ON_DEATH) $(CONFIG_H8_FLASH_LEDS_ON_DEATH)),)
romstage-y += panic.c
ramstage-y += panic.c
endif

ramstage-y += h8.c
ramstage-y += bluetooth.c
ramstage-y += wwan.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += ssdt.c

endif
