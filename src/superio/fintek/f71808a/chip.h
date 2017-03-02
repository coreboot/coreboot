/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 * Copyright (C) 2017 Nicola Corna <nicola@corna.info>
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
 */

#ifndef SUPERIO_FINTEK_F71808A_CHIP_H
#define SUPERIO_FINTEK_F71808A_CHIP_H

#include <stdint.h>

struct superio_fintek_f71808a_config {

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
