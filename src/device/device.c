/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Originally based on the Linux kernel (arch/i386/kernel/pci-pc.c).
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <memrange.h>
#include <post.h>
#include <stdlib.h>
#include <string.h>
#include <smp/spinlock.h>
#if CONFIG(ARCH_X86)
#include <arch/ebda.h>
#endif
#include <timer.h>

/** Pointer to the last device */
extern struct device *last_dev;
/** Linked list of free resources */
struct resource *free_resources = NULL;

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

#if CONFIG(GFXUMA)
/* IGD UMA memory */
uint64_t uma_memory_base = 0;
uint64_t uma_memory_size = 0;
#endif

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
	dev->bus = parent;
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
 * Round a number up to an alignment.
 *
 * @param val The starting value.
 * @param pow Alignment as a power of two.
 * @return Rounded up number.
 */
static resource_t round(resource_t val, unsigned long pow)
{
	return ALIGN_UP(val, POWER_OF_2(pow));
}

static const char *resource2str(const struct resource *res)
{
	if (res->flags & IORESOURCE_IO)
		return "io";
	if (res->flags & IORESOURCE_PREFETCH)
		return "prefmem";
	if (res->flags & IORESOURCE_MEM)
		return "mem";
	return "undefined";
}

/**
 * Read the resources on all devices of a given bus.
 *
 * @param bus Bus to read the resources on.
 */
static void read_resources(struct bus *bus)
{
	struct device *curdev;

	printk(BIOS_SPEW, "%s %s bus %x link: %d\n", dev_path(bus->dev),
	       __func__, bus->secondary, bus->link_num);

	/* Walk through all devices and find which resources they need. */
	for (curdev = bus->children; curdev; curdev = curdev->sibling) {
		struct bus *link;

		if (!curdev->enabled)
			continue;

		if (!curdev->ops || !curdev->ops->read_resources) {
			if (curdev->path.type != DEVICE_PATH_APIC)
				printk(BIOS_ERR, "%s missing read_resources\n",
				       dev_path(curdev));
			continue;
		}
		post_log_path(curdev);
		curdev->ops->read_resources(curdev);

		/* Read in the resources behind the current device's links. */
		for (link = curdev->link_list; link; link = link->next)
			read_resources(link);
	}
	post_log_clear();
	printk(BIOS_SPEW, "%s read_resources bus %d link: %d done\n",
	       dev_path(bus->dev), bus->secondary, bus->link_num);
}

struct pick_largest_state {
	struct resource *last;
	const struct device *result_dev;
	struct resource *result;
	int seen_last;
};

static void pick_largest_resource(void *gp, struct device *dev,
				  struct resource *resource)
{
	struct pick_largest_state *state = gp;
	struct resource *last;

	last = state->last;

	/* Be certain to pick the successor to last. */
	if (resource == last) {
		state->seen_last = 1;
		return;
	}
	if (resource->flags & IORESOURCE_FIXED)
		return;	/* Skip it. */
	if (last && ((last->align < resource->align) ||
		     ((last->align == resource->align) &&
		      (last->size < resource->size)) ||
		     ((last->align == resource->align) &&
		      (last->size == resource->size) && (!state->seen_last)))) {
		return;
	}
	if (!state->result ||
	    (state->result->align < resource->align) ||
	    ((state->result->align == resource->align) &&
	     (state->result->size < resource->size))) {
		state->result_dev = dev;
		state->result = resource;
	}
}

