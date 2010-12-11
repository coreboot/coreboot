#include <console/console.h>
#include <arch/io.h>
#include <arch/pciconf.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
/*
 * Functions for accessing PCI configuration space with type 2 accesses
 */

#define IOADDR(devfn, where)	((0xC000 | ((devfn & 0x78) << 5)) + where)
#define FUNC(devfn)		(((devfn & 7) << 1) | 0xf0)
#define SET(bus,devfn)		outb(FUNC(devfn), 0xCF8); outb(bus, 0xCFA);

static uint8_t pci_conf2_read_config8(struct bus *pbus, int bus, int devfn, int where)
{
	uint8_t value;
	SET(bus, devfn);
	value = inb(IOADDR(devfn, where));
	outb(0, 0xCF8);
	return value;
}

static uint16_t pci_conf2_read_config16(struct bus *pbus, int bus, int devfn, int where)
{
	uint16_t value;
	SET(bus, devfn);
	value = inw(IOADDR(devfn, where));
	outb(0, 0xCF8);
	return value;
}

static uint32_t pci_conf2_read_config32(struct bus *pbus, int bus, int devfn, int where)
{
	uint32_t value;
	SET(bus, devfn);
	value = inl(IOADDR(devfn, where));
	outb(0, 0xCF8);
	return value;
}

static void pci_conf2_write_config8(struct bus *pbus, int bus, int devfn, int where, uint8_t value)
{
	SET(bus, devfn);
	outb(value, IOADDR(devfn, where));
	outb(0, 0xCF8);
}

static void pci_conf2_write_config16(struct bus *pbus, int bus, int devfn, int where, uint16_t value)
{
	SET(bus, devfn);
	outw(value, IOADDR(devfn, where));
	outb(0, 0xCF8);
}

static void pci_conf2_write_config32(struct bus *pbus, int bus, int devfn, int where, uint32_t value)
{
	SET(bus, devfn);
	outl(value, IOADDR(devfn, where));
	outb(0, 0xCF8);
}

#undef SET
#undef IOADDR
#undef FUNC

const struct pci_bus_operations pci_cf8_conf2 =
{
	.read8  = pci_conf2_read_config8,
	.read16 = pci_conf2_read_config16,
	.read32 = pci_conf2_read_config32,
	.write8  = pci_conf2_write_config8,
	.write16 = pci_conf2_write_config16,
	.write32 = pci_conf2_write_config32,
};

