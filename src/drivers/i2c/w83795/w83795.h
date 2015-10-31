/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Raptor Engineering
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

#ifndef _W83795_H_
#define _W83795_H_

#define W83795_REG_I2C_ADDR		0xFC
#define W83795_REG_BANKSEL		0x00
#define W83795_REG_CONFIG		0x01
#define W83795_REG_CONFIG_START		0x01
#define W83795_REG_CONFIG_CONFIG48	0x04
#define W83795_REG_CONFIG_INIT		0x80

#define W83795_REG_VOLT_CTRL1		0x02
#define W83795_REG_VOLT_CTRL2		0x03
#define W83795_REG_TEMP_CTRL1		0x04 /* Temperature Monitoring Control Register */
#define W83795_REG_TEMP_CTRL2		0x05 /* Temperature Monitoring Control Register */
#define W83795_REG_FANIN_CTRL1		0x06
#define W83795_REG_FANIN_CTRL2		0x07
#define W83795_REG_TEMP_CTRL1_EN_DTS	0x20 /* Enable DTS (Digital Temperature Sensor) interface from INTEL PECI or AMD SB-TSI. */
#define DTS_SRC_INTEL_PECI		(0 << 0)
#define DTS_SRC_AMD_SBTSI		(1 << 0)

#define W83795_REG_TTTI(n)		(0x260 + (n)) /* Target temperature W83795G/ADG will try to tune the fan output to keep */
#define W83795_REG_CTFS(n)		(0x268 + (n)) /* Critical Temperature to Full Speed all fan */
#define W83795_REG_DTSC			0x301 /* Digital Temperature Sensor Configuration */

#define W83795_REG_DTSE			0x302 /* Digital Temperature Sensor Enable */
#define W83795_REG_DTS(n)		(0x26 + (n))
#define W83795_REG_VRLSB		0x3C

#define W83795_REG_TEMP_TR1		0x21
#define W83795_REG_TEMP_TR2		0x22
#define W83795_REG_TEMP_TR3		0x23
#define W83795_REG_TEMP_TR4		0x24
#define W83795_REG_TEMP_TR5		0x1F
#define W83795_REG_TEMP_TR6		0x20

#define W83795_REG_VOLT_LIM_HIGH(n)	(0x70 + (n * 2))	/* Voltage high limit (0 == VSEN1) */
#define W83795_REG_VOLT_LIM_LOW(n)	(0x71 + (n * 2))	/* Voltage low limit (0 == VSEN1) */
#define W83795_REG_VOLT_LIM_HIGH_2_M(n)	(0x96 + (n * 4))	/* Voltage high limit MSB (0 == VDSEN14) */
#define W83795_REG_VOLT_LIM_LOW_2_M(n)	(0x97 + (n * 4))	/* Voltage low limit MSB (0 == VDSEN14)  */
#define W83795_REG_VOLT_LIM_HIGH_2_L(n)	(0x98 + (n * 4))	/* Voltage high limit LSB (0 == VDSEN14) */
#define W83795_REG_VOLT_LIM_LOW_2_L(n)	(0x99 + (n * 4))	/* Voltage low limit LSB (0 == VDSEN14)  */

#define W83795_REG_TEMP_CRIT(n)		(0x96 + (n * 4))	/* Temperature critical limit */
#define W83795_REG_TEMP_CRIT_HYSTER(n)	(0x97 + (n * 4))	/* Temperature critical limit hysteresis */
#define W83795_REG_TEMP_WARN(n)		(0x98 + (n * 4))	/* Temperature warning limit */
#define W83795_REG_TEMP_WARN_HYSTER(n)	(0x99 + (n * 4))	/* Temperature warning limit hysteresis */

#define W83795_REG_DTS_CRIT		0xB2			/* Temperature critical limit */
#define W83795_REG_DTS_CRIT_HYSTER	0xB3			/* Temperature critical limit hysteresis */
#define W83795_REG_DTS_WARN		0xB4			/* Temperature warning limit */
#define W83795_REG_DTS_WARN_HYSTER	0xB5			/* Temperature warning limit hysteresis */

#define W83795_REG_FCMS1		0x201
#define W83795_REG_FCMS2		0x208
#define W83795_REG_TFMR(n)		(0x202 + (n))		/* Temperature to fan mapping */
#define W83795_REG_T12TSS		0x209			/* Temperature Source Selection Register 1 */
#define W83795_REG_T34TSS		0x20A			/* Temperature Source Selection Register 2 */
#define W83795_REG_T56TSS		0x20B			/* Temperature Source Selection Register 3 */
#define W83795_REG_FAN_MANUAL_SPEED(n)	(0x210 + n)
#define W83795_REG_DFSP			0x20C

#define W83795_REG_FAN_NONSTOP(n)	(0x228 + (n))	/* Fan Nonstop Value */

#define W83795_REG_FTSH(n)		(0x240 + (n) * 2)
#define W83795_REG_FTSL(n)		(0x241 + (n) * 2)
#define W83795_REG_TFTS			0x250

typedef enum w83795_fan_mode {
	SPEED_CRUISE_MODE = 0,		///< Fan Speed Cruise mode keeps the fan speed in a specified range
	THERMAL_CRUISE_MODE = 1,	///< Thermal Cruise mode is an algorithm to control the fan speed to keep the temperature source around the TTTI
	SMART_FAN_MODE = 2,		///< Smart Fan mode offers 6 slopes to control the fan speed
	MANUAL_MODE = 3,		///< control manually
} w83795_fan_mode_t;

#endif
