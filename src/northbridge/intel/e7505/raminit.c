/* This was originally for the e7500, modified for e7501
 * The primary differences are that 7501 apparently can
 * support single channel RAM (i haven't tested),
 * CAS1.5 is no longer supported, The ECC scrubber
 * now supports a mode to zero RAM and init ECC in one step
 * and the undocumented registers at 0x80 require new
 * (undocumented) values determined by guesswork and
 * comparison w/ OEM BIOS values.
 * Steven James 02/06/2003
 */

/* converted to C 6/2004 yhlu */

#include <assert.h>
#include <spd.h>
#include <sdram_mode.h>
#include <stdlib.h>
#include "e7501.h"

/*-----------------------------------------------------------------------------
Definitions:
-----------------------------------------------------------------------------*/

// Uncomment this to enable run-time checking of DIMM parameters
// for dual-channel operation
// Unfortunately the code seems to chew up several K of space.
//#define VALIDATE_DIMM_COMPATIBILITY

#if CONFIG_DEBUG_RAM_SETUP
#define RAM_DEBUG_MESSAGE(x)	print_debug(x)
#define RAM_DEBUG_HEX32(x)	print_debug_hex32(x)
#define RAM_DEBUG_HEX8(x)	print_debug_hex8(x)
#define DUMPNORTH()		dump_pci_device(PCI_DEV(0, 0, 0))
#else
#define RAM_DEBUG_MESSAGE(x)
#define RAM_DEBUG_HEX32(x)
#define RAM_DEBUG_HEX8(x)
#define DUMPNORTH()
#endif

#define E7501_SDRAM_MODE	(SDRAM_BURST_INTERLEAVED | SDRAM_BURST_4)
#define SPD_ERROR		"Error reading SPD info\n"

// NOTE: This used to be 0x100000.
//       That doesn't work on systems where A20M# is asserted, because
//       attempts to access 0x1000NN end up accessing 0x0000NN.
#define RCOMP_MMIO 0x200000

struct dimm_size {
	unsigned long side1;
	unsigned long side2;
};

static const uint32_t refresh_frequency[] = {
	/* Relative frequency (array value) of each E7501 Refresh Mode Select
	 * (RMS) value (array index)
	 * 0 == least frequent refresh (longest interval between refreshes)
	 * [0] disabled  -> 0
	 * [1] 15.6 usec -> 2
	 * [2]  7.8 usec -> 3
	 * [3] 64   usec -> 1
	 * [4] reserved  -> 0
	 * [5] reserved  -> 0
	 * [6] reserved  -> 0
	 * [7] 64 clocks -> 4
	 */
	0, 2, 3, 1, 0, 0, 0, 4
};

static const uint32_t refresh_rate_map[] = {
	/* Map the JEDEC spd refresh rates (array index) to E7501 Refresh Mode
	 * Select values (array value)
	 * These are all the rates defined by JESD21-C Appendix D, Rev. 1.0
	 * The E7501 supports only 15.6 us (1), 7.8 us (2), 64 us (3), and
	 * 64 clock (481 ns) (7) refresh.
	 * [0] ==  15.625 us -> 15.6 us
	 * [1] ==   3.9   us -> 481  ns
	 * [2] ==   7.8   us ->  7.8 us
	 * [3] ==  31.3   us -> 15.6 us
	 * [4] ==  62.5   us -> 15.6 us
	 * [5] == 125     us -> 64   us
	 */
	1, 7, 2, 1, 1, 3
};

#define MAX_SPD_REFRESH_RATE ((sizeof(refresh_rate_map) / sizeof(uint32_t)) - 1)

// SPD parameters that must match for dual-channel operation
static const uint8_t dual_channel_parameters[] = {
	SPD_MEMORY_TYPE,
	SPD_MODULE_VOLTAGE,
	SPD_NUM_COLUMNS,
	SPD_NUM_ROWS,
	SPD_NUM_DIMM_BANKS,
	SPD_PRIMARY_SDRAM_WIDTH,
	SPD_NUM_BANKS_PER_SDRAM
};

	/*
	 * Table:       constant_register_values
	 */
