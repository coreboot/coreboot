/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#ifndef DEVICE_DEVICE_H
#define DEVICE_DEVICE_H

#include <types.h>
#include <device/resource.h>
#include <device/path.h>

/**
 * Create a 32-bit value from four characters. This is better
 * than the usual enum values when using (JTAG) debuggers.
 * It also makes it harder for accidentally assigned type values
 * to be mistaken for a real value -- e.g. it is more likely in the event
 * of a programming error that a '1' is somehow assigned
 * to the type field, whereas these values are more complex. 
 * Thus errors may be easier to find. 
 */
#define TYPENAME(a,b,c,d) ((a<<24)|(b<<16)|(c<<8)|(d))
#define DEVICE_ID_MAX 64
enum device_id_type {
	DEVICE_ID_NONE	= 0,
	DEVICE_ID_ROOT	= TYPENAME('R','O','O','T'),
	DEVICE_ID_PCI	= TYPENAME(' ','P','C','I'),
	DEVICE_ID_PNP	= TYPENAME(' ','P','N','P'),
	DEVICE_ID_I2C	= TYPENAME(' ','I','2','C'),
	DEVICE_ID_APIC	= TYPENAME('A','P','I','C'),
	DEVICE_ID_PCI_DOMAIN = TYPENAME('P','C','I','D'),
	DEVICE_ID_APIC_CLUSTER = TYPENAME('A','P','C','C'),
	DEVICE_ID_CPU = TYPENAME(' ','C','P','U'),
	DEVICE_ID_CPU_BUS =  TYPENAME(' ','B','U','S'),
};

struct device;
struct pci_operations;
struct pci_bus_operations;
struct smbus_bus_operations;
struct bus;



struct pci_domain_id
{
	u16 vendor, device;
};

struct pci_id
{
	u16 vendor, device;
};

struct pnp_id
{
	u32 device;
};

struct i2c_id
{
	u32 id;
};

struct apic_id
{
	u16 vendor, device;
};

struct apic_cluster_id
{
	u16 vendor, device;
};

struct cpu_id
{
	u32 cpuid[3];
};

struct cpu_bus_id
{
	u16 vendor, device;
};

struct device_id {
	enum device_id_type type;
	union {
		struct pci_id			pci;
		struct pnp_id			pnp;
		struct i2c_id			i2c;
		struct apic_id			apic;
		struct pci_domain_id	pci_domain;
		struct apic_cluster_id	apic_cluster;
		struct cpu_id			cpu;
		struct cpu_bus_id		cpu_bus;
	} u;
};


struct device_operations {
	/* the device id for this set of device operations. 
	 * In almost all cases, this is non-zero. For the 
	 * default_device_constructor, it's zero
	 */
	struct device_id id;
	/* for now, we leave these, since they seem generic */
	void (*set_link)(struct device * dev, unsigned int link);
	void (*reset_bus)(struct bus *bus);

	/* A constructor. The constructor for a given device is defined in the
	 * device source file.  When is this called? Not for the static tree.
	 * When the scan bus code finds a new device, it must create it and
	 * insert it into the device tree. To initialize it, it calls a device
	 * constructor.  The set of all device constructors is concatenated
	 * into the constructors array of structures.
	 *
	 * The dev_constructor code in device.c iterates over the constructors
	 * array.  A match consists of a path type, a vendor (which may be
	 * ignored if the constructor vendor value is 0), and a device id.
	 * When it finds a match, the dev_constructor calls the function
	 * constructors->constructor(constructors->constructor) and a new
	 * device is created. 
	 */
	void (*constructor)(struct device *, struct device_operations *);

	/* set device ops */
	void (*phase1_set_device_operations)(struct device *dev);

	/* phase 2 is for any magic you have to do before the busses are scanned */
	void (*phase2_setup_scan_bus)(struct device * dev);

	/* phase 3 is for scanning the bus, if needed. */
	void (*phase3_enable_scan)(struct device *dev);
	unsigned int (*phase3_scan)(struct device * bus, unsigned int max);

	/* typically used by phase4 */
	/* again, if we never use this anywhere else, we may change the names */
	void (*phase4_read_resources)(struct device * dev);
	void (*phase4_set_resources)(struct device * dev);
	/* some devices need to be enabled to scan, then disabled again. */
	/* this function enables/disables according the value of 'enabled' in the device*/
	void (*phase4_enable_disable)(struct device * dev);

