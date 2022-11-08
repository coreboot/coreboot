/* SPDX-License-Identifier: GPL-2.0-only */

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

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <lib.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <assert.h>
#include <spd.h>
#include <sdram_mode.h>
#include <timestamp.h>

#include "raminit.h"
#include "e7505.h"

/*-----------------------------------------------------------------------------
Definitions:
-----------------------------------------------------------------------------*/

#if CONFIG(DEBUG_RAM_SETUP)
#define RAM_DEBUG_MESSAGE(x)	printk(BIOS_DEBUG, x)
#define RAM_DEBUG_HEX32(x)	printk(BIOS_DEBUG, "%08x", x)
#define RAM_DEBUG_HEX8(x)	printk(BIOS_DEBUG, "%02x", x)
#else
#define RAM_DEBUG_MESSAGE(x)
#define RAM_DEBUG_HEX32(x)
#define RAM_DEBUG_HEX8(x)
#endif

#define E7501_SDRAM_MODE	(SDRAM_BURST_INTERLEAVED | SDRAM_BURST_4)
#define SPD_ERROR		"Error reading SPD info\n"

#define MCHDEV		PCI_DEV(0, 0, 0)
#define RASDEV		PCI_DEV(0, 0, 1)
#define AGPDEV		PCI_DEV(0, 1, 0)
#define D060DEV		PCI_DEV(0, 6, 0)

// NOTE: This used to be 0x100000.
//       That doesn't work on systems where A20M# is asserted, because
//       attempts to access 0x1000NN end up accessing 0x0000NN.
#define RCOMP_MMIO ((u8 *)0x200000)

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

#define MAX_SPD_REFRESH_RATE (ARRAY_SIZE(refresh_rate_map) - 1)

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

	/* Comments here are remains of e7501 or even 855PM.
	 * They might be partially (in)correct for e7505.
	 */

	/* (DRAM Read Timing Control, if similar to 855PM?)
	 * 0x80 - 0x81   documented differently for e7505
	 * This register has something to do with CAS latencies,
	 * possibly this is the real chipset control.
	 * At 0x00 CAS latency 1.5 works.
	 * At 0x06 CAS latency 2.5 works.
	 * At 0x01 CAS latency 2.0 works.
	 *
	 * This is still undocumented in e7501, but with different values
	 * CAS 2.0 values taken from Intel BIOS settings, others are a guess
	 * and may be terribly wrong. Old values preserved as comments until I
	 * figure this out for sure.
	 * e7501 docs claim that CAS1.5 is unsupported, so it may or may not
	 * work at all.
	 * Steven James 02/06/2003
	 *
	 * NOTE: values now configured in configure_e7501_cas_latency() based
	 *       on SPD info and total number of DIMMs (per Intel)
	 */

	/* FDHC - Fixed DRAM Hole Control  ???
	 * 0x58  undocumented for e7505, memory hole in southbridge configuration?
	 * [7:7] Hole_Enable
	 *       0 == No memory Hole
	 *       1 == Memory Hole from 15MB to 16MB
	 * [6:0] Reserved
	 */

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

/* DDR RECOMP tables */
// Slew table for 2x drive?
static const uint32_t slew_2x[] = {
	0x00000000, 0x76543210, 0xffffeca8, 0xffffffff,
	0x21000000, 0xa8765432, 0xffffffec, 0xffffffff,
};

// Pull Up / Pull Down offset table, if analogous to IXP2800?
static const uint32_t pull_updown_offset_table[] = {
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0x88888888, 0x88888888, 0x88888888, 0x88888888,
};

/*-----------------------------------------------------------------------------
Delay functions:
-----------------------------------------------------------------------------*/

/* Estimate that SLOW_DOWN_IO takes about 1 us */
#define SLOW_DOWN_IO inb(0x80)
static void local_udelay(int i)
{
	while (i--) {
		SLOW_DOWN_IO;
	}
}

/* delay for 200us */
#define DO_DELAY local_udelay(200)
#define EXTRA_DELAY DO_DELAY

/*-----------------------------------------------------------------------------
Handle (undocumented) control bits MCHTST and PCI_DEV(0,6,0)
-----------------------------------------------------------------------------*/
typedef enum {
	MCHTST_CMD_0,
	D060_ENABLE,
	D060_DISABLE,
	RCOMP_BAR_ENABLE,
	RCOMP_BAR_DISABLE,
} mchtst_cc;

typedef enum {
	D060_CMD_0,
	D060_CMD_1,
} d060_cc;

typedef enum {
	RCOMP_HOLD,
	RCOMP_RELEASE,
	RCOMP_SMR_00,
	RCOMP_SMR_01,
} rcomp_smr_cc;

