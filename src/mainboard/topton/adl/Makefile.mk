/* SPDX-License-Identifier: GPL-2.0-or-later */

bootblock-y += bootblock.c

romstage-y += romstage_fsp_params.c

ramstage-y += mainboard.c
ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
