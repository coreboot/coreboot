/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

/* Global ACPI memory region. This region is used for passing information
 * between coreboot (aka "the system bios"), ACPI, and the SMI handler.
 * Since we don't know where this will end up in memory at ACPI compile time,
 * we have to fix it up in coreboot's ACPI creation phase.
 */


OperationRegion (GNVS, SystemMemory, 0xC0DEBABE, 0xf00)
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
	PCNT,	 8,	// 0x11 - Processor count
	TPMP,	 8,	// 0x12 - TPM Present and Enabled
	TLVL,	 8,	// 0x13 - Throttle Level
	PPCM,	 8,	// 0x14 - Maximum P-state usable by OS

	/* Device Config */
	Offset (0x20),
	S5U0,	 8,	// 0x20 - Enable USB0 in S5
	S5U1,	 8,	// 0x21 - Enable USB1 in S5
	S3U0,	 8,	// 0x22 - Enable USB0 in S3
	S3U1,	 8,	// 0x23 - Enable USB1 in S3
	TACT,	 8,	// 0x24 - Thermal Active trip point
	TPSV,	 8,	// 0x25 - Thermal Passive trip point
	TCRT,	 8,	// 0x26 - Thermal Critical trip point
	DPTE,	 8,	// 0x27 - Enable DPTF

	/* Base addresses */
	Offset (0x30),
	CMEM,	 32,	// 0x30 - CBMEM TOC
	TOLM,	 32,	// 0x34 - Top of Low Memory
	CBMC,	 32,	// 0x38 - coreboot mem console pointer

	/* Serial IO device BARs */
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
