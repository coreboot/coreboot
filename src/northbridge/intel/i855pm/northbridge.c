#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <mem.h>
#include <part/sizeram.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/chip.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include "chip.h"

struct mem_range *sizeram(void)
{
	static struct mem_range mem[4];
	/* the units of tolm are 64 KB */
	/* the units of drb16 are 64 MB */
	uint16_t tolm, remapbase, remaplimit, drb16;
	uint16_t tolm_r, remapbase_r, remaplimit_r;
	uint8_t  drb;
	int remap_high;
        device_t dev;

        dev = dev_find_slot(0, 0); // d0f0
	if (!dev) {
                printk_err("Cannot find PCI: 0:0\n");
                return 0;
        }
	
	/* Calculate and report the top of low memory and 
	 * any remapping.
	 */
	/* Test if the remap memory high option is set */
        remap_high = 0;
//        if(get_option(&remap_high, "remap_memory_high")){
//                remap_high = 0;
//        }
	printk_debug("remap_high is %d\n", remap_high);
	/* get out the value of the highest DRB. This tells the end of 
	 * physical memory. The units are ticks of 64 MB i.e. 1 means
	 * 64 MB. 
	 */
	drb = pci_read_config8(dev, 0x67);
	drb16 = (uint16_t)drb;
	if(remap_high && (drb16 > 0x08)) {
		/* We only come here if we have at least 512MB of memory,
		 * so it is safe to hard code tolm.
		 * 0x2000 means 512MB 
		 */

		tolm = 0x2000;
		/* i.e 0x40 * 0x40 is 0x1000 which is 4 GB */
		if(drb16 > 0x0040) {
			/* There is more than 4GB of memory put
			 * the remap window at the end of ram.
			 */
			remapbase = drb16;
			remaplimit = remapbase + 0x38;
		}
		else {
			remapbase = 0x0040;
			remaplimit = remapbase + (drb16-8);
		}
	}
	else {
		tolm = (uint16_t)((dev_root.resource[1].base >> 16)&0x0f800);
		if((tolm>>8) >= (drb16<<2)) {
			tolm = (drb16<<10);
			remapbase = 0x3ff;
			remaplimit = 0;
		}
		else {
			remapbase = drb16;
			remaplimit = remapbase + ((0x0040-(tolm>>10))-1);
		}
	}
	/* Write the ram configruation registers,
	 * preserving the reserved bits.
	 */
	tolm_r = pci_read_config16(dev, 0xc4);
	tolm |= (tolm_r & 0x7ff); 
	pci_write_config16(dev, 0xc4, tolm);
	remapbase_r = pci_read_config16(dev, 0xc6);
	remapbase |= (remapbase_r & 0xfc00);
	pci_write_config16(dev, 0xc6, remapbase);
	remaplimit_r = pci_read_config16(dev, 0xc8);
	remaplimit |= (remaplimit_r & 0xfc00);
	pci_write_config16(dev, 0xc8, remaplimit);

#if 0
    printk_debug("mem info tolm = %x, drb = %x, pci_memory_base = %x, remap = %x-%x\n",tolm,drb,pci_memory_base,remapbase,remaplimit);
#endif
	
	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 768;
	/* Convert size in 64K bytes to size in K bytes */
	mem[1].sizek = (tolm << 6) - mem[1].basek;
	mem[2].basek = 0;
	mem[2].sizek = 0;
	if ((drb << 16) > (tolm << 6)) {
		/* We don't need to consider the remap window
		 * here because we put it immediately after the
		 * rest of ram.
		 * All we must do is calculate the amount
		 * of unused memory and report it at 4GB.
		 */
		mem[2].basek = 4096*1024;
		mem[2].sizek = (drb << 16) - (tolm << 6);
	}
	mem[3].basek = 0;
	mem[3].sizek = 0;
	
	return mem;
}
static void enumerate(struct chip *chip)
{
        extern struct device_operations default_pci_ops_bus;
        chip_enumerate(chip);
        chip->dev->ops = &default_pci_ops_bus;
}
#if 0
static void northbridge_init(struct chip *chip, enum chip_pass pass)
{

        struct northbridge_intel_i855pm_config *conf =
                (struct northbridge_intel_i855pm_config *)chip->chip_info;

        switch (pass) {
        case CONF_PASS_PRE_PCI:
                break;

        case CONF_PASS_POST_PCI:
                break;

        case CONF_PASS_PRE_BOOT:
                break;

        default:
                /* nothing yet */
                break;
        }
}
#endif

struct chip_control northbridge_intel_i855pm_control = {
        .enumerate = enumerate,
//        .enable    = northbridge_init,
        .name      = "intel i855pm Northbridge",
};
