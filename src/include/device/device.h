#ifndef DEVICE_H

#define DEVICE_H

#include <device/resource.h>
#include <device/path.h>
#include <device/pci_type.h>
#include <smbios.h>
#include <static.h>
#include <types.h>

struct fw_config;
struct device;
struct pci_operations;
struct i2c_bus_operations;
struct smbus_bus_operations;
struct pnp_mode_ops;
struct spi_bus_operations;
struct usb_bus_operations;
struct gpio_operations;

/* Chip operations */
struct chip_operations {
	void (*enable_dev)(struct device *dev);
	void (*init)(void *chip_info);
	void (*final)(void *chip_info);
	unsigned int initialized : 1;
	unsigned int finalized : 1;
	const char *name;
};

#define CHIP_NAME(X) .name = X,

struct bus;

struct acpi_rsdp;

struct device_operations {
	void (*read_resources)(struct device *dev);
	void (*set_resources)(struct device *dev);
	void (*enable_resources)(struct device *dev);
	void (*init)(struct device *dev);
	void (*final)(struct device *dev);
	void (*scan_bus)(struct device *bus);
	void (*enable)(struct device *dev);
	void (*vga_disable)(struct device *dev);
	void (*reset_bus)(struct bus *bus);
#if CONFIG(GENERATE_SMBIOS_TABLES)
	int (*get_smbios_data)(struct device *dev, int *handle,
		unsigned long *current);
	void (*get_smbios_strings)(struct device *dev, struct smbios_type11 *t);
#endif
#if CONFIG(HAVE_ACPI_TABLES)
	unsigned long (*write_acpi_tables)(const struct device *dev,
		unsigned long start, struct acpi_rsdp *rsdp);
	void (*acpi_fill_ssdt)(const struct device *dev);
	void (*acpi_inject_dsdt)(const struct device *dev);
	const char *(*acpi_name)(const struct device *dev);
	/* Returns the optional _HID (Hardware ID) */
	const char *(*acpi_hid)(const struct device *dev);
#endif
	const struct pci_operations *ops_pci;
	const struct i2c_bus_operations *ops_i2c_bus;
	const struct spi_bus_operations *ops_spi_bus;
	const struct smbus_bus_operations *ops_smbus_bus;
	const struct pnp_mode_ops *ops_pnp_mode;
	const struct gpio_operations *ops_gpio;
};

/**
 * Standard device operations function pointers shims.
 */
static inline void noop_read_resources(struct device *dev) {}
static inline void noop_set_resources(struct device *dev) {}

struct bus {

	DEVTREE_CONST struct device *dev;	/* This bridge device */
	DEVTREE_CONST struct device *children;	/* devices behind this bridge */
	DEVTREE_CONST struct bus *next;    /* The next bridge on this device */
	unsigned int	bridge_ctrl;	/* Bridge control register */
	uint16_t	bridge_cmd;		/* Bridge command register */
	unsigned char	link_num;	/* The index of this link */
	uint16_t	secondary;	/* secondary bus number */
	uint16_t	subordinate;	/* max subordinate bus number */
	unsigned char   cap;		/* PCi capability offset */
	uint32_t	hcdn_reg;		/* For HyperTransport link  */

	unsigned int	reset_needed : 1;
	unsigned int	disable_relaxed_ordering : 1;
	unsigned int	ht_link_up : 1;
	unsigned int	no_vga16 : 1;	/* No support for 16-bit VGA decoding */
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
	DEVTREE_CONST struct bus *bus;	/* bus this device is on, for bridge
					 * devices, it is the up stream bus */

	DEVTREE_CONST struct device *sibling;	/* next device on this bus */

	DEVTREE_CONST struct device *next;	/* chain of all devices */

	struct device_path path;
	unsigned int	vendor;
	unsigned int	device;
	u16		subsystem_vendor;
	u16		subsystem_device;
	unsigned int	class;		/* 3 bytes: (base, sub, prog-if) */
	unsigned int	hdr_type;	/* PCI header type */
	unsigned int    enabled : 1;	/* set if we should enable the device */
	unsigned int  initialized : 1; /* 1 if we have initialized the device */
	unsigned int    on_mainboard : 1;
	unsigned int    disable_pcie_aspm : 1;
	/* set if we should hide from UI */
	unsigned int    hidden : 1;
	/* set if this device is used even in minimum PCI cases */
	unsigned int    mandatory : 1;
	u8 command;
	uint16_t hotplug_buses; /* Number of hotplug buses to allocate */

