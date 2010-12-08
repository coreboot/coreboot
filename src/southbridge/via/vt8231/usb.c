
static void usb_on(int enable)
{
	unsigned char regval;

	/* Base 8231 controller */
	device_t dev0 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	/* USB controller 1 */
	device_t dev2 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C586_2, 0);
	/* USB controller 2 */
	device_t dev3 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C586_2, dev2);

	/* enable USB1 */
	if(dev2) {
		if (enable) {
			pci_write_config8(dev2, 0x3c, 0x05);
			pci_write_config8(dev2, 0x04, 0x07);
		} else {
			pci_write_config8(dev2, 0x3c, 0x00);
			pci_write_config8(dev2, 0x04, 0x00);
		}
	}

	if(dev0) {
		regval = pci_read_config8(dev0, 0x50);
		if (enable)
			regval &= ~(0x10);
		else
			regval |= 0x10;
		pci_write_config8(dev0, 0x50, regval);
	}

	/* enable USB2 */
	if(dev3) {
		if (enable) {
			pci_write_config8(dev3, 0x3c, 0x05);
			pci_write_config8(dev3, 0x04, 0x07);
		} else {
			pci_write_config8(dev3, 0x3c, 0x00);
			pci_write_config8(dev3, 0x04, 0x00);
		}
	}

	if(dev0) {
		regval = pci_read_config8(dev0, 0x50);
		if (enable)
			regval &= ~(0x20);
		else
			regval |= 0x20;
		pci_write_config8(dev0, 0x50, regval);
	}
}
