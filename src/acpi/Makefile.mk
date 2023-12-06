# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_HAVE_ACPI_TABLES),y)

ramstage-y += acpi.c
ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32)$(CONFIG_ARCH_RAMSTAGE_X86_64),y)
ramstage-y += acpi_apic.c
ramstage-y += acpi_dmar.c
ramstage-y += acpi_hpet.c
ramstage-y += acpigen_pci_root_resource_producer.c
endif
ramstage-$(CONFIG_ACPI_PPTT) += acpi_pptt.c
ramstage-$(CONFIG_ACPI_IORT) += acpi_iort.c
ramstage-y += acpigen.c
ramstage-y += acpigen_dptf.c
ramstage-y += acpigen_dsm.c
ramstage-$(CONFIG_PCI) += acpigen_pci.c
ramstage-y += acpigen_ps2_keybd.c
ramstage-y += acpigen_usb.c
ramstage-y += device.c
ramstage-$(CONFIG_ACPI_SOC_NVS) += gnvs.c
ramstage-y += pld.c
ramstage-y += sata.c
ramstage-y += soundwire.c
ramstage-y += fadt_filler.c
ramstage-$(CONFIG_ACPI_COMMON_MADT_GICC_V3) += acpi_gic.c

all-y += acpi_pm.c
smm-y += acpi_pm.c

ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/acpi_tables.c),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/acpi_tables.c
endif
$(eval $(call asl_template,dsdt))

endif # CONFIG_HAVE_ACPI_TABLES