	/* Base registers for this device. I/O, MEM and Expansion ROM */
	DEVTREE_CONST struct resource *resource_list;

	/* links are (downstream) buses attached to the device, usually a leaf
	 * device with no children has 0 buses attached and a bridge has 1 bus
	 */
	DEVTREE_CONST struct bus *link_list;

#if !DEVTREE_EARLY
	struct pci_irq_info pci_irq_info[4];
	struct device_operations *ops;
	struct chip_operations *chip_ops;
	const char *name;
#if CONFIG(GENERATE_SMBIOS_TABLES)
	u8 smbios_slot_type;
	u8 smbios_slot_data_width;
	u8 smbios_slot_length;
	const char *smbios_slot_designation;

#if CONFIG(SMBIOS_TYPE41_PROVIDED_BY_DEVTREE)
	/*
	 * These fields are intentionally guarded so that attempts to use
	 * the corresponding devicetree syntax without selecting the Kconfig
	 * option result in build-time errors. Smaller size is a side effect.
	 */
	bool smbios_instance_id_valid;
	u8 smbios_instance_id;
	const char *smbios_refdes;
#endif
#endif
#endif
	DEVTREE_CONST void *chip_info;

	/* Zero-terminated array of fields and options to probe. */
	DEVTREE_CONST struct fw_config *probe_list;
};

/**
 * This is the root of the device tree. The device tree is defined in the
 * static.c file and is generated by the config tool at compile time.
 */
extern DEVTREE_CONST struct device	dev_root;
/* list of all devices */
extern DEVTREE_CONST struct device * DEVTREE_CONST all_devices;
extern struct resource	*free_resources;
extern struct bus	*free_links;

extern const char mainboard_name[];

/* Generic device interface functions */
struct device *alloc_dev(struct bus *parent, struct device_path *path);
void dev_initialize_chips(void);
void dev_enumerate(void);
void dev_configure(void);
void dev_enable(void);
void dev_initialize(void);
void dev_finalize(void);
void dev_finalize_chips(void);
/* Function used to override device state */
void devfn_disable(const struct bus *bus, unsigned int devfn);

/* Generic device helper functions */
int reset_bus(struct bus *bus);
void scan_bridges(struct bus *bus);
void assign_resources(struct bus *bus);
const char *dev_name(const struct device *dev);
const char *dev_path(const struct device *dev);
u32 dev_path_encode(const struct device *dev);
const char *bus_path(struct bus *bus);
void dev_set_enabled(struct device *dev, int enable);
void disable_children(struct bus *bus);
bool dev_is_active_bridge(struct device *dev);
void add_more_links(struct device *dev, unsigned int total_links);
bool is_dev_enabled(const struct device *const dev);
bool is_devfn_enabled(unsigned int devfn);

/* Option ROM helper functions */
void run_bios(struct device *dev, unsigned long addr);

/* Helper functions */
DEVTREE_CONST struct device *find_dev_path(
		const struct bus *parent,
		const struct device_path *path);
DEVTREE_CONST struct device *find_dev_nested_path(
		const struct bus *parent,
		const struct device_path nested_path[],
		size_t nested_path_length);
struct device *alloc_find_dev(struct bus *parent, struct device_path *path);
struct device *dev_find_device(u16 vendor, u16 device, struct device *from);
struct device *dev_find_class(unsigned int class, struct device *from);
DEVTREE_CONST struct device *dev_find_path(
		DEVTREE_CONST struct device *prev_match,
		enum device_path_type path_type);
struct device *dev_find_lapic(unsigned int apic_id);
int dev_count_cpu(void);
struct device *add_cpu_device(struct bus *cpu_bus, unsigned int apic_id,
				int enabled);
void set_cpu_topology(struct device *cpu, unsigned int node,
	unsigned int package, unsigned int core, unsigned int thread);

#define amd_cpu_topology(cpu, node, core) \
	set_cpu_topology(cpu, node, 0, core, 0)

#define intel_cpu_topology(cpu, package, core, thread) \
	set_cpu_topology(cpu, 0, package, core, thread)

