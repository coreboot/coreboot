/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _W83795_H_
#define _W83795_H_

#define W83795_DEV			0x2F /* Host I2c Addr (strap to addr1 addr0 1 1, 0x5E) */

#define W83795_REG_I2C_ADDR		0xFC
#define W83795_REG_BANKSEL		0x00
#define W83795_REG_CONFIG		0x01
#define W83795_REG_CONFIG_START		0x01
#define W83795_REG_CONFIG_CONFIG48	0x04
#define W83795_REG_CONFIG_INIT		0x80

#define W83795_REG_TEMP_CTRL1		0x04 /* Temperature Monitoring Control Register */
#define W83795_REG_TEMP_CTRL2		0x05 /* Temperature Monitoring Control Register */
#define W83795_REG_FANIN_CTRL1		0x06
#define W83795_REG_FANIN_CTRL2		0x07
#define W83795_REG_TEMP_CTRL1_EN_DTS	0x20 /* Enable DTS (Digital Temperature Sensor) interface from INTEL PECI or AMD SB-TSI. */
#define DTS_SRC_INTEL_PECI		(0 << 0)
#define DTS_SRC_AMD_SBTSI		(1 << 0)

#define W83795_REG_TSS(n)		(0x209 + (n)) /* Temperature Source Selection Register */
#define W83795_REG_TTTI(n)		(0x260 + (n)) /* Target temperature W83795G/ADG will try to tune the fan output to keep */
#define W83795_REG_CTFS(n)		(0x268 + (n)) /* Critical Temperature to Full Speed all fan */
#define W83795_REG_HT(n)		(0x270 + (n)) /* Hysteresis of Temperature */
#define W83795_REG_DTSC			0x301 /* Digital Temperature Sensor Configuration */

#define W83795_REG_DTSE			0x302 /* Digital Temperature Sensor Enable */
#define W83795_REG_DTS(n)		(0x26 + (n))
#define W83795_REG_VRLSB		0x3C

#define W83795_TEMP_REG_TR1		0x21
#define W83795_TEMP_REG_TR2		0x22
#define W83795_TEMP_REG_TR3		0x23
#define W83795_TEMP_REG_TR4		0x24
#define W83795_TEMP_REG_TR5		0x1F
#define W83795_TEMP_REG_TR6		0x20

#define W83795_REG_FCMS1		0x201
#define W83795_REG_FCMS2		0x208
#define W83795_REG_TFMR(n)		(0x202 + (n)) /*temperature to fam mappig*/
#define W83795_REG_DFSP			0x20C

#define W83795_REG_FTSH(n)		(0x240 + (n) * 2)
#define W83795_REG_FTSL(n)		(0x241 + (n) * 2)
#define W83795_REG_TFTS			0x250

typedef enum w83795_fan_mode {
	SPEED_CRUISE_MODE,	///< Fan Speed Cruise mode keeps the fan speed in a specified range
	THERMAL_CRUISE_MODE,	///< Thermal Cruise mode is an algorithm to control the fan speed to keep the temperature source around the TTTI
	SMART_FAN_MODE,		///< Smart Fan mode offers 6 slopes to control the fan speed
	MANUAL_MODE,		///< control manually
} w83795_fan_mode_t;

#endif
