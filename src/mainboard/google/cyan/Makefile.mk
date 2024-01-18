## SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_ENABLE_BUILTIN_COM1) += com_init.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-y += spd/spd.c

ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-y += ec.c
ramstage-y += irqroute.c
ramstage-y += w25q64.c

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
