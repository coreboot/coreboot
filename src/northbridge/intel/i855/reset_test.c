/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Jon Dufresne <jon.dufresne@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#define MCH_DRC 0x70
#define DRC_DONE (1 << 29)

	/* If I have already booted once skip a bunch of initialization */
	/* To see if I have already booted I check to see if memory
	 * has been enabled.
	 */
static int bios_reset_detected(void)
{
	uint32_t dword;

	dword = pci_read_config32(PCI_DEV(0, 0, 0), MCH_DRC);

	if( (dword & DRC_DONE) != 0 ) {
		return 1;
	}

	return 0;
}
