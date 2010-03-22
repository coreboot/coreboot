/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003-2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2003 Greg Watson <jarrah@users.sourceforge.net>
 * Copyright (C) 2004 Li-Ta Lo <ollie@lanl.gov>
 * Copyright (C) 2005-2006 Tyan
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci.h>
#include <device/resource.h>
#include <string.h>

/**
 * @brief See if a device structure exists for path
 *
 * @param bus The bus to find the device on
 * @param path The relative path from the bus to the appropriate device
 * @return pointer to a device structure for the device on bus at path
 *         or 0/NULL if no device is found
 */
device_t find_dev_path(struct bus *parent, struct device_path *path)
{
	device_t child;
	for(child = parent->children; child; child = child->sibling) {
		if (path_eq(path, &child->path)) {
			break;
		}
	}
	return child;
}

/**
 * @brief See if a device structure already exists and if not allocate it
 *
 * @param bus The bus to find the device on
 * @param path The relative path from the bus to the appropriate device
 * @return pointer to a device structure for the device on bus at path
 */
device_t alloc_find_dev(struct bus *parent, struct device_path *path)
{
	device_t child;
	child = find_dev_path(parent, path);
	if (!child) {
		child = alloc_dev(parent, path);
	}
	return child;
}

/**
 * @brief Given a PCI bus and a devfn number, find the device structure
 *
 * @param bus The bus number
 * @param devfn a device/function number
 * @return pointer to the device structure
 */
struct device *dev_find_slot(unsigned int bus, unsigned int devfn)
{
	struct device *dev, *result;

	result = 0;
	for (dev = all_devices; dev; dev = dev->next) {
		if ((dev->path.type == DEVICE_PATH_PCI) &&
			(dev->bus->secondary == bus) && 
			(dev->path.pci.devfn == devfn)) {
			result = dev;
			break;
		}
	}
	return result;
}

/**
 * @brief Given a smbus bus and a device number, find the device structure
 *
 * @param bus The bus number
 * @param addr a device number 
 * @return pointer to the device structure
 */
struct device *dev_find_slot_on_smbus(unsigned int bus, unsigned int addr)
{
        struct device *dev, *result;
        
        result = 0;
        for (dev = all_devices; dev; dev = dev->next) {
                if ((dev->path.type == DEVICE_PATH_I2C) &&
                        (dev->bus->secondary == bus) && 
                        (dev->path.i2c.device == addr)) {
                        result = dev;
                        break; 
                }       
        }       
        return result;
}    

/** Find a device of a given vendor and type
 * @param vendor Vendor ID (e.g. 0x8086 for Intel)
 * @param device Device ID
 * @param from Pointer to the device structure, used as a starting point
 *        in the linked list of all_devices, which can be 0 to start at the 
 *        head of the list (i.e. all_devices)
 * @return Pointer to the device struct 
 */
struct device *dev_find_device(unsigned int vendor, unsigned int device, struct device *from)
{
	if (!from)
		from = all_devices;
	else
		from = from->next;
	while (from && (from->vendor != vendor || from->device != device)) {
		from = from->next;
	}
	return from;
}

/** Find a device of a given class
 * @param class Class of the device
 * @param from Pointer to the device structure, used as a starting point
 *        in the linked list of all_devices, which can be 0 to start at the 
 *        head of the list (i.e. all_devices)
 * @return Pointer to the device struct 
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

/* Warning: This function uses a static buffer.  Don't call it more than once
 * from the same print statement! */

