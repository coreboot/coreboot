# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c

romstage-y += port_descriptors.c

ramstage-y += mainboard.c
ramstage-y += ec.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c

subdirs-y += variants/baseboard
subdirs-y += variants/$(VARIANT_DIR)

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

ifneq ($(wildcard $(MAINBOARD_BLOBS_DIR)/APCB_SBR_D5.bin),)
$(info APCB sources present.)
APCB_SOURCES = $(MAINBOARD_BLOBS_DIR)/APCB_SBR_D5.bin
APCB_SOURCES_RECOVERY = $(MAINBOARD_BLOBS_DIR)/APCB_SBR_D5.bin
else
$(info APCB sources not found.  Skipping APCB.)
endif
