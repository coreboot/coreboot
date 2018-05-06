/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2012 The Chromium OS Authors
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

/* Global Variables */

Name(\PICM, 0)		// IOAPIC/8259
Name(\DSEN, 1)		// Display Output Switching Enable

/* Global ACPI memory region. This region is used for passing information
 * between coreboot (aka "the system bios"), ACPI, and the SMI handler.
 * Since we don't know where this will end up in memory at ACPI compile time,
 * we have to fix it up in coreboot's ACPI creation phase.
 */

External(NVSA)
OperationRegion (GNVS, SystemMemory, NVSA, 0xf00)
Field (GNVS, ByteAcc, NoLock, Preserve)
{
	/* Miscellaneous */
	Offset (0x00),
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
	PCNT,	 8,	// 0x2d - Processor count
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
	CSTE,	16,	// 0x40 - Current display state
	NSTE,	16,	// 0x42 - Next display state
	SSTE,	16,	// 0x44 - Set display state
	Offset (0x46),
	NDID,	 8,	// 0x46 - Number of Device IDs
	DID1,	32,	// 0x47 - Device ID 1
	DID2,	32,	// 0x4b - Device ID 2
	DID3,	32,	// 0x4f - Device ID 3
	DID4,	32,	// 0x53 - Device ID 4
	DID5,	32,	// 0x57 - Device ID 5

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

	/* IGD OpRegion */
	Offset (0xb4),
	ASLB,	32,	// 0xb4 - IGD OpRegion Base Address
	IBTT,	 8,	// 0xb8 - IGD boot panel device
	IPAT,	 8,	// 0xb9 - IGD panel type cmos option
	ITVF,	 8,	// 0xba - IGD TV format cmos option
	ITVM,	 8,	// 0xbb - IGD TV minor format option
	IPSC,	 8,	// 0xbc - IGD panel scaling
	IBLC,	 8,	// 0xbd - IGD BLC config
	IBIA,	 8,	// 0xbe - IGD BIA config
	ISSC,	 8,	// 0xbf - IGD SSC config
	I409,	 8,	// 0xc0 - IGD 0409 modified settings
	I509,	 8,	// 0xc1 - IGD 0509 modified settings
	I609,	 8,	// 0xc2 - IGD 0609 modified settings
	I709,	 8,	// 0xc3 - IGD 0709 modified settings
	IDMM,	 8,	// 0xc4 - IGD Power conservation feature
	IDMS,	 8,	// 0xc5 - IGD DVMT memory size
	IF1E,	 8,	// 0xc6 - IGD function 1 enable
	HVCO,	 8,	// 0xc7 - IGD HPLL VCO
	NXD1,	32,	// 0xc8 - IGD _DGS next DID1
	NXD2,	32,	// 0xcc - IGD _DGS next DID2
	NXD3,	32,	// 0xd0 - IGD _DGS next DID3
	NXD4,	32,	// 0xd4 - IGD _DGS next DID4
	NXD5,	32,	// 0xd8 - IGD _DGS next DID5
	NXD6,	32,	// 0xdc - IGD _DGS next DID6
	NXD7,	32,	// 0xe0 - IGD _DGS next DID7
	NXD8,	32,	// 0xe4 - IGD _DGS next DID8

	ISCI,	 8,	// 0xe8 - IGD SMI/SCI mode (0: SCI)
	PAVP,	 8,	// 0xe9 - IGD PAVP data
	Offset (0xeb),
	OSCC,	 8,	// 0xeb - PCIe OSC control
	NPCE,	 8,	// 0xec - native pcie support
	PLFL,	 8,	// 0xed - platform flavor
	BREV,	 8,	// 0xee - board revision
	DPBM,	 8,	// 0xef - digital port b mode
	DPCM,	 8,	// 0xf0 - digital port c mode
	DPDM,	 8,	// 0xf1 - digital port d mode
	ALFP,	 8,	// 0xf2 - active lfp
	IMON,	 8,	// 0xf3 - current graphics turbo imon value
	MMIO,	 8,	// 0xf4 - 64bit mmio support

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
