#include <console/console.h>
#include <arch/pciconf.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

static uint8_t ppc_conf1_read_config8(struct bus *pbus, unsigned char bus, int devfn, int where)
{
	return pci_ppc_read_config8(bus, devfn, where);
}

static uint16_t ppc_conf1_read_config16(struct bus *pbus, unsigned char bus, int devfn, int where)
{
	return pci_ppc_read_config16(bus, devfn, where);
}

static uint32_t ppc_conf1_read_config32(struct bus *pbus, unsigned char bus, int devfn, int where)
{
	return pci_ppc_read_config32(bus, devfn, where);
}

static void ppc_conf1_write_config8(struct bus *pbus, unsigned char bus, int devfn, int where, uint8_t val)
{
	pci_ppc_write_config8(bus, devfn, where, val);
}

static void ppc_conf1_write_config16(struct bus *pbus, unsigned char bus, int devfn, int where, uint16_t val)
{
	pci_ppc_write_config16(bus, devfn, where, val);
}

static void ppc_conf1_write_config32(struct bus *pbus, unsigned char bus, int devfn, int where, uint32_t val)
{
	pci_ppc_write_config32(bus, devfn, where, val);
}

const struct pci_bus_operations pci_ppc_conf1 =
{
	.read8  = ppc_conf1_read_config8,
	.read16 = ppc_conf1_read_config16,
	.read32 = ppc_conf1_read_config32,
	.write8  = ppc_conf1_write_config8,
	.write16 = ppc_conf1_write_config16,
	.write32 = ppc_conf1_write_config32,
};
