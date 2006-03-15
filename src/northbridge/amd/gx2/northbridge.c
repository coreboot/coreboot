#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include "chip.h"
#include "northbridge.h"
#include <cpu/amd/gx2def.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/cache.h>

#define NORTHBRIDGE_FILE "northbridge.c"
/*
*/

/* todo: add a resource record. We don't do this here because this may be called when 
  * very little of the platform is actually working.
  */
int
sizeram(void)
{
	msr_t msr;
	int sizem;
	unsigned short dimm;

	msr = rdmsr(0x20000018);
	printk_debug("sizeram: %08x:%08x\n", msr.hi, msr.lo);

	/* dimm 0 */
	dimm = msr.hi;
	/* installed? */
	if ((dimm & 7) != 7)
		sizem = (1 << ((dimm >> 12)-1)) * 8;


	/* dimm 1*/
	dimm = msr.hi >> 16;
	/* installed? */
	if ((dimm & 7) != 7)
		sizem += (1 << ((dimm >> 12)-1)) * 8;

	printk_debug("sizeram: sizem 0x%x\n", sizem);
	return sizem;
}

#define CACHE_DISABLE (1<<0)
#define WRITE_ALLOCATE (1<<1)
#define WRITE_PROTECT (1<<2)
#define WRITE_THROUGH (1<<3)
#define WRITE_COMBINE (1<<4)
#define WRITE_SERIALIZE (1<<5)

/* ram has none of this stuff */
#define RAM_PROPERTIES (0)
#define DEVICE_PROPERTIES (WRITE_SERIALIZE|CACHE_DISABLE)
#define ROM_PROPERTIES (WRITE_SERIALIZE|WRITE_THROUGH|CACHE_DISABLE)

static void
setup_gx2_cache(int sizem)
{
	msr_t msr;
	unsigned long long val;
	printk_debug("enable_cache: enable for %dm bytes\n", sizem);
	/* build up the rconf word. */
	/* the SYSTOP bits 27:8 are actually the top bits from 31:12. Book fails to say that */
	/* set romrp */
	val = ((unsigned long long) ROM_PROPERTIES) << 56;
	/* make rom base useful for 1M roms */
	/* fuctory sets this to a weird value, just go with it. */
	val |= ((unsigned long long) 0xff800)<<36;
	/* set the devrp properties */
	val |= ((unsigned long long) DEVICE_PROPERTIES) << 28;
	/* sigh. Take our TOM, RIGHT shift 12, since it page-aligned, then LEFT-shift 8 for reg. */
	/* yank off 8M for frame buffer and 1M for VSA */
	sizem -= 9;
	sizem *= 0x100000;
	sizem >>= 12;
	sizem <<= 8;
	val |= sizem;
	val |= RAM_PROPERTIES;
	msr.lo = val;
	msr.hi = (val >> 32);
	printk_debug("msr will be set to %x:%x\n", msr.hi, msr.lo);
	wrmsr(0x1808, msr);
	
	enable_cache();
	wbinvd();
}
/* we have to do this here. We have not found a nicer way to do it */
void
setup_gx2(void)
{
	int sizem;
	sizem = sizeram();
	
	setup_gx2_cache(sizem);
}


static void optimize_xbus(device_t dev)
{
	/* Optimise X-Bus performance */
	pci_write_config8(dev, 0x40, 0x1e);
	pci_write_config8(dev, 0x41, 0x52);
	pci_write_config8(dev, 0x43, 0xc1);
	pci_write_config8(dev, 0x44, 0x00);
}

static void enable_shadow(device_t dev)
{
	
}

static void northbridge_init(device_t dev) 
{
	printk_debug("northbridge: %s()\n", __FUNCTION__);
	
	optimize_xbus(dev);
	enable_shadow(dev);
}


static struct device_operations northbridge_operations = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = northbridge_init,
	.enable           = 0,
	.ops_pci          = 0,
};

static struct pci_driver northbridge_driver __pci_driver = {
	.ops = &northbridge_operations,
	.vendor = PCI_VENDOR_ID_CYRIX,
	.device = PCI_DEVICE_ID_CYRIX_PCI_MASTER, 
};



#define BRIDGE_IO_MASK (IORESOURCE_IO | IORESOURCE_MEM)

static void pci_domain_read_resources(device_t dev)
{
        struct resource *resource;

	printk_spew("%s:%s()\n", NORTHBRIDGE_FILE, __FUNCTION__);

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

#define FRAMEBUFFERK 4096

static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;
        uint32_t pci_tolm;
#if 0
        pci_tolm = find_pci_tolm(&dev->link[0]);
	mc_dev = dev->link[0].children;
	if (mc_dev) {
		unsigned int tomk, tolmk;
		unsigned int ramreg = 0;
		int i, idx;
		unsigned int *bcdramtop = (unsigned int *)(GX_BASE + BC_DRAM_TOP);
		unsigned int *mcgbaseadd = (unsigned int *)(GX_BASE + MC_GBASE_ADD);

		for(i=0; i<0x20; i+= 0x10) {
			unsigned int *mcreg = (unsigned int *)(GX_BASE + MC_BANK_CFG);
			unsigned int mem_config = *mcreg;

			if (((mem_config & (DIMM_PG_SZ << i)) >> (4 + i)) == 7)
				continue;
			ramreg += 1 << (((mem_config & (DIMM_SZ << i)) >> (i + 8)) + 2);
		}
			
		tomk = ramreg << 10;

		/* Sort out the framebuffer size */
		tomk -= FRAMEBUFFERK;
		*bcdramtop = ((tomk << 10) - 1);
		*mcgbaseadd = (tomk >> 9);

		printk_debug("BC_DRAM_TOP = 0x%08x\n", *bcdramtop);
		printk_debug("MC_GBASE_ADD = 0x%08x\n", *mcgbaseadd);

		printk_debug("I would set ram size to %d Mbytes\n", (tomk >> 10));

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
#endif
}

static unsigned int pci_domain_scan_bus(device_t dev, unsigned int max)
{
        max = pci_scan_bus(&dev->link[0], PCI_DEVFN(0, 0), 0xff, max);
        return max;
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
	printk_debug("gx2 north: enable_dev\n");
        /* Set the operations if it is a special bus type */
        if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
		printk_debug("DEVICE_PATH_PCI_DOMAIN\n");
		setup_gx2();
		dev->ops = &pci_domain_ops;
		pci_set_method(dev);
        }
        else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
		printk_debug("DEVICE_PATH_APIC_CLUSTER\n");
                dev->ops = &cpu_bus_ops;
        }
}

struct chip_operations northbridge_amd_gx2_ops = {
	CHIP_NAME("AMD GX2 Northbridge")
	.enable_dev = enable_dev, 
};
