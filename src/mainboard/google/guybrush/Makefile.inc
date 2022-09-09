# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c
bootblock-$(CONFIG_CHROMEOS) += chromeos.c

romstage-y += port_descriptors.c
romstage-y += romstage.c

ramstage-y += mainboard.c
ramstage-y += ec.c
ramstage-y += port_descriptors.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c

all-y += spi_speeds.c

verstage-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += verstage.c
verstage-$(CONFIG_CHROMEOS) += chromeos.c

subdirs-y += variants/baseboard
subdirs-y += variants/$(VARIANT_DIR)

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

APCB_NAME=APCB_CZN_D4

LIB_SPD_DEPS = $(SPD_SOURCES)

APCB_SOURCES = $(obj)/$(APCB_NAME).gen
APCB_SOURCES_RECOVERY = $(obj)/$(APCB_NAME).gen

$(obj)/$(APCB_NAME).gen: $(SPD_SOURCES) \
			$(APCB_V3_EDIT_TOOL) \
			$(MAINBOARD_BLOBS_DIR)/$(APCB_NAME).bin
	$(APCB_V3_EDIT_TOOL) $(MAINBOARD_BLOBS_DIR)/$(APCB_NAME).bin \
			     $(obj)/$(APCB_NAME).gen  \
			     --spd_sources $(SPD_SOURCES)
