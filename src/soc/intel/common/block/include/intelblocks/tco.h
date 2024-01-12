/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_TCO_H
#define SOC_INTEL_COMMON_BLOCK_TCO_H

#include <stdint.h>

/* Get base address of TCO I/O registers. */
uint16_t tco_get_bar(void);
/*
 * Enable TCO BAR using SMBUS TCO base to access TCO related register
 * also disable the timer.
 */
void tco_configure(void);
/* Lockdown TCO registers before booting to OS */
void tco_lockdown(void);
/*
 * Resets the tco status registers. This function clears the tco_sts register
 * and returns the status bits set.
 */
uint32_t tco_reset_status(void);
uint16_t tco_read_reg(uint16_t tco_reg);
void tco_write_reg(uint16_t tco_reg, uint16_t value);

/* Get TCO timer period in milliseconds */
uint32_t tco_get_timer_period(void);
/* Get the minimum time value for the TCO timer */
uint32_t tco_get_timer_min_value(void);
/* Get the maximum time value for the TCO timer */
uint32_t tco_get_timer_max_value(void);

#endif /* SOC_INTEL_COMMON_BLOCK_TCO_H */
