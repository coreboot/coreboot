/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
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
	resource_t mask;
	mask = (1ULL << pow) - 1ULL;
	val += mask;
	val &= ~mask;
	return val;
}

static const char *resource2str(struct resource *res)
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
 * This function is the guts of the resource allocator.
 *
 * The problem.
 *  - Allocate resource locations for every device.
 *  - Don't overlap, and follow the rules of bridges.
 *  - Don't overlap with resources in fixed locations.
 *  - Be efficient so we don't have ugly strategies.
 *
 * The strategy.
 * - Devices that have fixed addresses are the minority so don't
 *   worry about them too much. Instead only use part of the address
 *   space for devices with programmable addresses. This easily handles
 *   everything except bridges.
 *
 * - PCI devices are required to have their sizes and their alignments
 *   equal. In this case an optimal solution to the packing problem
 *   exists. Allocate all devices from highest alignment to least
 *   alignment or vice versa. Use this.
 *
 * - So we can handle more than PCI run two allocation passes on bridges. The
 *   first to see how large the resources are behind the bridge, and what
 *   their alignment requirements are. The second to assign a safe address to
 *   the devices behind the bridge. This allows us to treat a bridge as just
 *   a device with a couple of resources, and not need to special case it in
 *   the allocator. Also this allows handling of other types of bridges.
 *
 * @param bus The bus we are traversing.
 * @param bridge The bridge resource which must contain the bus' resources.
 * @param type_mask This value gets ANDed with the resource type.
 * @param type This value must match the result of the AND.
 * @return TODO
 */
static void compute_resources(struct bus *bus, struct resource *bridge,
			      unsigned long type_mask, unsigned long type)
{
	const struct device *dev;
	struct resource *resource;
	resource_t base;
	base = round(bridge->base, bridge->align);

	if (!bus)
		return;

	printk(BIOS_SPEW,  "%s %s: base: %llx size: %llx align: %d gran: %d"
	       " limit: %llx\n", dev_path(bus->dev), resource2str(bridge),
	       base, bridge->size, bridge->align,
	       bridge->gran, bridge->limit);

	/* For each child which is a bridge, compute the resource needs. */
	for (dev = bus->children; dev; dev = dev->sibling) {
		struct resource *child_bridge;

		if (!dev->link_list)
			continue;

		/* Find the resources with matching type flags. */
		for (child_bridge = dev->resource_list; child_bridge;
		     child_bridge = child_bridge->next) {
			struct bus* link;

			if (!(child_bridge->flags & IORESOURCE_BRIDGE)
			    || (child_bridge->flags & type_mask) != type)
				continue;

			/*
			 * Split prefetchable memory if combined. Many domains
			 * use the same address space for prefetchable memory
			 * and non-prefetchable memory. Bridges below them need
			 * it separated. Add the PREFETCH flag to the type_mask
			 * and type.
			 */
			link = dev->link_list;
			while (link && link->link_num !=
					IOINDEX_LINK(child_bridge->index))
				link = link->next;

			if (link == NULL) {
				printk(BIOS_ERR, "link %ld not found on %s\n",
				       IOINDEX_LINK(child_bridge->index),
				       dev_path(dev));
			}

			compute_resources(link, child_bridge,
					  type_mask | IORESOURCE_PREFETCH,
					  type | (child_bridge->flags &
						  IORESOURCE_PREFETCH));
		}
	}

	/* Remember we haven't found anything yet. */
	resource = NULL;

	/*
	 * Walk through all the resources on the current bus and compute the
	 * amount of address space taken by them. Take granularity and
	 * alignment into account.
	 */
	while ((dev = largest_resource(bus, &resource, type_mask, type))) {

		/* Size 0 resources can be skipped. */
		if (!resource->size)
			continue;

		/* Propagate the resource alignment to the bridge resource. */
		if (resource->align > bridge->align)
			bridge->align = resource->align;

		/* Propagate the resource limit to the bridge register. */
		if (bridge->limit > resource->limit)
			bridge->limit = resource->limit;

		/* Warn if it looks like APICs aren't declared. */
		if ((resource->limit == 0xffffffff) &&
		    (resource->flags & IORESOURCE_ASSIGNED)) {
			printk(BIOS_ERR,
			       "Resource limit looks wrong! (no APIC?)\n");
			printk(BIOS_ERR, "%s %02lx limit %08llx\n",
			       dev_path(dev), resource->index, resource->limit);
		}

		if (resource->flags & IORESOURCE_IO) {
			/*
			 * Don't allow potential aliases over the legacy PCI
			 * expansion card addresses. The legacy PCI decodes
			 * only 10 bits, uses 0x100 - 0x3ff. Therefore, only
			 * 0x00 - 0xff can be used out of each 0x400 block of
			 * I/O space.
			 */
			if ((base & 0x300) != 0) {
				base = (base & ~0x3ff) + 0x400;
			}
			/*
			 * Don't allow allocations in the VGA I/O range.
			 * PCI has special cases for that.
			 */
			else if ((base >= 0x3b0) && (base <= 0x3df)) {
				base = 0x3e0;
			}
		}
		/* Base must be aligned. */
		base = round(base, resource->align);
		resource->base = base;
		base += resource->size;

		printk(BIOS_SPEW, "%s %02lx *  [0x%llx - 0x%llx] %s\n",
		       dev_path(dev), resource->index, resource->base,
		       resource->base + resource->size - 1,
		       resource2str(resource));
	}

	/*
	 * A PCI bridge resource does not need to be a power of two size, but
	 * it does have a minimum granularity. Round the size up to that
	 * minimum granularity so we know not to place something else at an
	 * address positively decoded by the bridge.
	 */
	bridge->size = round(base, bridge->gran) -
		       round(bridge->base, bridge->align);

	printk(BIOS_SPEW, "%s %s: base: %llx size: %llx align: %d gran: %d"
	       " limit: %llx done\n", dev_path(bus->dev),
	       resource2str(bridge),
	       base, bridge->size, bridge->align, bridge->gran, bridge->limit);
}

