#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/hypertransport.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include "chip.h"


/* hack for now */
void sc520_udelay(int microseconds) {
        volatile int x;
        for(x = 0; x < 1000; x++) 
                ;  
}

/* looks like we define this now */
void
udelay(int microseconds) {
        sc520_udelay(microseconds); 
}
/*
 * set up basic things ... PAR should NOT go here, as it might change with the mainboard. 
 */
static void cpu_init(device_t dev) 
{
  unsigned long *l = (unsigned long *) 0xfffef088;
  int i;
  for(i = 0; i < 16; i++, l++)
    printk_err("Par%d: 0x%lx\n", i, *l);

  printk_spew("SC520 random fixup ...\n");
}


/* Ollie says: make a northbridge/amd/sc520. Ron sez: 
 * there is no real northbridge, keep it here in cpu. 
 * Ron wins, he's writing the code. 
 */
void sc520_enable_resources(struct device *dev) {
	unsigned char command;

	printk_spew("%s\n", __FUNCTION__);
        command = pci_read_config8(dev, PCI_COMMAND);
        printk_spew("========>%s, command 0x%x\n", __FUNCTION__, command);
        command |= PCI_COMMAND_MEMORY | PCI_COMMAND_PARITY | PCI_COMMAND_SERR;
        printk_spew("========>%s, command 0x%x\n", __FUNCTION__, command);
        pci_write_config8(dev, PCI_COMMAND, command);
        command = pci_read_config8(dev, PCI_COMMAND);
        printk_spew("========>%s, command 0x%x\n", __FUNCTION__, command);
/*
 */

}


static struct device_operations cpu_operations = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = sc520_enable_resources,
	.init             = cpu_init,
	.enable           = 0,
	.ops_pci          = 0,
};

static struct pci_driver cpu_driver __pci_driver = {
	.ops = &cpu_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x3000
};



#define BRIDGE_IO_MASK (IORESOURCE_IO | IORESOURCE_MEM)

static void pci_domain_read_resources(device_t dev)
{
        struct resource *resource;
  printk_spew("%s\n", __FUNCTION__);
        /* Initialize the system wide io space constraints */
        resource = new_resource(dev, IOINDEX_SUBTRACTIVE(0,0));
        resource->limit = 0xffffUL;
        resource->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

        /* Initialize the system wide memory resources constraints */
        resource = new_resource(dev, IOINDEX_SUBTRACTIVE(1,0));
        resource->limit = 0xffffffffULL;
        resource->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
}

static void ram_resource(device_t dev, unsigned long index,
        unsigned long basek, unsigned long sizek)
{
        struct resource *resource;
  printk_spew("%s sizek 0x%x\n", __FUNCTION__, sizek);
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
  printk_spew("%s\n", __FUNCTION__);
	min = 0;
	search_bus_resources(bus, IORESOURCE_MEM, IORESOURCE_MEM, tolm_test, &min);
	tolm = 0xffffffffUL;
	if (min && tolm > min->base) {
		tolm = min->base;
	}
  printk_spew("%s returns 0x%x\n", __FUNCTION__, tolm);
	return tolm;
}

static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;
        uint32_t pci_tolm;
  printk_spew("%s\n", __FUNCTION__);
        pci_tolm = find_pci_tolm(&dev->link[0]);
	mc_dev = dev->link[0].children;
	if (mc_dev) {
		unsigned long tomk, tolmk;
		//		unsigned char rambits;
		// int i;
		int idx;
#if 0
		for(rambits = 0, i = 0; i < sizeof(ramregs)/sizeof(ramregs[0]); i++) {
			unsigned char reg;
			reg = pci_read_config8(mc_dev, ramregs[i]);
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
		tomk = rambits*8*1024;
#endif
		tomk = 32 * 1024;
		/* Compute the top of Low memory */
		tolmk = pci_tolm >> 10;
		if (tolmk >= tomk) {
			/* The PCI hole does does not overlap the memory.
			 */
			tolmk = tomk;
		}
		/* Report the memory regions */
		idx = 10;
		ram_resource(dev, idx++, 0, tolmk);
	}
	assign_resources(&dev->link[0]);
}

static unsigned int pci_domain_scan_bus(device_t dev, unsigned int max)
{
  printk_spew("%s\n", __FUNCTION__);
        max = pci_scan_bus(&dev->link[0], PCI_DEVFN(0, 0), 0xff, max);
        return max;
}


#if 0
void sc520_enable_resources(device_t dev) {

	printk_spew("%s\n", __FUNCTION__);
	printk_spew("THIS IS FOR THE SC520 =============================\n");

/*
	command = pci_read_config8(dev, PCI_COMMAND);
	printk_spew("%s, command 0x%x\n", __FUNCTION__, command);
	command |= PCI_COMMAND_MEMORY;
	printk_spew("%s, command 0x%x\n", __FUNCTION__, command);
	pci_write_config8(dev, PCI_COMMAND, command);
	command = pci_read_config8(dev, PCI_COMMAND);
	printk_spew("%s, command 0x%x\n", __FUNCTION__, command);
 */
	enable_childrens_resources(dev);
	printk_spew("%s\n", __FUNCTION__);
}
#endif

static struct device_operations pci_domain_ops = {
        .read_resources   = pci_domain_read_resources,
        .set_resources    = pci_domain_set_resources,
        .enable_resources = enable_resources,
        .init             = 0,
        .scan_bus         = pci_domain_scan_bus,
};  

static void cpu_bus_init(device_t dev)
{
  printk_spew("cpu_bus_init\n");
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
  printk_spew("%s\n", __FUNCTION__);
        /* Set the operations if it is a special bus type */
        if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
                dev->ops = &pci_domain_ops;
		pci_set_method(dev);
        }
        else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
                dev->ops = &cpu_bus_ops;
        }
}


struct chip_operations cpu_amd_sc520_ops = {
	CHIP_NAME("AMD SC520")
	.enable_dev = enable_dev, 
};
