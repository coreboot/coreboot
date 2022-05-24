/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci_def.h>
#include <device/resource.h>
#include <stdlib.h>
#include <string.h>

/**
 * Given a Local APIC ID, find the device structure.
 *
 * @param apic_id The Local APIC ID number.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */
struct device *dev_find_lapic(unsigned int apic_id)
{
	struct device *dev;
	struct device *result = NULL;

	for (dev = all_devices; dev; dev = dev->next) {
		if (dev->path.type == DEVICE_PATH_APIC &&
		    dev->path.apic.apic_id == apic_id) {
			result = dev;
			break;
		}
	}
	return result;
}

/**
 * Find a device of a given vendor and type.
 *
 * @param vendor A PCI vendor ID (e.g. 0x8086 for Intel).
 * @param device A PCI device ID.
 * @param from Pointer to the device structure, used as a starting point in
 *             the linked list of all_devices, which can be 0 to start at the
 *             head of the list (i.e. all_devices).
 * @return Pointer to the device struct.
 */
struct device *dev_find_device(u16 vendor, u16 device, struct device *from)
{
	if (!from)
		from = all_devices;
	else
		from = from->next;

	while (from && (from->vendor != vendor || from->device != device))
		from = from->next;

	return from;
}

/**
 * Find a device of a given class.
 *
 * @param class Class of the device.
 * @param from Pointer to the device structure, used as a starting point in
 *             the linked list of all_devices, which can be 0 to start at the
 *             head of the list (i.e. all_devices).
 * @return Pointer to the device struct.
 */
struct device *dev_find_class(unsigned int class, struct device *from)
{
	if (!from)
		from = all_devices;
	else
		from = from->next;

	while (from && (from->class & 0xffffff00) != class)
		from = from->next;

	return from;
}

/**
 * Encode the device path into 3 bytes for logging to CMOS.
 *
 * @param dev The device path to encode.
 * @return Device path encoded into lower 3 bytes of dword.
 */
u32 dev_path_encode(const struct device *dev)
{
	u32 ret;

	if (!dev)
		return 0;

	/* Store the device type in 3rd byte. */
	ret = dev->path.type << 16;

	/* Encode the device specific path in the low word. */
	switch (dev->path.type) {
	case DEVICE_PATH_ROOT:
		break;
	case DEVICE_PATH_PCI:
		ret |= dev->bus->secondary << 8 | dev->path.pci.devfn;
		break;
	case DEVICE_PATH_PNP:
		ret |= dev->path.pnp.port << 8 | dev->path.pnp.device;
		break;
	case DEVICE_PATH_I2C:
		ret |= dev->path.i2c.mode_10bit << 8 | dev->path.i2c.device;
		break;
	case DEVICE_PATH_APIC:
		ret |= dev->path.apic.apic_id;
		break;
	case DEVICE_PATH_DOMAIN:
		ret |= dev->path.domain.domain;
		break;
	case DEVICE_PATH_CPU_CLUSTER:
		ret |= dev->path.cpu_cluster.cluster;
		break;
	case DEVICE_PATH_CPU:
		ret |= dev->path.cpu.id;
		break;
	case DEVICE_PATH_CPU_BUS:
		ret |= dev->path.cpu_bus.id;
		break;
	case DEVICE_PATH_IOAPIC:
		ret |= dev->path.ioapic.ioapic_id;
		break;
	case DEVICE_PATH_GENERIC:
		ret |= dev->path.generic.subid << 8 | dev->path.generic.id;
		break;
	case DEVICE_PATH_SPI:
		ret |= dev->path.spi.cs;
		break;
	case DEVICE_PATH_USB:
		ret |= dev->path.usb.port_type << 8 | dev->path.usb.port_id;
		break;
	case DEVICE_PATH_GPIO:
		ret |= dev->path.gpio.id;
		break;
	case DEVICE_PATH_NONE:
	case DEVICE_PATH_MMIO:  /* don't care */
	default:
		break;
	}

	return ret;
}

/*
 * Warning: This function uses a static buffer. Don't call it more than once
 * from the same print statement!
 */
