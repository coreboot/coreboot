## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

romstage-y = gpio.c

ramstage-$(CONFIG_CARDBUS_PLUGIN_SUPPORT) += ti_pci7xx1.c
ramstage-y += cstates.c
ramstage-y += blc.c