/**
 * This function is the second part of the resource allocator.
 *
 * See the compute_resources function for a more detailed explanation.
 *
 * This function assigns the resources a value.
 *
 * @param bus The bus we are traversing.
 * @param bridge The bridge resource which must contain the bus' resources.
 * @param type_mask This value gets ANDed with the resource type.
 * @param type This value must match the result of the AND.
 *
 * @see compute_resources
 */
static void __allocate_resources(struct bus *bus, struct resource *bridge,
			       unsigned long type_mask, unsigned long type)
{
	const struct device *dev;
	struct resource *resource;
	resource_t base;
	base = bridge->base;

	if (!bus)
		return;

	printk(BIOS_SPEW, "%s %s: base:%llx size:%llx align:%d gran:%d "
	       "limit:%llx\n", dev_path(bus->dev),
	       resource2str(bridge),
	       base, bridge->size, bridge->align, bridge->gran, bridge->limit);

	/* Remember we haven't found anything yet. */
	resource = NULL;

	/*
	 * Walk through all the resources on the current bus and allocate them
	 * address space.
	 */
	while ((dev = largest_resource(bus, &resource, type_mask, type))) {

		/* Propagate the bridge limit to the resource register. */
		if (resource->limit > bridge->limit)
			resource->limit = bridge->limit;

		/* Size 0 resources can be skipped. */
		if (!resource->size)
			continue;

		if (resource->flags & IORESOURCE_IO) {
			/*
			 * Don't allow potential aliases over the legacy PCI
			 * expansion card addresses. The legacy PCI decodes
			 * only 10 bits, uses 0x100 - 0x3ff. Therefore, only
			 * 0x00 - 0xff can be used out of each 0x400 block of
			 * I/O space.
			 */
			if ((base & 0x300) != 0) {
				base = (base & ~0x3ff) + 0x400;
			}
			/*
			 * Don't allow allocations in the VGA I/O range.
			 * PCI has special cases for that.
			 */
			else if ((base >= 0x3b0) && (base <= 0x3df)) {
				base = 0x3e0;
			}
		}

		if ((round(base, resource->align) + resource->size - 1) <=
		    resource->limit) {
			/* Base must be aligned. */
			base = round(base, resource->align);
			resource->base = base;
			resource->limit = resource->base + resource->size - 1;
			resource->flags |= IORESOURCE_ASSIGNED;
			resource->flags &= ~IORESOURCE_STORED;
			base += resource->size;
		} else {
			printk(BIOS_ERR, "!! Resource didn't fit !!\n");
			printk(BIOS_ERR, "   aligned base %llx size %llx "
			       "limit %llx\n", round(base, resource->align),
			       resource->size, resource->limit);
			printk(BIOS_ERR, "   %llx needs to be <= %llx "
			       "(limit)\n", (round(base, resource->align) +
				resource->size) - 1, resource->limit);
			printk(BIOS_ERR, "   %s%s %02lx *  [0x%llx - 0x%llx]"
			       " %s\n", (resource->flags & IORESOURCE_ASSIGNED)
			       ? "Assigned: " : "", dev_path(dev),
			       resource->index, resource->base,
			       resource->base + resource->size - 1,
			       resource2str(resource));
		}

		printk(BIOS_SPEW, "%s %02lx *  [0x%llx - 0x%llx] %s\n",
		       dev_path(dev), resource->index, resource->base,
		       resource->size ? resource->base + resource->size - 1 :
		       resource->base, resource2str(resource));
	}

	/*
	 * A PCI bridge resource does not need to be a power of two size, but
	 * it does have a minimum granularity. Round the size up to that
	 * minimum granularity so we know not to place something else at an
	 * address positively decoded by the bridge.
	 */

	bridge->flags |= IORESOURCE_ASSIGNED;

	printk(BIOS_SPEW, "%s %s: next_base: %llx size: %llx align: %d "
	       "gran: %d done\n", dev_path(bus->dev),
	       resource2str(bridge), base, bridge->size, bridge->align,
	       bridge->gran);

	/* For each child which is a bridge, __allocate_resources. */
	for (dev = bus->children; dev; dev = dev->sibling) {
		struct resource *child_bridge;

		if (!dev->link_list)
			continue;

		/* Find the resources with matching type flags. */
		for (child_bridge = dev->resource_list; child_bridge;
		     child_bridge = child_bridge->next) {
			struct bus* link;

			if (!(child_bridge->flags & IORESOURCE_BRIDGE) ||
			    (child_bridge->flags & type_mask) != type)
				continue;

			/*
			 * Split prefetchable memory if combined. Many domains
			 * use the same address space for prefetchable memory
			 * and non-prefetchable memory. Bridges below them need
			 * it separated. Add the PREFETCH flag to the type_mask
			 * and type.
			 */
			link = dev->link_list;
			while (link && link->link_num !=
			               IOINDEX_LINK(child_bridge->index))
				link = link->next;
			if (link == NULL)
				printk(BIOS_ERR, "link %ld not found on %s\n",
				       IOINDEX_LINK(child_bridge->index),
				       dev_path(dev));

			__allocate_resources(link, child_bridge,
					   type_mask | IORESOURCE_PREFETCH,
					   type | (child_bridge->flags &
						   IORESOURCE_PREFETCH));
		}
	}
}

