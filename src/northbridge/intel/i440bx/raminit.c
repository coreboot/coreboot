/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <spd.h>
#include <sdram_mode.h>
#include <delay.h>
#include "i440bx.h"

/*-----------------------------------------------------------------------------
Macros and definitions.
-----------------------------------------------------------------------------*/

/* Uncomment this to enable debugging output. */
#define DEBUG_RAM_SETUP 1

/* Debugging macros. */
#if defined(DEBUG_RAM_SETUP)
#define PRINT_DEBUG(x)		print_debug(x)
#define PRINT_DEBUG_HEX8(x)	print_debug_hex8(x)
#define PRINT_DEBUG_HEX16(x)	print_debug_hex16(x)
#define PRINT_DEBUG_HEX32(x)	print_debug_hex32(x)
#define DUMPNORTH()		dump_pci_device(PCI_DEV(0, 0, 0))
#else
#define PRINT_DEBUG(x)
#define PRINT_DEBUG_HEX8(x)
#define PRINT_DEBUG_HEX16(x)
#define PRINT_DEBUG_HEX32(x)
#define DUMPNORTH()
#endif

/* SDRAMC[7:5] - SDRAM Mode Select (SMS). */
#define RAM_COMMAND_NORMAL	0x0
#define RAM_COMMAND_NOP		0x1
#define RAM_COMMAND_PRECHARGE	0x2
#define RAM_COMMAND_MRS		0x3
#define RAM_COMMAND_CBR		0x4

/* Map the JEDEC SPD refresh rates (array index) to 440BX refresh rates as
 * defined in DRAMC[2:0].
 *
 * [0] == Normal        15.625 us ->  15.6 us
 * [1] == Reduced(.25X)    3.9 us ->   7.8 ns
 * [2] == Reduced(.5X)     7.8 us ->   7.8 us
 * [3] == Extended(2x)    31.3 us ->  31.2 us
 * [4] == Extended(4x)    62.5 us ->  62.4 us
 * [5] == Extended(8x)     125 us -> 124.8 us
 */
static const uint32_t refresh_rate_map[] = {
	1, 5, 5, 2, 3, 4
};

