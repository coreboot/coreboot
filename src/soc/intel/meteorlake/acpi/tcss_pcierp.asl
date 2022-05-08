/* SPDX-License-Identifier: GPL-2.0-or-later */

OperationRegion (PXCS, SystemMemory, BASE(_ADR), 0x800)
Field (PXCS, AnyAcc, NoLock, Preserve)
{
	VDID, 32,
	Offset(0x50),  /* LCTL - Link Control Register */
	L0SE, 1,       /* 0, L0s Entry Enabled */
	, 3,
	LDIS, 1,       /* 1, Link Disable */
	, 3,
	Offset(0x52),  /* LSTS - Link Status Register */
	, 13,
	LASX, 1,       /* 0, Link Active Status */
	Offset(0x5A),  /* SLSTS[7:0] - Slot Status Register */
	ABPX, 1,       /* 0, Attention Button Pressed */
	, 2,
	PDCX, 1,       /* 3, Presence Detect Changed */
	, 2,
	PDSX, 1,       /* 6, Presence Detect State */
	, 1,
	DLSC, 1,       /* 8, Data Link Layer State Changed */
	Offset(0x60),  /* RSTS - Root Status Register */
	, 16,
	PSPX, 1,       /* 16,  PME Status */
	Offset(0xA4),
	D3HT, 2,       /* Power State */
	Offset(0xD8),  /* 0xD8, MPC - Miscellaneous Port Configuration Register */
	, 30,
	HPEX, 1,       /* 30,  Hot Plug SCI Enable */
	PMEX, 1,       /* 31,  Power Management SCI Enable */
	Offset(0xE2),  /* 0xE2, RPPGEN - Root Port Power Gating Enable */
	, 2,
	L23E, 1,       /* 2,   L23_Rdy Entry Request (L23ER) */
	L23R, 1,       /* 3,   L23_Rdy to Detect Transition (L23R2DT) */
	Offset(0x420), /* 0x420, PCIEPMECTL (PCIe PM Extension Control) */
	, 30,
	DPGE, 1,       /* PCIEPMECTL[30]: Disabled, Detect and L23_Rdy State PHY Lane */
		       /* Power Gating Enable (DLSULPPGE) */
	Offset(0x5BC), /* 0x5BC, PCIE ADVMCTRL */
	, 3,
	RPER, 1,       /*  RTD3PERST[3] */
	RPFE, 1,       /*  RTD3PFETDIS[4] */
}

Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
{
	Offset(0xDC),  /* 0xDC, SMSCS - SMI/SCI Status Register */
	, 30,
	HPSX, 1,       /* 30,  Hot Plug SCI Status */
	PMSX, 1        /* 31,  Power Management SCI Status */
}

/*
 * _DSM Device Specific Method
 *
 * Arg0: UUID Unique function identifier
 * Arg1: Integer Revision Level
 * Arg2: Integer Function Index (0 = Return Supported Functions)
 * Arg3: Package Parameters
 */
Method (_DSM, 4, Serialized)
{
	Return (Buffer() { 0x00 })
}

/*
 * A bitmask of functions support
 */
Name(OPTS, Buffer(2) {0, 0})

Device (PXSX)
{
	Name (_ADR, 0x00000000)

	/*
	 * _DSM Device Specific Method
	 *
	 * Arg0: UUID: E5C937D0-3553-4d7a-9117-EA4D19C3434D
	 * Arg1: Revision ID: 3
	 * Arg2: Function index: 0, 9
	 * Arg3: Empty package
	 */
	Method (_DSM, 4, Serialized)
	{
		If (Arg0 == ToUUID("E5C937D0-3553-4d7a-9117-EA4D19C3434D")) {
			If (Arg1 >= 3) {
				If (Arg2 == 0) {
					/*
					 * Function index: 0
					 * Standard query - A bitmask of functions supported
					 */
					CreateBitField(OPTS, 9, FUN9)
					FUN9 = 1
					Return (OPTS)
				} ElseIf (Arg2 == 9) {
					/*
					 * Function index: 9
					 * Specifying device readiness durations
					 */
					Return (Package() { FW_RESET_TIME, FW_DL_UP_TIME,
							FW_FLR_RESET_TIME, FW_D3HOT_TO_D0_TIME,
							FW_VF_ENABLE_TIME })
				}
			}
		}
		Return (Buffer() { 0x0 })
	}

	Method (_PRW, 0)
	{
		Return (Package() { 0x69, 4 })
	}
}

Method (_DSW, 3)
{
	/* If entering Sx (Arg1 > 1), need to skip TCSS D3Cold & TBT RTD3/D3Cold. */
	If ((TUID == 0) || (TUID == 1)) {
		\_SB.PCI0.TDM0.SD3C = Arg1
	} Else {
		\_SB.PCI0.TDM1.SD3C = Arg1
	}

	C2PM (Arg0, Arg1, Arg2, DCPM)
}

