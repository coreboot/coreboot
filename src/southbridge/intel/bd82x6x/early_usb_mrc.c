/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <device/pci_def.h>
#include "pch.h"

#define PCH_EHCI1_TEMP_BAR0 0xe8000000
#define PCH_EHCI2_TEMP_BAR0 0xe8000400

/*
 * Setup USB controller MMIO BAR to prevent the
 * reference code from resetting the controller.
 *
 * The BAR will be re-assigned during device
 * enumeration so these are only temporary.
 */
void enable_usb_bar(void)
{
	pci_devfn_t usb0 = PCH_EHCI1_DEV;
	pci_devfn_t usb1 = PCH_EHCI2_DEV;
	u32 cmd;

	/* USB Controller 1 */
	pci_write_config32(usb0, PCI_BASE_ADDRESS_0,
			   PCH_EHCI1_TEMP_BAR0);
	cmd = pci_read_config32(usb0, PCI_COMMAND);
	cmd |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_write_config32(usb0, PCI_COMMAND, cmd);

	/* USB Controller 2 */
	pci_write_config32(usb1, PCI_BASE_ADDRESS_0,
			   PCH_EHCI2_TEMP_BAR0);
	cmd = pci_read_config32(usb1, PCI_COMMAND);
	cmd |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_write_config32(usb1, PCI_COMMAND, cmd);
}
