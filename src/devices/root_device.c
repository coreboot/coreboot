#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>

/** 
 * Read the resources for the root device,
 * that encompass the resources for the entire system.
 * @param root Pointer to the device structure for the system root device
 */
void root_dev_read_resources(device_t root)
{
	int res = 0;

	printk_spew("%s . Root is %p\n", __FUNCTION__, root);
	/* Initialize the system wide io space constraints */
	root->resource[res].base  = 0x400;
	root->resource[res].size  = 0;
	root->resource[res].align = 0;
	root->resource[res].gran  = 0;
	root->resource[res].limit = 0xffffUL;
	root->resource[res].flags = IORESOURCE_IO;
	root->resource[res].index = 0;
	printk_spew("%s . link %p, resource %p\n", __FUNCTION__, 
		    &root->link[0], &root->resource[res]);
	compute_allocate_resource(&root->link[0], &root->resource[res], 
				  IORESOURCE_IO, IORESOURCE_IO);
	res++;

	/* Initialize the system wide memory resources constraints */
	root->resource[res].base  = 0;
	root->resource[res].size  = 0;
	root->resource[res].align = 0;
	root->resource[res].gran  = 0;
	root->resource[res].limit = 0xffffffffUL;
	root->resource[res].flags = IORESOURCE_MEM;
	root->resource[res].index = 1;
	printk_spew("%s . link %p, resource %p\n", __FUNCTION__, 
		    &root->link[0], &root->resource[res]);
	compute_allocate_resource(&root->link[0], &root->resource[res], 
				  IORESOURCE_MEM, IORESOURCE_MEM);
	res++;

	root->resources = res;
	printk_spew("%s DONE\n", __FUNCTION__);
}

/**
 * Write the resources for the root device,
 * and every device under it which are all of the devices.
 * @param root Pointer to the device structure for the system root device
 */
void root_dev_set_resources(device_t root)
{
	struct bus *bus = &root->link[0];

	compute_allocate_resource(bus, &root->resource[0],
				  IORESOURCE_IO, IORESOURCE_IO);
	compute_allocate_resource(bus, &root->resource[1],
				  IORESOURCE_MEM, IORESOURCE_MEM);
	assign_resources(bus);
}

/**
 * @brief Scan devices on static buses.
 *
 * The existence of devices on certain buses can be completely determined at
 * compile time by the config file. Typical expamles are the 'PNP' devices
 * on an legacy ISA/LPC bus. There is no need of probing of any kind, the
 * only thing we have to do is to walk through the bus and enable or disable
 * devices as indicated in the config file.
 *
 * This function is the default scan_bus() method for LPC bridges.
 *
 * @param root Pointer to the device structure for the system root device
 * @param max  Maximum bus number allowed in the system.
 * @return Largest bus number used.
 */
unsigned int scan_static_bus(device_t bus, unsigned int max)
{
	device_t child;
	unsigned link;

	printk_debug("%s entered\n", __FUNCTION__);

	for (link = 0; link < bus->links; link++) {
		for (child = bus->link[link].children; child; child = child->sibling) {
			if (child->ops && child->ops->enable) {
				child->ops->enable(child);
			}
			printk_debug("%s %s\n", dev_path(child),
				     child->enabled?"enabled": "disabled");
		}
	}
	for (link = 0; link < bus->links; link++) {
		for (child = bus->link[link].children; child; child = child->sibling) {
			if (!child->ops || !child->ops->scan_bus)
				continue;
			printk_debug("%s scanning...\n", dev_path(child));
			max = child->ops->scan_bus(child, max);
		}
	}

	printk_debug("%s done\n", __FUNCTION__);

	return max;
}

/**
 * @brief Enable resources for children devices
 *
 * @param dev the device whos children's resources are to be enabled
 *
 * This function is call by the enable_resource()
 */
void enable_childrens_resources(device_t dev)
{
	unsigned link;
	for (link = 0; link < dev->links; link++) {
		device_t child;
		for (child = dev->link[link].children; child; child = child->sibling) {
			enable_resources(child);
		}
	}
}

/**
 * @brief Scan root bus for generic PCI systems
 *
 * @param root The root device structure
 * @param max The current bus number scanned so fat, usually 0x00
 *
 * This function is the default scan_bus() method of the dynamic root device.
 * The bus heirachy is rooted at the host/northbridge. The northbridge of a
 * generic PCI bus system is at Bus 0, Dev 0, Fun 0 so we scan the whole PCI
 * buses from there. 
 */
unsigned int root_dev_scan_pci_bus(device_t root, unsigned int max)
{
	return pci_scan_bus(&root->link[0], 0, 0xff, max);
}

/**
 * @brief Default device operation for root device
 *
 * This is the default device operation for root devices in PCI based systems.
 * The static enumeration code chip_control::enumerate() of mainboards usually
 * override this operation with their own device operations. An notable example
 * is mainboard operations for AMD K8 mainboards. They replace the scan_bus()
 * method with amdk8_scan_root_bus() due to the special device layout of AMD K8
 * systems.
 */
struct device_operations default_dev_ops_root = {
	.read_resources   = root_dev_read_resources,
	.set_resources    = root_dev_set_resources,
	.enable_resources = enable_childrens_resources,
	.init             = 0,
	.scan_bus         = root_dev_scan_pci_bus,
};

/**
 * @brief The root of dynamic device tree.
 *
 * This is the root of the dynamic device tree. A PCI tree always has 
 * one bus, bus 0. Bus 0 contains devices and bridges. 
 */
struct device dev_root = {
	.ops = &default_dev_ops_root,
	.bus = &dev_root.link[0],
	.path = { .type = DEVICE_PATH_ROOT },
	.enabled = 1,
	.links   = 1,
	.link    = {
		 [0] = {
			 .dev = &dev_root,
			 .link = 0,
		 },
	 },
};
