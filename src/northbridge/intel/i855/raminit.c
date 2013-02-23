/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Jon Dufresne <jon.dufresne@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <assert.h>
#include <spd.h>
#include <sdram_mode.h>
#include <stdlib.h>
#include <delay.h>
#include "i855.h"

/*-----------------------------------------------------------------------------
Macros and definitions:
-----------------------------------------------------------------------------*/

#define VALIDATE_DIMM_COMPATIBILITY

/* Debugging macros. */
#if CONFIG_DEBUG_RAM_SETUP
#define PRINTK_DEBUG(x...)      printk(BIOS_DEBUG, x)
#define DUMPNORTH()             dump_pci_device(NORTHBRIDGE_MMC)
#else
#define PRINTK_DEBUG(x...)
#define DUMPNORTH()
#endif

#define delay() udelay(200)

#define VG85X_MODE (SDRAM_BURST_4 | SDRAM_BURST_INTERLEAVED | SDRAM_CAS_2_5)

/* DRC[10:8] - Refresh Mode Select (RMS).
 * 0x0 for Refresh Disabled (Self Refresh)
 * 0x1 for Refresh interval 15.6 us for 133MHz
 * 0x2 for Refresh interval 7.8 us for 133MHz
 * 0x7 for Refresh interval 64 Clocks. (Fast Refresh Mode)
 */
#define RAM_COMMAND_REFRESH		0x1

/* DRC[6:4] - SDRAM Mode Select (SMS). */
#define RAM_COMMAND_SELF_REFRESH	0x0
#define RAM_COMMAND_NOP			0x1
#define RAM_COMMAND_PRECHARGE		0x2
#define RAM_COMMAND_MRS			0x3
#define RAM_COMMAND_EMRS		0x4
#define RAM_COMMAND_CBR			0x6
#define RAM_COMMAND_NORMAL		0x7

/* DRC[29] - Initialization Complete (IC). */
#define RAM_COMMAND_IC			0x1

struct dimm_size {
	unsigned int side1;
	unsigned int side2;
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
	/* Map the JEDEC spd refresh rates (array index) to i855 Refresh Mode
	 * Select values (array value)
	 * These are all the rates defined by JESD21-C Appendix D, Rev. 1.0
	 * The i855 supports only 15.6 us (1), 7.8 us (2) and
	 * 64 clock (481 ns) (7) refresh.
	 * [0] ==  15.625 us -> 15.6 us
	 * [1] ==   3.9   us -> 481  ns
	 * [2] ==   7.8   us ->  7.8 us
	 * [3] ==  31.3   us -> 15.6 us
	 * [4] ==  62.5   us -> 15.6 us
	 * [5] == 125     us -> 15.6 us
	 */
	1, 7, 2, 1, 1, 1
};

#define MAX_SPD_REFRESH_RATE ((sizeof(refresh_rate_map) / sizeof(uint32_t)) - 1)

/*-----------------------------------------------------------------------------
SPD functions:
-----------------------------------------------------------------------------*/

static void die_on_spd_error(int spd_return_value)
{
	if (spd_return_value < 0)
		PRINTK_DEBUG("Error reading SPD info: got %d\n", spd_return_value);
/*
	if (spd_return_value < 0)
		die("Error reading SPD info\n");
*/
}

/**
 * Calculate the page size for each physical bank of the DIMM:
 *
 *   log2(page size) = (# columns) + log2(data width)
 *
 * NOTE: Page size is the total number of data bits in a row.
 *
 * @param dimm_socket_address SMBus address of DIMM socket to interrogate.
 * @return log2(page size) for each side of the DIMM.
 */
static struct dimm_size sdram_spd_get_page_size(u8 dimm_socket_address)
{
	uint16_t module_data_width;
	int value;
	struct dimm_size pgsz;

	pgsz.side1 = 0;
	pgsz.side2 = 0;

	// Side 1
	value = spd_read_byte(dimm_socket_address, SPD_NUM_COLUMNS);
	die_on_spd_error(value);

	pgsz.side1 = value & 0xf;	// # columns in bank 1

	/* Get the module data width and convert it to a power of two */
	value = spd_read_byte(dimm_socket_address, SPD_MODULE_DATA_WIDTH_MSB);
	die_on_spd_error(value);

	module_data_width = (value & 0xff) << 8;

	value = spd_read_byte(dimm_socket_address, SPD_MODULE_DATA_WIDTH_LSB);
	die_on_spd_error(value);

	module_data_width |= (value & 0xff);

