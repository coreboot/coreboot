/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Uwe Hermann <uwe@hermann-uwe.de>
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
#include <usbdebug.h>
#include <device/pci_def.h>
#include "sb700.h"

#define EHCI_EOR		(CONFIG_EHCI_BAR + 0x20)
#define DEBUGPORT_MISC_CONTROL	(EHCI_EOR + 0x80)

void set_debug_port(unsigned int port)
{
	u32 reg32;

	/* Write the port number to DEBUGPORT_MISC_CONTROL[31:28]. */
	reg32 = read32(DEBUGPORT_MISC_CONTROL);
	reg32 &= ~(0xf << 28);
	reg32 |= (port << 28);
	reg32 |= (1 << 27); /* Enable Debug Port port number remapping. */
	write32(DEBUGPORT_MISC_CONTROL, reg32);
}

/*
 * Note: The SB700 has two EHCI devices, D18:F2 and D19:F2.
 * This code currently only supports the first one, i.e., USB Debug devices
 * attached to physical USB ports belonging to the first EHCI device.
 */
void sb7xx_51xx_enable_usbdebug(unsigned int port)
{
	device_t dev = PCI_DEV(0, 0x12, 2); /* USB EHCI, D18:F2 */

	/* Set the EHCI BAR address. */
	pci_write_config32(dev, EHCI_BAR_INDEX, CONFIG_EHCI_BAR);

	/* Enable access to the EHCI memory space registers. */
	pci_write_config8(dev, PCI_COMMAND, PCI_COMMAND_MEMORY);

	/*
	* Select the requested physical USB port (1-15) as the Debug Port.
	* Must be called after the EHCI BAR has been set up (see above).
	*/
	set_debug_port(port);
}
