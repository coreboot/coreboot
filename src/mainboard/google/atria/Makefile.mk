# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-y += romstage.c

ramstage-y += ec.c
ramstage-y += mainboard.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c

subdirs-y += variants/baseboard
subdirs-y += variants/$(VARIANT_DIR)
subdirs-y += variants/$(VARIANT_DIR)/memory
LIB_SPD_DEPS = $(SPD_SOURCES)

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
