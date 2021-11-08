/* SPDX-License-Identifier: GPL-2.0-or-later */

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

Name (STAT, 0x1)  /* Variable to save power state 1 - D0, 0 - D3C */

Method (_S0W, 0x0)
{
	If (S0IX == 1) {
		Return (0x04)
	} Else {
		Return (0x03)
	}
}

/*
 * Get power resources that are dependent on this device for Operating System Power Management
 * to put the device in the D0 device state
 */
Method (_PR0)
{
	If (S0IX == 1) {
		If (DUID == 0) {
			Return (Package() { \_SB.PCI0.D3C, \_SB.PCI0.TBT0 })
		} Else {
			Return (Package() { \_SB.PCI0.D3C, \_SB.PCI0.TBT1 })
		}
	} Else {
		If (DUID == 0) {
			Return (Package() { \_SB.PCI0.TBT0 })
		} Else {
			Return (Package() { \_SB.PCI0.TBT1 })
		}
	}
}

Method (_PR3)
{
	If (S0IX == 1) {
		If (DUID == 0) {
			Return (Package() { \_SB.PCI0.D3C, \_SB.PCI0.TBT0 })
		} Else {
			Return (Package() { \_SB.PCI0.D3C, \_SB.PCI0.TBT1 })
		}
	} Else {
		If (DUID == 0) {
			Return (Package() { \_SB.PCI0.TBT0 })
		} Else {
			Return (Package() { \_SB.PCI0.TBT1 })
		}
	}
}

/*
 * RTD3 Exit Method to bring TBT controller out of RTD3 mode.
 */
Method (D3CX, 0, Serialized)
{
	DD3E = 0x00	/* Disable DMA RTD3 */
	STAT = 0x01
}

/*
 * RTD3 Entry method to enable TBT controller RTD3 mode.
 */
Method (D3CE, 0, Serialized)
{
	DD3E = 0x01	/* Enable DMA RTD3 */
	STAT = 0x00
}

/*
 * Variable to skip TCSS/TBT D3 cold; 1+: Skip D3CE, 0 - Enable D3CE
 * TCSS D3 Cold and TBT RTD3 is only available when system power state is in S0.
 */
Name (SD3C, 0)
Method (_PS0, 0, Serialized)
{
}

Method (_PS3, 0, Serialized)
{
}

Method (_DSW, 3)
{
	/* If entering Sx (Arg1 > 1), need to skip TCSS D3Cold & TBT RTD3/D3Cold. */
	SD3C = Arg1
}

Method (_PRW, 0)
{
	Return (Package() { 0x6D, 4 })
}
