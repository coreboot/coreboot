/* SPDX-License-Identifier: GPL-2.0-only */

// Intel NPK Controller 0:1f.7

Device (NPK0)
{
	Name (_ADR, 0x001f0007)

	// Northpeak DFX
	Method(_STA, 0, NotSerialized)
	{
		Return(0x0B)
	}
}
