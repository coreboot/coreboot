/* SPDX-License-Identifier: GPL-2.0-only */

/* Audio Controller - Device 31, Function 3 */

Device (HDAS)
{
	Name (_ADR, 0x001f0003)
	Name (_DDN, "Audio Controller")
	Name (UUID, ToUUID ("A69F886E-6CEB-4594-A41F-7B5DCE24C553"))

	/* Device is D3 wake capable */
	Name (_S0W, 3)
}
