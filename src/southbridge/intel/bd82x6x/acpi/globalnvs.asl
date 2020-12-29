/* SPDX-License-Identifier: GPL-2.0-only */

/* Global Variables */

Name(\PICM, 0)		// IOAPIC/8259

/* Global ACPI memory region. This region is used for passing information
 * between coreboot (aka "the system bios"), ACPI, and the SMI handler.
 * Since we don't know where this will end up in memory at ACPI compile time,
 * we have to fix it up in coreboot's ACPI creation phase.
 */

External(NVSA)
OperationRegion (GNVS, SystemMemory, NVSA, 0x1000)
Field (GNVS, ByteAcc, NoLock, Preserve)
{
	/* Miscellaneous */
	OSYS,	16,	// 0x00 - Operating System
	SMIF,	 8,	// 0x02 - SMI function
	PRM0,	 8,	// 0x03 - SMI function parameter
	PRM1,	 8,	// 0x04 - SMI function parameter
	SCIF,	 8,	// 0x05 - SCI function
	PRM2,	 8,	// 0x06 - SCI function parameter
	PRM3,	 8,	// 0x07 - SCI function parameter
	LCKF,	 8,	// 0x08 - Global Lock function for EC
	PRM4,	 8,	// 0x09 - Lock function parameter
	PRM5,	 8,	// 0x0a - Lock function parameter
	P80D,	32,	// 0x0b - Debug port (IO 0x80) value
	LIDS,	 8,	// 0x0f - LID state (open = 1)
	PWRS,	 8,	// 0x10 - Power State (AC = 1)
	/* Thermal policy */
	Offset (0x11),
	TLVL,    8,	// 0x11 - Throttle Level Limit
	FLVL,	 8,	// 0x12 - Current FAN Level
	TCRT,    8,	// 0x13 - Critical Threshold
	TPSV,	 8,	// 0x14 - Passive Threshold
	TMAX,	 8,	// 0x15 - CPU Tj_max
	F0OF,	 8,	// 0x16 - FAN 0 OFF Threshold
	F0ON,	 8,	// 0x17 - FAN 0 ON Threshold
	F0PW,	 8,	// 0x18 - FAN 0 PWM value
	F1OF,	 8,	// 0x19 - FAN 1 OFF Threshold
	F1ON,	 8,	// 0x1a - FAN 1 ON Threshold
	F1PW,	 8,	// 0x1b - FAN 1 PWM value
	F2OF,	 8,	// 0x1c - FAN 2 OFF Threshold
	F2ON,	 8,	// 0x1d - FAN 2 ON Threshold
	F2PW,	 8,	// 0x1e - FAN 2 PWM value
	F3OF,	 8,	// 0x1f - FAN 3 OFF Threshold
	F3ON,	 8,	// 0x20 - FAN 3 ON Threshold
	F3PW,	 8,	// 0x21 - FAN 3 PWM value
	F4OF,	 8,	// 0x22 - FAN 4 OFF Threshold
	F4ON,	 8,	// 0x23 - FAN 4 ON Threshold
	F4PW,	 8,	// 0x24 - FAN 4 PWM value
	TMPS,    8,	// 0x25 - Temperature Sensor ID
	/* Processor Identification */
	Offset (0x28),
	APIC,	 8,	// 0x28 - APIC Enabled by coreboot
	MPEN,	 8,	// 0x29 - Multi Processor Enable
	PCP0,	 8,	// 0x2a - PDC CPU/CORE 0
	PCP1,	 8,	// 0x2b - PDC CPU/CORE 1
	PPCM,	 8,	// 0x2c - Max. PPC state
	,	 8,	// 0x2d - Processor count
	/* Super I/O & CMOS config */
	Offset (0x32),
	NATP,	 8,	// 0x32 -
	S5U0,	 8,	// 0x33 - Enable USB0 in S5
	S5U1,	 8,	// 0x34 - Enable USB1 in S5
	S3U0,	 8,	// 0x35 - Enable USB0 in S3
	S3U1,	 8,	// 0x36 - Enable USB1 in S3
	S33G,	 8,	// 0x37 - Enable 3G in S3
	CMEM,	 32,	// 0x38 - CBMEM TOC
	/* Integrated Graphics Device */
	Offset (0x3c),
	IGDS,	 8,	// 0x3c - IGD state (primary = 1)
	TLST,	 8,	// 0x3d - Display Toggle List pointer
	CADL,	 8,	// 0x3e - Currently Attached Devices List
	PADL,	 8,	// 0x3f - Previously Attached Devices List
	/* Backlight Control */
	Offset (0x64),
	BLCS,	 8,	// 0x64 - Backlight control possible?
	BRTL,	 8,	// 0x65 - Brightness Level
	ODDS,	 8,	// 0x66
	/* Ambient Light Sensors */
	Offset (0x6e),
	ALSE,	 8,	// 0x6e - ALS enable
	ALAF,	 8,	// 0x6f - Ambient light adjustment factor
	LLOW,	 8,	// 0x70 - LUX Low
	LHIH,	 8,	// 0x71 - LUX High
	/* EMA */
	Offset (0x78),
	EMAE,	 8,	// 0x78 - EMA enable
	EMAP,	16,	// 0x79 - EMA pointer
	EMAL,	16,	// 0x7b - EMA length
	/* MEF */
	Offset (0x82),
	MEFE,	 8,	// 0x82 - MEF enable
	/* TPM support */
	Offset (0x8c),
	TPMP,	 8,	// 0x8c - TPM
	TPME,	 8,	// 0x8d - TPM enable
	/* SATA */
	Offset (0x96),
	GTF0,	56,	// 0x96 - GTF task file buffer for port 0
	GTF1,	56,	// 0x9d - GTF task file buffer for port 1
	GTF2,	56,	// 0xa4 - GTF task file buffer for port 2
	IDEM,	 8,	// 0xab - IDE mode (compatible / enhanced)
	IDET,	 8,	// 0xac - IDE
	/* XHCI */
	Offset (0xb2),
	XHCI,	 8,

	Offset (0xf5),
	TPIQ,	 8,	// 0xf5 - trackpad IRQ value
	CBMC,	32,

	/* ChromeOS specific */
	Offset (0x100),
	#include <vendorcode/google/chromeos/acpi/gnvs.asl>
}