/**
 * MCHTST - 0xF4 - 0xF7     --   Based on similarity to 855PM
 *
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
static void mchtest_control(mchtst_cc cmd)
{
	uint32_t dword = pci_read_config32(MCHDEV, MCHTST);
	switch (cmd) {
	case MCHTST_CMD_0:
		dword &= ~(3 << 30);
		break;
	case RCOMP_BAR_ENABLE:
		dword |= (1 << 22);
		break;
	case RCOMP_BAR_DISABLE:
		dword &= ~(1 << 22);
		break;
	case D060_ENABLE:
		dword |= (1 << 1);
		break;
	case D060_DISABLE:
		dword &= ~(1 << 1);
		break;
	};
	pci_write_config32(MCHDEV, MCHTST, dword);
}

/**
 *
 */
static void d060_control(d060_cc cmd)
{
	mchtest_control(D060_ENABLE);
	uint32_t dword = pci_read_config32(D060DEV, 0xf0);
	switch (cmd) {
	case D060_CMD_0:
		dword |= (1 << 2);
		break;
	case D060_CMD_1:
		dword |= (3 << 27);
		break;
	}
	pci_write_config32(D060DEV, 0xf0, dword);
	mchtest_control(D060_DISABLE);
}

/**
 *
 */
static void rcomp_smr_control(rcomp_smr_cc cmd)
{
	uint32_t dword = read32(RCOMP_MMIO + SMRCTL);
	switch (cmd) {
	case RCOMP_HOLD:
		dword |= (1 << 9);
		break;
	case RCOMP_RELEASE:
		dword &= ~((1 << 9) | (3 << 0));
		dword |= (1 << 10) | (1 << 0);
		break;
	case RCOMP_SMR_00:
		dword &= ~(1 << 8);
		break;
	case RCOMP_SMR_01:
		dword |= (1 << 10) | (1 << 8);
		break;
	}
	write32(RCOMP_MMIO + SMRCTL, dword);
}

/*-----------------------------------------------------------------------------
Serial presence detect (SPD) functions:
-----------------------------------------------------------------------------*/

