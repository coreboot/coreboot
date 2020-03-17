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
 */

/* The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method(_PTS,1)
{

}

/* The _WAK method is called on system wakeup */

Method(_WAK,1)
{
	/* Update in case state changed while asleep */
	/* Update AC status */
	Store (\_SB.PCI0.LPCB.EC0.ADPT, Local0)
	if (LNotEqual (Local0, \PWRS)) {
		Store (Local0, \PWRS)
		Notify (\_SB.PCI0.LPCB.EC0.AC, 0x80)
	}

	/* Update LID status */
	Store (GP15, Local0)
	if (LNotEqual (Local0, \LIDS)) {
		Store (Local0, \LIDS)
		Notify (\_SB.LID0, 0x80)
	}

	Return(Package(){0,0})
}
