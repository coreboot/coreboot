/*
 * This file is part of the coreboot project.
 *
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

/* 0:12.0 - EHCI */
Device(EHC0) {
	Name(_ADR, 0x00120000)
	Name(_PRW, Package() { 0xb, 3 })
	Device (RHUB) {
		Name (_ADR, Zero)
		Device (HS01) { Name (_ADR, 1) }
		Device (HS02) { Name (_ADR, 2) }
		Device (HS03) { Name (_ADR, 3) }
		Device (HS04) { Name (_ADR, 4) }
		Device (HS05) { Name (_ADR, 5) }
		Device (HS06) { Name (_ADR, 6) }
		Device (HS07) { Name (_ADR, 7) }
		Device (HS08) { Name (_ADR, 8) }
	}

	Method(_S0W,0) {
		Return(0)
	}

	Method(_S3W,0) {
		Return(4)
	}

	Method(_S4W,0) {
		Return(4)
	}
} /* end EHC0 */


/* 0:10.0 - XHCI 0*/
Device(XHC0) {
	Name(_ADR, 0x00100000)
	Name(_PRW, Package() { 0xb, 3 })
	Device (SS01) { Name (_ADR, 1) }
	Device (SS02) { Name (_ADR, 2) }
	Device (SS03) { Name (_ADR, 3) }

	Method(_S0W,0) {
		Return(0)
	}

	Method(_S3W,0) {
		Return(4)
	}

	Method(_S4W,0) {
		Return(4)
	}

} /* end XHC0 */
