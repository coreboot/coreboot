#include <arch/io.h>

#define PCI_DEV(BUS, DEV, FN) ( \
	(((BUS) & 0xFF) << 16) | \
	(((DEV) & 0x1f) << 11) | \
	(((FN)  & 0x7) << 8))

#define AMD8111_RESET PCI_DEV(     \
		HARD_RESET_BUS,    \
		HARD_RESET_DEVICE, \
		HARD_RESET_FUNCTION)

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

#include "../../../northbridge/amd/amdk8/reset_test.c"

void hard_reset(void)
{
	set_bios_reset();
	pci_write_config8(AMD8111_RESET, 0x47, 1);
}
