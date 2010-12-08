#include <arch/io.h>
#include <reset.h>
#include <device/pci_ids.h>

#define PCI_DEV(BUS, DEV, FN) ( \
	(((BUS) & 0xFFF) << 20) | \
	(((DEV) & 0x1F) << 15) | \
	(((FN)  & 0x7) << 12))

#define PCI_ID(VENDOR_ID, DEVICE_ID) \
	((((DEVICE_ID) & 0xFFFF) << 16) | ((VENDOR_ID) & 0xFFFF))

typedef unsigned device_t;

static void pci_write_config8(device_t dev, unsigned where, unsigned char value)
{
        unsigned addr;
        addr = (dev>>4) | where;
        outl(0x80000000 | (addr & ~3), 0xCF8);
        outb(value, 0xCFC + (addr & 3));
}

static void pci_write_config32(device_t dev, unsigned where, unsigned value)
{
	unsigned addr;
        addr = (dev>>4) | where;
        outl(0x80000000 | (addr & ~3), 0xCF8);
        outl(value, 0xCFC);
}

static unsigned pci_read_config32(device_t dev, unsigned where)
{
	unsigned addr;
        addr = (dev>>4) | where;
        outl(0x80000000 | (addr & ~3), 0xCF8);
        return inl(0xCFC);
}

#define PCI_DEV_INVALID (0xffffffffU)
static device_t pci_locate_device_on_bus(unsigned pci_id, unsigned bus)
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


void hard_reset(void)
{
	device_t dev;
	unsigned bus;
	unsigned node = 0;
	unsigned link = get_sblk();

	/* Find the device.
	 * There can only be one 8111 on a hypertransport chain/bus.
	 */
	bus = node_link_to_bus(node, link);
	dev = pci_locate_device_on_bus(
		PCI_ID(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_8111_ISA),
		bus);

	/* Reset */
	set_bios_reset();
	pci_write_config8(dev, 0x47, 1);
}
