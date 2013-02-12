#include <console/console.h>
#include <device/device.h>

static void my_pci_domain_read_resources(device_t dev)
{
	printk(BIOS_DEBUG, "%s: ENTERED!!\n", __func__);
	ram_resource(dev, 0,
	(CONFIG_SYS_SDRAM_BASE >> 10) + (CONFIG_COREBOOT_ROMSIZE_KB),
	((CONFIG_DRAM_SIZE_MB << 10UL) * CONFIG_NR_DRAM_BANKS) - CONFIG_COREBOOT_ROMSIZE_KB
/*	- (CONFIG_COREBOOT_TABLES_SIZE >> 10) */);
}

static void my_pci_domain_set_resources(device_t dev)
{
	printk(BIOS_DEBUG, "%s: ENTERED!!\n", __func__);
	assign_resources(dev->link_list);
}

static unsigned int my_pci_domain_scan_bus(device_t dev, unsigned int max)
{
	//max = pci_scan_bus(dev->link_list, PCI_DEVFN(0, 0), 0xff, max);
	return max;
}


static struct device_operations pci_domain_ops = {
	.read_resources   = my_pci_domain_read_resources,
	.set_resources    = my_pci_domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = my_pci_domain_scan_bus,
};

static void cpu_bus_init(device_t dev)
{
	printk(BIOS_DEBUG,"INITIALIZE CPUS!\n");
	//initialize_cpus(dev->link_list);
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

static void enable_dev(device_t dev)
{
	printk (BIOS_DEBUG, "OUR CPU CHIP OPS!!\n");
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations cpu_samsung_exynos5250_ops = {
	CHIP_NAME("CPU Samsung Exynos 5250")
	.enable_dev = enable_dev,
};
