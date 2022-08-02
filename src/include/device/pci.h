/* SPDX-License-Identifier: GPL-2.0-only */

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

#if CONFIG(PCI)

#include <stdint.h>
#include <stddef.h>
#include <device/pci_def.h>
#include <device/resource.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <device/pci_rom.h>
#include <device/pci_type.h>

/* Common pci operations without a standard interface */
struct pci_operations {
	/* set the Subsystem IDs for the PCI device */
	void (*set_subsystem)(struct device *dev, unsigned int vendor,
		unsigned int device);
	void (*get_ltr_max_latencies)(u16 *max_snoop, u16 *max_nosnoop);
};

struct pci_driver {
	const struct device_operations *ops;
	unsigned short vendor;
	unsigned short device;
	const unsigned short *devices;
};

struct msix_entry {
	union {
		struct {
			u32 lower_addr;
			u32 upper_addr;
		};
		struct {
			u64 addr;
		};
	};
	u32 data;
	u32 vec_control;
};

#if ENV_RAMSTAGE
#define __pci_driver __attribute__((used, __section__(".rodata.pci_driver")))
#else
#define __pci_driver __attribute__((unused))
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
uint16_t pci_find_cap_recursive(const struct device *dev, uint16_t cap);

const char *pin_to_str(int pin);
int get_pci_irq_pins(struct device *dev, struct device **parent_bdg);
void pci_assign_irqs(struct device *dev, const unsigned char pIntAtoD[4]);
const char *get_pci_class_name(struct device *dev);
const char *get_pci_subclass_name(struct device *dev);

size_t pci_msix_table_size(struct device *dev);
int pci_msix_table_bar(struct device *dev, u32 *offset, u8 *idx);
struct msix_entry *pci_msix_get_table(struct device *dev);

#define PCI_IO_BRIDGE_ALIGN 4096
#define PCI_MEM_BRIDGE_ALIGN (1024*1024)

#define PCI_ID(VENDOR_ID, DEVICE_ID) \
	((((DEVICE_ID) & 0xFFFF) << 16) | ((VENDOR_ID) & 0xFFFF))

pci_devfn_t pci_locate_device(unsigned int pci_id, pci_devfn_t dev);
pci_devfn_t pci_locate_device_on_bus(unsigned int pci_id, unsigned int bus);

void pci_s_assert_secondary_reset(pci_devfn_t p2p_bridge);
void pci_s_deassert_secondary_reset(pci_devfn_t p2p_bridge);
void pci_s_bridge_set_secondary(pci_devfn_t p2p_bridge, u8 secondary);

int pci_early_device_probe(u8 bus, u8 dev, u32 mmio_base);

static inline int pci_base_address_is_memory_space(unsigned int attr)
{
	return (attr & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_MEMORY;
}

void pci_dev_disable_bus_master(const struct device *dev);

static __always_inline
#if ENV_PCI_SIMPLE_DEVICE
void pci_dev_request_bus_master(pci_devfn_t dev)
#else
void pci_dev_request_bus_master(struct device *dev)
#endif /* ENV_PCI_SIMPLE_DEVICE */
{
	if (CONFIG(PCI_ALLOW_BUS_MASTER_ANY_DEVICE))
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);
}

#endif /* CONFIG_PCI */

void pci_early_bridge_init(void);

#endif /* PCI_H */
