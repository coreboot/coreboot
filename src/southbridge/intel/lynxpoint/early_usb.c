/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/pci_ops.h>
#include <device/pci_def.h>
#include "pch.h"

/* HCD_INDEX == 2 selects 0:1a.0 (PCH_EHCI2), any other index
 * selects 0:1d.0 (PCH_EHCI1) for usbdebug use.
 */
#if CONFIG_USBDEBUG_HCD_INDEX != 2
#define PCH_EHCI1_TEMP_BAR0 CONFIG_EHCI_BAR
#define PCH_EHCI2_TEMP_BAR0 (PCH_EHCI1_TEMP_BAR0 + 0x400)
#else
#define PCH_EHCI2_TEMP_BAR0 CONFIG_EHCI_BAR
#define PCH_EHCI1_TEMP_BAR0 (PCH_EHCI2_TEMP_BAR0 + 0x400)
#endif

/*
 * Setup USB controller MMIO BAR to prevent the
 * reference code from resetting the controller.
 *
 * The BAR will be re-assigned during device
 * enumeration so these are only temporary.
 */
static void enable_usb_bar_on_device(pci_devfn_t dev, u32 bar)
{
	u32 cmd;
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, bar);
	cmd = pci_read_config32(dev, PCI_COMMAND);
	cmd |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_write_config32(dev, PCI_COMMAND, cmd);
}

void enable_usb_bar(void)
{
	enable_usb_bar_on_device(PCH_EHCI1_DEV, PCH_EHCI1_TEMP_BAR0);
	if (!pch_is_lp())
		enable_usb_bar_on_device(PCH_EHCI2_DEV, PCH_EHCI2_TEMP_BAR0);
}
