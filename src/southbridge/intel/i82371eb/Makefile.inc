## SPDX-License-Identifier: GPL-2.0-or-later

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_I82371EB),y)

bootblock-y += bootblock.c

ramstage-y +=  i82371eb.c
ramstage-y +=  isa.c
ramstage-y +=  ide.c
ramstage-y +=  usb.c
ramstage-y +=  smbus.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi_tables.c

romstage-$(CONFIG_HAVE_ACPI_RESUME) += wakeup.c
romstage-y += early_pm.c
romstage-y += early_smbus.c

endif
