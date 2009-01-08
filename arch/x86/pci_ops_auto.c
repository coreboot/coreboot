#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <types.h>
#include <io.h>


const struct pci_bus_operations pci_cf8_conf1 = {
	.read8  = pci_conf1_read_config8,
	.read16 = pci_conf1_read_config16,
	.read32 = pci_conf1_read_config32,
	.write8  = pci_conf1_write_config8,
	.write16 = pci_conf1_write_config16,
	.write32 = pci_conf1_write_config32,
	.find = pci_conf1_find_device,
};


/*
 * Before we decide to use direct hardware access mechanisms, we try to do some
 * trivial checks to ensure it at least _seems_ to be working -- we just test
 * whether bus 00 contains a host bridge (this is similar to checking
 * techniques used in XFree86, but ours should be more reliable since we
 * attempt to make use of direct access hints provided by the PCI BIOS).
 *
 * This should be close to trivial, but it isn't, because there are buggy
 * chipsets (yes, you guessed it, by Intel and Compaq) that have no class ID.
 */
static int pci_sanity_check(const struct pci_bus_operations *o)
{
	u16 class, vendor;
	unsigned bus;
	int devfn;
#define PCI_CLASS_BRIDGE_HOST		0x0600
#define PCI_CLASS_DISPLAY_VGA		0x0300
#define PCI_VENDOR_ID_COMPAQ		0x0e11
#define PCI_VENDOR_ID_INTEL		0x8086
#define PCI_VENDOR_ID_MOTOROLA		0x1057

	for (bus = 0, devfn = 0; devfn < 0x100; devfn++) {
	  class = o->read16(PCI_BDEVFN(bus, devfn), PCI_CLASS_DEVICE);
		vendor = o->read16(PCI_BDEVFN(bus, devfn), PCI_VENDOR_ID);
		if (((class == PCI_CLASS_BRIDGE_HOST) || (class == PCI_CLASS_DISPLAY_VGA)) ||
			((vendor == PCI_VENDOR_ID_INTEL) || (vendor == PCI_VENDOR_ID_COMPAQ) ||
				(vendor == PCI_VENDOR_ID_MOTOROLA))) { 
			return 1;
		}
	}
	printk(BIOS_ERR, "PCI: Sanity check failed\n");
	return 0;
}

void pci_check_pci_ops(const struct pci_bus_operations *ops)
{
	unsigned int tmp;

	/*
	 * Check if configuration cycles work.
	 */
	if (ops == &pci_cf8_conf1)
	{
		outb(0x01, 0xCFB);
		tmp = inl(0xCF8);
		outl(0x80000000, 0xCF8);
		if ((inl(0xCF8) == 0x80000000) && 
			pci_sanity_check(ops)) 
		{
			outl(tmp, 0xCF8);
			printk(BIOS_DEBUG, "PCI: Using configuration type 1\n");
			return;
		}
		outl(tmp, 0xCF8);
	}

	die("pci_check_pci_ops failed\n");
}
