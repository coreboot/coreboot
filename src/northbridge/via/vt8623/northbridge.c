#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/hypertransport.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include "chip.h"
#include "northbridge.h"

/*
 * This fixup is based on capturing values from an Award BIOS.  Without
 * this fixup the DMA write performance is awful (i.e. hdparm -t /dev/hda is 20x
 * slower than normal, ethernet drops packets).
 * Apparently these registers govern some sort of bus master behavior.
 */

static void northbridge_init(device_t dev)
{
	device_t fb_dev;
	unsigned long fb;
	unsigned char c;

	printk(BIOS_DEBUG, "VT8623 random fixup ...\n");
	pci_write_config8(dev,  0x0d, 0x08);
	pci_write_config8(dev,  0x70, 0x82);
	pci_write_config8(dev,  0x71, 0xc8);
	pci_write_config8(dev,  0x72, 0x00);
	pci_write_config8(dev,  0x73, 0x01);
	pci_write_config8(dev,  0x74, 0x01);
	pci_write_config8(dev,  0x75, 0x08);
	pci_write_config8(dev,  0x76, 0x52);
	pci_write_config8(dev,  0x13, 0xd0);
	pci_write_config8(dev,  0x84, 0x80);
	pci_write_config16(dev, 0x80, 0x610f);
	pci_write_config32(dev, 0x88, 0x00000002);

	fb_dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x3122, 0);
	if (fb_dev) {
		/* Fixup GART and framebuffer addresses properly.
		 * First setup frame buffer properly.
		 */
		//fb = pci_read_config32(dev, 0x10);       /* Base addres of framebuffer */
		fb = 0xd0000000;
		printk(BIOS_DEBUG, "Frame buffer at %8lx\n",fb);

		c = pci_read_config8(dev, 0xe1) & 0xf0;  /* size of vga */
		c |= fb>>28;  /* upper nibble of frame buffer address */
		c = 0xdd;
		pci_write_config8(dev, 0xe1, c);
		c = 0x81;                                /* enable framebuffer */
		pci_write_config8(dev, 0xe0, c);
		pci_write_config8(dev, 0xe2, 0x42);      /* 'cos award does */
	}
}

static void nullfunc(device_t dev)
{
	/* Nothing to do */
}

static struct device_operations northbridge_operations = {
	.read_resources   = nullfunc,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = northbridge_init
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops = &northbridge_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_8623,
};

static void agp_init(device_t dev)
{
	printk(BIOS_DEBUG, "VT8623 AGP random fixup ...\n");

	pci_write_config8(dev, 0x3e, 0x0c);
	pci_write_config8(dev, 0x40, 0x83);
	pci_write_config8(dev, 0x41, 0xc5);
	pci_write_config8(dev, 0x43, 0x44);
	pci_write_config8(dev, 0x44, 0x34);
	pci_write_config8(dev, 0x83, 0x02);
}

static struct device_operations agp_operations = {
	.read_resources   = nullfunc,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = agp_init,
	.scan_bus         = pci_scan_bridge,
	.ops_pci          = 0,
};

static const struct pci_driver agp_driver __pci_driver = {
	.ops = &agp_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_8633_1,
};

static void ram_resource(device_t dev, unsigned long index,
        unsigned long basek, unsigned long sizek)
{
        struct resource *resource;

        if (!sizek) {
                return;
        }
        resource = new_resource(dev, index);
        resource->base  = ((resource_t)basek) << 10;
        resource->size  = ((resource_t)sizek) << 10;
        resource->flags =  IORESOURCE_MEM | IORESOURCE_CACHEABLE | \
                IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}

static void tolm_test(void *gp, struct device *dev, struct resource *new)
{
	struct resource **best_p = gp;
	struct resource *best;
	best = *best_p;
	if (!best || (best->base > new->base)) {
		best = new;
	}
	*best_p = best;
}

