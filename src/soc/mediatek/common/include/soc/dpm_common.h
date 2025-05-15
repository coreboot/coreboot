/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_COMMON_DPM_COMMON_H__
#define __SOC_MEDIATEK_COMMON_DPM_COMMON_H__

#include <soc/mcu_common.h>

void dpm_reset(struct mtk_mcu *mcu);
int dpm_init_mcu(struct mtk_mcu *mcu_list);
int dpm_init(void);

int dpm_4ch_para_setting(void);
int dpm_4ch_init(void);

#endif  /* __SOC_MEDIATEK_COMMON_DPM_COMMON_H__ */
