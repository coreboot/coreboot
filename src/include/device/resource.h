#ifndef DEVICE_RESOURCE_H
#define DEVICE_RESOURCE_H

#include <stdint.h>

#define IORESOURCE_BITS		0x000000ff	/* Bus-specific bits */

#define IORESOURCE_IO		0x00000100	/* Resource type */
#define IORESOURCE_MEM		0x00000200
#define IORESOURCE_IRQ		0x00000400
#define IORESOURCE_DRQ		0x00000800

#define IORESOURCE_PREFETCH	0x00001000	/* No side effects */
#define IORESOURCE_READONLY	0x00002000
#define IORESOURCE_CACHEABLE	0x00004000
#define IORESOURCE_RANGELENGTH	0x00008000
#define IORESOURCE_SHADOWABLE	0x00010000
#define IORESOURCE_BUS_HAS_VGA	0x00020000
#define IORESOURCE_SUBTRACTIVE  0x00040000	/* This resource filters all of the unclaimed transactions
						 * to the bus below.
						 */
#define IORESOURCE_BRIDGE	0x00080000	/* The IO resource has a bus below it. */
#define IORESOURCE_RESERVE	0x10000000	/* The resource needs to be reserved in the coreboot table */
#define IORESOURCE_STORED	0x20000000	/* The IO resource assignment has been stored in the device */
#define IORESOURCE_ASSIGNED	0x40000000	/* An IO resource that has been assigned a value */
#define IORESOURCE_FIXED	0x80000000	/* An IO resource the allocator must not change */

/* PCI specific resource bits */
#define IORESOURCE_PCI64	(1<<0)	/* 64bit long pci resource */
#define IORESOURCE_PCI_BRIDGE	(1<<1)  /* A bridge pci resource */

/* ISA PnP IRQ specific bits (IORESOURCE_BITS) */
#define IORESOURCE_IRQ_HIGHEDGE		(1<<0)
#define IORESOURCE_IRQ_LOWEDGE		(1<<1)
#define IORESOURCE_IRQ_HIGHLEVEL	(1<<2)
#define IORESOURCE_IRQ_LOWLEVEL		(1<<3)

/* ISA PnP DMA specific bits (IORESOURCE_BITS) */
#define IORESOURCE_DMA_TYPE_MASK	(3<<0)
#define IORESOURCE_DMA_8BIT		(0<<0)
#define IORESOURCE_DMA_8AND16BIT	(1<<0)
#define IORESOURCE_DMA_16BIT		(2<<0)

#define IORESOURCE_DMA_MASTER		(1<<2)
#define IORESOURCE_DMA_BYTE		(1<<3)
#define IORESOURCE_DMA_WORD		(1<<4)

#define IORESOURCE_DMA_SPEED_MASK	(3<<6)
#define IORESOURCE_DMA_COMPATIBLE	(0<<6)
#define IORESOURCE_DMA_TYPEA		(1<<6)
#define IORESOURCE_DMA_TYPEB		(2<<6)
#define IORESOURCE_DMA_TYPEF		(3<<6)


/* ISA PnP memory I/O specific bits (IORESOURCE_BITS) */
#define IORESOURCE_MEM_WRITEABLE	(1<<0)	/* dup: IORESOURCE_READONLY */
#define IORESOURCE_MEM_CACHEABLE	(1<<1)	/* dup: IORESOURCE_CACHEABLE */
#define IORESOURCE_MEM_RANGELENGTH	(1<<2)	/* dup: IORESOURCE_RANGELENGTH */
#define IORESOURCE_MEM_TYPE_MASK	(3<<3)
#define IORESOURCE_MEM_8BIT		(0<<3)
#define IORESOURCE_MEM_16BIT		(1<<3)
#define IORESOURCE_MEM_8AND16BIT	(2<<3)
#define IORESOURCE_MEM_SHADOWABLE	(1<<5)	/* dup: IORESOURCE_SHADOWABLE */
#define IORESOURCE_MEM_EXPANSIONROM	(1<<6)


typedef u64 resource_t;
struct resource {
	resource_t base;	/* Base address of the resource */
	resource_t size;	/* Size of the resource */
	resource_t limit;	/* Largest valid value base + size -1 */
	struct resource* next;	/* Next resource in the list */
	unsigned long flags;	/* Descriptions of the kind of resource */
	unsigned long index;	/* Bus specific per device resource id */
	unsigned char align;	/* Required alignment (log 2) of the resource */
	unsigned char gran;	/* Granularity (log 2) of the resource */
	/* Alignment must be >= the granularity of the resource */
};

/* Macros to generate index values for resources */
#define IOINDEX_SUBTRACTIVE(IDX,LINK) (0x10000000 + ((IDX) << 8) + LINK)
#define IOINDEX_SUBTRACTIVE_LINK(IDX) (IDX & 0xff)

#define IOINDEX(IDX,LINK) (((LINK) << 16) + IDX)
#define IOINDEX_LINK(IDX) (( IDX & 0xf0000) >> 16)
#define IOINDEX_IDX(IDX) (IDX & 0xffff)

/* Generic resource helper functions */
struct device;
struct bus;
extern void compact_resources(struct device * dev);
extern struct resource *probe_resource(struct device *dev, unsigned index);
extern struct resource *new_resource(struct device * dev, unsigned index);
extern struct resource *find_resource(struct device * dev, unsigned index);
extern resource_t resource_end(struct resource *resource);
extern resource_t resource_max(struct resource *resource);
extern void report_resource_stored(struct device * dev, struct resource *resource, const char *comment);

typedef void (*resource_search_t)(void *gp, struct device *dev, struct resource *res);
extern void search_bus_resources(struct bus *bus,
	unsigned long type_mask, unsigned long type,
	resource_search_t search, void *gp);

extern void search_global_resources(
	unsigned long type_mask, unsigned long type,
	resource_search_t search, void *gp);

#define RESOURCE_TYPE_MAX 20
extern const char *resource_type(struct resource *resource);

#endif /* DEVICE_RESOURCE_H */
