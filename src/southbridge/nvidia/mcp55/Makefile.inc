driver-y += mcp55.o
driver-y += mcp55_aza.o
driver-y += mcp55_ht.o
driver-y += mcp55_ide.o
driver-y += mcp55_lpc.o
driver-y += mcp55_nic.o
driver-y += mcp55_pci.o
driver-y += mcp55_pcie.o
driver-y += mcp55_sata.o
driver-y += mcp55_smbus.o
driver-y += mcp55_usb2.o
driver-y += mcp55_usb.o

driver-$(CONFIG_GENERATE_ACPI_TABLES) += mcp55_fadt.o

obj-y += mcp55_reset.o
