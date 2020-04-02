/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
