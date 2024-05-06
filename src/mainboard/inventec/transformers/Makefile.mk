## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
romstage-y += romstage.c
romstage-$(CONFIG_IPMI_KCS_ROMSTAGE) += ipmi.c
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include
