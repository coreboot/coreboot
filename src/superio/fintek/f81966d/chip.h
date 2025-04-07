/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_FINTEK_F81966D_CHIP_H
#define SUPERIO_FINTEK_F81966D_CHIP_H

#include <stdint.h>

struct superio_fintek_f81966d_config {
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

	/* gpio control, range bits [0:7] map to GPIOx0:GPIOx7 */
	uint8_t use_gpio0;
	uint8_t gpio0_enable;
	uint8_t gpio0_data;
	uint8_t gpio0_drive;

	uint8_t use_gpio1;
	uint8_t gpio1_enable;
	uint8_t gpio1_data;
	uint8_t gpio1_drive;

	uint8_t use_gpio2;
	uint8_t gpio2_enable;
	uint8_t gpio2_data;

	uint8_t use_gpio3;
	uint8_t gpio3_enable;
	uint8_t gpio3_data;
	uint8_t gpio3_drive;

	uint8_t use_gpio4;
	uint8_t gpio4_enable;
	uint8_t gpio4_data;
	uint8_t gpio4_drive;

	uint8_t use_gpio5;
	uint8_t gpio5_enable;
	uint8_t gpio5_data;
	uint8_t gpio5_drive;

	uint8_t use_gpio6;
	uint8_t gpio6_enable;
	uint8_t gpio6_data;
	uint8_t gpio6_drive;

	uint8_t use_gpio7;
	uint8_t gpio7_enable;
	uint8_t gpio7_data;
	uint8_t gpio7_drive;

	uint8_t use_gpio8;
	uint8_t gpio8_enable;
	uint8_t gpio8_data;
	uint8_t gpio8_drive;

	uint8_t use_gpio9;
	uint8_t gpio9_enable;
	uint8_t gpio9_data;
	uint8_t gpio9_drive;
};

#endif /* SUPERIO_FINTEK_F81966D_CHIP_H */
