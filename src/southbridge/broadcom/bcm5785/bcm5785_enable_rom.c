static void bcm5785_enable_rom(void)
{
	unsigned char byte;
	device_t addr;

	/* Enable 4MB rom access at 0xFFC00000 - 0xFFFFFFFF */
	/* Locate the BCM 5785 SB PCI Main */
	addr = pci_locate_device(PCI_ID(0x1166, 0x0205), 0); // 0x0201?

	/* Set the 4MB enable bit bit */
	byte = pci_read_config8(addr, 0x41);
	byte |= 0x0e;
	pci_write_config8(addr, 0x41, byte);
}
