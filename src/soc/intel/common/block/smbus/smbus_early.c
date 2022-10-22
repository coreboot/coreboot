/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <device/pci_type.h>
#include <intelblocks/smbus.h>
#include <soc/pci_devs.h>
#include <stdint.h>

#include "smbuslib.h"

void smbus_common_init(void)
{
	const pci_devfn_t dev = PCH_DEV_SMBUS;

	/* Set SMBus I/O base address */
	pci_write_config32(dev, PCI_BASE_ADDRESS_4, SMBUS_IO_BASE);
	/* Set SMBus enable */
	pci_write_config8(dev, HOSTC, HST_EN);
	/* Enable I/O access */
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_IO);
	/* Disable interrupts */
	outb(0, SMBUS_IO_BASE + SMBHSTCTL);
	/* Clear errors */
	outb(0xff, SMBUS_IO_BASE + SMBHSTSTAT);
}

uintptr_t smbus_base(void)
{
	return SMBUS_IO_BASE;
}
