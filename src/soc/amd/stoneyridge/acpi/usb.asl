/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* 0:12.0 - EHCI */
Device(EHC0) {
	Name(_ADR, 0x00120000)
	Name(_PRW, Package() { 0xb, 3 })
	Device (RHUB) {
		Name (_ADR, Zero)
		Device (HS01) { Name (_ADR, 1) }
		Device (HS02) { Name (_ADR, 2) }
		Device (HS03) { Name (_ADR, 3) }
		Device (HS04) { Name (_ADR, 4) }
		Device (HS05) { Name (_ADR, 5) }
		Device (HS06) { Name (_ADR, 6) }
		Device (HS07) { Name (_ADR, 7) }
		Device (HS08) { Name (_ADR, 8) }
	}

	Name(_PR0, Package() { P0U2 }) /* Indicate support for D0 */
	Name(_PR3, Package() { P3U2 }) /* Indicate support for D3cold */

	Method(_S0W,0) {
		Return(0)
	}

	Method(_S3W,0) {
		Return(4)
	}

	Method(_S4W,0) {
		Return(4)
	}
} /* end EHC0 */


/* 0:10.0 - XHCI 0*/
Device(XHC0) {
	Name(_ADR, 0x00100000)
	Name(_PRW, Package() { 0xb, 3 })
	Device (SS01) { Name (_ADR, 1) }
	Device (SS02) { Name (_ADR, 2) }
	Device (SS03) { Name (_ADR, 3) }

	Name(_PR0, Package() { P0U3 }) /* Indicate support for D0 */
	Name(_PR3, Package() { P3U3 }) /* Indicate support for D3cold */

	Method(_S0W,0) {
		Return(0)
	}

	Method(_S3W,0) {
		Return(4)
	}

	Method(_S4W,0) {
		Return(4)
	}

} /* end XHC0 */

Scope(\_SB)
{
	Name(XHD0, 0)
	Name(XHD3, 0)
	PowerResource(P0U3, 0, 0) {
		Method(_STA) {
			Return(XHD0)
		}
		Method(_ON) {
			Store(0x01, XHD0)
		}
		Method(_OFF) {
			Store(0x00, XHD0)
		}
	}
	PowerResource(P3U3, 0, 0) {
		Method(_STA) {
			Return(XHD3)
		}
		Method(_ON) {
			Store(0x01, XHD3)
		}
		Method(_OFF) {
			Store(0x00, XHD3)
		}
	}

	Name(EHD0, 0)
	Name(EHD3, 0)
	PowerResource(P0U2, 0, 0) {
		Method(_STA) {
			Return(EHD0)
		}
		Method(_ON) {
			Store(0x01, EHD0)
		}
		Method(_OFF) {
			Store(0x00, EHD0)
		}
	}
	PowerResource(P3U2, 0, 0) {
		Method(_STA) {
			Return(EHD3)
		}
		Method(_ON) {
			Store(0x01, EHD3)
		}
		Method(_OFF) {
			Store(0x00, EHD3)
		}
	}
}

OperationRegion(EHMC, SystemMemory, EH10, 0x100)
Field(EHMC, DwordAcc, NoLock, Preserve)
{
	Offset(0xb0),
	, 5,
	ESIM, 1,
}

Method(U2D3,0, Serialized)
{
	if (LNotEqual(EH10, Zero)) {
		Store (EH10, EHBA)
		Store (One, EHME)
		Store (ESIM, SSIM)
	}

	if (LEqual(E_PS, 3)) {
		Store (Zero, RQTY)
		Store (One, RQ18)

		Store (U2SR, Local0)
		while (Local0) {
			Store (U2SR, Local0)
		}

		Store (Zero, U2PD)

		Store (U2DS, Local0)
		while (LNotEqual(Local0, Zero)) {
			Store (U2DS, Local0)
		}

		Store (0x03,U2TD)

		if (LEqual(U3TD, 0x03)) { /* Shutdown USB2 PLL */
		    PWGC (0x40, 0)
		    Store (One, U2RP)
		}
	}
}

Method(U2D0,0, Serialized)
{
	PWGC (0x40, 1)
	Store (Zero, U2RP)
	Store (0x00,U2TD)

	Store (Zero, U2TD)
	Store (One, U2PD)

	Store (U2DS, Local0)
	while (LNotEqual(Local0,0x7)) {
		Store (U2DS, Local0)
	}

	Store (One, RQTY)
	Store (One, RQ18)
	Store (U2SR, Local0)
	while (LNot(Local0)) {
		Store (U2SR, Local0)
	}
	Store (EHID, EH2C)


	if (LNotEqual(EH10, Zero)) {
		Store (EH10, EHBA)
		Store (One, EHME)
		Store (SSIM, ESIM)
	}

	Store (ES54, EH54)
	Store (ES64, EH64)
}

