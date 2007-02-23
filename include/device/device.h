/*
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

*/
#ifndef DEVICE_H
#define DEVICE_H

#include <arch/types.h>
#include <device/resource.h>
#include <device/path.h>


struct device;
struct pci_operations;
struct pci_bus_operations;
struct smbus_bus_operations;

/* Chip operations */
struct chip_operations {
	void (*enable_dev)(struct device *dev);
	char *name;
};

struct bus;

/* we are moving from the confusing naming scheme to a numbering scheme. We are hoping
  * this makes it easier for people to know the order of operations. 
  * So far, it is not clear. We may actually want to have names like dev_phase5_enable_resources. 
  * The numbering is nice, the naming is nice, what to do?
  */
struct device_operations {
//	void (*enable)(struct device * dev);
	/* for now, we leave these, since they seem generic */
	void (*set_link)(struct device * dev, unsigned int link);
	void (*reset_bus)(struct bus *bus);

	/* phase1 is called ONLY if you CAN NEVER use printk. Only very early console needs this now */
	void (*phase1)(struct device * dev);

	/* phase 2 is for any magic you have to do before the busses are scanned */
	void (*phase2)(struct device * dev);

	/* phase 3 is for scanning the bus, if needed. */
	unsigned int (*phase3)(struct device * bus, unsigned int max);

	/* typically used by phase4 */
	/* again, if we never use this anywhere else, we may change the names */
	void (*phase4_read_resources)(struct device * dev);
	void (*phase4_set_resources)(struct device * dev);

	/* phase 5: enable devices */
	void (*phase5)(struct device * dev);

	/* phase 6: any post-setup device initialization that might be needed */
	void (*phase6)();

	const struct pci_operations *ops_pci;
	const struct smbus_bus_operations *ops_smbus_bus;
	const struct pci_bus_operations *ops_pci_bus;
};


struct bus {
	struct device * 	dev;		/* This bridge device */
	struct device * 	children;	/* devices behind this bridge */
	unsigned	bridge_ctrl;	/* Bridge control register */
	unsigned char	link;		/* The index of this link */
	unsigned char	secondary; 	/* secondary bus number */
	unsigned char	subordinate;	/* max subordinate bus number */
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
	struct device *	sibling;	/* next device on this bus */
	struct device *	next;		/* chain of all devices */

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

	u8 command;

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
extern struct device * alloc_dev(struct bus *parent, struct device_path *path);
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
extern const char *dev_path(struct device * dev);
const char *bus_path(struct bus *bus);
extern void dev_set_enabled(struct device * dev, int enable);
extern void disable_children(struct bus *bus);

/* Helper functions */
struct device * find_dev_path(struct bus *parent, struct device_path *path);
struct device * alloc_find_dev(struct bus *parent, struct device_path *path);
struct device * dev_find_device (unsigned int vendor, unsigned int device, struct device * from);
struct device * dev_find_class (unsigned int class, struct device * from);
struct device * dev_find_slot (unsigned int bus, unsigned int devfn);
struct device * dev_find_slot_on_smbus (unsigned int bus, unsigned int addr);


/* Rounding for boundaries. 
 * Due to some chip bugs, go ahead and roung IO to 16
 */
#define DEVICE_IO_ALIGN 16 
#define DEVICE_MEM_ALIGN 4096

struct device_operations default_dev_ops_root;
extern void root_dev_read_resources(struct device * dev);
extern void root_dev_set_resources(struct device * dev);
extern unsigned int scan_static_bus(struct device * bus, unsigned int max);
extern void enable_childrens_resources(struct device * dev);
extern void root_dev_enable_resources(struct device * dev);
extern unsigned int root_dev_scan_bus(struct device * root, unsigned int max);
extern void root_dev_init(struct device * dev);
extern void dev_init(void);
#endif /* DEVICE_H */
