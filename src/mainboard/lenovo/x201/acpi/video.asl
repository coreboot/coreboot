/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
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

#include "smi.h"

Scope (\_SB.PCI0.GFX0)
{
        Device (LCD0)
        {
              Name (_ADR, 0x0400)

              Name (BRIG, Package (0x12)
              {
	      0xb2,
	      0xb2,
	      0x3,
	      0x7,
	      0x9,
	      0xd,
	      0x11,
	      0x16,
	      0x1a,
	      0x21,
	      0x27,
	      0x2e,
	      0x37,
	      0x3f,
	      0x4e,
	      0x61,
	      0x81,
	      0xb2,
              })


              Method (_BCL, 0, NotSerialized)
              {
                  Return (BRIG)
              }

              Method (_BCM, 1, NotSerialized)
              {
                 Store (ShiftLeft (Arg0, 3), ^^BCLV)
              }
              Method (_BQC, 0, NotSerialized)
              {
                 Store (^^BCLV, Local0)
		 ShiftRight (Local0, 3, Local0)
                 Return (Local0)
              }
        }
}
