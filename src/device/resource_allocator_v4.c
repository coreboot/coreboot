/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include <device/device.h>
#include <memrange.h>
#include <post.h>
#include <types.h>

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

static void print_domain_res(const struct device *dev,
			     const struct resource *res, const char *suffix)
{
	printk(BIOS_DEBUG, "%s %s: base: %llx size: %llx align: %u gran: %u limit: %llx%s\n",
	       dev_path(dev), resource2str(res), res->base, res->size,
	       res->align, res->gran, res->limit, suffix);
}

#define res_printk(depth, str, ...)	printk(BIOS_DEBUG, "%*c"str, depth, ' ', __VA_ARGS__)

static void print_bridge_res(const struct device *dev, const struct resource *res,
			     int depth, const char *suffix)
{
	res_printk(depth, "%s %s: size: %llx align: %u gran: %u limit: %llx%s\n", dev_path(dev),
		   resource2str(res), res->size, res->align, res->gran, res->limit, suffix);
}

static void print_child_res(const struct device *dev, const struct resource *res, int depth)
{
	res_printk(depth + 1, "%s %02lx *  [0x%llx - 0x%llx] %s\n", dev_path(dev),
		   res->index, res->base, res->base + res->size - 1, resource2str(res));
}

static void print_fixed_res(const struct device *dev,
			    const struct resource *res, const char *prefix)
{
	printk(BIOS_DEBUG, " %s: %s %02lx base %08llx limit %08llx %s (fixed)\n",
	       prefix, dev_path(dev), res->index, res->base, res->base + res->size - 1,
	       resource2str(res));
}

static void print_assigned_res(const struct device *dev, const struct resource *res)
{
	printk(BIOS_DEBUG, "  %s %02lx *  [0x%llx - 0x%llx] limit: %llx %s\n",
	       dev_path(dev), res->index, res->base, res->limit, res->limit, resource2str(res));
}

static void print_failed_res(const struct device *dev, const struct resource *res)
{
	printk(BIOS_DEBUG, "  %s %02lx *  size: 0x%llx limit: %llx %s\n",
	       dev_path(dev), res->index, res->size, res->limit, resource2str(res));
}

static void print_resource_ranges(const struct device *dev, const struct memranges *ranges)
{
	const struct range_entry *r;

	printk(BIOS_INFO, " %s: Resource ranges:\n", dev_path(dev));

	if (memranges_is_empty(ranges))
		printk(BIOS_INFO, " * EMPTY!!\n");

	memranges_each_entry(r, ranges) {
		printk(BIOS_INFO, " * Base: %llx, Size: %llx, Tag: %lx\n",
		       range_entry_base(r), range_entry_size(r), range_entry_tag(r));
	}
}

static bool dev_has_children(const struct device *dev)
{
	const struct bus *bus = dev->link_list;
	return bus && bus->children;
}

static resource_t effective_limit(const struct resource *const res)
{
	/* Always allow bridge resources above 4G. */
	if (res->flags & IORESOURCE_BRIDGE)
		return res->limit;

	const resource_t quirk_4g_limit =
		res->flags & IORESOURCE_ABOVE_4G ? UINT64_MAX : UINT32_MAX;
	return MIN(res->limit, quirk_4g_limit);
}

/*
 * During pass 1, once all the requirements for downstream devices of a
 * bridge are gathered, this function calculates the overall resource
 * requirement for the bridge. It starts by picking the largest resource
 * requirement downstream for the given resource type and works by
 * adding requirements in descending order.
 *
 * Additionally, it takes alignment and limits of the downstream devices
 * into consideration and ensures that they get propagated to the bridge
 * resource. This is required to guarantee that the upstream bridge/
 * domain honors the limit and alignment requirements for this bridge
 * based on the tightest constraints downstream.
 *
 * Last but not least, it stores the offset inside the bridge resource
 * for each child resource in its base field. This simplifies pass 2
 * for resources behind a bridge, as we only have to add offsets to the
 * allocated base of the bridge resource.
 */
