# SPDX-License-Identifier: GPL-2.0-only
# This file is part of the coreboot project.

ifeq ($(CONFIG_HAVE_ACPI_TABLES),y)

ramstage-y += acpi.c
ramstage-y += acpi_device.c
ramstage-y += acpi_pld.c
ramstage-y += acpigen.c
ramstage-y += acpigen_dsm.c
ramstage-y += acpigen_ps2_keybd.c

ramstage-y += sata.c

endif # CONFIG_GENERATE_ACPI_TABLES
