## SPDX-License-Identifier: GPL-2.0-or-later

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-y += bootblock.c
bootblock-y += gpio_early.c

romstage-y += romstage.c

ramstage-y += fadt.c
ramstage-y += gpio.c
ramstage-y += hda_verb.c
ramstage-y += ramstage.c

# Embed the EC firmware into the SI_EC region, outside the BIOS region.
ifneq ($(CONFIG_FRAMEWORK_EC_FW_FILE),"")
$(call add_intermediate, add_ec_fw)
	$(CBFSTOOL) $< write -r SI_EC -f $(CONFIG_FRAMEWORK_EC_FW_FILE) --fill-upward
endif
