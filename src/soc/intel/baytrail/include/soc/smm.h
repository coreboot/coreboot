/*
 * This file is part of the coreboot project.
 *
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

#ifndef _BAYTRAIL_SMM_H_
#define _BAYTRAIL_SMM_H_

#include <types.h>

enum {
	SMM_SAVE_PARAM_GPIO_ROUTE = 0,
	SMM_SAVE_PARAM_PCIE_WAKE_ENABLE,
	SMM_SAVE_PARAM_COUNT
};

void smm_southcluster_save_param(int param, uint32_t data);

#endif /* _BAYTRAIL_SMM_H_ */
