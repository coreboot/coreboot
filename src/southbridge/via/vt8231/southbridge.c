#include <pci.h>
#include <pc80/keyboard.h>
#include <printk.h>
#include <pci_ids.h>

void keyboard_on()
{
	volatile unsigned char regval;
	struct pci_dev *dev;

	printk_debug("keyboard_on\n");
	regval = 0xcf;

	dev = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	if (dev) {
		pci_write_config_byte(dev, 0x51, regval);
	}

	/* disable USB1 */
	dev = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C586_2, 0);
	if (dev) {
		pci_write_config_byte(dev, 0x3c, 0x00);
	}
	if (dev) {
		pci_write_config_byte(dev, 0x04, 0x00);
	}

	dev = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	if (dev) {
		pci_read_config_byte(dev, 0x50, &regval);
		regval |= 0x10;
		pci_write_config_byte(dev, 0x50, regval);
	}

	/* disable USB2 */
	dev = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C586_2, \
								dev);
	if (dev) {
		pci_write_config_byte(dev, 0x3c, 0x00);
	}
	if (dev) {
		pci_write_config_byte(dev, 0x04, 0x00);
	}

	dev = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	if (dev) {
		pci_read_config_byte(dev, 0x50, &regval);
		regval |= 0x20;
		pci_write_config_byte(dev, 0x50, regval);
	}

	pc_keyboard_init();

}

void nvram_on()
{
	/* the VIA 8231 South has a very different nvram setup than the piix4e ... */
	/* turn on ProMedia nvram. */
	/* TO DO: use the PciWriteByte function here. */

	struct pci_dev *dev;

	dev = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	if (dev) {
		pci_write_config_byte(dev, 0x41, 0xc0);
	}
}

void southbridge_fixup()
{
	unsigned char enables;
	struct pci_dev  *dev;
	struct pci_dev  *dev0;
	struct pci_dev dev_cpy;
	unsigned int devfn;

	// enable the internal I/O decode
	dev0 = 0;
	dev = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	if (dev) {
		dev0 = dev;
		memcpy(&dev_cpy, dev, sizeof(dev_cpy));
		pci_read_config_byte(dev, 0x6c, &enables);
		enables |= 0x80;
		pci_write_config_byte(dev, 0x6c, enables);
	} else {
		printk_debug("IDE pci_find_device function 0 failed\n");
	}

#ifndef DISABLE_SOUTHBRIDGE_COM_PORTS
	// enable com1 and com2. 
	enables = 0x80 | 0x1 | 0x8 ;
	if (dev) {
		pci_write_config_byte(dev, 0x6e, enables);
	}
	// note: this is also a redo of some port of assembly, but we
	// want everything up. 
	// set com1 to 115 kbaud
	// not clear how to do this yet. 
	// forget it; done in assembly. 
#endif

	// enable IDE, since Linux won't do it.
	// First do some more things to devfn (17,0)
	// note: this should already be cleared, according to the book. 
	if (dev) {
		pci_read_config_byte(dev, 0x50, &enables);
		printk_debug("IDE enable in reg. 50 is 0x%x\n", enables);
		enables &= ~8; // need manifest constant here!
		printk_debug("set IDE reg. 50 to 0x%x\n", enables);
		pci_write_config_byte(dev, 0x50, enables);
	}

	// set default interrupt values (IDE)
	if (dev) {
		pci_read_config_byte(dev, 0x4c, &enables);
		printk_debug("IRQs in reg. 4c are 0x%x\n", enables & 0xf);
		// clear out whatever was there. 
		enables &= ~0xf;
		enables |= 4;
		printk_debug("setting reg. 4c to 0x%x\n", enables);
		pci_write_config_byte(dev, 0x4c, enables);
	}

	// set up the serial port interrupts. 
	// com2 to 3, com1 to 4
	if (dev) {
		pci_write_config_byte(dev, 0x46, 0x04);
		pci_write_config_byte(dev, 0x47, 0x03);
	}

	dev = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C586_1, 0);
	if (!dev) {
		if (dev0) {
			dev = &dev_cpy;
			dev_cpy.devfn |= 1;
		}
	}
	if (!dev) {
		printk_debug("IDE pci_find_device function 1 failed\n");
	} else {
		pci_read_config_byte(dev, 0x40, &enables);
		printk_debug("enables in reg 0x40 0x%x\n", enables);
		enables |= 3;
		pci_write_config_byte(dev, 0x40, enables);

		pci_read_config_byte(dev, 0x40, &enables);
		printk_debug("enables in reg 0x40 read back as 0x%x\n", \
								enables);
	}

	// address decoding. 
	// we want "flexible", i.e. 1f0-1f7 etc. or native PCI
	if (dev) {
		pci_read_config_byte(dev, 0x9, &enables);
		printk_debug("enables in reg 0x9 0x%x\n", enables);
		// by the book, set the low-order nibble to 0xa. 
		enables &= ~0xf;
		// cf/cg silicon needs an 'f' here. 
		enables |= 0xf;
		pci_write_config_byte(dev, 0x9, enables);
		pci_read_config_byte(dev, 0x9, &enables);
		printk_debug("enables in reg 0x9 read back as 0x%x\n", enables);

		// standard bios sets master bit. 
		pci_read_config_byte(dev, 0x4, &enables);
		printk_debug("command in reg 0x4 0x%x\n", enables);
		enables |= 5;
		pci_write_config_byte(dev, 0x4, enables);
		pci_read_config_byte(dev, 0x4, &enables);
		printk_debug("command in reg 0x4 reads back as 0x%x\n", \
								enables);
		// oh well, the PCI BARs don't work right. 
		// This chip will not work unless IDE runs at standard legacy
		// values. 

		pci_write_config_dword(dev, 0x10, 0x1f1);
		pci_write_config_dword(dev, 0x14, 0x3f5);
		pci_write_config_dword(dev, 0x18, 0x171);
		pci_write_config_dword(dev, 0x1c, 0x375);
		pci_write_config_dword(dev, 0x20, 0xcc0);
	}
}
