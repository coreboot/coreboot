
static void amd8111_enable_rom(void)
{
	unsigned char byte;
	device_t addr;

	/* Enable 4MB rom access at 0xFFC00000 - 0xFFFFFFFF */
	/* Locate the amd8111 */
	addr = pci_locate_device(PCI_ID(0x1022, 0x7468), 0);

	/* Set the 4MB enable bit bit */
	byte = pci_read_config8(addr, 0x43);
	byte |= 0x80;
	pci_write_config8(addr, 0x43, byte);
}
