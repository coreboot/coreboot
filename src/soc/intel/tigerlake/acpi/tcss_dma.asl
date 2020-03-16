/*
 * This file is part of the coreboot project.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

OperationRegion (DPME, SystemMemory, BASE(_ADR), 0x100)
Field (DPME, AnyAcc, NoLock, Preserve)
{
	VDID, 32,
	Offset(0x84),   /* 0x84, DMA CFG PM CAP */
	PMST, 2,        /* 1:0, PM_STATE */
	, 6,
	PMEE, 1,        /* 8, PME_EN */
	, 6,
	PMES, 1,        /* 15, PME_STATUS */
	Offset(0xC8),   /* 0xC8, TBT NVM FW Revision */
	,     31,
	INFR,  1,       /* TBT NVM FW Ready */
	Offset(0xEC),   /* 0xEC, TBT TO PCIE Register */
	TB2P, 32,       /* TBT to PCIe */
	P2TB, 32,       /* PCIe to TBT */
	Offset(0xFC),   /* 0xFC, DMA RTD3 Force Power */
	DD3E, 1,        /* 0:0 DMA RTD3 Enable */
	DFPE, 1,        /* 1:1 DMA Force Power */
	, 22,
	DMAD, 8         /* 31:24 DMA Active Delay */
}

/*
 * TBT MailBox Command Method
 * Arg0 - MailBox Cmd ID
 */
Method (ITMB, 1, Serialized)
{
	Local0 = Arg0 | 0x1  /* 0x1, PCIE2TBT_VLD_B */
	P2TB = Local0
}

/*
 * Wait For Command Completed
 * Arg0 - TimeOut value (unit is 1 millisecond)
 */
Method (WFCC, 1, Serialized)
{
	WTBS (Arg0)
	P2TB = 0
	WTBC (Arg0)
}

/*
 * Wait For Command Set
 * Arg0 - TimeOut value
 */
Method (WTBS, 1, Serialized)
{
	Local0 = Arg0
	While (Local0 > 0) {
		/* Wait for Bit to Set. */
		If (TB2P & 0x1) {  /* 0x1, TBT2PCIE_DON_R */
			Break
		}
		Local0--
		Sleep (1)
	}
}

/*
 * Wait For Command Clear
 * Arg0 - TimeOut value
 */
Method (WTBC, 1, Serialized)
{
	Local0 = Arg0
	While (Local0 > 0) {
		/* Wait for Bit to Clear. */
		If ((TB2P & 0x1) != 0x0) {  /* 0x1, TBT2PCIE_DON_R */
			Break
		}
		Local0--
		Sleep (1)
	}
}

/*
 * TCSS TBT CONNECT_TOPOLOGY MailBox Command Method
 */
Method (CNTP, 0, Serialized)
{
	Local0 = 0
	/* Set Force Power if it is not set */
	If (DFPE == 0) {
		DMAD = 0x22
		DFPE = 1
		/*
		 * Poll the TBT NVM FW Ready bit with timeout(default is 500ms) before
		 * send the TBT MailBox command.
		 */
		While ((INFR == 0) && (Local0 < 500)) {
			Sleep (1)
			Local0++
		}
	}
	If (Local0 != 100) {
		ITMB (0x3E)  /* 0x3E, PCIE2TBT_CONNECT_TOPOLOGY_COMMAND */
	}
}

Name (STAT, 0x1)  /* Variable to save power state 1 - D0, 0 - D3C */
Name (ALCT, 0x0)  /* Connect-Topology cmd can be sent or not 1 - yes, 0 - no */
/*
 * Wait Connect-Topology cmd done
 * 0 - no need to wait
 * 1 - need to wait
 * 2 - wait in progress
 */
Name (WACT, 0x0)

Method (_PS0, 0, Serialized)
{
	If (WACT == 1) {
		/*
		 * PCIe rp0/rp1 is grouped with DMA0 and PCIe rp2/rp3 is grouped wit DMA1.
		 * Whenever the Connect-Topology command is in the process, WACT flag is set 1.
		 * PCIe root ports 0/1/2/3/ and DMA 0/1 _PS0 method set WACT to 2 to indicate
		 * other thread's _PS0 to wait for the command completion. WACT is cleared to
		 * be 0 after command is finished.
		 */
		WACT = 2
		WFCC (100)  /* Wait for command complete. */
		WACT = 0
	} ElseIf (WACT == 2) {
		While (WACT != 0) {
			Sleep (5)
		}
	}
}

Method (_PS3, 0, Serialized)
{
}

Method (_S0W, 0x0)
{
	Return (0x4)
}

Method (_PR0)
{
	If (DUID == 0) {
		Return (Package() { \_SB.PCI0.D3C, \_SB.PCI0.TBT0 })
	} Else {
		Return (Package() { \_SB.PCI0.D3C, \_SB.PCI0.TBT1 })
	}
}

Method (_PR3)
{
	If (DUID == 0) {
		Return (Package() { \_SB.PCI0.D3C, \_SB.PCI0.TBT0 })
	} Else {
		Return (Package() { \_SB.PCI0.D3C, \_SB.PCI0.TBT1 })
	}
}

/*
 * RTD3 Exit Method to bring TBT controller out of RTD3 mode.
 */
Method (D3CX, 0, Serialized)
{
	DD3E = 0  	/* Disable DMA RTD3 */
	STAT = 0x1
}

/*
 * RTD3 Entry method to enable TBT controller RTD3 mode.
 */
Method (D3CE, 0, Serialized)
{
	DD3E = 1  	/* Enable DMA RTD3 */
	STAT = 0
	ALCT = 0x1  	/* Allow to send Connect-Topology cmd. */
}

/*
 * Variable to skip TCSS/TBT D3 cold; 1+: Skip D3CE, 0 - Enable D3CE
 * TCSS D3 Cold and TBT RTD3 is only available when system power state is in S0.
 */
Name (SD3C, 0)

Method (_DSW, 3)
{
	/* If entering Sx (Arg1 > 1), need to skip TCSS D3Cold & TBT RTD3/D3Cold. */
	SD3C = Arg1
}

Method (_PRW, 0)
{
	Return (Package() { 0x6D, 4 })
}

Method (_DSD, 0)
{
	Return(
		Package()
		{
			/* Thunderbolt GUID for IMR_VALID at ../drivers/acpi/property.c */
			ToUUID("C44D002F-69F9-4E7D-A904-A7BAABDF43F7"),
			Package ()
			{
				Package (2) { "IMR_VALID", 1 }
			},

			/* Thunderbolt GUID for WAKE_SUPPORTED at ../drivers/acpi/property.c */
			ToUUID("6C501103-C189-4296-BA72-9BF5A26EBE5D"),
			Package ()
			{
				Package (2) { "WAKE_SUPPORTED", 1 }
			}
		}
	)
}

Method (_DSM, 4, Serialized)
{
	Return (Buffer() { 0 })
}
