ifeq ($(CONFIG_NORTHBRIDGE_AMD_AMDFAM10),y)

ramstage-y += northbridge.c
ramstage-y += misc_control.c
romstage-y += amdfam10_util.c
ramstage-y += amdfam10_util.c

ramstage-y += ht_config.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c

ramstage-y += get_pci1234.c

# Enable this if you want to check the values of the PCI routing registers.
# Call show_all_routes() anywhere amdfam10.h is included.
#ramstage-y += util.c

endif
