## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-$(CONFIG_CHROMEOS) += chromeos.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-y += memory.c

ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-y += mainboard.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_HDA_VERB) += variants/$(VARIANT_DIR)/hda_verb.c

subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include

subdirs-y += variants/$(VARIANT_DIR)
ifeq ($(CONFIG_BOARD_INTEL_COMETLAKE_RVPU)$(CONFIG_BOARD_INTEL_WHISKEYLAKE_RVP),y)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
endif
