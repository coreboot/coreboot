# SPDX-License-Identifier: GPL-2.0-or-later

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/prodrive/hermes/variants/$(VARIANT_DIR)/include

bootblock-y += bootblock.c
romstage-y += memory.c
romstage-y += eeprom.c

ramstage-y += ramstage.c
ramstage-y += mainboard.c
ramstage-y += eeprom.c

ramstage-$(CONFIG_AZALIA_PLUGIN_SUPPORT) += variants/baseboard/hda_verb.c
ramstage-$(CONFIG_AZALIA_PLUGIN_SUPPORT) += variants/r04/hda_verb.c
