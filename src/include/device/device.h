#ifndef DEVICE_H
#define DEVICE_H

#ifndef __SMM__
#include <stdint.h>
#include <stddef.h>
#include <device/resource.h>
#include <device/path.h>

struct device;
#ifndef __PRE_RAM__
typedef struct device * device_t;
typedef unsigned int simple_device_t;
struct pci_operations;
struct pci_bus_operations;
struct smbus_bus_operations;

/* Chip operations */
struct chip_operations {
	void (*enable_dev)(struct device *dev);
	void (*init)(void *chip_info);
	unsigned int initialized : 1;
	const char *name;
};

#define CHIP_NAME(X) .name = X,

struct bus;

struct device_operations {
	void (*read_resources)(device_t dev);
	void (*set_resources)(device_t dev);
	void (*enable_resources)(device_t dev);
	void (*init)(device_t dev);
	unsigned int (*scan_bus)(device_t bus, unsigned int max);
	void (*enable)(device_t dev);
	void (*disable)(device_t dev);
	void (*set_link)(device_t dev, unsigned int link);
	void (*reset_bus)(struct bus *bus);
#if CONFIG_GENERATE_SMBIOS_TABLES
	int (*get_smbios_data)(device_t dev, int *handle, unsigned long *current);
#endif
	const struct pci_operations *ops_pci;
	const struct smbus_bus_operations *ops_smbus_bus;
	const struct pci_bus_operations *ops_pci_bus;
};
#endif


struct bus {

	ROMSTAGE_CONST struct device * 	dev;		/* This bridge device */
	ROMSTAGE_CONST struct device * 	children;	/* devices behind this bridge */
	ROMSTAGE_CONST struct bus	*next;		/* The next bridge on this device */
	unsigned	bridge_ctrl;	/* Bridge control register */
	unsigned char	link_num;	/* The index of this link */
	uint16_t	secondary; 	/* secondary bus number */
	uint16_t	subordinate;	/* max subordinate bus number */
	unsigned char   cap;		/* PCi capability offset */
	unsigned	reset_needed : 1;
	unsigned	disable_relaxed_ordering : 1;
};

/*
 * There is one device structure for each slot-number/function-number
 * combination:
 */

struct pci_irq_info {
	unsigned int	ioapic_irq_pin;
	unsigned int	ioapic_src_pin;
	unsigned int	ioapic_dst_id;
	unsigned int    ioapic_flags;
};

struct device {
	ROMSTAGE_CONST struct bus *	bus;		/* bus this device is on, for bridge
					 * devices, it is the up stream bus */

	ROMSTAGE_CONST struct device *	sibling;	/* next device on this bus */

	ROMSTAGE_CONST struct device *	next;		/* chain of all devices */

	struct device_path path;
	unsigned 	vendor;
	unsigned 	device;
	u16		subsystem_vendor;
	u16		subsystem_device;
	unsigned int	class;		/* 3 bytes: (base, sub, prog-if) */
	unsigned int	hdr_type;	/* PCI header type */
	unsigned int    enabled : 1;	/* set if we should enable the device */
	unsigned int    initialized : 1; /* set if we have initialized the device */
	unsigned int    on_mainboard : 1;
	struct pci_irq_info pci_irq_info[4];
	u8 command;

	/* Base registers for this device. I/O, MEM and Expansion ROM */
	ROMSTAGE_CONST struct resource *resource_list;

	/* links are (downstream) buses attached to the device, usually a leaf
	 * device with no children has 0 buses attached and a bridge has 1 bus
	 */
	ROMSTAGE_CONST struct bus *link_list;

	struct device_operations *ops;
#ifndef __PRE_RAM__
	struct chip_operations *chip_ops;
	const char *name;
#endif
	ROMSTAGE_CONST void *chip_info;
};

/**
 * This is the root of the device tree. The device tree is defined in the
 * static.c file and is generated by the config tool at compile time.
 */
extern ROMSTAGE_CONST struct device	dev_root;
#ifndef __PRE_RAM__
extern struct device	*all_devices;	/* list of all devices */

extern struct resource	*free_resources;
extern struct bus	*free_links;

extern const char mainboard_name[];

/* IGD UMA memory */
extern uint64_t uma_memory_base;
extern uint64_t uma_memory_size;

/* Generic device interface functions */
device_t alloc_dev(struct bus *parent, struct device_path *path);
void dev_initialize_chips(void);
void dev_enumerate(void);
void dev_configure(void);
void dev_enable(void);
void dev_initialize(void);
void dev_optimize(void);

