## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
ramstage-y += ramstage.c
ramstage-y += hda_verb.c
ramstage-$(CONFIG_DRIVERS_OPTION_CFR_ENABLED) += cfr.c
SPD_SOURCES += isocon8gb
