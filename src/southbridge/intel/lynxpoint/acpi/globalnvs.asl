/* SPDX-License-Identifier: GPL-2.0-only */

/* Global Variables */

Name (\PICM, 0)		// IOAPIC/8259

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
	TLVL,	 8,	// 0x11 - Throttle Level Limit
	FLVL,	 8,	// 0x12 - Current FAN Level
	TCRT,	 8,	// 0x13 - Critical Threshold
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
	TMPS,	 8,	// 0x25 - Temperature Sensor ID
	/* Processor Identification */
	Offset (0x28),
	,	 8,	// 0x28 -  Enabled by coreboot
	,	 8,	// 0x29 - Multi Processor Enable
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
	,	 32,	// 0x38 - CBMEM TOC
	/* Integrated Graphics Device */
	Offset (0x3c),
	IGDS,	 8,	// 0x3c - IGD state (primary = 1)
	TLST,	 8,	// 0x3d - Display Toggle List pointer
	CADL,	 8,	// 0x3e - Currently Attached Devices List
	PADL,	 8,	// 0x3f - Previously Attached Devices List

	/* TPM support */
	Offset (0x5b),
	TPMP,	 8,	// 0x5b - TPM Present
	TPME,	 8,	// 0x5c - TPM Enable

	/* LynxPoint Serial IO device BARs */
	Offset (0x60),
	S0B0,	32,	// 0x60 - D21:F0 Serial IO SDMA BAR0
	S1B0,	32,	// 0x64 - D21:F1 Serial IO I2C0 BAR0
	S2B0,	32,	// 0x68 - D21:F2 Serial IO I2C1 BAR0
	S3B0,	32,	// 0x6c - D21:F3 Serial IO SPI0 BAR0
	S4B0,	32,	// 0x70 - D21:F4 Serial IO SPI1 BAR0
	S5B0,	32,	// 0x74 - D21:F5 Serial IO UAR0 BAR0
	S6B0,	32,	// 0x78 - D21:F6 Serial IO UAR1 BAR0
	S7B0,	32,	// 0x7c - D23:F0 Serial IO SDIO BAR0
	S0B1,	32,	// 0x80 - D21:F0 Serial IO SDMA BAR1
	S1B1,	32,	// 0x84 - D21:F1 Serial IO I2C0 BAR1
	S2B1,	32,	// 0x88 - D21:F2 Serial IO I2C1 BAR1
	S3B1,	32,	// 0x8c - D21:F3 Serial IO SPI0 BAR1
	S4B1,	32,	// 0x90 - D21:F4 Serial IO SPI1 BAR1
	S5B1,	32,	// 0x94 - D21:F5 Serial IO UAR0 BAR1
	S6B1,	32,	// 0x98 - D21:F6 Serial IO UAR1 BAR1
	S7B1,	32,	// 0x9c - D23:F0 Serial IO SDIO BAR1

	Offset (0xa0),
	CBMC, 32,	// 0xa0 - coreboot mem console pointer
}

/* Set flag to enable USB charging in S3 */
Method (S3UE)
{
	\S3U0 = 1
	\S3U1 = 1
}

/* Set flag to disable USB charging in S3 */
Method (S3UD)
{
	\S3U0 = 0
	\S3U1 = 0
}

/* Set flag to enable USB charging in S5 */
Method (S5UE)
{
	\S5U0 = 1
	\S5U1 = 1
}

/* Set flag to disable USB charging in S5 */
Method (S5UD)
{
	\S5U0 = 0
	\S5U1 = 0
}

/* Set flag to enable 3G module in S3 */
Method (S3GE)
{
	\S33G = 1
}

/* Set flag to disable 3G module in S3 */
Method (S3GD)
{
	\S33G = 0
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
	\F0OF = Arg0
	\F0ON = Arg1
	TZUP ()
}

/* Update Fan 1 thresholds */
Method (F1UT, 2)
{
	\F1OF = Arg0
	\F1ON = Arg1
	TZUP ()
}

/* Update Fan 2 thresholds */
Method (F2UT, 2)
{
	\F2OF = Arg0
	\F2ON = Arg1
	TZUP ()
}

/* Update Fan 3 thresholds */
Method (F3UT, 2)
{
	\F3OF = Arg0
	\F3ON = Arg1
	TZUP ()
}

/* Update Fan 4 thresholds */
Method (F4UT, 2)
{
	\F4OF = Arg0
	\F4ON = Arg1
	TZUP ()
}

/* Update Temperature Sensor ID */
Method (TMPU, 1)
{
	\TMPS = Arg0
	TZUP ()
}