Method (_PRW, 0)
{
	Return (Package() { 0x69, 4 })
}

/*
 * Sub-Method of _L61 Hot-Plug event
 * _L61 event handler should invoke this method to support HotPlug wake event from TBT RP.
 */
Method (HPEV, 0, Serialized)
{
	If ((VDID != 0xFFFFFFFF) && HPSX) {
		If ((PDCX == 1) && (DLSC == 1)) {
			/* Clear all status bits first. */
			PDCX = 1
			HPSX = 1

			/* Perform proper notification to the OS. */
			Notify (^, 0)
		} Else {
			/* False event. Clear Hot-Plug Status, then exit. */
			HPSX = 1
		}
	}
}

/*
 * Power Management routine for D3
 */
Name (STAT, 0x1)  /* Variable to save power state 1 - D0, 0 - D3C */

/*
 * RTD3 Exit Method to bring TBT controller out of RTD3 mode.
 */
Method (D3CX, 0, Serialized)
{
	If (STAT == 0x1) {
		Return
	}

	RPFE = 0  /* Set RTD3PFETDIS = 0 */
	RPER = 0  /* Set RTD3PERST = 0 */
	L23R = 1  /* Set L23r2dt = 1 */

	/*
	 * Poll for L23r2dt == 0. Wait for transition to Detect.
	 */
	Local0 = 0
	Local1 = L23R
	While (Local1) {
		If (Local0 > 20) {
			Break
		}
		Sleep(5)
		Local0++
		Local1 = L23R
	}
	STAT = 0x1
}

/*
 * RTD3 Entry method to enable TBT controller RTD3 mode.
 */
Method (D3CE, 0, Serialized)
{
	If (STAT == 0x0) {
		Return
	}

	L23E = 1  /* Set L23er = 1 */

	/* Poll until L23er == 0 */
	Local0 = 0
	Local1 = L23E
	While (Local1) {
		If (Local0 > 20) {
			Break
		}
		Sleep(5)
		Local0++
		Local1 = L23E
	}

	STAT = 0  /* D3Cold */
	RPFE = 1  /* Set RTD3PFETDIS = 1 */
	RPER = 1  /* Set RTD3PERST = 1 */
}

Method (_PS0, 0, Serialized)
{
	HPEV ()  /* Check and handle Hot Plug SCI status. */
	If (HPEX == 1) {
		HPEX = 0  /* Disable Hot Plug SCI */
	}
	HPME ()  /* Check and handle PME SCI status */
	If (PMEX == 1) {
		PMEX = 0  /* Disable Power Management SCI */
	}
}

Method (_PS3, 0, Serialized)
{
	/* Check it is hotplug SCI or not, then clear PDC accordingly */
	If (PDCX == 1) {
		If (DLSC == 0) {
			/* Clear PDC since it is not a hotplug. */
			PDCX = 1
		}
	}

	If (HPEX == 0) {
		HPEX = 1  /* Enable Hot Plug SCI. */
		HPEV ()   /* Check and handle Hot Plug SCI status. */
	}
	If (PMEX == 0) {
		PMEX = 1  /* Enable Power Management SCI. */
		HPME ()   /* Check and handle PME SCI status. */
	}
}

Method (_S0W, 0x0, NotSerialized)
{
	Return (0x4)
}

Method (_PR0)
{
	If ((TUID == 0) || (TUID == 1)) {
		Return (Package() { \_SB.PCI0.D3C, \_SB.PCI0.TBT0 })
	} Else {
		Return (Package() { \_SB.PCI0.D3C, \_SB.PCI0.TBT1 })
	}
}

Method (_PR3)
{
	If ((TUID == 0) || (TUID == 1)) {
		Return (Package() { \_SB.PCI0.D3C, \_SB.PCI0.TBT0 })
	} Else {
		Return (Package() { \_SB.PCI0.D3C, \_SB.PCI0.TBT1 })
	}
}

/*
 * PCI_EXP_STS Handler for PCIE Root Port
 */
Method (HPME, 0, Serialized)
{
	If ((VDID != 0xFFFFFFFF) && (PMSX == 1)) {  /* if port exists and PME SCI Status set */
		/*
		 * Notify child device; this will cause its driver to clear PME_Status from
		 * device.
		 */
		Notify (PXSX, 0x2)
		PMSX = 1  /* clear rootport's PME SCI status */
		/*
		 * Consume one pending PME notification to prevent it from blocking the queue.
		 */
		PSPX = 1
		Return (0x01)
	}
	Return (0x00)
}
