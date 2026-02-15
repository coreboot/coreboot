## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c

romstage-y += romstage.c

ramstage-y += devtree.c
ramstage-y += gpio.c
ramstage-y += hda_verb.c
ramstage-y += ramstage.c