const char *dev_path(device_t dev)
{
	static char buffer[DEVICE_PATH_MAX];
	buffer[0] = '\0';
	if (!dev) {
		memcpy(buffer, "<null>", 7);
	}
	else {
		switch(dev->path.type) {
		case DEVICE_PATH_ROOT:
			memcpy(buffer, "Root Device", 12);
			break;
		case DEVICE_PATH_PCI:
#if CONFIG_PCI_BUS_SEGN_BITS
			sprintf(buffer, "PCI: %04x:%02x:%02x.%01x",
				dev->bus->secondary>>8, dev->bus->secondary & 0xff, 
				PCI_SLOT(dev->path.pci.devfn), PCI_FUNC(dev->path.pci.devfn));
#else
			sprintf(buffer, "PCI: %02x:%02x.%01x",
				dev->bus->secondary, 
				PCI_SLOT(dev->path.pci.devfn), PCI_FUNC(dev->path.pci.devfn));
#endif
			break;
		case DEVICE_PATH_PNP:
			sprintf(buffer, "PNP: %04x.%01x",
				dev->path.pnp.port, dev->path.pnp.device);
			break;
		case DEVICE_PATH_I2C:
			sprintf(buffer, "I2C: %02x:%02x",
				dev->bus->secondary,
				dev->path.i2c.device);
			break;
		case DEVICE_PATH_APIC:
			sprintf(buffer, "APIC: %02x",
				dev->path.apic.apic_id);
			break;
		case DEVICE_PATH_PCI_DOMAIN:
			sprintf(buffer, "PCI_DOMAIN: %04x",
				dev->path.pci_domain.domain);
			break;
		case DEVICE_PATH_APIC_CLUSTER:
			sprintf(buffer, "APIC_CLUSTER: %01x",
				dev->path.apic_cluster.cluster);
			break;
		case DEVICE_PATH_CPU:
			sprintf(buffer, "CPU: %02x", dev->path.cpu.id);
			break;
		case DEVICE_PATH_CPU_BUS:
			sprintf(buffer, "CPU_BUS: %02x", dev->path.cpu_bus.id);
			break;
		default:
			printk(BIOS_ERR, "Unknown device path type: %d\n", dev->path.type);
			break;
		}
	}
	return buffer;
}

const char *bus_path(struct bus *bus)
{
	static char buffer[BUS_PATH_MAX];
	sprintf(buffer, "%s,%d", dev_path(bus->dev), bus->link);
	return buffer;
}

int path_eq(struct device_path *path1, struct device_path *path2)
{
	int equal = 0;
	if (path1->type == path2->type) {
		switch(path1->type) {
		case DEVICE_PATH_NONE:
			break;
		case DEVICE_PATH_ROOT:
			equal = 1;
			break;
		case DEVICE_PATH_PCI:
			equal = (path1->pci.devfn == path2->pci.devfn);
			break;
		case DEVICE_PATH_PNP:
			equal = (path1->pnp.port == path2->pnp.port) &&
				(path1->pnp.device == path2->pnp.device);
			break;
		case DEVICE_PATH_I2C:
			equal = (path1->i2c.device == path2->i2c.device);
			break;
		case DEVICE_PATH_APIC:
			equal = (path1->apic.apic_id == path2->apic.apic_id);
			break;
		case DEVICE_PATH_PCI_DOMAIN:
			equal = (path1->pci_domain.domain == path2->pci_domain.domain);
			break;
		case DEVICE_PATH_APIC_CLUSTER:
			equal = (path1->apic_cluster.cluster == path2->apic_cluster.cluster);
			break;
		case DEVICE_PATH_CPU:
			equal = (path1->cpu.id == path2->cpu.id);
			break;
		case DEVICE_PATH_CPU_BUS:
			equal = (path1->cpu_bus.id == path2->cpu_bus.id);
			break;
		default:
			printk(BIOS_ERR, "Uknown device type: %d\n", path1->type);
			break;
		}
	}
	return equal;
}

/**
 * See if we have unused but allocated resource structures.
 * If so remove the allocation.
 * @param dev The device to find the resource on
 */
void compact_resources(device_t dev)
{
	struct resource *resource;
	int i;
	/* Move all of the free resources to the end */
	for(i = 0; i < dev->resources;) {
		resource = &dev->resource[i];
		if (!resource->flags) {
			memmove(resource, resource + 1, (dev->resources - i) *
				sizeof(*resource));
			dev->resources -= 1;
			memset(&dev->resource[dev->resources], 0, sizeof(*resource));
		} else {
			i++;
		}
	}
}


