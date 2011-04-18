/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
#include "sb600.h"

/* Required for successful build, but currently empty. */
void set_debug_port(unsigned int port)
{
	/* TODO: Allow changing the physical USB port used as Debug Port. */
}

void enable_usbdebug(unsigned int port)
{
	device_t dev = PCI_DEV(0, 0x13, 5); /* USB EHCI, D19:F5 */

	/* Select the requested physical USB port (1-15) as the Debug Port. */
	set_debug_port(port);

	/* Set the EHCI BAR address. */
	pci_write_config32(dev, EHCI_BAR_INDEX, CONFIG_EHCI_BAR);

	/* Enable access to the EHCI memory space registers. */
	pci_write_config8(dev, PCI_COMMAND, PCI_COMMAND_MEMORY);
}
