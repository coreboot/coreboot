/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <string.h>
#include <device/pci.h>
#include <arch/acpi.h>

static const char slic[0x4] = {
	0x53, 0x4c, 0x49, 0x43 /* incomplete. */
	/* If you wish to compile coreboot images with a windows license key
	 * built in, you need to extract the ACPI SLIC from your machine and
	 * add it here.
	 */
};

unsigned long acpi_create_slic(unsigned long current)
{
	memcpy((void *) current, slic, sizeof(slic));
	return sizeof(slic);
}