static void update_bridge_resource(const struct device *bridge, struct resource *bridge_res,
				   int print_depth)
{
	const struct device *child;
	struct resource *child_res;
	resource_t base;
	const unsigned long type_mask = IORESOURCE_TYPE_MASK | IORESOURCE_PREFETCH;
	const unsigned long type_match = bridge_res->flags & type_mask;
	struct bus *bus = bridge->link_list;

	child_res = NULL;

	/*
	 * `base` keeps track of where the next allocation for child resources
	 * can take place from within the bridge resource window. Since the
	 * bridge resource window allocation is not performed yet, it can start
	 * at 0. Base gets updated every time a resource requirement is
	 * accounted for in the loop below. After scanning all these resources,
	 * base will indicate the total size requirement for the current bridge
	 * resource window.
	 */
	base = 0;

	print_bridge_res(bridge, bridge_res, print_depth, "");

	while ((child = largest_resource(bus, &child_res, type_mask, type_match))) {

		/* Size 0 resources can be skipped. */
		if (!child_res->size)
			continue;

		/* Resources with 0 limit can't be assigned anything. */
		if (!child_res->limit)
			continue;

		/*
		 * Propagate the resource alignment to the bridge resource. The
		 * condition can only be true for the first (largest) resource. For all
		 * other child resources, alignment is taken care of by rounding their
		 * base up.
		 */
		if (child_res->align > bridge_res->align)
			bridge_res->align = child_res->align;

		/*
		 * Propagate the resource limit to the bridge resource. If a downstream
		 * device has stricter requirements w.r.t. limits for any resource, that
		 * constraint needs to be propagated back up to the bridges downstream
		 * of the domain. This way, the whole bridge resource fulfills the limit.
		 */
		if (effective_limit(child_res) < bridge_res->limit)
			bridge_res->limit = effective_limit(child_res);

		/*
		 * Alignment value of 0 means that the child resource has no alignment
		 * requirements and so the base value remains unchanged here.
		 */
		base = ALIGN_UP(base, POWER_OF_2(child_res->align));

		/*
		 * Store the relative offset inside the bridge resource for later
		 * consumption in allocate_bridge_resources(), and invalidate flags
		 * related to the base.
		 */
		child_res->base = base;
		child_res->flags &= ~(IORESOURCE_ASSIGNED | IORESOURCE_STORED);

		print_child_res(child, child_res, print_depth);

		base += child_res->size;
	}

	/*
	 * After all downstream device resources are scanned, `base` represents
	 * the total size requirement for the current bridge resource window.
	 * This size needs to be rounded up to the granularity requirement of
	 * the bridge to ensure that the upstream bridge/domain allocates big
	 * enough window.
	 */
	bridge_res->size = ALIGN_UP(base, POWER_OF_2(bridge_res->gran));

	print_bridge_res(bridge, bridge_res, print_depth, " done");
}

/*
 * During pass 1, at the bridge level, the resource allocator gathers
 * requirements from downstream devices and updates its own resource
 * windows for the provided resource type.
 */
static void compute_bridge_resources(const struct device *bridge, unsigned long type_match,
				     int print_depth)
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
			compute_bridge_resources(child, type_match, print_depth + 1);
		}

		/*
		 * Update the window for current bridge resource now that all downstream
		 * requirements are gathered.
		 */
		update_bridge_resource(bridge, res, print_depth);
	}
}

/*
 * During pass 1, the resource allocator walks down the entire sub-tree
 * of a domain. It gathers resource requirements for every downstream
 * bridge by looking at the resource requests of its children. Thus, the
 * requirement gathering begins at the leaf devices and is propagated
 * back up to the downstream bridges of the domain.
 *
 * At the domain level, it identifies every downstream bridge and walks
 * down that bridge to gather requirements for each resource type i.e.
 * i/o, mem and prefmem. Since bridges have separate windows for mem and
 * prefmem, requirements for each need to be collected separately.
 *
 * Domain resource windows are fixed ranges and hence requirement
 * gathering does not result in any changes to these fixed ranges.
 */
