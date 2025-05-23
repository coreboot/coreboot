## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c
verstage-$(CONFIG_BOARD_INTEL_PTLRVP) += intel.c

romstage-y += romstage.c
romstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-$(CONFIG_BOARD_INTEL_PTLRVP) += intel.c

ramstage-y += mainboard.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-y += ec.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_HDA_VERB) += variants/$(VARIANT_DIR)/hda_verb.c
ramstage-$(CONFIG_BOARD_INTEL_PTLRVP) += intel.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c

BASEBOARD_DIR:=$(call strip_quotes,$(CONFIG_BASEBOARD_DIR))

subdirs-y += variants/baseboard/$(BASEBOARD_DIR)
subdirs-y += variants/$(VARIANT_DIR)
subdirs-y += variants/$(VARIANT_DIR)/memory
subdirs-$(CONFIG_HAVE_SPD_IN_CBFS) += spd

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/$(BASEBOARD_DIR)/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
