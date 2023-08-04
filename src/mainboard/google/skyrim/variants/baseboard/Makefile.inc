## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c

romstage-y += gpio.c

ramstage-y += gpio.c

verstage-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += gpio.c

smm-y += gpio.c
smm-y += smihandler.c
