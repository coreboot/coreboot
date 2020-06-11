# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c

romstage-y += chromeos.c
romstage-y += ec.c
romstage-y += sku_id.c

ramstage-y += chromeos.c
ramstage-y += ec.c
ramstage-y += sku_id.c

ifeq ($(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),y)
verstage-y += verstage.c
else
verstage-y += chromeos.c
verstage-y += ec.c
endif

smm-y += smihandler.c

subdirs-y += variants/baseboard
subdirs-y += spd

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include/baseboard/acpi

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
