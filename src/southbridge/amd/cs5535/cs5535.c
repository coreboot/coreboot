
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "cs5535.h"

static void nvram_on(struct device *dev)
{
#if 0
	volatile char *flash = (volatile unsigned char *)0xFFFc0000;
	unsigned char id1, id2;
#endif
	unsigned char reg;

	/* Enable writes to flash at top of memory */
	pci_write_config8(dev, 0x52, 0xee);

	/* Set positive decode on ROM */
	/* Also, there is no apparent reason to turn off the devoce on the */
	/* IDE devices */
	
	reg = pci_read_config8(dev, 0x5b);
	reg |= 1 << 5;	/* ROM Decode */
	reg |= 1 << 3;	/* Primary IDE decode */
	reg |= 1 << 4;	/* Secondary IDE decode */

	pci_write_config8(dev, 0x5b, reg);

#if 0		// just to test if the flash is accessible!
	*(flash + 0x555) = 0xaa;
	*(flash + 0x2aa) = 0x55;
	*(flash + 0x555) = 0x90;

	id1 = *(volatile unsigned char *) flash;
	id2 = *(volatile unsigned char *) (flash + 1);

	*flash = 0xf0;

	printk_debug("Flash device: MFGID %02x, DEVID %02x\n", id1, id2);
#endif
}

	
static void southbridge_init(struct device *dev)
{
	printk_spew("cs5535: %s\n", __func__);
	nvram_on(dev);
}

/*
static void dump_south(struct device *dev)
{
	int i, j;

	for(i=0; i<256; i+=16) {
		printk_debug("0x%02x: ", i);
		for(j=0; j<16; j++)
			printk_debug("%02x ", pci_read_config8(dev, i+j));
		printk_debug("\n");
	}
}
*/

static void southbridge_enable(struct device *dev)
{
	printk_spew("%s: dev is %p\n", __func__, dev);
}

static void cs5535_pci_dev_enable_resources(device_t dev)
{
	printk_spew("cs5535.c: %s()\n", __func__);
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
}

static struct device_operations southbridge_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = cs5535_pci_dev_enable_resources,
	.init             = southbridge_init,
	.enable           = southbridge_enable,
	.scan_bus         = scan_static_bus,
};

static const struct pci_driver cs5535_pci_driver __pci_driver = {
	.ops 	= &southbridge_ops,
	.vendor = PCI_VENDOR_ID_NS,
	.device = PCI_DEVICE_ID_NS_CS5535
};
