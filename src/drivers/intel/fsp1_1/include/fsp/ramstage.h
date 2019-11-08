/*
 * This file is part of the coreboot project.
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

#ifndef _INTEL_COMMON_RAMSTAGE_H_
#define _INTEL_COMMON_RAMSTAGE_H_

#include <fsp/util.h>
#include <stdint.h>

/*
 * Load FSP from stage cache or CBFS. This allows SoCs to load FSP separately
 * from calling silicon init. It might be required in cases where stage cache is
 * no longer available by the point SoC calls into silicon init.
 */
void fsp_load(void);
/* Perform Intel silicon init. */
void intel_silicon_init(void);
void fsp_run_silicon_init(FSP_INFO_HEADER *fsp_info_header, int is_s3_wakeup);
/* Called after the silicon init code has run. */
void soc_after_silicon_init(void);
/* Initialize UPD data before SiliconInit call. */
void soc_silicon_init_params(SILICON_INIT_UPD *params);
void mainboard_silicon_init_params(SILICON_INIT_UPD *params);
void soc_display_silicon_init_params(const SILICON_INIT_UPD *old,
	SILICON_INIT_UPD *new);
void load_logo(SILICON_INIT_UPD *params);
void load_vbt(uint8_t s3_resume, SILICON_INIT_UPD *params);

#endif /* _INTEL_COMMON_RAMSTAGE_H_ */