static uint32_t find_pci_tolm(struct bus *bus)
{
	struct resource *min;
	uint32_t tolm;
	min = 0;
	search_bus_resources(bus, IORESOURCE_MEM, IORESOURCE_MEM, tolm_test, &min);
	tolm = 0xffffffffUL;
	if (min && tolm > min->base) {
		tolm = min->base;
	}
	return tolm;
}

#if CONFIG_WRITE_HIGH_TABLES==1
/* maximum size of high tables in KB */
#define HIGH_TABLES_SIZE 64
extern uint64_t high_tables_base, high_tables_size;
#endif

static void pci_domain_set_resources(device_t dev)
{
	static const uint8_t ramregs[] = {0x5a, 0x5b, 0x5c, 0x5d };
	device_t mc_dev;
        uint32_t pci_tolm;

	printk(BIOS_SPEW, "Entering vt8623 pci_domain_set_resources.\n");

        pci_tolm = find_pci_tolm(&dev->link[0]);
	mc_dev = dev->link[0].children;
	if (mc_dev) {
		unsigned long tomk, tolmk;
		unsigned char rambits;
		int i, idx;

		for(rambits = 0, i = 0; i < ARRAY_SIZE(ramregs); i++) {
			unsigned char reg;
			reg = pci_read_config8(mc_dev, ramregs[i]);
			/* these are ENDING addresses, not sizes.
			 * if there is memory in this slot, then reg will be > rambits.
			 * So we just take the max, that gives us total.
			 * We take the highest one to cover for once and future coreboot
			 * bugs. We warn about bugs.
			 */
			if (reg > rambits)
				rambits = reg;
			if (reg < rambits)
				printk(BIOS_ERR, "ERROR! register 0x%x is not set!\n",
					ramregs[i]);
		}
		printk(BIOS_DEBUG, "I would set ram size to 0x%x Kbytes\n", (rambits)*16*1024);
		tomk = rambits*16*1024 - 32768;
		/* Compute the top of Low memory */
		tolmk = pci_tolm >> 10;
		if (tolmk >= tomk) {
			/* The PCI hole does does not overlap the memory.
			 */
			tolmk = tomk;
		}

#if CONFIG_WRITE_HIGH_TABLES == 1
		high_tables_base = (tolmk - HIGH_TABLES_SIZE) * 1024;
		high_tables_size = HIGH_TABLES_SIZE* 1024;
		printk(BIOS_DEBUG, "tom: %lx, high_tables_base: %llx, high_tables_size: %llx\n", tomk*1024, high_tables_base, high_tables_size);
#endif

		/* Report the memory regions */
		idx = 10;
		ram_resource(dev, idx++, 0, 640);		/* first 640k */
		ram_resource(dev, idx++, 768, tolmk - 768);	/* leave a hole for vga */
	}
	assign_resources(&dev->link[0]);
}

static struct device_operations pci_domain_ops = {
        .read_resources   = pci_domain_read_resources,
        .set_resources    = pci_domain_set_resources,
        .enable_resources = enable_childrens_resources,
        .init             = 0,
        .scan_bus         = pci_domain_scan_bus,
};

static void cpu_bus_init(device_t dev)
{
        initialize_cpus(&dev->link[0]);
}

static void cpu_bus_noop(device_t dev)
{
}

static struct device_operations cpu_bus_ops = {
        .read_resources   = cpu_bus_noop,
        .set_resources    = cpu_bus_noop,
        .enable_resources = cpu_bus_noop,
        .init             = cpu_bus_init,
        .scan_bus         = 0,
};

static void enable_dev(struct device *dev)
{
	printk(BIOS_SPEW, "In vt8623 enable_dev for device %s.\n", dev_path(dev));

        /* Set the operations if it is a special bus type */
        if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
                dev->ops = &pci_domain_ops;
		pci_set_method(dev);
        }
        else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
                dev->ops = &cpu_bus_ops;
        }
}

struct chip_operations northbridge_via_vt8623_ops = {
	CHIP_NAME("VIA VT8623 Northbridge")
	.enable_dev = enable_dev,
};
