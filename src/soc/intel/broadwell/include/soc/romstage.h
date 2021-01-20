/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_ROMSTAGE_H_
#define _BROADWELL_ROMSTAGE_H_

#include <soc/pei_data.h>

void mainboard_fill_spd_data(struct pei_data *pei_data);
void mainboard_post_raminit(const int s3resume);

void sdram_initialize(struct pei_data *pei_data);
void save_mrc_data(struct pei_data *pei_data);
void setup_sdram_meminfo(struct pei_data *pei_data);

struct chipset_power_state;
struct chipset_power_state *fill_power_state(void);
void report_platform_info(void);

void systemagent_early_init(void);
void pch_early_init(void);
void pch_uart_init(void);
void intel_early_me_status(void);

#endif
