/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <spd.h>
#include <delay.h>
#include <stdint.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <device/smbus_host.h>
#include <console/console.h>
#include <timestamp.h>
#include "i440bx.h"
#include "raminit.h"

/*
 * Macros and definitions
 */

/* Debugging macros. */
#if CONFIG(DEBUG_RAM_SETUP)
#define PRINT_DEBUG(x...)	printk(BIOS_DEBUG, x)
#define DUMPNORTH()		dump_pci_device(NB)
#else
#define PRINT_DEBUG(x...)
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

/* Table format: register, value. */
static const u8 register_values[] = {
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
	NBXCFG + 0, 0x0c,
#if CONFIG(SMP)
	NBXCFG + 1, 0x00,
#else
	NBXCFG + 1, 0x80,
#endif
	NBXCFG + 2, 0x00,
	NBXCFG + 3, 0xff,

	/* DRAMC - DRAM Control Register
	 * 0x57
	 *
	 * [7:6] Reserved
	 * [5:5] Module Mode Configuration (MMCONFIG)
	 *       The combination of SDRAMPWR and this bit (set by an
	 *       external strapping option) determine how CKE works.
	 *       SDRAMPWR MMCONFIG
	 *	 0        0        = 3 DIMM, CKE[5:0] driven
	 *	 X        1        = 3 DIMM, CKE0 only
	 *	 1        0        = 4 DIMM, GCKE only
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
	DRAMC, 0x08,

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
	 * 0x5e [1:0] 0xE0000 - 0xE3FFF BIOS extension
	 * 0x5e [5:4] 0xE4000 - 0xE7FFF BIOS extension
	 * 0x5f [1:0] 0xE8000 - 0xEBFFF BIOS extension
	 * 0x5f [5:4] 0xEC000 - 0xEFFFF BIOS extension
	 *
	 * Bit assignment:
	 * 00 = DRAM Disabled (all access goes to memory mapped I/O space)
	 * 01 = Read Only (Reads to DRAM, writes to memory mapped I/O space)
	 * 10 = Write Only (Writes to DRAM, reads to memory mapped I/O space)
	 * 11 = Read/Write (all access goes to DRAM)
	 */

	/*
	 * Map all legacy regions to RAM (read/write). This is required if
	 * you want to use the RAM area from 768 KB - 1 MB. If the PAM
	 * registers are not set here appropriately, the RAM in that region
	 * will not be accessible, thus a RAM check of it will also fail.
	 */
	PAM0, 0x30,
	PAM1, 0x33,
	PAM2, 0x33,
	PAM3, 0x33,
	PAM4, 0x33,
	PAM5, 0x33,
	PAM6, 0x33,

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
	/* DRBs will be set later. */

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
	FDHC, 0x00,

	/* RPS - SDRAM Row Page Size Register
	 * 0x74 - 0x75
	 *
	 * Sets the row page size for SDRAM. For EDO memory, the page
	 * size is fixed at 2 KB.
	 *
	 * Bits[1:0] Page Size
	 * 00        2 KB
	 * 01        4 KB
	 * 10        8 KB
	 * 11        Reserved
	 *
	 * RPS bits Corresponding DRB register
	 * [01:00]  DRB[0], row 0
	 * [03:02]  DRB[1], row 1
	 * [05:04]  DRB[2], row 2
	 * [07:06]  DRB[3], row 3
	 * [09:08]  DRB[4], row 4
	 * [11:10]  DRB[5], row 5
	 * [13:12]  DRB[6], row 6
	 * [15:14]  DRB[7], row 7
	 */
	/* Power on defaults to 2KB. Will be set later. */

	/* SDRAMC - SDRAM Control Register
	 * 0x76 - 0x77
	 *
	 * [15:10] Reserved
	 * [09:08] Idle/Pipeline DRAM Leadoff Timing (IPDLT)
	 *         00 = Illegal
	 *         01 = Add a clock delay to the lead-off clock count
	 *         1x = Illegal
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
#if CONFIG(SDRAMPWR_4DIMM)
	SDRAMC, 0x10, /* The board has 4 DIMM slots. */
#else
	SDRAMC, 0x00, /* The board has 3 DIMM slots. */
#endif

	/* PGPOL - Paging Policy Register
	 * 0x78 - 0x79
	 *
	 * [15:08] Banks per Row (BPR)
	 *         Each bit in this field corresponds to one row of the memory
	 *         array. Bit 15 corresponds to row 7 while bit 8 corresponds
	 *         to row 0. Bits for empty rows are "don't care".
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
	/* PGPOL will be set later. */

	/* PMCR - Power Management Control Register
	 * 0x7a
	 *
	 * [7] Power Down SDRAM Enable (PDSE)
	 *     1 = Enable
	 *     0 = Disable
	 * [6] ACPI Control Register Enable (SCRE)
	 *     1 = Enable
	 *     0 = Disable (default)
	 * [5] Suspend Refresh Type (SRT)
	 *     1 = Self refresh mode
	 *     0 = CBR fresh mode
	 * [4] Normal Refresh Enable (NREF_EN)
	 *     1 = Enable
	 *     0 = Disable
	 * [3] Quick Start Mode (QSTART)
	 *     1 = Quick start mode for the processor is enabled
	 * [2] Gated Clock Enable (GCLKEN)
	 *     1 = Enable
	 *     0 = Disable
	 * [1] AGP Disable (AGP_DIS)
	 *     1 = AGP disabled (Hardware strap)
	 * [0] CPU reset without PCIRST enable (CRst_En)
	 *     1 = Enable
	 *     0 = Disable
	 */
	/* PMCR will be set later. */

	/* Enable SCRR.SRRAEN and let BX choose the SRR. */
	SCRR + 1, 0x10,
};