void mp_init_cpus(DEVTREE_CONST struct bus *cpu_bus);
static inline void mp_cpu_bus_init(struct device *dev)
{
	/*
	 * When no LAPIC device is specified in the devietree inside the CPU cluster device,
	 * neither a LAPIC device nor the link/bus between the CPU cluster and the LAPIC device
	 * will be present in the static device tree and the link_list struct element of the
	 * CPU cluster device will be NULL. In this case add one link, so that the
	 * alloc_find_dev calls in init_bsp and allocate_cpu_devices will be able to add a
	 * LAPIC device for the BSP and the APs on this link/bus.
	 */
	if (!dev->link_list)
		add_more_links(dev, 1);

	mp_init_cpus(dev->link_list);
}

/* Debug functions */
void print_resource_tree(const struct device *root, int debug_level,
			 const char *msg);
void show_devs_tree(const struct device *dev, int debug_level, int depth);
void show_devs_subtree(struct device *root, int debug_level, const char *msg);
void show_all_devs(int debug_level, const char *msg);
void show_all_devs_tree(int debug_level, const char *msg);
void show_one_resource(int debug_level, struct device *dev,
		       struct resource *resource, const char *comment);
void show_all_devs_resources(int debug_level, const char *msg);

/* Debug macros */
#if CONFIG(DEBUG_RESOURCES)
#include <console/console.h>
#define LOG_MEM_RESOURCE(type, dev, index, base_kb, size_kb) \
	printk(BIOS_SPEW, "%s:%d res: %s, dev: %s, index: 0x%x, base: 0x%llx, " \
		"end: 0x%llx, size_kb: 0x%llx\n", \
		__func__, __LINE__, type, dev_path(dev), index, (base_kb << 10), \
		(base_kb << 10) + (size_kb << 10) - 1, size_kb)

#define LOG_IO_RESOURCE(type, dev, index, base, size) \
	printk(BIOS_SPEW, "%s:%d res: %s, dev: %s, index: 0x%x, base: 0x%llx, " \
		"end: 0x%llx, size: 0x%llx\n", \
		__func__, __LINE__, type, dev_path(dev), index, base, base + size - 1, size)
#else /* DEBUG_RESOURCES*/
#define LOG_MEM_RESOURCE(type, dev, index, base_kb, size_kb)
#define LOG_IO_RESOURCE(type, dev, index, base, size)
#endif /* DEBUG_RESOURCES*/

#if CONFIG(DEBUG_FUNC)
#include <console/console.h>
#define DEV_FUNC_ENTER(dev) \
	printk(BIOS_SPEW, "%s:%s:%d: ENTER (dev: %s)\n", \
		__FILE__, __func__, __LINE__, dev_path(dev))

#define DEV_FUNC_EXIT(dev) \
	printk(BIOS_SPEW, "%s:%s:%d: EXIT (dev: %s)\n", __FILE__, \
		__func__, __LINE__, dev_path(dev))
#else /* DEBUG_FUNC */
#define DEV_FUNC_ENTER(dev)
#define DEV_FUNC_EXIT(dev)
#endif /* DEBUG_FUNC */

/* Rounding for boundaries.
 * Due to some chip bugs, go ahead and round IO to 16
 */
#define DEVICE_IO_ALIGN 16
#define DEVICE_MEM_ALIGN 4096

extern struct device_operations default_dev_ops_root;
void pci_domain_read_resources(struct device *dev);
void pci_domain_set_resources(struct device *dev);
void pci_domain_scan_bus(struct device *dev);

void fixed_io_resource(struct device *dev, unsigned long index,
		unsigned long base, unsigned long size);

void mmconf_resource(struct device *dev, unsigned long index);

/* These are temporary resource constructors to get us through the
   migration away from open-coding all the IORESOURCE_FLAGS. */

const struct resource *fixed_resource_range_idx(struct device *dev, unsigned long index,
					    uint64_t base, uint64_t size,
					    unsigned long flags);

static inline
const struct resource *fixed_mem_range_flags(struct device *dev, unsigned long index,
					    uint64_t base, uint64_t size,
					    unsigned long flags)
{
	return fixed_resource_range_idx(dev, index, base, size, IORESOURCE_MEM | flags);
}

static inline
const struct resource *fixed_mem_from_to_flags(struct device *dev, unsigned long index,
					uint64_t base, uint64_t end, unsigned long flags)
{
	if (end <= base)
		return NULL;
	return fixed_mem_range_flags(dev, index, base, end - base, flags);
}

/* Compatibility code */

static inline void fixed_mem_resource_kb(struct device *dev, unsigned long index,
			unsigned long basek, unsigned long sizek,
			unsigned long flags)
{
	fixed_mem_range_flags(dev, index, ((uint64_t)basek) << 10,
		((uint64_t)sizek) << 10, IORESOURCE_STORED | flags);
}

