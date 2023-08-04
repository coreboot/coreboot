## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c

ramstage-$(CONFIG_FW_CONFIG) += fw_config.c
ramstage-y += gpio.c

romstage-y += gpio.c

verstage-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += gpio.c

smm-y += smihandler.c
