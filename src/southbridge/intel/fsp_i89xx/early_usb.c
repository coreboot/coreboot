/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include "pch.h"

#define PCH_EHCI1_TEMP_BAR0 0xe8000000

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
	u32 cmd;

	/* USB Controller 0 */
	pci_write_config32(usb0, PCI_BASE_ADDRESS_0,
			   PCH_EHCI1_TEMP_BAR0);
	cmd = pci_read_config32(usb0, PCI_COMMAND);
	cmd |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_write_config32(usb0, PCI_COMMAND, cmd);
}
