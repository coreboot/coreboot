ramstage-y += boot.c
#ramstage-$(CONFIG_MULTIBOOT) += multiboot.c
ramstage-y += tables.c
#ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpi.c
#ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpigen.c
#ramstage-$(CONFIG_HAVE_ACPI_RESUME) += wakeup.S

#FIXME(dhendrix): is there anything preventing multiboot from
#working on ARM?

$(obj)/arch/armv7/boot/coreboot_table.ramstage.o : $(OPTION_TABLE_H)
#$(obj)/arch/x86/boot/smbios.ramstage.o: $(obj)/build.h
