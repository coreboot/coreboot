/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <assert.h>
#include "82870.h"

static void p64h2_ioapic_enable(struct device *dev)
{
	/* We have to enable MEM and Bus Master for IOAPIC */
	uint16_t command = PCI_COMMAND_SERR | PCI_COMMAND_PARITY | PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;

	pci_write_config16(dev, PCI_COMMAND, command);
}

/**
 * Configure one of the IOAPICs in a P64H2.
 *
 * Note that a PCI bus scan will detect both IOAPICs, so this function
 * will be called twice for each P64H2 in the system.
 *
 * @param dev PCI bus/device/function of P64H2 IOAPIC.
 *            NOTE: There are two IOAPICs per P64H2, at D28:F0 and D30:F0.
 */
static void p64h2_ioapic_init(struct device *dev)
{
	uint32_t memoryBase;

	// Read the MBAR address for setting up the IOAPIC in memory space
	// NOTE: this address was assigned during enumeration of the bus

	memoryBase = pci_read_config32(dev, PCI_BASE_ADDRESS_0);

	register_new_ioapic((void *)memoryBase);

	// Use Processor System Bus to deliver interrupts
	ioapic_set_boot_config((void *)memoryBase, true);
}

static struct device_operations ioapic_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init     = p64h2_ioapic_init,
	.enable   = p64h2_ioapic_enable,
};

static const struct pci_driver ioapic_driver __pci_driver = {
	.ops    = &ioapic_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_82870_1E0,

};
