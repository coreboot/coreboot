/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SMM_H_
#define _SOC_SMM_H_

#include <types.h>

enum {
	SMM_SAVE_PARAM_GPIO_ROUTE = 0,
	SMM_SAVE_PARAM_PCIE_WAKE_ENABLE,
	SMM_SAVE_PARAM_COUNT
};

void smm_southcluster_save_param(int param, uint32_t data);

#endif /* _SOC_SMM_H_ */