/*-----------------------------------------------------------------------------
SDRAM configuration functions.
-----------------------------------------------------------------------------*/

/**
 * Send the specified RAM command to all DIMMs.
 *
 * @param command The RAM command to send to the DIMM(s).
 */
static void do_ram_command(u32 command)
{
	int i, caslatency;
	u8 dimm_start, dimm_end;
	u16 reg16;
	void *addr;
	u32 addr_offset;

	/* Configure the RAM command. */
	reg16 = pci_read_config16(NB, SDRAMC);
	reg16 &= 0xff1f;		/* Clear bits 7-5. */
	reg16 |= (u16) (command << 5);	/* Write command into bits 7-5. */
	pci_write_config16(NB, SDRAMC, reg16);

	/*
	 * RAM_COMMAND_NORMAL affects only the memory controller and
	 * doesn't need to be "sent" to the DIMMs.
	 */
	if (command == RAM_COMMAND_NORMAL)
		return;

	/* Send the RAM command to each row of memory. */
	dimm_start = 0;
	for (i = 0; i < (DIMM_SOCKETS * 2); i++) {
		addr_offset = 0;
		caslatency = 3; /* TODO: Dynamically get CAS latency later. */
		if (command == RAM_COMMAND_MRS) {
			/*
			 * MAA[12:11,9:0] must be inverted when sent to DIMM
			 * 2 or 3 (no inversion if sent to DIMM 0 or 1).
			 */
			if ((i >= 0 && i <= 3) && caslatency == 3)
				addr_offset = 0x1d0;
			if ((i >= 4 && i <= 7) && caslatency == 3)
				addr_offset = 0x1e28;
			if ((i >= 0 && i <= 3) && caslatency == 2)
				addr_offset = 0x150;
			if ((i >= 4 && i <= 7) && caslatency == 2)
				addr_offset = 0x1ea8;
		}

		dimm_end = pci_read_config8(NB, DRB + i);

		addr = (void *)((dimm_start * 8 * 1024 * 1024) + addr_offset);
		if (dimm_end > dimm_start) {
			read32(addr);
		}

		/* Set the start of the next DIMM. */
		dimm_start = dimm_end;
	}
}

