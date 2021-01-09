/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _INTEL_COMMON_RAMSTAGE_H_
#define _INTEL_COMMON_RAMSTAGE_H_

#include <fsp/util.h>
#include <stdint.h>

/* Perform Intel silicon init. */
void intel_silicon_init(void);
void fsp_run_silicon_init(FSP_INFO_HEADER *fsp_info_header, int is_s3_wakeup);
const struct cbmem_entry *fsp_load_logo(UINT32 *logo_ptr, UINT32 *logo_size);
/* Called after the silicon init code has run. */
void soc_after_silicon_init(void);
/* Initialize UPD data before SiliconInit call. */
void soc_silicon_init_params(SILICON_INIT_UPD *params);
void mainboard_silicon_init_params(SILICON_INIT_UPD *params);
void soc_display_silicon_init_params(const SILICON_INIT_UPD *old,
	SILICON_INIT_UPD *new);
const struct cbmem_entry *soc_load_logo(SILICON_INIT_UPD *params);
void load_vbt(uint8_t s3_resume, SILICON_INIT_UPD *params);

#endif /* _INTEL_COMMON_RAMSTAGE_H_ */
