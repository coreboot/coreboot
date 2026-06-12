## SPDX-License-Identifier: GPL-2.0-or-later

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-y += bootblock.c
bootblock-y += gpio_early.c

romstage-y += romstage.c

ramstage-y += ec.c
ramstage-y += fadt.c
ramstage-y += gpio.c
ramstage-y += hda_verb.c
ramstage-y += ramstage.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c