static void set_dram_buffer_strength(void)
{
	/*
	 * Program MBSC[39:0] and MBFS[23:0].
	 *
	 * The 440BX datasheet says buffer frequency is independent from bus
	 * frequency and mismatch both ways are possible.
	 *
	 * MBSC[47:40] and MBFS[23] are reserved.
	 */

	unsigned int i, reg, drb;
	uint8_t mbsc0, mbfs0, mbfs1, mbfs2;
	uint16_t mbsc1, mbsc3;

	/*
	 * Tally how many rows between rows 0-3 and rows 4-7 are populated.
	 * This determines how to program MBFS and MBSC.
	 */
	uint8_t dimm03 = 0;
	uint8_t dimm47 = 0;

	for (drb = 0, i = DRB0; i <= DRB7; i++) {
		reg = pci_read_config8(NB, i);
		if (drb != reg) {
			if (i <= DRB3)
				dimm03++;
			else
				dimm47++;

			drb = reg;
		}
	}

	if (CONFIG(SDRAMPWR_4DIMM)) {
	/*
	 * For a 4 DIMM board, based on ASUS P2B-LS mainboard.
	 *
	 * There are four main conditions to check when programming
	 * DRAM buffer frequency and strength:
	 *
	 * a: >2 rows populated across DIMM0,1
	 * b: >2 rows populated across DIMM2,3
	 * c: >4 rows populated across all DIMM slots
	 * and either one of:
	 * 1: NBXCFG[13] strapped as 100MHz, or
	 * 6: NBXCFG[13] strapped as 66MHz
	 *
	 * CKE0/FENA ----------------------------------------------------------+
	 * CKE1/GCKE ----------------------[    MBFS    ]---------------------+|
	 * DQMA/CASA[764320]# -------------[ 0 = 66MHz  ]--------------------+||
	 * DQMB1/CASB1# (Fixed for 66MHz) -[ 1 = 100MHz ]-------------------+|||
	 * DQMB5/CASB5# (Fixed for 66MHz) ---------------------------------+||||
	 * DQMA1/CASA1# (Fixed for 66MHz) --------------------------------+|||||
	 * DQMA5/CASA5# (Fixed for 66MHz) -------------------------------+||||||
	 * CSA[5:0]#,CSB[5:0]# ------------------------------------++++++|||||||
	 * CS[B7,A7,B6,A6]#/CKE[5342] -------------------------++++|||||||||||||
	 * MECC[7:0] #2/#1 ----------------------------------++|||||||||||||||||
	 * MD[63:0] #2/#1 ---------------------------------++|||||||||||||||||||
	 * MAB[12:11,9:0]#,MAB[13,10],WEB#,SRASB#,SCASB# -+|||||||||||||||||||||
	 * MAA[13:0],WEA#,SRASA#,SCASA# -----------------+||||||||||||||||||||||
	 * Reserved ------------------------------------+|||||||||||||||||||||||
	 *                                              ||||||||||||||||||||||||
	 *  3        32        21        10        0  * 2  21        10        0
	 *  9876543210987654321098765432109876543210  * 321098765432109876543210
	 *  10------------------------1010----------  a -1---------------11-----
	 *  11------------------------1111---------- !a -0---------------00-----
	 *  --10--------------------------1010------  b --1----------------11---
	 *  --11--------------------------1111------ !b --0----------------00---
	 *  ----------------------------------1100--  c ----------------------1-
	 *  ----------------------------------1011-- !c ----------------------0-
	 *  ----1010101000000000000000------------00  1 ---11111111111111----1-0
	 *  ----000000000000000000000010101010----00  6 ---1111111111111100000-0
	 *  | | | | | | | | | | ||||||| | | | | | |
	 *  | | | | | | | | | | ||||||| | | | | | +- CKE0/FENA
	 *  | | | | | | | | | | ||||||| | | | | +--- CKE1/GCKE
	 *  | | | | | | | | | | ||||||| | | | +----- DQMA/CASA[764320]#
	 *  | | | | | | | | | | ||||||| | | +------- DQMB1/CASB1# (66MHz: 2x)
	 *  | | | | | | | | | | ||||||| | +--------- DQMB5/CASB5# (66MHz: 2x)
	 *  | | | | | | | | | | ||||||| +----------- DQMA1/CASA1# (66MHz: 2x)
	 *  | | | | | | | | | | ||||||+------------- DQMA5/CASA5# (66MHz: 2x)
	 *  | | | | | | | | | | ++++++-------------- CSA0-5#,CSB0-5# (1x)
	 *  | | | | | | | | | +--------------------- CSA6#/CKE2
	 *  | | | | | | | | +---[    MBSC    ]------ CSB6#/CKE4
	 *  | | | | | | | +-----[ 00 = 1x    ]------ CSA7#/CKE3
	 *  | | | | | | +-------[ 01 invalid ]------ CSB7#/CKE5
	 *  | | | | | +---------[ 10 = 2x    ]------ MECC[7:0] #1
	 *  | | | | +-----------[ 11 = 3x    ]------ MECC[7:0] #2
	 *  | | | +--------------------------------- MD[63:0] #1
	 *  | | +----------------------------------- MD[63:0] #2
	 *  | +------------------ MAB[12:11,9:0]#,MAB[13,10],WEB#,SRASB#,SCASB#
	 *  +------------------------------------- MAA[13:0],WEA#,SRASA#,SCASA#
	 */
		unsigned int fsb;

		mbsc0 = 0xa0;
		mbsc1 = 0x002a;
		mbfs1 = 0xff;
		mbfs2 = 0x1f;
		if (pci_read_config8(NB, NBXCFG + 1) & 0x30) {
			fsb = 66;
			mbsc3 = 0xa000;
			mbfs0 = 0x80;
		} else {
			fsb = 100;
			mbsc3 = 0xaaa0;
			mbfs0 = 0x84;
		}
		if (dimm03 > 2) {
			mbfs2 |= 0x40;
			if (fsb == 100)
				mbfs0 |= 0x60;
		} else {
			mbsc3 |= 0xc000;
			if (fsb == 100)
				mbsc1 |= 0x003c;
		}
		if (dimm47 > 2) {
			mbfs2 |= 0x20;
			if (fsb == 100)
				mbfs0 |= 0x18;
		} else {
			mbsc3 |= 0x3000;
			if (fsb == 100) {
				mbsc1 |= 0x0003;
				mbsc0 |= 0xc0;
			}
		}
		if ((dimm03 + dimm47) > 4) {
			mbsc0 |= 0x30;
			mbfs0 |= 0x02;
		} else {
			mbsc0 |= 0x2c;
		}
	} else {
	/*
	 * For a 3 DIMM board, based on ASUS P2B mainboard.
	 *
	 * There are two main conditions to check when programming DRAM buffer
	 * frequency and strength:
	 *
	 * a: >2 rows populated across DIMM0,1
	 * c: >4 rows populated across all DIMM slots
	 *
	 * CKE0 ---------------------------------------------------------------+
	 * CKE1 ------------------------[    MBFS    ]------------------------+|
	 * DQMA/CASA[764320]# ----------[ 0 = 66MHz  ]-----------------------+||
	 * DQMB1/CASB1# ----------------[ 1 = 100MHz ]----------------------+|||
	 * DQMB5/CASB5# ---------------------------------------------------+||||
	 * DQMA1/CASA1# --------------------------------------------------+|||||
	 * DQMA5/CASA5# -------------------------------------------------+||||||
	 * CSA0-5#,CSB0-5# ----------------------------------------++++++|||||||
	 * CS[B7,A7,B6,A6]#/CKE[5342] -------------------------++++|||||||||||||
	 * MECC[7:0] #2/#1 (100MHz) -------------------------++|||||||||||||||||
	 * MD[63:0] #2/#1 (100MHz) ------------------------++|||||||||||||||||||
	 * MAB[12:11,9:0]#,MAB[13,10],WEB#,SRASB#,SCASB# -+|||||||||||||||||||||
	 * MAA[13:0],WEA#,SRASA#,SCASA# -----------------+||||||||||||||||||||||
	 * Reserved ------------------------------------+|||||||||||||||||||||||
	 *                                              ||||||||||||||||||||||||
	 *  3        32        21        10        0  * 2  21        10        0
	 *  9876543210987654321098765432109876543210  * 321098765432109876543210
	 *  10------------------------1111----------  a -1----------------------
	 *  11------------------------1010---------- !a -0----------------------
	 *  --110000000010101010111111----1010--1010  * --01111000000000000000-0
	 *  ----------------------------------11----  c ----------------------1-
	 *  ----------------------------------10---- !c ----------------------0-
	 *  | | | | | | | | | | ||||||| | | | | | |
	 *  | | | | | | | | | | ||||||| | | | | | +- CKE0
	 *  | | | | | | | | | | ||||||| | | | | +--- CKE1
	 *  | | | | | | | | | | ||||||| | | | +----- DQMA/CASA[764320]#
	 *  | | | | | | | | | | ||||||| | | +------- DQMB1/CASB1#
	 *  | | | | | | | | | | ||||||| | +--------- DQMB5/CASB5#
	 *  | | | | | | | | | | ||||||| +----------- DQMA1/CASA1#
	 *  | | | | | | | | | | ||||||+------------- DQMA5/CASA5#
	 *  | | | | | | | | | | ++++++-------------- CSA0-5#,CSB0-5# (2x)
	 *  | | | | | | | | | +--------------------- CSA6#/CKE2
	 *  | | | | | | | | +---[    MBSC    ]------ CSB6#/CKE4
	 *  | | | | | | | +-----[ 00 = 1x    ]------ CSA7#/CKE3
	 *  | | | | | | +-------[ 01 invalid ]------ CSB7#/CKE5
	 *  | | | | | +---------[ 10 = 2x    ]------ MECC[7:0] #1 (1x)
	 *  | | | | +-----------[ 11 = 3x    ]------ MECC[7:0] #2 (1x)
	 *  | | | +--------------------------------- MD[63:0] #1 (1x)
	 *  | | +----------------------------------- MD[63:0] #2 (1x)
	 *  | +------------------ MAB[12:11,9:0]#,MAB[13,10],WEB#,SRASB#,SCASB#
	 *  +------------------------------------- MAA[13:0],WEA#,SRASA#,SCASA#
	 */

		mbsc0 = 0xaa;
		mbsc1 = 0xafea;
		mbsc3 = 0xb00a;
		mbfs0 = 0x00;
		mbfs1 = 0x00;
		mbfs2 = 0x1e;

		if (dimm03 > 2) {
			mbsc1 |= 0x003c;
			mbfs2 |= 0x40;
		} else {
			mbsc3 |= 0xc000;
		}
		if ((dimm03 + dimm47) > 4) {
			mbsc0 |= 0x30;
			mbfs0 |= 0x02;
		}
	}

	pci_write_config8(NB, MBSC + 0, mbsc0);
	pci_write_config16(NB, MBSC + 1, mbsc1);
	pci_write_config16(NB, MBSC + 3, mbsc3);
	pci_write_config16(NB, MBFS + 0, mbfs1 << 8 | mbfs0);
	pci_write_config8(NB, MBFS + 2, mbfs2);
}