	pgsz.side1 += log2(module_data_width);

	/* side two */
	value = spd_read_byte(dimm_socket_address, SPD_NUM_DIMM_BANKS);
	die_on_spd_error(value);

/*
	if (value > 2)
		die("Bad SPD value\n");
*/
	if (value > 2)
		PRINTK_DEBUG("Bad SPD value\n");

	if (value == 2) {
		pgsz.side2 = pgsz.side1;	// Assume symmetric banks until we know differently
		value = spd_read_byte(dimm_socket_address, SPD_NUM_COLUMNS);
		die_on_spd_error(value);

		if ((value & 0xf0) != 0) {
			// Asymmetric banks
			pgsz.side2 -= value & 0xf;	/* Subtract out columns on side 1 */
			pgsz.side2 += (value >> 4) & 0xf;	/* Add in columns on side 2 */
		}
	}

	return pgsz;
}

/**
 * Read the width in bits of each DIMM side's DRAMs via SPD (i.e. 4, 8, 16).
 *
 * @param dimm_socket_address SMBus address of DIMM socket to interrogate.
 * @return Width in bits of each DIMM side's DRAMs.
 */
static struct dimm_size sdram_spd_get_width(u8 dimm_socket_address)
{
	int value;
	struct dimm_size width;

	width.side1 = 0;
	width.side2 = 0;

	value = spd_read_byte(dimm_socket_address, SPD_PRIMARY_SDRAM_WIDTH);
	die_on_spd_error(value);

	width.side1 = value & 0x7f;	// Mask off bank 2 flag

