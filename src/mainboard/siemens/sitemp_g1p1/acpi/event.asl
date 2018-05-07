/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Siemens AG, Inc.
 * (Written by Josef Kellermann <joseph.kellermann@heitec.de> for Siemens AG, Inc.)
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

#include <southbridge/amd/common/acpi/sleepstates.asl>

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
	*    	Store(0,\_SB.PWDE)
	*}
	*/

	/* Clear wake status structure. */
	Store(0, Index(WKST,0))
	Store(0, Index(WKST,1))
	\_SB.PCI0.SIOS (Arg0)
} /* End Method(\_PTS) */

/*
*  The following method results in a "not a valid reserved NameSeg"
*  warning so I have commented it out for the duration.  It isn't
*  used, so it could be removed.
*
*
*  	\_GTS OEM Going To Sleep method
*
*  	Entry:
*  		Arg0=The value of the sleeping state S1=1, S2=2
*
*  	Exit:
*  		-none-
*
*  Method(\_GTS, 1) {
*  DBGO("\\_GTS\n")
*  DBGO("From S0 to S")
*  DBGO(Arg0)
*  DBGO("\n")
*  }
*/

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

	/* if(DeRefOf(Index(WKST,0))) {
	*	Store(0, Index(WKST,1))
	* } else {
	*	Store(Arg0, Index(WKST,1))
	* }
	*/
	\_SB.PCI0.SIOW ()
	Return(WKST)
} /* End Method(\_WAK) */

Scope(\_GPE) {	/* Start Scope GPE */
	/*  General event 3  */
	Method(_L03) {
		/* DBGO("\\_GPE\\_L00\n") */
		Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
	}

	/*  Legacy PM event  */
	Method(_L08) {
		/* DBGO("\\_GPE\\_L08\n") */
	}

	/*  Temp warning (TWarn) event  */
	Method(_L09) {
		/* DBGO("\\_GPE\\_L09\n") */
		Notify (\_TZ.TZ00, 0x80)
	}

	/*  Reserved  */
	/* Method(_L0A) {
	*	DBGO("\\_GPE\\_L0A\n")
	* }
	*/

	/*  USB controller PME#  */
	Method(_L0B) {
		/* DBGO("\\_GPE\\_L0B\n") */
		Notify(\_SB.PCI0.UOH1, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UOH2, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UOH3, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UOH4, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UOH5, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UEH1, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
	}

	/*  AC97 controller PME#  */
	/* Method(_L0C) {
	*	DBGO("\\_GPE\\_L0C\n")
	* }
	*/

	/*  OtherTherm PME#  */
	/* Method(_L0D) {
	*	DBGO("\\_GPE\\_L0D\n")
	* }
	*/

	/*  GPM9 SCI event - Moved to USB.asl */
	/* Method(_L0E) {
	*	DBGO("\\_GPE\\_L0E\n")
	* }
	*/

	/*  PCIe HotPlug event  */
	/* Method(_L0F) {
	* 	DBGO("\\_GPE\\_L0F\n")
	* }
	*/

	/*  ExtEvent0 SCI event  */
	Method(_L10) {
		/* DBGO("\\_GPE\\_L10\n") */
	}


	/*  ExtEvent1 SCI event  */
	Method(_L11) {
		/* DBGO("\\_GPE\\_L11\n") */
	}

	/*  PCIe PME# event  */
	/* Method(_L12) {
	*	DBGO("\\_GPE\\_L12\n")
	* }
	*/

	/*  GPM0 SCI event - Moved to USB.asl */
	/* Method(_L13) {
	* 	DBGO("\\_GPE\\_L13\n")
	* }
	*/

	/*  GPM1 SCI event - Moved to USB.asl */
	/* Method(_L14) {
	* 	DBGO("\\_GPE\\_L14\n")
	* }
	*/

	/*  GPM2 SCI event - Moved to USB.asl */
	/* Method(_L15) {
	* 	DBGO("\\_GPE\\_L15\n")
	* }
	*/

	/*  GPM3 SCI event - Moved to USB.asl */
	/* Method(_L16) {
	*	DBGO("\\_GPE\\_L16\n")
	* }
	*/

	/*  GPM8 SCI event - Moved to USB.asl */
	/* Method(_L17) {
	* 	DBGO("\\_GPE\\_L17\n")
	* }
	*/

	/*  GPIO0 or GEvent8 event  */
	Method(_L18) {
		/* DBGO("\\_GPE\\_L18\n") */
		Notify(\_SB.PCI0.PBR2, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBR4, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBR5, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBR6, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBR7, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
	}

	/*  GPM4 SCI event - Moved to USB.asl */
	/* Method(_L19) {
	* 	DBGO("\\_GPE\\_L19\n")
	* }
	*/

	/*  GPM5 SCI event - Moved to USB.asl */
	/* Method(_L1A) {
	*	DBGO("\\_GPE\\_L1A\n")
	* }
	*/

	/*  Azalia SCI event  */
	Method(_L1B) {
		/* DBGO("\\_GPE\\_L1B\n") */
		Notify(\_SB.PCI0.AZHD, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
	}

	/*  GPM6 SCI event - Reassigned to _L06 */
	/* Method(_L1C) {
	*	DBGO("\\_GPE\\_L1C\n")
	* }
	*/

	/*  GPM7 SCI event - Reassigned to _L07 */
	/* Method(_L1D) {
	*	DBGO("\\_GPE\\_L1D\n")
	* }
	*/

	/*  GPIO2 or GPIO66 SCI event  */
	/* Method(_L1E) {
	* 	DBGO("\\_GPE\\_L1E\n")
	* }
	*/

	/*  SATA SCI event  */
	/* SATA Hot Plug Support -> acpi/sata.asl */
} 	/* End Scope GPE */
