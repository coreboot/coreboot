/* Memory information */
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _MEMORY_INFO_H_
#define _MEMORY_INFO_H_

#include <stdint.h>

#define DIMM_INFO_SERIAL_SIZE		4
#define DIMM_INFO_PART_NUMBER_SIZE	33
#define DIMM_INFO_TOTAL			32

/**
 * If this table is filled and put in CBMEM,
 * then these info in CBMEM will be used to generate smbios type 17 table
 *
 * Values are specified according to the JEDEC SPD Standard.
 */
struct dimm_info {
	/*
	 * Size of the module in MiB.
	 */
	uint32_t dimm_size;
	/*
	 * SMBIOS (not SPD) device type.
	 *
	 * See the smbios.h smbios_memory_type enum.
	 */
	uint16_t ddr_type;
	/*
	 * ddr_frequency is deprecated.
	 * Use max_speed_mts and configured_speed_mts instead.
	 */
	uint16_t ddr_frequency;
	uint8_t rank_per_dimm;
	/*
	 * Socket-ID
	 */
	uint8_t soc_num;
	/*
	 * Memory-Controller-ID
	 */
	uint8_t ctrlr_num;
	/*
	 * Channel-ID
	 */
	uint8_t channel_num;
	/*
	 * DIMM-ID
	 */
	uint8_t dimm_num;
	uint8_t bank_locator;
	/*
	 * SPD serial number.
	 */
	uint8_t serial[DIMM_INFO_SERIAL_SIZE];
	/*
	 * The last byte is '\0' for the end of string
	 *
	 * Must contain only printable ASCII.
	 */
	uint8_t module_part_number[DIMM_INFO_PART_NUMBER_SIZE];
	/*
	 * SPD Manufacturer ID
	 */
	uint16_t mod_id;
	/*
	 * SPD Module Type.
	 *
	 * See spd.h for valid values.
	 *
	 * e.g., SPD_RDIMM, SPD_SODIMM, SPD_MICRO_DIMM
	 */
	uint8_t mod_type;
	/*
	 * SPD bus width.
	 *
	 * Bits 0 - 2 encode the primary bus width:
	 *   0b000 = 8 bit width
	 *   0b001 = 16 bit width
	 *   0b010 = 32 bit width
	 *   0b011 = 64 bit width
	 *
	 * Bits 3 - 4 encode the extension bits (ECC):
	 *   0b00 = 0 extension bits
	 *   0b01 = 8 bit of ECC
	 *
	 * e.g.,
	 *   64 bit bus with 8 bits of ECC (72 bits total): 0b1011
	 *   64 bit bus with 0 bits of ECC (64 bits total): 0b0011
	 *
	 * See the smbios.h smbios_memory_bus_width enum.
	 */
	uint8_t bus_width;
	/*
	 * Voltage Level
	 */
	uint16_t vdd_voltage;
	/*
	 * Max speed in MT/s
	 * If the value is 0, ddr_frequency should be used instead.
	 */
	uint16_t max_speed_mts;
	/*
	 * Configured speed in MT/s
	 * If the value is 0, ddr_frequency should be used instead.
	 */
	uint16_t configured_speed_mts;
} __packed;

struct memory_info {
	/*
	 * SMBIOS error correction type.
	 * See the smbios.h smbios_memory_array_ecc enum.
	 */
	uint8_t ecc_type;
	/* Maximum capacity the DRAM controller/mainboard supports */
	uint32_t max_capacity_mib;
	/* Maximum number of DIMMs the DRAM controller/mainboard supports */
	uint16_t number_of_devices;

	/* active DIMM configuration */
	uint8_t dimm_cnt;
	struct dimm_info dimm[DIMM_INFO_TOTAL];
} __packed;

/*
 * mainboard_get_dram_part_num returns a DRAM part number override string
 *  return NULL = no part number override provided by mainboard
 *  return non-NULL = pointer to a string terminating in '\0'
 */
const char *mainboard_get_dram_part_num(void);
#endif
