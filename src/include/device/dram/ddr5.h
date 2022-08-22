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

/**
 * Converts DDR5 clock speed in MHz to the standard reported speed in MT/s
 */
uint16_t ddr5_speed_mhz_to_reported_mts(uint16_t speed_mhz);

#endif /* DEVICE_DRAM_DDR5_H */
