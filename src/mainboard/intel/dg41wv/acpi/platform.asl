/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Method(_PIC, 1)
{
	/* Remember the OS' IRQ routing choice.  */
	Store(Arg0, PICM)
}

/* SMI I/O Trap */
Method(TRAP, 1, Serialized)
{
	Store (Arg0, SMIF)	/* SMI Function */
	Store (0, TRP0)		/* Generate trap */
	Return (SMIF)		/* Return value of SMI handler */
}
