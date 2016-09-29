/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2016 secunet Security Networks AG
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
 */

#ifndef SUPERIO_ITE_ENV_CTRL_CHIP_H
#define SUPERIO_ITE_ENV_CTRL_CHIP_H

#define ITE_EC_TMPIN_CNT	3
#define ITE_EC_FAN_CNT		3

/* Supported thermal mode on TMPINx */
enum ite_ec_thermal_mode {
	THERMAL_MODE_DISABLED = 0,
	THERMAL_DIODE,
	THERMAL_RESISTOR,
};

/* Bit mask for voltage pins VINx */
enum ite_ec_voltage_pin {
	VIN0 = 0x01,
	VIN1 = 0x02,
	VIN2 = 0x04,
	VIN3 = 0x08,
	VIN4 = 0x10,
	VIN5 = 0x20,
	VIN6 = 0x40,
	VIN7 = 0x80,
	VIN_ALL = 0xff
};

enum ite_ec_fan_mode {
	FAN_IGNORE = 0,
	FAN_MODE_ON,
	FAN_MODE_OFF,
	FAN_SMART_SOFTWARE,
	FAN_SMART_AUTOMATIC,
};

struct ite_ec_fan_smartconfig {
	u8 tmpin;	/* select TMPINx (1, 2 or 3)		*/
	u8 tmp_off;	/* turn fan off below (°C)		*/
	u8 tmp_start;	/* turn fan on above (°C)		*/
	u8 tmp_full;	/* 100% duty cycle above (°C)		*/
	u8 tmp_delta;	/* adapt fan speed when temperature
			   changed by at least `tmp_delta`°C	*/
	u8 smoothing;	/* enable smoothing			*/
	u8 pwm_start;	/* start at this duty cycle (%)		*/
	u8 slope;	/* increase duty cycle by `slope`%/°C	*/
};

struct ite_ec_fan_config {
	enum ite_ec_fan_mode mode;
	struct ite_ec_fan_smartconfig smart;
};

struct ite_ec_config {
	/*
	 * Enable external temperature sensor to use PECI GetTemp()
	 * command and store in register TMPIN 1, 2, or 3.
	 */
	u8 peci_tmpin;

	/*
	 * Enable thermal mode on TMPINx.
	 */
	enum ite_ec_thermal_mode tmpin_mode[ITE_EC_TMPIN_CNT];

	/*
	 * Enable reading of voltage pins VINx.
	 */
	enum ite_ec_voltage_pin vin_mask;

	/*
	 * Enable a FAN in given mode.
	 */
	struct ite_ec_fan_config fan[ITE_EC_FAN_CNT];
};

/* Some shorthands for device trees */
#define TMPIN1	ec.tmpin_mode[0]
#define TMPIN2	ec.tmpin_mode[1]
#define TMPIN3	ec.tmpin_mode[2]
#define FAN1	ec.fan[0]
#define FAN2	ec.fan[1]
#define FAN3	ec.fan[2]

#endif /* SUPERIO_ITE_ENV_CTRL_CHIP_H */
