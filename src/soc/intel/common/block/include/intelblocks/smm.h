/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 201 Intel Corp.
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

#ifndef SOC_INTEL_COMMON_BLOCK_SMM_H
#define SOC_INTEL_COMMON_BLOCK_SMM_H

/*
 * This common code block relies on each specific SOC defining the macro
 * ENABLE_SMI_PARAMS for the values needed for SMI enabling on the
 * specific SOC
 */

/*
 * The initialization of the southbridge is split into 2 compoments. One is
 * for clearing the state in the SMM registers. The other is for enabling
 * SMIs.
 */
void smm_southbridge_clear_state(void);
void smm_southbridge_enable(void);

#endif
