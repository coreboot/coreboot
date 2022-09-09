# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c

romstage-y += port_descriptors.c

ramstage-y += mainboard.c
ramstage-y += ec.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c

verstage-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += verstage.c

all-y += spi_speeds.c

subdirs-y += variants/baseboard
subdirs-y += variants/$(VARIANT_DIR)

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

APCB_NAME=APCB_MDN_D5

ifneq ($(wildcard $(MAINBOARD_BLOBS_DIR)/$(APCB_NAME).bin),)
$(info APCB sources present.)

ifneq ($(wildcard $(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/memory/Makefile.inc),)

LIB_SPD_DEPS = $(SPD_SOURCES)

APCB_SOURCES = $(obj)/$(APCB_NAME).gen
APCB_SOURCES_RECOVERY = $(obj)/$(APCB_NAME).gen

$(obj)/$(APCB_NAME).gen: $(SPD_SOURCES) \
			$(APCB_V3_EDIT_TOOL) \
			$(MAINBOARD_BLOBS_DIR)/$(APCB_NAME).bin
	$(APCB_V3_EDIT_TOOL) $(MAINBOARD_BLOBS_DIR)/$(APCB_NAME).bin \
			     $(obj)/$(APCB_NAME).gen  \
			     --spd_sources $(SPD_SOURCES) \
			     --mem_type 'lp5'
else
$(info SPD sources not found.  Skipping APCB.)
endif
else
$(info APCB sources not found.  Skipping APCB.)
endif