/**
 * See if a resource structure already exists for a given index
 * @param dev The device to find the resource on
 * @param index  The index of the resource on the device.
 * @return the resource if it already exists
 */
struct resource *probe_resource(device_t dev, unsigned index)
{
	struct resource *resource;
	int i;
	/* See if there is a resource with the appropriate index */
	resource = 0;
	for(i = 0; i < dev->resources; i++) {
		if (dev->resource[i].index == index) {
			resource = &dev->resource[i];
			break;
		}
	}
	return resource;
}

/**
 * See if a resource structure already exists for a given index and if
 * not allocate one.  Then initialize the initialize the resource
 * to default values.
 * @param dev The device to find the resource on
 * @param index  The index of the resource on the device.
 */
struct resource *new_resource(device_t dev, unsigned index)
{
	struct resource *resource;

	/* First move all of the free resources to the end */
	compact_resources(dev);

	/* See if there is a resource with the appropriate index */
	resource = probe_resource(dev, index);
	if (!resource) {
		if (dev->resources == MAX_RESOURCES) {
			die("MAX_RESOURCES exceeded.");
		}
		resource = &dev->resource[dev->resources];
		memset(resource, 0, sizeof(*resource));
		dev->resources++;
	}
	/* Initialize the resource values */
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
 * @param dev The device to find the resource on
 * @param index  The index of the resource on the device.
 */
struct resource *find_resource(device_t dev, unsigned index)
{
	struct resource *resource;

