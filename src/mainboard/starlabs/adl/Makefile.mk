## SPDX-License-Identifier: GPL-2.0-only

subdirs-$(CONFIG_HAVE_SPD_IN_CBFS) += ./spd
subdirs-y += variants/$(VARIANT_DIR)

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-y += mainboard.c
