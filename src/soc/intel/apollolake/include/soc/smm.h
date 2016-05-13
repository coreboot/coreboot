/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_SMM_H_
#define _SOC_SMM_H_

#include <stdint.h>

/* These helpers are for performing SMM relocation. */
void southbridge_clear_smi_status(void);

/*
 * The initialization of the southbridge is split into 2 compoments. One is
 * for clearing the state in the SMM registers. The other is for enabling
 * SMIs.
 */
void southbridge_smm_clear_state(void);
void southbridge_smm_enable_smi(void);


/* Fills in the arguments for the entire SMM region covered by chipset
 * protections. e.g. TSEG. */
void smm_region(void **start, size_t *size);
#endif
