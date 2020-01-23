/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

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
	uint8_t hwm_temp_sensor_type;
};

#endif /* SUPERIO_FINTEK_F71869AD_CHIP_H */
