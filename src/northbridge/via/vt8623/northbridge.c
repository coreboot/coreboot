#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <mem.h>
#include <part/sizeram.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/hypertransport.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include <cpu/p6/mtrr.h>
#include "chip.h"
#include "northbridge.h"

static const uint8_t ramregs[] = {0x5a, 0x5b, 0x5c, 0x5d };

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
	mem[0].sizek = 0xa0000 >>10; // first 640k
	mem[1].basek = 0xc0000 >>10; // leave a hole for vga
	idx = 2;
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
	
	printk_debug("I would set ram size to 0x%x Kbytes\n", (rambits)*16*1024);
	mem[1].sizek = rambits*16*1024 - 32768 - (0xc0000 >> 10);
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

/*
 * This fixup is based on capturing values from an Award bios.  Without
 * this fixup the DMA write performance is awful (i.e. hdparm -t /dev/hda is 20x
 * slower than normal, ethernet drops packets).
 * Apparently these registers govern some sort of bus master behavior.
 */
static void random_fixup() {
	device_t pcidev0 = dev_find_slot(0, 0);
	device_t pcidev1,pcidev2;
	unsigned long fb;
	unsigned char c;

	printk_debug("VT8623 random fixup ...\n");
	if (pcidev0) {
		pci_write_config8(pcidev0, 0x0d, 0x08);
		pci_write_config8(pcidev0, 0x70, 0x82);
		pci_write_config8(pcidev0, 0x71, 0xc8);
		pci_write_config8(pcidev0, 0x72, 0x0);
		pci_write_config8(pcidev0, 0x73, 0x01);
		pci_write_config8(pcidev0, 0x74, 0x01);
		pci_write_config8(pcidev0, 0x75, 0x08);
		pci_write_config8(pcidev0, 0x76, 0x52);
		pci_write_config8(pcidev0, 0x13, 0xd0);
		pci_write_config8(pcidev0, 0x84, 0x80);
		pci_write_config16(pcidev0,0x80, 0x610f);
		pci_write_config32(pcidev0,0x88, 0x02);
	}
	printk_debug("VT8623 AGP random fixup ...\n");
	pcidev1 = dev_find_device(PCI_VENDOR_ID_VIA,0xb091,0);
	if( pcidev1) {
		pci_write_config8(pcidev1,0x3e,0x0c);
		pci_write_config8(pcidev1,0x40,0x83);
		pci_write_config8(pcidev1,0x41,0xc5);
		pci_write_config8(pcidev1,0x43,0x44);
		pci_write_config8(pcidev1,0x44,0x34);
		pci_write_config8(pcidev1,0x83,0x02);
	}
	printk_debug("VGA random fixup ...\n");
	pcidev2 = dev_find_device(PCI_VENDOR_ID_VIA,0x3122,0);
	if( pcidev2 ){
		pci_write_config8(pcidev2,0x04,0x07);
		pci_write_config8(pcidev2,0x0d,0x20);
	}
	// fixup GART and framebuffer addresses properly
	// first set up frame buffer properly
	fb = pci_read_config32(pcidev2,0x10);  // base address of framebuffer
	printk_debug("Frame buffer at %8x\n",fb);
	c = pci_read_config8(pcidev0,0xe1) & 0xf0;    // size of vga
	c |= fb>>28;  // upper nibble of frame buffer address
	pci_write_config8(pcidev0,0xe1,c);
	c = (fb>>20) | 1; // enable framebuffer
	pci_write_config8(pcidev0,0xe0,c);
	pci_write_config8(pcidev0,0xe2,0x42); // 'cos award does

	
}
static void set_vga_mtrrs(void)
{
	device_t pcidev = dev_find_device(PCI_VENDOR_ID_VIA,0x3122,0);
	unsigned long fb;

	add_var_mtrr( 0xd0000000 >> 10, 0x08000000>>10, MTRR_TYPE_WRCOMB);
	fb = pci_read_config32(pcidev,0x10); // get the fb address
	add_var_mtrr( fb>>10, 8192, MTRR_TYPE_WRCOMB);
	

}

static void northbridge_init(struct chip *chip, enum chip_pass pass)
{

	struct northbridge_via_vt8623_config *conf = 
		(struct northbridge_via_vt8623_config *)chip->chip_info;

	switch (pass) {
	case CONF_PASS_PRE_PCI:
		break;
		
	case CONF_PASS_POST_PCI:
		random_fixup();
		break;
		
	case CONF_PASS_PRE_BOOT:
		set_vga_mtrrs();
		break;
		
	default:
		/* nothing yet */
		break;
	}
}

struct chip_control northbridge_via_vt8623_control = {
	.enumerate = enumerate,
	.enable    = northbridge_init,
	.name      = "VIA vt8623 Northbridge",
};