Method(LXFW,3, Serialized)  //Load Xhci FirmWare
{
	Store (One, FWLM)  /* Firmware Load Mode */
	Store (Arg0, ROAM) /* ROM/RAM */
	Store (Arg1, UA04)
	Store (Arg2, UA08)
	Store (One, FPLS)    /* Firmware Preload Start */
	Store (FPLC, Local0) /* Firmware Preload Complete */
	while (LNot(Local0)) {
		Store (FPLC, Local0)
	}
	Store (Zero, FPLS)
}

Method(U3D3,0, Serialized)
{
	if (LEqual(U_PS, 3)) {
		X0_S ()

		Or (PGA3, 0x20, PGA3) /* SwUsb3SlpShutdown */
		And (PGA3, 0x20, Local0)
		while (LNot(Local0)) { /* wait for it to complete */
			And (PGA3, 0x20, Local0)
		}
		Store (One, UD3P) /* U3P_D3Cold_PWRDN */

		Store (Zero, U3PD) /* PwrOnDev */
		Store (U3DS, Local0)
		while (Local0) { /* RstBState, RefClkOkState, PwrRstBState */
			Store (U3DS, Local0)
		}

		Store (0x3, U3TD) /* TargetedDeviceState */

		Store (One, U3RP) /* USB3_RefClk_Pwdn */

		if (Lequal(U2TD, 0x3)) { /* If EHCI targeted in D3cold */
			And (PGA3, 0x9f, PGA3) /* SwUsb2S5RstB */
			Store (One, U2RP)      /* USB2_RefClk_Pwdn */
		}
		Store (Zero, U3PG) /* XhcPwrGood  */
		Store (One, U3PS)  /* Usb3PowerSel */
	}
}

Method(U3D0,0, Serialized)
{
	Store (Zero, U3PS) /* Usb3PowerSel */
	Store (One, U3PG)  /* XhcPwrGood  */

	Store (Zero, U2RP)
	Store (Zero, U3RP)

	And (PGA3, 0xdf, Local0)
	Or (Local0, 0x40, Local0)
	Store (Local0, PGA3) /* SwUsb2S5RstB */

	Store (Zero, U3TD) /* TargetedDeviceState */
	Store (One, U3PD)  /* PwrOnDev */

	Store (U3DS, Local0) /* wait for RstBState, RefClkOkState, PwrRstBState */
	while (LNot(Lequal(Local0, 0x7))) {
		Store (U3DS, Local0)
	}

	Store (U3PY, Local0) /* USB3 PHY Lock */
	while (LNot(Local0)) {
		Store (U3PY, Local0)
	}

	Store (Zero, U3PR) /* U3P_RESTORE_RESET */

	Store (AUSS, Local0) /* AutoSizeStart */
	if (LNotEqual(Local0,1)) {
		Store(One, AUSS)
	}
	Store (AUSS, Local0)
	while (LNotEqual(Local0,1)) {
		Store (AUSS, Local0)
	}

	LXFW (1, FW00, FW01)
	LXFW (0, FW02, FW03)

	X0_R ()

	Store (One, U3PR)  /* U3P_RESTORE_RESET */
	Store (Zero, UD3P) /* U3P_D3Cold_PWRDN */
	Store (One, U3TD)  /* TargetedDeviceState */
}

Name (SVBF, Buffer (0x1000) {0})    /* length from FchCarrizo.asl, new fields */
CreateDWordField(SVBF, 0x000, S000) /* will be easier to add from there */
CreateDWordField(SVBF, 0x004, S004)
CreateDWordField(SVBF, 0x008, S008)
CreateDWordField(SVBF, 0x00C, S00C)
CreateDWordField(SVBF, 0x018, S018)
CreateDWordField(SVBF, 0x01C, S01C)
CreateDWordField(SVBF, 0x020, S020)
CreateDWordField(SVBF, 0x030, S030)
CreateDWordField(SVBF, 0x118, S118)
CreateDWordField(SVBF, 0x158, S158)
CreateDWordField(SVBF, 0x198, S198)
CreateDWordField(SVBF, 0x1D8, S1D8)
CreateDWordField(SVBF, 0x300, S300)
CreateDWordField(SVBF, 0x304, S304)
CreateDWordField(SVBF, 0x308, S308)
CreateDWordField(SVBF, 0x30C, S30C)
CreateDWordField(SVBF, 0x310, S310)
CreateDWordField(SVBF, 0x428, S428)
CreateDWordField(SVBF, 0x438, S438)
CreateDWordField(SVBF, 0x43C, S43C)
CreateDWordField(SVBF, 0x458, S458)
CreateDWordField(SVBF, 0x468, S468)
CreateDWordField(SVBF, 0x46C, S46C)
CreateDWordField(SVBF, 0x470, S470)
CreateDWordField(SVBF, 0x480, S480)
CreateDWordField(SVBF, 0x484, S484)
CreateDWordField(SVBF, 0x488, S488)
CreateDWordField(SVBF, 0x48C, S48C)
CreateDWordField(SVBF, 0x730, EHID) /* EHCI SSID    */
CreateDWordField(SVBF, 0x734, XHID) /* XHCI SSID    */
CreateByteField(SVBF, 0x740, ES54)  /* EHCI PCIx54  */
CreateByteField(SVBF, 0x741, ES64)  /* EHCI PCIx64  */
CreateDWordField(SVBF, 0x7B0, SSIM) /* EHCI SIM BIT */