static const long constant_register_values[] = {
	/* SVID - Subsystem Vendor Identification Register
	 * 0x2c - 0x2d
	 * [15:00] Subsytem Vendor ID (Indicates system board vendor)
	 */
	/* SID - Subsystem Identification Register
	 * 0x2e - 0x2f
	 * [15:00] Subsystem ID
	 */
	// Not everyone wants to be Super Micro Computer, Inc.
	// The mainboard should set this if desired.
	// 0x2c, 0, (0x15d9 << 0) | (0x3580 << 16),

	/* Undocumented
	 * (DRAM Read Timing Control, if similar to 855PM?)
	 * 0x80 - 0x81
	 * This register has something to do with CAS latencies,
	 * possibily this is the real chipset control.
	 * At 0x00 CAS latency 1.5 works.
	 * At 0x06 CAS latency 2.5 works.
	 * At 0x01 CAS latency 2.0 works.
	 */
	/* This is still undocumented in e7501, but with different values
	 * CAS 2.0 values taken from Intel BIOS settings, others are a guess
	 * and may be terribly wrong. Old values preserved as comments until I
	 * figure this out for sure.
	 * e7501 docs claim that CAS1.5 is unsupported, so it may or may not
	 * work at all.
	 * Steven James 02/06/2003
	 */
	/* NOTE: values now configured in configure_e7501_cas_latency() based
	 *       on SPD info and total number of DIMMs (per Intel)
	 */

	/* FDHC - Fixed DRAM Hole Control
	 * 0x58
	 * [7:7] Hole_Enable
	 *       0 == No memory Hole
	 *       1 == Memory Hole from 15MB to 16MB
	 * [6:0] Reserved
	 *
	 * PAM - Programmable Attribute Map
	 * 0x59 [1:0] Reserved
	 * 0x59 [5:4] 0xF0000 - 0xFFFFF
	 * 0x5A [1:0] 0xC0000 - 0xC3FFF
	 * 0x5A [5:4] 0xC4000 - 0xC7FFF
	 * 0x5B [1:0] 0xC8000 - 0xCBFFF
	 * 0x5B [5:4] 0xCC000 - 0xCFFFF
	 * 0x5C [1:0] 0xD0000 - 0xD3FFF
	 * 0x5C [5:4] 0xD4000 - 0xD7FFF
	 * 0x5D [1:0] 0xD8000 - 0xDBFFF
	 * 0x5D [5:4] 0xDC000 - 0xDFFFF
	 * 0x5E [1:0] 0xE0000 - 0xE3FFF
	 * 0x5E [5:4] 0xE4000 - 0xE7FFF
	 * 0x5F [1:0] 0xE8000 - 0xEBFFF
	 * 0x5F [5:4] 0xEC000 - 0xEFFFF
	 *       00 == DRAM Disabled (All Access go to memory mapped I/O space)
	 *       01 == Read Only (Reads to DRAM, Writes to memory mapped I/O space)
	 *       10 == Write Only (Writes to DRAM, Reads to memory mapped I/O space)
	 *       11 == Normal (All Access go to DRAM)
	 */

	// Map all legacy ranges to DRAM
	0x58, 0xcccccf7f, (0x00 << 0) | (0x30 << 8) | (0x33 << 16) | (0x33 << 24),
	0x5C, 0xcccccccc, (0x33 << 0) | (0x33 << 8) | (0x33 << 16) | (0x33 << 24),

	/* DRB - DRAM Row Boundary Registers
	 * 0x60 - 0x6F
	 *     An array of 8 byte registers, which hold the ending
	 *     memory address assigned  to each pair of DIMMS, in 64MB
	 *     granularity.
	 */
	// Conservatively say each row has 64MB of ram, we will fix this up later
	// NOTE: These defaults allow us to prime all of the DIMMs on the board
	//               without jumping through 36-bit adddressing hoops, even if the
	//               total memory is > 4 GB. Changing these values may break do_ram_command()!
	0x60, 0x00000000, (0x01 << 0) | (0x02 << 8) | (0x03 << 16) | (0x04 << 24),
	0x64, 0x00000000, (0x05 << 0) | (0x06 << 8) | (0x07 << 16) | (0x08 << 24),

	/* DRA - DRAM Row Attribute Register
	 * 0x70 Row 0,1
	 * 0x71 Row 2,3
	 * 0x72 Row 4,5
	 * 0x73 Row 6,7
	 * [7:7] Device width for Odd numbered rows
	 *       0 == 8 bits wide x8
	 *       1 == 4 bits wide x4
	 * [6:4] Row Attributes for Odd numbered rows
	 *       010 == 8KB (for dual-channel)
	 *       011 == 16KB (for dual-channel)
	 *       100 == 32KB (for dual-channel)
	 *       101 == 64KB (for dual-channel)
	 *       Others == Reserved
	 * [3:3] Device width for Even numbered rows
	 *       0 == 8 bits wide x8
	 *       1 == 4 bits wide x4
	 * [2:0] Row Attributes for Even numbered rows
	 *       010 == 8KB (for dual-channel)
	 *       011 == 16KB (for dual-channel)
	 *       100 == 32KB (for dual-channel)
	 *       101 == 64KB (This page size appears broken)
	 *       Others == Reserved
	 */
	// NOTE: overridden by configure_e7501_row_attributes(), later
	0x70, 0x00000000, 0,

	/* DRT - DRAM Timing Register
	 * 0x78
	 * [31:30] Reserved
	 * [29:29] Back to Back Write-Read Turn Around
	 *         0 == 3 clocks between WR-RD commands
	 *         1 == 2 clocks between WR-RD commands
	 * [28:28] Back to Back Read-Write Turn Around
	 *         0 == 5 clocks between RD-WR commands
	 *         1 == 4 clocks between RD-WR commands
	 * [27:27] Back to Back Read Turn Around
	 *         0 == 4 clocks between RD commands
	 *         1 == 3 clocks between RD commands
	 * [26:24] Read Delay (tRD)
	 *         000 == 7 clocks
	 *         001 == 6 clocks
	 *         010 == 5 clocks
	 *         Others == Reserved
	 * [23:19] Reserved
	 * [18:16] DRAM idle timer
	 *      000 == infinite
	 *      011 == 16 dram clocks
	 *      001 == 0 clocks
	 * [15:11] Reserved
	 * [10:09] Active to Precharge (tRAS)
	 *         00 == 7 clocks
	 *         01 == 6 clocks
	 *         10 == 5 clocks
	 *         11 == Reserved
	 * [08:06] Reserved
	 * [05:04] Cas Latency (tCL)
	 *         00 == 2.5 Clocks
	 *         01 == 2.0 Clocks
	 *         10 == Reserved (was 1.5 Clocks for E7500)
	 *         11 == Reserved
	 * [03:03] Write Ras# to Cas# Delay (tRCD)
	 *         0 == 3 DRAM Clocks
	 *         1 == 2 DRAM Clocks
	 * [02:01] Read RAS# to CAS# Delay (tRCD)
	 *         00 == reserved
	 *         01 == reserved
	 *         10 == 3 DRAM Clocks
	 *         11 == 2 DRAM Clocks
	 * [00:00] DRAM RAS# to Precharge (tRP)
	 *         0 == 3 DRAM Clocks
	 *         1 == 2 DRAM Clocks
	 */

	// Some earlier settings:
	/* Most aggressive settings possible */
//      0x78, 0xc0fff8c4, (1<<29)|(1<<28)|(1<<27)|(2<<24)|(2<<9)|CAS_LATENCY|(1<<3)|(1<<1)|(1<<0),
//      0x78, 0xc0f8f8c0, (1<<29)|(1<<28)|(1<<27)|(1<<24)|(1<<16)|(2<<9)|CAS_LATENCY|(1<<3)|(3<<1)|(1<<0),
//      0x78, 0xc0f8f9c0, (1<<29)|(1<<28)|(1<<27)|(1<<24)|(1<<16)|(2<<9)|CAS_LATENCY|(1<<3)|(3<<1)|(1<<0),

	// The only things we need to set here are DRAM idle timer, Back-to-Back Read Turnaround, and
	// Back-to-Back Write-Read Turnaround. All others are configured based on SPD.
	0x78, 0xD7F8FFFF, (1 << 29) | (1 << 27) | (1 << 16),

	/* FIXME why was I attempting to set a reserved bit? */
	/* 0x0100040f */

	/* DRC - DRAM Contoller Mode Register
	 * 0x7c
	 * [31:30] Reserved
	 * [29:29] Initialization Complete
	 *         0 == Not Complete
	 *         1 == Complete
	 * [28:23] Reserved
	 * [22:22]         Channels
	 *              0 == Single channel
	 *              1 == Dual Channel
	 * [21:20] DRAM Data Integrity Mode
	 *         00 == Disabled, no ECC
	 *         01 == Reserved
	 *         10 == Error checking, using chip-kill, with correction
	 *         11 == Reserved
	 * [19:18] DRB Granularity (Read-Only)
	 *         00 == 32 MB quantities (single channel mode)
	 *                 01 == 64 MB quantities (dual-channel mode)
	 *                 10 == Reserved
	 *                 11 == Reserved
	 * [17:17] (Intel Undocumented) should always be set to 1       (SJM: comment inconsistent with current setting, below)
	 * [16:16] Command Per Clock - Address/Control Assertion Rule (CPC)
	 *         0 == 2n Rule
	 *         1 == 1n rule
	 * [15:11] Reserved
	 * [10:08] Refresh mode select
	 *         000 == Refresh disabled
	 *         001 == Refresh interval 15.6 usec
	 *         010 == Refresh interval 7.8 usec
	 *         011 == Refresh interval 64 usec
	 *         111 == Refresh every 64 clocks (fast refresh)
	 * [07:07] Reserved
	 * [06:04] Mode Select (SMS)
	 *         000 == Reserved (was Self Refresh Mode in E7500)
	 *         001 == NOP Command
	 *         010 == All Banks Precharge
	 *         011 == Mode Register Set
	 *         100 == Extended Mode Register Set
	 *         101 == Reserved
	 *         110 == CBR Refresh
	 *         111 == Normal Operation
	 * [03:00] Reserved
	 */
//      .long 0x7c, 0xffcefcff, (1<<22)|(2 << 20)|(1 << 16)| (0 << 8),
//      .long 0x7c, 0xff8cfcff, (1<<22)|(2 << 20)|(1 << 17)|(1 << 16)| (0 << 8),
//      .long 0x7c, 0xff80fcff, (1<<22)|(2 << 20)|(1 << 18)|(1 << 17)|(1 << 16)| (0 << 8),

	// Default to dual-channel mode, ECC, 1-clock address/cmd hold
	// NOTE: configure_e7501_dram_controller_mode() configures further
	0x7c, 0xff8ef8ff, (1 << 22) | (2 << 20) | (1 << 16) | (0 << 8),

	/* Another Intel undocumented register
	 * 0x88 - 0x8B
	 * [31:31]      Purpose unknown
	 * [26:26]      Master DLL Reset?
	 *                      0 == Normal operation?
	 *                      1 == Reset?
	 * [07:07]      Periodic memory recalibration?
	 *                      0 == Disabled?
	 *                      1 == Enabled?
	 * [04:04]      Receive FIFO RE-Sync?
	 *                      0 == Normal operation?
	 *                      1 == Reset?
	 */
	// NOTE: Some factory BIOSs don't do this.
	//               Doesn't seem to matter either way.
	0x88, 0xffffff00, 0x80,

	/* CLOCK_DIS - CK/CK# Disable Register
	 * 0x8C
	 * [7:7] DDR Frequency
	 *               0 == 100 MHz (200 MHz data rate)
	 *               1 == 133 MHz (266 MHz data rate)
	 * [6:4] Reserved
	 * [3:3] CK3
	 *       0 == Enable
	 *       1 == Disable
	 * [2:2] CK2
	 *       0 == Enable
	 *       1 == Disable
	 * [1:1] CK1
	 *       0 == Enable
	 *       1 == Disable
	 * [0:0] CK0
	 *       0 == Enable
	 *       1 == Disable
	 */
	// NOTE: Disable all clocks initially; turn ones we need back on
	//               in enable_e7501_clocks()
	0x8C, 0xfffffff0, 0xf,

	/* TOLM - Top of Low Memory Register
	 * 0xC4 - 0xC5
	 * [15:11] Top of low memory (TOLM)
	 *         The address below 4GB that should be treated as RAM,
	 *         on a 128MB granularity.
	 * [10:00] Reserved
	 */
	/* REMAPBASE - Remap Base Address Regsiter
	 * 0xC6 - 0xC7
	 * [15:10] Reserved
	 * [09:00] Remap Base Address [35:26] 64M aligned
	 *         Bits [25:0] are assumed to be 0.
	 */

	// NOTE: TOLM overridden by configure_e7501_ram_addresses()
	0xc4, 0xfc0007ff, (0x2000 << 0) | (0x3ff << 16),

	/* REMAPLIMIT - Remap Limit Address Register
	 * 0xC8 - 0xC9
	 * [15:10] Reserved
	 * [09:00] Remap Limit Address [35:26] 64M aligned
	 * When remaplimit < remapbase the remap window is disabled.
	 */
	0xc8, 0xfffffc00, 0,

	/* DVNP - Device Not Present Register
	 * 0xE0 - 0xE1
	 * [15:05] Reserved
	 * [04:04] Device 4 Function 1 Present
	 *         0 == Present
	 *         1 == Absent
	 * [03:03] Device 3 Function 1 Present
	 *         0 == Present
	 *         1 == Absent
	 * [02:02] Device 2 Function 1 Present
	 *         0 == Present
	 *         1 == Absent
	 * [01:01] Reserved
	 * [00:00] Device 0 Function 1 Present
	 *         0 == Present
	 *         1 == Absent
	 */

	// Enable D0:D1, disable D2:F1, D3:F1, D4:F1
	0xe0, 0xffffffe2, (1 << 4) | (1 << 3) | (1 << 2) | (0 << 0),

	// Undocumented
	0xd8, 0xffff9fff, 0x00000000,

	// Undocumented - this is pure conjecture based on similarity to 855PM
	/* MCHTST - MCH Test Register
	 * 0xF4 - 0xF7
	 * [31:31] Purpose unknown
	 * [30:30] Purpose unknown
	 * [29:23] Unknown - not used?
	 * [22:22] System Memory MMR Enable
	 *         0 == Disable: mem space and BAR at 0x14 are not accessible
	 *         1 == Enable: mem space and BAR at 0x14 are accessible
	 * [21:20] Purpose unknown
	 * [19:02] Unknown - not used?
	 * [01:01] D6EN (Device #6 enable)
	 *         0 == Disable
	 *         1 == Enable
	 * [00:00] Unknown - not used?
	 */

	0xf4, 0x3f8ffffd, 0x40300002,

#ifdef SUSPICIOUS_LOOKING_CODE
	// SJM: Undocumented.
	//              This will access D2:F0:0x50, is this correct??
	0x1050, 0xffffffcf, 0x00000030,
#endif
};

	/* DDR RECOMP tables */

// Slew table for 1x drive?
static const uint32_t maybe_1x_slew_table[] = {
	0x44332211, 0xc9776655, 0xffffffff, 0xffffffff,
	0x22111111, 0x55444332, 0xfffca876, 0xffffffff,
};

// Slew table for 2x drive?
static const uint32_t maybe_2x_slew_table[] = {
	0x00000000, 0x76543210, 0xffffeca8, 0xffffffff,
	0x21000000, 0xa8765432, 0xffffffec, 0xffffffff,
};

