## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

ramstage-y += mainboard.c
ramstage-y += ramstage.c
ramstage-y += sku.c

romstage-y += romstage.c

bootblock-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-$(CONFIG_CHROMEOS) += chromeos.c
verstage-$(CONFIG_CHROMEOS) += chromeos.c

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_HDA_VERB) += hda_verb.c

bootblock-$(CONFIG_EC_GOOGLE_WILCO) += ec.c
ramstage-$(CONFIG_EC_GOOGLE_WILCO) += ec.c
romstage-$(CONFIG_EC_GOOGLE_WILCO) += ec.c
verstage-$(CONFIG_EC_GOOGLE_WILCO) += ec.c

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
