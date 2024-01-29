/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Originally based on the Linux kernel (arch/i386/kernel/pci-pc.c).
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <post.h>
#include <stdlib.h>
#include <string.h>
#include <smp/spinlock.h>
#include <timer.h>

/** Pointer to the last device */
extern struct device *last_dev;
/** Linked list of free resources */
struct resource *free_resources = NULL;
/* Disable a PCI device based on bus, device and function. */
void devfn_disable(const struct bus *bus, unsigned int devfn)
{
	struct device *dev = pcidev_path_behind(bus, devfn);
	if (dev)
		dev->enabled = 0;
}

/**
 * Initialize all chips of statically known devices.
 *
 * Will be called before bus enumeration to initialize chips stated in the
 * device tree.
 */
void dev_initialize_chips(void)
{
	const struct device *dev;

	for (dev = all_devices; dev; dev = dev->next) {
		/* Initialize chip if we haven't yet. */
		if (dev->chip_ops && dev->chip_ops->init &&
				!dev->chip_ops->initialized) {
			post_log_path(dev);
			dev->chip_ops->init(dev->chip_info);
			dev->chip_ops->initialized = 1;
		}
	}
	post_log_clear();
}

/**
 * Finalize all chips of statically known devices.
 *
 * This is the last call before calling the payload. This is a good place
 * to lock registers or other final cleanup.
 */
void dev_finalize_chips(void)
{
	const struct device *dev;

	for (dev = all_devices; dev; dev = dev->next) {
		/* Initialize chip if we haven't yet. */
		if (dev->chip_ops && dev->chip_ops->final &&
				!dev->chip_ops->finalized) {
			dev->chip_ops->final(dev->chip_info);
			dev->chip_ops->finalized = 1;
		}
	}
}

DECLARE_SPIN_LOCK(dev_lock)

/**
 * Allocate a new device structure.
 *
 * Allocate a new device structure and attach it to the device tree as a
 * child of the parent bus.
 *
 * @param parent Parent bus the newly created device should be attached to.
 * @param path Path to the device to be created.
 * @return Pointer to the newly created device structure.
 *
 * @see device_path
 */
static struct device *__alloc_dev(struct bus *parent, struct device_path *path)
{
	struct device *dev, *child;

	/* Find the last child of our parent. */
	for (child = parent->children; child && child->sibling; /* */)
		child = child->sibling;

	dev = malloc(sizeof(*dev));
	if (dev == 0)
		die("alloc_dev(): out of memory.\n");

	memset(dev, 0, sizeof(*dev));
	memcpy(&dev->path, path, sizeof(*path));

	/* By default devices are enabled. */
	dev->enabled = 1;

	/* Add the new device to the list of children of the bus. */
	dev->upstream = parent;
	if (child)
		child->sibling = dev;
	else
		parent->children = dev;

	/* Append a new device to the global device list.
	 * The list is used to find devices once everything is set up.
	 */
	last_dev->next = dev;
	last_dev = dev;

	return dev;
}

struct device *alloc_dev(struct bus *parent, struct device_path *path)
{
	struct device *dev;
	spin_lock(&dev_lock);
	dev = __alloc_dev(parent, path);
	spin_unlock(&dev_lock);
	return dev;
}

DECLARE_SPIN_LOCK(bus_lock)

/**
 * Allocate a new bus structure
 *
 * Allocate a new downstream bus structure below a device and attach it
 * to the device tree if the device doesn't already have a downstream bus.
 *
 * @param parent Parent device the to-be-created bus should be attached to.
 * @return Pointer to the newly created bus structure or the existing bus.
 *
 */
static struct bus *__alloc_bus(struct device *parent)
{
	if (parent->downstream)
		return parent->downstream;

	struct bus *bus = calloc(1, sizeof(struct bus));
	if (!bus)
		die("Couldn't allocate downstream bus!\n");
	parent->downstream = bus;
	bus->dev = parent;

	return bus;
}

struct bus *alloc_bus(struct device *parent)
{
	struct bus *bus;
	spin_lock(&bus_lock);
	bus = __alloc_bus(parent);
	spin_unlock(&bus_lock);
	return bus;
}

/**
 * See if a device structure already exists and if not allocate it.
 *
 * @param parent The bus to find the device on.
 * @param path The relative path from the bus to the appropriate device.
 * @return Pointer to a device structure for the device on bus at path.
 */
struct device *alloc_find_dev(struct bus *parent, struct device_path *path)
{
	struct device *child;
	spin_lock(&dev_lock);
	child = find_dev_path(parent, path);
	if (!child)
		child = __alloc_dev(parent, path);
	spin_unlock(&dev_lock);
	return child;
}

