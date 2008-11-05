/*
 * This file is part of the coreboot project.
 *
 *  PCI defines and function prototypes
 *  Copyright 1994, Drew Eckhardt
 *  Copyright 1997-1999 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
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

#ifndef DEVICE_PCI_H
#define DEVICE_PCI_H

#include <types.h>
#include <device/pci_def.h>
#include <device/resource.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <device/pci_rom.h>
#include <device/pci_ids.h>

/*
 *	For more information, please consult the following manuals (look at
 *	http://www.pcisig.com/ for how to get them):
 *
 *	PCI BIOS Specification
 *	PCI Local Bus Specification
 *	PCI to PCI Bridge Specification
 *	PCI System Design Guide
 */

/* Common pci operations without a standard interface */
struct pci_operations {
	/* set the Subsystem IDs for the PCI device */
	void (*set_subsystem)(struct device * dev, unsigned vendor, unsigned device);
};

/* Common pci bus operations */
struct pci_bus_operations {
	u8 (*read8)(u32 bdf, int where);
	u16 (*read16)(u32 bdf, int where);
	u32 (*read32)(u32 bdf, int where);
	void (*write8)(u32 bdf, int where, u8 val);
	void (*write16)(u32 bdf, int where, u16 val);
	void (*write32)(u32 bdf, int where, u32 val);
	int (*find)(u16 vendid, u16 devid, u32 *busdevfn);
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


extern const struct device_operations default_pci_ops_dev;
extern const struct device_operations default_pci_ops_bus;
extern const struct pci_operations pci_dev_ops_pci;
extern const struct pci_operations pci_bus_ops_pci;

void pci_dev_read_resources(struct device * dev);
void pci_bus_read_resources(struct device * dev);
void pci_dev_set_resources(struct device * dev);
void pci_dev_enable_resources(struct device * dev);
void pci_bus_enable_resources(struct device * dev);
void pci_bus_reset(struct bus *bus);
struct device * pci_probe_dev(struct device * dev, struct bus *bus, unsigned devfn);
unsigned int do_pci_scan_bridge(struct device * bus, unsigned int max,
	unsigned int (*do_scan_bus)(struct bus *bus, 
		unsigned min_devfn, unsigned max_devfn, unsigned int max));
unsigned int pci_scan_bridge(struct device * bus, unsigned int max);
unsigned int pci_scan_bus(struct bus *bus, unsigned min_devfn, unsigned max_devfn, unsigned int max);
u8 pci_moving_config8(struct device *dev, unsigned reg);
u16 pci_moving_config16(struct device *dev, unsigned reg);
u32 pci_moving_config32(struct device *dev, unsigned reg);
unsigned pci_find_next_capability(struct device * dev, unsigned cap, unsigned last);
unsigned pci_find_capability(struct device * dev, unsigned cap);
struct resource *pci_get_resource(struct device *dev, unsigned long index);
void pci_dev_set_subsystem(struct device * dev, unsigned vendor, unsigned device);

void pci_domain_read_resources(struct device *dev);
void ram_resource(struct device *dev, unsigned long index,
                  unsigned long basek, unsigned long sizek);
unsigned int pci_domain_scan_bus(struct device *dev, unsigned int max);
void pci_assign_irqs(unsigned int bus, unsigned int slot,
		     const unsigned char pIntAtoD[4]);



#define PCI_IO_BRIDGE_ALIGN 4096
#define PCI_MEM_BRIDGE_ALIGN (1024*1024)

#define PCI_BUS_SEGN_BITS 0

static inline const struct pci_operations *ops_pci(struct device * dev)
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
	return bops;
}

#endif /* DEVICE_PCI_H */
