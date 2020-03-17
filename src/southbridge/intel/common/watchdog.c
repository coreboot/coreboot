/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <southbridge/intel/common/pmbase.h>
#include <southbridge/intel/common/tco.h>
#include <watchdog.h>

/*
 *  Disable ICH-NM10-PCH watchdog timer
 */
void watchdog_off(void)
{
	unsigned int value;
	struct device *dev;

	/* Get LPC device. */
	dev = pcidev_on_root(0x1f, 0);

	value = pci_read_config16(dev, PCI_COMMAND);

	/* Disable interrupt. */
	value |= PCI_COMMAND_INT_DISABLE;
	pci_write_config16(dev, PCI_COMMAND, value);

	/* Disable the watchdog timer. */
	value = read_pmbase16(PMBASE_TCO_OFFSET + TCO1_CNT);
	value |= TCO_TMR_HLT;
	write_pmbase16(PMBASE_TCO_OFFSET + TCO1_CNT, value);

	/* Clear TCO timeout status. */
	write_pmbase16(PMBASE_TCO_OFFSET + TCO1_STS, TCO1_TIMEOUT);
	write_pmbase16(PMBASE_TCO_OFFSET + TCO2_STS, SECOND_TO_STS);

	printk(BIOS_DEBUG, "ICH-NM10-PCH: watchdog disabled\n");
}
