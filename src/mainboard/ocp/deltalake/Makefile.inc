## SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c

romstage-y += romstage.c
romstage-$(CONFIG_IPMI_KCS_ROMSTAGE) += ipmi.c

ramstage-y += ramstage.c ipmi.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c
all-$(CONFIG_CONSOLE_OVERRIDE_LOGLEVEL) += loglevel_vpd.c
all-$(CONFIG_OVERRIDE_UART_FOR_CONSOLE) += uartio_vpd.c
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include
CPPFLAGS_common += -I$(CONFIG_FSP_HEADER_PATH)
