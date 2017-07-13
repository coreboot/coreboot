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
 */

/* 0:12.0 - OHCI */
Device(UOH1) {
	Name(_ADR, 0x00120000)
	Name(_PRW, Package() {0x0b, 3})
} /* end UOH1 */

/* 0:12.2 - EHCI */
Device(UOH2) {
	Name(_ADR, 0x00120002)
	Name(_PRW, Package() {0x0b, 3})
} /* end UOH2 */

/* 0:13.0 - OHCI */
Device(UOH3) {
	Name(_ADR, 0x00130000)
	Name(_PRW, Package() {0x0b, 3})
} /* end UOH3 */

/* 0:13.2 - EHCI */
Device(UOH4) {
	Name(_ADR, 0x00130002)
	Name(_PRW, Package() {0x0b, 3})
} /* end UOH4 */

/* 0:16.0 - OHCI */
Device(UOH5) {
	Name(_ADR, 0x00160000)
	Name(_PRW, Package() {0x0b, 3})
} /* end UOH5 */

/* 0:16.2 - EHCI */
Device(UOH6) {
	Name(_ADR, 0x00160002)
	Name(_PRW, Package() {0x0b, 3})
} /* end UOH5 */

/* 0:10.0 - XHCI 0*/
Device(XHC0) {
	Name(_ADR, 0x00100000)
	Name(_PRW, Package() {0x0b, 4})
} /* end XHC0 */
