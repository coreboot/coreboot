void
final_superio_fixup()
{
        unsigned char enables;

	/* Base 8231 controller */
	struct pci_dev *dev0 = pci_find_device(PCI_VENDOR_ID_VIA, \
						0x3177, 0);

	/* enable com ports, since we're using this built-in superio */
	/* enable com1 and com2. */
	enables = pci_read_config_byte(dev, 0x6e, &enables);
	/*
	 * 0x80 is enable com port b, 0x1 is to make it com2, 0x8 is 
	 * enable com port a as com1
	 * kevinh/Ispiri - fixed, had 0x1 for com2
	 */
	enables = 0x80 | 0x10 | 0x8 ;
	pci_write_config_byte(dev, 0x6e, enables);
	/*
	 * note: this is also a redo of some port of assembly, but we want 
	 * everything up.
	 * set com1 to 115 kbaud
	 * not clear how to do this yet.
	 * forget it; done in assembly.
	 */
}

