obj-y += boot.o
obj-y += coreboot_table.o
obj-$(CONFIG_MULTIBOOT) += multiboot.o
obj-y += tables.o
obj-$(CONFIG_GENERATE_PIRQ_TABLE) += pirq_routing.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += acpi.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += acpigen.o
obj-$(CONFIG_HAVE_ACPI_RESUME) += wakeup.S

