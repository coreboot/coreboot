/* SPDX-License-Identifier: GPL-2.0-only */

#define DPTF_TSR0_SENSOR_ID	0
#define DPTF_TSR0_SENSOR_NAME	"TMP432_CPU"

#define DPTF_TSR1_SENSOR_ID	1
#define DPTF_TSR1_SENSOR_NAME	"TMP432_WLAN"

#define DPTF_TSR2_SENSOR_ID	2
#define DPTF_TSR2_SENSOR_NAME	"TMP432_CHARGER"

#define DPTF_TERRA2_TSR0_PASSIVE	60
#define DPTF_TERRA2_TSR0_CRITICAL	75

#define DPTF_TERRA2_TSR1_PASSIVE	53
#define DPTF_TERRA2_TSR1_CRITICAL	75

#define DPTF_TERRA2_TSR2_PASSIVE	53
#define DPTF_TERRA2_TSR2_CRITICAL	75

#define DPTF_TERRA3_TSR0_PASSIVE	50
#define DPTF_TERRA3_TSR0_CRITICAL	75

#define DPTF_TERRA3_TSR1_PASSIVE	52
#define DPTF_TERRA3_TSR1_CRITICAL	75

#define DPTF_TERRA3_TSR2_PASSIVE	53
#define DPTF_TERRA3_TSR2_CRITICAL	75

#define DPTF_ENABLE_CHARGER

/* Terra2 - Charger performance states, board-specific values from charger and EC */
Name (CPT2, Package () {
	Package () { 0, 0, 0, 0, 255, 0x6a4, "mA", 0 },	/* 1.7A (MAX) */
	Package () { 0, 0, 0, 0, 24, 0x320, "mA", 0 },	/* 0.8A */
	Package () { 0, 0, 0, 0, 16, 0x258, "mA", 0 },	/* 0.6A */
	Package () { 0, 0, 0, 0, 8, 0x190, "mA", 0 },	/* 0.4A */
	Package () { 0, 0, 0, 0, 0, 0x64, "mA", 0 },	/* 0.1A */
})

/* Terra3 - Charger performance states, board-specific values from charger and EC */
Name (CPT3, Package () {
	Package () { 0, 0, 0, 0, 255, 0x6a4, "mA", 0 },	/* 1.7A (MAX) */
	Package () { 0, 0, 0, 0, 24, 0x320, "mA", 0 },	/* 0.8A */
	Package () { 0, 0, 0, 0, 16, 0x258, "mA", 0 },	/* 0.6A */
	Package () { 0, 0, 0, 0, 8, 0x190, "mA", 0 },	/* 0.4A */
	Package () { 0, 0, 0, 0, 0, 0x64, "mA", 0 },	/* 0.1A */
})

/* Mainboard specific _PDL is 1GHz */
Name (MPDL, 8)

/* Terra2 - Thermal Relationship Table for method _TRT */
Name (TRT2, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.B0DB, \_SB.PCI0.B0DB, 100, 10, 0, 0, 0, 0 },
#ifdef DPTF_ENABLE_CHARGER
	/* Charger Effect on Temp Sensor 0 */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR0, 200, 100, 0, 0, 0, 0 },
#endif
	/* CPU Effect on Temp Sensor 0 */
	Package () { \_SB.PCI0.B0DB, \_SB.DPTF.TSR0, 100, 100, 0, 0, 0, 0 },
#ifdef DPTF_ENABLE_CHARGER
	/* Charger Effect on Temp Sensor 1 */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR1, 100, 100, 0, 0, 0, 0 },
#endif
	/* CPU Effect on Temp Sensor 1 */
	Package () { \_SB.PCI0.B0DB, \_SB.DPTF.TSR1, 200, 100, 0, 0, 0, 0 },
#ifdef DPTF_ENABLE_CHARGER
	/* Charger Effect on Temp Sensor 2 */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR2, 100, 100, 0, 0, 0, 0 },
