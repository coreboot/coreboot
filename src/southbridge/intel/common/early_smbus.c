/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include "early_smbus.h"

uintptr_t smbus_base(void)
{
	return CONFIG_FIXED_SMBUS_IO_BASE;
}

int smbus_enable_iobar(uintptr_t base)
{
	/* Set the SMBus device statically. */
	const pci_devfn_t dev = PCI_DEV_SMBUS;

	/* Check to make sure we've got the right device. */
	if (pci_read_config16(dev, PCI_VENDOR_ID) != PCI_VID_INTEL)
		return -1;

	/* Set SMBus I/O base. */
	pci_write_config32(dev, SMB_BASE, base | PCI_BASE_ADDRESS_SPACE_IO);

	/* Set SMBus enable. */
	pci_write_config8(dev, HOSTC, HST_EN);

	/* Set SMBus I/O space enable. */
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_IO);

	return 0;
}
