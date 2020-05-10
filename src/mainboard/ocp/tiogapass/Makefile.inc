## SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c
ramstage-y += ramstage.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include
CPPFLAGS_common += -I$(CONFIG_FSP_HEADER_PATH)