	/* See if there is a resource with the appropriate index */
	resource = probe_resource(dev, index);
	if (!resource) {
		printk(BIOS_EMERG, "%s missing resource: %02x\n",
			dev_path(dev), index);
		die("");
	}
	return resource;
}


/**
 * @brief round a number up to the next multiple of gran
 * @param val the starting value
 * @param gran granularity we are aligning the number to.
 * @returns aligned value
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
 * @brief round a number up to the previous multiple of gran
 * @param val the starting value
 * @param gran granularity we are aligning the number to.
 * @returns aligned value
 */
static resource_t align_down(resource_t val, unsigned long gran)
{
	resource_t mask;
	mask = (1ULL << gran) - 1ULL;
	val &= ~mask;
	return val;
}

/**
 * @brief Compute the maximum address that is part of a resource
 * @param resource the resource whose limit is desired
 * @returns the end
 */
resource_t resource_end(struct resource *resource)
{
	resource_t base, end;
	/* get the base address */
	base = resource->base;

	/* For a non bridge resource granularity and alignment are the same.
	 * For a bridge resource align is the largest needed alignment below
	 * the bridge.  While the granularity is simply how many low bits of the
	 * address cannot be set.
	 */
	
	/* Get the end (rounded up) */
	end = base + align_up(resource->size, resource->gran) - 1;

	return end;
}

/**
 * @brief Compute the maximum legal value for resource->base
 * @param resource the resource whose maximum is desired
 * @returns the maximum
 */
resource_t resource_max(struct resource *resource)
{
	resource_t max;

	max = align_down(resource->limit - resource->size + 1, resource->align);

	return max;
}

/**
 * @brief return the resource type of a resource
 * @param resource the resource type to decode.
 */
const char *resource_type(struct resource *resource)
{
	static char buffer[RESOURCE_TYPE_MAX];
	sprintf(buffer, "%s%s%s%s",
		((resource->flags & IORESOURCE_READONLY)? "ro": ""),
		((resource->flags & IORESOURCE_PREFETCH)? "pref":""),
		((resource->flags == 0)? "unused":
	 	(resource->flags & IORESOURCE_IO)? "io":
		(resource->flags & IORESOURCE_DRQ)? "drq":
		(resource->flags & IORESOURCE_IRQ)? "irq":
		(resource->flags & IORESOURCE_MEM)? "mem":"??????"),
		((resource->flags & IORESOURCE_PCI64)?"64":""));
	return buffer;
}

/**
 * @brief print the resource that was just stored.
 * @param dev the device the stored resorce lives on
 * @param resource the resource that was just stored.
 */
void report_resource_stored(device_t dev, struct resource *resource, const char *comment)
{
	if (resource->flags & IORESOURCE_STORED) {
		char buf[10];
		unsigned long long base, end;
		base = resource->base;
		end = resource_end(resource);
		buf[0] = '\0';
		if (resource->flags & IORESOURCE_PCI_BRIDGE) {
#if CONFIG_PCI_BUS_SEGN_BITS
			sprintf(buf, "bus %04x:%02x ", dev->bus->secondary>>8, dev->link[0].secondary & 0xff);
#else
			sprintf(buf, "bus %02x ", dev->link[0].secondary);
#endif
		}
		printk(BIOS_DEBUG, 
			"%s %02lx <- [0x%010Lx - 0x%010Lx] size 0x%08Lx gran 0x%02x %s%s%s\n",
			dev_path(dev),
			resource->index,
			base, end,
			resource->size, resource->gran,
			buf,
			resource_type(resource),
			comment);
	}
}

void search_bus_resources(struct bus *bus,
	unsigned long type_mask, unsigned long type,
	resource_search_t search, void *gp)
{
	struct device *curdev;
	for(curdev = bus->children; curdev; curdev = curdev->sibling) {
		int i;
		/* Ignore disabled devices */
		if (!curdev->enabled) continue;
		for(i = 0; i < curdev->resources; i++) {
			struct resource *resource = &curdev->resource[i];
			/* If it isn't the right kind of resource ignore it */
			if ((resource->flags & type_mask) != type) {
				continue;
			}
			/* If it is a subtractive resource recurse */
			if (resource->flags & IORESOURCE_SUBTRACTIVE) {
				struct bus * subbus;
				subbus = &curdev->link[IOINDEX_SUBTRACTIVE_LINK(resource->index)];
				search_bus_resources(subbus, type_mask, type, search, gp);
				continue;
			}
			search(gp, curdev, resource);
		}
	}
}

void search_global_resources(
	unsigned long type_mask, unsigned long type,
	resource_search_t search, void *gp)
{
	struct device *curdev;
	for(curdev = all_devices; curdev; curdev = curdev->next) {
		int i;
		/* Ignore disabled devices */
		if (!curdev->enabled) continue;
		for(i = 0; i < curdev->resources; i++) {
			struct resource *resource = &curdev->resource[i];
			/* If it isn't the right kind of resource ignore it */
			if ((resource->flags & type_mask) != type) {
				continue;
			}
			/* If it is a subtractive resource ignore it */
			if (resource->flags & IORESOURCE_SUBTRACTIVE) {
				continue;
			}
			search(gp, curdev, resource);
		}
	}
}

void dev_set_enabled(device_t dev, int enable)
{
	if (dev->enabled == enable) {
		return;
	}
	dev->enabled = enable;
	if (dev->ops && dev->ops->enable) {
		dev->ops->enable(dev);
	}
	else if (dev->chip_ops && dev->chip_ops->enable_dev) {
		dev->chip_ops->enable_dev(dev);
	}
}

void disable_children(struct bus *bus)
{
	device_t child;
	for(child = bus->children; child; child = child->sibling) {
		int link;
		for(link = 0; link < child->links; link++) {
			disable_children(&child->link[link]);
		}
		dev_set_enabled(child, 0);
	}
}

static void resource_tree(struct device *root, int debug_level, int depth)
{
	int i = 0, link = 0;
	struct device *child;
	char indent[30];	/* If your tree has more levels, it's wrong. */

	for (i = 0; i < depth + 1 && i < 29; i++)
		indent[i] = ' ';
	indent[i] = '\0';

	do_printk(BIOS_DEBUG, "%s%s links %x child on link 0", indent,
		  dev_path(root), root->links);
	do_printk(BIOS_DEBUG, " %s\n", root->link[0].children ?
		  dev_path(root->link[0].children) : "NULL");
	for (i = 0; i < root->resources; i++) {
		do_printk(BIOS_DEBUG,
			  "%s%s resource base %llx size %llx align %d gran %d limit %llx flags %lx index %lx\n",
			  indent, dev_path(root), root->resource[i].base,
			  root->resource[i].size, root->resource[i].align,
			  root->resource[i].gran, root->resource[i].limit,
			  root->resource[i].flags, root->resource[i].index);
	}

	for (link = 0; link < root->links; link++) {
		for (child = root->link[link].children; child;
		     child = child->sibling)
			resource_tree(child, debug_level, depth + 1);
	}
}

void print_resource_tree(struct device * root, int debug_level,
			 const char *msg)
{
	/* Bail if root is null. */
	if (!root) {
		do_printk(debug_level, "%s passed NULL for root!\n", __func__);
		return;
	}

	/* Bail if not printing to screen. */
	if (!do_printk(debug_level, "Show resources in subtree (%s)...%s\n",
		    dev_path(root), msg))
		return;
	resource_tree(root, debug_level, 0);
}

void show_devs_tree(struct device *dev, int debug_level, int depth, int linknum)
{
	char depth_str[20] = "";
	int i;
	struct device *sibling;
	for (i = 0; i < depth; i++)
		depth_str[i] = ' ';
	depth_str[i] = '\0';
	do_printk(debug_level, "%s%s: enabled %d, %d resources\n",
		  depth_str, dev_path(dev), dev->enabled, dev->resources);
	for (i = 0; i < dev->links; i++) {
		for (sibling = dev->link[i].children; sibling;
		     sibling = sibling->sibling)
			show_devs_tree(sibling, debug_level, depth + 1, i);
	}
}

void show_all_devs_tree(int debug_level, const char *msg)
{
	/* Bail if not printing to screen. */
	if (!do_printk(debug_level, "Show all devs in tree form...%s\n", msg))
		return;
	show_devs_tree(all_devices, debug_level, 0, -1);
}

void show_devs_subtree(struct device *root, int debug_level, const char *msg)
{
	/* Bail if not printing to screen. */
	if (!do_printk(debug_level, "Show all devs in subtree %s...%s\n",
		    dev_path(root), msg))
		return;
	do_printk(debug_level, "%s\n", msg);
	show_devs_tree(root, debug_level, 0, -1);
}

void show_all_devs(int debug_level, const char *msg)
{
	struct device *dev;

	/* Bail if not printing to screen. */
	if (!do_printk(debug_level, "Show all devs...%s\n", msg))
		return;
	for (dev = all_devices; dev; dev = dev->next) {
		do_printk(debug_level,
			  "%s: enabled %d, %d resources\n",
			  dev_path(dev), dev->enabled,
			  dev->resources);
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
/*
	if (resource->flags & IORESOURCE_BRIDGE) {
#if CONFIG_PCI_BUS_SEGN_BITS
		sprintf(buf, "bus %04x:%02x ", dev->bus->secondary >> 8,
			dev->link[0].secondary & 0xff);
#else
		sprintf(buf, "bus %02x ", dev->link[0].secondary);
#endif
	}
*/
	do_printk(debug_level, "%s %02lx <- [0x%010llx - 0x%010llx] "
		  "size 0x%08Lx gran 0x%02x %s%s%s\n",
		  dev_path(dev), resource->index, base, end,
		  resource->size, resource->gran, buf,
		  resource_type(resource), comment);

}

void show_all_devs_resources(int debug_level, const char* msg)
{
	struct device *dev;

	if(!do_printk(debug_level, "Show all devs with resources...%s\n", msg))
		return;

	for (dev = all_devices; dev; dev = dev->next) {
		int i;
		do_printk(debug_level,
			  "%s: enabled %d, %d resources\n",
			  dev_path(dev), dev->enabled,
			  dev->resources);
		for (i = 0; i < dev->resources; i++)
			show_one_resource(debug_level, dev, &dev->resource[i], "");
	}
}
