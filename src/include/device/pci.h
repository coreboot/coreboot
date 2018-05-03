/*
 *	PCI defines and function prototypes
 *	Copyright 1994, Drew Eckhardt
 *	Copyright 1997--1999 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *
 *	For more information, please consult the following manuals (look at
 *	http://www.pcisig.com/ for how to get them):
 *
 *	PCI BIOS Specification
 *	PCI Local Bus Specification
 *	PCI to PCI Bridge Specification
 *	PCI System Design Guide
 */

#ifndef PCI_H
#define PCI_H

#if IS_ENABLED(CONFIG_PCI)

#include <stdint.h>
#include <stddef.h>
#include <rules.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/resource.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <device/pci_rom.h>


/* Common pci operations without a standard interface */
struct pci_operations {
	/* set the Subsystem IDs for the PCI device */
	void (*set_subsystem)(struct device *dev, unsigned int vendor,
		unsigned int device);
	void (*set_L1_ss_latency)(struct device *dev, unsigned int off);
};

/* Common pci bus operations */
struct pci_bus_operations {
	uint8_t   (*read8)(struct bus *pbus, int bus, int devfn, int where);
	uint16_t (*read16)(struct bus *pbus, int bus, int devfn, int where);
	uint32_t (*read32)(struct bus *pbus, int bus, int devfn, int where);
	void     (*write8)(struct bus *pbus, int bus, int devfn, int where,
			uint8_t val);
	void    (*write16)(struct bus *pbus, int bus, int devfn, int where,
			uint16_t val);
	void    (*write32)(struct bus *pbus, int bus, int devfn, int where,
			uint32_t val);
};

struct pci_driver {
	const struct device_operations *ops;
	unsigned short vendor;
	unsigned short device;
	const unsigned short *devices;
};

#ifdef __SIMPLE_DEVICE__
#define __pci_driver __attribute__((unused))
#else
#define __pci_driver __attribute__((used, __section__(".rodata.pci_driver")))
#endif
/** start of compile time generated pci driver array */
extern struct pci_driver _pci_drivers[];
/** end of compile time generated pci driver array */
extern struct pci_driver _epci_drivers[];

/* Set Subsystem ID operation for PCI devices */
extern struct pci_operations pci_dev_ops_pci;
extern struct device_operations default_pci_ops_dev;
extern struct device_operations default_pci_ops_bus;

void pci_dev_read_resources(struct device *dev);
void pci_bus_read_resources(struct device *dev);
void pci_dev_set_resources(struct device *dev);
void pci_dev_enable_resources(struct device *dev);
void pci_bus_enable_resources(struct device *dev);
void pci_bus_reset(struct bus *bus);
struct device *pci_probe_dev(struct device *dev, struct bus *bus,
				unsigned int devfn);

void do_pci_scan_bridge(struct device *dev,
	void (*do_scan_bus)(struct bus *bus,
		unsigned int min_devfn, unsigned int max_devfn));

void pci_scan_bridge(struct device *bus);
void pci_scan_bus(struct bus *bus, unsigned int min_devfn,
	unsigned int max_devfn);

uint8_t pci_moving_config8(struct device *dev, unsigned int reg);
uint16_t pci_moving_config16(struct device *dev, unsigned int reg);
uint32_t pci_moving_config32(struct device *dev, unsigned int reg);
struct resource *pci_get_resource(struct device *dev, unsigned long index);
void pci_dev_set_subsystem(struct device *dev, unsigned int vendor,
	unsigned int device);
void pci_dev_init(struct device *dev);
unsigned int pci_match_simple_dev(struct device *dev, pci_devfn_t sdev);

const char *pin_to_str(int pin);
int get_pci_irq_pins(struct device *dev, struct device **parent_bdg);
void pci_assign_irqs(unsigned int bus, unsigned int slot,
		     const unsigned char pIntAtoD[4]);
const char *get_pci_class_name(struct device *dev);
const char *get_pci_subclass_name(struct device *dev);

#define PCI_IO_BRIDGE_ALIGN 4096
#define PCI_MEM_BRIDGE_ALIGN (1024*1024)

static inline const struct pci_operations *ops_pci(struct device *dev)
{
	const struct pci_operations *pops;
	pops = 0;
	if (dev && dev->ops)
		pops = dev->ops->ops_pci;
	return pops;
}

#ifdef __SIMPLE_DEVICE__
unsigned int pci_find_next_capability(pci_devfn_t dev, unsigned int cap,
	unsigned int last);
unsigned int pci_find_capability(pci_devfn_t dev, unsigned int cap);
#else /* !__SIMPLE_DEVICE__ */
unsigned int pci_find_next_capability(struct device *dev, unsigned int cap,
	unsigned int last);
unsigned int pci_find_capability(struct device *dev, unsigned int cap);
#endif /* __SIMPLE_DEVICE__ */

void pci_early_bridge_init(void);
int pci_early_device_probe(u8 bus, u8 dev, u32 mmio_base);

#ifndef __ROMCC__
static inline int pci_base_address_is_memory_space(unsigned int attr)
{
	return (attr & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_MEMORY;
}
#endif

#endif /* CONFIG_PCI */

#endif /* PCI_H */
