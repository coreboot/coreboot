/* SPDX-License-Identifier: GPL-2.0-only */

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
