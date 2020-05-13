## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

romstage-y += hsio.c

ramstage-y += ramstage.c
ramstage-y += hsio.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c

bootblock-y += bmcinfo.c
postcar-y += bmcinfo.c
romstage-y += bmcinfo.c
ramstage-y += bmcinfo.c
smm-y += bmcinfo.c

CPPFLAGS_common += -Isrc/mainboard/$(MAINBOARDDIR)/
