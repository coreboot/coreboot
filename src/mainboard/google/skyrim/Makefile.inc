# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c

romstage-y += port_descriptors.c

ramstage-y += mainboard.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c

subdirs-y += variants/baseboard

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
