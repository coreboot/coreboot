/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
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

#ifndef SUPERIO_FINTEK_F71869AD_CHIP_H
#define SUPERIO_FINTEK_F71869AD_CHIP_H

#include <stdint.h>

struct superio_fintek_f71869ad_config {
	/* Member variables are defined in devicetree.cb. */
	uint8_t multi_function_register_1;
	uint8_t multi_function_register_2;
	uint8_t multi_function_register_3;
	uint8_t multi_function_register_4;
	uint8_t multi_function_register_5;
	/* HWM configuration registers */
	uint8_t hwm_smbus_address;
	uint8_t hwm_smbus_control_reg;
	uint8_t hwm_fan_type_sel_reg;
	uint8_t hwm_fan1_temp_adj_rate_reg;
	uint8_t hwm_fan_mode_sel_reg;
	uint8_t hwm_fan1_idx_rpm_mode;
	uint8_t hwm_fan1_seg1_speed_count;
	uint8_t hwm_fan1_seg2_speed_count;
	uint8_t hwm_fan1_seg3_speed_count;
	uint8_t hwm_fan1_temp_map_sel;
};

#endif /* SUPERIO_FINTEK_F71869AD_CHIP_H */
