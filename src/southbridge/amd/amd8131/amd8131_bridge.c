/*
 * (C) 2003 Linux Networx
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

static void pcix_init(device_t dev)
{
	uint16_t word;
	uint8_t byte;

	/* Enable memory write and invalidate ??? */
	byte = pci_read_config8(dev, 0x04);
        byte |= 0x10;
        pci_write_config8(dev, 0x04, byte);
 	
	/* Set drive strength */
	word = pci_read_config16(dev, 0xe0);
        word = 0x0404;
        pci_write_config16(dev, 0xe0, word);
	word = pci_read_config16(dev, 0xe4);
        word = 0x0404;
        pci_write_config16(dev, 0xe4, word);
	
	/* Set impedance */
	word = pci_read_config16(dev, 0xe8);
        word = 0x0404;
        pci_write_config16(dev, 0xe8, word);
	
	return;
}

static struct device_operations pcix_ops  = {
        .read_resources = pci_bus_read_resources,
        .set_resources = pci_dev_set_resources,
        .init = pcix_init,
        .scan_bus = pci_scan_bridge,
};

static struct pci_driver pcix_driver __pci_driver = {
        .ops    = &pcix_ops,
        .vendor = PCI_VENDOR_ID_AMD,
        .device = 0x7450,
};


static void ioapic_enable(device_t dev)
{
	uint32_t value;
	value = pci_read_config32(dev, 0x44);
	if (dev->enable) {
		value |= ((1 << 1) | (1 << 0));
	} else {
		value &= ~((1 << 1) | (1 << 0));
	}
	pci_write_config32(dev, 0x44, value);

//BY LYH
        value = pci_read_config32(dev, 0x4);
        value |= 6;
        pci_write_config32(dev, 0x4, value);
//BY LYH END
 

}

static struct device_operations ioapic_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources  = pci_dev_set_resources,
	.init     = 0,
	.scan_bus = 0,
	.enable   = ioapic_enable,
};

static struct pci_driver ioapic_driver __pci_driver = {
	.ops    = &ioapic_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x7451,
	
};
