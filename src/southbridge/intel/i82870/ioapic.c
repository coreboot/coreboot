/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
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

	volatile uint32_t* pIndexRegister;    /* io apic io memory space command address */
	volatile uint32_t* pWindowRegister;    /* io apic io memory space data address */

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
	pIndexRegister  = (volatile uint32_t*) memoryBase;
	pWindowRegister = (volatile uint32_t*)(memoryBase + 0x10);

	printk(BIOS_DEBUG, "IOAPIC %d at %02x:%02x.%01x  MBAR = %p DataAddr = %p\n",
		apic_id, dev->bus->secondary, PCI_SLOT(dev->path.pci.devfn),
		PCI_FUNC(dev->path.pci.devfn), pIndexRegister, pWindowRegister);

	apic_id <<= 24;             // Convert ID to bitmask

	*pIndexRegister = 0;        // Select APIC ID register
	*pWindowRegister = (*pWindowRegister & ~(0x0f << 24)) | apic_id;   // Set the ID

	if ((*pWindowRegister & (0x0f << 24)) != apic_id)
		die("p64h2_ioapic_init failed");

	*pIndexRegister  = 3;   // Select Boot Configuration register
	*pWindowRegister |= 1;  // Use Processor System Bus to deliver interrupts

	if (!(*pWindowRegister & 1))
		die("p64h2_ioapic_init failed");
}

static struct device_operations ioapic_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init     = p64h2_ioapic_init,
	.scan_bus = 0,
	.enable   = p64h2_ioapic_enable,
};

static const struct pci_driver ioapic_driver __pci_driver = {
	.ops    = &ioapic_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82870_1E0,

};
