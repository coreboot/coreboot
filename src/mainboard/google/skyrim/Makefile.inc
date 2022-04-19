# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c

romstage-y += port_descriptors.c

ramstage-y += mainboard.c
ramstage-y += ec.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c

verstage-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += verstage.c

subdirs-y += variants/baseboard
subdirs-y += variants/$(VARIANT_DIR)

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

ifneq ($(wildcard $(MAINBOARD_BLOBS_DIR)/APCB_SBR_D5.bin),)
$(info APCB sources present.)

LIB_SPD_DEPS = $(SPD_SOURCES)

APCB_SOURCES = $(obj)/APCB_SBR_D5.gen
APCB_SOURCES_RECOVERY = $(obj)/APCB_SBR_D5.gen

$(obj)/APCB_SBR_D5.gen: $(SPD_SOURCES) \
			$(APCB_V3_EDIT_TOOL) \
			$(MAINBOARD_BLOBS_DIR)/APCB_SBR_D5.bin
	$(APCB_V3_EDIT_TOOL) $(MAINBOARD_BLOBS_DIR)/APCB_SBR_D5.bin \
			     $(obj)/APCB_SBR_D5.gen  \
			     --spd_sources $(SPD_SOURCES) \
			     --mem_type 'lp5'
else
$(info APCB sources not found.  Skipping APCB.)
endif
