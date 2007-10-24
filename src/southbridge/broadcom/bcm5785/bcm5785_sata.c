/*
 * Copyright  2005 AMD
 *  by yinghai.lu@amd.com
 */

#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "bcm5785.h"


static void sata_init(struct device *dev)
{

	uint8_t byte;

        uint8_t *base;
	uint8_t *mmio;
        struct resource *res;
        unsigned int mmio_base;
        volatile unsigned int *mmio_reg;
	int i;

	if(!(dev->path.u.pci.devfn & 7)) { // only set it in Func0
		byte = pci_read_config8(dev, 0x78);
		byte |= (1<<7);
        	pci_write_config8(dev, 0x78, byte);

	        res = find_resource(dev, 0x24);
	        base = res->base;

                mmio_base = base;
                mmio_base &= 0xfffffffc;
                mmio_reg = (unsigned int *)( mmio_base + 0x10f0 );
                * mmio_reg = 0x40000001;
                mmio_reg = ( unsigned int *)( mmio_base + 0x8c );
                * mmio_reg = 0x00ff2007;
                mdelay( 10 );
                * mmio_reg = 0x78592009;
                mdelay( 10 );
                * mmio_reg = 0x00082004;
                mdelay( 10 );
                * mmio_reg = 0x00002004;
                mdelay( 10 );

		//init PHY

		printk_debug("init PHY...\n");
		for(i=0; i<4; i++) {
			mmio = base + 0x100 * i; 
			byte = readb(mmio + 0x40);
			printk_debug("port %d PHY status = %02x\r\n", i, byte);
			if(byte & 0x4) {// bit 2 is set
				byte = readb(mmio+0x48);
				writeb(byte | 1, mmio + 0x48);
				writeb(byte & (~1), mmio + 0x48);
	                        byte = readb(mmio + 0x40);
	                        printk_debug("after reset port %d PHY status = %02x\r\n", i, byte);
			}
		}
		
	}


}

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
        pci_write_config32(dev, 0x40,
                ((device & 0xffff) << 16) | (vendor & 0xffff));
}
static struct pci_operations lops_pci = {
        .set_subsystem = lpci_set_subsystem,
};

static struct device_operations sata_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
//	.enable           = bcm5785_enable,
	.init             = sata_init,
	.scan_bus         = 0,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver sata0_driver __pci_driver = {
	.ops    = &sata_ops,
	.vendor = PCI_VENDOR_ID_SERVERWORKS,
	.device = PCI_DEVICE_ID_SERVERWORKS_BCM5785_SATA,
};

