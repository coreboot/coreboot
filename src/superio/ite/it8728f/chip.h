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
 */

#ifndef SUPERIO_ITE_IT8728F_CHIP_H
#define SUPERIO_ITE_IT8728F_CHIP_H

struct superio_ite_it8728f_config {
	/* HWM configuration registers */
	uint8_t hwm_ctl_register;
	uint8_t hwm_main_ctl_register;
	uint8_t hwm_adc_temp_chan_en_reg;
	uint8_t hwm_fan1_ctl_pwm;
	uint8_t hwm_fan2_ctl_pwm;
	uint8_t hwm_fan3_ctl_pwm;
};

#endif /* SUPERIO_ITE_IT8728F_CHIP_H */
