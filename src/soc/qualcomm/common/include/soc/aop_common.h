/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_COMMON_AOP_H__
#define _SOC_QUALCOMM_COMMON_AOP_H__

#include <security/vboot/vboot_common.h>

#define AOP_CBFS_NAME(base) \
	(CONFIG(SOC_QUALCOMM_SPLIT_AOP_CBFS) ? \
		(CONFIG(VBOOT) && vboot_recovery_mode_enabled() ? \
			CONFIG_CBFS_PREFIX "/" base "_ro" : \
			CONFIG_CBFS_PREFIX "/" base "_rw") : \
		CONFIG_CBFS_PREFIX "/" base)

void aop_fw_load_reset(void);

#endif  // _SOC_QUALCOMM_COMMON_AOP_H__
