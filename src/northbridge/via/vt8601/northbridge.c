#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <mem.h>
#include <part/sizeram.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/hypertransport.h>
#include <device/chip.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include "chip.h"
#include "northbridge.h"

struct mem_range *sizeram(void)
{
	unsigned long mmio_basek;
	static struct mem_range mem[10];
	device_t dev;
	int i, idx;

#warning "FIXME handle interleaved nodes"
	dev = dev_find_slot(0, PCI_DEVFN(0x18, 1));
	if (!dev) {
		printk_err("Cannot find PCI: 0:18.1\n");
		return 0;
	}
	mmio_basek = (dev_root.resource[1].base >> 10);
	/* Round mmio_basek to something the processor can support */
	mmio_basek &= ~((1 << 6) -1);

#if 1
#warning "FIXME improve mtrr.c so we don't use up all of the mtrrs with a 64M MMIO hole"
	/* Round the mmio hold to 256M */
	mmio_basek &= ~((256*1024) - 1);
#endif

#if 1
	printk_debug("mmio_base: %dKB\n", mmio_basek);
#endif

	for(idx = i = 0; i < 8; i++) {
		uint32_t base, limit;
		unsigned basek, limitk, sizek;
		base  = pci_read_config32(dev, 0x40 + (i<<3));
		limit = pci_read_config32(dev, 0x44 + (i<<3));
		if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
			continue;
		}
		basek = (base & 0xffff0000) >> 2;
		limitk = ((limit + 0x00010000) & 0xffff0000) >> 2;
		sizek = limitk - basek;
		if ((idx > 0) &&
			((mem[idx -1].basek + mem[idx - 1].sizek) == basek)) {
			mem[idx -1].sizek += sizek;
		}
		else {
			mem[idx].basek = basek;
			mem[idx].sizek = sizek;
			idx++;
		}
		/* See if I need to split the region to accomodate pci memory space */
		if ((mem[idx - 1].basek <= mmio_basek) &&
			((mem[idx - 1].basek + mem[idx - 1].sizek) >  mmio_basek)) {
			if (mem[idx - 1].basek < mmio_basek) {
				unsigned pre_sizek;
				pre_sizek = mmio_basek - mem[idx - 1].basek;
				mem[idx].basek = mmio_basek;
				mem[idx].sizek = mem[idx - 1].sizek - pre_sizek;
				mem[idx - 1].sizek = pre_sizek;
				idx++;
			}
			if ((mem[idx - 1].basek + mem[idx - 1].sizek) <= 4*1024*1024) {
				idx -= 1;
			}
			else {
				mem[idx - 1].basek = 4*1024*1024;
				mem[idx - 1].sizek -= (4*1024*1024 - mmio_basek);
			}
		}
	}
#if 0
	for(i = 0; i < idx; i++) {
		printk_debug("mem[%d].basek = %08x mem[%d].sizek = %08x\n",
			i, mem[i].basek, i, mem[i].sizek);
	}
#endif
	while(idx < sizeof(mem)/sizeof(mem[0])) {
		mem[idx].basek = 0;
		mem[idx].sizek = 0;
		idx++;
	}
	return mem;
}
static void enumerate(struct chip *chip)
{
	extern struct device_operations default_pci_ops_bus;
	chip_enumerate(chip);
	chip->dev->ops = &default_pci_ops_bus;
}

struct chip_control northbridge_via_vt8601_control = {
	.enumerate = enumerate,
	.name   = "VIA vt8601 Northbridge",
};
