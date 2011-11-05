#include <spd.h>
#include <stdlib.h>
#include <sdram_mode.h>

#include "i865.h"
#include "raminit.h"

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

static inline int spd_read_byte(unsigned device, unsigned address)
//static int spd_read_byte(unsigned device, unsigned address)
{
        return smbus_read_byte(device, address);
}

static void set_initialization_complete(void)
{
	printk(BIOS_DEBUG, "Setting initialization complete (DRC bit 29)\n");
	MCHBAR32(DRC) |= RAM_INITIALIZATION_COMPLETE;
}

//static void do_ram_command(u32 command)
static void do_ram_command(uint8_t command, uint16_t jedec_mode_bits)
{
	int i;
	u32 reg32;
//	uint8_t dimm_start_32M_multiple = 0;
	uint8_t dimm_start_64M_multiple = 0;
	uint16_t i855_mode_bits = jedec_mode_bits;

	/* Configure the RAM command. */
//	reg32 = pci_read_config32(NORTHBRIDGE_MMC, DRC);
	reg32 = MCHBAR32(DRC);
	reg32 &= ~(7 << 4);
	reg32 |= (command << 4);
	printk(BIOS_DEBUG, "  Sending RAM command 0x%08x\n", reg32);
//	pci_write_config32(NORTHBRIDGE_MMC, DRC, reg32);
	MCHBAR32(DRC) = reg32;

        // RAM_COMMAND_NORMAL is an exception.
        // It affects only the memory controller and does not need to be "sent" to the DIMMs.

        if (command != RAM_COMMAND_NORMAL) {

                // Send the command to all DIMMs by accessing a memory location within each
                // NOTE: for mode select commands, some of the location address bits
                // are part of the command

                // Map JEDEC mode bits to i855
                if (command == RAM_COMMAND_MRS || command == RAM_COMMAND_EMRS) {
			/* Host address lines [13:3] map to DIMM address lines [11, 9:0] */
			// i855_mode_bits = ((jedec_mode_bits & 0x800) << (13 - 11)) | ((jedec_mode_bits & 0x3ff) << (12 - 9));
			/* Host address lines [13:3] map to DIMM address lines [5:1] */
			// i855_mode_bits = ((jedec_mode_bits & 0x800) << (13 - 11)) | ((jedec_mode_bits & 0x3ff) << (6 - 1));
			i855_mode_bits = ((jedec_mode_bits & 0x800) << (13 - 11)) | ((jedec_mode_bits & 0x3f) << (6 - 1));
                }

                for (i = 0; i < (DIMM_SOCKETS * 2); ++i) {
//                        uint8_t dimm_end_32M_multiple = pci_read_config8(NORTHBRIDGE_MMC, DRB + i);
			/* i865 uses a granularity of 64 megabyte */
                        uint8_t dimm_end_64M_multiple = MCHBAR8(DRB + i);
//                        if (dimm_end_32M_multiple > dimm_start_32M_multiple) {
                        if (dimm_end_64M_multiple > dimm_start_64M_multiple) {

//                                uint32_t dimm_start_address = dimm_start_32M_multiple << 25;
                                uint32_t dimm_start_address = dimm_start_64M_multiple << 25;
				printk(BIOS_DEBUG, "  Sending RAM command to 0x%08x\n", dimm_start_address + i855_mode_bits);
                                //read32(dimm_start_address + i855_mode_bits);
								printk(BIOS_DEBUG, "  /* skipping read32() */\n");
								printk(BIOS_DEBUG, "  Sent. dimm_start_address = 0x%x and i855_mode_bits = 0x%x\n", dimm_start_address, i855_mode_bits);

                                // Set the start of the next DIMM
//                                dimm_start_32M_multiple = dimm_end_32M_multiple;
                                dimm_start_64M_multiple = dimm_end_64M_multiple;
                        }
		}
	}
}

