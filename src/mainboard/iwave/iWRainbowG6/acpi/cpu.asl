/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

/* Intel Core (2) Duo CPU node support
 *
 * Note: The ACPI P_BLK on the ICH7 (and probably others) lives at
 * PMBASE + 0x10, and it's 0x06 bytes long. On ICH8 it's 8 bytes.
 *
 * The second CPU core does not need its own P_BLK.
 */

Scope(\_PR)
{
	Processor(
		CPU1,	// name of cpu/core 0
		1,	// numeric id of cpu/core
		0x510,	// ACPI P_BLK base address
		6	// ACPI P_BLK size
	)
	{
		// TODO: _PDT
	}

	Processor(
		CPU2,	// name of cpu/core 1
		2,	// numeric id of cpu/core 1
		0,	// ACPI P_BLK base address
		0)	// ACPI P_BLK size
	{
		// TODO: _PDT
	}
}	// End _PR

