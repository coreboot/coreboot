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

Device(UOH1) {
	Name(_ADR, 0x00120000)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH1 */

Device(UOH2) {
	Name(_ADR, 0x00120002)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH2 */

Device(UOH3) {
	Name(_ADR, 0x00130000)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH3 */

Device(UOH4) {
	Name(_ADR, 0x00130002)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH4 */

Device(UOH5) {
	Name(_ADR, 0x00160000)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH5 */

Device(UOH6) {
	Name(_ADR, 0x00160002)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH5 */

Device(UEH1) {
	Name(_ADR, 0x00140005)
	Name(_PRW, Package() {0x0B, 3})
} /* end UEH1 */