static void sdram_enable(void)
{
	int i;

	print_debug("Ram enable 1\n");
	delay();
	delay();

	/* NOP command */
	printk(BIOS_DEBUG, " NOP\n");
	do_ram_command(RAM_COMMAND_NOP, 0);
	delay();
	delay();
	delay();

	/* Pre-charge all banks (at least 200 us after NOP) */
	printk(BIOS_DEBUG, " Pre-charging all banks\n");
	do_ram_command(RAM_COMMAND_PRECHARGE, 0);
	delay();
	delay();
	delay();

	printk(BIOS_DEBUG, "Ram enable 4\n");
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
	printk(BIOS_DEBUG, " 8 CBR refreshes\n");
	for(i = 0; i < 8; i++) {
		do_ram_command(RAM_COMMAND_CBR, 0);
		delay();
		delay();
		delay();
	}

	printk(BIOS_DEBUG, "Ram enable 8\n");
	do_ram_command(RAM_COMMAND_MRS, VG85X_MODE | SDRAM_MODE_NORMAL);

	/* Set GME-M Mode Select bits back to NORMAL operation mode */
	printk(BIOS_DEBUG, " Normal operation mode\n");
	do_ram_command(RAM_COMMAND_NORMAL, 0);
/*	delay();
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
*/
	/* dump_pci_devices(); */

	/*
	print_debug("\n\n***** RAM TEST *****\n");
	ram_check(0, 0xa0000);
	ram_check(0x100000, 0x40000000);
	*/
}

//static void ram_read32(u8 dimm_start, u32 offset)
//{
	/* not implemented */
//}

//static void initialize_dimm_rows(void)
//{
	/* copied from i82830/raminit.c */
	/* not implemented */
//}

static void die_on_spd_error(int spd_return_value)
{
	if (spd_return_value < 0)
		printk(BIOS_DEBUG, "Error reading SPD info: got %d\n", spd_return_value);
/*
	if (spd_return_value < 0)
		die("Error reading SPD info\n");
*/
}

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

	if (value > 2)
		printk(BIOS_DEBUG, "Bad SPD value\n");

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

		switch (value) {
		case 2:
			width.side2 = width.side1;
			break;

		default:
			break;
		}
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

// static void set_dram_row_boundaries(void) { /* TODO */ }
// static void set_dram_row_attributes(void) { /* TODO */ }
// static void set_dram_timing(void) { /* TODO */ }
// static void sdram_set_registers(void) { /* TODO */ }

static uint8_t spd_get_supported_dimms(void)
{
	int i;
	uint8_t dimm_mask = 0;

//	for (i = 0; i < DIMM_SOCKETS; i++) {
	for (i = 0; i < (2 * DIMM_SOCKETS); i++) {
		u8 dimm = DIMM0 + i;

		struct dimm_size page_size;
		struct dimm_size sdram_width;

		int spd_value;

		if (dimm == 0)
			continue;

		if (spd_read_byte(dimm, SPD_MEMORY_TYPE) != SPD_MEMORY_TYPE_SDRAM_DDR)
			// print a little warning, maybe?
			continue;

		if ((spd_value = spd_read_byte(dimm, SPD_MODULE_VOLTAGE)) != SPD_VOLTAGE_SSTL2) {
			printk(BIOS_DEBUG, "Skipping DIMM with unsupported voltage: %02x\n", spd_value);
			continue;
		}

		page_size = sdram_spd_get_page_size(dimm);
		sdram_width = sdram_spd_get_width(dimm);

		// Validate DIMM page size
		// The i855 only supports page sizes of 4, 8, 16 KB per channel
		// NOTE:  4 KB =  32 Kb = 2^15
		//       16 KB = 128 Kb = 2^17

		if ((page_size.side1 < 15) || (page_size.side1 > 17)) {
			printk(BIOS_DEBUG, "Skipping DIMM with unsupported page size: %d\n", page_size.side1);
			continue;
		}

		// If DIMM is double-sided, verify side2 page size
		if (page_size.side2 != 0) {
			if ((page_size.side2 < 15) || (page_size.side2 > 17)) {
				printk(BIOS_DEBUG, "Skipping DIMM with unsupported page size: %d\n", page_size.side2);
				continue;
			}
		}
		// Validate SDRAM width
		// The i855 only supports x8 and x16 devices
		if ((sdram_width.side1 != 8) && (sdram_width.side1 != 16)) {
			printk(BIOS_DEBUG, "Skipping DIMM with unsupported width: %d\n", sdram_width.side2);
			continue;
		}

		// If DIMM is double-sided, verify side2 width
		if (sdram_width.side2 != 0) {
			if ((sdram_width.side2 != 8)
			    && (sdram_width.side2 != 16)) {
				printk(BIOS_DEBUG, "Skipping DIMM with unsupported width: %d\n", sdram_width.side2);
				continue;
			}
		}
		// Made it through all the checks, this DIMM is usable
		dimm_mask |= (1 << i);
	}

	return dimm_mask;
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
//			row_attributes |= 0x77 << (i << 3);
			row_attributes |= 0x0 << (i << 3); // default value of DRA on i865 is 0x0
			continue;	// This DIMM not usable
		}

		// Get the relevant parameters via SPD
		page_size = sdram_spd_get_page_size(dimm);
		sdram_width = sdram_spd_get_width(dimm);

		// Update the DRAM Row Attributes.
		// Page size is encoded as log2(page size in bits) - log2(2 KB) or 4 KB == 1, 8 KB == 3, 16KB == 3
