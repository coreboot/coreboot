## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += variants/$(VARIANT_DIR)

bootblock-y += bootblock.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-y += devtree.c
ramstage-y += mainboard.c
