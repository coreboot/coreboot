/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_FINTEK_F71808A_CHIP_H
#define SUPERIO_FINTEK_F71808A_CHIP_H

#include <stdint.h>

struct superio_fintek_f71808a_config {

	uint8_t hwm_vt1_boundary_1_temperature;
	uint8_t hwm_vt1_boundary_2_temperature;
	uint8_t hwm_vt1_boundary_3_temperature;
	uint8_t hwm_vt1_boundary_4_temperature;
	uint8_t hwm_fan1_boundary_hysteresis;
	uint8_t hwm_domain1_en;

	/* Multi function registers */
	uint8_t multi_function_register_0;
	uint8_t multi_function_register_1;
	uint8_t multi_function_register_2;
	uint8_t multi_function_register_3;
	uint8_t multi_function_register_4;

	/* Intel Ibex Peak/PECI/AMD TSI */
	uint8_t hwm_peci_tsi_ctrl;
	uint8_t hwm_tcc_temp;

	/* Fan 1 control */
	uint8_t hwm_fan1_seg1_speed;
	uint8_t hwm_fan1_seg2_speed;
	uint8_t hwm_fan1_seg3_speed;
	uint8_t hwm_fan1_seg4_speed;
	uint8_t hwm_fan1_seg5_speed;
	uint8_t hwm_fan1_temp_src;

	/* Fan 2 control */
	uint8_t hwm_fan2_seg1_speed;
	uint8_t hwm_fan2_seg2_speed;
	uint8_t hwm_fan2_seg3_speed;
	uint8_t hwm_fan2_seg4_speed;
	uint8_t hwm_fan2_seg5_speed;
	uint8_t hwm_fan2_temp_src;
};

#endif /* SUPERIO_FINTEK_F71808A_CHIP_H */
