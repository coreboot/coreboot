## SPDX-License-Identifier: GPL-2.0-only

romstage-y += gpio.c
bootblock-y += early_init.c
romstage-y += early_init.c

ramstage-y += cstates.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c
