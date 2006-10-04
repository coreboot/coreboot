#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
#include <device/resource.h>
#include <device/path.h>


struct device;
typedef struct device * device_t;
struct pci_operations;
struct pci_bus_operations;
struct smbus_bus_operations;

/* Chip operations */
struct chip_operations {
	void (*enable_dev)(struct device *dev);
#if CONFIG_CHIP_NAME == 1
	char *name;
#endif
};

#if CONFIG_CHIP_NAME == 1
#define CHIP_NAME(X) .name = X,
#else
#define CHIP_NAME(X)
#endif

struct bus;

struct device_operations {
	void (*read_resources)(device_t dev);
	void (*set_resources)(device_t dev);
	void (*enable_resources)(device_t dev);
	void (*init)(device_t dev);
	unsigned int (*scan_bus)(device_t bus, unsigned int max);
	void (*enable)(device_t dev);
	void (*set_link)(device_t dev, unsigned int link);
	void (*reset_bus)(struct bus *bus);
	const struct pci_operations *ops_pci;
	const struct smbus_bus_operations *ops_smbus_bus;
	const struct pci_bus_operations *ops_pci_bus;
};


struct bus {
	device_t 	dev;		/* This bridge device */
	device_t 	children;	/* devices behind this bridge */
	unsigned	bridge_ctrl;	/* Bridge control register */
	unsigned char	link;		/* The index of this link */
	uint16_t	secondary; 	/* secondary bus number */
	uint16_t	subordinate;	/* max subordinate bus number */
	unsigned char   cap;		/* PCi capability offset */
	unsigned	reset_needed : 1;
	unsigned	disable_relaxed_ordering : 1;
};

#define MAX_RESOURCES 12 
#define MAX_LINKS    8 
/*
 * There is one device structure for each slot-number/function-number
 * combination:
 */

struct device {
	struct bus *	bus;		/* bus this device is on, for bridge
					 * devices, it is the up stream bus */
	device_t	sibling;	/* next device on this bus */
	device_t	next;		/* chain of all devices */

	struct device_path path;
	unsigned 	vendor;
	unsigned 	device;
	unsigned int	class;		/* 3 bytes: (base,sub,prog-if) */
	unsigned int	hdr_type;	/* PCI header type */
	unsigned int    enabled : 1;	/* set if we should enable the device */
	unsigned int    initialized : 1; /* set if we have initialized the device */
	unsigned int    have_resources : 1; /* Set if we have read the devices resources */
	unsigned int    on_mainboard : 1;
	unsigned long   rom_address;

	uint8_t command;

	/* Base registers for this device. I/O, MEM and Expansion ROM */
	struct resource resource[MAX_RESOURCES];
	unsigned int resources;

	/* link are (down sream) buses attached to the device, usually a leaf
	 * device with no children have 0 buses attached and a bridge has 1 bus 
	 */
	struct bus link[MAX_LINKS];
	/* number of buses attached to the device */
	unsigned int links;

	struct device_operations *ops;
	struct chip_operations *chip_ops;
	void *chip_info;
};

extern struct device	dev_root;	/* root bus */
extern struct device	*all_devices;	/* list of all devices */


/* Generic device interface functions */
extern device_t alloc_dev(struct bus *parent, struct device_path *path);
extern void dev_enumerate(void);
extern void dev_configure(void);
extern void dev_enable(void);
extern void dev_initialize(void);
extern void dev_optimize(void);

/* Generic device helper functions */
extern int reset_bus(struct bus *bus);
extern unsigned int scan_bus(struct device *bus, unsigned int max);
extern void compute_allocate_resource(struct bus *bus, struct resource *bridge,
	unsigned long type_mask, unsigned long type);
extern void assign_resources(struct bus *bus);
extern void enable_resources(struct device *dev);
extern void enumerate_static_device(void);
extern void enumerate_static_devices(void);
extern const char *dev_path(device_t dev);
const char *bus_path(struct bus *bus);
extern void dev_set_enabled(device_t dev, int enable);
extern void disable_children(struct bus *bus);

/* Helper functions */
device_t find_dev_path(struct bus *parent, struct device_path *path);
device_t alloc_find_dev(struct bus *parent, struct device_path *path);
device_t dev_find_device (unsigned int vendor, unsigned int device, device_t from);
device_t dev_find_class (unsigned int class, device_t from);
device_t dev_find_slot (unsigned int bus, unsigned int devfn);
device_t dev_find_slot_on_smbus (unsigned int bus, unsigned int addr);


/* Rounding for boundaries. 
 * Due to some chip bugs, go ahead and roung IO to 16
 */
#define DEVICE_IO_ALIGN 16 
#define DEVICE_MEM_ALIGN 4096

struct device_operations default_dev_ops_root;
extern void root_dev_read_resources(device_t dev);
extern void root_dev_set_resources(device_t dev);
extern unsigned int scan_static_bus(device_t bus, unsigned int max);
extern void enable_childrens_resources(device_t dev);
extern void root_dev_enable_resources(device_t dev);
extern unsigned int root_dev_scan_bus(device_t root, unsigned int max);
extern void root_dev_init(device_t dev);
#endif /* DEVICE_H */
