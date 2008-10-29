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

#include <console.h>
#include <io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>

/**
 * See if a device structure exists for path.
 *
 * @param parent The bus to find the device on.
 * @param path The relative path from the bus to the appropriate device.
 * @return Pointer to a device structure for the device on bus at path
 *         or 0/NULL if no device is found.
 */
struct device *find_dev_path(struct bus *parent, struct device_path *path)
{
	struct device *child;
	for (child = parent->children; child; child = child->sibling) {
		if (path_eq(path, &child->path)) {
			break;
		}
	}
	return child;
}

/**
 * See if a device structure already exists and if not allocate it.
 *
 * @param parent The bus to find the device on (TODO: comment correct?).
 * @param path The relative path from the bus to the appropriate device.
 * @param device_id TODO
 * @return Pointer to a device structure for the device on bus at path.
 */
struct device *alloc_find_dev(struct bus *parent, struct device_path *path,
			      struct device_id *id)
{
	struct device *child;
	child = find_dev_path(parent, path);
	if (!child) {
		child = alloc_dev(parent, path, id);
	}
	return child;
}

/**
 * Given a PCI bus and a devfn number, find the device structure.
 *
 * @param bus The bus number.
 * @param devfn A device/function number.
 * @return Pointer to the device structure.
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
 * Given a smbus bus and a device number, find the device structure.
 *
 * @param bus The bus number.
 * @param addr A device number.
 * @return Pointer to the device structure.
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

/**
 * Find a device of a given vendor and type.
 *
 * @param devid Pointer to device_id struct 
 * @param from Pointer to the device structure, used as a starting point
 *             in the linked list of all_devices, which can be 0 to start
 *             at the head of the list (i.e. all_devices).
 * @return Pointer to the device struct.
 */
struct device *dev_find_device(struct device_id *devid, struct device *from)
{
	printk(BIOS_SPEW, "%s: find %s\n", __FUNCTION__, dev_id_string(devid));

	if (!from)
		from = all_devices;
	else
		from = from->next;
	for(;from;from = from->next){
		printk(BIOS_SPEW, "Check %s\n", dev_id_string(&from->id));
		if (id_eq(devid, &from->id))
			break;
	}
	printk(BIOS_SPEW, "%sfound\n", from ? "" : "not ");
	return from;
}

/**
 * Find a PCI device of a given vendor and type.
 * This is a convenience function since PCI device searches
 * are by far the most common. 
 *
 * @param vendor vendor number
 * @param device device number
 * @param from Pointer to the device structure, used as a starting point
 *             in the linked list of all_devices, which can be 0 to start
 *             at the head of the list (i.e. all_devices).
 * @return Pointer to the device struct.
 */
struct device *dev_find_pci_device(u16 vendor, u16 device, struct device *from)
{
	struct device_id id;

	id.type = DEVICE_ID_PCI;
	id.pci.vendor = vendor;
	id.pci.device = device;
	return dev_find_device(&id, from);
}