	/* phase 5: enable devices */
	void (*phase5_enable_resources)(struct device * dev);

	/* phase 6: any post-setup device initialization that might be needed */
	void (*phase6_init)(struct device * dev);

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
#define MAX_DTSNAME_SIZE 64
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
	/* note there is a device id maintained here. This covers the special case
	 * of default_device_operations, which has an id of zero. 
	 */
	struct device_id id;
	char 		dtsname[MAX_DTSNAME_SIZE];	/* the name from the dts */
	u16 status;
	u8 revision;
	u8 cache_line;
	u8 irq_line;
	u8 irq_pin;
	u8 min_gnt;
	u8 max_lat;
	u16 subsystem_vendor;
	u16 subsystem_device;

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

	/* link are (downstream) buses attached to the device, usually a leaf
	 * device with no children have 0 buses attached and a bridge has 1 bus 
	 */
	struct bus link[MAX_LINKS];
	/* number of buses attached to the device */
	unsigned int links;

	struct device_operations *ops;
	void *device_configuration;
};

extern struct device	dev_root;	/* root bus */
extern struct device	*all_devices;	/* list of all devices */


/* Generic device interface functions */
struct device_operations *find_device_operations(struct device_id *id);
struct device * alloc_dev(struct bus *parent, struct device_path *path, struct device_id *id);
void dev_enumerate(void);
void dev_configure(void);
void dev_enable(void);
void dev_initialize(void);
void dev_optimize(void);

/* Generic device helper functions */
int reset_bus(struct bus *bus);
unsigned int scan_bus(struct device *bus, unsigned int max);
void compute_allocate_resource(struct bus *bus, struct resource *bridge,
	unsigned long type_mask, unsigned long type);
void assign_resources(struct bus *bus);
void enable_resources(struct device *dev);
void enumerate_static_device(void);
void enumerate_static_devices(void);
const char *dev_path(struct device * dev);
const char *dev_id_string(struct device_id *id);
const char *bus_path(struct bus *bus);
void dev_set_enabled(struct device * dev, int enable);
void disable_children(struct bus *bus);

/* Helper functions */
struct device * find_dev_path(struct bus *parent, struct device_path *path);
struct device * alloc_find_dev(struct bus *parent, struct device_path *path, struct device_id *id);
struct device * dev_find_device (struct device_id *devid, struct device * from);
struct device *dev_find_pci_device(u16 vendor, u16 device, struct device *from);
struct device * dev_find_class (unsigned int class, struct device * from);
struct device * dev_find_slot (unsigned int bus, unsigned int devfn);
struct device * dev_find_slot_on_smbus (unsigned int bus, unsigned int addr);
void default_device_constructor(struct device *dev, struct device_operations *constructor);


/* Rounding for boundaries. 
 * Due to some chip bugs, go ahead and roung IO to 16
 */
#define DEVICE_IO_ALIGN 16 
#define DEVICE_MEM_ALIGN 4096

resource_t align_up(resource_t val, unsigned long gran);
resource_t align_down(resource_t val, unsigned long gran);

extern const struct device_operations default_dev_ops_root;

extern int id_eq(struct device_id *id1, struct device_id *id2);
void root_dev_read_resources(struct device * dev);
void root_dev_set_resources(struct device * dev);
unsigned int scan_static_bus(struct device * bus, unsigned int max);
void enable_childrens_resources(struct device * dev);
void root_dev_enable_resources(struct device * dev);
unsigned int root_dev_scan_bus(struct device * root, unsigned int max);
void root_dev_init(struct device * dev);
void dev_init(void);
void dev_phase1(void);
void dev_phase2(void);
void dev_root_phase3(void);
unsigned int dev_phase3_scan(struct device * busdevice, unsigned int max);
void dev_phase4(void);
void dev_root_phase5(void);
void dev_phase6(void);

void phase4_assign_resources(struct bus *bus);
unsigned int dev_phase3(struct device * bus, unsigned int max);
void dev_phase5(struct device *dev);

#endif /* DEVICE_DEVICE_H */
