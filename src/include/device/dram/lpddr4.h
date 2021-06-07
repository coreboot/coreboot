/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DEVICE_DRAM_LPDDR4_H
#define DEVICE_DRAM_LPDDR4_H

/**
 * @file lpddr4.h
 *
 * \brief Utilities for decoding LPDDR4 info
 */

#include <device/dram/common.h>
#include <types.h>

/**
 * Converts LPDDR4 clock speed in MHz to the standard reported speed in MT/s
 */
uint16_t lpddr4_speed_mhz_to_reported_mts(uint16_t speed_mhz);

#endif /* DEVICE_DRAM_LPDDR4_H */