const char *dev_path(const struct device *dev)
{
	static char buffer[DEVICE_PATH_MAX];

	buffer[0] = '\0';
	if (!dev) {
		strcpy(buffer, "<null>");
	} else {
		switch (dev->path.type) {
		case DEVICE_PATH_NONE:
			strcpy(buffer, "NONE");
			break;
		case DEVICE_PATH_ROOT:
			strcpy(buffer, "Root Device");
			break;
		case DEVICE_PATH_PCI:
			snprintf(buffer, sizeof(buffer),
				 "PCI: %02x:%02x.%01x",
				 dev->bus->secondary,
				 PCI_SLOT(dev->path.pci.devfn),
				 PCI_FUNC(dev->path.pci.devfn));
			break;
		case DEVICE_PATH_PNP:
			snprintf(buffer, sizeof(buffer), "PNP: %04x.%01x",
				 dev->path.pnp.port, dev->path.pnp.device);
			break;
		case DEVICE_PATH_I2C:
			snprintf(buffer, sizeof(buffer), "I2C: %02x:%02x",
				 dev->bus->secondary,
				 dev->path.i2c.device);
			break;
		case DEVICE_PATH_APIC:
			snprintf(buffer, sizeof(buffer), "APIC: %02x",
				 dev->path.apic.apic_id);
			break;
		case DEVICE_PATH_IOAPIC:
			snprintf(buffer, sizeof(buffer), "IOAPIC: %02x",
				 dev->path.ioapic.ioapic_id);
			break;
		case DEVICE_PATH_DOMAIN:
			snprintf(buffer, sizeof(buffer), "DOMAIN: %04x",
				dev->path.domain.domain);
			break;
		case DEVICE_PATH_CPU_CLUSTER:
			snprintf(buffer, sizeof(buffer), "CPU_CLUSTER: %01x",
				dev->path.cpu_cluster.cluster);
			break;
		case DEVICE_PATH_CPU:
			snprintf(buffer, sizeof(buffer),
				 "CPU: %02x", dev->path.cpu.id);
			break;
		case DEVICE_PATH_CPU_BUS:
			snprintf(buffer, sizeof(buffer),
				 "CPU_BUS: %02x", dev->path.cpu_bus.id);
			break;
		case DEVICE_PATH_GENERIC:
			snprintf(buffer, sizeof(buffer),
				 "GENERIC: %d.%d", dev->path.generic.id,
				 dev->path.generic.subid);
			break;
		case DEVICE_PATH_SPI:
			snprintf(buffer, sizeof(buffer), "SPI: %02x",
				 dev->path.spi.cs);
			break;
		case DEVICE_PATH_USB:
			snprintf(buffer, sizeof(buffer), "USB%u port %u",
				 dev->path.usb.port_type, dev->path.usb.port_id);
			break;
		case DEVICE_PATH_MMIO:
			snprintf(buffer, sizeof(buffer), "MMIO: %08lx",
				 dev->path.mmio.addr);
			break;
		case DEVICE_PATH_GPIO:
			snprintf(buffer, sizeof(buffer), "GPIO: %d", dev->path.gpio.id);
			break;
		default:
			printk(BIOS_ERR, "Unknown device path type: %d\n",
			       dev->path.type);
			break;
		}
	}
	return buffer;
}

const char *dev_name(const struct device *dev)
{
	if (dev->name)
		return dev->name;
	else if (dev->chip_ops && dev->chip_ops->name)
		return dev->chip_ops->name;
	else
		return "unknown";
}

const char *bus_path(struct bus *bus)
{
	static char buffer[BUS_PATH_MAX];
	snprintf(buffer, sizeof(buffer),
		 "%s,%d", dev_path(bus->dev), bus->link_num);
	return buffer;
}

/**
 * Allocate 64 more resources to the free list.
 *
 * @return TODO.
 */
static int allocate_more_resources(void)
{
	int i;
	struct resource *new_res_list;

	new_res_list = malloc(64 * sizeof(*new_res_list));

	if (new_res_list == NULL)
		return 0;

	memset(new_res_list, 0, 64 * sizeof(*new_res_list));

	for (i = 0; i < 64 - 1; i++)
		new_res_list[i].next = &new_res_list[i+1];

	free_resources = new_res_list;
	return 1;
}

/**
 * Remove resource res from the device's list and add it to the free list.
 *
 * @param dev TODO
 * @param res TODO
 * @param prev TODO
 * @return TODO.
 */
static void free_resource(struct device *dev, struct resource *res,
			  struct resource *prev)
{
	if (prev)
		prev->next = res->next;
	else
		dev->resource_list = res->next;

	res->next = free_resources;
	free_resources = res;
}

/**
 * See if we have unused but allocated resource structures.
 *
 * If so remove the allocation.
 *
 * @param dev The device to find the resource on.
 */
