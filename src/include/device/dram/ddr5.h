/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DEVICE_DRAM_DDR5_H
#define DEVICE_DRAM_DDR5_H

/**
 * @file ddr5.h
 *
 * \brief Utilities for decoding (LP)DDR5 info
 */

#include <device/dram/common.h>
#include <types.h>

/** Maximum SPD size supported */
#define SPD_SIZE_MAX_DDR5	1024

enum spd_dimm_type_ddr5 {
	SPD_DDR5_DIMM_TYPE_RDIMM = 0x01,
	SPD_DDR5_DIMM_TYPE_UDIMM = 0x02,
	SPD_DDR5_DIMM_TYPE_SODIMM = 0x03,
	SPD_DDR5_DIMM_TYPE_LRDIMM = 0x04,
	SPD_DDR5_DIMM_TYPE_MINI_RDIMM = 0x05,
	SPD_DDR5_DIMM_TYPE_MINI_UDIMM = 0x06,
	SPD_DDR5_DIMM_TYPE_72B_SO_UDIMM = 0x08,
	SPD_DDR5_DIMM_TYPE_72B_SO_RDIMM = 0x09,
	SPD_DDR5_DIMM_TYPE_SOLDERED_DOWN = 0x0b,
	SPD_DDR5_DIMM_TYPE_16B_SO_DIMM = 0x0c,
	SPD_DDR5_DIMM_TYPE_32B_SO_RDIMM = 0x0d,
	SPD_DDR5_DIMM_TYPE_1DPC = 0x0e,
	SPD_DDR5_DIMM_TYPE_2DPC = 0x0f,
};

/**
 * Converts DDR5 clock speed in MHz to the standard reported speed in MT/s
 */
uint16_t ddr5_speed_mhz_to_reported_mts(uint16_t speed_mhz);

#endif /* DEVICE_DRAM_DDR5_H */
