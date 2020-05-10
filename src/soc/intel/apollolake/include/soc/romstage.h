/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_APOLLOLAKE_ROMSTAGE_H_
#define _SOC_APOLLOLAKE_ROMSTAGE_H_

#include <fsp/api.h>

void set_max_freq(void);
void mainboard_memory_init_params(FSPM_UPD *mupd);
void mainboard_save_dimm_info(void);
void report_platform_info(void);

#endif /* _SOC_APOLLOLAKE_ROMSTAGE_H_ */
