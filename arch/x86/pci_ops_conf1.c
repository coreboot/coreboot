#include <console/console.h>
#include <device/device.h>
#include <arch/pciconf.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/types.h>
#include <arch/io.h>

/*
 * Functions for accessing PCI configuration space with type 1 accesses
 */

/* this shit really should come with comments ...this is annoying.
#if PCI_IO_CFG_EXT == 0
#define CONFIG_CMD(bus,devfn, where)   (0x80000000 | (bus << 16) | (devfn << 8) | (where & ~3))
#else
#define CONFIG_CMD(bus,devfn, where)   (0x80000000 | (bus << 16) | (devfn << 8) | ((where & 0xff) & ~3) | ((where & 0xf00)<<16) )
#endif
*/
#define CONFIG_CMD(bus,devfn, where)   (0x80000000 | (bus << 16) | (devfn << 8) | (where & ~3))

static u8 pci_conf1_read_config8(struct bus *pbus, int bus, int devfn, int where)
{
		outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
		return inb(0xCFC + (where & 3));
}

static u16 pci_conf1_read_config16(struct bus *pbus, int bus, int devfn, int where)
{
		outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
		return inw(0xCFC + (where & 2));
}

static u32 pci_conf1_read_config32(struct bus *pbus, int bus, int devfn, int where)
{
		outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
		return inl(0xCFC);
}

static void  pci_conf1_write_config8(struct bus *pbus, int bus, int devfn, int where, u8 value)
{
		outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
		outb(value, 0xCFC + (where & 3));
}

static void pci_conf1_write_config16(struct bus *pbus, int bus, int devfn, int where, u16 value)
{
		outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
		outw(value, 0xCFC + (where & 2));
}

static void pci_conf1_write_config32(struct bus *pbus, int bus, int devfn, int where, u32 value)
{
		outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
		outl(value, 0xCFC);
}

#undef CONFIG_CMD

struct pci_bus_operations pci_cf8_conf1 =
{
	.read8  = pci_conf1_read_config8,
	.read16 = pci_conf1_read_config16,
	.read32 = pci_conf1_read_config32,
	.write8  = pci_conf1_write_config8,
	.write16 = pci_conf1_write_config16,
	.write32 = pci_conf1_write_config32,
};
