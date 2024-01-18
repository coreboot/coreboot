## SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_ENABLE_BUILTIN_COM1) += com_init.c

ramstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-$(CONFIG_CHROMEOS) += chromeos.c

ramstage-y += ec.c
ramstage-y += gpio.c
ramstage-y += irqroute.c
ramstage-y += ramstage.c
ramstage-y += w25q64.c