static const struct device *largest_resource(struct bus *bus,
				       struct resource **result_res,
				       unsigned long type_mask,
				       unsigned long type)
{
	struct pick_largest_state state;

	state.last = *result_res;
	state.result_dev = NULL;
	state.result = NULL;
	state.seen_last = 0;

	search_bus_resources(bus, type_mask, type, pick_largest_resource,
			     &state);

	*result_res = state.result;
	return state.result_dev;
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
		if (dev->bus->no_vga16) {
			printk(BIOS_WARNING,
				"A bridge on the path doesn't support 16-bit VGA decoding!");
		}

		if (dev->on_mainboard) {
			vga_onboard = dev;
		} else {
			vga = dev;
		}

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
		vga_onboard && vga_onboard->ops && vga_onboard->ops->disable) {
		printk(BIOS_DEBUG, "Use plugin graphics over integrated.\n");
		vga_onboard->ops->disable(vga_onboard);
	}

	if (vga) {
		/* VGA is first add-on card or the only onboard VGA. */
		printk(BIOS_DEBUG, "Setting up VGA for %s\n", dev_path(vga));
		/* All legacy VGA cards have MEM & I/O space registers. */
		vga->command |= (PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		vga_pri = vga;
		bus = vga->bus;
	}

	/* Now walk up the bridges setting the VGA enable. */
	while (bus) {
		printk(BIOS_DEBUG, "Setting PCI_BRIDGE_CTL_VGA for bridge %s\n",
		       dev_path(bus->dev));
		bus->bridge_ctrl |= PCI_BRIDGE_CTL_VGA | PCI_BRIDGE_CTL_VGA16;
		bus = (bus == bus->dev->bus) ? 0 : bus->dev->bus;
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

	printk(BIOS_SPEW, "%s assign_resources, bus %d link: %d\n",
	       dev_path(bus->dev), bus->secondary, bus->link_num);

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
	printk(BIOS_SPEW, "%s assign_resources, bus %d link: %d\n",
	       dev_path(bus->dev), bus->secondary, bus->link_num);
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
	struct bus *c_link;

	for (dev = link->children; dev; dev = dev->sibling) {
		if (dev->enabled && dev->ops && dev->ops->enable_resources) {
			post_log_path(dev);
			dev->ops->enable_resources(dev);
		}
	}

	for (dev = link->children; dev; dev = dev->sibling) {
		for (c_link = dev->link_list; c_link; c_link = c_link->next)
			enable_resources(c_link);
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
		struct bus *link;
		busdev->ops->scan_bus(busdev);
		do_scan_bus = 0;
		for (link = busdev->link_list; link; link = link->next) {
			if (link->reset_needed) {
				if (reset_bus(link))
					do_scan_bus = 1;
				else
					busdev->bus->reset_needed = 1;
			}
		}
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

static bool dev_has_children(const struct device *dev)
{
	const struct bus *bus = dev->link_list;
	return bus && bus->children;
}

/*
 * During pass 1, once all the requirements for downstream devices of a bridge are gathered,
 * this function calculates the overall resource requirement for the bridge. It starts by
 * picking the largest resource requirement downstream for the given resource type and works by
 * adding requirements in descending order.
 *
 * Additionally, it takes alignment and limits of the downstream devices into consideration and
 * ensures that they get propagated to the bridge resource. This is required to guarantee that
 * the upstream bridge/domain honors the limit and alignment requirements for this bridge based
 * on the tightest constraints downstream.
 */
static void update_bridge_resource(const struct device *bridge, struct resource *bridge_res,
				   unsigned long type_match)
{
	const struct device *child;
	struct resource *child_res;
	resource_t base;
	bool first_child_res = true;
	const unsigned long type_mask = IORESOURCE_TYPE_MASK | IORESOURCE_PREFETCH;
	struct bus *bus = bridge->link_list;

	child_res = NULL;

	/*
	 * `base` keeps track of where the next allocation for child resource can take place
	 * from within the bridge resource window. Since the bridge resource window allocation
	 * is not performed yet, it can start at 0. Base gets updated every time a resource
	 * requirement is accounted for in the loop below. After scanning all these resources,
	 * base will indicate the total size requirement for the current bridge resource
	 * window.
	 */
	base = 0;

	printk(BIOS_SPEW, "%s %s: size: %llx align: %d gran: %d limit: %llx\n",
	       dev_path(bridge), resource2str(bridge_res), bridge_res->size,
	       bridge_res->align, bridge_res->gran, bridge_res->limit);

	while ((child = largest_resource(bus, &child_res, type_mask, type_match))) {

		/* Size 0 resources can be skipped. */
		if (!child_res->size)
			continue;

		/*
		 * Propagate the resource alignment to the bridge resource if this is the first
		 * child resource with non-zero size being considered. For all other children
		 * resources, alignment is taken care of by updating the base to round up as per
		 * the child resource alignment. It is guaranteed that pass 2 follows the exact
		 * same method of picking the resource for allocation using
		 * largest_resource(). Thus, as long as the alignment for first child resource
		 * is propagated up to the bridge resource, it can be guaranteed that the
		 * alignment for all resources is appropriately met.
		 */
		if (first_child_res && (child_res->align > bridge_res->align))
			bridge_res->align = child_res->align;

		first_child_res = false;

		/*
		 * Propagate the resource limit to the bridge resource only if child resource
		 * limit is non-zero. If a downstream device has stricter requirements
		 * w.r.t. limits for any resource, that constraint needs to be propagated back
		 * up to the downstream bridges of the domain. This guarantees that the resource
		 * allocation which starts at the domain level takes into account all these
		 * constraints thus working on a global view.
		 */
		if (child_res->limit && (child_res->limit < bridge_res->limit))
			bridge_res->limit = child_res->limit;

		/*
		 * Propagate the downstream resource request to allocate above 4G boundary to
		 * upstream bridge resource. This ensures that during pass 2, the resource
		 * allocator at domain level has a global view of all the downstream device
		 * requirements and thus address space is allocated as per updated flags in the
		 * bridge resource.
		 *
		 * Since the bridge resource is a single window, all the downstream resources of
		 * this bridge resource will be allocated space above 4G boundary.
		 */
		if (child_res->flags & IORESOURCE_ABOVE_4G)
			bridge_res->flags |= IORESOURCE_ABOVE_4G;

		/*
		 * Alignment value of 0 means that the child resource has no alignment
		 * requirements and so the base value remains unchanged here.
		 */
		base = round(base, child_res->align);

		printk(BIOS_SPEW, "%s %02lx *  [0x%llx - 0x%llx] %s\n",
			dev_path(child), child_res->index, base, base + child_res->size - 1,
			resource2str(child_res));

		base += child_res->size;
	}

	/*
	 * After all downstream device resources are scanned, `base` represents the total size
	 * requirement for the current bridge resource window. This size needs to be rounded up
	 * to the granularity requirement of the bridge to ensure that the upstream
	 * bridge/domain allocates big enough window.
	 */
	bridge_res->size = round(base, bridge_res->gran);

	printk(BIOS_SPEW, "%s %s: size: %llx align: %d gran: %d limit: %llx done\n",
		dev_path(bridge), resource2str(bridge_res), bridge_res->size,
		bridge_res->align, bridge_res->gran, bridge_res->limit);
}

/*
 * During pass 1, resource allocator at bridge level gathers requirements from downstream
 * devices and updates its own resource windows for the provided resource type.
 */
static void compute_bridge_resources(const struct device *bridge, unsigned long type_match)
{
	const struct device *child;
	struct resource *res;
	struct bus *bus = bridge->link_list;
	const unsigned long type_mask = IORESOURCE_TYPE_MASK | IORESOURCE_PREFETCH;

	for (res = bridge->resource_list; res; res = res->next) {
		if (!(res->flags & IORESOURCE_BRIDGE))
			continue;

		if ((res->flags & type_mask) != type_match)
			continue;

		/*
		 * Ensure that the resource requirements for all downstream bridges are
		 * gathered before updating the window for current bridge resource.
		 */
		for (child = bus->children; child; child = child->sibling) {
			if (!dev_has_children(child))
				continue;
			compute_bridge_resources(child, type_match);
		}

		/*
		 * Update the window for current bridge resource now that all downstream
		 * requirements are gathered.
		 */
		update_bridge_resource(bridge, res, type_match);
	}
}

/*
 * During pass 1, resource allocator walks down the entire sub-tree of a domain. It gathers
 * resource requirements for every downstream bridge by looking at the resource requests of its
 * children. Thus, the requirement gathering begins at the leaf devices and is propagated back
 * up to the downstream bridges of the domain.
 *
 * At domain level, it identifies every downstream bridge and walks down that bridge to gather
 * requirements for each resource type i.e. i/o, mem and prefmem. Since bridges have separate
 * windows for mem and prefmem, requirements for each need to be collected separately.
 *
 * Domain resource windows are fixed ranges and hence requirement gathering does not result in
 * any changes to these fixed ranges.
 */
static void compute_domain_resources(const struct device *domain)
{
	const struct device *child;

	if (domain->link_list == NULL)
		return;

	for (child = domain->link_list->children; child; child = child->sibling) {

		/* Skip if this is not a bridge or has no children under it. */
		if (!dev_has_children(child))
			continue;

		compute_bridge_resources(child, IORESOURCE_IO);
		compute_bridge_resources(child, IORESOURCE_MEM);
		compute_bridge_resources(child, IORESOURCE_MEM | IORESOURCE_PREFETCH);
	}
}

/*
 * If the resource base is set to the limit, then it means that the resource is invalid and
 * hence cannot be used for allocation.
 */
static bool is_resource_invalid(const struct resource *res)
{
	return res->base == res->limit;
}

/*
 * This function initializes memranges for domain device. If the resource crosses 4G boundary,
 * then this function splits it into two ranges -- one for the window below 4G and the other for
 * the window above 4G. The latter range has IORESOURCE_ABOVE_4G flag set to satisfy resource
 * requests from downstream devices for allocations above 4G.
 */
static void initialize_domain_memranges(struct memranges *ranges, const struct resource *res,
					unsigned long memrange_type)
{
	resource_t res_base;
	resource_t res_limit;
	const resource_t limit_4g = 0xffffffff;

	memranges_init_empty(ranges, NULL, 0);

	if ((res == NULL) || is_resource_invalid(res))
		return;

	res_base = res->base;
	res_limit = res->limit;

	/*
	 * Split the resource into two separate ranges if it crosses the 4G boundary. Memrange
	 * type is set differently to ensure that memrange does not merge these two ranges. For
	 * the range above 4G boundary, given memrange type is ORed with IORESOURCE_ABOVE_4G.
	 */
	if (res_base <= limit_4g) {

		resource_t range_limit;

		/* Clip the resource limit at 4G boundary if necessary. */
		range_limit = MIN(res_limit, limit_4g);
		memranges_insert(ranges, res_base, range_limit - res_base + 1, memrange_type);

		/*
		 * If the resource lies completely below the 4G boundary, nothing more needs to
		 * be done.
		 */
		if (res_limit <= limit_4g)
			return;

		/*
		 * If the resource window crosses the 4G boundary, then update res_base to add
		 * another entry for the range above the boundary.
		 */
		res_base = limit_4g + 1;
	}

	if (res_base > res_limit)
		return;

	/*
	 * If resource lies completely above the 4G boundary or if the resource was clipped to
	 * add two separate ranges, the range above 4G boundary has the resource flag
	 * IORESOURCE_ABOVE_4G set. This allows domain to handle any downstream requests for
	 * resource allocation above 4G differently.
	 */
	memranges_insert(ranges, res_base, res_limit - res_base + 1,
			 memrange_type | IORESOURCE_ABOVE_4G);
}

/*
 * This function initializes memranges for bridge device. Unlike domain, bridge does not need to
 * care about resource window crossing 4G boundary. This is handled by the resource allocator at
 * domain level to ensure that all downstream bridges are allocated space either above or below
 * 4G boundary as per the state of IORESOURCE_ABOVE_4G for the respective bridge resource.
 *
 * So, this function creates a single range of the entire resource window available for the
 * bridge resource. Thus all downstream resources of the bridge for the given resource type get
 * allocated space from the same window. If there is any downstream resource of the bridge which
 * requests allocation above 4G, then all other downstream resources of the same type under the
 * bridge get allocated above 4G.
 */
static void initialize_bridge_memranges(struct memranges *ranges, const struct resource *res,
					unsigned long memrange_type)
{

	memranges_init_empty(ranges, NULL, 0);

	if ((res == NULL) || is_resource_invalid(res))
		return;

	memranges_insert(ranges, res->base, res->limit - res->base + 1, memrange_type);
}

static void print_resource_ranges(const struct memranges *ranges)
{
	const struct range_entry *r;

	printk(BIOS_INFO, "Resource ranges:\n");

	if (memranges_is_empty(ranges))
		printk(BIOS_INFO, "EMPTY!!\n");

	memranges_each_entry(r, ranges) {
		printk(BIOS_INFO, "Base: %llx, Size: %llx, Tag: %lx\n",
		       range_entry_base(r), range_entry_size(r), range_entry_tag(r));
	}
}

static void mark_resource_invalid(struct resource *res)
{
	res->base = res->limit;
	res->flags |= IORESOURCE_ASSIGNED;
}

/*
 * This is where the actual allocation of resources happens during pass 2. Given the list of
 * memory ranges corresponding to the resource of given type, it finds the biggest unallocated
 * resource using the type mask on the downstream bus. This continues in a descending
 * order until all resources of given type are allocated address space within the current
 * resource window.
 *
 * If a downstream resource cannot be allocated space for any reason, then its base is set to
 * its limit and flags are updated to indicate that the resource assignment is complete. This is
 * done to ensure that it does not confuse find_pci_tolm().
 */
static void allocate_child_resources(struct bus *bus, struct memranges *ranges,
					unsigned long type_mask, unsigned long type_match)
{
	struct resource *resource = NULL;
	const struct device *dev;

	while ((dev = largest_resource(bus, &resource, type_mask, type_match))) {

		if (!resource->size) {
			mark_resource_invalid(resource);
			continue;
		}

		if (memranges_steal(ranges, resource->limit, resource->size, resource->align,
				    type_match, &resource->base) == false) {
			printk(BIOS_ERR, "ERROR: Resource didn't fit!!! ");
			printk(BIOS_SPEW, "%s %02lx *  size: 0x%llx limit: %llx %s\n",
			       dev_path(dev), resource->index,
			       resource->size, resource->limit, resource2str(resource));
			mark_resource_invalid(resource);
			continue;
		}

		resource->limit = resource->base + resource->size - 1;
		resource->flags |= IORESOURCE_ASSIGNED;

		printk(BIOS_SPEW, "%s %02lx *  [0x%llx - 0x%llx] limit: %llx %s\n",
		       dev_path(dev), resource->index, resource->base,
		       resource->size ? resource->base + resource->size - 1 :
		       resource->base, resource->limit, resource2str(resource));
	}
}

static void update_constraints(void *gp, struct device *dev, struct resource *res)
{
	struct memranges *ranges = gp;

	if (!res->size)
		return;

	printk(BIOS_SPEW, "%s: %s %02lx base %08llx limit %08llx %s (fixed)\n",
	       __func__, dev_path(dev), res->index, res->base,
	       res->base + res->size - 1, resource2str(res));

	memranges_create_hole(ranges, res->base, res->size);
}

static void constrain_domain_resources(struct bus *bus, struct memranges *ranges,
				       unsigned long type)
{
	/*
	 * Scan the entire tree to identify any fixed resources allocated by any device to
	 * ensure that the address map for domain resources are appropriately updated.
	 *
	 * Domains can typically provide memrange for entire address space. So, this function
	 * punches holes in the address space for all fixed resources that are already
	 * defined. Both IO and normal memory resources are added as fixed. Both need to be
	 * removed from address space where dynamic resource allocations are sourced.
	 */
	search_bus_resources(bus, type | IORESOURCE_FIXED, type | IORESOURCE_FIXED,
			     update_constraints, ranges);

	if (type == IORESOURCE_IO) {
		/*
		 * Don't allow allocations in the VGA I/O range. PCI has special cases for
		 * that.
		 */
		memranges_create_hole(ranges, 0x3b0, 0x3df);

		/*
		 * Resource allocator no longer supports the legacy behavior where I/O resource
		 * allocation is guaranteed to avoid aliases over legacy PCI expansion card
		 * addresses.
		 */
	}
}

/*
 * This function creates a list of memranges of given type using the resource that is
 * provided. If the given resource is NULL or if the resource window size is 0, then it creates
 * an empty list. This results in resource allocation for that resource type failing for all
 * downstream devices since there is nothing to allocate from.
 *
 * In case of domain, it applies additional constraints to ensure that the memranges do not
 * overlap any of the fixed resources under that domain. Domain typically seems to provide
 * memrange for entire address space. Thus, it is up to the chipset to add DRAM and all other
 * windows which cannot be used for resource allocation as fixed resources.
 */
static void setup_resource_ranges(const struct device *dev, const struct resource *res,
				  unsigned long type, struct memranges *ranges)
{
	printk(BIOS_SPEW, "%s %s: base: %llx size: %llx align: %d gran: %d limit: %llx\n",
		dev_path(dev), resource2str(res), res->base, res->size, res->align,
		res->gran, res->limit);

	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		initialize_domain_memranges(ranges, res, type);
		constrain_domain_resources(dev->link_list, ranges, type);
	} else {
		initialize_bridge_memranges(ranges, res, type);
	}

	print_resource_ranges(ranges);
}

static void cleanup_resource_ranges(const struct device *dev, struct memranges *ranges,
					const struct resource *res)
{
	memranges_teardown(ranges);
	printk(BIOS_SPEW, "%s %s: base: %llx size: %llx align: %d gran: %d limit: %llx done\n",
		dev_path(dev), resource2str(res), res->base, res->size, res->align,
		res->gran, res->limit);
}

/*
 * Pass 2 of resource allocator at the bridge level loops through all the resources for the
 * bridge and generates a list of memory ranges similar to that at the domain level. However,
 * there is no need to apply any additional constraints since the window allocated to the bridge
 * is guaranteed to be non-overlapping by the allocator at domain level.
 *
 * Allocation at the bridge level works the same as at domain level (starts with the biggest
 * resource requirement from downstream devices and continues in descending order). One major
 * difference at the bridge level is that it considers prefmem resources separately from mem
 * resources.
 *
 * Once allocation at the current bridge is complete, resource allocator continues walking down
 * the downstream bridges until it hits the leaf devices.
 */
static void allocate_bridge_resources(const struct device *bridge)
{
	struct memranges ranges;
	const struct resource *res;
	struct bus *bus = bridge->link_list;
	unsigned long type_match;
	struct device *child;
	const unsigned long type_mask = IORESOURCE_TYPE_MASK | IORESOURCE_PREFETCH;

	for (res = bridge->resource_list; res; res = res->next) {
		if (!res->size)
			continue;

		if (!(res->flags & IORESOURCE_BRIDGE))
			continue;

		type_match = res->flags & type_mask;

		setup_resource_ranges(bridge, res, type_match, &ranges);
		allocate_child_resources(bus, &ranges, type_mask, type_match);
		cleanup_resource_ranges(bridge, &ranges, res);
	}

	for (child = bus->children; child; child = child->sibling) {
		if (!dev_has_children(child))
			continue;

		allocate_bridge_resources(child);
	}
}

static const struct resource *find_domain_resource(const struct device *domain,
							unsigned long type)
{
	const struct resource *res;

	for (res = domain->resource_list; res; res = res->next) {
		if (res->flags & IORESOURCE_FIXED)
			continue;

		if ((res->flags & IORESOURCE_TYPE_MASK) == type)
			return res;
	}

	return NULL;
}

/*
 * Pass 2 of resource allocator begins at the domain level. Every domain has two types of
 * resources - io and mem. For each of these resources, this function creates a list of memory
 * ranges that can be used for downstream resource allocation. This list is constrained to
 * remove any fixed resources in the domain sub-tree of the given resource type. It then uses
 * the memory ranges to apply best fit on the resource requirements of the downstream devices.
 *
 * Once resources are allocated to all downstream devices of the domain, it walks down each
 * downstream bridge to continue the same process until resources are allocated to all devices
 * under the domain.
 */
static void allocate_domain_resources(const struct device *domain)
{
	struct memranges ranges;
	struct device *child;
	const struct resource *res;

	/* Resource type I/O */
	res = find_domain_resource(domain, IORESOURCE_IO);
	if (res) {
		setup_resource_ranges(domain, res, IORESOURCE_IO, &ranges);
		allocate_child_resources(domain->link_list, &ranges, IORESOURCE_TYPE_MASK,
					 IORESOURCE_IO);
		cleanup_resource_ranges(domain, &ranges, res);
	}

	/*
	 * Resource type Mem:
	 * Domain does not distinguish between mem and prefmem resources. Thus, the resource
	 * allocation at domain level considers mem and prefmem together when finding the best
	 * fit based on the biggest resource requirement.
	 *
	 * However, resource requests for allocation above 4G boundary need to be handled
	 * separately if the domain resource window crosses this boundary. There is a single
	 * window for resource of type IORESOURCE_MEM. When creating memranges, this resource
	 * is split into two separate ranges -- one for the window below 4G boundary and other
	 * for the window above 4G boundary (with IORESOURCE_ABOVE_4G flag set). Thus, when
	 * allocating child resources, requests for below and above the 4G boundary are handled
	 * separately by setting the type_mask and type_match to allocate_child_resources()
	 * accordingly.
	 */
	res = find_domain_resource(domain, IORESOURCE_MEM);
	if (res) {
		setup_resource_ranges(domain, res, IORESOURCE_MEM, &ranges);
		allocate_child_resources(domain->link_list, &ranges,
					 IORESOURCE_TYPE_MASK | IORESOURCE_ABOVE_4G,
					 IORESOURCE_MEM);
		allocate_child_resources(domain->link_list, &ranges,
					 IORESOURCE_TYPE_MASK | IORESOURCE_ABOVE_4G,
					 IORESOURCE_MEM | IORESOURCE_ABOVE_4G);
		cleanup_resource_ranges(domain, &ranges, res);
	}

	for (child = domain->link_list->children; child; child = child->sibling) {
		if (!dev_has_children(child))
			continue;

		/* Continue allocation for all downstream bridges. */
		allocate_bridge_resources(child);
	}
}

/*
 * This function forms the guts of the resource allocator. It walks through the entire device
 * tree for each domain two times.
 *
 * Every domain has a fixed set of ranges. These ranges cannot be relaxed based on the
 * requirements of the downstream devices. They represent the available windows from which
 * resources can be allocated to the different devices under the domain.
 *
 * In order to identify the requirements of downstream devices, resource allocator walks in a
 * DFS fashion. It gathers the requirements from leaf devices and propagates those back up
 * to their upstream bridges until the requirements for all the downstream devices of the domain
 * are gathered. This is referred to as pass 1 of resource allocator.
 *
 * Once the requirements for all the devices under the domain are gathered, resource allocator
 * walks a second time to allocate resources to downstream devices as per the
 * requirements. It always picks the biggest resource request as per the type (i/o and mem) to
 * allocate space from its fixed window to the immediate downstream device of the domain. In
 * order to accomplish best fit for the resources, a list of ranges is maintained by each
 * resource type (i/o and mem). Domain does not differentiate between mem and prefmem. Since
 * they are allocated space from the same window, the resource allocator at the domain level
 * ensures that the biggest requirement is selected indepedent of the prefetch type. Once the
 * resource allocation for all immediate downstream devices is complete at the domain level,
 * resource allocator walks down the subtree for each downstream bridge to continue the
 * allocation process at the bridge level. Since bridges have separate windows for i/o, mem and
 * prefmem, best fit algorithm at bridge level looks for the biggest requirement considering
 * prefmem resources separately from non-prefmem resources. This continues until resource
 * allocation is performed for all downstream bridges in the domain sub-tree. This is referred
 * to as pass 2 of resource allocator.
 *
 * Some rules that are followed by the resource allocator:
 *  - Allocate resource locations for every device as long as the requirements can be satisfied.
 *  - If a resource cannot be allocated any address space, then that resource needs to be
 * properly updated to ensure that it does not incorrectly overlap some address space reserved
 * for a different purpose.
 *  - Don't overlap with resources in fixed locations.
 *  - Don't overlap and follow the rules of bridges -- downstream devices of bridges should use
 * parts of the address space allocated to the bridge.
 */
static void allocate_resources(const struct device *root)
{
	const struct device *child;

	if ((root == NULL) || (root->link_list == NULL))
		return;

	for (child = root->link_list->children; child; child = child->sibling) {

		if (child->path.type != DEVICE_PATH_DOMAIN)
			continue;

		post_log_path(child);

		/* Pass 1 - Gather requirements. */
		printk(BIOS_INFO, "Resource allocator: %s - Pass 1 (gathering requirements)\n",
		       dev_path(child));
		compute_domain_resources(child);

		/* Pass 2 - Allocate resources as per gathered requirements. */
		printk(BIOS_INFO, "Resource allocator: %s - Pass 2 (allocating resources)\n",
		       dev_path(child));
		allocate_domain_resources(child);
	}
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
	read_resources(root->link_list);
	printk(BIOS_INFO, "Done reading resources.\n");

	print_resource_tree(root, BIOS_SPEW, "After reading.");

	allocate_resources(root);

	assign_resources(root->link_list);
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
	struct bus *link;

	printk(BIOS_INFO, "Enabling resources...\n");

	/* Now enable everything. */
	for (link = dev_root.link_list; link; link = link->next)
		enable_resources(link);

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
			printk(BIOS_DEBUG, "smbus: %s[%d]->",
			       dev_path(dev->bus->dev), dev->bus->link_num);
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
	struct bus *c_link;

	for (dev = link->children; dev; dev = dev->sibling) {
		post_code(POST_BS_DEV_INIT);
		post_log_path(dev);
		init_dev(dev);
	}

	for (dev = link->children; dev; dev = dev->sibling) {
		for (c_link = dev->link_list; c_link; c_link = c_link->next)
			init_link(c_link);
	}
}

/**
 * Initialize all devices in the global device tree.
 *
 * Starting at the root device, call the device's init() method to do
 * device-specific setup, then call each child's init() method.
 */
void dev_initialize(void)
{
	struct bus *link;

	printk(BIOS_INFO, "Initializing devices...\n");

#if CONFIG(ARCH_X86)
	/* Ensure EBDA is prepared before Option ROMs. */
	setup_default_ebda();
#endif

	/* First call the mainboard init. */
	init_dev(&dev_root);

	/* Now initialize everything. */
	for (link = dev_root.link_list; link; link = link->next)
		init_link(link);
	post_log_clear();

	printk(BIOS_INFO, "Devices initialized\n");
	show_all_devs(BIOS_SPEW, "After init.");
}

/**
 * Finalize a specific device.
 *
 * The parent should be finalized first to avoid having an ordering problem.
 * This is done by calling the parent's final() method before its childrens'
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
	struct bus *c_link;

	for (dev = link->children; dev; dev = dev->sibling)
		final_dev(dev);

	for (dev = link->children; dev; dev = dev->sibling) {
		for (c_link = dev->link_list; c_link; c_link = c_link->next)
			final_link(c_link);
	}
}
/**
 * Finalize all devices in the global device tree.
 *
 * Starting at the root device, call the device's final() method to do
 * device-specific cleanup, then call each child's final() method.
 */
void dev_finalize(void)
{
	struct bus *link;

	printk(BIOS_INFO, "Finalize devices...\n");

	/* First call the mainboard finalize. */
	final_dev(&dev_root);

	/* Now finalize everything. */
	for (link = dev_root.link_list; link; link = link->next)
		final_link(link);

	printk(BIOS_INFO, "Devices finalized\n");
}