/* Table format: register, bitmask, value. */
static const long register_values[] = {
	/* NBXCFG - NBX Configuration Register
	 * 0x50 - 0x53
	 *
	 * [31:24] SDRAM Row Without ECC
	 *         0 = ECC components are populated in this row
	 *         1 = ECC components are not populated in this row
	 * [23:19] Reserved
	 * [18:18] Host Bus Fast Data Ready Enable (HBFDRE)
	 *         Assertion of DRAM data on host bus occurs...
	 *         0 = ...one clock after sampling snoop results (default)
	 *         1 = ...on the same clock the snoop result is being sampled
	 *             (this mode is faster by one clock cycle)
	 * [17:17] ECC - EDO static Drive mode
	 *         0 = Normal mode (default)
	 *         1 = ECC signals are always driven
	 * [16:16] IDSEL_REDIRECT
	 *         0 = IDSEL1 is allocated to this bridge (default)
	 *         1 = IDSEL7 is allocated to this bridge
	 * [15:15] WSC# Handshake Disable
	 *         1 = Uni-processor mode
	 *         0 = Dual-processor mode with external IOAPIC (default)
	 * [14:14] Intel Reserved
	 * [13:12] Host/DRAM Frequency
	 *         00 = 100 MHz
	 *         01 = Reserved
	 *         10 = 66 MHz
	 *         11 = Reserved
	 * [11:11] AGP to PCI Access Enable
	 *         1 = Enable
	 *         0 = Disable
	 * [10:10] PCI Agent to Aperture Access Disable
	 *         1 = Disable
	 *         0 = Enable (default)
	 * [09:09] Aperture Access Global Enable
	 *         1 = Enable
	 *         0 = Disable
	 * [08:07] DRAM Data Integrity Mode (DDIM)
	 *         00 = Non-ECC
	 *         01 = EC-only
	 *         10 = ECC Mode
	 *         11 = ECC Mode with hardware scrubbing enabled
	 * [06:06] ECC Diagnostic Mode Enable (EDME)
	 *         1 = Enable
	 *         0 = Normal operation mode (default)
	 * [05:05] MDA Present (MDAP)
	 *         Works in conjunction with the VGA_EN bit.
	 *         VGA_EN MDAP
	 *           0     x   All VGA cycles are sent to PCI
	 *           1     0   All VGA cycles are sent to AGP
	 *           1     1   All VGA cycles are sent to AGP, except for
	 *                     cycles in the MDA range.
	 * [04:04] Reserved
	 * [03:03] USWC Write Post During I/O Bridge Access Enable (UWPIO)
	 *         1 = Enable
	 *         0 = Disable
	 * [02:02] In-Order Queue Depth (IOQD)
	 *         1 = In-order queue = maximum
	 *         0 = A7# is sampled asserted (i.e., 0)
	 * [01:00] Reserved
	 */
	// TODO
	NBXCFG + 0, 0x00, 0x0c,
	// NBXCFG + 1, 0x00, 0xa0,
	NBXCFG + 1, 0x00, 0x80,
	NBXCFG + 2, 0x00, 0x00,
	NBXCFG + 3, 0x00, 0xff,

	/* DRAMC - DRAM Control Register
	 * 0x57
	 *
	 * [7:6] Reserved
	 * [5:5] Module Mode Configuration (MMCONFIG)
	 *       TODO
	 * [4:3] DRAM Type (DT)
	 *       00 = EDO
	 *       01 = SDRAM
	 *       10 = Registered SDRAM
	 *       11 = Reserved
	 *       Note: EDO, SDRAM and Registered SDRAM cannot be mixed.
	 * [2:0] DRAM Refresh Rate (DRR)
	 *       000 = Refresh disabled
	 *       001 = 15.6 us
	 *       010 = 31.2 us
	 *       011 = 62.4 us
	 *       100 = 124.8 us
	 *       101 = 249.6 us
	 *       110 = Reserved
	 *       111 = Reserved
	 */
	/* Choose SDRAM (not registered), and disable refresh for now. */
	DRAMC, 0x00, 0x08,

	/*
	 * PAM[6:0] - Programmable Attribute Map Registers
	 * 0x59 - 0x5f
	 *
	 * 0x59 [3:0] Reserved
	 * 0x59 [5:4] 0xF0000 - 0xFFFFF BIOS area
	 * 0x5a [1:0] 0xC0000 - 0xC3FFF ISA add-on BIOS
	 * 0x5a [5:4] 0xC4000 - 0xC7FFF ISA add-on BIOS
	 * 0x5b [1:0] 0xC8000 - 0xCBFFF ISA add-on BIOS
	 * 0x5b [5:4] 0xCC000 - 0xCFFFF ISA add-on BIOS
	 * 0x5c [1:0] 0xD0000 - 0xD3FFF ISA add-on BIOS
	 * 0x5c [5:4] 0xD4000 - 0xD7FFF ISA add-on BIOS
	 * 0x5d [1:0] 0xD8000 - 0xDBFFF ISA add-on BIOS
	 * 0x5d [5:4] 0xDC000 - 0xDFFFF ISA add-on BIOS
	 * 0x5e [1:0] 0xE0000 - 0xE3FFF BIOS entension
	 * 0x5e [5:4] 0xE4000 - 0xE7FFF BIOS entension
	 * 0x5f [1:0] 0xE8000 - 0xEBFFF BIOS entension
	 * 0x5f [5:4] 0xEC000 - 0xEFFFF BIOS entension
	 *
	 * Bit assignment:
	 * 00 = DRAM Disabled (all access goes to memory mapped I/O space)
	 * 01 = Read Only (Reads to DRAM, writes to memory mapped I/O space)
	 * 10 = Write Only (Writes to DRAM, reads to memory mapped I/O space)
	 * 11 = Read/Write (all access goes to DRAM)
	 */
	// TODO
	PAM0, 0x00, 0x00,
	PAM1, 0x00, 0x00,
	PAM2, 0x00, 0x00,
	PAM3, 0x00, 0x00,
	PAM4, 0x00, 0x00,
	PAM5, 0x00, 0x00,
	PAM6, 0x00, 0x00,

	/* DRB[0:7] - DRAM Row Boundary Registers
	 * 0x60 - 0x67
	 *
	 * An array of 8 byte registers, which hold the ending memory address
	 * assigned to each pair of DIMMs, in 8MB granularity.   
	 *
	 * 0x60 DRB0 = Total memory in row0 (in 8 MB)
	 * 0x61 DRB1 = Total memory in row0+1 (in 8 MB)
	 * 0x62 DRB2 = Total memory in row0+1+2 (in 8 MB)
	 * 0x63 DRB3 = Total memory in row0+1+2+3 (in 8 MB)
	 * 0x64 DRB4 = Total memory in row0+1+2+3+4 (in 8 MB)
	 * 0x65 DRB5 = Total memory in row0+1+2+3+4+5 (in 8 MB)
	 * 0x66 DRB6 = Total memory in row0+1+2+3+4+5+6 (in 8 MB)
	 * 0x67 DRB7 = Total memory in row0+1+2+3+4+5+6+7 (in 8 MB)
	 */
	/* Set the DRBs to zero for now, this will be fixed later. */
	DRB0, 0x00, 0x00,
	DRB1, 0x00, 0x00,
	DRB2, 0x00, 0x00,
	DRB3, 0x00, 0x00,
	DRB4, 0x00, 0x00,
	DRB5, 0x00, 0x00,
	DRB6, 0x00, 0x00,
	DRB7, 0x00, 0x00,

	/* FDHC - Fixed DRAM Hole Control Register
	 * 0x68
	 *
	 * Controls two fixed DRAM holes: 512 KB - 640 KB and 15 MB - 16 MB.
	 *
	 * [7:6] Hole Enable (HEN)
	 *       00 = None
	 *       01 = 512 KB - 640 KB (128 KB)
	 *       10 = 15 MB - 16 MB (1 MB)
	 *       11 = Reserved
	 * [5:0] Reserved
	 */
	/* No memory holes. */
	FDHC, 0x00, 0x00,

	/* RPS - SDRAM Row Page Size Register
	 * 0x74 - 0x75
	 *
	 * Sets the row page size for SDRAM. For EDO memory, the page
	 * size is fixed at 2 KB.
	 *
	 * [15:0] Page Size (PS)
	 *        TODO
	 */
	// TODO
	RPS + 0, 0x00, 0x00,
	RPS + 1, 0x00, 0x00,

	/* SDRAMC - SDRAM Control Register
	 * 0x76 - 0x77
	 *
	 * [15:10] Reserved
	 * [09:08] Idle/Pipeline DRAM Leadoff Timing (IPDLT)
	 *         00 = Illegal
	 *         01 = Add a clock delay to the lead-off clock count
	 *         10 = Illegal
	 *         11 = Illegal
	 * [07:05] SDRAM Mode Select (SMS)
	 *         000 = Normal SDRAM Operation (default)
	 *         001 = NOP Command Enable
	 *         010 = All Banks Precharge Enable
	 *         011 = Mode Register Set Enable
	 *         100 = CBR Enable
	 *         101 = Reserved
	 *         110 = Reserved
	 *         111 = Reserved
	 * [04:04] SDRAMPWR
	 *         0 = 3 DIMM configuration
	 *         1 = 4 DIMM configuration
	 * [03:03] Leadoff Command Timing (LCT)
	 *         0 = 4 CS# Clock
	 *         1 = 3 CS# Clock
	 * [02:02] CAS# Latency (CL)
	 *         0 = 3 DCLK CAS# latency
	 *         1 = 2 DCLK CAS# latency
	 * [01:01] SDRAM RAS# to CAS# Delay (SRCD)
	 *         0 = 3 clocks between a row activate and a read or write cmd.
	 *         1 = 2 clocks between a row activate and a read or write cmd.
	 * [00:00] SDRAM RAS# Precharge (SRP)
	 *         0 = 3 clocks of RAS# precharge
	 *         1 = 2 clocks of RAS# precharge
	 */
	SDRAMC + 0, 0x00, 0x00,
	SDRAMC + 0, 0x00, 0x00,

	/* PGPOL - Paging Policy Register
	 * 0x78 - 0x79
	 *
	 * [15:08] Banks per Row (BPR)
	 *         TODO
	 *         0 = 2 banks
	 *         1 = 4 banks
	 * [07:05] Reserved
	 * [04:04] Intel Reserved
	 * [03:00] DRAM Idle Timer (DIT)
	 *         0000 = 0 clocks
	 *         0001 = 2 clocks
	 *         0010 = 4 clocks
	 *         0011 = 8 clocks
	 *         0100 = 10 clocks
	 *         0101 = 12 clocks
	 *         0110 = 16 clocks
	 *         0111 = 32 clocks
	 *         1xxx = Infinite (pages are not closed for idle condition)
	 */
	// TODO
	PGPOL + 0, 0x00, 0x00,
	PGPOL + 1, 0x00, 0xff,

	/* PMCR - Power Management Control Register
	 * 0x7a
	 *
	 * [07:07] Power Down SDRAM Enable (PDSE)
	 *         1 = Enable
	 *         0 = Disable
	 * [06:06] ACPI Control Register Enable (SCRE)
	 *         1 = Enable
	 *         0 = Disable (default)
	 * [05:05] Suspend Refresh Type (SRT)
	 *         1 = Self refresh mode
	 *         0 = CBR fresh mode
	 * [04:04] Normal Refresh Enable (NREF_EN)
	 *         1 = Enable
	 *         0 = Disable
	 * [03:03] Quick Start Mode (QSTART)
	 *         1 = Quick start mode for the processor is enabled
	 * [02:02] Gated Clock Enable (GCLKEN)
	 *         1 = Enable
	 *         0 = Disable
	 * [01:01] AGP Disable (AGP_DIS)
	 *         1 = Disable
	 *         0 = Enable
	 * [00:00] CPU reset without PCIRST enable (CRst_En)
	 *         1 = Enable
	 *         0 = Disable
	 */
	/* Enable normal refresh and the gated clock. */
	// TODO: Only do this later?
	// PMCR, 0x00, 0x14,
	// PMCR, 0x00, 0x10,
	PMCR, 0x00, 0x00,
};

