/*
 * (C) 2004 Linux Networx
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/ioapic.h>
#include "esb6300.h"

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
	clear_ioapic(IO_APIC_ADDR + 0x10000);
}

static void pic_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	/* Report the pic1 mbar resource */
	res = new_resource(dev, 0x44);
	res->base  = IO_APIC_ADDR + 0x10000;
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
	.device = PCI_DEVICE_ID_INTEL_6300ESB_APIC1,
};

