## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

ramstage-y += ramstage.c

romstage-y += romstage.c

bootblock-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-$(CONFIG_CHROMEOS) += chromeos.c
verstage-$(CONFIG_CHROMEOS) += chromeos.c

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_HDA_VERB) += hda_verb.c

bootblock-y += ec.c
ramstage-y += ec.c
romstage-y += ec.c
verstage-y += ec.c

subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

subdirs-y += spd