/*-----------------------------------------------------------------------------
SDRAM configuration functions.
-----------------------------------------------------------------------------*/

/**
 * Send the specified RAM command to all DIMMs.
 *
 * @param Memory controller
 * @param TODO
 * @param TODO
 */
static void do_ram_command(const struct mem_controller *ctrl,
			   uint32_t command, uint32_t addr_offset)
{
	int i;
	uint16_t reg;

	/* TODO: Support for multiple DIMMs. */

	/* Configure the RAM command. */
	reg = pci_read_config16(ctrl->d0, SDRAMC);
	reg &= 0xff1f;		/* Clear bits 7-5. */
	reg |= (uint16_t) (command << 5);
	pci_write_config16(ctrl->d0, SDRAMC, reg);

	/* RAM_COMMAND_NORMAL affects only the memory controller and
	   doesn't need to be "sent" to the DIMMs. */
	/* if (command == RAM_COMMAND_NORMAL) return; */

	PRINT_DEBUG("    Sending RAM command 0x");
	PRINT_DEBUG_HEX16(reg);
	PRINT_DEBUG(" to 0x");
	PRINT_DEBUG_HEX32(0 + addr_offset); // FIXME
	PRINT_DEBUG("\r\n");

	/* Read from (DIMM start address + addr_offset). */
	read32(0 + addr_offset); // FIXME
}

