#include <console/console.h>
#include <arch/io.h>
#include <arch/pciconf.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

const struct pci_ops *conf = 0;

/*
 * Direct access to PCI hardware...
 */

uint8_t pci_read_config8(device_t dev, unsigned where)
{
	uint8_t value;
	value = conf->read8(dev->bus->secondary, dev->path.u.pci.devfn, where);
	printk_spew("Read config 8 bus %d,devfn 0x%x,reg 0x%x,val 0x%x\n",
		    dev->bus->secondary, dev->path.u.pci.devfn, where, value);
	return value;
}

uint16_t pci_read_config16(device_t dev, unsigned where)
{
	uint16_t value;
	value = conf->read16(dev->bus->secondary, dev->path.u.pci.devfn, where);
	printk_spew( "Read config 16 bus %d,devfn 0x%x,reg 0x%x,val 0x%x\n",
		     dev->bus->secondary, dev->path.u.pci.devfn, where, value);
	return value;
}

uint32_t pci_read_config32(device_t dev, unsigned where)
{
	uint32_t value;
	value = conf->read32(dev->bus->secondary, dev->path.u.pci.devfn, where);
	printk_spew( "Read config 32 bus %d,devfn 0x%x,reg 0x%x,val 0x%x\n",
		     dev->bus->secondary, dev->path.u.pci.devfn, where, value);
	return value;
}

void pci_write_config8(device_t dev, unsigned where, uint8_t val)
{
	printk_spew( "Write config 8 bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     dev->bus->secondary, dev->path.u.pci.devfn, where, val);
	conf->write8(dev->bus->secondary, dev->path.u.pci.devfn, where, val);
}

void pci_write_config16(device_t dev, unsigned where, uint16_t val)
{
	printk_spew( "Write config 16 bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     dev->bus->secondary, dev->path.u.pci.devfn, where, val);
	conf->write16(dev->bus->secondary, dev->path.u.pci.devfn, where, val);
}

void pci_write_config32(device_t dev, unsigned where, uint32_t val)
{
	printk_spew( "Write config 32 bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     dev->bus->secondary, dev->path.u.pci.devfn, where, val);
	conf->write32(dev->bus->secondary, dev->path.u.pci.devfn, where, val);
}