// i865: Page size is encoded as log2(page size in bits) - log2(2 KB) or 4 KB == 0, 8 KB == 1, 16KB == 2, 32KB == 3
		// NOTE:  2 KB =  16 Kb = 2^14
//		row_attributes |= (page_size.side1 - 14) << (i << 3);	// Side 1 of each DIMM is an EVEN row
		// i865: NOTE:  4 KB =  32 Kb = 2^15
		row_attributes |= (page_size.side1 - 15) << (i << 3);	// Side 1 of each DIMM is an EVEN row

		if (sdram_width.side2 > 0)
//			row_attributes |= (page_size.side2 - 14) << ((i << 3) + 4);	// Side 2 is ODD
			row_attributes |= (page_size.side2 - 15) << ((i << 3) + 4);	// Side 2 is ODD
//		else
//			row_attributes |= 7 << ((i << 3) + 4); // "not populated", i865 doesn't have that bit

		/* on the asrock pi465gv (rev g/a 1.11),
		physical ram slots "DDR2" and "DDR1"
		are logically slots 2 and 1 (or 1 and 0) */
//		printk(BIOS_DEBUG, "DRA%d: 0x%x\n", i, row_attributes);
//		printk(BIOS_DEBUG, "DRA: row %d,%d: 0x%x\n", i, i + 1, row_attributes);
		printk(BIOS_DEBUG, "DRA: row %d,%d: 0x%x\n", i * 2, i * 2 + 1, row_attributes);
		MCHBAR8(DRA + i) = row_attributes;

		/* go to the next DIMM */
	}

//	PRINTK_DEBUG("DRA: %04x\n", row_attributes);

	/* Write the new row attributes register */
//	pci_write_config16(NORTHBRIDGE_MMC, DRA, row_attributes);
}

static void spd_set_dram_controller_mode(uint8_t dimm_mask)
{
	int i;

	// Initial settings
//	u32 controller_mode = pci_read_config32(NORTHBRIDGE_MMC, DRC);
	u32 controller_mode = MCHBAR32(DRC);
//	u32 system_refresh_mode = (controller_mode >> 7) & 7;
	u32 system_refresh_mode = (controller_mode >> 8) & 7; // i865: RMS is at bits 10:8

//	controller_mode |= (1 << 20);  // ECC
//	controller_mode |= (1 << 15);  // RAS lockout
//	controller_mode |= (1 << 12);  // Address Tri-state enable (ADRTRIEN), FIXME: how is this detected?????
//	controller_mode |= (2 << 10);  // FIXME: Undocumented, really needed?????

//	for (i = 0; i < DIMM_SOCKETS; i++) {
	for (i = 0; i < (2 * DIMM_SOCKETS); i++) {
		u8 dimm = DIMM0 + i;
		uint32_t dimm_refresh_mode;
		int value;
//		u8 tRCD, tRP;

		if (!(dimm_mask & (1 << i))) {
			continue;	// This DIMM not usable
		}

		// Disable ECC mode if any one of the DIMMs does not support ECC
// no ECC on i865, disabling
//		value = spd_read_byte(dimm, SPD_DIMM_CONFIG_TYPE);
//		die_on_spd_error(value);
//		if (value != ERROR_SCHEME_ECC)
//			controller_mode &= ~(3 << 20);

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
//		tRCD = spd_read_byte(dimm, SPD_tRCD);
//		tRP = spd_read_byte(dimm, SPD_tRP);
// no RAS lockout on i865
//		if (tRCD != tRP) {
//			PRINTK_DEBUG(" Disabling RAS lockouk due to tRCD (%d) != tRP (%d)\n", tRCD, tRP);
//			printk(BIOS_DEBUG, " Disabling RAS lockouk due to tRCD (%d) != tRP (%d)\n", tRCD, tRP);
//			controller_mode &= ~(1 << 15);
//		}

		/* go to the next DIMM */
	}

//	controller_mode &= ~(7 << 7);
//	controller_mode |= (system_refresh_mode << 7);
	controller_mode &= ~(7 << 8);
	controller_mode |= (system_refresh_mode << 8);
//	PRINTK_DEBUG("DRC: %08x\n", controller_mode);
	printk(BIOS_DEBUG, "DRC: %08x\n", controller_mode);

//	pci_write_config32(NORTHBRIDGE_MMC, DRC, controller_mode);
	MCHBAR32(DRC) = controller_mode;
}

