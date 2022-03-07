/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <assert.h>
#include "82870.h"

static int num_p64h2_ioapics = 0;

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
	int apic_index, apic_id;

	apic_index = num_p64h2_ioapics;
	num_p64h2_ioapics++;

	// A note on IOAPIC addresses:
	//  0 and 1 are used for the local APICs of the dual virtual
	//  (hyper-threaded) CPUs of physical CPU 0 (devicetree.cb).
	//  6 and 7 are used for the local APICs of the dual virtual
	//  (hyper-threaded) CPUs of physical CPU 1 (devicetree.cb).
	//  2 is used for the IOAPIC in the 82801 southbridge (hard-coded in i82801xx_lpc.c)

	// Map APIC index into APIC ID
	// IDs 3, 4, 5, and 8+ are available (see above note)

	if (apic_index < 3)
		apic_id = apic_index + 3;
	else
		apic_id = apic_index + 5;

	ASSERT(apic_id < 16);       // ID is only 4 bits

	// Read the MBAR address for setting up the IOAPIC in memory space
	// NOTE: this address was assigned during enumeration of the bus

	memoryBase = pci_read_config32(dev, PCI_BASE_ADDRESS_0);

	set_ioapic_id((void *)memoryBase, apic_id);

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
