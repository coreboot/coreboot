/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Iru Cai <mytbk920423@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Method(_WAK,1)
{
	/* ME may not be up yet. */
	Store (0, \_TZ.MEB1)
	Store (0, \_TZ.MEB2)
	Return(Package(){0,0})
}

Method(_PTS,1)
{
	\_SB.PCI0.LPCB.EC.RADI(0)
}
