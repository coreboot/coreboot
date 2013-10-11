romstage-$(CONFIG_EARLY_CBMEM_INIT) += cbmem.c
romstage-$(CONFIG_HAVE_ACPI_RESUME) += cbmem.c

ramstage-y += boot.c
ramstage-$(CONFIG_MULTIBOOT) += multiboot.c
ramstage-y += gdt.c
ramstage-y += tables.c
ramstage-y += cbmem.c
ramstage-$(CONFIG_GENERATE_MP_TABLE) += mpspec.c
ramstage-$(CONFIG_GENERATE_PIRQ_TABLE) += pirq_routing.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_GENERATE_SMBIOS_TABLES) += smbios.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpigen.c
ramstage-$(CONFIG_HAVE_ACPI_RESUME) += wakeup.S
ramstage-$(CONFIG_RELOCATABLE_RAMSTAGE) += ramstage_module_header.c

$(obj)/arch/x86/boot/coreboot_table.ramstage.o : $(OPTION_TABLE_H)
$(obj)/arch/x86/boot/smbios.ramstage.o: $(obj)/build.h

