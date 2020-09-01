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

Device (PXSX)
{
	Name (_ADR, 0x00000000)

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

	/* Wait for LA = 1 */
	Local0 = 0
	Local1 = LASX
	While (Local1 == 0) {
		If (Local0 > 20) {
			Break
		}
		Sleep(5)
		Local0++
		Local1 = LASX
	}
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

	Sleep(100)  /* Wait for 100ms before return to OS starts any OS activities. */
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

Method (_DSD, 0) {
	If ((TUID == 0) || (TUID == 1)) {
		Return ( Package() {
			/* acpi_pci_bridge_d3 at ../drivers/pci/pci-acpi.c */
			ToUUID("6211E2C0-58A3-4AF3-90E1-927A4E0C55A4"),
			Package ()
			{
				Package (2) { "HotPlugSupportInD3", 1 },
			},

			/* pci_acpi_set_untrusted at ../drivers/pci/pci-acpi.c */
			ToUUID("EFCC06CC-73AC-4BC3-BFF0-76143807C389"),
			Package () {
				Package (2) { "ExternalFacingPort", 1 },  /* TBT/CIO port */
				/*
				 * UID of the TBT RP on platform, range is: 0, 1 ...,
				 * (NumOfTBTRP - 1).
				 */
				Package (2) { "UID", TUID },
			},
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package (2) { "usb4-host-interface", \_SB.PCI0.TDM0 },
				Package (2) { "usb4-port-number", TUID },
			}
		})
	} ElseIf (TUID == 2) {
		Return ( Package () {
			/* acpi_pci_bridge_d3 at ../drivers/pci/pci-acpi.c */
			ToUUID("6211E2C0-58A3-4AF3-90E1-927A4E0C55A4"),
			Package ()
			{
				Package (2) { "HotPlugSupportInD3", 1 },
			},

			/* pci_acpi_set_untrusted at ../drivers/pci/pci-acpi.c */
			ToUUID("EFCC06CC-73AC-4BC3-BFF0-76143807C389"),
			Package () {
				Package (2) { "ExternalFacingPort", 1 },  /* TBT/CIO port */
				/*
				 * UID of the TBT RP on platform, range is: 0, 1 ...,
				 * (NumOfTBTRP - 1).
				 */
				Package (2) { "UID", TUID },
			},
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package (2) { "usb4-host-interface", \_SB.PCI0.TDM1 },
				Package (2) { "usb4-port-number", 0 },
			}
		})
	} Else {  /* TUID == 3 */
		Return ( Package () {
			/* acpi_pci_bridge_d3 at ../drivers/pci/pci-acpi.c */
			ToUUID("6211E2C0-58A3-4AF3-90E1-927A4E0C55A4"),
			Package ()
			{
				Package (2) { "HotPlugSupportInD3", 1 },
			},

			/* pci_acpi_set_untrusted at ../drivers/pci/pci-acpi.c */
			ToUUID("EFCC06CC-73AC-4BC3-BFF0-76143807C389"),
			Package () {
				Package (2) { "ExternalFacingPort", 1 },  /* TBT/CIO port */
				/*
				 * UID of the TBT RP on platform, range is: 0, 1 ...,
				 * (NumOfTBTRP - 1).
				 */
				Package (2) { "UID", TUID },
			},
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package (2) { "usb4-host-interface", \_SB.PCI0.TDM1 },
				Package (2) { "usb4-port-number", 1 },
			}
		})
	}
}

Method (_S0W, 0x0, NotSerialized)
{
	Return (0x4)
}

Method (_PR0)
{
	Return (Package() { \_SB.PCI0.D3C })
}

Method (_PR3)
{
	Return (Package() { \_SB.PCI0.D3C })
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
