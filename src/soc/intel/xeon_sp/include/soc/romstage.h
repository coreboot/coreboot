/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_ROMSTAGE_H_
#define _SOC_ROMSTAGE_H_

#include <fsp/api.h>
#include <soc/soc_util.h>

/* These functions are weak and can be overridden by a mainboard functions. */
void mainboard_memory_init_params(FSPM_UPD * mupd);
void mainboard_rtc_failed(void);
uint32_t get_max_capacity_mib(void);
uint8_t get_error_correction_type(const uint8_t RasModesEnabled);
uint8_t get_max_dimm_count(void);
uint8_t get_dram_type(const struct SystemMemoryMapHob *hob);
void save_dimm_info(void);
void mainboard_ewl_check(void);
void mainboard_fsp_error_handle(void);

#endif /* _SOC_ROMSTAGE_H_ */
