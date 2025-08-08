## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

romstage-y += romstage_fsp_params.c

ramstage-y += ramstage.c
ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