static int resource_is(struct resource *res, u32 type)
{
	return (res->flags & IORESOURCE_TYPE_MASK) == type;
}

struct constraints {
	struct resource io, mem;
};

static struct resource *resource_limit(struct constraints *limits,
				       struct resource *res)
{
	struct resource *lim = NULL;

	/* MEM, or I/O - skip any others. */
	if (resource_is(res, IORESOURCE_MEM))
		lim = &limits->mem;
	else if (resource_is(res, IORESOURCE_IO))
		lim = &limits->io;

	return lim;
}

static void constrain_resources(const struct device *dev,
				struct constraints* limits)
{
	const struct device *child;
	struct resource *res;
	struct resource *lim;
	struct bus *link;

	/* Constrain limits based on the fixed resources of this device. */
	for (res = dev->resource_list; res; res = res->next) {
		if (!(res->flags & IORESOURCE_FIXED))
			continue;
		if (!res->size) {
			/* It makes no sense to have 0-sized, fixed resources.*/
			printk(BIOS_ERR, "skipping %s@%lx fixed resource, "
			       "size=0!\n", dev_path(dev), res->index);
			continue;
		}

		lim = resource_limit(limits, res);
		if (!lim)
			continue;

		/*
		 * Is it a fixed resource outside the current known region?
		 * If so, we don't have to consider it - it will be handled
		 * correctly and doesn't affect current region's limits.
		 */
		if (((res->base + res->size -1) < lim->base)
		    || (res->base > lim->limit))
			continue;

		printk(BIOS_SPEW, "%s: %s %02lx base %08llx limit %08llx %s (fixed)\n",
			__func__, dev_path(dev), res->index, res->base,
			res->base + res->size - 1, resource2str(res));

		/*
		 * Choose to be above or below fixed resources. This check is
		 * signed so that "negative" amounts of space are handled
		 * correctly.
		 */
		if ((signed long long)(lim->limit - (res->base + res->size -1))
		    > (signed long long)(res->base - lim->base))
			lim->base = res->base + res->size;
		else
			lim->limit = res->base -1;
	}

