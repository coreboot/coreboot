## SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_ENABLE_BUILTIN_COM1) += com_init.c

romstage-$(CONFIG_MAINBOARD_HAS_CHROMEOS) += chromeos.c

ramstage-$(CONFIG_MAINBOARD_HAS_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_MAINBOARD_HAS_CHROMEOS) += ec.c
ramstage-$(CONFIG_MAINBOARD_HAS_CHROMEOS) += gpio.c
ramstage-y += irqroute.c
ramstage-y += ramstage.c
ramstage-y += w25q64.c