Method(X0_S,0)
{
	Store (XH2C, XHID)
	Store (0x00000000, IDEX) Store (DATA, S000)
	Store (0x00000004, IDEX) Store (DATA, S004)
	Store (0x00000008, IDEX) Store (DATA, S008)
	Store (0x0000000c, IDEX) Store (DATA, S00C)
	Store (0x00000018, IDEX) Store (DATA, S018)
	Store (0x0000001c, IDEX) Store (DATA, S01C)
	Store (0x00000020, IDEX) Store (DATA, S020)
	Store (0x00000030, IDEX) Store (DATA, S030)
	Store (0x00000118, IDEX) Store (DATA, S118)
	Store (0x00000158, IDEX) Store (DATA, S158)
	Store (0x00000198, IDEX) Store (DATA, S198)
	Store (0x000001d8, IDEX) Store (DATA, S1D8)
	Store (0x00000300, IDEX) Store (DATA, S300)
	Store (0x00000304, IDEX) Store (DATA, S304)
	Store (0x00000308, IDEX) Store (DATA, S308)
	Store (0x0000030c, IDEX) Store (DATA, S30C)
	Store (0x00000310, IDEX) Store (DATA, S310)
	Store (0x40000028, IDEX) Store (DATA, S428)
	Store (0x40000038, IDEX) Store (DATA, S438)
	Store (0x4000003c, IDEX) Store (DATA, S43C)
	Store (0x40000058, IDEX) Store (DATA, S458)
	Store (0x40000068, IDEX) Store (DATA, S468)
	Store (0x4000006c, IDEX) Store (DATA, S46C)
	Store (0x40000070, IDEX) Store (DATA, S470)
	Store (0x40000080, IDEX) Store (DATA, S480)
	Store (0x40000084, IDEX) Store (DATA, S484)
	Store (0x40000088, IDEX) Store (DATA, S488)
	Store (0x4000008c, IDEX) Store (DATA, S48C)
}

Method(X0_R,0)
{
	Store (XHID, XH2C)
	Store (0x00000000, IDEX) Store (S000, DATA)
	Store (0x00000004, IDEX) Store (S004, DATA)
	Store (0x00000008, IDEX) Store (S008, DATA)
	Store (0x0000000c, IDEX) Store (S00C, DATA)
	Store (0x00000018, IDEX) Store (S018, DATA)
	Store (0x0000001c, IDEX) Store (S01C, DATA)
	Store (0x00000020, IDEX) Store (S020, DATA)
	Store (0x00000030, IDEX) Store (S030, DATA)
	Store (0x00000118, IDEX) Store (S118, DATA)
	Store (0x00000158, IDEX) Store (S158, DATA)
	Store (0x00000198, IDEX) Store (S198, DATA)
	Store (0x000001d8, IDEX) Store (S1D8, DATA)
	Store (0x00000300, IDEX) Store (S300, DATA)
	Store (0x00000304, IDEX) Store (S304, DATA)
	Store (0x00000308, IDEX) Store (S308, DATA)
	Store (0x0000030c, IDEX) Store (S30C, DATA)
	Store (0x00000310, IDEX) Store (S310, DATA)
	Store (0x40000028, IDEX) Store (S428, DATA)
	Store (0x40000038, IDEX) Store (S438, DATA)
	Store (0x4000003c, IDEX) Store (S43C, DATA)
	Store (0x40000058, IDEX) Store (S458, DATA)
	Store (0x40000068, IDEX) Store (S468, DATA)
	Store (0x4000006c, IDEX) Store (S46C, DATA)
	Store (0x40000070, IDEX) Store (S470, DATA)
	Store (0x40000080, IDEX) Store (S480, DATA)
	Store (0x40000084, IDEX) Store (S484, DATA)
	Store (0x40000088, IDEX) Store (S488, DATA)
	Store (0x4000008c, IDEX) Store (S48C, DATA)
}