// Pull Up / Pull Down offset table, if analogous to IXP2800?
static const uint32_t maybe_pull_updown_offset_table[] = {
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0x88888888, 0x88888888, 0x88888888, 0x88888888,
};

/*-----------------------------------------------------------------------------
Delay functions:
-----------------------------------------------------------------------------*/

#define SLOW_DOWN_IO inb(0x80)
//#define SLOW_DOWN_IO udelay(40);

	/* Estimate that SLOW_DOWN_IO takes about 50&76us */
	/* delay for 200us */

#if 1
static void do_delay(void)
{
	int i;
	for (i = 0; i < 16; i++) {
		SLOW_DOWN_IO;
	}
}

#define DO_DELAY do_delay()
#else
#define DO_DELAY \
	udelay(200)
#endif

#define EXTRA_DELAY DO_DELAY

static void die_on_spd_error(int spd_return_value)
{
	if (spd_return_value < 0)
		die("Error reading SPD info\n");
}

/*-----------------------------------------------------------------------------
Serial presence detect (SPD) functions:
-----------------------------------------------------------------------------*/

/**
 * Calculate the page size for each physical bank of the DIMM:
 *   log2(page size) = (# columns) + log2(data width)
 *
 * NOTE: Page size is the total number of data bits in a row.
 *
 * @param dimm_socket_address SMBus address of DIMM socket to interrogate.
 * @return log2(page size) for each side of the DIMM.
 */
static struct dimm_size sdram_spd_get_page_size(uint16_t dimm_socket_address)
{
	uint16_t module_data_width;
	int value;
	struct dimm_size pgsz;

	pgsz.side1 = 0;
	pgsz.side2 = 0;

	// Side 1
	value = spd_read_byte(dimm_socket_address, SPD_NUM_COLUMNS);
	if (value < 0)
		goto hw_err;
	pgsz.side1 = value & 0xf;	// # columns in bank 1

	/* Get the module data width and convert it to a power of two */
	value =
	    spd_read_byte(dimm_socket_address, SPD_MODULE_DATA_WIDTH_MSB);
	if (value < 0)
		goto hw_err;
	module_data_width = (value & 0xff) << 8;

	value =
	    spd_read_byte(dimm_socket_address, SPD_MODULE_DATA_WIDTH_LSB);
	if (value < 0)
		goto hw_err;
	module_data_width |= (value & 0xff);

	pgsz.side1 += log2(module_data_width);

	/* side two */
	value = spd_read_byte(dimm_socket_address, SPD_NUM_DIMM_BANKS);
	if (value < 0)
		goto hw_err;
	if (value > 2)
		die("Bad SPD value\n");
	if (value == 2) {

		pgsz.side2 = pgsz.side1;	// Assume symmetric banks until we know differently
		value =
		    spd_read_byte(dimm_socket_address, SPD_NUM_COLUMNS);
		if (value < 0)
			goto hw_err;
		if ((value & 0xf0) != 0) {
			// Asymmetric banks
			pgsz.side2 -= value & 0xf;	/* Subtract out columns on side 1 */
			pgsz.side2 += (value >> 4) & 0xf;	/* Add in columns on side 2 */
		}
	}

	return pgsz;

      hw_err:
	die(SPD_ERROR);
	return pgsz;		// Never reached
}

/**
 * Read the width in bits of each DIMM side's DRAMs via SPD (i.e. 4, 8, 16).
 *
 * @param dimm_socket_address SMBus address of DIMM socket to interrogate.
 * @return Width in bits of each DIMM side's DRAMs.
 */
static struct dimm_size sdram_spd_get_width(uint16_t dimm_socket_address)
{
	int value;
	struct dimm_size width;

	width.side1 = 0;
	width.side2 = 0;

	value =
	    spd_read_byte(dimm_socket_address, SPD_PRIMARY_SDRAM_WIDTH);
	die_on_spd_error(value);

	width.side1 = value & 0x7f;	// Mask off bank 2 flag

	if (value & 0x80) {
		width.side2 = width.side1 << 1;	// Bank 2 exists and is double-width
	} else {
		// If bank 2 exists, it's the same width as bank 1
		value =
		    spd_read_byte(dimm_socket_address, SPD_NUM_DIMM_BANKS);
		die_on_spd_error(value);

#ifdef ROMCC_IF_BUG_FIXED
		if (value == 2)
			width.side2 = width.side1;
#else
		switch (value) {
		case 2:
			width.side2 = width.side1;
			break;

		default:
			break;
		}
#endif
	}

	return width;
}

/**
 * Calculate the log base 2 size in bits of both DIMM sides.
 *
 * log2(# bits) = (# columns) + log2(data width) +
 *                (# rows) + log2(banks per SDRAM)
 *
 * Note that it might be easier to use SPD byte 31 here, it has the DIMM size
 * as a multiple of 4MB. The way we do it now we can size both sides of an
 * asymmetric DIMM.
 *
 * @param dimm_socket_address SMBus address of DIMM socket to interrogate.
 * @return log2(number of bits) for each side of the DIMM.
 */
static struct dimm_size spd_get_dimm_size(unsigned dimm_socket_address)
{
	int value;

	// Start with log2(page size)
	struct dimm_size sz = sdram_spd_get_page_size(dimm_socket_address);

	if (sz.side1 > 0) {

		value = spd_read_byte(dimm_socket_address, SPD_NUM_ROWS);
		die_on_spd_error(value);

		sz.side1 += value & 0xf;

		if (sz.side2 > 0) {

			// Double-sided DIMM
			if (value & 0xF0)
				sz.side2 += value >> 4;	// Asymmetric
			else
				sz.side2 += value;	// Symmetric
		}

		value =
		    spd_read_byte(dimm_socket_address,
				  SPD_NUM_BANKS_PER_SDRAM);
		die_on_spd_error(value);

		value = log2(value);
		sz.side1 += value;
		if (sz.side2 > 0)
			sz.side2 += value;
	}

	return sz;
}

#ifdef VALIDATE_DIMM_COMPATIBILITY

/**
 * Determine whether two DIMMs have the same value for an SPD parameter.
 *
 * @param spd_byte_number The SPD byte number to compare in both DIMMs.
 * @param dimm0_address SMBus address of the 1st DIMM socket to interrogate.
 * @param dimm1_address SMBus address of the 2nd DIMM socket to interrogate.
 * @return 1 if both DIMM sockets report the same value for the specified
 *         SPD parameter, 0 if the values differed or an error occurred.
 */
static uint8_t are_spd_values_equal(uint8_t spd_byte_number,
				    uint16_t dimm0_address,
				    uint16_t dimm1_address)
{
	uint8_t bEqual = 0;
	int dimm0_value = spd_read_byte(dimm0_address, spd_byte_number);
	int dimm1_value = spd_read_byte(dimm1_address, spd_byte_number);

	if ((dimm0_value >= 0) && (dimm1_value >= 0)
	    && (dimm0_value == dimm1_value))
		bEqual = 1;

	return bEqual;
}
#endif

/**
 * Scan for compatible DIMMs.
 *
 * The code in this module only supports dual-channel operation, so we test
 * that compatible DIMMs are paired.
 *
 * @param ctrl PCI addresses of memory controller functions, and SMBus
 *             addresses of DIMM slots on the mainboard.
 * @return A bitmask indicating which of the possible sockets for each channel
 *         was found to contain a compatible DIMM.
 *         Bit 0 corresponds to the closest socket for channel 0
 *         Bit 1 to the next socket for channel 0
 *         ...
 *         Bit MAX_DIMM_SOCKETS_PER_CHANNEL-1 to the last socket for channel 0
 *         Bit MAX_DIMM_SOCKETS_PER_CHANNEL is the closest socket for channel 1
 *         ...
 *         Bit 2*MAX_DIMM_SOCKETS_PER_CHANNEL-1 is the last socket for channel 1
 */
