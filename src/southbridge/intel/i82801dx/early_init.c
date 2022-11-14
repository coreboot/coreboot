/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/smbus_host.h>
#include <device/pci_ops.h>
#include "i82801dx.h"

void i82801dx_early_init(void)
{
	const pci_devfn_t dev = PCI_DEV(0, 0x1f, 0);

	/* Enable ACPI I/O range decode and ACPI power management. */
	pci_write_config32(dev, PMBASE, DEFAULT_PMBASE | 1);
	pci_write_config8(dev, ACPI_CNTL, ACPI_EN);

	pci_write_config32(dev, GPIO_BASE, GPIOBASE_ADDR | 1);
	pci_write_config8(dev, GPIO_CNTL, 0x10);

	if (ENV_RAMINIT)
		enable_smbus();
}

void i82801dx_lpc_setup(void)
{
	const pci_devfn_t dev = PCI_DEV(0, 0x1f, 0);

	/* Decode 0x3F8-0x3FF (COM1) for COMA port, 0x2F8-0x2FF (COM2) for COMB.
	 * LPT decode defaults to 0x378-0x37F and 0x778-0x77F.
	 * Floppy decode defaults to 0x3F0-0x3F5, 0x3F7.
	 * We also need to set the value for LPC I/F Enables Register.
	 */
	pci_write_config8(dev, COM_DEC, 0x10);
	pci_write_config16(dev, LPC_EN, 0x300F);
}