void compact_resources(struct device *dev)
{
	struct resource *res, *next, *prev = NULL;

	/* Move all of the free resources to the end */
	for (res = dev->resource_list; res; res = next) {
		next = res->next;
		if (!res->flags)
			free_resource(dev, res, prev);
		else
			prev = res;
	}
}

/**
 * See if a resource structure already exists for a given index.
 *
 * @param dev The device to find the resource on.
 * @param index The index of the resource on the device.
 * @return The resource, if it already exists.
 */
struct resource *probe_resource(const struct device *dev, unsigned int index)
{
	struct resource *res;

	/* See if there is a resource with the appropriate index */
	for (res = dev->resource_list; res; res = res->next) {
		if (res->index == index)
			break;
	}

	return res;
}

/**
 * See if a resource structure already exists for a given index and if not
 * allocate one.
 *
 * Then initialize the resource to default values.
 *
 * @param dev The device to find the resource on.
 * @param index The index of the resource on the device.
 * @return TODO.
 */
struct resource *new_resource(struct device *dev, unsigned int index)
{
	struct resource *resource, *tail;

	/* First move all of the free resources to the end. */
	compact_resources(dev);

	/* See if there is a resource with the appropriate index. */
	resource = probe_resource(dev, index);
	if (!resource) {
		if (free_resources == NULL && !allocate_more_resources())
			die("Couldn't allocate more resources.");

		resource = free_resources;
		free_resources = free_resources->next;
		memset(resource, 0, sizeof(*resource));
		resource->next = NULL;
		tail = dev->resource_list;
		if (tail) {
			while (tail->next)
				tail = tail->next;
			tail->next = resource;
		} else {
			dev->resource_list = resource;
		}
	}

	/* Initialize the resource values. */
	if (!(resource->flags & IORESOURCE_FIXED)) {
		resource->flags = 0;
		resource->base = 0;
	}
	resource->size  = 0;
	resource->limit = 0;
	resource->index = index;
	resource->align = 0;
	resource->gran  = 0;

	return resource;
}

/**
 * Return an existing resource structure for a given index.
 *
 * @param dev The device to find the resource on.
 * @param index The index of the resource on the device.
 * return TODO.
 */
struct resource *find_resource(const struct device *dev, unsigned int index)
{
	struct resource *resource;

	/* See if there is a resource with the appropriate index. */
	resource = probe_resource(dev, index);
	if (!resource) {
		printk(BIOS_EMERG, "%s missing resource: %02x\n",
		       dev_path(dev), index);
		die("");
	}
	return resource;
}

/**
 * Round a number up to the next multiple of gran.
 *
 * @param val The starting value.
 * @param gran Granularity we are aligning the number to.
 * @return The aligned value.
 */
static resource_t align_up(resource_t val, unsigned long gran)
{
	resource_t mask;
	mask = (1ULL << gran) - 1ULL;
	val += mask;
	val &= ~mask;
	return val;
}

/**
 * Round a number up to the previous multiple of gran.
 *
 * @param val The starting value.
 * @param gran Granularity we are aligning the number to.
 * @return The aligned value.
 */
static resource_t align_down(resource_t val, unsigned long gran)
{
	resource_t mask;
	mask = (1ULL << gran) - 1ULL;
	val &= ~mask;
	return val;
}

/**
 * Compute the maximum address that is part of a resource.
 *
 * @param resource The resource whose limit is desired.
 * @return The end.
 */
resource_t resource_end(const struct resource *resource)
{
	resource_t base, end;

	/* Get the base address. */
	base = resource->base;

	/*
	 * For a non bridge resource granularity and alignment are the same.
	 * For a bridge resource align is the largest needed alignment below
	 * the bridge. While the granularity is simply how many low bits of
	 * the address cannot be set.
	 */

	/* Get the end (rounded up). */
	end = base + align_up(resource->size, resource->gran) - 1;

	return end;
}

/**
 * Compute the maximum legal value for resource->base.
 *
 * @param resource The resource whose maximum is desired.
 * @return The maximum.
 */
resource_t resource_max(const struct resource *resource)
{
	resource_t max;

	max = align_down(resource->limit - resource->size + 1, resource->align);

	return max;
}

/**
 * Return the resource type of a resource.
 *
 * @param resource The resource type to decode.
 * @return TODO.
 */