static uint8_t spd_get_supported_dimms(const struct mem_controller *ctrl)
{
	int i;
	uint8_t dimm_mask = 0;

	// Have to increase size of dimm_mask if this assertion is violated
	ASSERT(MAX_DIMM_SOCKETS_PER_CHANNEL <= 4);

	// Find DIMMs we can support on channel 0.
	// Then see if the corresponding channel 1 DIMM has the same parameters,
	// since we only support dual-channel.

	for (i = 0; i < MAX_DIMM_SOCKETS_PER_CHANNEL; i++) {

		uint16_t channel0_dimm = ctrl->channel0[i];
		uint16_t channel1_dimm = ctrl->channel1[i];
		uint8_t bDualChannel = 1;
#ifdef VALIDATE_DIMM_COMPATIBILITY
		struct dimm_size page_size;
		struct dimm_size sdram_width;
#endif
		int spd_value;

		if (channel0_dimm == 0)
			continue;	// No such socket on this mainboard

		if (spd_read_byte(channel0_dimm, SPD_MEMORY_TYPE) !=
		    SPD_MEMORY_TYPE_SDRAM_DDR)
			continue;

#ifdef VALIDATE_DIMM_COMPATIBILITY
		if (spd_read_byte(channel0_dimm, SPD_MODULE_VOLTAGE) !=
		    SPD_VOLTAGE_SSTL2)
			continue;	// Unsupported voltage

		// E7501 does not support unregistered DIMMs
		spd_value =
		    spd_read_byte(channel0_dimm, SPD_MODULE_ATTRIBUTES);
		if (!(spd_value & MODULE_REGISTERED) || (spd_value < 0))
			continue;

		// Must support burst = 4 for dual-channel operation on E7501
		// NOTE: for single-channel, burst = 8 is required
		spd_value =
		    spd_read_byte(channel0_dimm,
				  SPD_SUPPORTED_BURST_LENGTHS);
		if (!(spd_value & SPD_BURST_LENGTH_4) || (spd_value < 0))
			continue;

		page_size = sdram_spd_get_page_size(channel0_dimm);
		sdram_width = sdram_spd_get_width(channel0_dimm);

		// Validate DIMM page size
		// The E7501 only supports page sizes of 4, 8, 16, or 32 KB per channel
		// NOTE: 4 KB =  32 Kb = 2^15
		//              32 KB = 262 Kb = 2^18

		if ((page_size.side1 < 15) || (page_size.side1 > 18))
			continue;

		// If DIMM is double-sided, verify side2 page size
		if (page_size.side2 != 0) {
			if ((page_size.side2 < 15)
			    || (page_size.side2 > 18))
				continue;
		}
		// Validate SDRAM width
		// The E7501 only supports x4 and x8 devices

		if ((sdram_width.side1 != 4) && (sdram_width.side1 != 8))
			continue;

		// If DIMM is double-sided, verify side2 width
		if (sdram_width.side2 != 0) {
			if ((sdram_width.side2 != 4)
			    && (sdram_width.side2 != 8))
				continue;
		}
#endif
		// Channel 0 DIMM looks compatible.
		// Now see if it is paired with the proper DIMM on channel 1.

		ASSERT(channel1_dimm != 0);	// No such socket on this mainboard??

		// NOTE: unpopulated DIMMs cause read to fail
		spd_value =
		    spd_read_byte(channel1_dimm, SPD_MODULE_ATTRIBUTES);
		if (!(spd_value & MODULE_REGISTERED) || (spd_value < 0)) {

			print_debug("Skipping un-matched DIMMs - only dual-channel operation supported\n");
			continue;
		}
#ifdef VALIDATE_DIMM_COMPATIBILITY
		spd_value =
		    spd_read_byte(channel1_dimm,
				  SPD_SUPPORTED_BURST_LENGTHS);
		if (!(spd_value & SPD_BURST_LENGTH_4) || (spd_value < 0))
			continue;

		int j;
		for (j = 0; j < sizeof(dual_channel_parameters); ++j) {
			if (!are_spd_values_equal
			    (dual_channel_parameters[j], channel0_dimm,
			     channel1_dimm)) {

				bDualChannel = 0;
				break;
			}
		}
#endif

		// Code around ROMCC bug in optimization of "if" statements
#ifdef ROMCC_IF_BUG_FIXED
		if (bDualChannel) {
			// Made it through all the checks, this DIMM pair is usable
			dimm_mask |= ((1 << i) | (1 << (MAX_DIMM_SOCKETS_PER_CHANNEL + i)));
		} else
			print_debug("Skipping un-matched DIMMs - only dual-channel operation supported\n");
#else
		switch (bDualChannel) {
		case 0:
			print_debug("Skipping un-matched DIMMs - only dual-channel operation supported\n");
			break;

		default:
			// Made it through all the checks, this DIMM pair is usable
			dimm_mask |= (1 << i) | (1 << (MAX_DIMM_SOCKETS_PER_CHANNEL + i));
			break;
		}
#endif
	}

	return dimm_mask;
}

/*-----------------------------------------------------------------------------
SDRAM configuration functions:
-----------------------------------------------------------------------------*/

/**
 * Send the specified command to all DIMMs.
 *
 * @param command Specifies the command to be sent to the DIMMs.
 * @param jedec_mode_bits For the MRS & EMRS commands, bits 0-12 contain the
 *                        register value in JEDEC format.
 */
static void do_ram_command(uint8_t command, uint16_t jedec_mode_bits)
{
	int i;
	uint32_t dram_controller_mode;
	uint8_t dimm_start_64M_multiple = 0;
	uint16_t e7501_mode_bits = jedec_mode_bits;

	// Configure the RAM command
	dram_controller_mode = pci_read_config32(PCI_DEV(0, 0, 0), DRC);
	dram_controller_mode &= 0xFFFFFF8F;
	dram_controller_mode |= command;
	pci_write_config32(PCI_DEV(0, 0, 0), DRC, dram_controller_mode);

	// RAM_COMMAND_NORMAL is an exception.
	// It affects only the memory controller and does not need to be "sent" to the DIMMs.

	if (command != RAM_COMMAND_NORMAL) {

		// Send the command to all DIMMs by accessing a memory location within each
		// NOTE: for mode select commands, some of the location address bits
		// are part of the command

		// Map JEDEC mode bits to E7501
		if (command == RAM_COMMAND_MRS) {
			// Host address lines [15:5] map to DIMM address lines [12:11, 9:1]
			// The E7501 hard-sets DIMM address lines 10 & 0 to zero

			ASSERT(!(jedec_mode_bits & 0x0401));

			e7501_mode_bits = ((jedec_mode_bits & 0x1800) << (15 - 12)) |	// JEDEC bits 11-12 move to bits 14-15
			    ((jedec_mode_bits & 0x03FE) << (13 - 9));	// JEDEC bits 1-9 move to bits 5-13

		} else if (command == RAM_COMMAND_EMRS) {
			// Host address lines [15:3] map to DIMM address lines [12:0]
			e7501_mode_bits = jedec_mode_bits <<= 3;
		} else
			ASSERT(jedec_mode_bits == 0);

		dimm_start_64M_multiple = 0;

		for (i = 0; i < (MAX_NUM_CHANNELS * MAX_DIMM_SOCKETS_PER_CHANNEL); ++i) {

			uint8_t dimm_end_64M_multiple =
			    pci_read_config8(PCI_DEV(0, 0, 0), DRB_ROW_0 + i);
			if (dimm_end_64M_multiple > dimm_start_64M_multiple) {

				// This code assumes DRAM row boundaries are all set below 4 GB
				// NOTE: 0x40 * 64 MB == 4 GB
				ASSERT(dimm_start_64M_multiple < 0x40);

				// NOTE: 2^26 == 64 MB

				uint32_t dimm_start_address =
				    dimm_start_64M_multiple << 26;

				RAM_DEBUG_MESSAGE("    Sending RAM command to 0x");
				RAM_DEBUG_HEX32(dimm_start_address + e7501_mode_bits);
				RAM_DEBUG_MESSAGE("\n");

				read32(dimm_start_address + e7501_mode_bits);

				// Set the start of the next DIMM
				dimm_start_64M_multiple =
				    dimm_end_64M_multiple;
			}
		}
	}
}

/**
 * Set the mode register of all DIMMs.
 *
 * The proper CAS# latency setting is added to the mode bits specified
 * by the caller.
 *
 * @param jedec_mode_bits For the MRS & EMRS commands, bits 0-12 contain the
 *                        register value in JEDEC format.
 */
static void set_ram_mode(uint16_t jedec_mode_bits)
{
	ASSERT(!(jedec_mode_bits & SDRAM_CAS_MASK));

	uint32_t dram_cas_latency =
	    pci_read_config32(PCI_DEV(0, 0, 0), DRT) & DRT_CAS_MASK;

	switch (dram_cas_latency) {
	case DRT_CAS_2_5:
		jedec_mode_bits |= SDRAM_CAS_2_5;
		break;

	case DRT_CAS_2_0:
		jedec_mode_bits |= SDRAM_CAS_2_0;
		break;

	default:
		BUG();
		break;
	}

	do_ram_command(RAM_COMMAND_MRS, jedec_mode_bits);
}

/*-----------------------------------------------------------------------------
DIMM-independant configuration functions:
-----------------------------------------------------------------------------*/

/**
 * Configure the E7501's DRAM Row Boundary (DRB) registers for the memory
 * present in the specified DIMM.
 *
 * @param dimm_log2_num_bits Specifies log2(number of bits) for each side of
 *                           the DIMM.
 * @param total_dram_64M_multiple Total DRAM in the system (as a multiple of
 *                                64 MB) for DIMMs < dimm_index.
 * @param dimm_index Which DIMM pair is being processed
 *                   (0..MAX_DIMM_SOCKETS_PER_CHANNEL).
 * @return New multiple of 64 MB total DRAM in the system.
 */
static uint8_t configure_dimm_row_boundaries(struct dimm_size dimm_log2_num_bits, uint8_t total_dram_64M_multiple, unsigned dimm_index)
{
	int i;

	ASSERT(dimm_index < MAX_DIMM_SOCKETS_PER_CHANNEL);

	// DIMM sides must be at least 32 MB
	ASSERT(dimm_log2_num_bits.side1 >= 28);
	ASSERT((dimm_log2_num_bits.side2 == 0)
	       || (dimm_log2_num_bits.side2 >= 28));

	// In dual-channel mode, we are called only once for each pair of DIMMs.
	// Each time we process twice the capacity of a single DIMM.

	// Convert single DIMM capacity to paired DIMM capacity
	// (multiply by two ==> add 1 to log2)
	dimm_log2_num_bits.side1++;
	if (dimm_log2_num_bits.side2 > 0)
		dimm_log2_num_bits.side2++;

	// Add the capacity of side 1 this DIMM pair (as a multiple of 64 MB)
	// to the total capacity of the system
	// NOTE: 64 MB == 512 Mb, and log2(512 Mb) == 29

	total_dram_64M_multiple += (1 << (dimm_log2_num_bits.side1 - 29));

	// Configure the boundary address for the row on side 1
	pci_write_config8(PCI_DEV(0, 0, 0), DRB_ROW_0 + (dimm_index << 1),
			  total_dram_64M_multiple);

	// If the DIMMs are double-sided, add the capacity of side 2 this DIMM pair
	// (as a multiple of 64 MB) to the total capacity of the system
	if (dimm_log2_num_bits.side2 >= 29)
		total_dram_64M_multiple +=
		    (1 << (dimm_log2_num_bits.side2 - 29));

	// Configure the boundary address for the row (if any) on side 2
	pci_write_config8(PCI_DEV(0, 0, 0), DRB_ROW_1 + (dimm_index << 1),
			  total_dram_64M_multiple);

	// Update boundaries for rows subsequent to these.
	// These settings will be overridden by a subsequent call if a populated physical slot exists

	for (i = dimm_index + 1; i < MAX_DIMM_SOCKETS_PER_CHANNEL; i++) {
		pci_write_config8(PCI_DEV(0, 0, 0), DRB_ROW_0 + (i << 1),
				  total_dram_64M_multiple);
		pci_write_config8(PCI_DEV(0, 0, 0), DRB_ROW_1 + (i << 1),
				  total_dram_64M_multiple);
	}

	return total_dram_64M_multiple;
}

