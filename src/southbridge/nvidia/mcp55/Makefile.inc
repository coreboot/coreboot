driver-y += mcp55.c
driver-y += mcp55_azalia.c
driver-y += mcp55_ht.c
driver-y += mcp55_ide.c
driver-y += mcp55_lpc.c
driver-y += mcp55_nic.c
driver-y += mcp55_pci.c
driver-y += mcp55_pcie.c
driver-y += mcp55_sata.c
driver-y += mcp55_smbus.c
driver-y += mcp55_usb2.c
driver-y += mcp55_usb.c

driver-$(CONFIG_GENERATE_ACPI_TABLES) += mcp55_fadt.c

ramstage-y += mcp55_reset.c

chipset_bootblock_inc += $(src)/southbridge/nvidia/mcp55/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/nvidia/mcp55/romstrap.lds
