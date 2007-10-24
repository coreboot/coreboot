/*
 * (C) 2004 Linux Networx
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "esb6300.h"

#define ALL		(0xff << 24)
#define NONE		(0)
#define DISABLED	(1 << 16)
#define ENABLED		(0 << 16)
#define TRIGGER_EDGE	(0 << 15)
#define TRIGGER_LEVEL	(1 << 15)
#define POLARITY_HIGH	(0 << 13)
#define POLARITY_LOW	(1 << 13)
#define PHYSICAL_DEST	(0 << 11)
#define LOGICAL_DEST	(1 << 11)
#define ExtINT		(7 << 8)
#define NMI		(4 << 8)
#define SMI		(2 << 8)
#define INT		(1 << 8)

static void setup_ioapic(device_t dev)
{
	int i;
	unsigned long value_low, value_high;
	unsigned long ioapic_base = 0xfec10000;
	volatile unsigned long *l;
	unsigned interrupts;

	l = (unsigned long *) ioapic_base;

	l[0] = 0x01;
	interrupts = (l[04] >> 16) & 0xff;
	for (i = 0; i < interrupts; i++) {
		l[0] = (i * 2) + 0x10;
		l[4] = DISABLED;
		value_low = l[4];
		l[0] = (i * 2) + 0x11;
		l[4] = NONE; /* Should this be an address? */
		value_high = l[4];
		if (value_low == 0xffffffff) {
			printk_warning("%s IO APIC not responding.\n", 
				dev_path(dev));
			return;
		}
	}
}

static void pic_init(struct device *dev)
{

	uint16_t word;

	/* Clear system errors */
	word = pci_read_config16(dev, 0x06);
	word |= 0xf900; /* Clear possible errors */
	pci_write_config16(dev, 0x06, word);

	/* enable interrupt lines */
	pci_write_config8(dev, 0x3c, 0xff);

	/* Setup the ioapic */
	setup_ioapic(dev);
}

static void pic_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	/* Report the pic1 mbar resource */
	res = new_resource(dev, 0x44);
	res->base  = 0xfec10000;
	res->size  = 256;
	res->limit = res->base + res->size -1;
	res->align = 8;
	res->gran  = 8;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | 
		IORESOURCE_STORED | IORESOURCE_ASSIGNED;
	dev->command |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
}

static struct pci_operations lops_pci = {
	/* Can we set the pci subsystem and device id? */
	.set_subsystem = 0,
};

static struct device_operations pci_ops  = {
	.read_resources   = pic_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = pic_init,
	.scan_bus         = 0,
	.enable           = esb6300_enable,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver pci_driver __pci_driver = {
	.ops    = &pci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_6300ESB_PIC1,
};

