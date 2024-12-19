/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_COMMON_INCLUDE_SOC_MTK_FSP_H__
#define __SOC_MEDIATEK_COMMON_INCLUDE_SOC_MTK_FSP_H__

#include <soc/mtk_fsp_common.h>

void mtk_fsp_init(enum fsp_phase phase);
enum cb_err mtk_fsp_add_param(enum fsp_param_type type, size_t param_size,
			      void *param);
enum cb_err mtk_fsp_load_and_run(void);

#endif /* __SOC_MEDIATEK_COMMON_INCLUDE_SOC_MTK_FSP_H__ */
