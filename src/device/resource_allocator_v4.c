/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <memrange.h>
#include <post.h>

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

static bool dev_has_children(const struct device *dev)
{
	const struct bus *bus = dev->link_list;
	return bus && bus->children;
}

#define res_printk(depth, str, ...)	printk(BIOS_DEBUG, "%*c"str, depth, ' ', __VA_ARGS__)

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
 */
static void update_bridge_resource(const struct device *bridge, struct resource *bridge_res,
				   unsigned long type_match, int print_depth)
{
	const struct device *child;
	struct resource *child_res;
	resource_t base;
	bool first_child_res = true;
	const unsigned long type_mask = IORESOURCE_TYPE_MASK | IORESOURCE_PREFETCH;
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

	res_printk(print_depth, "%s %s: size: %llx align: %d gran: %d limit: %llx\n",
	       dev_path(bridge), resource2str(bridge_res), bridge_res->size,
	       bridge_res->align, bridge_res->gran, bridge_res->limit);

	while ((child = largest_resource(bus, &child_res, type_mask, type_match))) {

		/* Size 0 resources can be skipped. */
		if (!child_res->size)
			continue;

		/*
		 * Propagate the resource alignment to the bridge resource if this is
		 * the first child resource with non-zero size being considered. For all
		 * other children resources, alignment is taken care of by updating the
		 * base to round up as per the child resource alignment. It is
		 * guaranteed that pass 2 follows the exact same method of picking the
		 * resource for allocation using largest_resource(). Thus, as long as
		 * the alignment for first child resource is propagated up to the bridge
		 * resource, it can be guaranteed that the alignment for all resources
		 * is appropriately met.
		 */
		if (first_child_res && (child_res->align > bridge_res->align))
			bridge_res->align = child_res->align;

		first_child_res = false;

		/*
		 * Propagate the resource limit to the bridge resource only if child
		 * resource limit is non-zero. If a downstream device has stricter
		 * requirements w.r.t. limits for any resource, that constraint needs to
		 * be propagated back up to the downstream bridges of the domain. This
		 * guarantees that the resource allocation which starts at the domain
		 * level takes into account all these constraints thus working on a
		 * global view.
		 */
		if (child_res->limit && (child_res->limit < bridge_res->limit))
			bridge_res->limit = child_res->limit;

		/*
		 * Propagate the downstream resource request to allocate above 4G
		 * boundary to upstream bridge resource. This ensures that during
		 * pass 2, the resource allocator at domain level has a global view
		 * of all the downstream device requirements and thus address space
		 * is allocated as per updated flags in the bridge resource.
		 *
		 * Since the bridge resource is a single window, all the downstream
		 * resources of this bridge resource will be allocated in space above
		 * the 4G boundary.
		 */
		if (child_res->flags & IORESOURCE_ABOVE_4G)
			bridge_res->flags |= IORESOURCE_ABOVE_4G;

		/*
		 * Alignment value of 0 means that the child resource has no alignment
		 * requirements and so the base value remains unchanged here.
		 */
		base = round(base, child_res->align);

		res_printk(print_depth + 1, "%s %02lx *  [0x%llx - 0x%llx] %s\n",
		       dev_path(child), child_res->index, base, base + child_res->size - 1,
		       resource2str(child_res));

		base += child_res->size;
	}

	/*
	 * After all downstream device resources are scanned, `base` represents
	 * the total size requirement for the current bridge resource window.
	 * This size needs to be rounded up to the granularity requirement of
	 * the bridge to ensure that the upstream bridge/domain allocates big
	 * enough window.
	 */
	bridge_res->size = round(base, bridge_res->gran);

	res_printk(print_depth, "%s %s: size: %llx align: %d gran: %d limit: %llx done\n",
	       dev_path(bridge), resource2str(bridge_res), bridge_res->size,
	       bridge_res->align, bridge_res->gran, bridge_res->limit);
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
		update_bridge_resource(bridge, res, type_match, print_depth);
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

static unsigned char get_alignment_by_resource_type(const struct resource *res)
{
	if (res->flags & IORESOURCE_MEM)
		return 12;  /* Page-aligned --> log2(4KiB) */
	else if (res->flags & IORESOURCE_IO)
		return 0;   /* No special alignment required --> log2(1) */

	die("Unexpected resource type: flags(%d)!\n", res->flags);
}

/*
 * If the resource is NULL or if the resource is not assigned, then it
 * cannot be used for allocation for downstream devices.
 */
static bool is_resource_invalid(const struct resource *res)
{
	return (res == NULL) || !(res->flags & IORESOURCE_ASSIGNED);
}

static void initialize_domain_io_resource_memranges(struct memranges *ranges,
						    const struct resource *res,
						    unsigned long memrange_type)
{
	memranges_insert(ranges, res->base, res->limit - res->base + 1, memrange_type);
}

static void initialize_domain_mem_resource_memranges(struct memranges *ranges,
						     const struct resource *res,
						     unsigned long memrange_type)
{
	resource_t res_base;
	resource_t res_limit;

	const resource_t limit_4g = 0xffffffff;

	res_base = res->base;
	res_limit = res->limit;

	/*
	 * Split the resource into two separate ranges if it crosses the 4G
	 * boundary. Memrange type is set differently to ensure that memrange
	 * does not merge these two ranges. For the range above 4G boundary,
	 * given memrange type is ORed with IORESOURCE_ABOVE_4G.
	 */
	if (res_base <= limit_4g) {

		resource_t range_limit;

		/* Clip the resource limit at 4G boundary if necessary. */
		range_limit = MIN(res_limit, limit_4g);
		memranges_insert(ranges, res_base, range_limit - res_base + 1, memrange_type);

		/*
		 * If the resource lies completely below the 4G boundary, nothing more
		 * needs to be done.
		 */
		if (res_limit <= limit_4g)
			return;

		/*
		 * If the resource window crosses the 4G boundary, then update res_base
		 * to add another entry for the range above the boundary.
		 */
		res_base = limit_4g + 1;
	}

	if (res_base > res_limit)
		return;

	/*
	 * If resource lies completely above the 4G boundary or if the resource
	 * was clipped to add two separate ranges, the range above 4G boundary
	 * has the resource flag IORESOURCE_ABOVE_4G set. This allows domain to
	 * handle any downstream requests for resource allocation above 4G
	 * differently.
	 */
	memranges_insert(ranges, res_base, res_limit - res_base + 1,
			 memrange_type | IORESOURCE_ABOVE_4G);
}

/*
 * This function initializes memranges for domain device. If the
 * resource crosses 4G boundary, then this function splits it into two
 * ranges -- one for the window below 4G and the other for the window
 * above 4G. The latter range has IORESOURCE_ABOVE_4G flag set to
 * satisfy resource requests from downstream devices for allocations
 * above 4G.
 */
static void initialize_domain_memranges(struct memranges *ranges, const struct resource *res,
					unsigned long memrange_type)
{
	unsigned char align = get_alignment_by_resource_type(res);

	memranges_init_empty_with_alignment(ranges, NULL, 0, align);

	if (is_resource_invalid(res))
		return;

	if (res->flags & IORESOURCE_IO)
		initialize_domain_io_resource_memranges(ranges, res, memrange_type);
	else
		initialize_domain_mem_resource_memranges(ranges, res, memrange_type);
}

/*
 * This function initializes memranges for bridge device. Unlike domain,
 * bridge does not need to care about resource window crossing 4G
 * boundary. This is handled by the resource allocator at domain level
 * to ensure that all downstream bridges are allocated space either
 * above or below 4G boundary as per the state of IORESOURCE_ABOVE_4G
 * for the respective bridge resource.
 *
 * So, this function creates a single range of the entire resource
 * window available for the bridge resource. Thus all downstream
 * resources of the bridge for the given resource type get allocated
 * space from the same window. If there is any downstream resource of
 * the bridge which requests allocation above 4G, then all other
 * downstream resources of the same type under the bridge get allocated
 * above 4G.
 */
static void initialize_bridge_memranges(struct memranges *ranges, const struct resource *res,
					unsigned long memrange_type)
{
	unsigned char align = get_alignment_by_resource_type(res);

	memranges_init_empty_with_alignment(ranges, NULL, 0, align);

	if (is_resource_invalid(res))
		return;

	memranges_insert(ranges, res->base, res->limit - res->base + 1, memrange_type);
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

/*
 * This is where the actual allocation of resources happens during
 * pass 2. Given the list of memory ranges corresponding to the
 * resource of given type, it finds the biggest unallocated resource
 * using the type mask on the downstream bus. This continues in a
 * descending order until all resources of given type are allocated
 * address space within the current resource window.
 */
static void allocate_child_resources(struct bus *bus, struct memranges *ranges,
				     unsigned long type_mask, unsigned long type_match)
{
	struct resource *resource = NULL;
	const struct device *dev;

	while ((dev = largest_resource(bus, &resource, type_mask, type_match))) {

		if (!resource->size)
			continue;

		if (memranges_steal(ranges, resource->limit, resource->size, resource->align,
				    type_match, &resource->base) == false) {
			printk(BIOS_ERR, "  ERROR: Resource didn't fit!!! ");
			printk(BIOS_DEBUG, "  %s %02lx *  size: 0x%llx limit: %llx %s\n",
			       dev_path(dev), resource->index,
			       resource->size, resource->limit, resource2str(resource));
			continue;
		}

		resource->limit = resource->base + resource->size - 1;
		resource->flags |= IORESOURCE_ASSIGNED;

		printk(BIOS_DEBUG, "  %s %02lx *  [0x%llx - 0x%llx] limit: %llx %s\n",
		       dev_path(dev), resource->index, resource->base,
		       resource->size ? resource->base + resource->size - 1 :
		       resource->base, resource->limit, resource2str(resource));
	}
}

static void update_constraints(struct memranges *ranges, const struct device *dev,
			      const struct resource *res)
{
	if (!res->size)
		return;

	printk(BIOS_DEBUG, " %s: %s %02lx base %08llx limit %08llx %s (fixed)\n",
	       __func__, dev_path(dev), res->index, res->base,
	       res->base + res->size - 1, resource2str(res));

	memranges_create_hole(ranges, res->base, res->size);
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
		update_constraints(ranges, dev, res);
	}

	bus = dev->link_list;
	if (bus == NULL)
		return;

	for (child = bus->children; child != NULL; child = child->sibling)
		avoid_fixed_resources(ranges, child, mask_match);
}

static void constrain_domain_resources(const struct device *domain, struct memranges *ranges,
				       unsigned long type)
{
	unsigned long mask_match = type | IORESOURCE_FIXED;

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

	avoid_fixed_resources(ranges, domain, mask_match);
}

/*
 * This function creates a list of memranges of given type using the
 * resource that is provided. If the given resource is NULL or if the
 * resource window size is 0, then it creates an empty list. This
 * results in resource allocation for that resource type failing for
 * all downstream devices since there is nothing to allocate from.
 *
 * In case of domain, it applies additional constraints to ensure that
 * the memranges do not overlap any of the fixed resources under that
 * domain. Domain typically seems to provide memrange for entire address
 * space. Thus, it is up to the chipset to add DRAM and all other
 * windows which cannot be used for resource allocation as fixed
 * resources.
 */
static void setup_resource_ranges(const struct device *dev, const struct resource *res,
				  unsigned long type, struct memranges *ranges)
{
	printk(BIOS_DEBUG, "%s %s: base: %llx size: %llx align: %d gran: %d limit: %llx\n",
	       dev_path(dev), resource2str(res), res->base, res->size, res->align,
	       res->gran, res->limit);

	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		initialize_domain_memranges(ranges, res, type);
		constrain_domain_resources(dev, ranges, type);
	} else {
		initialize_bridge_memranges(ranges, res, type);
	}

	print_resource_ranges(dev, ranges);
}