/* Set flag to enable USB charging in S3 */
Method (S3UE)
{
	Store (One, \S3U0)
	Store (One, \S3U1)
}

/* Set flag to disable USB charging in S3 */
Method (S3UD)
{
	Store (Zero, \S3U0)
	Store (Zero, \S3U1)
}

/* Set flag to enable USB charging in S5 */
Method (S5UE)
{
	Store (One, \S5U0)
	Store (One, \S5U1)
}

/* Set flag to disable USB charging in S5 */
Method (S5UD)
{
	Store (Zero, \S5U0)
	Store (Zero, \S5U1)
}

/* Set flag to enable 3G module in S3 */
Method (S3GE)
{
	Store (One, \S33G)
}

/* Set flag to disable 3G module in S3 */
Method (S3GD)
{
	Store (Zero, \S33G)
}

/* Set XHCI Mode enable */
Method (XHCE)
{
	Store (One, \XHCI)
}

/* Set XHCI Mode disable */
Method (XHCD)
{
	Store (Zero, \XHCI)
}
External (\_TZ.SKIN)

Method (TZUP)
{
#ifdef HAVE_THERMALZONE
	/* Update Primary Thermal Zone */
	If (CondRefOf (\_TZ.THRM)) {
		Notify (\_TZ.THRM, 0x81)
	}
#endif

	/* Update Secondary Thermal Zone */
	If (CondRefOf (\_TZ.SKIN)) {
		Notify (\_TZ.SKIN, 0x81)
	}
}

/* Update Fan 0 thresholds */
Method (F0UT, 2)
{
	Store (Arg0, \F0OF)
	Store (Arg1, \F0ON)
	TZUP ()
}

/* Update Fan 1 thresholds */
Method (F1UT, 2)
{
	Store (Arg0, \F1OF)
	Store (Arg1, \F1ON)
	TZUP ()
}

/* Update Fan 2 thresholds */
Method (F2UT, 2)
{
	Store (Arg0, \F2OF)
	Store (Arg1, \F2ON)
	TZUP ()
}

/* Update Fan 3 thresholds */
Method (F3UT, 2)
{
	Store (Arg0, \F3OF)
	Store (Arg1, \F3ON)
	TZUP ()
}

/* Update Fan 4 thresholds */
Method (F4UT, 2)
{
	Store (Arg0, \F4OF)
	Store (Arg1, \F4ON)
	TZUP ()
}

/* Update Temperature Sensor ID */
Method (TMPU, 1)
{
	Store (Arg0, \TMPS)
	TZUP ()
}
