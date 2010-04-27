#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "chip.h"

static void ide_init(struct device *dev)
{
	struct southbridge_via_vt8235_config *conf = dev->chip_info;
	unsigned char enables;

	printk(BIOS_INFO, "Enabling VIA IDE.\n");

	/*if (!conf->enable_native_ide) { */
		/*
		 * Run the IDE controller in 'compatiblity mode - i.e. don't
		 * use PCI interrupts.  Using PCI ints confuses linux for some
		 * reason.
		 */
		printk(BIOS_INFO, "%s: enabling compatibility IDE addresses\n",
				__func__);
		enables = pci_read_config8(dev, 0x42);
		printk(BIOS_DEBUG, "enables in reg 0x42 0x%x\n", enables);
		enables &= ~0xc0;		// compatability mode
		pci_write_config8(dev, 0x42, enables);
		enables = pci_read_config8(dev, 0x42);
		printk(BIOS_DEBUG, "enables in reg 0x42 read back as 0x%x\n",
				enables);
	/* } */

	enables = pci_read_config8(dev, 0x40);
	printk(BIOS_DEBUG, "enables in reg 0x40 0x%x\n", enables);
	enables |= 3;
	pci_write_config8(dev, 0x40, enables);
	enables = pci_read_config8(dev, 0x40);
	printk(BIOS_DEBUG, "enables in reg 0x40 read back as 0x%x\n", enables);

	// Enable prefetch buffers
	enables = pci_read_config8(dev, 0x41);
	enables |= 0xf0;
	pci_write_config8(dev, 0x41, enables);

	// Lower thresholds (cause award does it)
	enables = pci_read_config8(dev, 0x43);
	enables &= ~0x0f;
	enables |=  0x05;
	pci_write_config8(dev, 0x43, enables);

	// PIO read prefetch counter (cause award does it)
	pci_write_config8(dev, 0x44, 0x18);

	// Use memory read multiple
	pci_write_config8(dev, 0x45, 0x1c);

	// address decoding.
	// we want "flexible", i.e. 1f0-1f7 etc. or native PCI
	// kevinh@ispiri.com - the standard linux drivers seem ass slow when
	// used in native mode - I've changed back to classic
	enables = pci_read_config8(dev, 0x9);
	printk(BIOS_DEBUG, "enables in reg 0x9 0x%x\n", enables);
	// by the book, set the low-order nibble to 0xa.
	if (conf->enable_native_ide) {
		enables &= ~0xf;
		// cf/cg silicon needs an 'f' here.
		enables |= 0xf;
	} else {
		enables &= ~0x5;
	}

	pci_write_config8(dev, 0x9, enables);
	enables = pci_read_config8(dev, 0x9);
	printk(BIOS_DEBUG, "enables in reg 0x9 read back as 0x%x\n", enables);

	// standard bios sets master bit.
	enables = pci_read_config8(dev, 0x4);
	printk(BIOS_DEBUG, "command in reg 0x4 0x%x\n", enables);
	enables |= 7;

	// No need for stepping - kevinh@ispiri.com
	enables &= ~0x80;

	pci_write_config8(dev, 0x4, enables);
	enables = pci_read_config8(dev, 0x4);
	printk(BIOS_DEBUG, "command in reg 0x4 reads back as 0x%x\n", enables);

	if (!conf->enable_native_ide) {
		// Use compatability mode - per award bios
		pci_write_config32(dev, 0x10, 0x0);
		pci_write_config32(dev, 0x14, 0x0);
		pci_write_config32(dev, 0x18, 0x0);
		pci_write_config32(dev, 0x1c, 0x0);

		// Force interrupts to use compat mode - just like Award bios
		pci_write_config8(dev, 0x3d, 0x0);
		pci_write_config8(dev, 0x3c, 0xff);
	}
}

static struct device_operations ide_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ide_init,
	.enable           = 0,
	.ops_pci          = 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops    = &ide_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_82C586_1,
};
