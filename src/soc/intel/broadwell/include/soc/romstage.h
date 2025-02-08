/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_ROMSTAGE_H_
#define _BROADWELL_ROMSTAGE_H_

void mainboard_post_raminit(const bool s3resume);

struct chipset_power_state;
struct chipset_power_state *fill_power_state(void);
void report_platform_info(void);

void perform_raminit(const struct chipset_power_state *const power_state);

void systemagent_early_init(void);
void pch_early_init(void);

#endif
