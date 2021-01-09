/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _INTEL_COMMON_RAMSTAGE_H_
#define _INTEL_COMMON_RAMSTAGE_H_

#include <fsp/util.h>
#include <stdint.h>

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
void load_vbt(SILICON_INIT_UPD *params);

#endif /* _INTEL_COMMON_RAMSTAGE_H_ */