const char *resource_type(const struct resource *resource)
{
	static char buffer[RESOURCE_TYPE_MAX];
	snprintf(buffer, sizeof(buffer), "%s%s%s%s",
		 ((resource->flags & IORESOURCE_READONLY) ? "ro" : ""),
		 ((resource->flags & IORESOURCE_PREFETCH) ? "pref" : ""),
		 ((resource->flags == 0) ? "unused" :
		  (resource->flags & IORESOURCE_IO) ? "io" :
		  (resource->flags & IORESOURCE_DRQ) ? "drq" :
		  (resource->flags & IORESOURCE_IRQ) ? "irq" :
		  (resource->flags & IORESOURCE_MEM) ? "mem" : "??????"),
		 ((resource->flags & IORESOURCE_PCI64) ? "64" : ""));
	return buffer;
}

/**
 * Print the resource that was just stored.
 *
 * @param dev The device the stored resource lives on.
 * @param resource The resource that was just stored.
 * @param comment TODO
 */
void report_resource_stored(struct device *dev, const struct resource *resource,
			    const char *comment)
{
	char buf[10];
	unsigned long long base, end;

	if (!(resource->flags & IORESOURCE_STORED))
		return;

	base = resource->base;
	end = resource_end(resource);
	buf[0] = '\0';

	if (dev->link_list && (resource->flags & IORESOURCE_PCI_BRIDGE)) {
		snprintf(buf, sizeof(buf),
			 "bus %02x ", dev->link_list->secondary);
	}
	printk(BIOS_DEBUG, "%s %02lx <- [0x%010llx - 0x%010llx] size 0x%08llx "
	       "gran 0x%02x %s%s%s\n", dev_path(dev), resource->index,
		base, end, resource->size, resource->gran, buf,
		resource_type(resource), comment);
}

void search_bus_resources(struct bus *bus, unsigned long type_mask,
			  unsigned long type, resource_search_t search,
			  void *gp)
{
	struct device *curdev;

	for (curdev = bus->children; curdev; curdev = curdev->sibling) {
		struct resource *res;

		/* Ignore disabled devices. */
		if (!curdev->enabled)
			continue;

		for (res = curdev->resource_list; res; res = res->next) {
			/* If it isn't the right kind of resource ignore it. */
			if ((res->flags & type_mask) != type)
				continue;

			/* If it is a subtractive resource recurse. */
			if (res->flags & IORESOURCE_SUBTRACTIVE) {
				struct bus *subbus;
				for (subbus = curdev->link_list; subbus;
				     subbus = subbus->next)
					if (subbus->link_num
					== IOINDEX_SUBTRACTIVE_LINK(res->index))
						break;
				if (!subbus) /* Why can subbus be NULL?  */
					break;
				search_bus_resources(subbus, type_mask, type,
						     search, gp);
				continue;
			}
			search(gp, curdev, res);
		}
	}
}

void search_global_resources(unsigned long type_mask, unsigned long type,
			     resource_search_t search, void *gp)
{
	struct device *curdev;

	for (curdev = all_devices; curdev; curdev = curdev->next) {
		struct resource *res;

		/* Ignore disabled devices. */
		if (!curdev->enabled)
			continue;

		for (res = curdev->resource_list; res; res = res->next) {
			/* If it isn't the right kind of resource ignore it. */
			if ((res->flags & type_mask) != type)
				continue;

			/* If it is a subtractive resource ignore it. */
			if (res->flags & IORESOURCE_SUBTRACTIVE)
				continue;

			search(gp, curdev, res);
		}
	}
}

void dev_set_enabled(struct device *dev, int enable)
{
	if (dev->enabled == enable)
		return;

	dev->enabled = enable;
	if (dev->ops && dev->ops->enable)
		dev->ops->enable(dev);
	else if (dev->chip_ops && dev->chip_ops->enable_dev)
		dev->chip_ops->enable_dev(dev);
}

void disable_children(struct bus *bus)
{
	struct device *child;

	for (child = bus->children; child; child = child->sibling) {
		struct bus *link;
		for (link = child->link_list; link; link = link->next)
			disable_children(link);
		dev_set_enabled(child, 0);
	}
}

/*
 * Returns true if the device is an enabled bridge that has at least
 * one enabled device on its secondary bus that is not of type NONE.
 */
