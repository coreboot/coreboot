#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <mem.h>
#include <part/sizeram.h>
#include <device/device.h>
#include <device/pci.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include "chip.h"
#if 0
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
#endif

#define BRIDGE_IO_MASK (IORESOURCE_IO | IORESOURCE_MEM | IORESOURCE_PREFETCH)

static void pci_domain_read_resources(device_t dev)
{
        struct resource *resource;
        unsigned reg;

        /* Initialize the system wide io space constraints */
        resource = new_resource(dev, 0);
        resource->base  = 0x400;
        resource->limit = 0xffffUL;
        resource->flags = IORESOURCE_IO;
        compute_allocate_resource(&dev->link[0], resource,
                IORESOURCE_IO, IORESOURCE_IO);

        /* Initialize the system wide prefetchable memory resources constraints */
        resource = new_resource(dev, 1);
        resource->limit = 0xfcffffffffULL;
        resource->flags = IORESOURCE_MEM | IORESOURCE_PREFETCH;
        compute_allocate_resource(&dev->link[0], resource,
                IORESOURCE_MEM | IORESOURCE_PREFETCH,
                IORESOURCE_MEM | IORESOURCE_PREFETCH);

        /* Initialize the system wide memory resources constraints */
        resource = new_resource(dev, 2);
        resource->limit = 0xfcffffffffULL;
        resource->flags = IORESOURCE_MEM;
        compute_allocate_resource(&dev->link[0], resource,
                IORESOURCE_MEM | IORESOURCE_PREFETCH,
                IORESOURCE_MEM);
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

static void pci_domain_set_resources(device_t dev)
{
        struct resource *io, *mem1, *mem2;
        struct resource *resource, *last;
        unsigned long mmio_basek;
        uint32_t pci_tolm;
        int idx;
	uint8_t  drb;
        unsigned basek, sizek;
	device_t dev_memctrl;

#if 0
        /* Place the IO devices somewhere safe */
        io = find_resource(dev, 0);
        io->base = DEVICE_IO_START;
#endif
#if 1
        /* Now reallocate the pci resources memory with the
         * highest addresses I can manage.
         */
        mem1 = find_resource(dev, 1);
        mem2 = find_resource(dev, 2);

#if 1
                printk_debug("base1: 0x%08Lx limit1: 0x%08Lx size: 0x%08Lx align: %d\n",
                        mem1->base, mem1->limit, mem1->size, mem1->align);
                printk_debug("base2: 0x%08Lx limit2: 0x%08Lx size: 0x%08Lx align: %d\n",
                        mem2->base, mem2->limit, mem2->size, mem2->align);
#endif

        /* See if both resources have roughly the same limits */
        if (((mem1->limit <= 0xffffffff) && (mem2->limit <= 0xffffffff)) ||
                ((mem1->limit > 0xffffffff) && (mem2->limit > 0xffffffff)))
        {
                /* If so place the one with the most stringent alignment first
                 */
                if (mem2->align > mem1->align) {
                        struct resource *tmp;
                        tmp = mem1;
                        mem1 = mem2;
                        mem2 = tmp;
                }
                /* Now place the memory as high up as it will go */
                mem2->base = resource_max(mem2);
                mem1->limit = mem2->base - 1;
                mem1->base = resource_max(mem1);
        }
        else {
                /* Place the resources as high up as they will go */
                mem2->base = resource_max(mem2);
                mem1->base = resource_max(mem1);
        }

#if 1
                printk_debug("base1: 0x%08Lx limit1: 0x%08Lx size: 0x%08Lx align: %d\n",
                        mem1->base, mem1->limit, mem1->size, mem1->align);
                printk_debug("base2: 0x%08Lx limit2: 0x%08Lx size: 0x%08Lx align: %d\n",
                        mem2->base, mem2->limit, mem2->size, mem2->align);
#endif
#endif
        pci_tolm = 0xffffffffUL;
        last = &dev->resource[dev->resources];
        for(resource = &dev->resource[0]; resource < last; resource++)
        {
#if 1
                resource->flags |= IORESOURCE_ASSIGNED;
                resource->flags &= ~IORESOURCE_STORED;
#endif
                compute_allocate_resource(&dev->link[0], resource,
                        BRIDGE_IO_MASK, resource->flags & BRIDGE_IO_MASK);

                resource->flags |= IORESOURCE_STORED;
                report_resource_stored(dev, resource, "");

                if ((resource->flags & IORESOURCE_MEM) &&
                        (pci_tolm > resource->base))
                {
                        pci_tolm = resource->base;
                }
        }

#warning "FIXME handle interleaved nodes"
        mmio_basek = pci_tolm >> 10;
        /* Round mmio_basek to something the processor can support */
        mmio_basek &= ~((1 << 6) -1);

#if 1
#warning "FIXME improve mtrr.c so we don't use up all of the mtrrs with a 64M MMIO hole"
        /* Round the mmio hold to 64M */
        mmio_basek &= ~((64*1024) - 1);
#endif

	dev_memctrl = dev_find_slot(0, 0); // d0f0
	drb = pci_read_config8(dev_memctrl, 0x67);

        idx = 10;

	basek = 0;
	sizek = 640;
	ram_resource(dev, idx++, basek, sizek);

	basek = 768;
	sizek = mmio_basek - basek;
	ram_resource(dev, idx++, basek, sizek);

	if ((drb << 16) > mmio_basek) {
                /* We don't need to consider the remap window
                 * here because we put it immediately after the
                 * rest of ram.
                 * All we must do is calculate the amount
                 * of unused memory and report it at 4GB.
                 */
                basek = 4096*1024;
                sizek = (drb << 16) - mmio_basek;
		ram_resource(dev, idx++, basek, sizek);
	}

        assign_resources(&dev->link[0]);
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

static unsigned int cpu_bus_scan(device_t dev, unsigned int max)
{
        struct bus *cpu_bus;
        unsigned reg;
        int i;

        /* Find which cpus are present */
        cpu_bus = &dev->link[0];
        for(i = 0; i < 7; i+=6) {
                device_t dev, cpu;
                struct device_path cpu_path;
#if 0
//How to identify Intel CPU
                /* Find the cpu's memory controller */
                dev = dev_find_slot(0, PCI_DEVFN(0x18 + i, 0));
#endif

                /* Build the cpu device path */
                cpu_path.type = DEVICE_PATH_APIC;
                cpu_path.u.apic.apic_id = i;

                /* See if I can find the cpu */
                cpu = find_dev_path(cpu_bus, &cpu_path);
#if 0
                /* Enable the cpu if I have the processor */
                if (dev && dev->enabled) {
                        if (!cpu) {
                                cpu = alloc_dev(cpu_bus, &cpu_path);
                        }
                        if (cpu) {
                                cpu->enabled = 1;
                        }
                }

                /* Disable the cpu if I don't have the processor */
                if (cpu && (!dev || !dev->enabled)) {
                        cpu->enabled = 0;
                }
#else
		cpu->enabled = 1;
#endif
                /* Report what I have done */
                if (cpu) {
                        printk_debug("CPU: %s %s\n",
                                dev_path(cpu), cpu->enabled?"enabled":"disabled");
                }
        }
        return max;
}

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
        .scan_bus         = cpu_bus_scan,
};

static void enable_dev(struct device *dev)
{
        struct device_path path;

        /* Set the operations if it is a special bus type */
        if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
                dev->ops = &pci_domain_ops;
        }
        else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
                dev->ops = &cpu_bus_ops;
        }
}
struct chip_operations northbridge_intel_e7501_ops = {
        .name      = "intel E7501 Northbridge",
	.enable_dev = enable_dev,
};