static void compute_domain_resources(const struct device *domain)
{
	const struct device *child;
	const int print_depth = 1;

	if (domain->link_list == NULL)
		return;

	for (child = domain->link_list->children; child; child = child->sibling) {

		/* Skip if this is not a bridge or has no children under it. */
		if (!dev_has_children(child))
			continue;

		compute_bridge_resources(child, IORESOURCE_IO, print_depth);
		compute_bridge_resources(child, IORESOURCE_MEM, print_depth);
		compute_bridge_resources(child, IORESOURCE_MEM | IORESOURCE_PREFETCH,
					 print_depth);
	}
}

/*
 * Scan the entire tree to identify any fixed resources allocated by
 * any device to ensure that the address map for domain resources are
 * appropriately updated.
 *
 * Domains can typically provide a memrange for entire address space.
 * So, this function punches holes in the address space for all fixed
 * resources that are already defined. Both I/O and normal memory
 * resources are added as fixed. Both need to be removed from address
 * space where dynamic resource allocations are sourced.
 */
static void avoid_fixed_resources(struct memranges *ranges, const struct device *dev,
				  unsigned long mask_match)
{
	const struct resource *res;
	const struct device *child;
	const struct bus *bus;

	for (res = dev->resource_list; res != NULL; res = res->next) {
		if ((res->flags & mask_match) != mask_match)
			continue;
		if (!res->size)
			continue;
		print_fixed_res(dev, res, __func__);
		memranges_create_hole(ranges, res->base, res->size);
	}

	bus = dev->link_list;
	if (bus == NULL)
		return;

	for (child = bus->children; child != NULL; child = child->sibling)
		avoid_fixed_resources(ranges, child, mask_match);
}

/*
 * This function creates a list of memranges of given type using the
 * resource that is provided. It applies additional constraints to
 * ensure that the memranges do not overlap any of the fixed resources
 * under the domain. The domain typically provides a memrange for the
 * entire address space. Thus, it is up to the chipset to add DRAM and
 * all other windows which cannot be used for resource allocation as
 * fixed resources.
 */
static void setup_resource_ranges(const struct device *const domain,
				  const unsigned long type,
				  struct memranges *const ranges)
{
	/* Align mem resources to 2^12 (4KiB pages) at a minimum, so they
	   can be memory-mapped individually (e.g. for virtualization guests). */
	const unsigned char alignment = type == IORESOURCE_MEM ? 12 : 0;
	const unsigned long type_mask = IORESOURCE_TYPE_MASK | IORESOURCE_FIXED;

	memranges_init_empty_with_alignment(ranges, NULL, 0, alignment);

	for (struct resource *res = domain->resource_list; res != NULL; res = res->next) {
		if ((res->flags & type_mask) != type)
			continue;
		print_domain_res(domain, res, "");
		memranges_insert(ranges, res->base, res->limit - res->base + 1, type);
	}

	if (type == IORESOURCE_IO) {
		/*
		 * Don't allow allocations in the VGA I/O range. PCI has special
		 * cases for that.
		 */
		memranges_create_hole(ranges, 0x3b0, 0x3df - 0x3b0 + 1);

		/*
		 * Resource allocator no longer supports the legacy behavior where
		 * I/O resource allocation is guaranteed to avoid aliases over legacy
		 * PCI expansion card addresses.
		 */
	}

	avoid_fixed_resources(ranges, domain, type | IORESOURCE_FIXED);

	print_resource_ranges(domain, ranges);
}

