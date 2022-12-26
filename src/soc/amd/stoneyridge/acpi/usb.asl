/* SPDX-License-Identifier: GPL-2.0-only */

/* 0:12.0 - EHCI */
Device(EHC0) {
	Name(_ADR, 0x00120000)
	Name(_PRW, Package() { 0xb, 3 })
	Device (RHUB) {
		Name (_ADR, 0)
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
			XHD0 = 0x01
		}
		Method(_OFF) {
			XHD0 = 0x00
		}
	}
	PowerResource(P3U3, 0, 0) {
		Method(_STA) {
			Return(XHD3)
		}
		Method(_ON) {
			XHD3 = 0x01
		}
		Method(_OFF) {
			XHD3 = 0x00
		}
	}

	Name(EHD0, 0)
	Name(EHD3, 0)
	PowerResource(P0U2, 0, 0) {
		Method(_STA) {
			Return(EHD0)
		}
		Method(_ON) {
			EHD0 = 0x01
		}
		Method(_OFF) {
			EHD0 = 0x00
		}
	}
	PowerResource(P3U2, 0, 0) {
		Method(_STA) {
			Return(EHD3)
		}
		Method(_ON) {
			EHD3 = 0x01
		}
		Method(_OFF) {
			EHD3 = 0x00
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
	if (EH10 != 0) {
		EHBA = EH10
		EHME = 1
		SSIM = ESIM
	}

	if (E_PS == 3) {
		RQTY = 0
		RQ18 = 1

		Local0 = U2SR
		while (Local0) {
			Local0 = U2SR
		}

		U2PD = 0

		Local0 = U2DS
		while (Local0 != 0) {
			Local0 = U2DS
		}

		U2TD = 0x03

		if (U3TD == 0x03) { /* Shutdown USB2 PLL */
		    PWGC (0x40, 0)
		    U2RP = 1
		}
	}
}

Method(U2D0,0, Serialized)
{
	PWGC (0x40, 1)
	U2RP = 0
	U2TD =  0x00

	U2TD = 0
	U2PD = 1

	Local0 = U2DS
	while (Local0 != 0x7) {
		Local0 = U2DS
	}

	RQTY = 1
	RQ18 = 1
	Local0 = U2SR
	while (!Local0) {
		Local0 = U2SR
	}
	EH2C = EHID


	if (EH10 != 0) {
		EHBA = EH10
		EHME = 1
		ESIM = SSIM
	}

	EH54 = ES54
	EH64 = ES64
}

Method(LXFW,3, Serialized)  //Load Xhci FirmWare
{
	FWLM = 1  /* Firmware Load Mode */
	ROAM = Arg0 /* ROM/RAM */
	UA04 = Arg1
	UA08 = Arg2
	FPLS = 1    /* Firmware Preload Start */
	Local0 = FPLC /* Firmware Preload Complete */
	while (!Local0) {
		Local0 = FPLC
	}
	FPLS = 0
}

Method(U3D3,0, Serialized)
{
	if (U_PS == 3) {
		X0_S ()

		PGA3 |= 0x20 /* SwUsb3SlpShutdown */
		Local0 = PGA3 & 0x20
		while (!Local0) { /* wait for it to complete */
			Local0 = PGA3 & 0x20
		}
		UD3P = 1 /* U3P_D3Cold_PWRDN */

		U3PD = 0 /* PwrOnDev */
		Local0 = U3DS
		while (Local0) { /* RstBState, RefClkOkState, PwrRstBState */
			Local0 = U3DS
		}

		U3TD = 0x3 /* TargetedDeviceState */

		U3RP = 1 /* USB3_RefClk_Pwdn */

		if (U2TD == 0x3) { /* If EHCI targeted in D3cold */
			PGA3 &= 0x9f /* SwUsb2S5RstB */
			U2RP = 1   /* USB2_RefClk_Pwdn */
		}
		U3PG = 0 /* XhcPwrGood  */
		U3PS = 1  /* Usb3PowerSel */
	}
}

Method(U3D0,0, Serialized)
{
	U3PS = 0 /* Usb3PowerSel */
	U3PG = 1  /* XhcPwrGood  */

	U2RP = 0
	U3RP = 0

	Local0 = PGA3 & 0xdf
	Local0 |= 0x40
	PGA3 = Local0 /* SwUsb2S5RstB */

	U3TD = 0 /* TargetedDeviceState */
	U3PD = 1  /* PwrOnDev */

	Local0 = U3DS /* wait for RstBState, RefClkOkState, PwrRstBState */
	while (Local0 != 0x7) {
		Local0 = U3DS
	}

	Local0 = U3PY /* USB3 PHY Lock */
	while (!Local0) {
		Local0 = U3PY
	}

	U3PR = 0 /* U3P_RESTORE_RESET */

	Local0 = AUSS /* AutoSizeStart */
	if (Local0 != 1) {
		AUSS = 1
	}
	Local0 = AUSS
	while (Local0 != 1) {
		Local0 = AUSS
	}

	LXFW (1, FW00, FW01)
	LXFW (0, FW02, FW03)

	X0_R ()

	U3PR = 1  /* U3P_RESTORE_RESET */
	UD3P = 0 /* U3P_D3Cold_PWRDN */
	U3TD = 1  /* TargetedDeviceState */
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
	XHID = XH2C
	IDEX = 0
	S000 = DATA
	IDEX = 0x00000004
	S004 = DATA
	IDEX = 0x00000008
	S008 = DATA
	IDEX = 0x0000000C
	S00C = DATA
	IDEX = 0x00000018
	S018 = DATA
	IDEX = 0x0000001C
	S01C = DATA
	IDEX = 0x00000020
	S020 = DATA
	IDEX = 0x00000030
	S030 = DATA
	IDEX = 0x00000118
	S118 = DATA
	IDEX = 0x00000158
	S158 = DATA
	IDEX = 0x00000198
	S198 = DATA
	IDEX = 0x000001D8
	S1D8 = DATA
	IDEX = 0x00000300
	S300 = DATA
	IDEX = 0x00000304
	S304 = DATA
	IDEX = 0x00000308
	S308 = DATA
	IDEX = 0x0000030C
	S30C = DATA
	IDEX = 0x00000310
	S310 = DATA
	IDEX = 0x40000028
	S428 = DATA
	IDEX = 0x40000038
	S438 = DATA
	IDEX = 0x4000003C
	S43C = DATA
	IDEX = 0x40000058
	S458 = DATA
	IDEX = 0x40000068
	S468 = DATA
	IDEX = 0x4000006C
	S46C = DATA
	IDEX = 0x40000070
	S470 = DATA
	IDEX = 0x40000080
	S480 = DATA
	IDEX = 0x40000084
	S484 = DATA
	IDEX = 0x40000088
	S488 = DATA
	IDEX = 0x4000008C
	S48C = DATA
}

Method(X0_R,0)
{
	XH2C = XHID
	IDEX = 0
	DATA = S000
	IDEX = 0x000000004
	DATA = S004
	IDEX = 0x000000008
	DATA = S008
	IDEX = 0x00000000C
	DATA = S00C
	IDEX = 0x000000018
	DATA = S018
	IDEX = 0x00000001C
	DATA = S01C
	IDEX = 0x000000020
	DATA = S020
	IDEX = 0x00000030
	DATA = S030
	IDEX = 0x00000118
	DATA = S118
	IDEX = 0x00000158
	DATA = S158
	IDEX = 0x00000198
	DATA = S198
	IDEX = 0x000001D8
	DATA = S1D8
	IDEX = 0x00000300
	DATA = S300
	IDEX = 0x00000304
	DATA = S304
	IDEX = 0x00000308
	DATA = S308
	IDEX = 0x0000030C
	DATA = S30C
	IDEX = 0x00000310
	DATA = S310
	IDEX = 0x40000028
	DATA = S428
	IDEX = 0x40000038
	DATA = S438
	IDEX = 0x4000003C
	DATA = S43C
	IDEX = 0x40000058
	DATA = S458
	IDEX = 0x40000068
	DATA = S468
	IDEX = 0x4000006C
	DATA = S46C
	IDEX = 0x40000070
	DATA = S470
	IDEX = 0x40000080
	DATA = S480
	IDEX = 0x40000084
	DATA = S484
	IDEX = 0x40000088
	DATA = S488
	IDEX = 0x4000008C
	DATA = S48C
}
