# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c

romstage-y += port_descriptors.c

ramstage-y += mainboard.c
ramstage-y += ec.c
ramstage-y += port_descriptors.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c

verstage-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += verstage.c

all-y += spi_speeds.c

subdirs-y += variants/baseboard
subdirs-y += variants/$(VARIANT_DIR)

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

APCB_NAME=data.apcb
APCB_PATH=$(src)/mainboard/$(MAINBOARDDIR)

ifneq ($(wildcard $(APCB_PATH)/$(APCB_NAME)),)
$(info APCB sources present.)

ifneq ($(wildcard $(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/memory/Makefile.mk),)

LIB_SPD_DEPS = $(SPD_SOURCES)

APCB_SOURCES = $(obj)/$(APCB_NAME).gen

$(obj)/$(APCB_NAME).gen: $(SPD_SOURCES) \
			$(APCB_V3_EDIT_TOOL) \
			$(APCB_PATH)/$(APCB_NAME)
	$(APCB_V3_EDIT_TOOL) $(APCB_PATH)/$(APCB_NAME) \
			     $(obj)/$(APCB_NAME).gen  \
			     --spd_sources $(SPD_SOURCES) \
			     --mem_type 'lp5'
else
$(info SPD sources not found.  Skipping APCB.)
show_notices:: die_no_apcb
endif
else
$(info APCB sources not found.  Skipping APCB.)
show_notices:: die_no_apcb
endif