static void cleanup_domain_resource_ranges(const struct device *dev, struct memranges *ranges,
					   unsigned long type)
{
	memranges_teardown(ranges);
	for (struct resource *res = dev->resource_list; res != NULL; res = res->next) {
		if (res->flags & IORESOURCE_FIXED)
			continue;
		if ((res->flags & IORESOURCE_TYPE_MASK) != type)
			continue;
		print_domain_res(dev, res, " done");
	}
}

static void assign_resource(struct resource *const res, const resource_t base,
			    const struct device *const dev)
{
	res->base = base;
	res->limit = res->base + res->size - 1;
	res->flags |= IORESOURCE_ASSIGNED;
	res->flags &= ~IORESOURCE_STORED;

	print_assigned_res(dev, res);
}

/*
 * This is where the actual allocation of resources happens during
 * pass 2. We construct a list of memory ranges corresponding to the
 * resource of a given type, then look for the biggest unallocated
 * resource on the downstream bus. This continues in a descending order
 * until all resources of a given type have space allocated within the
 * domain's resource window.
 */
static void allocate_toplevel_resources(const struct device *const domain,
					const unsigned long type)
{
	const unsigned long type_mask = IORESOURCE_TYPE_MASK;
	struct resource *res = NULL;
	const struct device *dev;
	struct memranges ranges;
	resource_t base;

	if (!dev_has_children(domain))
		return;

	setup_resource_ranges(domain, type, &ranges);

	while ((dev = largest_resource(domain->link_list, &res, type_mask, type))) {

		if (!res->size)
			continue;

		if (!memranges_steal(&ranges, effective_limit(res), res->size, res->align,
				     type, &base, CONFIG(RESOURCE_ALLOCATION_TOP_DOWN))) {
			printk(BIOS_ERR, "Resource didn't fit!!!\n");
			print_failed_res(dev, res);
			continue;
		}

		assign_resource(res, base, dev);
	}

	cleanup_domain_resource_ranges(domain, &ranges, type);
}

/*
 * Pass 2 of the resource allocator at the bridge level loops through
 * all the resources for the bridge and assigns all the base addresses
 * of its children's resources of the same type. update_bridge_resource()
 * of pass 1 pre-calculated the offsets of these bases inside the bridge
 * resource. Now that the bridge resource is allocated, all we have to
 * do is to add its final base to these offsets.
 *
 * Once allocation at the current bridge is complete, resource allocator
 * continues walking down the downstream bridges until it hits the leaf
 * devices.
 */
static void assign_resource_cb(void *param, struct device *dev, struct resource *res)
{
	/* We have to filter the same resources as update_bridge_resource(). */
	if (!res->size || !res->limit)
		return;

	assign_resource(res, *(const resource_t *)param + res->base, dev);
}
static void allocate_bridge_resources(const struct device *bridge)
{
	const unsigned long type_mask =
		IORESOURCE_TYPE_MASK | IORESOURCE_PREFETCH | IORESOURCE_FIXED;
	struct bus *const bus = bridge->link_list;
	struct resource *res;
	struct device *child;

	for (res = bridge->resource_list; res != NULL; res = res->next) {
		if (!res->size)
			continue;

		if (!(res->flags & IORESOURCE_BRIDGE))
			continue;

		if (!(res->flags & IORESOURCE_ASSIGNED))
			continue;

		/* Run assign_resource_cb() for all downstream resources of the same type. */
		search_bus_resources(bus, type_mask, res->flags & type_mask,
				     assign_resource_cb, &res->base);
	}

	for (child = bus->children; child != NULL; child = child->sibling) {
		if (!dev_has_children(child))
			continue;

		allocate_bridge_resources(child);
	}
}

/*
 * Pass 2 of resource allocator begins at the domain level. Every domain
 * has two types of resources - io and mem. For each of these resources,
 * this function creates a list of memory ranges that can be used for
 * downstream resource allocation. This list is constrained to remove
 * any fixed resources in the domain sub-tree of the given resource
 * type. It then uses the memory ranges to apply best fit on the
 * resource requirements of the downstream devices.
 *
 * Once resources are allocated to all downstream devices of the domain,
 * it walks down each downstream bridge to finish resource assignment
 * of its children resources within its own window.
 */
