/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

/*
 * Workaround for RTC on Cezanne.
 * See https://source.chromium.org/chromiumos/chromiumos/codesearch/+/main:src/third_party/kernel/v5.10/drivers/platform/x86/amd-pmc.c;l=416;drc=54a96af06ae6851e4a02e8dd700de0d579ef7839
 */

Scope (\_SB.PEP) {
	Name (_PRW, Package () {
		Package() {\_SB.GPIO, 0},
		0x03
	})
}

Scope (\_SB.GPIO) {
	Name (_AEI, ResourceTemplate () {
		GpioInt(Edge, ActiveHigh, ExclusiveAndWake, PullNone, 0x0000, "\\_SB.GPIO",,,,)
		{
			44 /* int_shdwsysalarmfire */
		}
	})

	Method (_E2C, 0, Serialized) {
		Notify (\_SB_.PEP, 0x02)
	}
}
