## SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c
bootblock-$(CONFIG_CHROMEOS) += chromeos.c
bootblock-y += ec.c

romstage-y += romstage.c
romstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-y += ec.c

ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-y += ec.c
ramstage-y += mainboard.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_HDA_VERB) += hda_verb.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c
verstage-y += ec.c

subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include

VARIANT_DIR:=$(call strip_quotes,$(CONFIG_VARIANT_DIR))
subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

subdirs-y += spd
