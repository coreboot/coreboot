/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

Device(MBRS) {
	Name (_HID, EisaId ("PNP0C02"))
	Name (_UID, 0x01)

	External(_CRS) /* Resource Template in SSDT */
}
