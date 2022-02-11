/* SPDX-License-Identifier: GPL-2.0-only */

/* 0:12.0 - OHCI */
Device(UOH1) {
	Name(_ADR, 0x00120000)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH1 */

/* 0:12.2 - EHCI */
Device(UOH2) {
	Name(_ADR, 0x00120002)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH2 */

/* 0:13.0 - OHCI */
Device(UOH3) {
	Name(_ADR, 0x00130000)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH3 */

/* 0:13.2 - EHCI */
Device(UOH4) {
	Name(_ADR, 0x00130002)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH4 */

/* 0:16.0 - OHCI */
Device(UOH5) {
	Name(_ADR, 0x00160000)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH5 */

/* 0:16.2 - EHCI */
Device(UOH6) {
	Name(_ADR, 0x00160002)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH5 */

Device(UEH1) {
	Name(_ADR, 0x00140005)
	Name(_PRW, Package() {0x0B, 3})
} /* end UEH1 */