bool dev_is_active_bridge(struct device *dev)
{
	struct bus *link;
	struct device *child;

	if (!dev || !dev->enabled)
		return 0;

	if (!dev->link_list || !dev->link_list->children)
		return 0;

	for (link = dev->link_list; link; link = link->next) {
		for (child = link->children; child; child = child->sibling) {
			if (child->path.type == DEVICE_PATH_NONE)
				continue;

			if (child->enabled)
				return 1;
		}
	}

	return 0;
}

/**
 * Ensure the device has a minimum number of bus links.
 *
 * @param dev The device to add links to.
 * @param total_links The minimum number of links to have.
 */
void add_more_links(struct device *dev, unsigned int total_links)
{
	struct bus *link, *last = NULL;
	int link_num = -1;

	for (link = dev->link_list; link; link = link->next) {
		if (link_num < link->link_num)
			link_num = link->link_num;
		last = link;
	}

	if (last) {
		int links = total_links - (link_num + 1);
		if (links > 0) {
			link = malloc(links * sizeof(*link));
			if (!link)
				die("Couldn't allocate more links!\n");
			memset(link, 0, links * sizeof(*link));
			last->next = link;
		} else {
			/* No more links to add */
			return;
		}
	} else {
		link = malloc(total_links * sizeof(*link));
		if (!link)
			die("Couldn't allocate more links!\n");
		memset(link, 0, total_links * sizeof(*link));
		dev->link_list = link;
	}

	for (link_num = link_num + 1; link_num < total_links; link_num++) {
		link->link_num = link_num;
		link->dev = dev;
		link->next = link + 1;
		last = link;
		link = link->next;
	}
	last->next = NULL;
}

static void resource_tree(const struct device *root, int debug_level, int depth)
{
	int i = 0;
	struct device *child;
	struct bus *link;
	struct resource *res;
	char indent[30];	/* If your tree has more levels, it's wrong. */

	for (i = 0; i < depth + 1 && i < 29; i++)
		indent[i] = ' ';
	indent[i] = '\0';

	printk(BIOS_DEBUG, "%s%s", indent, dev_path(root));
	if (root->link_list && root->link_list->children)
		printk(BIOS_DEBUG, " child on link 0 %s",
			  dev_path(root->link_list->children));
	printk(BIOS_DEBUG, "\n");

	for (res = root->resource_list; res; res = res->next) {
		printk(debug_level, "%s%s resource base %llx size %llx "
			  "align %d gran %d limit %llx flags %lx index %lx\n",
			  indent, dev_path(root), res->base, res->size,
			  res->align, res->gran, res->limit, res->flags,
			  res->index);
	}

	for (link = root->link_list; link; link = link->next) {
		for (child = link->children; child; child = child->sibling)
			resource_tree(child, debug_level, depth + 1);
	}
}

void print_resource_tree(const struct device *root, int debug_level,
			 const char *msg)
{
	/* Bail if root is null. */
	if (!root) {
		printk(debug_level, "%s passed NULL for root!\n", __func__);
		return;
	}

	/* Bail if not printing to screen. */
	if (!printk(debug_level, "Show resources in subtree (%s)...%s\n",
		       dev_path(root), msg))
		return;

	resource_tree(root, debug_level, 0);
}

void show_devs_tree(const struct device *dev, int debug_level, int depth)
{
	char depth_str[20];
	int i;
	struct device *sibling;
	struct bus *link;

	for (i = 0; i < depth; i++)
		depth_str[i] = ' ';
	depth_str[i] = '\0';

	printk(debug_level, "%s%s: enabled %d\n",
		  depth_str, dev_path(dev), dev->enabled);

	for (link = dev->link_list; link; link = link->next) {
		for (sibling = link->children; sibling;
		     sibling = sibling->sibling)
			show_devs_tree(sibling, debug_level, depth + 1);
	}
}

void show_all_devs_tree(int debug_level, const char *msg)
{
	/* Bail if not printing to screen. */
	if (!printk(debug_level, "Show all devs in tree form... %s\n", msg))
		return;
	show_devs_tree(all_devices, debug_level, 0);
}

void show_devs_subtree(struct device *root, int debug_level, const char *msg)
{
	/* Bail if not printing to screen. */
	if (!printk(debug_level, "Show all devs in subtree %s... %s\n",
		       dev_path(root), msg))
		return;
	printk(debug_level, "%s\n", msg);
	show_devs_tree(root, debug_level, 0);
}

void show_all_devs(int debug_level, const char *msg)
{
	struct device *dev;

	/* Bail if not printing to screen. */
	if (!printk(debug_level, "Show all devs... %s\n", msg))
		return;
	for (dev = all_devices; dev; dev = dev->next) {
		printk(debug_level, "%s: enabled %d\n",
			  dev_path(dev), dev->enabled);
	}
}