/*-----------------------------------------------------------------------------
DIMM-independent configuration functions.
-----------------------------------------------------------------------------*/

static void spd_enable_refresh(void)
{
	int i, value;
	uint8_t reg;

	reg = pci_read_config8(NB, DRAMC);

	for (i = 0; i < DIMM_SOCKETS; i++) {
		value = smbus_read_byte(DIMM0 + i, SPD_REFRESH);
		if (value < 0)
			continue;
		reg = (reg & 0xf8) | refresh_rate_map[(value & 0x7f)];

		PRINT_DEBUG("    Enabling refresh (DRAMC = 0x%02x) for DIMM %02x\n", reg, i);
	}

	pci_write_config8(NB, DRAMC, reg);
}

/*-----------------------------------------------------------------------------
Public interface.
-----------------------------------------------------------------------------*/

static void sdram_set_registers(void)
{
	int i, max;

	PRINT_DEBUG("Northbridge prior to SDRAM init:\n");
	DUMPNORTH();

	max = ARRAY_SIZE(register_values);

	/* Set registers as specified in the register_values[] array. */
	for (i = 0; i < max; i += 2)
		pci_write_config8(NB, register_values[i], register_values[i + 1]);
}

struct dimm_size {
	u32 side1;
	u32 side2;
};

static struct dimm_size spd_get_dimm_size(unsigned int device)
{
	struct dimm_size sz;
	int i, module_density, dimm_banks;
	sz.side1 = 0;
	module_density = smbus_read_byte(device, SPD_DENSITY_OF_EACH_ROW_ON_MODULE);
	dimm_banks = smbus_read_byte(device, SPD_NUM_DIMM_BANKS);

