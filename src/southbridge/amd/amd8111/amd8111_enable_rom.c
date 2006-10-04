
static void amd8111_enable_rom(void)
{
	unsigned char byte;
	device_t dev;

	/* Enable 5MB rom access at 0xFFB00000 - 0xFFFFFFFF */
	/* Locate the amd8111 */
	dev = pci_io_locate_device(PCI_ID(0x1022, 0x7468), 0);

	/* Set the 5MB enable bits */
	byte = pci_io_read_config8(dev, 0x43);
	byte |= 0xC0;
	pci_io_write_config8(dev, 0x43, byte);
}