	if (value & 0x80) {
		width.side2 = width.side1 << 1;	// Bank 2 exists and is double-width
	} else {
		// If bank 2 exists, it's the same width as bank 1
		value = spd_read_byte(dimm_socket_address, SPD_NUM_DIMM_BANKS);
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
 * @param dimm SMBus address of DIMM socket to interrogate.
 * @return log2(number of bits) for each side of the DIMM.
 */
static struct dimm_size spd_get_dimm_size(unsigned dimm)
{
	int value;

	// Start with log2(page size)
	struct dimm_size sz = sdram_spd_get_page_size(dimm);

	if (sz.side1 > 0) {
		value = spd_read_byte(dimm, SPD_NUM_ROWS);
		die_on_spd_error(value);

		sz.side1 += value & 0xf;

		if (sz.side2 > 0) {
			// Double-sided DIMM
			if (value & 0xF0)
				sz.side2 += value >> 4;	// Asymmetric
			else
				sz.side2 += value;	// Symmetric
		}

		value = spd_read_byte(dimm, SPD_NUM_BANKS_PER_SDRAM);
		die_on_spd_error(value);

		value = log2(value);
		sz.side1 += value;
		if (sz.side2 > 0)
			sz.side2 += value;
	}

	return sz;
}

/**
 * Scan for compatible DIMMs.
 *
 * @return A bitmask indicating which sockets contain a compatible DIMM.
 */
static uint8_t spd_get_supported_dimms(void)
{
	int i;
	uint8_t dimm_mask = 0;

	for (i = 0; i < DIMM_SOCKETS; i++) {
		u8 dimm = DIMM0 + i;

#ifdef VALIDATE_DIMM_COMPATIBILITY
		struct dimm_size page_size;
		struct dimm_size sdram_width;
#endif
		int spd_value;

		if (dimm == 0)
			continue;	// No such socket on this mainboard

		if (spd_read_byte(dimm, SPD_MEMORY_TYPE) != SPD_MEMORY_TYPE_SDRAM_DDR)
			continue;

#ifdef VALIDATE_DIMM_COMPATIBILITY
		if ((spd_value = spd_read_byte(dimm, SPD_MODULE_VOLTAGE)) != SPD_VOLTAGE_SSTL2) {
			PRINTK_DEBUG("Skipping DIMM with unsupported voltage: %02x\n", spd_value);
			continue;	// Unsupported voltage
		}

/*
		// E7501 does not support unregistered DIMMs
		spd_value = spd_read_byte(dimm, SPD_MODULE_ATTRIBUTES);
		if (!(spd_value & MODULE_REGISTERED) || (spd_value < 0)) {
			PRINTK_DEBUG("Skipping unregistered DIMM: %02x\n", spd_value);
			continue;
		}
*/

		page_size = sdram_spd_get_page_size(dimm);
		sdram_width = sdram_spd_get_width(dimm);

		// Validate DIMM page size
		// The i855 only supports page sizes of 4, 8, 16 KB per channel
		// NOTE:  4 KB =  32 Kb = 2^15
		//       16 KB = 128 Kb = 2^17

		if ((page_size.side1 < 15) || (page_size.side1 > 17)) {
			PRINTK_DEBUG("Skipping DIMM with unsupported page size: %d\n", page_size.side1);
			continue;
		}

		// If DIMM is double-sided, verify side2 page size
		if (page_size.side2 != 0) {
			if ((page_size.side2 < 15) || (page_size.side2 > 17)) {
				PRINTK_DEBUG("Skipping DIMM with unsupported page size: %d\n", page_size.side2);
				continue;
			}
		}
		// Validate SDRAM width
		// The i855 only supports x8 and x16 devices
		if ((sdram_width.side1 != 8) && (sdram_width.side1 != 16)) {
			PRINTK_DEBUG("Skipping DIMM with unsupported width: %d\n", sdram_width.side2);
			continue;
		}

		// If DIMM is double-sided, verify side2 width
		if (sdram_width.side2 != 0) {
			if ((sdram_width.side2 != 8)
			    && (sdram_width.side2 != 16)) {
				PRINTK_DEBUG("Skipping DIMM with unsupported width: %d\n", sdram_width.side2);
				continue;
			}
		}
#endif
		// Made it through all the checks, this DIMM is usable
		dimm_mask |= (1 << i);
	}

	return dimm_mask;
}

/*-----------------------------------------------------------------------------
SDRAM configuration functions:
-----------------------------------------------------------------------------*/

static void do_ram_command(uint8_t command, uint16_t jedec_mode_bits)
{
	int i;
	u32 reg32;
	uint8_t dimm_start_32M_multiple = 0;
	uint16_t i855_mode_bits = jedec_mode_bits;

	/* Configure the RAM command. */
	reg32 = pci_read_config32(NORTHBRIDGE_MMC, DRC);
	reg32 &= ~(7 << 4);
	reg32 |= (command << 4);
	PRINTK_DEBUG("  Sending RAM command 0x%08x\n", reg32);
	pci_write_config32(NORTHBRIDGE_MMC, DRC, reg32);

        // RAM_COMMAND_NORMAL is an exception.
        // It affects only the memory controller and does not need to be "sent" to the DIMMs.

        if (command != RAM_COMMAND_NORMAL) {

                // Send the command to all DIMMs by accessing a memory location within each
                // NOTE: for mode select commands, some of the location address bits
                // are part of the command

                // Map JEDEC mode bits to i855
                if (command == RAM_COMMAND_MRS || command == RAM_COMMAND_EMRS) {
			/* Host address lines [13:3] map to DIMM address lines [11, 9:0] */
			i855_mode_bits = ((jedec_mode_bits & 0x800) << (13 - 11)) | ((jedec_mode_bits & 0x3ff) << (12 - 9));
                }

                for (i = 0; i < (DIMM_SOCKETS * 2); ++i) {
                        uint8_t dimm_end_32M_multiple = pci_read_config8(NORTHBRIDGE_MMC, DRB + i);
                        if (dimm_end_32M_multiple > dimm_start_32M_multiple) {

                                uint32_t dimm_start_address = dimm_start_32M_multiple << 25;
				PRINTK_DEBUG("  Sending RAM command to 0x%08x\n", dimm_start_address + i855_mode_bits);
                                read32(dimm_start_address + i855_mode_bits);

                                // Set the start of the next DIMM
                                dimm_start_32M_multiple = dimm_end_32M_multiple;
                        }
		}
	}
}

static void set_initialize_complete(void)
{
	uint32_t drc_reg;

	drc_reg = pci_read_config32(NORTHBRIDGE_MMC, DRC);
	drc_reg |= (1 << 29);
	pci_write_config32(NORTHBRIDGE_MMC, DRC, drc_reg);
}

static void sdram_enable(void)
{
	int i;

	print_debug("Ram enable 1\n");
	delay();
	delay();

	/* NOP command */
	PRINTK_DEBUG(" NOP\n");
	do_ram_command(RAM_COMMAND_NOP, 0);
	delay();
	delay();
	delay();

	/* Pre-charge all banks (at least 200 us after NOP) */
	PRINTK_DEBUG(" Pre-charging all banks\n");
	do_ram_command(RAM_COMMAND_PRECHARGE, 0);
	delay();
	delay();
	delay();

	print_debug("Ram enable 4\n");
	do_ram_command(RAM_COMMAND_EMRS, SDRAM_EXTMODE_DLL_ENABLE);
	delay();
	delay();
	delay();

	print_debug("Ram enable 5\n");
	do_ram_command(RAM_COMMAND_MRS, VG85X_MODE | SDRAM_MODE_DLL_RESET);

	print_debug("Ram enable 6\n");
	do_ram_command(RAM_COMMAND_PRECHARGE, 0);
	delay();
	delay();
	delay();

	/* 8 CBR refreshes (Auto Refresh) */
	PRINTK_DEBUG(" 8 CBR refreshes\n");
	for(i = 0; i < 8; i++) {
		do_ram_command(RAM_COMMAND_CBR, 0);
		delay();
		delay();
		delay();
	}

	print_debug("Ram enable 8\n");
	do_ram_command(RAM_COMMAND_MRS, VG85X_MODE | SDRAM_MODE_NORMAL);

	/* Set GME-M Mode Select bits back to NORMAL operation mode */
	PRINTK_DEBUG(" Normal operation mode\n");
	do_ram_command(RAM_COMMAND_NORMAL, 0);
	delay();
	delay();
	delay();

	print_debug("Ram enable 9\n");
	set_initialize_complete();

	delay();
	delay();
	delay();
	delay();
	delay();

	print_debug("After configuration:\n");
	/* dump_pci_devices(); */

	/*
	print_debug("\n\n***** RAM TEST *****\n");
	ram_check(0, 0xa0000);
	ram_check(0x100000, 0x40000000);
	*/
}

/*-----------------------------------------------------------------------------
DIMM-independant configuration functions:
-----------------------------------------------------------------------------*/

/**
 * Set only what I need until it works, then make it figure things out on boot
 * assumes only one DIMM is populated.
 */
static void sdram_set_registers(void)
{
	/*
	print_debug("Before configuration:\n");
	dump_pci_devices();
	*/
}

static void spd_set_row_attributes(uint8_t dimm_mask)
{
	int i;
	uint16_t row_attributes = 0;

	for (i = 0; i < DIMM_SOCKETS; i++) {
		u8 dimm = DIMM0 + i;
		struct dimm_size page_size;
		struct dimm_size sdram_width;

		if (!(dimm_mask & (1 << i))) {
			row_attributes |= 0x77 << (i << 3);
			continue;	// This DIMM not usable
		}

		// Get the relevant parameters via SPD
		page_size = sdram_spd_get_page_size(dimm);
		sdram_width = sdram_spd_get_width(dimm);

		// Update the DRAM Row Attributes.
		// Page size is encoded as log2(page size in bits) - log2(2 KB) or 4 KB == 1, 8 KB == 3, 16KB == 3
		// NOTE:  2 KB =  16 Kb = 2^14
		row_attributes |= (page_size.side1 - 14) << (i << 3);	// Side 1 of each DIMM is an EVEN row

		if (sdram_width.side2 > 0)
			row_attributes |= (page_size.side2 - 14) << ((i << 3) + 4);	// Side 2 is ODD
		else
			row_attributes |= 7 << ((i << 3) + 4);
		/* go to the next DIMM */
	}

	PRINTK_DEBUG("DRA: %04x\n", row_attributes);

	/* Write the new row attributes register */
	pci_write_config16(NORTHBRIDGE_MMC, DRA, row_attributes);
}

static void spd_set_dram_controller_mode(uint8_t dimm_mask)
{
	int i;

	// Initial settings
	u32 controller_mode = pci_read_config32(NORTHBRIDGE_MMC, DRC);
	u32 system_refresh_mode = (controller_mode >> 7) & 7;

	controller_mode |= (1 << 20);  // ECC
	controller_mode |= (1 << 15);  // RAS lockout
	controller_mode |= (1 << 12);  // Address Tri-state enable (ADRTRIEN), FIXME: how is this detected?????
	controller_mode |= (2 << 10);  // FIXME: Undocumented, really needed?????

	for (i = 0; i < DIMM_SOCKETS; i++) {
		u8 dimm = DIMM0 + i;
		uint32_t dimm_refresh_mode;
		int value;
		u8 tRCD, tRP;

		if (!(dimm_mask & (1 << i))) {
			continue;	// This DIMM not usable
		}

		// Disable ECC mode if any one of the DIMMs does not support ECC
		value = spd_read_byte(dimm, SPD_DIMM_CONFIG_TYPE);
		die_on_spd_error(value);
		if (value != ERROR_SCHEME_ECC)
			controller_mode &= ~(3 << 20);

		value = spd_read_byte(dimm, SPD_REFRESH);
		die_on_spd_error(value);
		value &= 0x7f;	// Mask off self-refresh bit
		if (value > MAX_SPD_REFRESH_RATE) {
			print_err("unsupported refresh rate\n");
			continue;
		}
		// Get the appropriate i855 refresh mode for this DIMM
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

		/* FIXME: is this correct? */
		tRCD = spd_read_byte(dimm, SPD_tRCD);
		tRP = spd_read_byte(dimm, SPD_tRP);
		if (tRCD != tRP) {
			PRINTK_DEBUG(" Disabling RAS lockouk due to tRCD (%d) != tRP (%d)\n", tRCD, tRP);
			controller_mode &= ~(1 << 15);
		}

		/* go to the next DIMM */
	}

	controller_mode &= ~(7 << 7);
	controller_mode |= (system_refresh_mode << 7);
	PRINTK_DEBUG("DRC: %08x\n", controller_mode);

	pci_write_config32(NORTHBRIDGE_MMC, DRC, controller_mode);
}

static void spd_set_dram_timing(uint8_t dimm_mask)
{
	int i;
	u32 dram_timing;

	// CAS# latency bitmasks in SPD_ACCEPTABLE_CAS_LATENCIES format
	// NOTE: i82822 supports only 2.0 and 2.5
	uint32_t system_compatible_cas_latencies = SPD_CAS_LATENCY_2_0 | SPD_CAS_LATENCY_2_5;
	uint8_t slowest_row_precharge = 0;
	uint8_t slowest_ras_cas_delay = 0;
	uint8_t slowest_active_to_precharge_delay = 0;

	for (i = 0; i < DIMM_SOCKETS; i++) {
		u8 dimm = DIMM0 + i;
		int value;
		uint32_t current_cas_latency;
		uint32_t dimm_compatible_cas_latencies;
		if (!(dimm_mask & (1 << i)))
			continue;	// This DIMM not usable

		value = spd_read_byte(dimm, SPD_ACCEPTABLE_CAS_LATENCIES);
		PRINTK_DEBUG("SPD_ACCEPTABLE_CAS_LATENCIES: %d\n", value);
		die_on_spd_error(value);

		dimm_compatible_cas_latencies = value & 0x7f;	// Start with all supported by DIMM
		PRINTK_DEBUG("dimm_compatible_cas_latencies #1: %d\n", dimm_compatible_cas_latencies);

		current_cas_latency = 1 << log2(dimm_compatible_cas_latencies);	// Max supported by DIMM
		PRINTK_DEBUG("current_cas_latency: %d\n", current_cas_latency);

		// Can we support the highest CAS# latency?
		value = spd_read_byte(dimm, SPD_MIN_CYCLE_TIME_AT_CAS_MAX);
		die_on_spd_error(value);
		PRINTK_DEBUG("SPD_MIN_CYCLE_TIME_AT_CAS_MAX: %d.%d\n", value >> 4, value & 0xf);

		// NOTE: At 133 MHz, 1 clock == 7.52 ns
		if (value > 0x75) {
			// Our bus is too fast for this CAS# latency
			// Remove it from the bitmask of those supported by the DIMM that are compatible
			dimm_compatible_cas_latencies &= ~current_cas_latency;
			PRINTK_DEBUG("dimm_compatible_cas_latencies #2: %d\n", dimm_compatible_cas_latencies);
		}
		// Can we support the next-highest CAS# latency (max - 0.5)?

		current_cas_latency >>= 1;
		if (current_cas_latency != 0) {
			value = spd_read_byte(dimm, SPD_SDRAM_CYCLE_TIME_2ND);
			die_on_spd_error(value);
			PRINTK_DEBUG("SPD_SDRAM_CYCLE_TIME_2ND: %d.%d\n", value >> 4, value & 0xf);
			if (value > 0x75) {
				dimm_compatible_cas_latencies &= ~current_cas_latency;
				PRINTK_DEBUG("dimm_compatible_cas_latencies #2: %d\n", dimm_compatible_cas_latencies);
			}
		}
		// Can we support the next-highest CAS# latency (max - 1.0)?
		current_cas_latency >>= 1;
		if (current_cas_latency != 0) {
			value = spd_read_byte(dimm, SPD_SDRAM_CYCLE_TIME_3RD);
			PRINTK_DEBUG("SPD_SDRAM_CYCLE_TIME_3RD: %d.%d\n", value >> 4, value & 0xf);
			die_on_spd_error(value);
			if (value > 0x75) {
				dimm_compatible_cas_latencies &= ~current_cas_latency;
				PRINTK_DEBUG("dimm_compatible_cas_latencies #2: %d\n", dimm_compatible_cas_latencies);
			}
		}
		// Restrict the system to CAS# latencies compatible with this DIMM
		system_compatible_cas_latencies &= dimm_compatible_cas_latencies;

		value = spd_read_byte(dimm, SPD_MIN_ROW_PRECHARGE_TIME);
		die_on_spd_error(value);
		if (value > slowest_row_precharge)
			slowest_row_precharge = value;

		value = spd_read_byte(dimm, SPD_MIN_RAS_TO_CAS_DELAY);
		die_on_spd_error(value);
		if (value > slowest_ras_cas_delay)
			slowest_ras_cas_delay = value;

		value = spd_read_byte(dimm, SPD_MIN_ACTIVE_TO_PRECHARGE_DELAY);
		die_on_spd_error(value);
		if (value > slowest_active_to_precharge_delay)
			slowest_active_to_precharge_delay = value;

		/* go to the next DIMM */
	}
	PRINTK_DEBUG("CAS latency: %d\n", system_compatible_cas_latencies);

	dram_timing = pci_read_config32(NORTHBRIDGE_MMC, DRT);
	dram_timing &= ~(DRT_CAS_MASK | DRT_TRP_MASK | DRT_RCD_MASK);
	PRINTK_DEBUG("DRT: %08x\n", dram_timing);

	if (system_compatible_cas_latencies & SPD_CAS_LATENCY_2_0) {
		dram_timing |= DRT_CAS_2_0;
	} else if (system_compatible_cas_latencies & SPD_CAS_LATENCY_2_5) {
		dram_timing |= DRT_CAS_2_5;
	} else
		die("No CAS# latencies compatible with all DIMMs!!\n");

	uint32_t current_cas_latency = dram_timing & DRT_CAS_MASK;

	/* tRP */

	PRINTK_DEBUG("slowest_row_precharge: %d.%d\n", slowest_row_precharge >> 2, slowest_row_precharge & 0x3);
	// i855 supports only 2, 3 or 4 clocks for tRP
	if (slowest_row_precharge > ((30 << 2)))
		die("unsupported DIMM tRP");	//  > 30.0 ns: 5 or more clocks
	else if (slowest_row_precharge > ((22 << 2) | (2 << 0)))
		dram_timing |= DRT_TRP_4;	//  > 22.5 ns: 4 or more clocks
	else if (slowest_row_precharge > (15 << 2))
		dram_timing |= DRT_TRP_3;	//  > 15.0 ns: 3 clocks
	else
		dram_timing |= DRT_TRP_2;	// <= 15.0 ns: 2 clocks

	/*  tRCD */

	PRINTK_DEBUG("slowest_ras_cas_delay: %d.%d\n", slowest_ras_cas_delay >> 2, slowest_ras_cas_delay & 0x3);
	// i855 supports only 2, 3 or 4 clocks for tRCD
	if (slowest_ras_cas_delay > ((30 << 2)))
		die("unsupported DIMM tRCD");	//  > 30.0 ns: 5 or more clocks
	else if (slowest_ras_cas_delay > ((22 << 2) | (2 << 0)))
		dram_timing |= DRT_RCD_4;	//  > 22.5 ns: 4 or more clocks
	else if (slowest_ras_cas_delay > (15 << 2))
		dram_timing |= DRT_RCD_3;	//  > 15.0 ns: 3 clocks
	else
		dram_timing |= DRT_RCD_2;	// <= 15.0 ns: 2 clocks

	/* tRAS, min */

	PRINTK_DEBUG("slowest_active_to_precharge_delay: %d\n", slowest_active_to_precharge_delay);
	// i855 supports only 5, 6, 7 or 8 clocks for tRAS
	// 5 clocks ~= 37.6 ns, 6 clocks ~= 45.1 ns, 7 clocks ~= 52.6 ns, 8 clocks ~= 60.1 ns
	if (slowest_active_to_precharge_delay > 60)
		die("unsupported DIMM tRAS");	// > 52 ns:      8 or more clocks
	else if (slowest_active_to_precharge_delay > 52)
		dram_timing |= DRT_TRAS_MIN_8;	// 46-52 ns:     7 clocks
	else if (slowest_active_to_precharge_delay > 45)
		dram_timing |= DRT_TRAS_MIN_7;	// 46-52 ns:     7 clocks
	else if (slowest_active_to_precharge_delay > 37)
		dram_timing |= DRT_TRAS_MIN_6;	// 38-45 ns:     6 clocks
	else
		dram_timing |= DRT_TRAS_MIN_5;	// < 38 ns:      5 clocks

	/* FIXME: guess work starts here...
	 *
	 * Intel refers to DQ turn-arround values for back to calculate the values,
	 * but i have no idea what this means
	 */

	/*
	 * Back to Back Read-Write command spacing (DDR, different Rows/Bank)
	 */
	/* Set to a 3 clock back to back read to write turn around.
	 *  2 is a good delay if the CAS latency is 2.0 */
	dram_timing &= ~(3 << 28);
	if (current_cas_latency == DRT_CAS_2_0)
		dram_timing |= (2 << 28);	// 2 clocks
	else
		dram_timing |= (1 << 28);	// 3 clocks

	/*
	 * Back to Back Read-Write command spacing (DDR, same or different Rows/Bank)
	 */
	dram_timing &= ~(3 << 26);
	if (current_cas_latency == DRT_CAS_2_0)
		dram_timing |= (2 << 26);	// 5 clocks
	else
		dram_timing |= (1 << 26);	// 6 clocks

	/*
	 * Back To Back Read-Read commands spacing (DDR, different Rows):
	 */
	dram_timing &= ~(1 << 25);
	dram_timing |= (1 << 25);	// 3 clocks

	PRINTK_DEBUG("DRT: %08x\n", dram_timing);
	pci_write_config32(NORTHBRIDGE_MMC, DRT, dram_timing);
}

static void spd_set_dram_size(uint8_t dimm_mask)
{
	int i;
	int total_dram = 0;
	uint32_t drb_reg = 0;

	for (i = 0; i < DIMM_SOCKETS; i++) {
		u8 dimm = DIMM0 + i;
		struct dimm_size sz;

		if (!(dimm_mask & (1 << i))) {
			/* fill values even for not present DIMMs */
			drb_reg |= (total_dram << (i * 16));
			drb_reg |= (total_dram << ((i * 16) + 8));

			continue;	// This DIMM not usable
		}
		sz = spd_get_dimm_size(dimm);

		total_dram += (1 << (sz.side1 - 28));
		drb_reg |= (total_dram << (i * 16));

		total_dram += (1 << (sz.side2 - 28));
		drb_reg |= (total_dram << ((i * 16) + 8));
	}
	PRINTK_DEBUG("DRB: %08x\n", drb_reg);
	pci_write_config32(NORTHBRIDGE_MMC, DRB, drb_reg);
}


static void spd_set_dram_pwr_management(void)
{
	uint32_t pwrmg_reg;

	pwrmg_reg = 0x10f10430;
	pci_write_config32(NORTHBRIDGE_MMC, PWRMG, pwrmg_reg);
}

static void spd_set_dram_throttle_control(void)
{
	uint32_t dtc_reg = 0;

	/* DDR SDRAM Throttle Mode (TMODE):
	 *   0011 = Both Rank and GMCH Thermal Sensor based throttling is enabled. When the external SO-
	 *          DIMM Thermal Sensor is Tripped DDR SDRAM Throttling begins based on the setting in RTT
	 */
	dtc_reg |= (3 << 28);

	/* Read Counter Based Power Throttle Control (RCTC):
	 *   0 = 85%
	 */
	dtc_reg |= (0 << 24);

	/* Write Counter Based Power Throttle Control (WCTC):
	 *   0 = 85%
	 */
	dtc_reg |= (0 << 20);

	/* Read Thermal Based Power Throttle Control (RTTC):
	 *   0xA = 20%
	 */
	dtc_reg |= (0xA << 16);

	/* Write Thermal Based Power Throttle Control (WTTC):
	 *   0xA = 20%
	 */
	dtc_reg |= (0xA << 12);

	/* Counter Based Throttle Lock (CTLOCK): */
	dtc_reg |= (0 << 11);

	/* Thermal Throttle Lock (TTLOCK): */
	dtc_reg |= (0 << 10);

	/* Thermal Power Throttle Control fields Enable: */
	dtc_reg |= (1 << 9);

	/* High Priority Stream Throttling Enable: */
	dtc_reg |= (0 << 8);

	/* Global DDR SDRAM Sampling Window (GDSW): */
	dtc_reg |= 0xff;
	PRINTK_DEBUG("DTC: %08x\n", dtc_reg);
	pci_write_config32(NORTHBRIDGE_MMC, DTC, dtc_reg);
}

static void spd_update(u8 reg, u32 new_value)
{
#if CONFIG_DEBUG_RAM_SETUP
	u32 value1 = pci_read_config32(NORTHBRIDGE_MMC, reg);
#endif
	pci_write_config32(NORTHBRIDGE_MMC, reg, new_value);
#if CONFIG_DEBUG_RAM_SETUP
	u32 value2 = pci_read_config32(NORTHBRIDGE_MMC, reg);
	PRINTK_DEBUG("update reg %02x, old: %08x, new: %08x, read back: %08x\n", reg, value1, new_value, value2);
#endif
}

/* if ram still doesn't work do this function */
static void spd_set_undocumented_registers(void)
{
	spd_update(0x74, 0x00000001);
	spd_update(0x78, 0x001fe974);
	spd_update(0x80, 0x00af0039);
	spd_update(0x84, 0x0000033c);
	spd_update(0x88, 0x00000010);

	spd_update(0xc0, 0x00000003);
}

static void northbridge_set_registers(void)
{
	u16 value;
	int video_memory = 0;

	printk(BIOS_DEBUG, "Setting initial Northbridge registers....\n");

	/* Set the value for Fixed DRAM Hole Control Register */
	pci_write_config8(NORTHBRIDGE, FDHC, 0x00);

	/* Set the value for Programable Attribute Map Registers
	 * Ideally, this should be R/W for as many ranges as possible.
	 */
	pci_write_config8(NORTHBRIDGE, PAM0, 0x30);
	pci_write_config8(NORTHBRIDGE, PAM1, 0x33);
	pci_write_config8(NORTHBRIDGE, PAM2, 0x33);
	pci_write_config8(NORTHBRIDGE, PAM3, 0x33);
	pci_write_config8(NORTHBRIDGE, PAM4, 0x33);
	pci_write_config8(NORTHBRIDGE, PAM5, 0x33);
	pci_write_config8(NORTHBRIDGE, PAM6, 0x33);

	/* Set the value for System Management RAM Control Register */
	pci_write_config8(NORTHBRIDGE, SMRAM, 0x02);

	/* Set the value for GMCH Control Register #1 */
	switch (CONFIG_VIDEO_MB) {
	case 1: /* 1M of memory */
		video_memory = 0x1;
		break;
	case 4: /* 4M of memory */
		video_memory = 0x2;
		break;
	case 8: /* 8M of memory */
		video_memory = 0x3;
		break;
	case 16: /* 16M of memory */
		video_memory = 0x4;
		break;
	case 32: /* 32M of memory */
		video_memory = 0x5;
		break;
	default: /* No memory */
		pci_write_config16(NORTHBRIDGE, GMC, pci_read_config16(NORTHBRIDGE, GMC) | 1);
		video_memory = 0x0;
	}

	value = pci_read_config16(NORTHBRIDGE, GGC);
	value |= video_memory << 4;
	if (video_memory == 0) {
		value &= ~(1 < 1);
	} else
		value |= (1 < 1);
	pci_write_config16(NORTHBRIDGE, GGC, value);

	/* AGPCMD: disable AGP, Data-Rate: 1x */
	pci_write_config32(NORTHBRIDGE, AGPCMD, 0x00000001);

	pci_write_config8(NORTHBRIDGE, AMTT, 0x20);
	pci_write_config8(NORTHBRIDGE, LPTT, 0x10);

	printk(BIOS_DEBUG, "Initial Northbridge registers have been set.\n");
}

static void sdram_set_spd_registers(void)
{
	uint8_t dimm_mask;

	PRINTK_DEBUG("Reading SPD data...\n");

	dimm_mask = spd_get_supported_dimms();

	if (dimm_mask == 0) {
		print_debug("No usable memory for this controller\n");
	} else {
		PRINTK_DEBUG("DIMM MASK: %02x\n", dimm_mask);

		spd_set_row_attributes(dimm_mask);
		spd_set_dram_controller_mode(dimm_mask);
		spd_set_dram_timing(dimm_mask);
		spd_set_dram_size(dimm_mask);
		spd_set_dram_pwr_management();
		spd_set_dram_throttle_control();
		spd_set_undocumented_registers();
	}

	/* Setup Initial Northbridge Registers */
	northbridge_set_registers();
}

