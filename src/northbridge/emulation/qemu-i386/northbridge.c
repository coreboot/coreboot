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

static void enumerate(struct chip *chip)
{
	extern struct device_operations default_pci_ops_bus;
	chip_enumerate(chip);
	chip->dev->ops = &default_pci_ops_bus;
}

static void random_fixup() {
	device_t pcidev = dev_find_slot(0, 0);

	printk_warning("QEMU random fixup ...\n");
	if (pcidev) {
		// pci_write_config8(pcidev, 0x0, 0x0);
	}
}

static void northbridge_init(struct chip *chip, enum chip_pass pass)
{

	struct northbridge_dummy_qemu_i386_config *conf = 
		(struct northbridge_dummy_qemu_i386_config *)chip->chip_info;

	switch (pass) {
	case CONF_PASS_PRE_PCI:
		break;
		
	case CONF_PASS_POST_PCI:
		break;
		
	case CONF_PASS_PRE_BOOT:
		random_fixup();
		break;
		
	default:
		/* nothing yet */
		break;
	}
}

struct chip_control northbridge_emulation_qemu_i386_control = {
	.enumerate = enumerate,
	.enable    = northbridge_init,
	.name      = "QEMU Northbridge",
};