static void spd_set_dram_timing(uint8_t dimm_mask)
{
	int i;
	u32 dram_timing;

	// CAS# latency bitmasks in SPD_ACCEPTABLE_CAS_LATENCIES format
	// NOTE: i82822 supports only 2.0 and 2.5
//	uint32_t system_compatible_cas_latencies = SPD_CAS_LATENCY_2_0 | SPD_CAS_LATENCY_2_5;

/*  SPD_CAS_LATENCY_2_0 | SPD_CAS_LATENCY_2_5 | SPD_CAS_LATENCY_3_0 == 0x1c */
	uint32_t system_compatible_cas_latencies = SPD_CAS_LATENCY_2_0 | SPD_CAS_LATENCY_2_5 | SPD_CAS_LATENCY_3_0;
//	uint32_t system_compatible_cas_latencies = DRT_CAS_2_0 | DRT_CAS_2_5 | DRT_CAS_3_0;
	uint8_t slowest_row_precharge = 0;
	uint8_t slowest_ras_cas_delay = 0;
	uint8_t slowest_active_to_precharge_delay = 0;

//	for (i = 0; i < DIMM_SOCKETS; i++) {
	for (i = 0; i < (2 * DIMM_SOCKETS); i++) {
		u8 dimm = DIMM0 + i;
		int value;
		uint32_t current_cas_latency;
		uint32_t dimm_compatible_cas_latencies;
		if (!(dimm_mask & (1 << i)))
			continue;	// This DIMM not usable

		value = spd_read_byte(dimm, SPD_ACCEPTABLE_CAS_LATENCIES);
		printk(BIOS_DEBUG, "SPD_ACCEPTABLE_CAS_LATENCIES: %d\n", value);
		die_on_spd_error(value);

		dimm_compatible_cas_latencies = value & 0x7f;	// Start with all supported by DIMM
		printk(BIOS_DEBUG, "dimm_compatible_cas_latencies #1: %d\n", dimm_compatible_cas_latencies);

		current_cas_latency = 1 << log2(dimm_compatible_cas_latencies);	// Max supported by DIMM
		printk(BIOS_DEBUG, "current_cas_latency: %d\n", current_cas_latency);

		// Can we support the highest CAS# latency?
		value = spd_read_byte(dimm, SPD_MIN_CYCLE_TIME_AT_CAS_MAX);
		die_on_spd_error(value);
		printk(BIOS_DEBUG, "SPD_MIN_CYCLE_TIME_AT_CAS_MAX: %d.%d\n", value >> 4, value & 0xf);

		// NOTE: At 133 MHz, 1 clock == 7.52 ns
		if (value > 0x75) {
			// Our bus is too fast for this CAS# latency
			// Remove it from the bitmask of those supported by the DIMM that are compatible
			dimm_compatible_cas_latencies &= ~current_cas_latency;
			printk(BIOS_DEBUG, "dimm_compatible_cas_latencies #2: %d\n", dimm_compatible_cas_latencies);
		}
		// Can we support the next-highest CAS# latency (max - 0.5)?

		current_cas_latency >>= 1;
		if (current_cas_latency != 0) {
			value = spd_read_byte(dimm, SPD_SDRAM_CYCLE_TIME_2ND);
			die_on_spd_error(value);
			printk(BIOS_DEBUG, "SPD_SDRAM_CYCLE_TIME_2ND: %d.%d\n", value >> 4, value & 0xf);
			if (value > 0x75) {
				dimm_compatible_cas_latencies &= ~current_cas_latency;
				printk(BIOS_DEBUG, "dimm_compatible_cas_latencies #2: %d\n", dimm_compatible_cas_latencies);
			}
		}
		// Can we support the next-highest CAS# latency (max - 1.0)?
		current_cas_latency >>= 1;
		if (current_cas_latency != 0) {
			value = spd_read_byte(dimm, SPD_SDRAM_CYCLE_TIME_3RD);
			printk(BIOS_DEBUG, "SPD_SDRAM_CYCLE_TIME_3RD: %d.%d\n", value >> 4, value & 0xf);
			die_on_spd_error(value);
			if (value > 0x75) {
				dimm_compatible_cas_latencies &= ~current_cas_latency;
				printk(BIOS_DEBUG, "dimm_compatible_cas_latencies #2: %d\n", dimm_compatible_cas_latencies);
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
	printk(BIOS_DEBUG, "CAS latency: %d\n", system_compatible_cas_latencies);

//	dram_timing = pci_read_config32(NORTHBRIDGE_MMC, DRT);
	dram_timing = MCHBAR32(DRT);
//	dram_timing &= ~(DRT_CAS_MASK | DRT_TRP_MASK | DRT_RCD_MASK);
	dram_timing &= ~(DRT_CAS_MASK | DRT_TRP_MASK | DRT_TRCD_MASK);
	printk(BIOS_DEBUG, "DRT: %08x\n", dram_timing);

	if (system_compatible_cas_latencies & SPD_CAS_LATENCY_2_0) {
		dram_timing |= DRT_CAS_2_0;
	} else if (system_compatible_cas_latencies & SPD_CAS_LATENCY_2_5) {
		dram_timing |= DRT_CAS_2_5;
	} else if (system_compatible_cas_latencies & SPD_CAS_LATENCY_3_0) {
		dram_timing |= DRT_CAS_3_0;
	} else
		die("No CAS# latencies compatible with all DIMMs!!\n");

//	uint32_t current_cas_latency = dram_timing & DRT_CAS_MASK;

	/* tRP */

	printk(BIOS_DEBUG, "slowest_row_precharge: %d.%d\n", slowest_row_precharge >> 2, slowest_row_precharge & 0x3);
	// i855 supports only 2, 3 or 4 clocks for tRP
	if (slowest_row_precharge > (30 << 2)) // 30 = 11110b = tRCD = 2 dram clocks, tCL = reserved
		die("unsupported DIMM tRP");	//  > 30.0 ns: 5 or more clocks
	else if (slowest_row_precharge > ( (22 << 2) | (2 << 0) ))
	// 22 = 10110b = tRCD = 2 dram clocks, tCL = 2
	// 2 = 10b = 2 dram clocks (tRP)
		dram_timing |= DRT_TRP_4;	//  > 22.5 ns: 4 or more clocks
	else if (slowest_row_precharge > (15 << 2)) // 15 = 1111b = tRCD = reserved, tCL = 2
		dram_timing |= DRT_TRP_3;	//  > 15.0 ns: 3 clocks
	else
		dram_timing |= DRT_TRP_2;	// <= 15.0 ns: 2 clocks

	/*  tRCD */

	printk(BIOS_DEBUG, "slowest_ras_cas_delay: %d.%d\n", slowest_ras_cas_delay >> 2, slowest_ras_cas_delay & 0x3);
	// i855 supports only 2, 3 or 4 clocks for tRCD
	if (slowest_ras_cas_delay > ((30 << 2)))
		die("unsupported DIMM tRCD");	//  > 30.0 ns: 5 or more clocks
	else if (slowest_ras_cas_delay > ((22 << 2) | (2 << 0)))
//		dram_timing |= DRT_RCD_4;	//  > 22.5 ns: 4 or more clocks
		dram_timing |= DRT_TRCD_4;	//  > 22.5 ns: 4 or more clocks
	else if (slowest_ras_cas_delay > (15 << 2))
//		dram_timing |= DRT_RCD_3;	//  > 15.0 ns: 3 clocks
		dram_timing |= DRT_TRCD_3;	//  > 15.0 ns: 3 clocks
	else
//		dram_timing |= DRT_RCD_2;	// <= 15.0 ns: 2 clocks
		dram_timing |= DRT_TRCD_2;	// <= 15.0 ns: 2 clocks

	/* tRAS, min */

	printk(BIOS_DEBUG, "slowest_active_to_precharge_delay: %d\n", slowest_active_to_precharge_delay);
	// i855 supports only 5, 6, 7 or 8 clocks for tRAS
	// 5 clocks ~= 37.6 ns, 6 clocks ~= 45.1 ns, 7 clocks ~= 52.6 ns, 8 clocks ~= 60.1 ns
	// TODO
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
//	dram_timing &= ~(3 << 28);
//	if (current_cas_latency == DRT_CAS_2_0)
//		dram_timing |= (2 << 28);	// 2 clocks
//	else
//		dram_timing |= (1 << 28);	// 3 clocks

	/*
	 * Back to Back Read-Write command spacing (DDR, same or different Rows/Bank)
	 */
//	dram_timing &= ~(3 << 26);
//	if (current_cas_latency == DRT_CAS_2_0)
//		dram_timing |= (2 << 26);	// 5 clocks
//	else
//		dram_timing |= (1 << 26);	// 6 clocks

	/*
	 * Back To Back Read-Read commands spacing (DDR, different Rows):
	 */
//	dram_timing &= ~(1 << 25);
//	dram_timing |= (1 << 25);	// 3 clocks

	printk(BIOS_DEBUG, "DRT: %08x\n", dram_timing);
//	pci_write_config32(NORTHBRIDGE_MMC, DRT, dram_timing);
	MCHBAR32(DRT) = dram_timing;
}

static void spd_set_dram_size(uint8_t dimm_mask)
{
	int i;
	int total_dram = 0;
	uint32_t drb_reg = 0;

//	for (i = 0; i < DIMM_SOCKETS; i++) {
	for (i = 0; i < (2 * DIMM_SOCKETS); i++) {
		u8 dimm = DIMM0 + i;
		struct dimm_size sz;

		if (!(dimm_mask & (1 << i))) {
			/* fill values even for not present DIMMs */
//			drb_reg |= (total_dram << (i * 16));
//			drb_reg |= (total_dram << ((i * 16) + 8));
			drb_reg |= (total_dram << (i * 32));
			drb_reg |= (total_dram << ((i * 32) + 8));

			continue;	// This DIMM not usable
		}
		sz = spd_get_dimm_size(dimm);

//		total_dram += (1 << (sz.side1 - 28));
//		drb_reg |= (total_dram << (i * 16));
		total_dram += (1 << (sz.side1 - 30));
		drb_reg |= (total_dram << (i * 32));

//		total_dram += (1 << (sz.side2 - 28));
//		drb_reg |= (total_dram << ((i * 16) + 8));
		total_dram += (1 << (sz.side2 - 30));
		drb_reg |= (total_dram << ((i * 32) + 8));

		printk(BIOS_DEBUG, "DRB: %08x\n", drb_reg);
		MCHBAR8(DRB + i) = drb_reg;
	}
//	printk(BIOS_DEBUG, "DRB: %08x\n", drb_reg);
//	pci_write_config32(NORTHBRIDGE_MMC, DRB, drb_reg);
}

static void enable_refresh(void)
{
	printk(BIOS_DEBUG, "Enabling refresh (RAM_COMMAND_REFRESH)\n");
	MCHBAR32(DRC) |= RAM_COMMAND_REFRESH << 8;
}

/* from src/northbridge/intel/i945/raminit.c and util/inteltool/memory.c */
static void sdram_dump_mchbar_registers(void)
{
	int i;

	printk(BIOS_DEBUG, "Dumping MCHBAR registers:\n");
	for (i = 0; i < 0xffff; i += 4) {
		if ( (MCHBAR32(i) == 0) || (MCHBAR32(i) == 0xffffffff) )
			continue;
		switch (i) {
		case (DRB + 0):
			printk(BIOS_DEBUG, " DRB0: 0x%x\n", MCHBAR32(i));
		case (DRB + 1):
			printk(BIOS_DEBUG, " DRB1: 0x%x\n", MCHBAR32(i));
		case (DRB + 2):
			printk(BIOS_DEBUG, " DRB2: 0x%x\n", MCHBAR32(i));
		case (DRB + 3):
			printk(BIOS_DEBUG, " DRB3: 0x%x\n", MCHBAR32(i));
		case (DRB + 4):
			printk(BIOS_DEBUG, " DRB4: 0x%x\n", MCHBAR32(i));
		case (DRB + 5):
			printk(BIOS_DEBUG, " DRB5: 0x%x\n", MCHBAR32(i));
		case (DRB + 6):
			printk(BIOS_DEBUG, " DRB6: 0x%x\n", MCHBAR32(i));
		case (DRB + 7):
			printk(BIOS_DEBUG, " DRB7: 0x%x\n", MCHBAR32(i));

		case (DRA + 0):
			printk(BIOS_DEBUG, " DRA0: 0x%x\n", MCHBAR32(i));
		case (DRA + 1):
			printk(BIOS_DEBUG, " DRA1: 0x%x\n", MCHBAR32(i));
		case (DRA + 2):
			printk(BIOS_DEBUG, " DRA2: 0x%x\n", MCHBAR32(i));
		case (DRA + 3):
			printk(BIOS_DEBUG, " DRA3: 0x%x\n", MCHBAR32(i));
		case (DRA + 4):
			printk(BIOS_DEBUG, " DRA4: 0x%x\n", MCHBAR32(i));
		case (DRA + 5):
			printk(BIOS_DEBUG, " DRA5: 0x%x\n", MCHBAR32(i));
		case (DRA + 6):
			printk(BIOS_DEBUG, " DRA6: 0x%x\n", MCHBAR32(i));
		case (DRA + 7):
			printk(BIOS_DEBUG, " DRA7: 0x%x\n", MCHBAR32(i));

		case DRT:
			printk(BIOS_DEBUG, " DRT: 0x%x\n", MCHBAR32(i));
		case DRC:
			printk(BIOS_DEBUG, " DRC: 0x%x\n", MCHBAR32(i));
		default:
			printk(BIOS_DEBUG, "0x%x: 0x%x\n", i, MCHBAR32(i));
		}
	}
}

static void early_northbridge_set_registers(void)
{
	u8 reg8;

	/* Undocumented register */
	reg8 = pci_read_config8(NORTHBRIDGE, 0xf4);

	/*
	Writing 0x2 to northbridge register 0xf4
	exposes the PCI registers of device 6.
	*/
	//reg8 |= 0x2; /* guesswork: the default value is 0x0 */
	reg8 = 0x2;

	printk(BIOS_DEBUG, "Enabling access to (the registers of) device 6\n");
	pci_write_config8(NORTHBRIDGE, 0xf4, reg8);

	printk(BIOS_DEBUG, "Configuring BAR6 as 0x%x\n", DEFAULT_MCHBAR);
	pci_write_config32(NORTHBRIDGE_MMC, BAR6, DEFAULT_MCHBAR);

	printk(BIOS_DEBUG, "Enabling Memory Access Enable (MAE) in PCICMD6\n");
	pci_write_config16(NORTHBRIDGE_MMC, PCICMD6, 0x2);
}

static void northbridge_set_pam(void)
{
	printk(BIOS_DEBUG, "Configuring PAM0..PAM6\n");
	pci_write_config8(NORTHBRIDGE, PAM0, 0x10);
	pci_write_config8(NORTHBRIDGE, PAM1, 0x11);
	pci_write_config8(NORTHBRIDGE, PAM2, 0x1);
	pci_write_config8(NORTHBRIDGE, PAM3, 0x0);
	pci_write_config8(NORTHBRIDGE, PAM4, 0x0);
	pci_write_config8(NORTHBRIDGE, PAM5, 0x33);
	pci_write_config8(NORTHBRIDGE, PAM6, 0x33);
}

static void northbridge_set_registers(void)
{
	printk(BIOS_DEBUG, "Configuring APBASE\n");
	pci_write_config32(NORTHBRIDGE, APBASE, 0xfe800008);

	printk(BIOS_DEBUG, "Configuring APSIZE\n");
	pci_write_config8(NORTHBRIDGE, APSIZE, 0x3f);

	printk(BIOS_DEBUG, "Configuring GC\n"); /* IVD=0, IGDIS=0, GMS=8 megabyte */
	pci_write_config8(NORTHBRIDGE, GC, 0x34);

	printk(BIOS_DEBUG, "Configuring AMTT\n");
	pci_write_config8(NORTHBRIDGE, AMTT, 0x20);

	printk(BIOS_DEBUG, "Configuring LPTT\n");
	pci_write_config8(NORTHBRIDGE, LPTT, 0x10);

	printk(BIOS_DEBUG, "Configuring GMCHCFG\n");
	pci_write_config16(NORTHBRIDGE, GMCHCFG, 0x40d);

	printk(BIOS_DEBUG, "Configuring TOUD\n");
	pci_write_config16(NORTHBRIDGE, TOUD, 0x1f80);

	printk(BIOS_DEBUG, "Configuring PCISTS\n");
	pci_write_config16(NORTHBRIDGE, PCISTS, 0x2009);

	printk(BIOS_DEBUG, "Writing SVID and SID\n");
	pci_write_config16(NORTHBRIDGE, SVID, 0x1849);
	pci_write_config16(NORTHBRIDGE, SID, 0x2570);

	printk(BIOS_DEBUG, "Configuring ERRSTS\n");
	pci_write_config16(NORTHBRIDGE, ERRSTS, 0x100);
}

static void northbridge_set_undocumented_registers(void)
{
	u8 reg8;

	printk(BIOS_DEBUG, "Configuring undocumented register 0x54\n");
	/*
	guesswork, the default value is 0x18 and 0x1c is the value from lspci -nnvvvxxx.
	conclusion: add 0x4 */
	reg8 = pci_read_config8(NORTHBRIDGE, 0x54);
	reg8 |= 0x4;
	pci_write_config8(NORTHBRIDGE, 0x54, reg8);

	printk(BIOS_DEBUG, "Configuring undocumented register 0x9d\n");
	reg8 = pci_read_config8(NORTHBRIDGE, 0xc9d);
	reg8 |= 0x8;
	pci_write_config8(NORTHBRIDGE, 0x9d, reg8);
}

//static void hardcoded_initialization(void)
//{
	// int i;

//	printk(BIOS_DEBUG, "Configuring DRT\n");
//	MCHBAR32(DRT) = 0x56e40d00; /* single-channel, 512MB in the first (physical) slot */
//	printk(BIOS_DEBUG, "  DRT after configuring (should be 0x56e40d00): 0x%x\n", MCHBAR32(DRT));

//	printk(BIOS_DEBUG, "Configuring DRB0..DRB7\n");

	/* single-channel, 512MB in the first (physical) slot */
//	MCHBAR8(DRB + 0) = 0x4;
//	MCHBAR8(DRB + 1) = 0x8;
//	MCHBAR8(DRB + 2) = 0x8;
//	MCHBAR8(DRB + 3) = 0x8;
//	MCHBAR8(DRB + 4) = 0x8;
//	MCHBAR8(DRB + 5) = 0x8;
//	MCHBAR8(DRB + 6) = 0x8;
//	MCHBAR8(DRB + 7) = 0x8;
	// for (i = 0; i < 8; i++) {
		// printk(BIOS_DEBUG, "  DRB%i after configuring (should be 0x4 for DRB0, 0x8 for DRB1-7): 0x%x\n", i, MCHBAR8(DRB + i));
	// }

//	printk(BIOS_DEBUG, "Configuring DRA0..DRA7\n");

	/* single-channel, 512MB in the first (physical) slot */
//	MCHBAR8(DRA + 0) = 0x0;
//	MCHBAR8(DRA + 1) = 0x11; // 8 kilobyte on each row
//	MCHBAR8(DRA + 2) = 0x0;
//	MCHBAR8(DRA + 3) = 0x0;
	// for (i = 0; i < 8; i++) {
		// printk(BIOS_DEBUG, "  DRA%i after configuring (should be 0x0 for DRA0, DRA2 and DRA3, 0x11 for DRA1: 0x%x\n",
			// i, MCHBAR8(DRA + i));
	// }

	// BAR6_32(DRC) = 0x20104271;

	// printk(BIOS_DEBUG, "All aboard the undocumented registers train :(\n");

/*
	MCHBAR32(0x6c) = 0xa801;
	MCHBAR32(0xb0) = 0x40830;
	MCHBAR32(0x100) = 0x89b;
	MCHBAR32(0x104) = 0x4ad;
	MCHBAR32(0x130) = 0x88d;
	MCHBAR32(0x138) = 0x210843c;
	MCHBAR32(0x140) = 0x1721c;
	MCHBAR32(0x144) = 0xe380fff;
	MCHBAR32(0x170) = 0x1a00018b;
	MCHBAR32(0x1a0) = 0x3d0524fd;
	MCHBAR32(0x200) = 0x1;
	MCHBAR32(0x208) = 0x3937;
	MCHBAR32(0x300) = 0x32;
	MCHBAR32(0x800) = 0xdddddddd;
*/
	// sdram_dump_mchbar_registers(); // from i945
//}

static void sdram_initialize(void)
//void sdram_initialize(void)
{
	/* First things first. */
	early_northbridge_set_registers();

	uint8_t dimm_mask;

	printk(BIOS_DEBUG, "Reading SPD data...\n");
	dimm_mask = spd_get_supported_dimms();

	if (dimm_mask == 0) {
		printk(BIOS_DEBUG, "No usable memory for this controller\n");
	}
	else {
		printk(BIOS_DEBUG, "DIMM mask: 0x%x\n", dimm_mask);

		spd_set_row_attributes(dimm_mask); // done?
		spd_set_dram_controller_mode(dimm_mask); // TODO
		spd_set_dram_timing(dimm_mask); // TODO: tRAS
		spd_set_dram_size(dimm_mask); // TODO
	}

	sdram_enable();

	enable_refresh();

	set_initialization_complete();

	/* Setup Initial Northbridge Registers */
	northbridge_set_pam();
	northbridge_set_registers();
	northbridge_set_undocumented_registers();

	sdram_dump_mchbar_registers();

	printk(BIOS_DEBUG, "Northbridge following SDRAM init:\n");

	dump_pci_device(NORTHBRIDGE);
	dump_pci_device(NORTHBRIDGE_MMC);
//	dump_pci_device(PCI_DEV(0, 0x1e, 0));
//	dump_pci_device(PCI_DEV(0, 0x1f, 0));
//	dump_pci_device(PCI_DEV(0, 0x1f, 3));
}
