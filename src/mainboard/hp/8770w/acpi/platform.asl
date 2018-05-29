/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2012 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Method(_WAK,1)
{
	\_SB.PCI0.LPCB.EC0.ACPI = 1
	\_SB.PCI0.LPCB.EC0.SLPT = 0

	Return(Package(){0,0})
}

Method(_PTS,1)
{
	\_SB.PCI0.LPCB.EC0.SLPT = Arg0
}
