/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdint.h>
#include <device/pci_ops.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/smbus_host.h>
#include "i82371eb.h"

void i82371eb_early_init(void)
{
	enable_smbus();
	enable_pm();
}

uintptr_t smbus_base(void)
{
	return SMBUS_IO_BASE;
}

int smbus_enable_iobar(uintptr_t base)
{
	u8 reg8;
	u16 reg16;

	/* Get the SMBus/PM device of the 82371AB/EB/MB. */
	const pci_devfn_t dev = pci_locate_device(PCI_ID(PCI_VID_INTEL,
				PCI_DID_INTEL_82371AB_SMB_ACPI), 0);

	/* Set the SMBus I/O base. */
	pci_write_config32(dev, SMBBA, base | 1);

	/* Enable the SMBus controller host interface. */
	reg8 = pci_read_config8(dev, SMBHSTCFG);
	reg8 |= SMB_HST_EN;
	pci_write_config8(dev, SMBHSTCFG, reg8);

	/* Enable access to the SMBus I/O space. */
	reg16 = pci_read_config16(dev, PCI_COMMAND);
	reg16 |= PCI_COMMAND_IO;
	pci_write_config16(dev, PCI_COMMAND, reg16);

	return 0;
}