void show_one_resource(int debug_level, struct device *dev,
		       struct resource *resource, const char *comment)
{
	char buf[10];
	unsigned long long base, end;
	base = resource->base;
	end = resource_end(resource);
	buf[0] = '\0';

	printk(debug_level, "%s %02lx <- [0x%010llx - 0x%010llx] "
		  "size 0x%08llx gran 0x%02x %s%s%s\n", dev_path(dev),
		  resource->index, base, end, resource->size, resource->gran,
		  buf, resource_type(resource), comment);
}

void show_all_devs_resources(int debug_level, const char *msg)
{
	struct device *dev;

	if (!printk(debug_level, "Show all devs with resources... %s\n", msg))
		return;

	for (dev = all_devices; dev; dev = dev->next) {
		struct resource *res;
		printk(debug_level, "%s: enabled %d\n",
			  dev_path(dev), dev->enabled);
		for (res = dev->resource_list; res; res = res->next)
			show_one_resource(debug_level, dev, res, "");
	}
}

void fixed_mem_resource_kb(struct device *dev, unsigned long index,
			unsigned long basek, unsigned long sizek,
			unsigned long type)
{
	struct resource *resource;

	if (!sizek)
		return;

	resource = new_resource(dev, index);
	resource->base = ((resource_t)basek) << 10;
	resource->size = ((resource_t)sizek) << 10;
	resource->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
		 IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	resource->flags |= type;
}

void fixed_io_resource(struct device *dev, unsigned long index,
			unsigned long base, unsigned long size)
{
	struct resource *resource;

	resource = new_resource(dev, index);
	resource->base = (resource_t)base;
	resource->size = (resource_t)size;
	resource->limit = resource->base + resource->size - 1;
	resource->flags = IORESOURCE_IO | IORESOURCE_FIXED |
		 IORESOURCE_STORED | IORESOURCE_ASSIGNED |
		 IORESOURCE_RESERVE;
}

void mmconf_resource(struct device *dev, unsigned long index)
{
	struct resource *resource = new_resource(dev, index);
	resource->base = CONFIG_ECAM_MMCONF_BASE_ADDRESS;
	resource->size = CONFIG_ECAM_MMCONF_LENGTH;
	resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	printk(BIOS_DEBUG, "Adding PCIe enhanced config space BAR 0x%08lx-0x%08lx.\n",
			(unsigned long)(resource->base),
			(unsigned long)(resource->base + resource->size));
}

void tolm_test(void *gp, struct device *dev, struct resource *new)
{
	struct resource **best_p = gp;
	struct resource *best;

	best = *best_p;

	/*
	 * If resource is not allocated any space i.e. size is zero,
	 * then do not consider this resource in tolm calculations.
	 */
	if (new->size == 0)
		return;

	if (!best || (best->base > new->base))
		best = new;

	*best_p = best;
}

u32 find_pci_tolm(struct bus *bus)
{
	struct resource *min = NULL;
	u32 tolm;
	unsigned long mask_match = IORESOURCE_MEM | IORESOURCE_ASSIGNED;

	search_bus_resources(bus, mask_match, mask_match, tolm_test, &min);

	tolm = 0xffffffffUL;

	if (min && tolm > min->base)
		tolm = min->base;

	return tolm;
}

/* Count of enabled CPUs */
int dev_count_cpu(void)
{
	struct device *cpu;
	int count = 0;

	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
		    (cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER))
			continue;
		if (!cpu->enabled)
			continue;
		count++;
	}

	return count;
}

/* Get device path name */
const char *dev_path_name(enum device_path_type type)
{
	static const char *const type_names[] = DEVICE_PATH_NAMES;
	const char *type_name = "Unknown";

	/* Translate the type value into a string */
	if (type < ARRAY_SIZE(type_names))
		type_name = type_names[type];
	return type_name;
}

void log_resource(const char *type, const struct device *dev, const struct resource *res,
			const char *srcfile, const int line)
{
	printk(BIOS_SPEW, "%s:%d res: %s, dev: %s, index: 0x%lx, base: 0x%llx, "
			  "end: 0x%llx, size_kb: 0x%llx\n",
			  srcfile, line, type, dev_path(dev), res->index, res->base,
			  resource_end(res), res->size / KiB);
}
