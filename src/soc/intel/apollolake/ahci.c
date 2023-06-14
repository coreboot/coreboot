/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_ops.h>
#include <soc/ahci.h>
#include <soc/pci_devs.h>
#include <soc/soc_chip.h>
#include <types.h>

void ahci_set_speed(enum sata_speed_limit speed)
{
	if (speed == SATA_DEFAULT)
		return;

	/* Setup temporary base address for BAR5. */
	pci_write_config32(PCH_DEV_SATA, PCI_BASE_ADDRESS_5, AHCI_TMP_BASE_ADDR);
	/* Enable memory access for pci_dev. */
	pci_or_config16(PCH_DEV_SATA, PCI_COMMAND, PCI_COMMAND_MEMORY);

	printk(BIOS_DEBUG, "AHCI: Set SATA speed to Gen %d\n", speed);
	clrsetbits32((void *)(AHCI_TMP_BASE_ADDR + AHCI_CAP), AHCI_CAP_ISS_MASK,
			AHCI_SPEED(speed));

	/* Disable memory access for pci_dev. */
	pci_and_config16(PCH_DEV_SATA, PCI_COMMAND, ~PCI_COMMAND_MEMORY);
	/* Clear temporary base address for BAR5. */
	pci_write_config32(PCH_DEV_SATA, PCI_BASE_ADDRESS_5, 0);
}
