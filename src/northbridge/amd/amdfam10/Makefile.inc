ramstage-y += northbridge.c
ramstage-y += misc_control.c

ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += ssdt.asl
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += sspr1.asl
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += sspr2.asl
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += sspr3.asl
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += sspr4.asl
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += sspr5.asl

ramstage-y += get_pci1234.c

# Enable this if you want to check the values of the PCI routing registers.
# Call show_all_routes() anywhere amdfam10.h is included.
#ramstage-y += util.c
