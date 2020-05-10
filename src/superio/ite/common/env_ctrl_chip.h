/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_ITE_ENV_CTRL_CHIP_H
#define SUPERIO_ITE_ENV_CTRL_CHIP_H

#define ITE_EC_TMPIN_CNT	3

#if CONFIG(SUPERIO_ITE_ENV_CTRL_5FANS)
#define ITE_EC_FAN_CNT		5
#else
#define ITE_EC_FAN_CNT		3
#endif

/* Supported thermal mode on TMPINx */
enum ite_ec_thermal_mode {
	THERMAL_MODE_DISABLED = 0,
	THERMAL_DIODE,
	THERMAL_RESISTOR,
	THERMAL_PECI,
};

struct ite_ec_thermal_config {
	enum ite_ec_thermal_mode mode;
	/* Offset is used for diode sensors and PECI */
	u8 offset;
	/* Limits */
	u8 min;
	u8 max;
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
	 * Enable reading of voltage pins VINx.
	 */
	enum ite_ec_voltage_pin vin_mask;

	/*
	 * Enable temperature sensors in given mode.
	 */
	struct ite_ec_thermal_config tmpin[ITE_EC_TMPIN_CNT];

	/*
	 * Enable a FAN in given mode.
	 */
	struct ite_ec_fan_config fan[ITE_EC_FAN_CNT];

	bool tmpin_beep;
	bool fan_beep;
	bool vin_beep;

	/*
	 * Enable SMBus for external thermal sensor.
	 */
	bool smbus_en;
};

/* Some shorthands for device trees */
#define TMPIN1	ec.tmpin[0]
#define TMPIN2	ec.tmpin[1]
#define TMPIN3	ec.tmpin[2]

#define FAN1	ec.fan[0]
#define FAN2	ec.fan[1]
#define FAN3	ec.fan[2]
#define FAN4	ec.fan[3]
#define FAN5	ec.fan[4]

#endif /* SUPERIO_ITE_ENV_CTRL_CHIP_H */
