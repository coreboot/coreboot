/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
* for S1-S5. The sleeping value is passed to the _PTS control method.  This
* control method may be executed a relatively long time before entering the
* sleep state and the OS may abort the operation without notification to
* the ACPI driver.  This method cannot modify the configuration or power
* state of any device in the system.
*/
Method(\_PTS, 1) {
	/* DBGO("\\_PTS\n") */
	/* DBGO("From S0 to S") */
	/* DBGO(Arg0) */
	/* DBGO("\n") */

	/* Don't allow PCIRST# to reset USB */
	if (LEqual(Arg0,3)){
		Store(0,URRE)
	}

	/* Clear sleep SMI status flag and enable sleep SMI trap. */
	/*Store(One, CSSM)
	Store(One, SSEN)*/

	/* On older chips, clear PciExpWakeDisEn */
	/*if (LLessEqual(\_SB.SBRI, 0x13)) {
	*	Store(0,\_SB.PWDE)
	*}
	*/

	/* Clear wake status structure. */
	Store(0, Index(WKST,0))
	Store(0, Index(WKST,1))
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

	/* Re-enable HPET */
	Store(1,HPDE)

	/* Restore PCIRST# so it resets USB */
	if (LEqual(Arg0,3)){
		Store(1,URRE)
	}

	/* Arbitrarily clear PciExpWakeStatus */
	Store(PWST, Local1)
	Store(Local1, PWST)

	/* if (DeRefOf(Index(WKST,0))) {
	*	Store(0, Index(WKST,1))
	* } else {
	*	Store(Arg0, Index(WKST,1))
	* }
	*/
	Return(WKST)
} /* End Method(\_WAK) */