static void die_on_spd_error(int spd_return_value)
{
	if (spd_return_value < 0)
		die("Error reading SPD info\n");
}

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
	value = smbus_read_byte(dimm_socket_address, SPD_NUM_COLUMNS);
	if (value < 0)
		goto hw_err;
	pgsz.side1 = value & 0xf;	// # columns in bank 1

	/* Get the module data width and convert it to a power of two */
	value = smbus_read_byte(dimm_socket_address, SPD_MODULE_DATA_WIDTH_MSB);
	if (value < 0)
		goto hw_err;
	module_data_width = (value & 0xff) << 8;

	value = smbus_read_byte(dimm_socket_address, SPD_MODULE_DATA_WIDTH_LSB);
	if (value < 0)
		goto hw_err;
	module_data_width |= (value & 0xff);

	pgsz.side1 += log2(module_data_width);

	/* side two */
	value = smbus_read_byte(dimm_socket_address, SPD_NUM_DIMM_BANKS);
	if (value < 0)
		goto hw_err;
	if (value > 2)
		die("Bad SPD value\n");
	if (value == 2) {

		pgsz.side2 = pgsz.side1;	// Assume symmetric banks until we know differently
		value = smbus_read_byte(dimm_socket_address, SPD_NUM_COLUMNS);
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

	value = smbus_read_byte(dimm_socket_address, SPD_PRIMARY_SDRAM_WIDTH);
	die_on_spd_error(value);

	width.side1 = value & 0x7f;	// Mask off bank 2 flag

	if (value & 0x80) {
		width.side2 = width.side1 << 1;	// Bank 2 exists and is double-width
	} else {
		// If bank 2 exists, it's the same width as bank 1
		value = smbus_read_byte(dimm_socket_address, SPD_NUM_DIMM_BANKS);
		die_on_spd_error(value);

		if (value == 2)
			width.side2 = width.side1;
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
static struct dimm_size spd_get_dimm_size(unsigned int dimm_socket_address)
{
	int value;

	// Start with log2(page size)
	struct dimm_size sz = sdram_spd_get_page_size(dimm_socket_address);

	if (sz.side1 > 0) {

		value = smbus_read_byte(dimm_socket_address, SPD_NUM_ROWS);
		die_on_spd_error(value);

		sz.side1 += value & 0xf;

		if (sz.side2 > 0) {

			// Double-sided DIMM
			if (value & 0xF0)
				sz.side2 += value >> 4;	// Asymmetric
			else
				sz.side2 += value;	// Symmetric
		}

		value = smbus_read_byte(dimm_socket_address,
				  SPD_NUM_BANKS_PER_SDRAM);
		die_on_spd_error(value);

		value = log2(value);
		sz.side1 += value;
		if (sz.side2 > 0)
			sz.side2 += value;
	}

	return sz;
}

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
	int dimm0_value = smbus_read_byte(dimm0_address, spd_byte_number);
	int dimm1_value = smbus_read_byte(dimm1_address, spd_byte_number);

	if ((dimm0_value >= 0) && (dimm1_value >= 0)
	    && (dimm0_value == dimm1_value))
		bEqual = 1;

	return bEqual;
}

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
		struct dimm_size page_size;
		struct dimm_size sdram_width;
		int spd_value;

		if (channel0_dimm == 0)
			continue;	// No such socket on this mainboard

		if (smbus_read_byte(channel0_dimm, SPD_MEMORY_TYPE) !=
		    SPD_MEMORY_TYPE_SDRAM_DDR)
			continue;

		if (smbus_read_byte(channel0_dimm, SPD_MODULE_VOLTAGE) !=
		    SPD_VOLTAGE_SSTL2)
			continue;	// Unsupported voltage

		// E7501 does not support unregistered DIMMs
		spd_value = smbus_read_byte(channel0_dimm, SPD_MODULE_ATTRIBUTES);
		if (!(spd_value & MODULE_REGISTERED) || (spd_value < 0))
			continue;

		// Must support burst = 4 for dual-channel operation on E7501
		// NOTE: for single-channel, burst = 8 is required
		spd_value = smbus_read_byte(channel0_dimm,
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

		// Channel 0 DIMM looks compatible.
		// Now see if it is paired with the proper DIMM on channel 1.

		ASSERT(channel1_dimm != 0);	// No such socket on this mainboard??

		// NOTE: unpopulated DIMMs cause read to fail
		spd_value = smbus_read_byte(channel1_dimm, SPD_MODULE_ATTRIBUTES);
		if (!(spd_value & MODULE_REGISTERED) || (spd_value < 0)) {

			printk(BIOS_DEBUG, "Skipping un-matched DIMMs - only dual-channel operation supported\n");
			continue;
		}

		spd_value = smbus_read_byte(channel1_dimm,
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

		if (bDualChannel) {
			// This DIMM pair is usable
			dimm_mask |= 1 << i;
			dimm_mask |= 1 << (MAX_DIMM_SOCKETS_PER_CHANNEL + i);
		} else
			printk(BIOS_DEBUG, "Skipping un-matched DIMMs - only dual-channel operation supported\n");
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
	uint8_t dimm_start_64M_multiple;
	uintptr_t dimm_start_address;
	uint32_t dram_controller_mode;
	uint8_t i;

	// Configure the RAM command
	dram_controller_mode = pci_read_config32(MCHDEV, DRC);
	dram_controller_mode &= 0xFFFFFF8F;
	dram_controller_mode |= command;
	pci_write_config32(MCHDEV, DRC, dram_controller_mode);

	// RAM_COMMAND_NORMAL is an exception.
	// It affects only the memory controller and does not need to be "sent" to the DIMMs.
	if (command == RAM_COMMAND_NORMAL) {
		EXTRA_DELAY;
		return;
	}

	// NOTE: for mode select commands, some of the location address bits are part of the command
	// Map JEDEC mode bits to E7505
	if (command == RAM_COMMAND_MRS) {
		// Host address lines [25:18] map to DIMM address lines [7:0]
		// Host address lines [17:16] map to DIMM address lines [9:8]
		// Host address lines [15:4] map to DIMM address lines [11:0]
		dimm_start_address = (jedec_mode_bits & 0x00ff) << 18;
		dimm_start_address |= (jedec_mode_bits & 0x0300) << 8;
		dimm_start_address |= (jedec_mode_bits & 0x0fff) << 4;
	} else if (command == RAM_COMMAND_EMRS) {
		// Host address lines [15:4] map to DIMM address lines [11:0]
		dimm_start_address = (jedec_mode_bits << 4);
	} else {
		ASSERT(jedec_mode_bits == 0);
		dimm_start_address = 0;
	}

	// Send the command to all DIMMs by accessing a memory location within each

	dimm_start_64M_multiple = 0;

	/* FIXME: Only address the number of rows present in the system?
	 * Seems like rows 4-7 overlap with 0-3.
	 */
	for (i = 0; i < (MAX_NUM_CHANNELS * MAX_DIMM_SOCKETS_PER_CHANNEL); ++i) {

		uint8_t dimm_end_64M_multiple = pci_read_config8(MCHDEV, DRB_ROW_0 + i);

		if (dimm_end_64M_multiple > dimm_start_64M_multiple) {
			dimm_start_address &= 0x3ffffff;
			dimm_start_address |= dimm_start_64M_multiple << 26;
			read32((void *)dimm_start_address);
			// Set the start of the next DIMM
			dimm_start_64M_multiple = dimm_end_64M_multiple;
		}
	}
	EXTRA_DELAY;
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
	    pci_read_config32(MCHDEV, DRT) & DRT_CAS_MASK;

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
DIMM-independent configuration functions:
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
static uint8_t configure_dimm_row_boundaries(struct dimm_size dimm_log2_num_bits, uint8_t total_dram_64M_multiple, unsigned int dimm_index)
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
	pci_write_config8(MCHDEV, DRB_ROW_0 + (dimm_index << 1),
			  total_dram_64M_multiple);

	// If the DIMMs are double-sided, add the capacity of side 2 this DIMM pair
	// (as a multiple of 64 MB) to the total capacity of the system
	if (dimm_log2_num_bits.side2 >= 29)
		total_dram_64M_multiple +=
		    (1 << (dimm_log2_num_bits.side2 - 29));

	// Configure the boundary address for the row (if any) on side 2
	pci_write_config8(MCHDEV, DRB_ROW_1 + (dimm_index << 1),
			  total_dram_64M_multiple);

	// Update boundaries for rows subsequent to these.
	// These settings will be overridden by a subsequent call if a populated physical slot exists

	for (i = dimm_index + 1; i < MAX_DIMM_SOCKETS_PER_CHANNEL; i++) {
		pci_write_config8(MCHDEV, DRB_ROW_0 + (i << 1),
				  total_dram_64M_multiple);
		pci_write_config8(MCHDEV, DRB_ROW_1 + (i << 1),
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
	uint64_t tolm, tom;
	uint16_t reg;

	/* FIXME: Is there standard presence detect bit somewhere. */
	const int agp_slot_disabled = 1;

	/* Start with disabled remap range. */
	uint16_t remapbase_r = 0x3ff;
	uint16_t remaplimit_r = 0;

	// Configure the E7501's DRAM row boundaries
	// Start by zeroing out the temporary initial configuration
	pci_write_config32(MCHDEV, DRB_ROW_0, 0);
	pci_write_config32(MCHDEV, DRB_ROW_4, 0);

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

	tom = total_dram_64M_multiple * 64ULL * MiB;

	/* Reserve MMIO space. */
	tolm = 4ULL * GiB - 512 * MiB;
	if (agp_slot_disabled) {
		/* Reduce apertures to 2 x 4 MiB. */
		pci_write_config8(MCHDEV, APSIZE, 0x3F);
		pci_write_config16(AGPDEV, APSIZE1, 0x3F);
	} else {
		/* Add MMIO reserve for 2 x 256 MiB apertures. */
		tolm -= 512 * MiB;
	}
	tolm = MIN(tolm, tom);

	/* The PCI memory hole overlaps memory setup the remap window. */
	if (tolm < tom) {
		uint64_t remapbase = MAX(tom, 4ULL * GiB);
		uint64_t remaplimit = remapbase + (4ULL * GiB - tolm);

		remapbase_r = remapbase / (64 * MiB);
		remaplimit_r = remaplimit / (64 * MiB);

		/* Limit register is inclusive. */
		remaplimit_r -= 1;
	}

	/* Write the RAM configuration registers,
	   preserving the reserved bits. */
	reg = pci_read_config16(MCHDEV, TOLM) & 0x7ff;
	reg |= (tolm / (128 * MiB)) << 11;
	pci_write_config16(MCHDEV, TOLM, reg);

	reg = pci_read_config16(MCHDEV, REMAPBASE) & 0xfc00;
	reg |= remapbase_r;
	pci_write_config16(MCHDEV, REMAPBASE, reg);

	reg = pci_read_config16(MCHDEV, REMAPLIMIT) & 0xfc00;
	reg |= remaplimit_r;
	pci_write_config16(MCHDEV, REMAPLIMIT, reg);
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
	    pci_read_config32(MCHDEV, DRT) & DRT_CAS_MASK;

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

		value = smbus_read_byte(dimm_socket_address,
				  SPD_MIN_ROW_PRECHARGE_TIME);
		if (value < 0)
			goto hw_err;
		if (value > slowest_row_precharge)
			slowest_row_precharge = value;

		value = smbus_read_byte(dimm_socket_address,
				  SPD_MIN_RAS_TO_CAS_DELAY);
		if (value < 0)
			goto hw_err;
		if (value > slowest_ras_cas_delay)
			slowest_ras_cas_delay = value;

		value = smbus_read_byte(dimm_socket_address,
				  SPD_MIN_ACTIVE_TO_PRECHARGE_DELAY);
		if (value < 0)
			goto hw_err;
		if (value > slowest_active_to_precharge_delay)
			slowest_active_to_precharge_delay = value;
	}

	// NOTE for timing parameters:
	//              At 133 MHz, 1 clock == 7.52 ns

	/* Read the initial state */
	dram_timing = pci_read_config32(MCHDEV, DRT);

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

	/* Set to a 7 clock read delay. This is for 133MHz
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

	pci_write_config32(MCHDEV, DRT, dram_timing);

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
	uint16_t dram_read_timing;
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

		value = smbus_read_byte(dimm_socket_address,
				  SPD_ACCEPTABLE_CAS_LATENCIES);
		if (value < 0)
			goto hw_err;

		dimm_compatible_cas_latencies = value & 0x7f;	// Start with all supported by DIMM
		current_cas_latency = 1 << log2(dimm_compatible_cas_latencies);	// Max supported by DIMM

		// Can we support the highest CAS# latency?

		value = smbus_read_byte(dimm_socket_address,
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
			value = smbus_read_byte(dimm_socket_address,
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
			value = smbus_read_byte(dimm_socket_address,
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

	dram_timing = pci_read_config32(MCHDEV, DRT);
	dram_timing &= ~(DRT_CAS_MASK);

	dram_read_timing =
	    pci_read_config16(MCHDEV, DRDCTL);
	dram_read_timing &= 0xF000;

	if (system_compatible_cas_latencies & SPD_CAS_LATENCY_2_0) {
		dram_timing |= DRT_CAS_2_0;
		dram_read_timing |= 0x0222;
	} else if (system_compatible_cas_latencies & SPD_CAS_LATENCY_2_5) {

		uint32_t dram_row_attributes =
		    pci_read_config32(MCHDEV, DRA);

		dram_timing |= DRT_CAS_2_5;

		// At CAS# 2.5, DRAM Read Timing (if that's what it its) appears to need a slightly
		// different value if all DIMM slots are populated

		if ((dram_row_attributes & 0xff)
		    && (dram_row_attributes & 0xff00)
		    && (dram_row_attributes & 0xff0000)
		    && (dram_row_attributes & 0xff000000)) {

			// All slots populated
			dram_read_timing |= 0x0882;
		} else {
			// Some unpopulated slots
			dram_read_timing |= 0x0662;
		}
	} else
		die("No CAS# latencies compatible with all DIMMs!!\n");

	pci_write_config32(MCHDEV, DRT, dram_timing);

	/* set master DLL reset */
	dword = pci_read_config32(MCHDEV, 0x88);
	dword |= (1 << 26);
	pci_write_config32(MCHDEV, 0x88, dword);
	/* patch try register 88 is undocumented tnz */
	dword &= 0x0ca17fff;
	dword |= 0xd14a5000;
	pci_write_config32(MCHDEV, 0x88, dword);

	pci_write_config16(MCHDEV, DRDCTL,
			   dram_read_timing);

	/* clear master DLL reset */
	dword = pci_read_config32(MCHDEV, 0x88);
	dword &= ~(1 << 26);
	pci_write_config32(MCHDEV, 0x88, dword);

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
static void configure_e7501_dram_controller_mode(const struct mem_controller *ctrl,
						 uint8_t dimm_mask)
{
	int i;

	// Initial settings
	uint32_t controller_mode = pci_read_config32(MCHDEV, DRC);
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

		value = smbus_read_byte(dimm_socket_address,
				  SPD_DIMM_CONFIG_TYPE);
		die_on_spd_error(value);
		if (value != ERROR_SCHEME_ECC) {
			controller_mode &= ~(3 << 20);
		}

		value = smbus_read_byte(dimm_socket_address, SPD_REFRESH);
		die_on_spd_error(value);
		value &= 0x7f;	// Mask off self-refresh bit
		if (value > MAX_SPD_REFRESH_RATE) {
			printk(BIOS_ERR, "unsupported refresh rate\n");
			continue;
		}
		// Get the appropriate E7501 refresh mode for this DIMM
		dimm_refresh_mode = refresh_rate_map[value];
		if (dimm_refresh_mode > 7) {
			printk(BIOS_ERR, "unsupported refresh rate\n");
			continue;
		}
		// If this DIMM requires more frequent refresh than others,
		// update the system setting
		if (refresh_frequency[dimm_refresh_mode] >
		    refresh_frequency[system_refresh_mode])
			system_refresh_mode = dimm_refresh_mode;

		/* go to the next DIMM */
	}

	controller_mode |= (system_refresh_mode << 8);

	// Configure the E7501
	pci_write_config32(MCHDEV, DRC, controller_mode);
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
	pci_write_config32(MCHDEV, DRA, row_attributes);
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
	uint8_t clock_disable = pci_read_config8(MCHDEV, CKDIS);

	pci_write_config8(MCHDEV, 0x8e, 0xb0);

	for (i = 0; i < MAX_DIMM_SOCKETS_PER_CHANNEL; i++) {

		uint8_t socket_mask = 1 << i;

		if (dimm_mask & socket_mask)
			clock_disable &= ~socket_mask;	// DIMM present, enable clock
		else
			clock_disable |= socket_mask;	// DIMM absent, disable clock
	}

	pci_write_config8(MCHDEV, CKDIS, clock_disable);
}

/* DIMM-dependent configuration functions */

/**
 * DDR Receive FIFO RE-Sync (?)
 */
static void RAM_RESET_DDR_PTR(void)
{
	uint8_t byte;
	byte = pci_read_config8(MCHDEV, 0x88);
	byte |= (1 << 4);
	pci_write_config8(MCHDEV, 0x88, byte);

	byte = pci_read_config8(MCHDEV, 0x88);
	byte &= ~(1 << 4);
	pci_write_config8(MCHDEV, 0x88, byte);
}

/**
 * Copy 64 bytes from one location to another.
 *
 * @param src_addr TODO
 * @param dst_addr TODO
 */
static void write_8dwords(const uint32_t *src_addr, u8 *dst_addr)
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
static void rcomp_copy_registers(void)
{
	uint32_t dword;
	uint8_t strength_control;

	RAM_DEBUG_MESSAGE("Setting RCOMP registers.\n");

	/* Begin to write the RCOMP registers */
	write8(RCOMP_MMIO + 0x2c, 0x0);

	// Set CMD and DQ/DQS strength to 2x (?)
	strength_control = read8(RCOMP_MMIO + DQCMDSTR) & 0x88;
	strength_control |= 0x40;
	write8(RCOMP_MMIO + DQCMDSTR, strength_control);
	write_8dwords(slew_2x, RCOMP_MMIO + 0x80);
	write16(RCOMP_MMIO + 0x42, 0);

	// Set CMD and DQ/DQS strength to 2x (?)
	strength_control = read8(RCOMP_MMIO + DQCMDSTR) & 0xF8;
	strength_control |= 0x04;
	write8(RCOMP_MMIO + DQCMDSTR, strength_control);
	write_8dwords(slew_2x, RCOMP_MMIO + 0x60);
	write16(RCOMP_MMIO + 0x40, 0);

	// Set RCVEnOut# strength to 2x (?)
	strength_control = read8(RCOMP_MMIO + RCVENSTR) & 0xF8;
	strength_control |= 0x04;
	write8(RCOMP_MMIO + RCVENSTR, strength_control);
	write_8dwords(slew_2x, RCOMP_MMIO + 0x1c0);
	write16(RCOMP_MMIO + 0x50, 0);

	// Set CS# strength for x4 SDRAM to 2x (?)
	strength_control = read8(RCOMP_MMIO + CSBSTR) & 0x88;
	strength_control |= 0x04;
	write8(RCOMP_MMIO + CSBSTR, strength_control);
	write_8dwords(slew_2x, RCOMP_MMIO + 0x140);
	write16(RCOMP_MMIO + 0x48, 0);

	// Set CS# strength for x4 SDRAM to 2x (?)
	strength_control = read8(RCOMP_MMIO + CSBSTR) & 0x8F;
	strength_control |= 0x40;
	write8(RCOMP_MMIO + CSBSTR, strength_control);
	write_8dwords(slew_2x, RCOMP_MMIO + 0x160);
	write16(RCOMP_MMIO + 0x4a, 0);

	// Set CKE strength for x4 SDRAM to 2x (?)
	strength_control = read8(RCOMP_MMIO + CKESTR) & 0x88;
	strength_control |= 0x04;
	write8(RCOMP_MMIO + CKESTR, strength_control);
	write_8dwords(slew_2x, RCOMP_MMIO + 0xa0);
	write16(RCOMP_MMIO + 0x44, 0);

	// Set CKE strength for x4 SDRAM to 2x (?)
	strength_control = read8(RCOMP_MMIO + CKESTR) & 0x8F;
	strength_control |= 0x40;
	write8(RCOMP_MMIO + CKESTR, strength_control);
	write_8dwords(slew_2x, RCOMP_MMIO + 0xc0);
	write16(RCOMP_MMIO + 0x46, 0);

	// Set CK strength for x4 SDRAM to 1x (?)
	strength_control = read8(RCOMP_MMIO + CKSTR) & 0x88;
	strength_control |= 0x01;
	write8(RCOMP_MMIO + CKSTR, strength_control);
	write_8dwords(pull_updown_offset_table, RCOMP_MMIO + 0x180);
	write16(RCOMP_MMIO + 0x4c, 0);

	// Set CK strength for x4 SDRAM to 1x (?)
	strength_control = read8(RCOMP_MMIO + CKSTR) & 0x8F;
	strength_control |= 0x10;
	write8(RCOMP_MMIO + CKSTR, strength_control);
	write_8dwords(pull_updown_offset_table, RCOMP_MMIO + 0x1a0);
	write16(RCOMP_MMIO + 0x4e, 0);

	dword = read32(RCOMP_MMIO + 0x400);
	dword &= 0x7f7fffff;
	write32(RCOMP_MMIO + 0x400, dword);

	dword = read32(RCOMP_MMIO + 0x408);
	dword &= 0x7f7fffff;
	write32(RCOMP_MMIO + 0x408, dword);
}

static void ram_set_rcomp_regs(void)
{
	/* Set the RCOMP MMIO base address */
	mchtest_control(RCOMP_BAR_ENABLE);
	pci_write_config32(MCHDEV, SMRBASE, (uintptr_t)RCOMP_MMIO);

	/* Block RCOMP updates while we configure the registers */
	rcomp_smr_control(RCOMP_HOLD);
	rcomp_copy_registers();
	d060_control(D060_CMD_0);
	mchtest_control(MCHTST_CMD_0);

	uint8_t revision = pci_read_config8(MCHDEV, 0x08);
	if (revision >= 3) {
		rcomp_smr_control(RCOMP_SMR_00);
		rcomp_smr_control(RCOMP_SMR_01);
	}
	rcomp_smr_control(RCOMP_RELEASE);

	/* Wait 40 usec */
	SLOW_DOWN_IO;

	/* Clear the RCOMP MMIO base address */
	pci_write_config32(MCHDEV, SMRBASE, 0);
	mchtest_control(RCOMP_BAR_DISABLE);
}

/*-----------------------------------------------------------------------------
Public interface:
-----------------------------------------------------------------------------*/

/**
 * Go through the JEDEC initialization sequence for all DIMMs, then enable
 * refresh and initialize ECC and memory to zero. Upon exit, SDRAM is up
 * and running.
 *
 * @param ctrl PCI addresses of memory controller functions, and SMBus
 *             addresses of DIMM slots on the mainboard.
 */
static void sdram_enable(const struct mem_controller *ctrl)
{
	uint8_t dimm_mask = pci_read_config16(MCHDEV, SKPD);
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

	/* 4 Precharge all */
	RAM_DEBUG_MESSAGE("Ram Enable 4\n");
	do_ram_command(RAM_COMMAND_PRECHARGE, 0);
	/* wait until the all banks idle state... */

	/* 5. Issue EMRS to enable DLL */
	RAM_DEBUG_MESSAGE("Ram Enable 5\n");
	do_ram_command(RAM_COMMAND_EMRS,
		       SDRAM_EXTMODE_DLL_ENABLE |
		       SDRAM_EXTMODE_DRIVE_NORMAL);

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

	/* 8 Now we need 2 AUTO REFRESH / CBR cycles to be performed */
	/* And for good luck 6 more CBRs */
	RAM_DEBUG_MESSAGE("Ram Enable 8\n");
	int i;
	for (i = 0; i < 8; i++)
		do_ram_command(RAM_COMMAND_CBR, 0);

	/* 9 mode register set */
	RAM_DEBUG_MESSAGE("Ram Enable 9\n");
	set_ram_mode(E7501_SDRAM_MODE | SDRAM_MODE_NORMAL);

	/* 10 DDR Receive FIFO RE-Sync */
	RAM_DEBUG_MESSAGE("Ram Enable 10\n");
	RAM_RESET_DDR_PTR();
	EXTRA_DELAY;

	/* 11 normal operation */
	RAM_DEBUG_MESSAGE("Ram Enable 11\n");
	do_ram_command(RAM_COMMAND_NORMAL, 0);

	// Reconfigure the row boundaries and Top of Low Memory
	// to match the true size of the DIMMs
	configure_e7501_ram_addresses(ctrl, dimm_mask);

	/* Finally enable refresh */
	dram_controller_mode = pci_read_config32(MCHDEV, DRC);
	dram_controller_mode |= (1 << 29);
	pci_write_config32(MCHDEV, DRC, dram_controller_mode);
	EXTRA_DELAY;
}

/**
 * @param ctrl PCI addresses of memory controller functions, and SMBus
 *             addresses of DIMM slots on the mainboard.
 */
static void sdram_post_ecc(const struct mem_controller *ctrl)
{
	/* Fast CS# Enable. */
	uint32_t dram_controller_mode = pci_read_config32(MCHDEV, DRC);
	dram_controller_mode = pci_read_config32(MCHDEV, DRC);
	dram_controller_mode |= (1 << 17);
	pci_write_config32(MCHDEV, DRC, dram_controller_mode);
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
		printk(BIOS_DEBUG, "No usable memory for this controller\n");
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
	pci_write_config16(MCHDEV, SKPD, dimm_mask);
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
	uint32_t dword;
	uint16_t word;
	uint8_t byte;

	ram_set_rcomp_regs();

	/* Enable 0:0.1, 0:2.1 */
	word = pci_read_config16(MCHDEV, DVNP);
	word &= ~0x05;
	pci_write_config16(MCHDEV, DVNP, word);

	/* Disable high-memory remap (power-on defaults, really) */
	pci_write_config16(MCHDEV, REMAPBASE, 0x03ff);
	pci_write_config16(MCHDEV, REMAPLIMIT, 0x0);

	/* Disable legacy MMIO (0xC0000-0xEFFFF is DRAM) */
	int i;
	pci_write_config8(MCHDEV, PAM_0, 0x30);
	for (i = 1; i <= 6; i++)
		pci_write_config8(MCHDEV, PAM_0 + i, 0x33);

	/* Conservatively say each row has 64MB of ram, we will fix this up later
	 * Initial TOLM 8 rows 64MB each  (1<<3 * 1<<26) >> 16 = 1<<13
	 *
	 * FIXME: Hard-coded limit to first four rows to prevent overlap!
	 */
	pci_write_config32(MCHDEV, DRB_ROW_0, 0x04030201);
	pci_write_config32(MCHDEV, DRB_ROW_4, 0x04040404);
	//pci_write_config32(MCHDEV, DRB_ROW_4, 0x08070605);
	pci_write_config16(MCHDEV, TOLM, (1<<13));

	/* DIMM clocks off */
	pci_write_config8(MCHDEV, CKDIS, 0xff);

	/* reset row attributes */
	pci_write_config32(MCHDEV, DRA, 0x0);

	// The only things we need to set here are DRAM idle timer, Back-to-Back Read Turnaround, and
	// Back-to-Back Write-Read Turnaround. All others are configured based on SPD.
	dword = pci_read_config32(MCHDEV, DRT);
	dword &= 0xC7F8FFFF;
	dword |= (0x28<<24)|(0x03<<16);
	pci_write_config32(MCHDEV, DRT, dword);

	dword = pci_read_config32(MCHDEV, DRC);
	dword &= 0xffcef8f7;
	dword |= 0x00210008;
	pci_write_config32(MCHDEV, DRC, dword);

	/* Undocumented */
	pci_write_config8(MCHDEV, 0x88, 0x80);

	/* Undocumented. Set much later in vendor BIOS. */
	byte = pci_read_config8(MCHDEV, 0xd9);
	byte &= ~0x60;
	pci_write_config8(MCHDEV, 0xd9, byte);

	uint8_t revision = pci_read_config8(MCHDEV, 0x08);
	if (revision >= 3)
		d060_control(D060_CMD_1);
}

static int e7505_mch_is_ready(void)
{
	uint32_t dword = pci_read_config32(MCHDEV, DRC);
	return !!(dword & DRC_DONE);
}

void sdram_initialize(void)
{
	static const struct mem_controller memctrl[] = {
		{
			.d0 = PCI_DEV(0, 0, 0),
			.d0f1 = PCI_DEV(0, 0, 1),
			.channel0 = { 0x50, 0x52, 0, 0 },
			.channel1 = { 0x51, 0x53, 0, 0 },
		},
	};

	/* If this is a warm boot, some initialisation can be skipped */
	if (!e7505_mch_is_ready()) {

		/* The real MCH initialisation. */
		timestamp_add_now(TS_INITRAM_START);

		sdram_set_registers(memctrl);
		sdram_set_spd_registers(memctrl);
		sdram_enable(memctrl);

		/* Hook for post ECC scrub settings and debug. */
		sdram_post_ecc(memctrl);

		timestamp_add_now(TS_INITRAM_END);
	}

	printk(BIOS_DEBUG, "SDRAM is up.\n");
}
