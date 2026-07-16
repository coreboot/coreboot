## SPDX-License-Identifier: GPL-2.0-only

subdirs-$(CONFIG_HAVE_SPD_IN_CBFS) += ./spd
subdirs-y += variants/$(VARIANT_DIR)

bootblock-y += bootblock.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
