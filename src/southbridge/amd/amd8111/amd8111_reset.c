/* Include this file in the mainboards reset.c
 */
#include <arch/io.h>
#include <device/pci_ids.h>

#define PCI_DEV(BUS, DEV, FN) ( \
	(((BUS) & 0xFF) << 16) | \
	(((DEV) & 0x1f) << 11) | \
	(((FN)  & 0x7) << 8))

#define PCI_ID(VENDOR_ID, DEVICE_ID) \
	((((DEVICE_ID) & 0xFFFF) << 16) | ((VENDOR_ID) & 0xFFFF))

typedef unsigned device_t;

static void pci_write_config8(device_t dev, unsigned where, unsigned char value)
{
        unsigned addr;
        addr = dev | where;
        outl(0x80000000 | (addr & ~3), 0xCF8);
        outb(value, 0xCFC + (addr & 3));
}

static void pci_write_config32(device_t dev, unsigned where, unsigned value)
{
	unsigned addr;
        addr = dev | where;
        outl(0x80000000 | (addr & ~3), 0xCF8);
        outl(value, 0xCFC);
}

static unsigned pci_read_config32(device_t dev, unsigned where)
{
	unsigned addr;
        addr = dev | where;
        outl(0x80000000 | (addr & ~3), 0xCF8);
        return inl(0xCFC);
}

#define PCI_DEV_INVALID (0xffffffffU)
static device_t pci_locate_device(unsigned pci_id, unsigned bus)
{
	device_t dev, last;
	dev = PCI_DEV(bus, 0, 0);
	last = PCI_DEV(bus, 31, 7);
	for(; dev <= last; dev += PCI_DEV(0,0,1)) {
		unsigned int id;
		id = pci_read_config32(dev, 0);
		if (id == pci_id) {
			return dev;
		}
	}
	return PCI_DEV_INVALID;
}

#include "../../../northbridge/amd/amdk8/reset_test.c"

static unsigned node_link_to_bus(unsigned node, unsigned link)
{
	unsigned reg;

	for(reg = 0xE0; reg < 0xF0; reg += 0x04) {
		unsigned config_map;
		config_map = pci_read_config32(PCI_DEV(0, 0x18, 1), reg);
		if ((config_map & 3) != 3) {
			continue;
		}
		if ((((config_map >> 4) & 7) == node) &&
			(((config_map >> 8) & 3) == link)) 
		{
			return (config_map >> 16) & 0xff;
		}
	}
	return 0;
}

static void amd8111_hard_reset(unsigned node, unsigned link)
{
	device_t dev;
	unsigned bus;

	/* Find the device.
	 * There can only be one 8111 on a hypertransport chain/bus.
	 */
	bus = node_link_to_bus(node, link);
	dev = pci_locate_device(
		PCI_ID(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_8111_ISA), 
		bus);

	/* Reset */
	set_bios_reset();
	pci_write_config8(dev, 0x47, 1);
}
