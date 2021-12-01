/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Labeling:
 * - just offset bits
 * ? unknown / not fully understood, used by vendor fw / ec fw
 */

/*
 * Note: not all fields are used by all mainboards. Also some bits/fields are
 *       still unknown and will be amended as soon as more details are known.
 *       Naming differs from vendor firmware, since there were many completely
 *       wrong/misleading names. Fields unused by the vendor EC fw have been
 *       dropped, since they just were present due to vendor doing copy-pasting.
 */

OperationRegion (RAM1, SystemMemory, CONFIG_EC_CLEVO_IT5570E_MEM_BASE + 0x100, 0x400)
Field (RAM1, ByteAcc, Lock, Preserve)
{
	Offset (0x03),
	LSTE,	 1,	// Lid open					  3h.0
	    ,	 1,	// -						  3h.1
	LWKE,	 1,	// Lid wake enable				  3h.2

	Offset (0x04),
	 AC0,	 8,	// Active cooling temp 0			  4h
	 PSV,	 8,	// Passive cooling temp				  5h
	 CRT,	 8,	// Critical temp				  6h
	 TMP,	 8,	// CPU temp read from PECI			  7h
	 AC1,	 8,	// Active cooling temp 1			  8h
	BBST,	 8,	// ? dGPU related (BatteryBooST?)		  9h
	BTMP,	 8,	// Board temperature				  ah

	Offset (0x10),
	 ADP,	 1,	// AC connected					 10h.0
	    ,	 1,	// -						 10h.1
	BAT0,	 1,	// BAT0 connected				 10h.2

	Offset (0x11),
	WFNO,	 8,	// Wake cause					 11h
			//   0x01 = lid
			//   0x04 = sleep button
			//   0x05 = timer
			//   0x10 = battery low

	Offset (0x16),
	BDC0,	32,	// BAT0 design capacity				 16h-19h
	BFC0,	32,	// BAT0 last full charge capacity		 1ah-1dh

	Offset (0x22),
	BDV0,	32,	// BAT0 design voltage				 22h-25h
	BST0,	 8,	// BAT0 status					 26h
			//   BST0[0] : discharging
			//   BST0[1] : charging
			//   BST0[2] : critical low

	Offset (0x2a),
	BPR0,	32,	// BAT0 present current				 2ah-2dh
	BRC0,	32,	// BAT0 remaining capacity			 2eh-31h
	BPV0,	32,	// BAT0 present voltage				 32h-35h

	Offset (0x38),
	BRS0,	 8,	// BAT0 relative charge				 38h

	Offset (0x3a),
	BCW0,	32,	// Design capacity of warning			 3ah-3dh
	BCL0,	32,	// Design capacity of low			 3eh-41h

	Offset (0x4a),
	BMO0,	64,	// Model					 4ah-51h
	BIF0,	64,	// Vendor					 52h-59h
	BSN0,	32,	// Serial number				 5ah-5dh
	BTY0,	64,	// Type						 5eh-65h

	Offset (0x68),
	ECOS,	 8,	// ACPI OS support				 68h
			//   0 = no ACPI
			//   1 = ACPI w/o driver
			//   2 = ACPI w/  driver

	Offset (0x78),
	/*
	 * PECI
	 * Maybe usable for debugging. Must never be written directly!
	 */
	PCAD,	 8,	// PECI address					 78h
	PEWL,	 8,	// PECI write length				 79h
	PWRL,	 8,	// PECI read length				 7ah
	PECD,	 8,	// PECI command					 7bh
	PEHI,	 8,	// PECI host ID					 7ch
	PECI,	 8,	// PECI index					 7dh
	PEPL,	 8,	// PECI LSB					 7eh
	PEPM,	 8,	// PECI MSB					 7fh
	PWFC,	 8,	// ?						 80h
	PECC,	 8,	// PECI completion code				 81h
	PDT0,	 8,	// PECI data					 82h
	PDT1,	 8,	// PECI data					 83h
	PDT2,	 8,	// PECI data					 84h
	PDT3,	 8,	// PECI data					 85h

	Offset (0x92),
	BMD0,	16,	// BAT0 manufacturing date			 92h-93h
			//   BMD0[4:0]  : day
			//   BMD0[8:5]  : month
			//   BMD0[15:9] : year - 1980
	CYC0,   16,	// BAT0 cycle count				 94h-95h

	Offset (0xc7),
	VOFF,	 8,	// VGA fan base offset				 c7h
	FANC,	 8,	// FAN count (FANC == FANQ)			 c8h
	BLVL,	 8,	// Legacy display brightness level (unused)	 c9h

	Offset (0xca),
	    ,	 1,	// -						 cah.0
	    ,	 1,	// ?						 cah.1
	CAMK,	 1,	// Enable webcam hotkey				 cah.2
	    ,	 2,	// -						 cah.3-4
	WWAN,	 1,	// WWAN/3G/LTE present (enables WWAN)		 cah.5

	Offset (0xcb),
	    ,    5,	// -						 cbh.0-4
	B15C,	 1,	// ?						 cbh.5
	    ,    1,	// -						 cbh.6
	SLFG,	 1,	// silent fan mode flag				 cbh.7

	Offset (0xcc),
	SCIE,	 8,	// SCI extra value				 cch

	Offset (0xce),
	DUT1,	 8,	// Fan 1 duty					 ceh
	DUT2,	 8,	// Fan 2 duty					 cfh

	Offset (0xd0),
	RPM1,	16,	// Fan 1 RPM					 d0h-d1h
	RPM2,	16,	// Fan 2 RPM					 d2h-d3h
	RPM4,	16,	// Fan 4 RPM					 d4h-d5h

	Offset (0xd7),
	DTHL,	 8,	// ?						 d7h
	DTBP,	 8,	// ?						 d8h
	    ,	 1,	// -						 d9h.0
	WOLD,	 1,	// ? Disable Wake-on-LAN			 d9h.1
	PWRM,	 2,	// current performance profile			 d9h.2-3
			//   0 = entertainment
			//   1 = performance
			//   2 = quiet
			//   3 = powersave
	FN3E,	 1,	// Fn + 3 enable (power profile toggle)		 d9h.4
	    ,	 1,	// ?						 d9h.5
	AIRP,	 1,	// airplane mode status (in non-ACPI mode)	 d9h.6
	GPUP,	 1,	// dGPU power status				 d9h.7

	Offset (0xda),
	BLCT,	 1,	// ACPI backlight control			 dah.0
	DBGP,	 1,	// 3IN1 debug card present flag			 dah.1
	    ,	 1,	// WINF[2] ?					 dah.2
	MEUL,	 1,	// ME/IFD unlock (ACPI usage unclear)		 dah.3

	Offset (0xdb),
	RINF,	 8,	//						 dbh
			//   RINF[0] : set when EC cmd A8 was called
			//   RINF[1] : -
			//   RINF[2] : ? TP
			//   RINF[3] : ?
			//   RINF[4] : I2C TP SupportSandTPScanCode
			//   RINF[5] : ?
			//   RINF[6] : set on first airplane mode activation
			//   RINF[7] : ?
	 DBG,    8,	// P80 + 3in1 debug				 dch

	Offset (0xdd),
	    ,	 1,	//						 ddh.0
	    ,	 1,	//   INF2[1] : ?				 ddh.1
	    ,	 4,	// -						 ddh.2-5
	BWKE,	 1,	// S3 wake on low battery			 ddh.6
	FF2D,	 1,	// Fn + F2 (LCD off) disable			 ddh.7
	EID2,	 8,	// EC CHIPID LSB				 deh
	BWKT,    8,	// threshold for S3 wake on low battery		 dfh

	Offset (0xe0),
	RPM3,	16,	// Fan 3 RPM					 e0h-e1h

	Offset (0xe2),
	    ,	 3,	// -						 e2h.0-2
	SWFN,	 1,	// swap Fn and left Win key			 e2h.3
	LWIN,	 1,	// enable left Win key				 e2h.4
	    ,	 2,	// -						 e2h.5-6
	AIRK,	 1,	// enable airplane hotkey support		 e2h.7

	Offset (0xe4),
	    ,	 1,	// ?						 e4h.0
	    ,	 2,	// -						 e4h.1-2
	    ,	 1,	// ?						 e4h.3
	    ,	 1,	// -						 e4h.4
	EP12,	 1,	// ? (gpu related)				 e4h.5
	FN_G,    1,	// Fn + G pressed (GPU reset on vendor fw)	 e4h.6
	    ,    1,	// ?						 e4h.7

	Offset (0xe5),
	ECSZ,	 8,	// EC eFlash size				 e5h

	Offset (0xe6),
	    ,	 2,	// -						 e6h.0-1
	G3FG,	 1,	// Enter G3 (all power off) in S4/S5		 e6h.2
	    ,	 3,	// -						 e6h.3-5
	FOAC,	 1,	// Fan always on when AC connected		 e6h.6

	Offset (0xe7),
	FOFF,	 8,	// Fan base offset				 e7h

	Offset (0xe8),
	    ,    1,	// ? static 1; vendor: if 1: eccmd c6, val 0	 e8h.0
	CNVI,    1,	// CNVI card present				 e8h.1
	    ,    3,	// -						 e8h.2-4
	FN_D,    1,	// Fn + D pressed (CMOS reset on vendor fw)	 e8h.5
	    ,    1,	// ? fan related				 e8h.6

	Offset (0xe9),
	KBBO,	 1,	// KB LED supports boot effect override		 e9h.0

	Offset (0xea),
	    ,	 3,	// -						 eah.0-2
	PDFG,	 1,	// Power supplied via USB-C			 eah.3
	MSFG,	 1,	// Modern standby flag				 eah.4
	RCHG,	 1,	// ?						 eah.5
	ACOT,	 1,	// ?						 eah.6
	S5FG,	 1,	// ?						 eah.7

	Offset (0xeb),
	    ,    1,	// ? (unknown keypress status, NV4x only)	 ebh.0
	    ,    1,	// ? (unknown keypress status, NV4x only)	 ebh.1
	    ,    1,	// -						 ebh.2
	DGPT,    1,	// ?						 ebh.3
	TOPN,    1,	// ?						 ebh.4
	    ,    1,	// ? (kbc beep related?)			 ebh.5
	    ,    1,	// ? (kbc beep related?)			 ebh.6
	APRD,    1,	// AP ready					 ebh.7

	Offset (0xf0),
	PL2B,	16,	// Power Limit 2 set when on battery		 f0h-f1h
	PL2T,	16,	// Power Limit 2 (note: never enabled by EC!)	 f2h-f3h
	TAUT,	 8,	// Tau (for PL1, not effective)			 f4h

	/* FCMD interface */
	Offset (0xf8),
	FCMD,	 8,	// Command					 f8h
	FDAT,	 8,	// Data						 f9h
	FBUF,	 8,	// Buffer[0]					 fah
	FBF1,	 8,	// Buffer[1]					 fbh
	FBF2,	 8,	// Buffer[2]					 fch
	FBF3,	 8,	// Buffer[3]					 fdh

	Offset (0xff),
	    ,	 8,	// ? static, l14xcu/mu: 0xe0, nv4x 0x22		 ffh

	Offset (0x28a),
	FANQ,	 8,	// FAN count (FANC == FANQ)			28ah
	KBTP,	 8,	// Keyboard backlight type			28bh
			//   0x00 = none
			//   0x01 = white
			//   0x02 = RGB
			//   0x*3 = per-key RGB
			//   0x06 = RGB15Color
			//   0x16 = RGB15ColorCustom
}
