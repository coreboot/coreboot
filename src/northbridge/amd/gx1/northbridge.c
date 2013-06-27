#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/amd/gx1def.h>
#include <cpu/x86/cache.h>
#include <cpu/cpu.h>

#define NORTHBRIDGE_FILE "northbridge.c"
/*
*/

static void optimize_xbus(device_t dev)
{
	/* Optimise X-Bus performance */
	pci_write_config8(dev, 0x40, 0x1e);
	pci_write_config8(dev, 0x41, 0x52);
	pci_write_config8(dev, 0x43, 0xc1);
	pci_write_config8(dev, 0x44, 0x00);
}

/**
 * Enables memory from 0xC0000 up to 0xFFFFF.
 * So this region is read/write and cache able
 *
 * FIXME: What about PCI master access into
 *        this region?
 **/

static void enable_shadow(device_t dev)
{
       write32(GX_BASE+BC_XMAP_2, 0x77777777);
       write32(GX_BASE+BC_XMAP_3, 0x77777777);
}

static void northbridge_init(device_t dev)
{
	printk(BIOS_DEBUG, "northbridge: %s()\n", __func__);

	optimize_xbus(dev);
	enable_shadow(dev);
	printk(BIOS_SPEW, "Calling enable_cache()\n");
	enable_cache();
}


static struct device_operations northbridge_operations = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = northbridge_init,
	.enable           = 0,
	.ops_pci          = 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops = &northbridge_operations,
	.vendor = PCI_VENDOR_ID_CYRIX,
	.device = PCI_DEVICE_ID_CYRIX_PCI_MASTER,
};

#include <cbmem.h>

static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;
        uint32_t pci_tolm;

        pci_tolm = find_pci_tolm(dev->link_list);
	mc_dev = dev->link_list->children;
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
		tomk -= CONFIG_VIDEO_MB * 1024;
		*bcdramtop = ((tomk << 10) - 1);
		*mcgbaseadd = (tomk >> 9);

		printk(BIOS_DEBUG, "BC_DRAM_TOP = 0x%08x\n", *bcdramtop);
		printk(BIOS_DEBUG, "MC_GBASE_ADD = 0x%08x\n", *mcgbaseadd);

		printk(BIOS_DEBUG, "I would set ram size to %d Mbytes\n", (tomk >> 10));

		/* Compute the top of Low memory */
		tolmk = pci_tolm >> 10;
		if (tolmk >= tomk) {
			/* The PCI hole does does not overlap the memory.
			 */
			tolmk = tomk;
		}

		set_top_of_ram(tolmk * 1024);

		/* Report the memory regions */
		idx = 10;
		ram_resource(dev, idx++, 0, tolmk);
	}
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
        .read_resources   = pci_domain_read_resources,
        .set_resources    = pci_domain_set_resources,
        .enable_resources = NULL,
        .init             = NULL,
        .scan_bus         = pci_domain_scan_bus,
        .ops_pci_bus      = pci_bus_default_ops,
};

static void cpu_bus_init(device_t dev)
{
	printk(BIOS_SPEW, "%s:%s()\n", NORTHBRIDGE_FILE, __func__);
	initialize_cpus(dev->link_list);
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
        printk(BIOS_SPEW, "%s:%s()\n", NORTHBRIDGE_FILE, __func__);
        /* Set the operations if it is a special bus type */
        if (dev->path.type == DEVICE_PATH_DOMAIN) {
        	printk(BIOS_SPEW, "DEVICE_PATH_DOMAIN\n");
                dev->ops = &pci_domain_ops;
        }
        else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
        	printk(BIOS_SPEW, "DEVICE_PATH_CPU_CLUSTER\n");
                dev->ops = &cpu_bus_ops;
        } else {
        	printk(BIOS_SPEW, "device path type %d\n",dev->path.type);
	}
}

struct chip_operations northbridge_amd_gx1_ops = {
	CHIP_NAME("AMD GX1 Northbridge")
	.enable_dev = enable_dev,
};
