/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DEVICE_RESOURCE_H
#define DEVICE_RESOURCE_H

#include <stdint.h>
#include <stddef.h>

#define IORESOURCE_BITS		0x000000ff	/* Bus-specific bits */

#define IORESOURCE_IO		0x00000100	/* Resource type */
#define IORESOURCE_MEM		0x00000200
#define IORESOURCE_IRQ		0x00000400
#define IORESOURCE_DRQ		0x00000800

#define IORESOURCE_TYPE_MASK	(IORESOURCE_IO | IORESOURCE_MEM \
				| IORESOURCE_IRQ | IORESOURCE_DRQ)

#define IORESOURCE_PREFETCH	0x00001000	/* No side effects */
#define IORESOURCE_READONLY	0x00002000
#define IORESOURCE_CACHEABLE	0x00004000
#define IORESOURCE_RANGELENGTH	0x00008000
#define IORESOURCE_SHADOWABLE	0x00010000
#define IORESOURCE_BUS_HAS_VGA	0x00020000
/* This resource filters all of the unclaimed transactions to the bus below. */
#define IORESOURCE_SUBTRACTIVE  0x00040000
/* The IO resource has a bus below it. */
#define IORESOURCE_BRIDGE	0x00080000
/* This is a request to allocate resource about 4G boundary. */
#define IORESOURCE_ABOVE_4G	0x00100000
/* The resource needs to be reserved in the coreboot table */
#define IORESOURCE_RESERVE	0x10000000
/* The IO resource assignment has been stored in the device */
#define IORESOURCE_STORED	0x20000000
/* An IO resource that has been assigned a value */
#define IORESOURCE_ASSIGNED	0x40000000
/* An IO resource the allocator must not change */
#define IORESOURCE_FIXED	0x80000000

/* PCI specific resource bits (IORESOURCE_BITS) */
#define IORESOURCE_PCI64		(1<<0)	/* 64bit long pci resource */
#define IORESOURCE_PCI_BRIDGE		(1<<1)  /* A bridge pci resource */
#define IORESOURCE_PCIE_RESIZABLE_BAR	(1<<2)  /* A Resizable BAR */

typedef u64 resource_t;
struct resource {
	resource_t base;	/* Base address of the resource */
	resource_t size;	/* Size of the resource */
	resource_t limit;	/* Largest valid value base + size -1 */
	DEVTREE_CONST struct resource *next;	/* Next resource in the list */
	unsigned long flags;	/* Descriptions of the kind of resource */
	unsigned long index;	/* Bus specific per device resource id */
	unsigned char align;	/* Required alignment (log 2) of the resource */
	unsigned char gran;	/* Granularity (log 2) of the resource */
	/* Alignment must be >= the granularity of the resource */
};

/* Macros to generate index values for resources */
#define IOINDEX_SUBTRACTIVE(IDX, LINK) (0x10000000 + ((IDX) << 8) + LINK)
#define IOINDEX_SUBTRACTIVE_LINK(IDX) (IDX & 0xff)

#define IOINDEX(IDX, LINK) (((LINK) << 16) + IDX)
#define IOINDEX_LINK(IDX) ((IDX & 0xf0000) >> 16)
#define IOINDEX_IDX(IDX) (IDX & 0xffff)

/* Generic resource helper functions */
struct device;
struct bus;
void compact_resources(struct device *dev);
struct resource *probe_resource(const struct device *dev, unsigned int index);
struct resource *new_resource(struct device *dev, unsigned int index);
struct resource *find_resource(const struct device *dev, unsigned int index);
resource_t resource_end(const struct resource *resource);
resource_t resource_max(const struct resource *resource);
void report_resource_stored(struct device *dev, const struct resource *resource,
			    const char *comment);

typedef void (*resource_search_t)(void *gp, struct device *dev, struct resource *res);

void search_bus_resources(struct bus *bus, unsigned long type_mask, unsigned long type,
			  resource_search_t search, void *gp);

void search_global_resources(unsigned long type_mask, unsigned long type,
			     resource_search_t search, void *gp);

#define RESOURCE_TYPE_MAX 20
const char *resource_type(const struct resource *resource);

static inline void *res2mmio(const struct resource *res, unsigned long offset,
			     unsigned long mask)
{
	return (void *)(uintptr_t)((res->base + offset) & ~mask);
}

void log_resource(const char *type, const struct device *dev, const struct resource *res,
		  const char *srcfile, const int line);

#define LOG_RESOURCE(type, dev, res)						\
	do {									\
		if (CONFIG(DEBUG_RESOURCES) && (dev) && (res))			\
			log_resource(type, (dev), (res), __func__, __LINE__);	\
	} while (0)

/*
 * Pick largest resource on the bus using the given mask and type.
 * Params:
 * bus = Bus from which the resource needs to picked from.
 * result_res = If NULL, there was no previous resource picked on this bus, else it points to
 *              the last picked resource.
 * type_mask = Mask to be applied when searching for resource
 * type = Expected type for the resource
 *
 * Returns:
 * If resource is found, returns the device and sets result_rest to point to the resource. Else
 * returns NULL.
 */
const struct device *largest_resource(struct bus *bus, struct resource **result_res,
				      unsigned long type_mask, unsigned long type);

/* Compute and allocate resources. This is the main resource allocator entry point. */
void allocate_resources(const struct device *root);

#endif /* DEVICE_RESOURCE_H */
