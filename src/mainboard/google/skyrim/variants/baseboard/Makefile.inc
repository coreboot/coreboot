## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c

romstage-y += gpio.c
romstage-y += port_descriptors.c

ramstage-y += gpio.c
ramstage-y += port_descriptors.c

verstage-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += gpio.c

smm-y += gpio.c
smm-y += smihandler.c
