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

#include <device/pci_def.h>
#include <device/resource.h>
#include <device/device.h>
#include <device/pci_ops.h>


/* Common pci operations without a standard interface */
struct pci_operations {
	void (*set_subsystem)(device_t dev, unsigned vendor, unsigned device);
};

struct pci_driver {
	struct device_operations *ops;
	unsigned short vendor;
	unsigned short device;
};

#define __pci_driver __attribute__ ((used,__section__(".rodata.pci_driver")))
/** start of compile time generated pci driver array */
extern struct pci_driver pci_drivers[];
/** end of compile time generated pci driver array */
extern struct pci_driver epci_drivers[];


struct device_operations default_pci_ops_dev;
struct device_operations default_pci_ops_bus;

void pci_dev_read_resources(device_t dev);
void pci_bus_read_resources(device_t dev);
void pci_dev_set_resources(device_t dev);
void pci_dev_enable_resources(device_t dev);
void pci_bus_enable_resources(device_t dev);
unsigned int pci_scan_bridge(device_t bus, unsigned int max);
unsigned int pci_scan_bus(struct bus *bus, unsigned min_devfn, unsigned max_devfn, unsigned int max);
struct resource *pci_get_resource(struct device *dev, unsigned long index);

#define PCI_IO_BRIDGE_ALIGN 4096
#define PCI_MEM_BRIDGE_ALIGN (1024*1024)

static inline struct pci_operations *ops_pci(device_t dev)
{
	struct pci_operations *pops;
	pops = 0;
	if (dev && dev->ops) {
		pops = dev->ops->ops_pci;
	}
	return pops;
}

#endif /* PCI_H */