/*-----------------------------------------------------------------------------
DIMM-independant configuration functions.
-----------------------------------------------------------------------------*/

/**
 * TODO.
 *
 * @param Memory controller
 */
static void spd_enable_refresh(const struct mem_controller *ctrl)
{
	int i, value;
	uint8_t reg;

	reg = pci_read_config8(ctrl->d0, DRAMC);

	for (i = 0; i < DIMM_SOCKETS; i++) {
		value = spd_read_byte(ctrl->channel0[i], SPD_REFRESH);
		if (value < 0)
			continue;
		reg = (reg & 0xf8) | refresh_rate_map[(value & 0x7f)];

		PRINT_DEBUG("    Enabling refresh (DRAMC = 0x");
		PRINT_DEBUG_HEX8(reg);
		PRINT_DEBUG(") for DIMM ");
		PRINT_DEBUG_HEX8(i);
		PRINT_DEBUG("\r\n");
	}

	pci_write_config8(ctrl->d0, DRAMC, reg);
}

/*-----------------------------------------------------------------------------
Public interface.
-----------------------------------------------------------------------------*/

/**
 * TODO.
 *
 * @param Memory controller
 */
static void sdram_set_registers(const struct mem_controller *ctrl)
{
	int i, max;
	uint8_t reg;

	PRINT_DEBUG("Northbridge prior to SDRAM init:\r\n");
	DUMPNORTH();

	max = sizeof(register_values) / sizeof(register_values[0]);

	/* Set registers as specified in the register_values[] array. */
	for (i = 0; i < max; i += 3) {
		reg = pci_read_config8(ctrl->d0, register_values[i]);
		reg &= register_values[i + 1];
		reg |= register_values[i + 2] & ~(register_values[i + 1]);
		pci_write_config8(ctrl->d0, register_values[i], reg);

		PRINT_DEBUG("    Set register 0x");
		PRINT_DEBUG_HEX8(register_values[i]);
		PRINT_DEBUG(" to 0x");
		PRINT_DEBUG_HEX8(reg);
		PRINT_DEBUG("\r\n");
	}
}

/**
 * TODO.
 *
 * @param Memory controller
 */
