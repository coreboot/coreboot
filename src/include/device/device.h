#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
#include <device/resource.h>
#include <device/path.h>

struct device;
typedef struct device * device_t;

struct device_operations {
	void (*read_resources)(device_t dev);
	void (*set_resources)(device_t dev);
	void (*enable_resources)(device_t dev);
	void (*init)(device_t dev);
	unsigned int (*scan_bus)(device_t  bus, unsigned int max);
	void (*enable)(device_t dev);
};


struct bus {
	device_t 	dev;		/* This bridge device */
	device_t 	children;	/* devices behind this bridge */
	unsigned	bridge_ctrl;	/* Bridge control register */
	unsigned char	link;		/* The index of this link */
	unsigned char	secondary; 	/* secondary bus number */
	unsigned char	subordinate;	/* max subordinate bus number */
	unsigned char   cap;		/* PCi capability offset */
};

#define MAX_RESOURCES 8
#define MAX_LINKS     3
/*
 * There is one device structure for each slot-number/function-number
 * combination:
 */

struct chip;
struct device {
	struct bus *	bus;		/* bus this device is on */
	device_t	sibling;	/* next device on this bus */
	device_t	next;		/* chain of all devices */

	struct device_path path;
	unsigned short	vendor;
	unsigned short	device;
	unsigned int	class;		/* 3 bytes: (base,sub,prog-if) */
	unsigned int	hdr_type;	/* PCI header type */
	/* @todo rename this to 'enabled' */
	unsigned int    enable : 1;	/* set if we should enable the device */

	uint8_t command;

	/* Base registers for this device, can be adjusted by
	 * pcibios_fixup() as necessary.
	 */
	struct resource resource[MAX_RESOURCES];
	unsigned int resources;

	struct bus link[MAX_LINKS];
	unsigned int links;

	unsigned long rom_address;
	struct device_operations *ops;
	struct chip *chip;
};

extern struct device	dev_root;	/* root bus */
extern struct device	*all_devices;	/* list of all devices */


/* Generic device interface functions */
extern device_t alloc_dev(struct bus *parent, struct device_path *path);
extern void dev_enumerate(void);
extern void dev_configure(void);
extern void dev_enable(void);
extern void dev_initialize(void);

/* Generic device helper functions */
extern void compute_allocate_resource(struct bus *bus, struct resource *bridge,
	unsigned long type_mask, unsigned long type);
extern void assign_resources(struct bus *bus);
extern void enable_resources(struct device *dev);
extern void enumerate_static_device(void);
extern const char *dev_path(device_t dev);
extern void compact_resources(device_t dev);
extern struct resource *get_resource(device_t dev, unsigned index);

/* Helper functions */
device_t alloc_find_dev(struct bus *parent, struct device_path *path);
device_t dev_find_device (unsigned int vendor, unsigned int device, device_t from);
device_t dev_find_class (unsigned int class, device_t from);
device_t dev_find_slot (unsigned int bus, unsigned int devfn);

/* Rounding for boundaries. 
 * Due to some chip bugs, go ahead and roung IO to 16
 */
#define DEVICE_IO_ALIGN 16 
#define DEVICE_MEM_ALIGN 4096

struct device_operations default_dev_ops_root;
extern void root_dev_read_resources(device_t dev);
extern void root_dev_set_resources(device_t dev);
extern unsigned int walk_static_devices(device_t bus, unsigned int max);
extern void enable_childrens_resources(device_t dev);
extern unsigned int root_dev_scan_pci_bus(device_t root, unsigned int max);

#endif /* DEVICE_H */
