/*
 * This file is part of the coreboot project.
 *
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

#ifndef _SOC_CAR_H_
#define _SOC_CAR_H_

#include <fsp/util.h>

/* Mainboard and SoC initialization prior to console. */
void car_mainboard_pre_console_init(void);
void car_soc_pre_console_init(void);

/* Mainboard and SoC initialization post console initialization. */
void car_mainboard_post_console_init(void);
void car_soc_post_console_init(void);

#endif /* _SOC_CAR_H_ */
