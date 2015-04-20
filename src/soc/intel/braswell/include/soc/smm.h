/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef _SOC_SMM_H_
#define _SOC_SMM_H_

#include <rules.h>

#if ENV_RAMSTAGE
#include <stdint.h>
void southcluster_smm_clear_state(void);
void southcluster_smm_enable_smi(void);
void southcluster_smm_save_param(int param, uint32_t data);
#endif

enum {
	SMM_SAVE_PARAM_GPIO_ROUTE = 0,
	SMM_SAVE_PARAM_PCIE_WAKE_ENABLE,
	SMM_SAVE_PARAM_COUNT
};

#endif /* _SOC_SMM_H_ */
