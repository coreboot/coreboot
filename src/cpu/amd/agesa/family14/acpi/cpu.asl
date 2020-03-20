/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Processor Object
 *
 */
Scope (\_SB) {		/* define processor scope */

	Device (C000) {
	Name (_HID, "ACPI0007")
	Name (_UID, 0)
	}

	Device (C001) {
	Name (_HID, "ACPI0007")
	Name (_UID, 1)
	}
} /* End _SB scope */
