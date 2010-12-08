/*
 * (C) 2004 Linux Networx
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>

static void amd8131_bus_read_resources(device_t dev)
{
	return;
}

static void amd8131_bus_set_resources(device_t dev)
{
#if 0
	pci_bus_read_resources(dev);
#endif
	return;
}

static void amd8131_bus_enable_resources(device_t dev)
{
#if 0
	pci_dev_set_resources(dev);
#endif
	return;
}

static void amd8131_bus_init(device_t dev)
{
#if 0
	pcix_init(dev);
#endif
	return;
}

static unsigned int amd8131_scan_bus(device_t bus, unsigned int max)
{
#if 0
	max = pcix_scan_bridge(bus, max);
#endif
	return max;
}

static void amd8131_enable(device_t dev)
{
	uint32_t buses;
	uint16_t cr;

	/* Clear all status bits and turn off memory, I/O and master enables. */
	pci_write_config16(dev, PCI_COMMAND, 0x0000);
	pci_write_config16(dev, PCI_STATUS, 0xffff);

	/*
	 * Read the existing primary/secondary/subordinate bus
	 * number configuration.
	 */
	buses = pci_read_config32(dev, PCI_PRIMARY_BUS);

	/* Configure the bus numbers for this bridge: the configuration
	 * transactions will not be propagated by the bridge if it is not
	 * correctly configured.
	 */
	buses &= 0xff000000;
	buses |= (((unsigned int) (dev->bus->secondary) << 0) |
		((unsigned int) (dev->bus->secondary) << 8) |
		((unsigned int) (dev->bus->secondary) << 16));
	pci_write_config32(dev, PCI_PRIMARY_BUS, buses);
}

static struct device_operations pcix_ops  = {
        .read_resources   = amd8131_bus_read_resources,
        .set_resources    = amd8131_bus_set_resources,
	.enable_resources = amd8131_bus_enable_resources,
        .init             = amd8131_bus_init,
	.scan_bus         = 0,
	.enable           = amd8131_enable,
};

static const struct pci_driver pcix_driver __pci_driver = {
        .ops    = &pcix_ops,
        .vendor = PCI_VENDOR_ID_AMD,
        .device = 0x7450,
};


static void ioapic_enable(device_t dev)
{
	uint32_t value;
	value = pci_read_config32(dev, 0x44);
	if (dev->enabled) {
		value |= ((1 << 1) | (1 << 0));
	} else {
		value &= ~((1 << 1) | (1 << 0));
	}
	pci_write_config32(dev, 0x44, value);
}

static struct device_operations ioapic_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init     = 0,
	.scan_bus = 0,
	.enable   = ioapic_enable,
};

static const struct pci_driver ioapic_driver __pci_driver = {
	.ops    = &ioapic_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x7451,

};
