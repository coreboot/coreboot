# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c

romstage-y += cbmem.c

bootblock-y += media.c
romstage-y += media.c
ramstage-y += media.c

bootblock-y += uart.c
romstage-y += uart.c
ramstage-y += uart.c
ramstage-y += acpi.c
ramstage-$(CONFIG_ACPI_PPTT) += pptt.c

bootblock-y += bootblock_custom.S

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include
