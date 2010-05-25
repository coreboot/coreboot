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

#ifndef SB600_DEVN_BASE

#define SB600_DEVN_BASE 0

#endif

#define EHCI_BAR_INDEX 0x10
#define EHCI_BAR 0xFEF00000
#define EHCI_DEBUG_OFFSET 0xE0

static void sb600_enable_usbdebug_direct(u32 port)
{
	set_debug_port(port);
	pci_write_config32(PCI_DEV(0, SB600_DEVN_BASE + 0x13, 5),
			   EHCI_BAR_INDEX, EHCI_BAR);
	pci_write_config8(PCI_DEV(0, SB600_DEVN_BASE + 0x13, 5), 0x04, 0x2);	/* mem space enabe */
}
