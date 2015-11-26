/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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

// #define ACPI_INCLUDE_PMIO 1 /* uncomment to enable PMIO block in soc.asl */
// #define ACPI_INCLUDE_GPIO 1 /* uncomment to enable GPIO block in soc.asl */

Device (PWRB)
{
	Name(_HID, EisaId("PNP0C0C"))

	// Wake
	Name(_PRW, Package(){0x1d, 0x05})
}