/* Generic device helper functions */
int reset_bus(struct bus *bus);
unsigned int scan_bus(struct device *bus, unsigned int max);
void assign_resources(struct bus *bus);
void enumerate_static_device(void);
void enumerate_static_devices(void);
const char *dev_name(device_t dev);
const char *dev_path(device_t dev);
const char *bus_path(struct bus *bus);
void dev_set_enabled(device_t dev, int enable);
void disable_children(struct bus *bus);

/* Option ROM helper functions */
void run_bios(struct device *dev, unsigned long addr);

/* Helper functions */
device_t find_dev_path(struct bus *parent, struct device_path *path);
device_t alloc_find_dev(struct bus *parent, struct device_path *path);
device_t dev_find_device (u16 vendor, u16 device, device_t from);
device_t dev_find_class (unsigned int class, device_t from);
device_t dev_find_slot (unsigned int bus, unsigned int devfn);
device_t dev_find_slot_on_smbus (unsigned int bus, unsigned int addr);
device_t dev_find_lapic(unsigned apic_id);
int dev_count_cpu(void);

void remap_bsp_lapic(struct bus *cpu_bus);
device_t add_cpu_device(struct bus *cpu_bus, unsigned apic_id, int enabled);
void set_cpu_topology(device_t cpu, unsigned node, unsigned package, unsigned core, unsigned thread);

#define amd_cpu_topology(cpu, node, core) \
	set_cpu_topology(cpu, node, 0, core, 0)

#define intel_cpu_topology(cpu, package, core, thread) \
	set_cpu_topology(cpu, 0, package, core, thread)

/* Debug functions */
void print_resource_tree(struct device * root, int debug_level,
			 const char *msg);
void show_devs_tree(struct device *dev, int debug_level, int depth, int linknum);
void show_devs_subtree(struct device *root, int debug_level, const char *msg);
void show_all_devs(int debug_level, const char *msg);
void show_all_devs_tree(int debug_level, const char *msg);
void show_one_resource(int debug_level, struct device *dev,
		       struct resource *resource, const char *comment);
void show_all_devs_resources(int debug_level, const char* msg);

/* Rounding for boundaries.
 * Due to some chip bugs, go ahead and round IO to 16
 */
#define DEVICE_IO_ALIGN 16
#define DEVICE_MEM_ALIGN 4096

extern struct device_operations default_dev_ops_root;
void pci_domain_read_resources(struct device *dev);
unsigned int pci_domain_scan_bus(struct device *dev, unsigned int max);
unsigned int scan_static_bus(device_t bus, unsigned int max);

void fixed_mem_resource(device_t dev, unsigned long index,
		  unsigned long basek, unsigned long sizek, unsigned long type);


/* It is the caller's responsibility to adjust regions such that ram_resource()
 * and mmio_resource() do not overlap.
 *
 * Current MTRR setup creates exclusive uncacheable holes for uma_resource()
 * only and these are allowed to overlap any ram_resource(). This approach
 * is used for all UMA except Intel Sandy/IvyBridge.
 */
#define ram_resource(dev, idx, basek, sizek) \
	fixed_mem_resource(dev, idx, basek, sizek, IORESOURCE_CACHEABLE)

#define reserved_ram_resource(dev, idx, basek, sizek) \
	fixed_mem_resource(dev, idx, basek, sizek, IORESOURCE_CACHEABLE | IORESOURCE_RESERVE)

#define bad_ram_resource(dev, idx, basek, sizek) \
	reserved_ram_resource((dev), (idx), (basek), (sizek))

#define uma_resource(dev, idx, basek, sizek) \
	fixed_mem_resource(dev, idx, basek, sizek, IORESOURCE_RESERVE)

#define mmio_resource(dev, idx, basek, sizek) \
	fixed_mem_resource(dev, idx, basek, sizek, IORESOURCE_RESERVE)

void tolm_test(void *gp, struct device *dev, struct resource *new);
u32 find_pci_tolm(struct bus *bus);
#else
ROMSTAGE_CONST struct device * dev_find_slot (unsigned int bus,
						unsigned int devfn);
ROMSTAGE_CONST struct device * dev_find_slot_on_smbus (unsigned int bus,
							unsigned int addr);
#endif
#else /* __SMM__ */
#include <arch/io.h>
#endif /* __SMM__ */
#endif /* DEVICE_H */