/**
 * Set the E7501's DRAM row boundary addresses & its Top Of Low Memory (TOLM).
 *
 * If necessary, set up a remap window so we don't waste DRAM that ordinarily
 * would lie behind addresses reserved for memory-mapped I/O.
 *
 * @param ctrl PCI addresses of memory controller functions, and SMBus
 *             addresses of DIMM slots on the mainboard.
 * @param dimm_mask Bitmask of populated DIMMs, see spd_get_supported_dimms().
 */
static void configure_e7501_ram_addresses(const struct mem_controller
					  *ctrl, uint8_t dimm_mask)
{
	int i;
	uint8_t total_dram_64M_multiple = 0;

	// Configure the E7501's DRAM row boundaries
	// Start by zeroing out the temporary initial configuration
	pci_write_config32(PCI_DEV(0, 0, 0), DRB_ROW_0, 0);
	pci_write_config32(PCI_DEV(0, 0, 0), DRB_ROW_4, 0);

	for (i = 0; i < MAX_DIMM_SOCKETS_PER_CHANNEL; i++) {

		uint16_t dimm_socket_address = ctrl->channel0[i];
		struct dimm_size sz;

		if (!(dimm_mask & (1 << i)))
			continue;	// This DIMM not present

		sz = spd_get_dimm_size(dimm_socket_address);

		RAM_DEBUG_MESSAGE("dimm size =");
		RAM_DEBUG_HEX32((u32)sz.side1);
		RAM_DEBUG_MESSAGE(" ");
		RAM_DEBUG_HEX32((u32)sz.side2);
		RAM_DEBUG_MESSAGE("\n");

		if (sz.side1 == 0)
			die("Bad SPD value\n");

		total_dram_64M_multiple =
		    configure_dimm_row_boundaries(sz, total_dram_64M_multiple, i);
	}

	// Configure the Top Of Low Memory (TOLM) in the E7501
	// This address must be a multiple of 128 MB that is less than 4 GB.
	// NOTE: 16-bit wide TOLM register stores only the highest 5 bits of a 32-bit address
	//               in the highest 5 bits.

	// We set TOLM to the smaller of 0xC0000000 (3 GB) or the total DRAM in the system.
	// This reserves addresses from 0xC0000000 - 0xFFFFFFFF for non-DRAM purposes
	// such as flash and memory-mapped I/O.

	// If there is more than 3 GB of DRAM, we define a remap window which
	// makes the DRAM "behind" the reserved region available above the top of physical
	// memory.

	// NOTE: 0xC0000000 / (64 MB) == 0x30

	if (total_dram_64M_multiple <= 0x30) {

		// <= 3 GB total RAM

		/* I should really adjust all of this in C after I have resources
		 * to all of the pci devices.
		 */

		// Round up to 128MB granularity
		// SJM: Is "missing" 64 MB of memory a potential issue? Should this round down?

		uint8_t total_dram_128M_multiple =
		    (total_dram_64M_multiple + 1) >> 1;

		// Convert to high 16 bits of address
		uint16_t top_of_low_memory =
		    total_dram_128M_multiple << 11;

		pci_write_config16(PCI_DEV(0, 0, 0), TOLM,
				   top_of_low_memory);

	} else {

		// > 3 GB total RAM

		// Set defaults for > 4 GB DRAM, i.e. remap a 1 GB (= 0x10 * 64 MB) range of memory
		uint16_t remap_base = total_dram_64M_multiple;	// A[25:0] == 0
		uint16_t remap_limit = total_dram_64M_multiple + 0x10 - 1;	// A[25:0] == 0xF

		// Put TOLM at 3 GB

		pci_write_config16(PCI_DEV(0, 0, 0), TOLM, 0xc000);

		// Define a remap window to make the RAM that would appear from 3 GB - 4 GB
		// visible just beyond 4 GB or the end of physical memory, whichever is larger
		// NOTE: 16-bit wide REMAP registers store only the highest 10 bits of a 36-bit address,
		//               (i.e. a multiple of 64 MB) in the lowest 10 bits.
		// NOTE: 0x100000000 / (64 MB) == 0x40

		if (total_dram_64M_multiple < 0x40) {
			remap_base = 0x40;	// 0x100000000
			remap_limit =
			    0x40 + (total_dram_64M_multiple - 0x30) - 1;
		}

		pci_write_config16(PCI_DEV(0, 0, 0), REMAPBASE,
				   remap_base);
		pci_write_config16(PCI_DEV(0, 0, 0), REMAPLIMIT,
				   remap_limit);
	}
}

/**
 * If we're configured to use ECC, initialize the SDRAM and clear the E7501's
 * ECC error flags.
 */
static void initialize_ecc(void)
{
	uint32_t dram_controller_mode;

	/* Test to see if ECC support is enabled */
	dram_controller_mode = pci_read_config32(PCI_DEV(0, 0, 0), DRC);
	dram_controller_mode >>= 20;
	dram_controller_mode &= 3;
	if (dram_controller_mode == 2) {

		uint8_t byte;

		RAM_DEBUG_MESSAGE("Initializing ECC state...\n");
		/* Initialize ECC bits , use ECC zero mode (new to 7501) */
		pci_write_config8(PCI_DEV(0, 0, 0), MCHCFGNS, 0x06);
		pci_write_config8(PCI_DEV(0, 0, 0), MCHCFGNS, 0x07);

		// Wait for scrub cycle to complete
		do {
			byte =
			    pci_read_config8(PCI_DEV(0, 0, 0), MCHCFGNS);
		} while ((byte & 0x08) == 0);

		pci_write_config8(PCI_DEV(0, 0, 0), MCHCFGNS, byte & 0xfc);
		RAM_DEBUG_MESSAGE("ECC state initialized.\n");

		/* Clear the ECC error bits */
		pci_write_config8(PCI_DEV(0, 0, 1), DRAM_FERR, 0x03);
		pci_write_config8(PCI_DEV(0, 0, 1), DRAM_NERR, 0x03);

		// Clear DRAM Interface error bits (write-one-clear)
		pci_write_config32(PCI_DEV(0, 0, 1), FERR_GLOBAL, 1 << 18);
		pci_write_config32(PCI_DEV(0, 0, 1), NERR_GLOBAL, 1 << 18);

		// Start normal ECC scrub
		pci_write_config8(PCI_DEV(0, 0, 0), MCHCFGNS, 5);
	}

}

/**
 * Program the DRAM Timing register (DRT) of the E7501 (except for CAS#
 * latency, which is assumed to have been programmed already), based on the
 * parameters of the various installed DIMMs.
 *
 * @param ctrl PCI addresses of memory controller functions, and SMBus
 *             addresses of DIMM slots on the mainboard.
 * @param dimm_mask Bitmask of populated DIMMs, see spd_get_supported_dimms().
 */
static void configure_e7501_dram_timing(const struct mem_controller *ctrl,
					uint8_t dimm_mask)
{
	int i;
	uint32_t dram_timing;
	int value;
	uint8_t slowest_row_precharge = 0;
	uint8_t slowest_ras_cas_delay = 0;
	uint8_t slowest_active_to_precharge_delay = 0;
	uint32_t current_cas_latency =
	    pci_read_config32(PCI_DEV(0, 0, 0), DRT) & DRT_CAS_MASK;

	// CAS# latency must be programmed beforehand
	ASSERT((current_cas_latency == DRT_CAS_2_0)
	       || (current_cas_latency == DRT_CAS_2_5));

	// Each timing parameter is determined by the slowest DIMM

	for (i = 0; i < MAX_DIMM_SOCKETS; i++) {
		uint16_t dimm_socket_address;

		if (!(dimm_mask & (1 << i)))
			continue;	// This DIMM not present

		if (i < MAX_DIMM_SOCKETS_PER_CHANNEL)
			dimm_socket_address = ctrl->channel0[i];
		else
			dimm_socket_address =
			    ctrl->channel1[i - MAX_DIMM_SOCKETS_PER_CHANNEL];

		value =
		    spd_read_byte(dimm_socket_address,
				  SPD_MIN_ROW_PRECHARGE_TIME);
		if (value < 0)
			goto hw_err;
		if (value > slowest_row_precharge)
			slowest_row_precharge = value;

		value =
		    spd_read_byte(dimm_socket_address,
				  SPD_MIN_RAS_TO_CAS_DELAY);
		if (value < 0)
			goto hw_err;
		if (value > slowest_ras_cas_delay)
			slowest_ras_cas_delay = value;

		value =
		    spd_read_byte(dimm_socket_address,
				  SPD_MIN_ACTIVE_TO_PRECHARGE_DELAY);
		if (value < 0)
			goto hw_err;
		if (value > slowest_active_to_precharge_delay)
			slowest_active_to_precharge_delay = value;
	}

	// NOTE for timing parameters:
	//              At 133 MHz, 1 clock == 7.52 ns

	/* Read the initial state */
	dram_timing = pci_read_config32(PCI_DEV(0, 0, 0), DRT);

	/* Trp */

	// E7501 supports only 2 or 3 clocks for tRP
	if (slowest_row_precharge > ((22 << 2) | (2 << 0)))
		die("unsupported DIMM tRP");	// > 22.5 ns: 4 or more clocks
	else if (slowest_row_precharge > (15 << 2))
		dram_timing &= ~(1 << 0);	// > 15.0 ns: 3 clocks
	else
		dram_timing |= (1 << 0);	// <= 15.0 ns: 2 clocks

	/*  Trcd */

	// E7501 supports only 2 or 3 clocks for tRCD
	// Use the same value for both read & write
	dram_timing &= ~((1 << 3) | (3 << 1));
	if (slowest_ras_cas_delay > ((22 << 2) | (2 << 0)))
		die("unsupported DIMM tRCD");	// > 22.5 ns: 4 or more clocks
	else if (slowest_ras_cas_delay > (15 << 2))
		dram_timing |= (2 << 1);	// > 15.0 ns: 3 clocks
	else
		dram_timing |= ((1 << 3) | (3 << 1));	// <= 15.0 ns: 2 clocks

	/* Tras */

	// E7501 supports only 5, 6, or 7 clocks for tRAS
	// 5 clocks ~= 37.6 ns, 6 clocks ~= 45.1 ns, 7 clocks ~= 52.6 ns
	dram_timing &= ~(3 << 9);

	if (slowest_active_to_precharge_delay > 52)
		die("unsupported DIMM tRAS");	// > 52 ns:      8 or more clocks
	else if (slowest_active_to_precharge_delay > 45)
		dram_timing |= (0 << 9);	// 46-52 ns: 7 clocks
	else if (slowest_active_to_precharge_delay > 37)
		dram_timing |= (1 << 9);	// 38-45 ns: 6 clocks
	else
		dram_timing |= (2 << 9);	// < 38 ns:      5 clocks

	/* Trd */

	/* Set to a 7 clock read delay. This is for 133Mhz
	 *  with a CAS latency of 2.5  if 2.0 a 6 clock
	 *  delay is good  */

	dram_timing &= ~(7 << 24);	// 7 clocks
	if (current_cas_latency == DRT_CAS_2_0)
		dram_timing |= (1 << 24);	// 6 clocks

	/*
	 * Back to Back Read-Write Turn Around
	 */
	/* Set to a 5 clock back to back read to write turn around.
	 *  4 is a good delay if the CAS latency is 2.0 */

	dram_timing &= ~(1 << 28);	// 5 clocks
	if (current_cas_latency == DRT_CAS_2_0)
		dram_timing |= (1 << 28);	// 4 clocks

	pci_write_config32(PCI_DEV(0, 0, 0), DRT, dram_timing);

	return;

      hw_err:
	die(SPD_ERROR);
}

