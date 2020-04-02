/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
