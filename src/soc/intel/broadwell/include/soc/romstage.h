/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_ROMSTAGE_H_
#define _BROADWELL_ROMSTAGE_H_

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

void systemagent_early_init(void);
void pch_early_init(void);
void pch_uart_init(void);
void intel_early_me_status(void);

#endif
