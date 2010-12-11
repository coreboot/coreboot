#include <stddef.h>
#include <console/console.h>
#include <arch/io.h>
#include <arch/pciconf.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

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
	uint16_t class, vendor;
	unsigned bus;
	int devfn;
	struct bus pbus; /* Dummy device */
#define PCI_CLASS_BRIDGE_HOST		0x0600
#define PCI_CLASS_DISPLAY_VGA		0x0300
#define PCI_VENDOR_ID_COMPAQ		0x0e11
#define PCI_VENDOR_ID_INTEL		0x8086
#define PCI_VENDOR_ID_MOTOROLA		0x1057

	for (bus = 0, devfn = 0; devfn < 0x100; devfn++) {
		class = o->read16(&pbus, bus, devfn, PCI_CLASS_DEVICE);
		vendor = o->read16(&pbus, bus, devfn, PCI_VENDOR_ID);
		if (((class == PCI_CLASS_BRIDGE_HOST) || (class == PCI_CLASS_DISPLAY_VGA)) ||
			((vendor == PCI_VENDOR_ID_INTEL) || (vendor == PCI_VENDOR_ID_COMPAQ) ||
				(vendor == PCI_VENDOR_ID_MOTOROLA))) {
			return 1;
		}
	}
	printk(BIOS_ERR, "PCI: Sanity check failed\n");
	return 0;
}

struct pci_bus_operations *pci_bus_fallback_ops = NULL;

static const struct pci_bus_operations *pci_check_direct(void)
{
	unsigned int tmp;

	/*
	 * Check if configuration type 1 works.
	 */
	{
		outb(0x01, 0xCFB);
		tmp = inl(0xCF8);
		outl(0x80000000, 0xCF8);
		if ((inl(0xCF8) == 0x80000000) &&
			pci_sanity_check(&pci_cf8_conf1))
		{
			outl(tmp, 0xCF8);
			printk(BIOS_DEBUG, "PCI: Using configuration type 1\n");
			return &pci_cf8_conf1;
		}
		outl(tmp, 0xCF8);
	}

	/*
	 * Check if configuration type 2 works.
	 */
	{
		outb(0x00, 0xCFB);
		outb(0x00, 0xCF8);
		outb(0x00, 0xCFA);
		if ((inb(0xCF8) == 0x00 && inb(0xCFA) == 0x00) &&
			pci_sanity_check(&pci_cf8_conf2))
		{
			printk(BIOS_DEBUG, "PCI: Using configuration type 2\n");
			return &pci_cf8_conf2;
		}
	}

	die("pci_check_direct failed\n");
	return NULL;
}

const struct pci_bus_operations *pci_remember_direct(void)
{
	if (!pci_bus_fallback_ops)
		pci_bus_fallback_ops = (struct pci_bus_operations *)pci_check_direct();
	return pci_bus_fallback_ops;
}

/** Set the method to be used for PCI, type I or type II
 */
void pci_set_method(device_t dev)
{
	printk(BIOS_INFO, "Finding PCI configuration type.\n");
	dev->ops->ops_pci_bus = pci_remember_direct();
	post_code(0x5f);
}
