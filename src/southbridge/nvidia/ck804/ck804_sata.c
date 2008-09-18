/*
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
 */
#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "ck804.h"


static void sata_com_reset(struct device *dev, unsigned reset)
// reset = 1 : reset
// reset = 0 : clear
{
	uint32_t *base;
	uint32_t dword;
	int loop;

	base = (uint32_t *) pci_read_config32(dev, 0x24);

	printk_debug("base = %08x\r\n", base);

	if(reset) {
	        *(base + 4) = 0xffffffff;
	        *(base + 0x44) = 0xffffffff;
	}

	dword = *(base +8);
	dword &= ~(0xf);
	dword |= reset;

	*(base + 8) = dword;
	*(base + 0x48) = dword;

#if 0
	udelay(1000);
	dword &= ~(0xf);
	*(base + 8) = dword;
	*(base + 0x48) = dword;
#endif

	if(reset) return;

	dword = *(base+ 0);
	printk_debug("*(base+0)=%08x\r\n",dword);
	if(dword == 0x113) {
		loop = 200000;// 2
		do {
		        dword = *(base + 4);
			if((dword & 0x10000)!=0) break;
			udelay(10);
		} while (--loop>0);
		printk_debug("loop=%d, *(base+4)=%08x\r\n",loop,  dword);
	}

	dword = *(base+ 0x40);
	printk_debug("*(base+0x40)=%08x\r\n",dword);
	if(dword == 0x113) {
	        loop = 200000;//2
	        do {
		        dword = *(base + 0x44);
		        if((dword & 0x10000)!=0) break;
			udelay(10);
		} while (--loop>0);
		printk_debug("loop=%d, *(base+0x44)=%08x\r\n",loop,  dword);
	}
}

static void sata_init(struct device *dev)
{

	uint32_t dword;

	struct southbridge_nvidia_ck804_config *conf;
	conf = dev->chip_info;

	dword = pci_read_config32(dev, 0x50);
	/* Ensure prefetch is disabled */
	dword &= ~((1 << 15) | (1 << 13));
	if (conf->sata1_enable) {
	        /* Enable secondary SATA interface */
	        dword |= (1<<0);
	        printk_debug("SATA S \t");
	}
	if (conf->sata0_enable) {
	        /* Enable primary SATA interface */
	        dword |= (1<<1);
	        printk_debug("SATA P \n");
	}
#if 0
//	write back
	dword |= (1<<12);
	dword |= (1<<14);
#endif

#if 0
//	ADMA
	dword |= (1<<16);
	dword |= (1<<17);
#endif

#if 1
//DO NOT relay OK and PAGE_FRNDLY_DTXFR_CNT.
	dword &= ~(0x1f<<24);
	dword |= (0x15<<24);
#endif
	pci_write_config32(dev, 0x50, dword);

#if 0
//SLUMBER_DURING_D3.
	dword = pci_read_config32(dev, 0x7c);
	dword &=  ~(1<<4);
	pci_write_config32(dev, 0x7c, dword);

	dword = pci_read_config32(dev, 0xd0);
	dword &=  ~(0xff<<24);
	dword |= (0x68<<24);
	pci_write_config32(dev, 0xd0, dword);

	dword = pci_read_config32(dev, 0xe0);
	dword &=  ~(0xff<<24);
	dword |= (0x68<<24);
	pci_write_config32(dev, 0xe0, dword);
#endif

	dword = pci_read_config32(dev, 0xf8);
	dword |= 2;
	pci_write_config32(dev, 0xf8, dword);

#if 0
	dword = pci_read_config32(dev, 0xac);
	dword &= ~((1<<13)|(1<<14));
	dword |= (1<<13)|(0<<14);
	pci_write_config32(dev, 0xac, dword);

	sata_com_reset(dev, 1); // for discover some s-atapi device
#endif

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
//	.enable           = ck804_enable,
	.init             = sata_init,
	.scan_bus         = 0,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver sata0_driver __pci_driver = {
	.ops    = &sata_ops,
	.vendor = PCI_VENDOR_ID_NVIDIA,
	.device = PCI_DEVICE_ID_NVIDIA_CK804_SATA0,
};

static const struct pci_driver sata1_driver __pci_driver = {
	.ops    = &sata_ops,
	.vendor = PCI_VENDOR_ID_NVIDIA,
	.device = PCI_DEVICE_ID_NVIDIA_CK804_SATA1,
};