/**
 * Determine the shortest CAS# latency that the E7501 and all DIMMs have in
 * common, and program the E7501 to use it.
 *
 * @param ctrl PCI addresses of memory controller functions, and SMBus
 *             addresses of DIMM slots on the mainboard.
 * @param dimm_mask Bitmask of populated DIMMs, spd_get_supported_dimms().
 */
static void configure_e7501_cas_latency(const struct mem_controller *ctrl,
					uint8_t dimm_mask)
{
	int i;
	int value;
	uint32_t dram_timing;
	uint16_t maybe_dram_read_timing;
	uint32_t dword;

	// CAS# latency bitmasks in SPD_ACCEPTABLE_CAS_LATENCIES format
	// NOTE: E7501 supports only 2.0 and 2.5
	uint32_t system_compatible_cas_latencies =
	    SPD_CAS_LATENCY_2_0 | SPD_CAS_LATENCY_2_5;
	uint32_t current_cas_latency;
	uint32_t dimm_compatible_cas_latencies;

	for (i = 0; i < MAX_DIMM_SOCKETS; i++) {

		uint16_t dimm_socket_address;

		if (!(dimm_mask & (1 << i)))
			continue;	// This DIMM not usable

		if (i < MAX_DIMM_SOCKETS_PER_CHANNEL)
			dimm_socket_address = ctrl->channel0[i];
		else
			dimm_socket_address =
			    ctrl->channel1[i - MAX_DIMM_SOCKETS_PER_CHANNEL];

		value =
		    spd_read_byte(dimm_socket_address,
				  SPD_ACCEPTABLE_CAS_LATENCIES);
		if (value < 0)
			goto hw_err;

		dimm_compatible_cas_latencies = value & 0x7f;	// Start with all supported by DIMM
		current_cas_latency = 1 << log2(dimm_compatible_cas_latencies);	// Max supported by DIMM

		// Can we support the highest CAS# latency?

		value =
		    spd_read_byte(dimm_socket_address,
				  SPD_MIN_CYCLE_TIME_AT_CAS_MAX);
		if (value < 0)
			goto hw_err;

		// NOTE: At 133 MHz, 1 clock == 7.52 ns
		if (value > 0x75) {
			// Our bus is too fast for this CAS# latency
			// Remove it from the bitmask of those supported by the DIMM that are compatible
			dimm_compatible_cas_latencies &= ~current_cas_latency;
		}
		// Can we support the next-highest CAS# latency (max - 0.5)?

		current_cas_latency >>= 1;
		if (current_cas_latency != 0) {
			value =
			    spd_read_byte(dimm_socket_address,
					  SPD_SDRAM_CYCLE_TIME_2ND);
			if (value < 0)
				goto hw_err;
			if (value > 0x75)
				dimm_compatible_cas_latencies &=
				    ~current_cas_latency;
		}
		// Can we support the next-highest CAS# latency (max - 1.0)?
		current_cas_latency >>= 1;
		if (current_cas_latency != 0) {
			value =
			    spd_read_byte(dimm_socket_address,
					  SPD_SDRAM_CYCLE_TIME_3RD);
			if (value < 0)
				goto hw_err;
			if (value > 0x75)
				dimm_compatible_cas_latencies &=
				    ~current_cas_latency;
		}
		// Restrict the system to CAS# latencies compatible with this DIMM
		system_compatible_cas_latencies &=
		    dimm_compatible_cas_latencies;

		/* go to the next DIMM */
	}

	/* After all of the arduous calculation setup with the fastest
	 * cas latency I can use.
	 */

	dram_timing = pci_read_config32(PCI_DEV(0, 0, 0), DRT);
	dram_timing &= ~(DRT_CAS_MASK);

	maybe_dram_read_timing =
	    pci_read_config16(PCI_DEV(0, 0, 0), MAYBE_DRDCTL);
	maybe_dram_read_timing &= 0xF00C;

	if (system_compatible_cas_latencies & SPD_CAS_LATENCY_2_0) {
		dram_timing |= DRT_CAS_2_0;
		maybe_dram_read_timing |= 0xBB1;
	} else if (system_compatible_cas_latencies & SPD_CAS_LATENCY_2_5) {

		uint32_t dram_row_attributes =
		    pci_read_config32(PCI_DEV(0, 0, 0), DRA);

		dram_timing |= DRT_CAS_2_5;

		// At CAS# 2.5, DRAM Read Timing (if that's what it its) appears to need a slightly
		// different value if all DIMM slots are populated

		if ((dram_row_attributes & 0xff)
		    && (dram_row_attributes & 0xff00)
		    && (dram_row_attributes & 0xff0000)
		    && (dram_row_attributes & 0xff000000)) {

			// All slots populated
			maybe_dram_read_timing |= 0x0882;
		} else {
			// Some unpopulated slots
			maybe_dram_read_timing |= 0x0662;
		}
	} else
		die("No CAS# latencies compatible with all DIMMs!!\n");

	pci_write_config32(PCI_DEV(0, 0, 0), DRT, dram_timing);

	/* set master DLL reset */
	dword = pci_read_config32(PCI_DEV(0, 0, 0), 0x88);
	dword |= (1 << 26);
	pci_write_config32(PCI_DEV(0, 0, 0), 0x88, dword);

	dword &= 0x0c0007ff;	/* patch try register 88 is undocumented tnz */
	dword |= 0xd2109800;

	pci_write_config32(PCI_DEV(0, 0, 0), 0x88, dword);

	pci_write_config16(PCI_DEV(0, 0, 0), MAYBE_DRDCTL,
			   maybe_dram_read_timing);

	dword = pci_read_config32(PCI_DEV(0, 0, 0), 0x88);	/* reset master DLL reset */
	dword &= ~(1 << 26);
	pci_write_config32(PCI_DEV(0, 0, 0), 0x88, dword);

	return;

      hw_err:
	die(SPD_ERROR);
}

/**
 * Configure the refresh interval so that we refresh no more often than
 * required by the "most needy" DIMM. Also disable ECC if any of the DIMMs
 * don't support it.
 *
 * @param ctrl PCI addresses of memory controller functions, and SMBus
 *             addresses of DIMM slots on the mainboard.
 * @param dimm_mask Bitmask of populated DIMMs, spd_get_supported_dimms().
 */