/**
 * Find a device of a given class.
 *
 * @param class Class of the device.
 * @param from Pointer to the device structure, used as a starting point
 *             in the linked list of all_devices, which can be 0 to start
 *             at the head of the list (i.e. all_devices).
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

/* WARNING: NOT SMP-safe! */
const char *dev_path(struct device *dev)
{
	static char buffer[DEVICE_PATH_MAX];
	buffer[0] = '\0';
	if (!dev) {
		memcpy(buffer, "<null>", 7);
	} else {
		switch (dev->path.type) {
		case DEVICE_PATH_ROOT:
			memcpy(buffer, "Root Device", 12);
			break;
		case DEVICE_PATH_PCI:
#if PCI_BUS_SEGN_BITS
			sprintf(buffer, "PCI: %04x:%02x:%02x.%01x",
				dev->bus->secondary >> 8,
				dev->bus->secondary & 0xff,
				PCI_SLOT(dev->path.pci.devfn),
				PCI_FUNC(dev->path.pci.devfn));
#else
			sprintf(buffer, "PCI: %02x:%02x.%01x",
				dev->bus->secondary,
				PCI_SLOT(dev->path.pci.devfn),
				PCI_FUNC(dev->path.pci.devfn));
#endif
			break;
		case DEVICE_PATH_PNP:
			sprintf(buffer, "PNP: %04x.%01x",
				dev->path.pnp.port, dev->path.pnp.device);
			break;
		case DEVICE_PATH_I2C:
			sprintf(buffer, "I2C: %02x:%02x",
				dev->bus->secondary, dev->path.i2c.device);
			break;
		case DEVICE_PATH_APIC:
			sprintf(buffer, "APIC: %02x", dev->path.apic.apic_id);
			break;
		case DEVICE_PATH_PCI_DOMAIN:
			sprintf(buffer, "PCI_DOMAIN: %04x",
				dev->path.pci_domain.domain);
			break;
		case DEVICE_PATH_PCI_BUS:
			sprintf(buffer, "PCI_BUS: %04x",
				dev->path.pci_bus.bus);
			break;
		case DEVICE_PATH_APIC_CLUSTER:
			sprintf(buffer, "APIC_CLUSTER: %01x",
				dev->path.apic_cluster.cluster);
			break;
		case DEVICE_PATH_CPU:
			sprintf(buffer, "CPU: %02x", dev->path.cpu.id);
			break;
		case DEVICE_PATH_CPU_BUS:
			sprintf(buffer, "CPU_BUS: %02x",
				dev->path.cpu_bus.id);
			break;
		case DEVICE_PATH_IOPORT:
			sprintf(buffer, "IOPORT: %02x",
				dev->path.ioport.iobase);
			break;
		default:
			printk(BIOS_ERR, "%s: Unknown device path type: %d\n",
			       dev->dtsname, dev->path.type);
			break;
		}
	}
	return buffer;
}

