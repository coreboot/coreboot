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

enum ddr5_module_type {
	DDR5_SPD_RDIMM = 0x01,
	DDR5_SPD_UDIMM = 0x02,
	DDR5_SPD_SODIMM = 0x03,
	DDR5_SPD_LRDIMM = 0x04,
	DDR5_SPD_MINI_RDIMM = 0x05,
	DDR5_SPD_MINI_UDIMM = 0x06,
	DDR5_SPD_72B_SO_UDIMM = 0x08,
	DDR5_SPD_72B_SO_RDIMM = 0x09,
	DDR5_SPD_SOLDERED_DOWN = 0x0b,
	DDR5_SPD_16B_SO_DIMM = 0x0c,
	DDR5_SPD_32B_SO_RDIMM = 0x0d,
	DDR5_SPD_1DPC = 0x0e,
	DDR5_SPD_2DPC = 0x0f,
};

/**
 * Converts DDR5 clock speed in MHz to the standard reported speed in MT/s
 */
uint16_t ddr5_speed_mhz_to_reported_mts(uint16_t speed_mhz);

#endif /* DEVICE_DRAM_DDR5_H */
