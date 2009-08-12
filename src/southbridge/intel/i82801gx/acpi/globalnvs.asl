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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/* Global Variables */

Name(\PICM, 0)		// IOAPIC/8259
Name(\DSEN, 1)		// Display Output Switching Enable

/* Global ACPI memory region. This region is used for passing information
 * between coreboot (aka "the system bios"), ACPI, and the SMI handler.
 * Since we don't know where this will end up in memory at ACPI compile time,
 * we have to fix it up in coreboot's ACPI creation phase.
 */


OperationRegion (GNVS, SystemMemory, 0xC0DEBABE, 0x100)
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
	DBGS,	 8,	// 0x11 - Debug State
	LINX,    8,	// 0x12 - Linux OS
	DCKN,	 8,	// 0x13 - PCIe docking state
	/* Thermal policy */
	Offset (0x14),
	ACTT,	 8,	// 0x14 - active trip point
	PSVT,	 8,	// 0x15 - passive trip point
	TC1V,	 8,	// 0x16 - passive trip point TC1
	TC2V,	 8,	// 0x17 - passive trip point TC2
	TSPV,	 8,	// 0x18 - passive trip point TSP
	CRTT,	 8,	// 0x19 - critical trip point
	DTSE,	 8,	// 0x1a - Digital Thermal Sensor enable
	DTS1,	 8,	// 0x1b - DT sensor 1
	DTS2,	 8,	// 0x1c - DT sensor 2
	/* Battery Support */
	Offset (0x1e),
	BNUM,	 8,	// 0x1e - number of batteries
	B0SC,	 8,	// 0x1f - BAT0 stored capacity
	B1SC,	 8,	// 0x20 - BAT1 stored capacity
	B2SC,	 8,	// 0x21 - BAT2 stored capacity
	B0SS,	 8,	// 0x22 - BAT0 stored status
	B1SS,	 8,	// 0x23 - BAT1 stored status
	B2SS,	 8,	// 0x24 - BAT2 stored status
	/* Processor Identification */
	Offset (0x28),
	APIC,	 8,	// 0x28 - APIC Enabled by coreboot
	MPEN,	 8,	// 0x29 - Multi Processor Enable
	PCP0,	 8,	// 0x2a - PDC CPU/CORE 0
	PCP1,	 8,	// 0x2b - PDC CPU/CORE 1
	PPCM,	 8,	// 0x2c - Max. PPC state
	/* Super I/O & CMOS config */
	Offset (0x32),
	NATP,	 8,	// 0x32 - ...
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
	IDMM,	 8,	// 0xc4 - IGD DVMT Mode
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
	/* Mainboard Specific (TODO move elsewhere) */
	Offset (0xf0),
	DOCK,	 8,	// 0xf0 - Docking Status
}
