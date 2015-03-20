
ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32),y)

romstage-y += cbmem.c
romstage-y += boot.c

endif # CONFIG_ARCH_ROMSTAGE_X86_32

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)

ramstage-y += boot.c
ramstage-y += gdt.c
ramstage-y += tables.c
ramstage-y += cbmem.c
ramstage-$(CONFIG_GENERATE_MP_TABLE) += mpspec.c
ramstage-$(CONFIG_GENERATE_PIRQ_TABLE) += pirq_routing.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_GENERATE_SMBIOS_TABLES) += smbios.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpigen.c
ramstage-$(CONFIG_HAVE_ACPI_RESUME) += wakeup.S

endif # CONFIG_ARCH_RAMSTAGE_X86_32
