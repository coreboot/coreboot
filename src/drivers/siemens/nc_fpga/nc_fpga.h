/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Siemens AG.
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

#ifndef _SIEMENS_NC_FPGA_H_
#define _SIEMENS_NC_FPGA_H_

#include <compiler.h>

#define NC_MAGIC_OFFSET			0x020
#define  NC_FPGA_MAGIC			0x4E433746
#define NC_CAP1_OFFSET			0x080
#define  NC_CAP1_DSAVE_NMI_DELAY	0x008
#define  NC_CAP1_BL_BRIGHTNESS_CTRL	0x010
#define  NC_CAP1_FAN_CTRL		0x080
#define  NC_CAP1_TEMP_MON		0x100
#define NC_DSAVE_OFFSET			0x58
#define NC_BL_BRIGHTNESS_OFFSET		0x88
#define NC_BL_PWM_OFFSET		0x8C
#define NC_FANMON_CTRL_OFFSET		0x400

#define MAX_NUM_SENSORS			8

typedef struct {
	uint16_t	rmin;
	uint16_t	rmax;
	uint16_t	nmin;
	uint16_t	nmax;
} temp_cc_t;

typedef struct {
	uint16_t	res0;
	uint8_t		sensornum;
	uint8_t		res1;
	uint32_t	sensordelay;
	uint32_t	res2[4];
	temp_cc_t	sensorcfg[8];
	uint32_t	res3[4];
	uint8_t		sensorselect;
	uint8_t		res4[3];
	uint16_t	t_warn;
	uint16_t	t_crit;
	uint16_t	res5;
	uint8_t		res6[2];
	uint32_t	samplingtime;
	uint16_t	setpoint;
	uint8_t		hystctrl;
	uint8_t		res7;
	uint16_t	kp;
	uint16_t	ki;
	uint16_t	kd;
	uint16_t	res8[2];
	uint16_t	fanmax;
	uint16_t	hystval;
	uint16_t	hystthreshold;
	uint16_t	res9[4];
	uint32_t	fanmon;
} __packed fan_ctrl_t;

#endif /* _SIEMENS_NC_FPGA_H_ */
