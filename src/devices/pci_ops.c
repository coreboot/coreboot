#include <console/console.h>
#include <arch/pciconf.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

static struct bus *get_pbus(device_t dev)
{
	struct bus *pbus = dev->bus;
	while(pbus && pbus->dev && !ops_pci_bus(pbus)) {
		pbus = pbus->dev->bus;
	}
	if (!pbus || !pbus->dev || !pbus->dev->ops || !pbus->dev->ops->ops_pci_bus) {
		printk_alert("%s Cannot find pci bus operations", dev_path(dev));
		die("");
		for(;;);
	}
	return pbus;
}

uint8_t pci_read_config8(device_t dev, unsigned where)
{
	struct bus *pbus = get_pbus(dev);
	return ops_pci_bus(pbus)->read8(pbus, dev->bus->secondary, dev->path.u.pci.devfn, where);
}

uint16_t pci_read_config16(device_t dev, unsigned where)
{
	struct bus *pbus = get_pbus(dev);
	return ops_pci_bus(pbus)->read16(pbus, dev->bus->secondary, dev->path.u.pci.devfn, where);
}

uint32_t pci_read_config32(device_t dev, unsigned where)
{
	struct bus *pbus = get_pbus(dev);
	return ops_pci_bus(pbus)->read32(pbus, dev->bus->secondary, dev->path.u.pci.devfn, where);
}

void pci_write_config8(device_t dev, unsigned where, uint8_t val)
{
	struct bus *pbus = get_pbus(dev);
	ops_pci_bus(pbus)->write8(pbus, dev->bus->secondary, dev->path.u.pci.devfn, where, val);
}

void pci_write_config16(device_t dev, unsigned where, uint16_t val)
{
	struct bus *pbus = get_pbus(dev);
	ops_pci_bus(pbus)->write16(pbus, dev->bus->secondary, dev->path.u.pci.devfn, where, val);
}

void pci_write_config32(device_t dev, unsigned where, uint32_t val)
{
	struct bus *pbus = get_pbus(dev);
	ops_pci_bus(pbus)->write32(pbus, dev->bus->secondary, dev->path.u.pci.devfn, where, val);
}
