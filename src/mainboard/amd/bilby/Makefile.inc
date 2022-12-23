# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += early_gpio.c

ramstage-y += gpio.c
ramstage-y += port_descriptors.c

ifneq ($(CONFIG_BILBY_LPC),y)
ramstage-y += emmc_gpio.c
endif

ifeq ($(CONFIG_BOARD_AMD_BILBY),y)
APCB_SOURCES = $(MAINBOARD_BLOBS_DIR)/APCB_bilby.bin
endif