static void allocate_domain_resources(const struct device *domain)
{
	/* Resource type I/O */
	allocate_toplevel_resources(domain, IORESOURCE_IO);

	/*
	 * Resource type Mem:
	 * Domain does not distinguish between mem and prefmem resources. Thus,
	 * the resource allocation at domain level considers mem and prefmem
	 * together when finding the best fit based on the biggest resource
	 * requirement.
	 */
	allocate_toplevel_resources(domain, IORESOURCE_MEM);

	struct device *child;
	for (child = domain->link_list->children; child; child = child->sibling) {
		if (!dev_has_children(child))
			continue;

		/* Continue allocation for all downstream bridges. */
		allocate_bridge_resources(child);
	}
}

/*
 * This function forms the guts of the resource allocator. It walks
 * through the entire device tree for each domain two times.
 *
 * Every domain has a fixed set of ranges. These ranges cannot be
 * relaxed based on the requirements of the downstream devices. They
 * represent the available windows from which resources can be allocated
 * to the different devices under the domain.
 *
 * In order to identify the requirements of downstream devices, resource
 * allocator walks in a DFS fashion. It gathers the requirements from
 * leaf devices and propagates those back up to their upstream bridges
 * until the requirements for all the downstream devices of the domain
 * are gathered. This is referred to as pass 1 of the resource allocator.
 *
 * Once the requirements for all the devices under the domain are
 * gathered, the resource allocator walks a second time to allocate
 * resources to downstream devices as per the requirements. It always
 * picks the biggest resource request as per the type (i/o and mem) to
 * allocate space from its fixed window to the immediate downstream
 * device of the domain. In order to accomplish best fit for the
 * resources, a list of ranges is maintained by each resource type (i/o
 * and mem). At the domain level we don't differentiate between mem and
 * prefmem. Since they are allocated space from the same window, the
 * resource allocator at the domain level ensures that the biggest
 * requirement is selected independent of the prefetch type. Once the
 * resource allocation for all immediate downstream devices is complete
 * at the domain level, the resource allocator walks down the subtree
 * for each downstream bridge to continue the allocation process at the
 * bridge level. Since bridges have either their whole window allocated
 * or nothing, we only need to place downstream resources inside these
 * windows by re-using offsets that were pre-calculated in pass 1. This
 * continues until resource allocation is realized for all downstream
 * bridges in the domain sub-tree. This is referred to as pass 2 of the
 * resource allocator.
 *
 * Some rules that are followed by the resource allocator:
 *  - Allocate resource locations for every device as long as
 *    the requirements can be satisfied.
 *  - Don't overlap with resources in fixed locations.
 *  - Don't overlap and follow the rules of bridges -- downstream
 *    devices of bridges should use parts of the address space
 *    allocated to the bridge.
 */
void allocate_resources(const struct device *root)
{
	const struct device *child;

	if ((root == NULL) || (root->link_list == NULL))
		return;

	for (child = root->link_list->children; child; child = child->sibling) {

		if (child->path.type != DEVICE_PATH_DOMAIN)
			continue;

		post_log_path(child);

		/* Pass 1 - Relative placement. */
		printk(BIOS_INFO, "=== Resource allocator: %s - Pass 1 (relative placement) ===\n",
		       dev_path(child));
		compute_domain_resources(child);

		/* Pass 2 - Allocate resources as per gathered requirements. */
		printk(BIOS_INFO, "=== Resource allocator: %s - Pass 2 (allocating resources) ===\n",
		       dev_path(child));
		allocate_domain_resources(child);

		printk(BIOS_INFO, "=== Resource allocator: %s - resource allocation complete ===\n",
		       dev_path(child));
	}
}