/* WARNING: NOT SMP-safe! */
const char *dev_id_string(struct device_id *id)
{
	static char buffer[DEVICE_ID_MAX];
	buffer[0] = '\0';
	if (!id) {
		memcpy(buffer, "<null>", 7);
	} else {
		switch (id->type) {
		case DEVICE_ID_ROOT:
			memcpy(buffer, "Root Device", 12);
			break;
		case DEVICE_ID_PCI:
			sprintf(buffer, "PCI: %04x:%04x", id->pci.vendor,
				id->pci.device);
			break;
		case DEVICE_ID_PNP:
			sprintf(buffer, "PNP: %04x", id->pnp.device);
			break;
		case DEVICE_ID_I2C:
			sprintf(buffer, "I2C: %04x", id->i2c.id);
			break;
		case DEVICE_ID_APIC:
			sprintf(buffer, "APIC: %02x:%02x", id->apic.vendor,
				id->apic.device);
			break;
		case DEVICE_ID_PCI_DOMAIN:
			sprintf(buffer, "PCI_DOMAIN: %04x:%04x",
				id->pci_domain.vendor,
				id->pci_domain.device);
			break;
		case DEVICE_ID_APIC_CLUSTER:
			sprintf(buffer, "APIC_CLUSTER: %02x:%02x",
				id->apic_cluster.vendor,
				id->apic_cluster.device);
			break;
		case DEVICE_ID_CPU:
			sprintf(buffer, "CPU", id->cpu.cpuid[0],
				id->cpu.cpuid[1], id->cpu.cpuid[2]);
			break;
		case DEVICE_ID_CPU_BUS:
			sprintf(buffer, "CPU_BUS: %02x:%02x",
				id->cpu_bus.vendor, id->cpu_bus.device);
			break;
		default:
			printk(BIOS_ERR, "%s: Unknown device ID type: %d\n",
			       __func__, id->type);
			memcpy(buffer, "Unknown", 8);
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
		switch (path1->type) {
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
			equal =
			    (path1->apic.apic_id == path2->apic.apic_id);
			break;
		case DEVICE_PATH_PCI_DOMAIN:
			equal =
			    (path1->pci_domain.domain ==
			     path2->pci_domain.domain);
			break;
		case DEVICE_PATH_APIC_CLUSTER:
			equal =
			    (path1->apic_cluster.cluster ==
			     path2->apic_cluster.cluster);
			break;
		case DEVICE_PATH_CPU:
			equal = (path1->cpu.id == path2->cpu.id);
			break;
		case DEVICE_PATH_CPU_BUS:
			equal = (path1->cpu_bus.id == path2->cpu_bus.id);
			break;
		default:
			printk(BIOS_ERR, "Unknown device type: %d\n",
			       path1->type);
			break;
		}
	}
	return equal;
}

int id_eq(struct device_id *path1, struct device_id *path2)
{
	int equal = 0;
	if (path1->type == path2->type) {
		switch (path1->type) {
		case DEVICE_ID_NONE:
			break;
		case DEVICE_ID_ROOT:
			equal = 1;
			break;
		case DEVICE_ID_PCI:
			equal = (path1->pci.vendor == path2->pci.vendor)
			    && (path1->pci.device == path2->pci.device);
			break;
		case DEVICE_ID_PNP:
			equal = (path1->pnp.device == path2->pnp.device);
			break;
		case DEVICE_ID_I2C:
			equal = (path1->i2c.id == path2->i2c.id);
			break;
		case DEVICE_ID_APIC:
			equal = (path1->apic.vendor == path2->apic.vendor)
			    && (path1->apic.device == path2->apic.device);
			break;
		case DEVICE_ID_PCI_DOMAIN:
			equal =
			    (path1->pci_domain.vendor ==
			     path2->pci_domain.vendor)
			    && (path1->pci_domain.device ==
				path2->pci_domain.device);
			break;
		case DEVICE_ID_APIC_CLUSTER:
			equal =
			    (path1->apic_cluster.vendor ==
			     path2->apic_cluster.vendor)
			    && (path1->apic_cluster.device ==
				path2->apic_cluster.device);
			break;
		case DEVICE_ID_CPU:
			equal = (path1->cpu.cpuid == path2->cpu.cpuid);
			break;
		case DEVICE_ID_CPU_BUS:
			equal =
			    (path1->cpu_bus.vendor == path2->cpu_bus.vendor)
			    && (path1->cpu_bus.device ==
				path2->cpu_bus.device);
			break;
		default:
			printk(BIOS_ERR, "Unknown device type: %d\n",
			       path1->type);
			break;
		}
	}
	return equal;
}

/**
 * See if we have unused but allocated resource structures.
 * If so remove the allocation.
 *
 * @param dev The device to find the resource on.
 */
void compact_resources(struct device *dev)
{
	struct resource *resource;
	int i;
	/* Move all of the free resources to the end. */
	for (i = 0; i < dev->resources;) {
		resource = &dev->resource[i];
		if (!resource->flags) {
			memmove(resource, resource + 1, (dev->resources-i)* sizeof(*resource));
			dev->resources -= 1;
			memset(&dev->resource[dev->resources], 0,
			       sizeof(*resource));
		} else {
			i++;
		}
	}
}

/**
 * See if a resource structure already exists for a given index.
 *
 * @param dev The device to find the resource on.
 * @param index The index of the resource on the device.
 * @return The resource if it already exists.
 */
struct resource *probe_resource(struct device *dev, unsigned int index)
{
	struct resource *resource;
	int i;
	/* See if there is a resource with the appropriate index. */
	resource = 0;
	for (i = 0; i < dev->resources; i++) {
		if (dev->resource[i].index == index) {
			resource = &dev->resource[i];
			break;
		}
	}
	return resource;
}

/**
 * See if a resource structure already exists for a given index and if
 * not allocate one. Then initialize the resource to default values.
 *
 * @param dev The device to find the resource on.
 * @param index The index of the resource on the device.
 */
struct resource *new_resource(struct device *dev, unsigned int index)
{
	struct resource *resource;

	/* First move all of the free resources to the end. */
	compact_resources(dev);

	/* See if there is a resource with the appropriate index. */
	resource = probe_resource(dev, index);
	if (!resource) {
		if (dev->resources == MAX_RESOURCES) {
			die("MAX_RESOURCES exceeded.");
		}
		resource = &dev->resource[dev->resources];
		memset(resource, 0, sizeof(*resource));
		dev->resources++;
	}
	/* Initialize the resource values. */
	if (!(resource->flags & IORESOURCE_FIXED)) {
		resource->flags = 0;
		resource->base = 0;
	}
	resource->size = 0;
	resource->limit = 0;
	resource->index = index;
	resource->align = 0;
	resource->gran = 0;

	return resource;
}

/**
 * Return an existing resource structure for a given index.
 *
 * @param dev The device to find the resource on.
 * @param index The index of the resource on the device.
 */
struct resource *find_resource(struct device *dev, unsigned int index)
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
 * @returns The aligned value.
 */
resource_t align_up(resource_t val, unsigned long gran)
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
 * @returns The aligned value.
 */
resource_t align_down(resource_t val, unsigned long gran)
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
 * @returns The end.
 */
resource_t resource_end(struct resource *resource)
{
	resource_t base, end;

	/* Get the base address. */
	base = resource->base;

	/* For a non-bridge resource granularity and alignment are the same.
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
 * @returns The maximum.
 */
resource_t resource_max(struct resource *resource)
{
	resource_t max;
	max = align_down(resource->limit - resource->size + 1, resource->align);
	return max;
}

/**
 * Return the resource type of a resource.
 *
 * @param resource The resource type to decode.
 */
const char *resource_type(struct resource *resource)
{
	static char buffer[RESOURCE_TYPE_MAX];
	sprintf(buffer, "%s%s%s%s",
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
void report_resource_stored(struct device *dev, struct resource *resource,
			    const char *comment)
{
	if (resource->flags & IORESOURCE_STORED) {
		char buf[10];
		unsigned long long base, end;
		base = resource->base;
		end = resource_end(resource);
		buf[0] = '\0';
		if (resource->flags & IORESOURCE_PCI_BRIDGE) {
#if PCI_BUS_SEGN_BITS
			sprintf(buf, "bus %04x:%02x ", dev->bus->secondary >> 8,
				dev->link[0].secondary & 0xff);
#else
			sprintf(buf, "bus %02x ", dev->link[0].secondary);
#endif
		}
		printk(BIOS_DEBUG, "%s %02lx <- [0x%010llx - 0x%010llx] "
			"size 0x%08Lx gran 0x%02x %s%s%s\n",
			dev_path(dev), resource->index, base, end,
			resource->size, resource->gran, buf,
			resource_type(resource), comment);
	}
}

void search_bus_resources(struct bus *bus,
			  unsigned long type_mask, unsigned long type,
			  resource_search_t search, void *gp)
{
	struct device *curdev;
	for (curdev = bus->children; curdev; curdev = curdev->sibling) {
		int i;
		/* Ignore disabled devices. */
		if (!curdev->have_resources)
			continue;
		for (i = 0; i < curdev->resources; i++) {
			struct resource *resource = &curdev->resource[i];
			/* If it isn't the right kind of resource ignore it. */
			if ((resource->flags & type_mask) != type) {
				continue;
			}
			/* If it is a subtractive resource recurse. */
			if (resource->flags & IORESOURCE_SUBTRACTIVE) {
				struct bus *subbus;
				subbus = &curdev->link[IOINDEX_SUBTRACTIVE_LINK
					 (resource->index)];
				search_bus_resources(subbus, type_mask, type,
						     search, gp);
				continue;
			}
			search(gp, curdev, resource);
		}
	}
}

void search_global_resources(unsigned long type_mask, unsigned long type,
			     resource_search_t search, void *gp)
{
	struct device *curdev;
	printk(BIOS_SPEW, "%s: mask %lx type %lx \n", __func__, type_mask, type);
	for (curdev = all_devices; curdev; curdev = curdev->next) {
		int i;
		printk(BIOS_SPEW,
		       "%s: dev %s, have_resources %d #resources %d\n",
		       __func__, curdev->dtsname, curdev->have_resources,
		       curdev->resources);
		/* Ignore disabled devices. */
		if (!curdev->have_resources)
			continue;
		for (i = 0; i < curdev->resources; i++) {
			struct resource *resource = &curdev->resource[i];
			printk(BIOS_SPEW,
			       "%s: dev %s, resource %d, flags %lx base 0x%llx size 0x%llx\n",
			       __func__, curdev->dtsname, i, resource->flags,
			       resource->base, resource->size);
			/* If it isn't the right kind of resource ignore it. */
			if ((resource->flags & type_mask) != type) {
				continue;
			}
			/* If it is a subtractive resource ignore it. */
			if (resource->flags & IORESOURCE_SUBTRACTIVE) {
				continue;
			}
			search(gp, curdev, resource);
		}
	}
}

void dev_set_enabled(struct device *dev, int enable)
{
	if (dev->enabled == enable) {
		return;
	}
	dev->enabled = enable;
	if (dev->ops && dev->ops->phase5_enable_resources) {
		dev->ops->phase5_enable_resources(dev);
	}
}

void disable_children(struct bus *bus)
{
	struct device *child;
	for (child = bus->children; child; child = child->sibling) {
		int link;
		for (link = 0; link < child->links; link++) {
			disable_children(&child->link[link]);
		}
		dev_set_enabled(child, 0);
	}
}
