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

#ifndef _BROADWELL_ROMSTAGE_H_
#define _BROADWELL_ROMSTAGE_H_

#include <stdint.h>
#include <soc/pei_data.h>

struct chipset_power_state;
struct romstage_params {
	struct chipset_power_state *power_state;
	struct pei_data pei_data;
};

void mainboard_pre_raminit(struct romstage_params *params);
void mainboard_post_raminit(struct romstage_params *params);

void raminit(struct pei_data *pei_data);

struct chipset_power_state;
struct chipset_power_state *fill_power_state(void);
void report_platform_info(void);
void report_memory_config(void);

void set_max_freq(void);

void systemagent_early_init(void);
void pch_early_init(void);
void pch_uart_init(void);
void intel_early_me_status(void);

#endif
