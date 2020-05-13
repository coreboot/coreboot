## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += dock.c

romstage-y += variants/$(VARIANT_DIR)/gpio.c

ramstage-y += dock.c
ramstage-y += cstates.c
ramstage-y += blc.c

ifeq ($(CONFIG_MAINBOARD_USE_LIBGFXINIT),y)
ramstage-$(CONFIG_BOARD_LENOVO_T400) += variants/t400/malibu-3/gma-mainboard.ads
ramstage-$(CONFIG_BOARD_LENOVO_R400) += variants/t400/malibu-3/gma-mainboard.ads
ramstage-$(CONFIG_BOARD_LENOVO_T500) += variants/t400/coronado-5/gma-mainboard.ads
ramstage-$(CONFIG_BOARD_LENOVO_W500) += variants/t400/coronado-5/gma-mainboard.ads
ramstage-$(CONFIG_BOARD_LENOVO_R500) += variants/r500/gma-mainboard.ads
endif
