/*
 * This file needs a major cleanup. Too much #if 0 code
 */

#include <console/console.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/hypertransport.h>
#include <stdlib.h>
#include <string.h>
#include <delay.h>

/*
 * set up basic things ...
 * PAR should NOT go here, as it might change with the mainboard.
 */
static void cpu_init(device_t dev)
{
  unsigned long *l = (unsigned long *) 0xfffef088;
  int i;
  for(i = 0; i < 16; i++, l++)
    printk(BIOS_ERR, "Par%d: 0x%lx\n", i, *l);

  printk(BIOS_SPEW, "SC520 random fixup ...\n");
}


/* Ollie says: make a northbridge/amd/sc520. Ron sez:
 * there is no real northbridge, keep it here in cpu.
 * Ron wins, he's writing the code.
 */
static void sc520_enable_resources(struct device *dev) {
	unsigned char command;

	printk(BIOS_SPEW, "%s\n", __func__);
        command = pci_read_config8(dev, PCI_COMMAND);
        printk(BIOS_SPEW, "========>%s, command 0x%x\n", __func__, command);
        command |= PCI_COMMAND_MEMORY | PCI_COMMAND_PARITY | PCI_COMMAND_SERR;
        printk(BIOS_SPEW, "========>%s, command 0x%x\n", __func__, command);
        pci_write_config8(dev, PCI_COMMAND, command);
        command = pci_read_config8(dev, PCI_COMMAND);
        printk(BIOS_SPEW, "========>%s, command 0x%x\n", __func__, command);
/*
 */

}

static void sc520_read_resources(device_t dev)
{
	struct resource* res;

	pci_dev_read_resources(dev);

	res = new_resource(dev, 1);
	res->base = 0x0UL;
	res->size = 0x400UL;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}


static struct device_operations cpu_operations = {
	.read_resources   = sc520_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = sc520_enable_resources,
	.init             = cpu_init,
	.enable           = 0,
	.ops_pci          = 0,
};

static const struct pci_driver cpu_driver __pci_driver = {
	.ops = &cpu_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x3000
};

static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;
        uint32_t pci_tolm;
  printk(BIOS_SPEW, "%s\n", __func__);
        pci_tolm = find_pci_tolm(dev->link_list);
	mc_dev = dev->link_list->children;
	if (mc_dev) {
		unsigned long tomk, tolmk;
		//		unsigned char rambits;
		// int i;
		int idx;
#if 0
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
		printk(BIOS_DEBUG, "I would set ram size to 0x%x Kbytes\n", (rambits)*8*1024);
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
	assign_resources(dev->link_list);
}

#if 0
void sc520_enable_resources(device_t dev) {

	printk(BIOS_SPEW, "%s\n", __func__);
	printk(BIOS_SPEW, "THIS IS FOR THE SC520 =============================\n");

/*
	command = pci_read_config8(dev, PCI_COMMAND);
	printk(BIOS_SPEW, "%s, command 0x%x\n", __func__, command);
	command |= PCI_COMMAND_MEMORY;
	printk(BIOS_SPEW, "%s, command 0x%x\n", __func__, command);
	pci_write_config8(dev, PCI_COMMAND, command);
	command = pci_read_config8(dev, PCI_COMMAND);
	printk(BIOS_SPEW, "%s, command 0x%x\n", __func__, command);
 */
	enable_childrens_resources(dev);
	printk(BIOS_SPEW, "%s\n", __func__);
}
#endif

static struct device_operations pci_domain_ops = {
        .read_resources   = pci_domain_read_resources,
        .set_resources    = pci_domain_set_resources,
	/*
	 * If enable_resources is set to the generic enable_resources
	 * function the whole thing will hang in an endless loop on
	 * the ts5300. If this is really needed on another platform,
	 * something is conceptually wrong.
	 */
        .enable_resources = 0, //enable_resources,
        .init             = 0,
        .scan_bus         = pci_domain_scan_bus,
        .ops_pci_bus      = pci_bus_default_ops,
};

#if 0
static void cpu_bus_init(device_t dev)
{
  printk(BIOS_SPEW, "cpu_bus_init\n");
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
#endif

static void enable_dev(struct device *dev)
{
  printk(BIOS_SPEW, "%s\n", __func__);
        /* Set the operations if it is a special bus type */
        if (dev->path.type == DEVICE_PATH_DOMAIN) {
                dev->ops = &pci_domain_ops;
        }
#if 0
	/* This is never hit as none of the sc520 boards have
	 * an APIC cluster defined
	 */
        else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
                dev->ops = &cpu_bus_ops;
        }
#endif
}


struct chip_operations cpu_amd_sc520_ops = {
	CHIP_NAME("AMD Elan SC520 CPU")
	.enable_dev = enable_dev,
};
