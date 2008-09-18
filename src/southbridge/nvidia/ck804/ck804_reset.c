/*
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
 */

#include <arch/io.h>

#define PCI_DEV(BUS, DEV, FN) ( \
	(((BUS) & 0xFFF) << 20) | \
	(((DEV) & 0x1F) << 15) | \
	(((FN)  & 0x7) << 12))

typedef unsigned device_t;

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

#include "../../../northbridge/amd/amdk8/reset_test.c"

void hard_reset(void)
{
	set_bios_reset();
	/* Try rebooting through port 0xcf9 */
	outb((0 <<3)|(0<<2)|(1<<1), 0xcf9);
      	outb((0 <<3)|(1<<2)|(1<<1), 0xcf9);
}

