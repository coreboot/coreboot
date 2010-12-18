/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdint.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <console/console.h>
#include <usbdebug.h>
#include <device/pci_def.h>

/* Required for successful build, but currently empty. */
void set_debug_port(unsigned int port)
{
	/* Not needed, the southbridges hardcode physical USB port 1. */
}

void sch_enable_usbdebug(unsigned int port)
{
	u32 dbgctl;
	device_t dev = PCI_DEV(0, 0x1d, 7); /* USB EHCI, D29:F7 */

	/* Set the EHCI BAR address. */
	pci_write_config32(dev, EHCI_BAR_INDEX, CONFIG_EHCI_BAR);

	/* Enable access to the EHCI memory space registers. */
	pci_write_config8(dev, PCI_COMMAND, PCI_COMMAND_MEMORY);

	/* Force ownership of the Debug Port to the EHCI controller. */
	printk(BIOS_DEBUG, "Enabling OWNER_CNT\n");
	dbgctl = read32(CONFIG_EHCI_BAR + CONFIG_EHCI_DEBUG_OFFSET);
	dbgctl |= (1 << 30);
	write32(CONFIG_EHCI_BAR + CONFIG_EHCI_DEBUG_OFFSET, dbgctl);
}
