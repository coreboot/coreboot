
#include <arch/io.h>
#include <arch/ioapic.h>
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

	printk(BIOS_DEBUG, "Flash device: MFGID %02x, DEVID %02x\n", id1, id2);
#endif
}


static void southbridge_init(struct device *dev)
{
	printk(BIOS_SPEW, "cs5535: %s\n", __func__);
	nvram_on(dev);
}

/*
static void dump_south(struct device *dev)
{
	int i, j;

	for(i=0; i<256; i+=16) {
		printk(BIOS_DEBUG, "0x%02x: ", i);
		for(j=0; j<16; j++)
			printk(BIOS_DEBUG, "%02x ", pci_read_config8(dev, i+j));
		printk(BIOS_DEBUG, "\n");
	}
}
*/

static void southbridge_enable(struct device *dev)
{
	printk(BIOS_SPEW, "%s: dev is %p\n", __func__, dev);
}

static void cs5535_read_resources(device_t dev)
{
	struct resource *res;

	pci_dev_read_resources(dev);

	res = new_resource(dev, 1);
	res->base = 0x0UL;
	res->size = 0x1000UL;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static struct device_operations southbridge_ops = {
	.read_resources   = cs5535_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = southbridge_init,
	.enable           = southbridge_enable,
	.scan_bus         = scan_static_bus,
};

static const struct pci_driver cs5535_pci_driver __pci_driver = {
	.ops 	= &southbridge_ops,
	.vendor = PCI_VENDOR_ID_NS,
	.device = PCI_DEVICE_ID_NS_CS5535
};

struct chip_operations southbridge_amd_cs5535_ops = {
        CHIP_NAME("AMD Geode CS5535 Southbridge")
            /* This is only called when this device is listed in the
             * static device tree.
             */
            .enable_dev = southbridge_enable,
};