	/* Descend into every enabled child and look for fixed resources. */
	for (link = dev->link_list; link; link = link->next) {
		for (child = link->children; child; child = child->sibling) {
			if (child->enabled)
				constrain_resources(child, limits);
		}
	}
}

static void avoid_fixed_resources(const struct device *dev)
{
	struct constraints limits;
	struct resource *res;
	struct resource *lim;

	printk(BIOS_SPEW, "%s: %s\n", __func__, dev_path(dev));

	/* Initialize constraints to maximum size. */
	limits.io.base = 0;
	limits.io.limit = 0xffffffffffffffffULL;
	limits.mem.base = 0;
	limits.mem.limit = 0xffffffffffffffffULL;

	/* Constrain the limits to dev's initial resources. */
	for (res = dev->resource_list; res; res = res->next) {
		if ((res->flags & IORESOURCE_FIXED))
			continue;
		printk(BIOS_SPEW, "%s:@%s %02lx limit %08llx\n", __func__,
		       dev_path(dev), res->index, res->limit);

		lim = resource_limit(&limits, res);
		if (!lim)
			continue;

		if (res->base > lim->base)
			lim->base = res->base;
		if (res->limit < lim->limit)
			lim->limit = res->limit;
	}

	/* Look through the tree for fixed resources and update the limits. */
	constrain_resources(dev, &limits);

	/* Update dev's resources with new limits. */
	for (res = dev->resource_list; res; res = res->next) {
		if ((res->flags & IORESOURCE_FIXED))
			continue;

		lim = resource_limit(&limits, res);
		if (!lim)
			continue;

		/* Is the resource outside the limits? */
		if (lim->base > res->base)
			res->base = lim->base;
		if (res->limit > lim->limit)
			res->limit = lim->limit;

		/* MEM resources need to start at the highest address manageable. */
		if (res->flags & IORESOURCE_MEM)
			res->base = resource_max(res);

		printk(BIOS_SPEW, "%s:@%s %02lx base %08llx limit %08llx\n",
			__func__, dev_path(dev), res->index, res->base, res->limit);
	}
}

void allocate_resources(const struct device *root)
{
	struct resource *res;
	const struct device *child;

	/* Compute resources for all domains. */
	for (child = root->link_list->children; child; child = child->sibling) {
		if (!(child->path.type == DEVICE_PATH_DOMAIN))
			continue;
		post_log_path(child);
		for (res = child->resource_list; res; res = res->next) {
			if (res->flags & IORESOURCE_FIXED)
				continue;
			if (res->flags & IORESOURCE_MEM) {
				compute_resources(child->link_list,
						  res, IORESOURCE_TYPE_MASK, IORESOURCE_MEM);
				continue;
			}
			if (res->flags & IORESOURCE_IO) {
				compute_resources(child->link_list,
						  res, IORESOURCE_TYPE_MASK, IORESOURCE_IO);
				continue;
			}
		}
	}

	/* For all domains. */
	for (child = root->link_list->children; child; child=child->sibling)
		if (child->path.type == DEVICE_PATH_DOMAIN)
			avoid_fixed_resources(child);

	/* Store the computed resource allocations into device registers ... */
	printk(BIOS_INFO, "Setting resources...\n");
	for (child = root->link_list->children; child; child = child->sibling) {
		if (!(child->path.type == DEVICE_PATH_DOMAIN))
			continue;
		post_log_path(child);
		for (res = child->resource_list; res; res = res->next) {
			if (res->flags & IORESOURCE_FIXED)
				continue;
			if (res->flags & IORESOURCE_MEM) {
				__allocate_resources(child->link_list,
						     res, IORESOURCE_TYPE_MASK, IORESOURCE_MEM);
				continue;
			}
			if (res->flags & IORESOURCE_IO) {
				__allocate_resources(child->link_list,
						     res, IORESOURCE_TYPE_MASK, IORESOURCE_IO);
				continue;
			}
		}
	}
}
