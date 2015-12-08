//
// This file is part of the coreboot project.
//
// Copyright (C) 2007 Advanced Micro Devices, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//

	Device (HTXA)
	{
		//  HTX
		Method (_ADR, 0, NotSerialized)
		{
			Return (DADD(GHCD(HCIN, 0), 0x00000000))
		}

		Method (_PRW, 0, NotSerialized)
		{
			If (CondRefOf (\_S3)) { Return (Package (0x02) { 0x29, 0x03 }) }
			Else { Return (Package (0x02) { 0x29, 0x01 }) }
		}

	}