/* It is the caller's responsibility to adjust regions such that ram_resource_kb()
 * and mmio_resource_kb() do not overlap.
 */
#define ram_resource_kb(dev, idx, basek, sizek) \
	fixed_mem_resource_kb(dev, idx, basek, sizek, IORESOURCE_CACHEABLE)

#define reserved_ram_resource_kb(dev, idx, basek, sizek) \
	fixed_mem_resource_kb(dev, idx, basek, sizek, IORESOURCE_CACHEABLE \
		| IORESOURCE_RESERVE)

#define bad_ram_resource_kb(dev, idx, basek, sizek) \
	reserved_ram_resource_kb((dev), (idx), (basek), (sizek))

#define uma_resource_kb(dev, idx, basek, sizek) \
	fixed_mem_resource_kb(dev, idx, basek, sizek, IORESOURCE_RESERVE)

#define mmio_resource_kb(dev, idx, basek, sizek) \
	fixed_mem_resource_kb(dev, idx, basek, sizek, IORESOURCE_RESERVE)

#define io_resource(dev, idx, base, size) \
	fixed_io_resource(dev, idx, base, size)

void tolm_test(void *gp, struct device *dev, struct resource *new);
u32 find_pci_tolm(struct bus *bus);

DEVTREE_CONST struct device *dev_find_next_pci_device(
				DEVTREE_CONST struct device *previous_dev);
DEVTREE_CONST struct device *dev_find_slot_on_smbus(unsigned int bus,
							unsigned int addr);
DEVTREE_CONST struct device *dev_find_slot_pnp(u16 port, u16 device);
DEVTREE_CONST struct device *dev_bus_each_child(const struct bus *parent,
				DEVTREE_CONST struct device *prev_child);

DEVTREE_CONST struct device *pcidev_path_behind(const struct bus *parent,
		pci_devfn_t devfn);
DEVTREE_CONST struct device *pcidev_path_on_root(pci_devfn_t devfn);
DEVTREE_CONST struct device *pcidev_path_on_bus(unsigned int bus, pci_devfn_t devfn);
DEVTREE_CONST struct device *pcidev_on_root(uint8_t dev, uint8_t fn);
DEVTREE_CONST struct bus *pci_root_bus(void);
/* Find PCI device with given D#:F# sitting behind the given PCI-to-PCI bridge device. */
DEVTREE_CONST struct device *pcidev_path_behind_pci2pci_bridge(
							const struct device *bridge,
							pci_devfn_t devfn);

/* To be deprecated, avoid using.
 *
 * Note that this function can return the incorrect device prior
 * to PCI enumeration because the secondary field of the bus object
 * is 0. The failing scenario is determined by the order of the
 * devices in all_devices singly-linked list as well as the time
 * when this function is called (secondary reflecting topology).
 */
DEVTREE_CONST struct device *pcidev_path_on_root_debug(pci_devfn_t devfn, const char *func);

/* Robust discovery of chip_info. */
void devtree_bug(const char *func, pci_devfn_t devfn);
void __noreturn devtree_die(void);

/*
 * Dies if `dev` or `dev->chip_info` are NULL. Returns `dev->chip_info` otherwise.
 *
 * Only use if missing `chip_info` is fatal and we can't boot. If it's
 * not fatal, please handle the NULL case gracefully.
 */
static inline DEVTREE_CONST void *config_of(const struct device *dev)
{
	if (dev && dev->chip_info)
		return dev->chip_info;

	devtree_die();
}

/*
 * Returns pointer to config structure of root device (B:D:F = 0:00:0) defined by
 * sconfig in static.{h/c}.
 */
#define config_of_soc()		__pci_0_00_0_config

void enable_static_device(struct device *dev);
void enable_static_devices(struct device *bus);
void scan_smbus(struct device *bus);
void scan_generic_bus(struct device *bus);
void scan_static_bus(struct device *bus);

/* Macro to generate `struct device *` name that points to a device with the given alias. */
#define DEV_PTR(_alias)		_dev_##_alias##_ptr

/* Macro to generate weak `struct device *` definition that points to a device with the given
   alias. */
#define WEAK_DEV_PTR(_alias)			\
	__weak DEVTREE_CONST struct device *const DEV_PTR(_alias)

#endif /* DEVICE_H */
