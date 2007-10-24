/* Copyright 2003-2004 Tyan Computer*/

/* Author: Yinghai Lu yhlu@tyan.com
 *
 */


#include <delay.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>


static void si_sata_init(struct device *dev)
{
	uint16_t word;
	uint32_t dword;
#if 0
        word = pci_read_config16(dev, 0x4);
        word |= ((1 << 2) |(1<<4)); // Command: 3--> 17
        pci_write_config16(dev, 0x4, word);
#endif

	/* some driver change class code to 0x104, but not change deviceid without reason*/
        /* restore it so we don't need to unplug AC power to restore it*/
	
	word = pci_read_config16(dev, 0x0a);
	if(word!=0x0180) {
		 /* enble change device id and class id*/	
	        dword = pci_read_config32(dev,0x40);
	        dword |= (1<<0);
        	pci_write_config32(dev, 0x40, dword);

		word = 0x0180;
		pci_write_config16(dev, 0x0a, word);

	        /* disable change device id and class id*/
        	dword = pci_read_config32(dev,0x40);
        	dword &= ~(1<<0);
        	pci_write_config32(dev, 0x40, dword);

		printk_debug("Class code restored.\n");

	}

	
}
static struct device_operations si_sata_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = si_sata_init,
	.scan_bus         = 0,
};

static const struct pci_driver si_sata_driver __pci_driver = {
        .ops    = &si_sata_ops,
        .vendor = 0x1095,
        .device = 0x3114,
};
 
