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

	dev = dev_find_slot(0, 0);
	if (!dev) {
		printk_err("Cannot find PCI: 0:0\n");
		return 0;
	}
	mem[0].basek = 0;
	mem[0].sizek = 65536;
	idx = 1;
	while(idx < sizeof(mem)/sizeof(mem[0])) {
		mem[idx].basek = 0;
		mem[idx].sizek = 0;
		idx++;
	}

#if 1
	for(i = 0; i < idx; i++) {
		printk_debug("mem[%d].basek = %08x mem[%d].sizek = %08x\n",
			i, mem[i].basek, i, mem[i].sizek);
	}
#endif

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