	/* Find the size of side1. */
	/* Find the larger value. The larger value is always side1. */
	for (i = 512; i >= 0; i >>= 1) {
		if ((module_density & i) == i) {
			sz.side1 = i;
			break;
		}
	}

	/* Set to 0 in case it's single sided. */
	sz.side2 = 0;

	/* Test if it's a dual-sided DIMM. */
	if (dimm_banks > 1) {
		/* Test if there's a second value. If so it's asymmetrical. */
		if (module_density != i) {
			/*
			 * Find second value, picking up where we left off.
			 * i >>= 1 done initially to make sure we don't get
			 * the same value again.
			 */
			for (i >>= 1; i >= 0; i >>= 1) {
				if (module_density == (sz.side1 | i)) {
					sz.side2 = i;
					break;
				}
			}
			/* If not, it's symmetrical. */
		} else {
			sz.side2 = sz.side1;
		}
	}

	/*
	 * SPD byte 31 is the memory size divided by 4 so we
	 * need to multiply by 4 to get the total size.
	 */
	sz.side1 *= 4;
	sz.side2 *= 4;

	/*
	 * It is possible to partially use larger than supported
	 * modules by setting them to a supported size.
	 */
	if (sz.side1 > 128) {
		PRINT_DEBUG("Side1 was %dMB but only 128MB will be used.\n",
			sz.side1);
		sz.side1 = 128;

		if (sz.side2 > 128) {
			PRINT_DEBUG("Side2 was %dMB but only 128MB will be used.\n",
				sz.side2);
			sz.side2 = 128;
		}
	}

