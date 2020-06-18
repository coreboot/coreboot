# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_HAVE_ACPI_TABLES),y)

ramstage-y += acpi.c
ramstage-y += acpi_pm.c
ramstage-y += acpigen.c
ramstage-y += acpigen_dptf.c
ramstage-y += acpigen_dsm.c
ramstage-y += acpigen_ps2_keybd.c
ramstage-y += acpigen_usb.c
ramstage-y += device.c
ramstage-$(CONFIG_CHROMEOS) += chromeos-gnvs.c
ramstage-y += gnvs.c
ramstage-y += pld.c
ramstage-y += sata.c
ramstage-y += soundwire.c

postcar-y += acpi_pm.c

ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/acpi_tables.c),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/acpi_tables.c
endif
$(eval $(call asl_template,dsdt))

endif # CONFIG_HAVE_ACPI_TABLES