static void configure_e7501_dram_controller_mode(const struct
						 mem_controller *ctrl,
						 uint8_t dimm_mask)
{
	int i;

	// Initial settings
	uint32_t controller_mode =
	    pci_read_config32(PCI_DEV(0, 0, 0), DRC);
	uint32_t system_refresh_mode = (controller_mode >> 8) & 7;

	// Code below assumes that most aggressive settings are in
	// force when we are called, either via E7501 reset defaults
	// or by sdram_set_registers():
	//      - ECC enabled
	//      - No refresh

	ASSERT((controller_mode & (3 << 20)) == (2 << 20));	// ECC
	ASSERT(!(controller_mode & (7 << 8)));	// Refresh

	/* Walk through _all_ dimms and find the least-common denominator for:
	 *  - ECC support
	 *  - refresh rates
	 */

	for (i = 0; i < MAX_DIMM_SOCKETS; i++) {

		uint32_t dimm_refresh_mode;
		int value;
		uint16_t dimm_socket_address;

		if (!(dimm_mask & (1 << i))) {
			continue;	// This DIMM not usable
		}

		if (i < MAX_DIMM_SOCKETS_PER_CHANNEL)
			dimm_socket_address = ctrl->channel0[i];
		else
			dimm_socket_address =
			    ctrl->channel1[i -
					   MAX_DIMM_SOCKETS_PER_CHANNEL];

		// Disable ECC mode if any one of the DIMMs does not support ECC
		// SJM: Should we just die here? E7501 datasheet says non-ECC DIMMs aren't supported.

		value =
		    spd_read_byte(dimm_socket_address,
				  SPD_DIMM_CONFIG_TYPE);
		die_on_spd_error(value);
		if (value != ERROR_SCHEME_ECC) {
			controller_mode &= ~(3 << 20);
		}

		value = spd_read_byte(dimm_socket_address, SPD_REFRESH);
		die_on_spd_error(value);
		value &= 0x7f;	// Mask off self-refresh bit
		if (value > MAX_SPD_REFRESH_RATE) {
			print_err("unsupported refresh rate\n");
			continue;
		}
		// Get the appropriate E7501 refresh mode for this DIMM
		dimm_refresh_mode = refresh_rate_map[value];
		if (dimm_refresh_mode > 7) {
			print_err("unsupported refresh rate\n");
			continue;
		}
		// If this DIMM requires more frequent refresh than others,
		// update the system setting
		if (refresh_frequency[dimm_refresh_mode] >
		    refresh_frequency[system_refresh_mode])
			system_refresh_mode = dimm_refresh_mode;

#ifdef SUSPICIOUS_LOOKING_CODE
// SJM NOTE: This code doesn't look right. SPD values are an order of magnitude smaller
//                       than the clock period of the memory controller. Also, no other northbridge
//                       looks at SPD_CMD_SIGNAL_INPUT_HOLD_TIME.

		// Switch to 2 clocks for address/command if required by any one of the DIMMs
		// NOTE: At 133 MHz, 1 clock == 7.52 ns
		value =
		    spd_read_byte(dimm_socket_address,
				  SPD_CMD_SIGNAL_INPUT_HOLD_TIME);
		die_on_spd_error(value);
		if (value >= 0xa0) {	/* At 133MHz this constant should be 0x75 */
			controller_mode &= ~(1 << 16);	/* Use two clock cyles instead of one */
		}
#endif

		/* go to the next DIMM */
	}

	controller_mode |= (system_refresh_mode << 8);

	// Configure the E7501
	pci_write_config32(PCI_DEV(0, 0, 0), DRC, controller_mode);
}

/**
 * Configure the E7501's DRAM Row Attributes (DRA) registers based on DIMM
 * parameters read via SPD. This tells the controller the width of the SDRAM
 * chips on each DIMM side (x4 or x8) and the page size of each DIMM side
 * (4, 8, 16, or 32 KB).
 *
 * @param ctrl PCI addresses of memory controller functions, and SMBus
 *             addresses of DIMM slots on the mainboard.
 * @param dimm_mask Bitmask of populated DIMMs, spd_get_supported_dimms().
 */
static void configure_e7501_row_attributes(const struct mem_controller
					   *ctrl, uint8_t dimm_mask)
{
	int i;
	uint32_t row_attributes = 0;

	for (i = 0; i < MAX_DIMM_SOCKETS_PER_CHANNEL; i++) {

		uint16_t dimm_socket_address = ctrl->channel0[i];
		struct dimm_size page_size;
		struct dimm_size sdram_width;

		if (!(dimm_mask & (1 << i)))
			continue;	// This DIMM not usable

		// Get the relevant parameters via SPD
		page_size = sdram_spd_get_page_size(dimm_socket_address);
		sdram_width = sdram_spd_get_width(dimm_socket_address);

		// Update the DRAM Row Attributes.
		// Page size is encoded as log2(page size in bits) - log2(8 Kb)
		// NOTE: 8 Kb = 2^13
		row_attributes |= (page_size.side1 - 13) << (i << 3);	// Side 1 of each DIMM is an EVEN row

		if (sdram_width.side2 > 0)
			row_attributes |= (page_size.side2 - 13) << ((i << 3) + 4);	// Side 2 is ODD

		// Set x4 flags if appropriate
		if (sdram_width.side1 == 4) {
			row_attributes |= 0x08 << (i << 3);
		}

		if (sdram_width.side2 == 4) {
			row_attributes |= 0x08 << ((i << 3) + 4);
		}

		/* go to the next DIMM */
	}

	/* Write the new row attributes register */
	pci_write_config32(PCI_DEV(0, 0, 0), DRA, row_attributes);
}

/*
 * Enable clock signals for populated DIMM sockets and disable them for
 * unpopulated sockets (to reduce EMI).
 *
 * @param dimm_mask Bitmask of populated DIMMs, see spd_get_supported_dimms().
 */
static void enable_e7501_clocks(uint8_t dimm_mask)
{
	int i;
	uint8_t clock_disable = pci_read_config8(PCI_DEV(0, 0, 0), CKDIS);

	for (i = 0; i < MAX_DIMM_SOCKETS_PER_CHANNEL; i++) {

		uint8_t socket_mask = 1 << i;

		if (dimm_mask & socket_mask)
			clock_disable &= ~socket_mask;	// DIMM present, enable clock
		else
			clock_disable |= socket_mask;	// DIMM absent, disable clock
	}

	pci_write_config8(PCI_DEV(0, 0, 0), CKDIS, clock_disable);
}

/* DIMM-dedependent configuration functions */

/**
 * DDR Receive FIFO RE-Sync (?)
 */
static void RAM_RESET_DDR_PTR(void)
{
	uint8_t byte;
	byte = pci_read_config8(PCI_DEV(0, 0, 0), 0x88);
	byte |= (1 << 4);
	pci_write_config8(PCI_DEV(0, 0, 0), 0x88, byte);

	byte = pci_read_config8(PCI_DEV(0, 0, 0), 0x88);
	byte &= ~(1 << 4);
	pci_write_config8(PCI_DEV(0, 0, 0), 0x88, byte);
}

/**
 * Set E7501 registers that are either independent of DIMM specifics, or
 * establish default settings that will be overridden when we learn the
 * specifics.
 *
 * This sets PCI configuration registers to known good values based on the
 * table 'constant_register_values', which are a triple of configuration
 * register offset, mask, and bits to set.
 */
static void ram_set_d0f0_regs(void)
{
	int i;
	int num_values = ARRAY_SIZE(constant_register_values);

	ASSERT((num_values % 3) == 0);	// Bad table?

	for (i = 0; i < num_values; i += 3) {

		uint32_t register_offset = constant_register_values[i];
		uint32_t bits_to_mask = constant_register_values[i + 1];
		uint32_t bits_to_set = constant_register_values[i + 2];
		uint32_t register_value;

		// It's theoretically possible to set values for something other than D0:F0,
		// but it's not typically done here
		ASSERT(!(register_offset & 0xFFFFFF00));

		// bits_to_mask and bits_to_set should not reference the same bits
		// Again, not strictly an error, but flagged as a potential bug
		ASSERT((bits_to_mask & bits_to_set) == 0);

		register_value =
		    pci_read_config32(PCI_DEV(0, 0, 0), register_offset);
		register_value &= bits_to_mask;
		register_value |= bits_to_set;

		pci_write_config32(PCI_DEV(0, 0, 0), register_offset,
				   register_value);
	}
}

/**
 * Copy 64 bytes from one location to another.
 *
 * @param src_addr TODO
 * @param dst_addr TODO
 */
static void write_8dwords(const uint32_t *src_addr, uint32_t dst_addr)
{
	int i;
	for (i = 0; i < 8; i++) {
		write32(dst_addr, *src_addr);
		src_addr++;
		dst_addr += sizeof(uint32_t);
	}
}

/**
 * Set the E7501's (undocumented) RCOMP registers.
 *
 * Per the 855PM datasheet and IXP2800 HW Initialization Reference Manual,
 * RCOMP registers appear to affect drive strength, pullup/pulldown offset,
 * and slew rate of various signal groups.
 *
 * Comments below are conjecture based on apparent similarity between the
 * E7501 and these two chips.
 */
