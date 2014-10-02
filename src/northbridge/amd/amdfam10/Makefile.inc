ramstage-y += northbridge.c
ramstage-y += misc_control.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c

ramstage-y += get_pci1234.c

# Enable this if you want to check the values of the PCI routing registers.
# Call show_all_routes() anywhere amdfam10.h is included.
#ramstage-y += util.c
