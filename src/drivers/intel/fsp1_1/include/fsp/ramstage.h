/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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
#include <soc/intel/common/util.h>
#include <stdint.h>

/* Perform Intel silicon init. */
void intel_silicon_init(void);
/* Called after the silicon init code has run. */
void soc_after_silicon_init(void);
/* Initialize UPD data before SiliconInit call. */
void soc_silicon_init_params(SILICON_INIT_UPD *params);
void mainboard_silicon_init_params(SILICON_INIT_UPD *params);
void soc_display_silicon_init_params(const SILICON_INIT_UPD *old,
	SILICON_INIT_UPD *new);
void load_vbt(uint8_t s3_resume, SILICON_INIT_UPD *params);

#endif /* _INTEL_COMMON_RAMSTAGE_H_ */
