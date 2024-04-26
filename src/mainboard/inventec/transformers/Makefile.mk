## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
romstage-y += romstage.c
romstage-y += util.c
romstage-$(CONFIG_IPMI_KCS_ROMSTAGE) += ipmi.c
ramstage-y += util.c
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include
