/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

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
