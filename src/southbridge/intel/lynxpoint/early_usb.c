/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <device/pci_def.h>
#include "pch.h"

/*
 * Setup USB controller MMIO BAR to prevent the
 * reference code from resetting the controller.
 *
 * The BAR will be re-assigned during device
 * enumeration so these are only temporary.
 */
static void enable_usb_bar_on_device(pci_devfn_t dev, u32 bar)
{
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, bar);
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
}

void enable_usb_bar(void)
{
	enable_usb_bar_on_device(PCH_EHCI1_DEV, PCH_EHCI1_TEMP_BAR0);
	if (!pch_is_lp())
		enable_usb_bar_on_device(PCH_EHCI2_DEV, PCH_EHCI2_TEMP_BAR0);
}
