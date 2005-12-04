#if 0
//#include "arch/romcc_io.h"
#include <arch/io.h>

typedef unsigned device_t;

#define PCI_DEV(BUS, DEV, FN) ( \
	(((BUS) & 0xFF) << 16) | \
	(((DEV) & 0x1f) << 11) | \
	(((FN)  & 0x7) << 8))

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
#endif
void hard_reset(void)
{
	//set_bios_reset();
	//pci_write_config8(PCI_DEV(1, 0x04, 0), 0x47, 1);
}

