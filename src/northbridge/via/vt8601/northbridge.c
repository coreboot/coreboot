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

static const uint8_t ramregs[] = {0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 
	  				0x56, 0x57};

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
#if 1
	for(i = 0; i < idx; i++) {
		printk_debug("mem[%d].basek = %08x mem[%d].sizek = %08x\n",
			i, mem[i].basek, i, mem[i].sizek);
	}
#endif

	return mem;
}

/*
 * This fixup is based on capturing values from an Award bios.  Without
 * this fixup the DMA write performance is awful (i.e. hdparm -t /dev/hda is 20x
 * slower than normal, ethernet drops packets).
 * Apparently these registers govern some sort of bus master behavior.
 */
static void random_fixup() {
	device_t pcidev = dev_find_slot(0, 0);

	printk_spew("VT8601 random fixup ...\n");
	if (pcidev) {
		pci_write_config8(pcidev, 0x70, 0xc0);
		pci_write_config8(pcidev, 0x71, 0x88);
		pci_write_config8(pcidev, 0x72, 0xec);
		pci_write_config8(pcidev, 0x73, 0x0c);
		pci_write_config8(pcidev, 0x74, 0x0e);
		pci_write_config8(pcidev, 0x75, 0x81);
		pci_write_config8(pcidev, 0x76, 0x52);
	}
}

static void northbridge_init(struct chip *chip, enum chip_pass pass)
{

	struct northbridge_via_vt8601_config *conf = 
		(struct northbridge_via_vt8601_config *)chip->chip_info;

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

struct chip_operations northbridge_via_vt8601_control = {
	.enable    = northbridge_init,
	.name      = "VIA vt8601 Northbridge",
};
