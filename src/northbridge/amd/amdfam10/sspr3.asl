/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

DefinitionBlock ("SSPR.aml", "SSDT", 1, "AMD-FAM10", "AMD-ACPI", 100925441)
{
	Scope (\_SB)
	{
		Processor (\_SB.CPAA, 0xbb, 0x120, 6) // CPU0 and 0x01 need to be updated
		{
			Name(_PCT, Package ()
			{
				ResourceTemplate() {Register (FFixedHW, 0, 0, 0)}, //PERF_CTRL
				ResourceTemplate() {Register (FFixedHW, 0, 0, 0)}, //PERF_STATUS
			})

			Name(_PSS, Package()
			{
				Package(0x06) {0x1111, 0x222222, 0x3333, 0x4444, 0x55, 0x66 },
				Package(0x06) {0x7777, 0x222222, 0x3333, 0x4444, 0x55, 0x66 },
				Package(0x06) {0x8888, 0x222222, 0x3333, 0x4444, 0x55, 0x66 },
			})
		}

	}
}
