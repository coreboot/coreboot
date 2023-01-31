/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdint.h>
#include <device/pci_ops.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82371eb.h"

void enable_pm(void)
{
	u8 reg8;
	u16 reg16;

	/* Get the SMBus/PM device of the 82371AB/EB/MB. */
	const pci_devfn_t dev = pci_locate_device(PCI_ID(PCI_VID_INTEL,
				PCI_DID_INTEL_82371AB_SMB_ACPI), 0);

	/* Set the PM I/O base. */
	pci_write_config32(dev, PMBA, DEFAULT_PMBASE | 1);

	/* Enable access to the PM I/O space. */
	reg16 = pci_read_config16(dev, PCI_COMMAND);
	reg16 |= PCI_COMMAND_IO;
	pci_write_config16(dev, PCI_COMMAND, reg16);

	/* PM I/O Space Enable (PMIOSE). */
	reg8 = pci_read_config8(dev, PMREGMISC);
	reg8 |= PMIOSE;
	pci_write_config8(dev, PMREGMISC, reg8);
}
