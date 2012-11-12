#include <console/console.h>
#include <arch/io.h>
#include <arch/pciconf.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

/*
 * Functions for accessing PCI configuration space with mmconf accesses
 */

#define PCI_MMIO_ADDR(SEGBUS, DEVFN, WHERE)	\
				(CONFIG_MMCONF_BASE_ADDRESS |\
				(((SEGBUS) & 0xFFF) << 20) |\
				(((DEVFN) & 0xFF) << 12) |\
				((WHERE) & 0xFFF))

static uint8_t pci_mmconf_read_config8(struct bus *pbus, int bus, int devfn,
				       int where)
{
	return (read8(PCI_MMIO_ADDR(bus, devfn, where)));
}

static uint16_t pci_mmconf_read_config16(struct bus *pbus, int bus, int devfn,
					 int where)
{
	return (read16(PCI_MMIO_ADDR(bus, devfn, where) & ~1));
}

static uint32_t pci_mmconf_read_config32(struct bus *pbus, int bus, int devfn,
					 int where)
{
	return (read32(PCI_MMIO_ADDR(bus, devfn, where) & ~3));
}

static void pci_mmconf_write_config8(struct bus *pbus, int bus, int devfn,
				     int where, uint8_t value)
{
	write8(PCI_MMIO_ADDR(bus, devfn, where), value);
}

static void pci_mmconf_write_config16(struct bus *pbus, int bus, int devfn,
				      int where, uint16_t value)
{
	write16(PCI_MMIO_ADDR(bus, devfn, where) & ~1, value);
}

static void pci_mmconf_write_config32(struct bus *pbus, int bus, int devfn,
				      int where, uint32_t value)
{
	write32(PCI_MMIO_ADDR(bus, devfn, where) & ~3, value);
}

const struct pci_bus_operations pci_ops_mmconf = {
	.read8 = pci_mmconf_read_config8,
	.read16 = pci_mmconf_read_config16,
	.read32 = pci_mmconf_read_config32,
	.write8 = pci_mmconf_write_config8,
	.write16 = pci_mmconf_write_config16,
	.write32 = pci_mmconf_write_config32,
};
