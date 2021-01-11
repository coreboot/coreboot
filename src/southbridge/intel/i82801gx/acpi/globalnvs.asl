/* SPDX-License-Identifier: GPL-2.0-only */

/* Global Variables */

Name(\PICM, 0)		// IOAPIC/8259

/* Global ACPI memory region. This region is used for passing information
 * between coreboot (aka "the system bios"), ACPI, and the SMI handler.
 * Since we don't know where this will end up in memory at ACPI compile time,
 * we have to fix it up in coreboot's ACPI creation phase.
 */

External(NVSA)
OperationRegion (GNVS, SystemMemory, NVSA, 0x100)
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
	DBGS,	 8,	// 0x11 - Debug State
	LINX,    8,	// 0x12 - Linux OS
	DCKN,	 8,	// 0x13 - PCIe docking state
	/* Thermal policy */
	ACTT,	 8,	// 0x14 - active trip point
	TPSV,	 8,	// 0x15 - passive trip point
	TC1V,	 8,	// 0x16 - passive trip point TC1
	TC2V,	 8,	// 0x17 - passive trip point TC2
	TSPV,	 8,	// 0x18 - passive trip point TSP
	TCRT,	 8,	// 0x19 - critical trip point
	DTSE,	 8,	// 0x1a - Digital Thermal Sensor enable
	DTS1,	 8,	// 0x1b - DT sensor 1
	FLVL,	 8,	// 0x1c - current fan level
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
	NATP,	 8,	// 0x32 -
	CMAP,	 8,	// 0x33 -
	CMBP,	 8,	// 0x34 -
	LPTP,	 8,	// 0x35 - LPT Port
	FDCP,	 8,	// 0x36 - Floppy Disk Controller
	RFDV,	 8,	// 0x37 -
	HOTK,	 8,	// 0x38 -
	RTCF,	 8,	// 0x39 -
	UTIL,	 8,	// 0x3a -
	ACIN,	 8,	// 0x3b -
	/* Integrated Graphics Device */
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
	/* Mainboard Specific (TODO move elsewhere) */
	Offset (0xf0),
	DOCK,	 8,	// 0xf0 - Docking Status
	BTEN,	 8,	// 0xf1 - Bluetooth Enable
	CBMC,	32,
}
