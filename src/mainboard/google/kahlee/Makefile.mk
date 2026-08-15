# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock/bootblock.c
bootblock-y += BiosCallOuts.c
bootblock-y += ec.c

romstage-y += BiosCallOuts.c
romstage-y += chromeos.c
romstage-y += OemCustomize.c

ramstage-y += BiosCallOuts.c
ramstage-y += chromeos.c
ramstage-y += ec.c
ramstage-y += OemCustomize.c

verstage-y += chromeos.c
verstage-y += ec.c

subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
