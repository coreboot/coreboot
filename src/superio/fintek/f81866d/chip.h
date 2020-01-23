/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

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
