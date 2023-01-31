/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
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
	write_pmbase16(PMBASE_TCO_OFFSET + TCO1_STS, TCO_TIMEOUT);
	write_pmbase16(PMBASE_TCO_OFFSET + TCO2_STS, TCO2_STS_SECOND_TO);

	printk(BIOS_DEBUG, "ICH-NM10-PCH: watchdog disabled\n");
}