	return sz;
}
/*
 * Sets DRAM attributes one DIMM at a time, based on SPD data.
 * Northbridge settings that are set: NBXCFG[31:24], DRB0-DRB7, RPS, DRAMC.
 */
static void set_dram_row_attributes(void)
{
	int i, dra, drb, col, width, value, rps;
	u8 bpr; /* Top 8 bits of PGPOL */
	u8 nbxecc = 0; /* NBXCFG[31:24] */
	u8 edo, sd, regsd; /* EDO, SDRAM, registered SDRAM */

	edo = 0;
	sd = 0;
	regsd = 1;
	rps = 0;
	drb = 0;
	bpr = 0;

	for (i = 0; i < DIMM_SOCKETS; i++) {
		unsigned int device;
		device = DIMM0 + i;
		bpr >>= 2;
		nbxecc >>= 2;

		/* First check if a DIMM is actually present. */
		value = smbus_read_byte(device, SPD_MEMORY_TYPE);
		/* This is 440BX! We do EDO too! */
		if (value == SPD_MEMORY_TYPE_EDO
			|| value == SPD_MEMORY_TYPE_SDRAM) {

			if (value == SPD_MEMORY_TYPE_EDO) {
				edo = 1;
			} else if (value == SPD_MEMORY_TYPE_SDRAM) {
				sd = 1;
			}
			PRINT_DEBUG("Found DIMM in slot %d\n", i);

			if (edo && sd) {
				printk(BIOS_ERR, "Mixing EDO/SDRAM unsupported!\n");
				die("HALT\n");
			}

			/* "DRA" is our RPS for the two rows on this DIMM. */
			dra = 0;

			/* Columns */
			col = smbus_read_byte(device, SPD_NUM_COLUMNS);

			/*
			 * Is this an ECC DIMM? Actually will be a 2 if so.
			 * TODO: Other register than NBXCFG also needs this
			 * ECC information.
			 */
			value = smbus_read_byte(device, SPD_DIMM_CONFIG_TYPE);

			/* Data width */
			width = smbus_read_byte(device, SPD_MODULE_DATA_WIDTH_LSB);

			/* Exclude error checking data width from page size calculations */
			if (value) {
				value = smbus_read_byte(device,
					SPD_ERROR_CHECKING_SDRAM_WIDTH);
				width -= value;
				/* ### ECC */
				/* Clear top 2 bits to help set up NBXCFG. */
				nbxecc &= 0x3f;
			} else {
				/* Without ECC, top 2 bits should be 11. */
				nbxecc |= 0xc0;
			}

			/* If any installed DIMM is *not* registered, this system cannot be
			 * configured for registered SDRAM.
			 * By registered, only the address and control lines need to be, which
			 * we can tell by reading SPD byte 21, bit 1.
			 */
			value = smbus_read_byte(device, SPD_MODULE_ATTRIBUTES);

			PRINT_DEBUG("DIMM is ");
			if ((value & MODULE_REGISTERED) == 0) {
				regsd = 0;
				PRINT_DEBUG("not ");
			}
			PRINT_DEBUG("registered\n");

			/* Calculate page size in bits. */
			value = ((1 << col) * width);

			/* Convert to KB. */
			dra = (value >> 13);

			/* Number of banks of DIMM (single or double sided). */
			value = smbus_read_byte(device, SPD_NUM_DIMM_BANKS);

			/* Once we have dra, col is done and can be reused.
			 * So it's reused for number of banks.
			 */
			col = smbus_read_byte(device, SPD_NUM_BANKS_PER_SDRAM);

			if (value == 1) {
				/*
				 * Second bank of 1-bank DIMMs "doesn't have
				 * ECC" - or anything.
				 */
				if (dra == 2) {
					dra = 0x0; /* 2KB */
				} else if (dra == 4) {
					dra = 0x1; /* 4KB */
				} else if (dra == 8) {
					dra = 0x2; /* 8KB */
				} else if (dra >= 16) {
					/* Page sizes larger than supported are
					 * set to 8KB to use module partially.
					 */
					PRINT_DEBUG("Page size forced to 8KB.\n");
					dra = 0x2; /* 8KB */
				} else {
					dra = -1;
				}
				/*
				 * Sets a flag in PGPOL[BPR] if this DIMM has
				 * 4 banks per row.
				 */
				if (col == 4)
					bpr |= 0x40;
			} else if (value == 2) {
				if (dra == 2) {
					dra = 0x0; /* 2KB */
				} else if (dra == 4) {
					dra = 0x05; /* 4KB */
				} else if (dra == 8) {
					dra = 0x0a; /* 8KB */
				} else if (dra >= 16) {
					/* Ditto */
					PRINT_DEBUG("Page size forced to 8KB.\n");
					dra = 0x0a; /* 8KB */
				} else {
					dra = -1;
				}
				/* Ditto */
				if (col == 4)
					bpr |= 0xc0;
			} else {
				printk(BIOS_ERR, "# of banks of DIMM unsupported!\n");
				die("HALT\n");
			}
			if (dra == -1) {
				printk(BIOS_ERR, "Page size not supported\n");
				die("HALT\n");
			}

			/*
			 * 440BX supports asymmetrical dual-sided DIMMs,
			 * but can't handle DIMMs smaller than 8MB per
			 * side.
			 */
			struct dimm_size sz = spd_get_dimm_size(device);
			if ((sz.side1 < 8)) {
				printk(BIOS_ERR, "DIMMs smaller than 8MB per side\n"
					  "are not supported on this NB.\n");
				die("HALT\n");
			}

			/* Divide size by 8 to set up the DRB registers. */
			drb += (sz.side1 / 8);

			/*
			 * Build the DRB for the next row in MSB so it gets
			 * placed in DRB[n+1] where it belongs when written
			 * as a 16-bit word.
			 */
			drb &= 0xff;
			drb |= (drb + (sz.side2 / 8)) << 8;
		} else {
			/* If there's no DIMM in the slot, set dra to 0x00. */
			dra = 0x00;
			/* Still have to propagate DRB over. */
			drb &= 0xff;
			drb |= (drb << 8);
		}

		pci_write_config16(NB, DRB + (2 * i), drb);

		/* Brings the upper DRB back down to be base for
		 * DRB calculations for the next two rows.
		 */
		drb >>= 8;

		rps |= (dra & 0x0f) << (i * 4);
	}

	/* Set paging policy register. */
	pci_write_config8(NB, PGPOL + 1, bpr);
	PRINT_DEBUG("PGPOL[BPR] has been set to 0x%02x\n", bpr);

	/* Set DRAM row page size register. */
	pci_write_config16(NB, RPS, rps);
	PRINT_DEBUG("RPS has been set to 0x%04x\n", rps);

	/* ### ECC */
	pci_write_config8(NB, NBXCFG + 3, nbxecc);
	PRINT_DEBUG("NBXECC[31:24] has been set to 0x%02x\n", nbxecc);

	/* Set DRAMC[4:3] to proper memory type (EDO/SDRAM/Registered SDRAM). */

	/* i will be used to set DRAMC[4:3]. */
	if (regsd && sd) {
		i = 0x10; // Registered SDRAM
	} else if (sd) {
		i = 0x08; // SDRAM
	} else {
		i = 0; // EDO
	}

	value = pci_read_config8(NB, DRAMC) & 0xe7;
	value |= i;
	pci_write_config8(NB, DRAMC, value);
	PRINT_DEBUG("DRAMC has been set to 0x%02x\n", value);
}

