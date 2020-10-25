/* SPDX-License-Identifier: GPL-2.0-only */

/* Global Variables */

Name (\PICM, 0)		// IOAPIC/8259

/*
 * Global ACPI memory region. This region is used for passing information
 * between coreboot (aka "the system bios"), ACPI, and the SMI handler.
 * Since we don't know where this will end up in memory at ACPI compile time,
 * we have to fix it up in coreboot's ACPI creation phase.
 */

External(NVSA)
OperationRegion (GNVS, SystemMemory, NVSA, 0x2000)
Field (GNVS, ByteAcc, NoLock, Preserve)
{
	/* Miscellaneous */
	Offset (0x00),
	OSYS,	16,	// 0x00 - Operating System
	SMIF,	8,	// 0x02 - SMI function
	PRM0,	8,	// 0x03 - SMI function parameter
	PRM1,	8,	// 0x04 - SMI function parameter
	SCIF,	8,	// 0x05 - SCI function
	PRM2,	8,	// 0x06 - SCI function parameter
	PRM3,	8,	// 0x07 - SCI function parameter
	LCKF,	8,	// 0x08 - Global Lock function for EC
	PRM4,	8,	// 0x09 - Lock function parameter
	PRM5,	8,	// 0x0a - Lock function parameter
	PCNT,	8,	// 0x0b - Processor Count
	PPCM,	8,	// 0x0c - Max PPC State
	TMPS,	8,	// 0x0d - Temperature Sensor ID
	TLVL,	8,	// 0x0e - Throttle Level Limit
	FLVL,	8,	// 0x0f - Current FAN Level
	TCRT,	8,	// 0x10 - Critical Threshold
	TPSV,	8,	// 0x11 - Passive Threshold
	TMAX,	8,	// 0x12 - CPU Tj_max
	S5U0,	8,	// 0x13 - Enable USB in S5
	S3U0,	8,	// 0x14 - Enable USB in S3
	S33G,	8,	// 0x15 - Enable 3G in S3
	LIDS,	8,	// 0x16 - LID State
	PWRS,	8,	// 0x17 - AC Power State
	CMEM,	32,	// 0x18 - 0x1b - CBMEM TOC
	CBMC,	32,	// 0x1c - 0x1f - coreboot Memory Console
	PM1I,	64,	// 0x20 - 0x27 - PM1 wake status bit
	GPEI,	64,	// 0x28 - 0x2f - GPE wake status bit

	/* ChromeOS specific */
	Offset (0x100),
	#include <vendorcode/google/chromeos/acpi/gnvs.asl>

	Offset (0x1000),
	/* Device enables in ACPI mode */
	S0EN,	8,	// DMA Enable
	S1EN,	8,	// I2C0 Enable
	S2EN,	8,	// I2C1 Enable
	S3EN,	8,	// SPI0 Enable
	S4EN,	8,	// SPI1 Enable
	S5EN,	8,	// UART0 Enable
	S6EN,	8,	// UART1 Enable
	S7EN,	8,	// SDIO Enable
	S8EN,	8,	// ADSP Enable

	/* BAR 0 */
	S0B0,	32,	// DMA BAR0
	S1B0,	32,	// I2C0 BAR0
	S2B0,	32,	// I2C1 BAR0
	S3B0,	32,	// SPI0 BAR0
	S4B0,	32,	// SPI1 BAR0
	S5B0,	32,	// UART0 BAR0
	S6B0,	32,	// UART1 BAR0
	S7B0,	32,	// SDIO BAR0
	S8B0,	32,	// ADSP BAR0

	/* BAR 1 */
	S0B1,	32,	// DMA BAR1
	S1B1,	32,	// I2C0 BAR1
	S2B1,	32,	// I2C1 BAR1
	S3B1,	32,	// SPI0 BAR1
	S4B1,	32,	// SPI1 BAR1
	S5B1,	32,	// UART0 BAR1
	S6B1,	32,	// UART1 BAR1
	S7B1,	32,	// SDIO BAR1
	S8B1,	32,	// ADSP BAR1
}

/* Set flag to enable USB charging in S3 */
Method (S3UE)
{
	Store (One, \S3U0)
}

/* Set flag to disable USB charging in S3 */
Method (S3UD)
{
	Store (Zero, \S3U0)
}

/* Set flag to enable USB charging in S5 */
Method (S5UE)
{
	Store (One, \S5U0)
}

/* Set flag to disable USB charging in S5 */
Method (S5UD)
{
	Store (Zero, \S5U0)
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