#endif
	/* CPU Effect on Temp Sensor 2 */
	Package () { \_SB.PCI0.B0DB, \_SB.DPTF.TSR2, 200, 100, 0, 0, 0, 0 },
})

/* Terra3 - Thermal Relationship Table for method _TRT */
Name (TRT3, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.B0DB, \_SB.PCI0.B0DB, 100, 10, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 0 */
	Package () { \_SB.PCI0.B0DB, \_SB.DPTF.TSR0, 100, 100, 0, 0, 0, 0 },
#ifdef DPTF_ENABLE_CHARGER
	/* Charger Effect on Temp Sensor 1 */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR1, 100, 100, 0, 0, 0, 0 },
#endif

	/* CPU Effect on Temp Sensor 1 */
	Package () { \_SB.PCI0.B0DB, \_SB.DPTF.TSR1, 200, 100, 0, 0, 0, 0 },

	/* Charger Effect on Temp Sensor 2 */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR2, 100, 100, 0, 0, 0, 0 },
})

/* Terra2 - PPCC table defined by mainboard for method PPCC */
Name (PPT2, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		1600,	/* PowerLimitMinimum */
		10000,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
		200	/* StepSize */
	},
	Package () {	/* Power Limit 2 */
		1,	/* PowerLimitIndex, 1 for Power Limit 2 */
		8000,	/* PowerLimitMinimum */
		8000,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
		1000	/* StepSize */
	}
})

/* Terra3 - PPCC table defined by mainboard for method PPCC */
Name (PPT3, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		1600,	/* PowerLimitMinimum */
		10000,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
		200	/* StepSize */
	},
	Package () {	/* Power Limit 2 */
		1,	/* PowerLimitIndex, 1 for Power Limit 2 */
		8000,	/* PowerLimitMinimum */
		8000,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
		1000	/* StepSize */
	}
})

Device (DPTF)
{
	Name (_HID, EISAID ("INT3400"))
	Name (_UID, 0)

	Name (IDSP, Package()
	{
		/* DPPM Passive Policy 1.0 */
		ToUUID ("42A441D6-AE6A-462B-A84B-4A8CE79027D3"),

		/* DPPM Critical Policy */
		ToUUID ("97C68AE7-15FA-499c-B8C9-5DA81D606E0A"),

		/* DPPM Cooling Policy */
		ToUUID ("16CAF1B7-DD38-40ED-B1C1-1B8A1913D531"),
	})

	Method (_STA)
	{
		If (\DPTE == 1) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}

	/*
	 * Arg0: Buffer containing UUID
	 * Arg1: Integer containing Revision ID of buffer format
	 * Arg2: Integer containing count of entries in Arg3
	 * Arg3: Buffer containing list of DWORD capabilities
	 * Return: Buffer containing list of DWORD capabilities
	 */
	Method (_OSC, 4, Serialized)
	{
		/* Check for Passive Policy UUID */
		If (DeRefOf (IDSP[0]) == Arg0) {
			/* Initialize Thermal Devices */
			^TINI ()

#ifdef DPTF_ENABLE_CHARGER
			/* Initialize Charger Device */
			^TCHG.INIT ()
#endif
		}

		Return (Arg3)
	}

	/* Priority based _TRT */
	Name (TRTR, 1)

	/* Return TRT table defined by Terra2 or Terra3 mainboard */
	Method (_TRT)
	{
		If (\_SB.GPID == TERRA2_PROJECT_ID)
		{
			Return (\_SB.TRT2)
		} Else {
			Return (\_SB.TRT3)
		}
	}

	/* Convert from Degrees C to 1/10 Kelvin for ACPI */
	Method (CTOK, 1) {
		/* 10th of Degrees C */
		Local0 = Arg0 * 10

		/* Convert to Kelvin */
		Local0 += 2732

		Return (Local0)
	}

	/* Include Thermal Participants */
	#include "thermal.asl"

#ifdef DPTF_ENABLE_CHARGER
	/* Include Charger Participant */
	#include "charger.asl"
#endif
}
