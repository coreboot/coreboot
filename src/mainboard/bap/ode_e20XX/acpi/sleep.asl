/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
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

/* Wake status package */
Name(WKST,Package(){Zero, Zero})

/*
* \_PTS - Prepare to Sleep method
*
*	Entry:
*		Arg0=The value of the sleeping state S1=1, S2=2, etc
*
* Exit:
*		-none-
*
* The _PTS control method is executed at the beginning of the sleep process
* for S1-S5. The sleeping value is passed to the _PTS control method.	This
* control method may be executed a relatively long time before entering the
* sleep state and the OS may abort	the operation without notification to
* the ACPI driver.  This method cannot modify the configuration or power
* state of any device in the system.
*/

External(\_SB.APTS, MethodObj)
External(\_SB.AWAK, MethodObj)

Method(_PTS, 1) {
	/* DBGO("\\_PTS\n") */
	/* DBGO("From S0 to S") */
	/* DBGO(Arg0) */
	/* DBGO("\n") */

	/* Clear wake status structure. */
	Store(0, Index(WKST,0))
	Store(0, Index(WKST,1))
	Store(7, UPWS)
	\_SB.APTS(Arg0)
} /* End Method(\_PTS) */

/*
*	\_BFS OEM Back From Sleep method
*
*	Entry:
*		Arg0=The value of the sleeping state S1=1, S2=2
*
*	Exit:
*		-none-
*/
Method(\_BFS, 1) {
	/* DBGO("\\_BFS\n") */
	/* DBGO("From S") */
	/* DBGO(Arg0) */
	/* DBGO(" to S0\n") */
}

/*
*  \_WAK System Wake method
*
*	Entry:
*		Arg0=The value of the sleeping state S1=1, S2=2
*
*	Exit:
*		Return package of 2 DWords
*		Dword 1 - Status
*			0x00000000	wake succeeded
*			0x00000001	Wake was signaled but failed due to lack of power
*			0x00000002	Wake was signaled but failed due to thermal condition
*		Dword 2 - Power Supply state
*			if non-zero the effective S-state the power supply entered
*/
Method(\_WAK, 1) {
	/* DBGO("\\_WAK\n") */
	/* DBGO("From S") */
	/* DBGO(Arg0) */
	/* DBGO(" to S0\n") */
	Store(1,USBS)

	\_SB.AWAK(Arg0)

	Return(WKST)
} /* End Method(\_WAK) */
