## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c
romstage-y += memory.c
romstage-y += gpio.c
ramstage-y += gpio.c
romstage-$(CONFIG_FW_CONFIG) += variant.c
ramstage-$(CONFIG_FW_CONFIG) += variant.c
ramstage-y += ramstage.c
smm-y += smihandler.c