static void sdram_set_spd_registers(const struct mem_controller *ctrl)
{
	/* TODO: Don't hardcode the values here, get info via SPD. */

	/* Map all legacy regions to RAM (read/write). This is required if
	 * you want to use the RAM area from 768 KB - 1 MB. If the PAM
	 * registers are not set here appropriately, the RAM in that region
	 * will not be accessible, thus a RAM check of it will also fail.
	 */
	pci_write_config8(ctrl->d0, PAM0, 0x30);
	pci_write_config8(ctrl->d0, PAM1, 0x33);
	pci_write_config8(ctrl->d0, PAM2, 0x33);
	pci_write_config8(ctrl->d0, PAM3, 0x33);
	pci_write_config8(ctrl->d0, PAM4, 0x33);
	pci_write_config8(ctrl->d0, PAM5, 0x33);
	pci_write_config8(ctrl->d0, PAM6, 0x33);

	/* TODO: Set DRB0-DRB7. */
	/* Currently this is hardcoded to one 64 MB DIMM in slot 0. */
	pci_write_config8(ctrl->d0, DRB0, 0x08);
	pci_write_config8(ctrl->d0, DRB1, 0x08);
	pci_write_config8(ctrl->d0, DRB2, 0x08);
	pci_write_config8(ctrl->d0, DRB3, 0x08);
	pci_write_config8(ctrl->d0, DRB4, 0x08);
	pci_write_config8(ctrl->d0, DRB5, 0x08);
	pci_write_config8(ctrl->d0, DRB6, 0x08);
	pci_write_config8(ctrl->d0, DRB7, 0x08);

	/* TODO: Set DRAMC. Don't enable refresh for now. */
	pci_write_config8(ctrl->d0, DRAMC, 0x08);

	/* TODO: Set RPS. */
	pci_write_config16(ctrl->d0, RPS, 0x0001);

	/* TODO: Set SDRAMC. */
	// pci_write_config16(ctrl->d0, SDRAMC, 0x010f); // FIXME?
	pci_write_config16(ctrl->d0, SDRAMC, 0x0003); // FIXME?

	/* TODO: Set PGPOL. */
	// pci_write_config16(ctrl->d0, PGPOL, 0x0107);
	pci_write_config16(ctrl->d0, PGPOL, 0x0123);

	/* TODO: Set NBXCFG. */
	// pci_write_config32(ctrl->d0, NBXCFG, 0x0100220c); // FIXME?
	pci_write_config32(ctrl->d0, NBXCFG, 0xff00800c);

	/* TODO: Set PMCR? */
	// pci_write_config8(ctrl->d0, PMCR, 0x14);
	pci_write_config8(ctrl->d0, PMCR, 0x10);

	/* TODO? */
	pci_write_config8(ctrl->d0, MLT, 0x40);
	pci_write_config8(ctrl->d0, DRAMT, 0x03);
	pci_write_config8(ctrl->d0, MBSC, 0x03);
	pci_write_config8(ctrl->d0, SCRR, 0x38);
}

/**
 * Enable SDRAM.
 *
 * @param Number of controllers
 * @param Memory controller
 */
static void sdram_enable(int controllers, const struct mem_controller *ctrl)
{
	int i;

	/* 0. Wait until power/voltages and clocks are stable (200us). */
	udelay(200);

	/* 1. Apply NOP. Wait 200 clock cycles (200us should do). */
	PRINT_DEBUG("RAM Enable 1: Apply NOP\r\n");
	do_ram_command(ctrl, RAM_COMMAND_NOP, 0);
	udelay(200);

	/* 2. Precharge all. Wait tRP. */
	PRINT_DEBUG("RAM Enable 2: Precharge all\r\n");
	do_ram_command(ctrl, RAM_COMMAND_PRECHARGE, 0);
	udelay(1);

	/* 3. Perform 8 refresh cycles. Wait tRC each time. */
	PRINT_DEBUG("RAM Enable 3: CBR\r\n");
	for (i = 0; i < 8; i++) {
		do_ram_command(ctrl, RAM_COMMAND_CBR, 0);
		udelay(1);
	}

	/* 4. Mode register set. Wait two memory cycles. */
	PRINT_DEBUG("RAM Enable 4: Mode register set\r\n");
	do_ram_command(ctrl, RAM_COMMAND_MRS, 0x1d0);
	udelay(2);

	/* 5. Normal operation. */
	PRINT_DEBUG("RAM Enable 5: Normal operation\r\n");
	do_ram_command(ctrl, RAM_COMMAND_NORMAL, 0);
	udelay(1);

	/* 6. Finally enable refresh. */
	PRINT_DEBUG("RAM Enable 6: Enable refresh\r\n");
	// pci_write_config8(ctrl->d0, PMCR, 0x10);
	spd_enable_refresh(ctrl);
	udelay(1);

	PRINT_DEBUG("Northbridge following SDRAM init:\r\n");
	DUMPNORTH();
}
