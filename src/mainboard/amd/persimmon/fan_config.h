/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Sage Electronic Engineering, LLC
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef _SB800_FAN_CONFIG_H_
#define _SB800_FAN_CONFIG_H_

#include <southbridge/amd/cimx/sb800/fan.h>

/*****************************************************************************
 * Initial Register values
 ****************************************************************************/
#define IMC_PORT_ADDRESS		0x6E	/* 0x2E and 0x6E are common */

/*
 * Set Enabled Zones, fans, and temperature sensors
 * use 1 = enabled, 0 = disabled
 */
#define FAN0_ENABLED	1
#define FAN1_ENABLED	1
#define FAN2_ENABLED	0
#define FAN3_ENABLED	0
#define FAN4_ENABLED	0
#define IMC_FAN_ZONE0_ENABLED 1
#define IMC_FAN_ZONE1_ENABLED 1
#define IMC_FAN_ZONE2_ENABLED 0
#define IMC_FAN_ZONE3_ENABLED 0
#define IMC_TEMPIN0_ENABLED 0
#define IMC_TEMPIN1_ENABLED 0
#define IMC_TEMPIN2_ENABLED 0
#define IMC_TEMPIN3_ENABLED 0

/* Initial Fan configuration Setting the SB800 registers directly */
#define FAN0_CONTROL_REG_VALUE		FAN_POLARITY_HIGH
#define FAN0_FREQUENCY_REG_VALUE	FREQ_25KHZ
#define FAN0_LOW_DUTY_REG_VALUE		0x08
#define FAN0_REG_VALUES { FAN_INPUT_INTERNAL_DIODE, FAN0_CONTROL_REG_VALUE, \
                          FAN0_FREQUENCY_REG_VALUE, FAN0_LOW_DUTY_REG_VALUE, \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                          0x00, 0x00, 0x00 }

#define FAN1_CONTROL_REG_VALUE		FAN_POLARITY_HIGH
#define FAN1_FREQUENCY_REG_VALUE	FREQ_25KHZ
#define FAN1_LOW_DUTY_REG_VALUE		0x10
#define FAN1_REG_VALUES { FAN_INPUT_INTERNAL_DIODE, FAN1_CONTROL_REG_VALUE, \
                          FAN1_FREQUENCY_REG_VALUE, FAN1_LOW_DUTY_REG_VALUE, \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                          0x00, 0x00, 0x00 }

/* 8-bit I2c address of the CPU thermal sensor */
#define SB_TSI_ADDRESS		0x98

/*
 ********** Zone 0 **********
 */
#define ZONE0_MODE1			IMC_MODE1_FAN_ENABLED | \
                   			IMC_MODE1_FAN_IMC_CONTROLLED | \
                   			IMC_MODE1_FAN_STEP_MODE | \
                   			IMC_MODE1_FANOUT0
#define ZONE0_MODE2			IMC_MODE2_TEMPIN_SB_TSI | \
                   			IMC_MODE2_FANIN0 | \
                   			IMC_MODE2_TEMP_AVERAGING_DISABLED
#define ZONE0_TEMP_OFFSET	0x00	/* No temp offset */
#define ZONE0_HYSTERESIS	0x05	/* Degrees C Hysteresis */
#define ZONE0_SMBUS_ADDR	SB_TSI_ADDRESS	/* Temp Sensor SMBus address */
#define ZONE0_SMBUS_NUM		IMC_TEMP_SENSOR_ON_SMBUS_3	/* SMBUS number */
#define ZONE0_PWM_STEP		0x01	/* Fan PWM stepping rate */
#define ZONE0_RAMPING		0x00	/* Disable Fan PWM ramping and stepping */

/* T56N has a Maximum operating temperature  of 90°C */
/* ZONEX_THRESHOLDS - _AC0 - _AC7, _CRT - Temp Threshold in degrees C */
/* ZONEX_FANSPEEDS - Fan speeds as a percentage */
#define ZONE0_THRESHOLDS	{ 87, 82, 77, 72, 65, 1, 0, 0, 90}
#define ZONE0_FANSPEEDS		{100,  7,  5,  4,  3, 2, 0, 0}

#define ZONE0_CONFIG_VALS	{ ZONE0_MODE1, ZONE0_MODE2, ZONE0_TEMP_OFFSET, \
                         	  ZONE0_HYSTERESIS, ZONE0_SMBUS_ADDR, ZONE0_SMBUS_NUM, \
                         	  ZONE0_PWM_STEP, ZONE0_RAMPING }

/*
 ********** Zone 1 **********
 */
#define ZONE1_MODE1			IMC_MODE1_FAN_ENABLED | \
                   			IMC_MODE1_FAN_IMC_CONTROLLED | \
                   			IMC_MODE1_FAN_STEP_MODE | \
                   			IMC_MODE1_FANOUT1
#define ZONE1_MODE2			IMC_MODE2_TEMPIN_SB_TSI | \
                   			IMC_MODE2_FANIN1 | \
                   			IMC_MODE2_TEMP_AVERAGING_DISABLED
#define ZONE1_TEMP_OFFSET	0x00	/* No temp offset */
#define ZONE1_HYSTERESIS	0x05	/* Degrees C Hysteresis */
#define ZONE1_SMBUS_ADDR	SB_TSI_ADDRESS	/* Temp Sensor SMBus address */
#define ZONE1_SMBUS_NUM		IMC_TEMP_SENSOR_ON_SMBUS_3	/* SMBUS number*/
#define ZONE1_PWM_STEP		0x01	/* Fan PWM stepping rate  */
#define ZONE1_RAMPING		0x00	/* Disable Fan PWM ramping and stepping */

/* ZONEX_THRESHOLDS - _AC0 - _AC7, _CRT - Temp Threshold in degrees C */
/* ZONEX_FANSPEEDS - Fan speeds as a percentage */
#define ZONE1_THRESHOLDS	{ 85, 80, 75, 65, 1, 0, 0, 0, 90}
#define ZONE1_FANSPEEDS		{100, 10,  6,  4, 3, 0, 0, 0}

#define ZONE1_CONFIG_VALS	{ ZONE1_MODE1, ZONE1_MODE2, ZONE1_TEMP_OFFSET, \
                         	  ZONE1_HYSTERESIS, ZONE1_SMBUS_ADDR, ZONE1_SMBUS_NUM, \
                         	  ZONE1_PWM_STEP, ZONE1_RAMPING }

#endif /* _SB800_FAN_CONFIG_H_ */