static void cleanup_resource_ranges(const struct device *dev, struct memranges *ranges,
				    const struct resource *res)
{
	memranges_teardown(ranges);
	printk(BIOS_DEBUG, "%s %s: base: %llx size: %llx align: %d gran: %d limit: %llx done\n",
	       dev_path(dev), resource2str(res), res->base, res->size, res->align,
	       res->gran, res->limit);
}

/*
 * Pass 2 of the resource allocator at the bridge level loops through
 * all the resources for the bridge and generates a list of memory
 * ranges similar to that at the domain level. However, there is no need
 * to apply any additional constraints since the window allocated to the
 * bridge is guaranteed to be non-overlapping by the allocator at domain
 * level.
 *
 * Allocation at the bridge level works the same as at domain level
 * (starts with the biggest resource requirement from downstream devices
 * and continues in descending order). One major difference at the
 * bridge level is that it considers prefmem resources separately from
 * mem resources.
 *
 * Once allocation at the current bridge is complete, resource allocator
 * continues walking down the downstream bridges until it hits the leaf
 * devices.
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
 * Pass 2 of resource allocator begins at the domain level. Every domain
 * has two types of resources - io and mem. For each of these resources,
 * this function creates a list of memory ranges that can be used for
 * downstream resource allocation. This list is constrained to remove
 * any fixed resources in the domain sub-tree of the given resource
 * type. It then uses the memory ranges to apply best fit on the
 * resource requirements of the downstream devices.
 *
 * Once resources are allocated to all downstream devices of the domain,
 * it walks down each downstream bridge to continue the same process
 * until resources are allocated to all devices under the domain.
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
	 * Domain does not distinguish between mem and prefmem resources. Thus,
	 * the resource allocation at domain level considers mem and prefmem
	 * together when finding the best fit based on the biggest resource
	 * requirement.
	 *
	 * However, resource requests for allocation above 4G boundary need to
	 * be handled separately if the domain resource window crosses this
	 * boundary. There is a single window for resource of type
	 * IORESOURCE_MEM. When creating memranges, this resource is split into
	 * two separate ranges -- one for the window below 4G boundary and other
	 * for the window above 4G boundary (with IORESOURCE_ABOVE_4G flag set).
	 * Thus, when allocating child resources, requests for below and above
	 * the 4G boundary are handled separately by setting the type_mask and
	 * type_match to allocate_child_resources() accordingly.
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
 * bridge level. Since bridges have separate windows for i/o, mem and
 * prefmem, best fit algorithm at bridge level looks for the biggest
 * requirement considering prefmem resources separately from non-prefmem
 * resources. This continues until resource allocation is performed for
 * all downstream bridges in the domain sub-tree. This is referred to as
 * pass 2 of the resource allocator.
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

		/* Pass 1 - Gather requirements. */
		printk(BIOS_INFO, "=== Resource allocator: %s - Pass 1 (gathering requirements) ===\n",
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