static void sdram_set_spd_registers(void)
{
	/* Setup DRAM row boundary registers and other attributes. */
	set_dram_row_attributes();

	/* Setup DRAM buffer strength. */
	set_dram_buffer_strength();
}

static void sdram_enable(void)
{
	int i;

	/* 0. Wait until power/voltages and clocks are stable (200us). */
	udelay(200);

	/* 1. Apply NOP. Wait 200 clock cycles (200us should do). */
	PRINT_DEBUG("RAM Enable 1: Apply NOP\n");
	do_ram_command(RAM_COMMAND_NOP);
	udelay(200);

	/* 2. Precharge all. Wait tRP. */
	PRINT_DEBUG("RAM Enable 2: Precharge all\n");
	do_ram_command(RAM_COMMAND_PRECHARGE);
	udelay(1);

	/* 3. Perform 8 refresh cycles. Wait tRC each time. */
	PRINT_DEBUG("RAM Enable 3: CBR\n");
	for (i = 0; i < 8; i++) {
		do_ram_command(RAM_COMMAND_CBR);
		udelay(1);
	}

	/* 4. Mode register set. Wait two memory cycles. */
	PRINT_DEBUG("RAM Enable 4: Mode register set\n");
	do_ram_command(RAM_COMMAND_MRS);
	udelay(2);

	/* 5. Normal operation. */
	PRINT_DEBUG("RAM Enable 5: Normal operation\n");
	do_ram_command(RAM_COMMAND_NORMAL);
	udelay(1);

	/* 6. Finally enable refresh. */
	PRINT_DEBUG("RAM Enable 6: Enable refresh\n");
	pci_write_config8(NB, PMCR, 0x10);
	spd_enable_refresh();
	udelay(1);

	PRINT_DEBUG("Northbridge following SDRAM init:\n");
	DUMPNORTH();
}

/* Implemented under mainboard. */
void __weak enable_spd(void) { }
void __weak disable_spd(void) { }

void sdram_initialize(int s3resume)
{
	timestamp_add_now(TS_INITRAM_START);
	enable_spd();

	dump_spd_registers();
	sdram_set_registers();
	sdram_set_spd_registers();
	sdram_enable();

	disable_spd();
	timestamp_add_now(TS_INITRAM_END);
}
