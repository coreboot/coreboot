/*
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
 */


static void ck804_enable_rom(void)
{
	unsigned char byte;
	device_t addr;

	/* Enable 4MB rom access at 0xFFC00000 - 0xFFFFFFFF */
	/* Locate the ck804 LPC */
	addr = PCI_DEV(0, (CK804_DEVN_BASE+1), 0);

	/* Set the 4MB enable bit bit */
	byte = pci_read_config8(addr, 0x88);
	byte |= 0x80;
	pci_write_config8(addr, 0x88, byte);
}
