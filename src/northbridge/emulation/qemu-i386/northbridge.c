#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <mem.h>
#include <part/sizeram.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/hypertransport.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include "chip.h"
#include "northbridge.h"

void hard_reset(void)
{
	printk_err("Hard_RESET!!!\n");
}

struct mem_range *sizeram(void)
{
	unsigned long mmio_basek;
	static struct mem_range mem[10];
	device_t dev;
	int i, idx;
	unsigned char rambits;

	dev = dev_find_slot(0, 0);
	if (!dev) {
		printk_err("Cannot find PCI: 0:0\n");
		return 0;
	}
	mem[0].basek = 0;
	mem[0].sizek = 65536;
#if 0
	idx = 1;
	while(idx < sizeof(mem)/sizeof(mem[0])) {
		mem[idx].basek = 0;
		mem[idx].sizek = 0;
		idx++;
	}
	for(rambits = 0, i = 0; i < sizeof(ramregs)/sizeof(ramregs[0]); i++) {
		unsigned char reg;
		reg = pci_read_config8(dev, ramregs[i]);
		/* these are ENDING addresses, not sizes. 
		 * if there is memory in this slot, then reg will be > rambits.
		 * So we just take the max, that gives us total. 
		 * We take the highest one to cover for once and future linuxbios
		 * bugs. We warn about bugs.
		 */
		if (reg > rambits)
			rambits = reg;
		if (reg < rambits)
			printk_err("ERROR! register 0x%x is not set!\n", 
				ramregs[i]);
	}
	
	printk_debug("I would set ram size to 0x%x Kbytes\n", (rambits)*8*1024);
	mem[0].sizek = rambits*8*1024;
#endif
#if 1
	for(i = 0; i < idx; i++) {
		printk_debug("mem[%d].basek = %08x mem[%d].sizek = %08x\n",
			i, mem[i].basek, i, mem[i].sizek);
	}
#endif

	return mem;
}

struct chip_operations northbridge_emulation_qemu_i386_control = {
	.name      = "QEMU Northbridge",
};
