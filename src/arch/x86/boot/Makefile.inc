ramstage-y += boot.c
ramstage-y += coreboot_table.c
ramstage-$(CONFIG_MULTIBOOT) += multiboot.c
ramstage-y += gdt.c
ramstage-y += tables.c
ramstage-$(CONFIG_GENERATE_MP_TABLE) += mpspec.c
ramstage-$(CONFIG_GENERATE_PIRQ_TABLE) += pirq_routing.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpigen.c
ramstage-$(CONFIG_HAVE_ACPI_RESUME) += wakeup.S

$(obj)/arch/x86/boot/coreboot_table.ramstage.o : $(OPTION_TABLE_H)