static void ram_set_rcomp_regs(void)
{
	uint32_t dword;
	uint8_t maybe_strength_control;

	RAM_DEBUG_MESSAGE("Setting RCOMP registers.\n");

	/*enable access to the rcomp bar */
	dword = pci_read_config32(PCI_DEV(0, 0, 0), MAYBE_MCHTST);
	dword |= (1 << 22);
	pci_write_config32(PCI_DEV(0, 0, 0), MAYBE_MCHTST, dword);

	// Set the RCOMP MMIO base address
	pci_write_config32(PCI_DEV(0, 0, 0), MAYBE_SMRBASE, RCOMP_MMIO);

	// Block RCOMP updates while we configure the registers
	dword = read32(RCOMP_MMIO + MAYBE_SMRCTL);
	dword |= (1 << 9);
	write32(RCOMP_MMIO + MAYBE_SMRCTL, dword);

	/* Begin to write the RCOMP registers */

	// Set CMD and DQ/DQS strength to 2x (?)
	maybe_strength_control = read8(RCOMP_MMIO + MAYBE_DQCMDSTR) & 0x88;
	maybe_strength_control |= 0x44;
	write8(RCOMP_MMIO + MAYBE_DQCMDSTR, maybe_strength_control);

	write_8dwords(maybe_2x_slew_table, RCOMP_MMIO + 0x80);
	write16(RCOMP_MMIO + 0x42, 0);

	write_8dwords(maybe_1x_slew_table, RCOMP_MMIO + 0x60);

	// NOTE: some factory BIOS set 0x9088 here. Seems to work either way.
	write16(RCOMP_MMIO + 0x40, 0);

	// Set RCVEnOut# strength to 2x (?)
	maybe_strength_control = read8(RCOMP_MMIO + MAYBE_RCVENSTR) & 0xF8;
	maybe_strength_control |= 4;
	write8(RCOMP_MMIO + MAYBE_RCVENSTR, maybe_strength_control);

	write_8dwords(maybe_2x_slew_table, RCOMP_MMIO + 0x1c0);
	write16(RCOMP_MMIO + 0x50, 0);

	// Set CS# strength for x4 SDRAM to 2x (?)
	maybe_strength_control = read8(RCOMP_MMIO + MAYBE_CSBSTR) & 0xF8;
	maybe_strength_control |= 4;
	write8(RCOMP_MMIO + MAYBE_CSBSTR, maybe_strength_control);

	write_8dwords(maybe_2x_slew_table, RCOMP_MMIO + 0x140);
	write16(RCOMP_MMIO + 0x48, 0);

	// Set CKE strength for x4 SDRAM to 2x (?)
	maybe_strength_control = read8(RCOMP_MMIO + MAYBE_CKESTR) & 0xF8;
	maybe_strength_control |= 4;
	write8(RCOMP_MMIO + MAYBE_CKESTR, maybe_strength_control);

	write_8dwords(maybe_2x_slew_table, RCOMP_MMIO + 0xa0);
	write16(RCOMP_MMIO + 0x44, 0);

	// Set CK strength for x4 SDRAM to 1x (?)
	maybe_strength_control = read8(RCOMP_MMIO + MAYBE_CKSTR) & 0xF8;
	maybe_strength_control |= 1;
	write8(RCOMP_MMIO + MAYBE_CKSTR, maybe_strength_control);

	write_8dwords(maybe_pull_updown_offset_table, RCOMP_MMIO + 0x180);
	write16(RCOMP_MMIO + 0x4c, 0);

	write8(RCOMP_MMIO + 0x2c, 0xff);

	// Set the digital filter length to 8 (?)
	dword = read32(RCOMP_MMIO + MAYBE_SMRCTL);

	// NOTE: Some factory BIOS don't do this.
	//               Doesn't seem to matter either way.
	dword &= ~2;

	dword |= 1;
	write32(RCOMP_MMIO + MAYBE_SMRCTL, dword);

	/* Wait 40 usec */
	SLOW_DOWN_IO;

	/* unblock updates */
	dword = read32(RCOMP_MMIO + MAYBE_SMRCTL);
	dword &= ~(1 << 9);
	write32(RCOMP_MMIO + MAYBE_SMRCTL, dword);

	// Force a RCOMP measurement cycle?
	dword |= (1 << 8);
	write32(RCOMP_MMIO + MAYBE_SMRCTL, dword);
	dword &= ~(1 << 8);
	write32(RCOMP_MMIO + MAYBE_SMRCTL, dword);

	/* Wait 40 usec */
	SLOW_DOWN_IO;

	/*disable access to the rcomp bar */
	dword = pci_read_config32(PCI_DEV(0, 0, 0), MAYBE_MCHTST);
	dword &= ~(1 << 22);
	pci_write_config32(PCI_DEV(0, 0, 0), MAYBE_MCHTST, dword);

}

/*-----------------------------------------------------------------------------
Public interface:
-----------------------------------------------------------------------------*/

/**
 * Go through the JEDEC initialization sequence for all DIMMs, then enable
 * refresh and initialize ECC and memory to zero. Upon exit, SDRAM is up
 * and running.
 *
 * @param controllers Not used.
 * @param ctrl PCI addresses of memory controller functions, and SMBus
 *             addresses of DIMM slots on the mainboard.
 */
static void sdram_enable(int controllers,
			 const struct mem_controller *ctrl)
{
	uint8_t dimm_mask = pci_read_config16(PCI_DEV(0, 0, 0), SKPD);
	uint32_t dram_controller_mode;

	if (dimm_mask == 0)
		return;

	/* 1 & 2 Power up and start clocks */
	RAM_DEBUG_MESSAGE("Ram Enable 1\n");
	RAM_DEBUG_MESSAGE("Ram Enable 2\n");

	/* A 200us delay is needed */
	DO_DELAY; EXTRA_DELAY;

	/* 3. Apply NOP */
	RAM_DEBUG_MESSAGE("Ram Enable 3\n");
	do_ram_command(RAM_COMMAND_NOP, 0);
	EXTRA_DELAY;

	/* 4 Precharge all */
	RAM_DEBUG_MESSAGE("Ram Enable 4\n");
	do_ram_command(RAM_COMMAND_PRECHARGE, 0);
	EXTRA_DELAY;
	/* wait until the all banks idle state... */

	/* 5. Issue EMRS to enable DLL */
	RAM_DEBUG_MESSAGE("Ram Enable 5\n");
	do_ram_command(RAM_COMMAND_EMRS,
		       SDRAM_EXTMODE_DLL_ENABLE |
		       SDRAM_EXTMODE_DRIVE_NORMAL);
	EXTRA_DELAY;

	/* 6. Reset DLL */
	RAM_DEBUG_MESSAGE("Ram Enable 6\n");
	set_ram_mode(E7501_SDRAM_MODE | SDRAM_MODE_DLL_RESET);
	EXTRA_DELAY;
	/* Ensure a 200us delay between the DLL reset in step 6 and the final
	 * mode register set in step 9.
	 * Infineon needs this before any other command is sent to the ram.
	 */
	DO_DELAY; EXTRA_DELAY;

	/* 7 Precharge all */
	RAM_DEBUG_MESSAGE("Ram Enable 7\n");
	do_ram_command(RAM_COMMAND_PRECHARGE, 0);
	EXTRA_DELAY;

	/* 8 Now we need 2 AUTO REFRESH / CBR cycles to be performed */
	RAM_DEBUG_MESSAGE("Ram Enable 8\n");
	do_ram_command(RAM_COMMAND_CBR, 0);
	EXTRA_DELAY;
	do_ram_command(RAM_COMMAND_CBR, 0);
	EXTRA_DELAY;

	/* And for good luck 6 more CBRs */
	do_ram_command(RAM_COMMAND_CBR, 0);
	EXTRA_DELAY;
	do_ram_command(RAM_COMMAND_CBR, 0);
	EXTRA_DELAY;
	do_ram_command(RAM_COMMAND_CBR, 0);
	EXTRA_DELAY;
	do_ram_command(RAM_COMMAND_CBR, 0);
	EXTRA_DELAY;
	do_ram_command(RAM_COMMAND_CBR, 0);
	EXTRA_DELAY;
	do_ram_command(RAM_COMMAND_CBR, 0);
	EXTRA_DELAY;

	/* 9 mode register set */
	RAM_DEBUG_MESSAGE("Ram Enable 9\n");
	set_ram_mode(E7501_SDRAM_MODE | SDRAM_MODE_NORMAL);
	EXTRA_DELAY;

	/* 10 DDR Receive FIFO RE-Sync */
	RAM_DEBUG_MESSAGE("Ram Enable 10\n");
	RAM_RESET_DDR_PTR();
	EXTRA_DELAY;

	/* 11 normal operation */
	RAM_DEBUG_MESSAGE("Ram Enable 11\n");
	do_ram_command(RAM_COMMAND_NORMAL, 0);
	EXTRA_DELAY;

	// Reconfigure the row boundaries and Top of Low Memory
	// to match the true size of the DIMMs
	configure_e7501_ram_addresses(ctrl, dimm_mask);

	/* Finally enable refresh */
	dram_controller_mode = pci_read_config32(PCI_DEV(0, 0, 0), DRC);
	dram_controller_mode |= (1 << 29);
	pci_write_config32(PCI_DEV(0, 0, 0), DRC, dram_controller_mode);
	EXTRA_DELAY;
	initialize_ecc();

	dram_controller_mode = pci_read_config32(PCI_DEV(0, 0, 0), DRC);	/* FCS_EN */
	dram_controller_mode |= (1 << 17);	// NOTE: undocumented reserved bit
	pci_write_config32(PCI_DEV(0, 0, 0), DRC, dram_controller_mode);

	RAM_DEBUG_MESSAGE("Northbridge following SDRAM init:\n");
	DUMPNORTH();
}

/**
 * Configure SDRAM controller parameters that depend on characteristics of the
 * DIMMs installed in the system. These characteristics are read from the
 * DIMMs via the standard Serial Presence Detect (SPD) interface.
 *
 * @param ctrl PCI addresses of memory controller functions, and SMBus
 *             addresses of DIMM slots on the mainboard.
 */
static void sdram_set_spd_registers(const struct mem_controller *ctrl)
{
	uint8_t dimm_mask;

	RAM_DEBUG_MESSAGE("Reading SPD data...\n");

	dimm_mask = spd_get_supported_dimms(ctrl);

	if (dimm_mask == 0) {
		print_debug("No usable memory for this controller\n");
	} else {
		enable_e7501_clocks(dimm_mask);

		RAM_DEBUG_MESSAGE("setting based on SPD data...\n");

		configure_e7501_row_attributes(ctrl, dimm_mask);
		configure_e7501_dram_controller_mode(ctrl, dimm_mask);
		configure_e7501_cas_latency(ctrl, dimm_mask);
		RAM_RESET_DDR_PTR();

		configure_e7501_dram_timing(ctrl, dimm_mask);
		DO_DELAY;
		RAM_DEBUG_MESSAGE("done\n");
	}

	/* NOTE: configure_e7501_ram_addresses() is NOT called here.
	 * We want to keep the default 64 MB/row mapping until sdram_enable() is called,
	 * even though the default mapping is almost certainly incorrect.
	 * The default mapping makes it easy to initialize all of the DIMMs
	 * even if the total system memory is > 4 GB.
	 *
	 * Save the dimm_mask for when sdram_enable is called, so it can call
	 * configure_e7501_ram_addresses() without having to regenerate the bitmask
	 * of usable DIMMs.
	 */
	pci_write_config16(PCI_DEV(0, 0, 0), SKPD, dimm_mask);
}

/**
 * Do basic RAM setup that does NOT depend on serial presence detect
 * information (i.e. independent of DIMM specifics).
 *
 * @param ctrl PCI addresses of memory controller functions, and SMBus
 *             addresses of DIMM slots on the mainboard.
 */
static void sdram_set_registers(const struct mem_controller *ctrl)
{
	RAM_DEBUG_MESSAGE("Northbridge prior to SDRAM init:\n");
	DUMPNORTH();

	ram_set_rcomp_regs();
	ram_set_d0f0_regs();
}
