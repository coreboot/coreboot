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

#include <stdint.h>
#include <device/pci_def.h>
#include <device/resource.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <device/pci_rom.h>

/* Common pci operations without a standard interface */
struct pci_operations {
	/* set the Subsystem IDs for the PCI device */
	void (*set_subsystem)(device_t dev, unsigned vendor, unsigned device);
};

/* Common pci bus operations */
struct pci_bus_operations {
	uint8_t (*read8)   (struct bus *pbus, int bus, int devfn, int where);
	uint16_t (*read16) (struct bus *pbus, int bus, int devfn, int where);
	uint32_t (*read32) (struct bus *pbus, int bus, int devfn, int where);
	void (*write8)  (struct bus *pbus, int bus, int devfn, int where, uint8_t val);
	void (*write16) (struct bus *pbus, int bus, int devfn, int where, uint16_t val);
	void (*write32) (struct bus *pbus, int bus, int devfn, int where, uint32_t val);
};

struct pci_driver {
	const struct device_operations *ops;
	unsigned short vendor;
	unsigned short device;
};

#define __pci_driver __attribute__ ((used,__section__(".rodata.pci_driver")))
/** start of compile time generated pci driver array */
extern struct pci_driver pci_drivers[];
/** end of compile time generated pci driver array */
extern struct pci_driver epci_drivers[];


extern struct device_operations default_pci_ops_dev;
extern struct device_operations default_pci_ops_bus;

void pci_dev_read_resources(device_t dev);
void pci_bus_read_resources(device_t dev);
void pci_dev_set_resources(device_t dev);
void pci_dev_enable_resources(device_t dev);
void pci_bus_enable_resources(device_t dev);
void pci_bus_reset(struct bus *bus);
device_t pci_probe_dev(device_t dev, struct bus *bus, unsigned devfn);
unsigned int do_pci_scan_bridge(device_t bus, unsigned int max,
	unsigned int (*do_scan_bus)(struct bus *bus,
		unsigned min_devfn, unsigned max_devfn, unsigned int max));
unsigned int pci_scan_bridge(device_t bus, unsigned int max);
unsigned int pci_scan_bus(struct bus *bus, unsigned min_devfn, unsigned max_devfn, unsigned int max);
uint8_t pci_moving_config8(struct device *dev, unsigned reg);
uint16_t pci_moving_config16(struct device *dev, unsigned reg);
uint32_t pci_moving_config32(struct device *dev, unsigned reg);
unsigned pci_find_next_capability(device_t dev, unsigned cap, unsigned last);
unsigned pci_find_capability(device_t dev, unsigned cap);
struct resource *pci_get_resource(struct device *dev, unsigned long index);
void pci_dev_set_subsystem(device_t dev, unsigned vendor, unsigned device);
void pci_dev_init(struct device *dev);

void pci_assign_irqs(unsigned bus, unsigned slot,
		     const unsigned char pIntAtoD[4]);

#define PCI_IO_BRIDGE_ALIGN 4096
#define PCI_MEM_BRIDGE_ALIGN (1024*1024)

static inline const struct pci_operations *ops_pci(device_t dev)
{
	const struct pci_operations *pops;
	pops = 0;
	if (dev && dev->ops) {
		pops = dev->ops->ops_pci;
	}
	return pops;
}

static inline const struct pci_bus_operations *ops_pci_bus(struct bus *bus)
{
	const struct pci_bus_operations *bops;
	bops = 0;
	if (bus && bus->dev && bus->dev->ops) {
		bops = bus->dev->ops->ops_pci_bus;
	}
	if (!bops)
		bops = pci_remember_direct();
	return bops;
}

unsigned mainboard_pci_subsystem_vendor_id(struct device *dev);
unsigned mainboard_pci_subsystem_device_id(struct device *dev);

#endif /* PCI_H */
