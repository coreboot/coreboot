/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 * Copyright (C) 2015 BAP - Bruhnspace Advanced Projects
 * (Written by Fabian Kunkel <fabi@adv.bruhnspace.com> for BAP)
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef SUPERIO_FINTEK_F81866D_CHIP_H
#define SUPERIO_FINTEK_F81866D_CHIP_H

#include <stdint.h>

struct superio_fintek_f81866d_config {

	/* AMD TSI */
	uint8_t hwm_amd_tsi_addr;
	uint8_t hwm_amd_tsi_control;

	/* Fan control */
	uint8_t hwm_fan_select;
	uint8_t hwm_fan_mode;
	uint8_t hwm_fan3_control;
	uint8_t hwm_fan2_temp_map_select;

	uint8_t hwm_fan2_bound1;
	uint8_t hwm_fan2_bound2;
	uint8_t hwm_fan2_bound3;
	uint8_t hwm_fan2_bound4;
	uint8_t hwm_fan2_seg1_speed;
	uint8_t hwm_fan2_seg2_speed;
	uint8_t hwm_fan2_seg3_speed;
	uint8_t hwm_fan2_seg4_speed;
	uint8_t hwm_fan2_seg5_speed;

	/* Temp sensor type */
	uint8_t hwm_temp_sens_type;
};

#endif /* SUPERIO_FINTEK_F81866D_CHIP_H */