/**
 * Read the resources on all devices of a given bus.
 *
 * @param bus Bus to read the resources on.
 */
static void read_resources(struct bus *bus)
{
	struct device *curdev;

	printk(BIOS_SPEW, "%s %s segment group %d bus %d\n", dev_path(bus->dev),
	       __func__, bus->segment_group, bus->secondary);

	/* Walk through all devices and find which resources they need. */
	for (curdev = bus->children; curdev; curdev = curdev->sibling) {
		if (!curdev->enabled)
			continue;

		if (!curdev->ops || !curdev->ops->read_resources) {
			if (curdev->path.type != DEVICE_PATH_APIC)
				printk(BIOS_ERR, "%s missing %s\n",
				       dev_path(curdev), __func__);
			continue;
		}
		post_log_path(curdev);
		curdev->ops->read_resources(curdev);

		/* Read in the resources behind the current device's links. */
		if (curdev->downstream)
			read_resources(curdev->downstream);
	}
	post_log_clear();
	printk(BIOS_SPEW, "%s %s segment group %d bus %d done\n",
	       dev_path(bus->dev), __func__, bus->segment_group, bus->secondary);
}

struct device *vga_pri = NULL;
static void set_vga_bridge_bits(void)
{
	/*
	 * FIXME: Modify set_vga_bridge() so it is less PCI-centric!
	 * This function knows too much about PCI stuff, it should be just
	 * an iterator/visitor.
	 */

	/* FIXME: Handle the VGA palette snooping. */
	struct device *dev, *vga, *vga_onboard;
	struct bus *bus;

	bus = 0;
	vga = 0;
	vga_onboard = 0;

	dev = NULL;
	while ((dev = dev_find_class(PCI_CLASS_DISPLAY_VGA << 8, dev))) {
		if (!dev->enabled)
			continue;

		printk(BIOS_DEBUG, "found VGA at %s\n", dev_path(dev));
		if (dev->upstream->no_vga16) {
			printk(BIOS_WARNING,
				"A bridge on the path doesn't support 16-bit VGA decoding!");
		}

		if (dev->on_mainboard)
			vga_onboard = dev;
		else
			vga = dev;

		/* It isn't safe to enable all VGA cards. */
		dev->command &= ~(PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
	}

	if (!vga)
		vga = vga_onboard;

	if (CONFIG(ONBOARD_VGA_IS_PRIMARY) && vga_onboard)
		vga = vga_onboard;

	/* If we prefer plugin VGA over chipset VGA, the chipset might
	   want to know. */
	if (!CONFIG(ONBOARD_VGA_IS_PRIMARY) && (vga != vga_onboard) &&
		vga_onboard && vga_onboard->ops && vga_onboard->ops->vga_disable) {
		printk(BIOS_DEBUG, "Use plugin graphics over integrated.\n");
		vga_onboard->ops->vga_disable(vga_onboard);
	}

	if (vga) {
		/* VGA is first add-on card or the only onboard VGA. */
		printk(BIOS_DEBUG, "Setting up VGA for %s\n", dev_path(vga));
		/* All legacy VGA cards have MEM & I/O space registers. */
		vga->command |= (PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		vga_pri = vga;
		bus = vga->upstream;
	}

	/* Now walk up the bridges setting the VGA enable. */
	while (bus) {
		printk(BIOS_DEBUG, "Setting PCI_BRIDGE_CTL_VGA for bridge %s\n",
		       dev_path(bus->dev));
		bus->bridge_ctrl |= PCI_BRIDGE_CTL_VGA | PCI_BRIDGE_CTL_VGA16;
		bus = (bus == bus->dev->upstream) ? 0 : bus->dev->upstream;
	}
}

/**
 * Assign the computed resources to the devices on the bus.
 *
 * Use the device specific set_resources() method to store the computed
 * resources to hardware. For bridge devices, the set_resources() method
 * has to recurse into every down stream buses.
 *
 * Mutual recursion:
 *	assign_resources() -> device_operation::set_resources()
 *	device_operation::set_resources() -> assign_resources()
 *
 * @param bus Pointer to the structure for this bus.
 */
void assign_resources(struct bus *bus)
{
	struct device *curdev;

	printk(BIOS_SPEW, "%s %s, segment group %d bus %d\n",
	       dev_path(bus->dev), __func__, bus->segment_group, bus->secondary);

	for (curdev = bus->children; curdev; curdev = curdev->sibling) {
		if (!curdev->enabled || !curdev->resource_list)
			continue;

		if (!curdev->ops || !curdev->ops->set_resources) {
			printk(BIOS_ERR, "%s missing set_resources\n",
			       dev_path(curdev));
			continue;
		}
		post_log_path(curdev);
		curdev->ops->set_resources(curdev);
	}
	post_log_clear();
	printk(BIOS_SPEW, "%s %s, segment group %d bus %d done\n",
	       dev_path(bus->dev), __func__, bus->segment_group, bus->secondary);
}

/**
 * Enable the resources for devices on a link.
 *
 * Enable resources of the device by calling the device specific
 * enable_resources() method.
 *
 * The parent's resources should be enabled first to avoid having enabling
 * order problem. This is done by calling the parent's enable_resources()
 * method before its children's enable_resources() methods.
 *
 * @param link The link whose devices' resources are to be enabled.
 */
static void enable_resources(struct bus *link)
{
	struct device *dev;

	for (dev = link->children; dev; dev = dev->sibling) {
		if (dev->enabled && dev->ops && dev->ops->enable_resources) {
			post_log_path(dev);
			dev->ops->enable_resources(dev);
		}
	}

	for (dev = link->children; dev; dev = dev->sibling) {
		if (dev->downstream)
			enable_resources(dev->downstream);
	}
	post_log_clear();
}

/**
 * Reset all of the devices on a bus and clear the bus's reset_needed flag.
 *
 * @param bus Pointer to the bus structure.
 * @return 1 if the bus was successfully reset, 0 otherwise.
 */
int reset_bus(struct bus *bus)
{
	if (bus && bus->dev && bus->dev->ops && bus->dev->ops->reset_bus) {
		bus->dev->ops->reset_bus(bus);
		bus->reset_needed = 0;
		return 1;
	}
	return 0;
}

/**
 * Scan for devices on a bus.
 *
 * If there are bridges on the bus, recursively scan the buses behind the
 * bridges. If the setting up and tuning of the bus causes a reset to be
 * required, reset the bus and scan it again.
 *
 * @param busdev Pointer to the bus device.
 */
static void scan_bus(struct device *busdev)
{
	int do_scan_bus;
	struct stopwatch sw;
	long scan_time;

	if (!busdev->enabled)
		return;

	printk(BIOS_DEBUG, "%s scanning...\n", dev_path(busdev));

	post_log_path(busdev);

	stopwatch_init(&sw);

	do_scan_bus = 1;
	while (do_scan_bus) {
		struct bus *link = busdev->downstream;
		busdev->ops->scan_bus(busdev);
		do_scan_bus = 0;
		if (!link || !link->reset_needed)
			continue;
		if (reset_bus(link))
			do_scan_bus = 1;
		else
			busdev->upstream->reset_needed = 1;
	}

	scan_time = stopwatch_duration_msecs(&sw);
	printk(BIOS_DEBUG, "%s: bus %s finished in %ld msecs\n", __func__,
	       dev_path(busdev), scan_time);
}

void scan_bridges(struct bus *bus)
{
	struct device *child;

	for (child = bus->children; child; child = child->sibling) {
		if (!child->ops || !child->ops->scan_bus)
			continue;
		scan_bus(child);
	}
}

/**
 * Determine the existence of devices and extend the device tree.
 *
 * Most of the devices in the system are listed in the mainboard devicetree.cb
 * file. The device structures for these devices are generated at compile
 * time by the config tool and are organized into the device tree. This
 * function determines if the devices created at compile time actually exist
 * in the physical system.
 *
 * For devices in the physical system but not listed in devicetree.cb,
 * the device structures have to be created at run time and attached to the
 * device tree.
 *
 * This function starts from the root device 'dev_root', scans the buses in
 * the system recursively, and modifies the device tree according to the
 * result of the probe.
 *
 * This function has no idea how to scan and probe buses and devices at all.
 * It depends on the bus/device specific scan_bus() method to do it. The
 * scan_bus() method also has to create the device structure and attach
 * it to the device tree.
 */
void dev_enumerate(void)
{
	struct device *root;

	printk(BIOS_INFO, "Enumerating buses...\n");

	root = &dev_root;

	show_all_devs(BIOS_SPEW, "Before device enumeration.");
	printk(BIOS_SPEW, "Compare with tree...\n");
	show_devs_tree(root, BIOS_SPEW, 0);

	if (root->chip_ops && root->chip_ops->enable_dev)
		root->chip_ops->enable_dev(root);

	if (!root->ops || !root->ops->scan_bus) {
		printk(BIOS_ERR, "dev_root missing scan_bus operation");
		return;
	}
	scan_bus(root);
	post_log_clear();
	printk(BIOS_INFO, "done\n");
}

/**
 * Configure devices on the devices tree.
 *
 * Starting at the root of the device tree, travel it recursively in two
 * passes. In the first pass, we compute and allocate resources (ranges)
 * required by each device. In the second pass, the resources ranges are
 * relocated to their final position and stored to the hardware.
 *
 * I/O resources grow upward. MEM resources grow downward.
 *
 * Since the assignment is hierarchical we set the values into the dev_root
 * struct.
 */
void dev_configure(void)
{
	const struct device *root;

	set_vga_bridge_bits();

	printk(BIOS_INFO, "Allocating resources...\n");

	root = &dev_root;

	/*
	 * Each domain should create resources which contain the entire address
	 * space for IO, MEM, and PREFMEM resources in the domain. The
	 * allocation of device resources will be done from this address space.
	 */

	/* Read the resources for the entire tree. */

	printk(BIOS_INFO, "Reading resources...\n");
	read_resources(root->downstream);
	printk(BIOS_INFO, "Done reading resources.\n");

	print_resource_tree(root, BIOS_SPEW, "After reading.");

	allocate_resources(root);

	assign_resources(root->downstream);
	printk(BIOS_INFO, "Done setting resources.\n");
	print_resource_tree(root, BIOS_SPEW, "After assigning values.");

	printk(BIOS_INFO, "Done allocating resources.\n");
}

/**
 * Enable devices on the device tree.
 *
 * Starting at the root, walk the tree and enable all devices/bridges by
 * calling the device's enable_resources() method.
 */
void dev_enable(void)
{
	printk(BIOS_INFO, "Enabling resources...\n");

	/* Now enable everything. */
	if (dev_root.downstream)
		enable_resources(dev_root.downstream);

	printk(BIOS_INFO, "done.\n");
}

/**
 * Initialize a specific device.
 *
 * The parent should be initialized first to avoid having an ordering problem.
 * This is done by calling the parent's init() method before its children's
 * init() methods.
 *
 * @param dev The device to be initialized.
 */
static void init_dev(struct device *dev)
{
	if (!dev->enabled)
		return;

	if (!dev->initialized && dev->ops && dev->ops->init) {
		struct stopwatch sw;
		long init_time;

		if (dev->path.type == DEVICE_PATH_I2C) {
			printk(BIOS_DEBUG, "smbus: %s->", dev_path(dev->upstream->dev));
		}

		printk(BIOS_DEBUG, "%s init\n", dev_path(dev));

		stopwatch_init(&sw);
		dev->initialized = 1;
		dev->ops->init(dev);

		init_time = stopwatch_duration_msecs(&sw);
		printk(BIOS_DEBUG, "%s init finished in %ld msecs\n", dev_path(dev),
		       init_time);
	}
}

static void init_link(struct bus *link)
{
	struct device *dev;

	for (dev = link->children; dev; dev = dev->sibling) {
		post_code(POSTCODE_BS_DEV_INIT);
		post_log_path(dev);
		init_dev(dev);
	}

	for (dev = link->children; dev; dev = dev->sibling)
		if (dev->downstream)
			init_link(dev->downstream);
}

/**
 * Initialize all devices in the global device tree.
 *
 * Starting at the root device, call the device's init() method to do
 * device-specific setup, then call each child's init() method.
 */
void dev_initialize(void)
{
	printk(BIOS_INFO, "Initializing devices...\n");

	/* First call the mainboard init. */
	init_dev(&dev_root);

	/* Now initialize everything. */
	if (dev_root.downstream)
		init_link(dev_root.downstream);
	post_log_clear();

	printk(BIOS_INFO, "Devices initialized\n");
	show_all_devs(BIOS_SPEW, "After init.");
}

/**
 * Finalize a specific device.
 *
 * The parent should be finalized first to avoid having an ordering problem.
 * This is done by calling the parent's final() method before its children's
 * final() methods.
 *
 * @param dev The device to be initialized.
 */
static void final_dev(struct device *dev)
{
	if (!dev->enabled)
		return;

	if (dev->ops && dev->ops->final) {
		printk(BIOS_DEBUG, "%s final\n", dev_path(dev));
		dev->ops->final(dev);
	}
}

static void final_link(struct bus *link)
{
	struct device *dev;

	for (dev = link->children; dev; dev = dev->sibling)
		final_dev(dev);

	for (dev = link->children; dev; dev = dev->sibling)
		if (dev->downstream)
			final_link(dev->downstream);
}
/**
 * Finalize all devices in the global device tree.
 *
 * Starting at the root device, call the device's final() method to do
 * device-specific cleanup, then call each child's final() method.
 */
void dev_finalize(void)
{
	printk(BIOS_INFO, "Finalize devices...\n");

	/* First call the mainboard finalize. */
	final_dev(&dev_root);

	/* Now finalize everything. */
	final_link(dev_root.downstream);

	printk(BIOS_INFO, "Devices finalized\n");
}
