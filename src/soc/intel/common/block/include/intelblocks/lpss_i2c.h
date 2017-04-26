/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SOC_INTEL_COMMON_BLOCK_LPSS_I2C_H
#define SOC_INTEL_COMMON_BLOCK_LPSS_I2C_H

#include <device/i2c.h>
#include <stdint.h>

/*
 * Timing values are in units of clock period, with the clock speed
 * provided by the SOC in CONFIG_SOC_INTEL_COMMON_LPSS_CLOCK_MHZ.
 * Automatic configuration is done based on requested speed, but the
 * values may need tuned depending on the board and the number of
 * devices present on the bus.
 */
struct lpss_i2c_speed_config {
	enum i2c_speed speed;
	/* SCL high and low period count */
	uint16_t scl_lcnt;
	uint16_t scl_hcnt;
	/*
	 * SDA hold time should be 300ns in standard and fast modes
	 * and long enough for deterministic logic level change in
	 * fast-plus and high speed modes.
	 *
	 *  [15:0] SDA TX Hold Time
	 * [23:16] SDA RX Hold Time
	 */
	uint32_t sda_hold;
};

/*
 * This I2C controller has support for 3 independent speed configs but can
 * support both FAST_PLUS and HIGH speeds through the same set of speed
 * config registers.  These are treated separately so the speed config values
 * can be provided via ACPI to the OS.
 */
#define LPSS_I2C_SPEED_CONFIG_COUNT	4

struct lpss_i2c_bus_config {
	/* Bus should be enabled prior to ramstage with temporary base */
	int early_init;
	/* Bus speed in Hz, default is I2C_SPEED_FAST (400 KHz) */
	enum i2c_speed speed;
	/* If rise_time_ns is non-zero the calculations for lcnt and hcnt
	 * registers take into account the times of the bus. However, if
	 * there is a match in speed_config those register values take
	 * precedence. */
	int rise_time_ns;
	int fall_time_ns;
	int data_hold_time_ns;
	/* Specific bus speed configuration */
	struct lpss_i2c_speed_config speed_config[LPSS_I2C_SPEED_CONFIG_COUNT];
};

#define LPSS_I2C_SPEED_CONFIG(speedval, lcnt, hcnt, hold)	\
	{							\
		.speed = I2C_SPEED_ ## speedval,		\
		.scl_lcnt = (lcnt),				\
		.scl_hcnt = (hcnt),				\
		.sda_hold = (hold),				\
	}

/* Functions to be implemented by SoC code */

/* Get base address for early init of I2C controllers. */
uintptr_t i2c_get_soc_early_base(unsigned int bus);

/*
 * Map given I2C bus number to devfn.
 * Return value:
 * -1 = error
 * otherwise, devfn(>=0) corresponding to I2C bus number.
 */
int i2c_soc_devfn_to_bus(unsigned int devfn);

/*
 * Map given bus number to a I2C Controller.
 * Return value:
 * -1 = error
 * otherwise, devfn(>=0) corresponding to I2C bus number.
 */
int i2c_soc_bus_to_devfn(unsigned int bus);

/*
 * SoC implemented callback for getting I2C bus configuration.
 *
 * Returns NULL if i2c config is not found
 */
const struct lpss_i2c_bus_config *i2c_get_soc_cfg(unsigned int bus,
						const struct device *dev);
#endif /* SOC_INTEL_COMMON_BLOCK_LPSS_I2C_H */
