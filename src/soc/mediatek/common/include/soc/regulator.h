/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_REGULATOR_H
#define SOC_MEDIATEK_COMMON_REGULATOR_H

#include <stdint.h>

enum mtk_regulator {
	MTK_REGULATOR_VDD1,
	MTK_REGULATOR_VDD2,
	MTK_REGULATOR_VDDQ,
	MTK_REGULATOR_VMDDR,
	MTK_REGULATOR_VCORE,
};

void mainboard_set_regulator_vol(enum mtk_regulator regulator,
				 uint32_t voltage_uv);
uint32_t mainboard_get_regulator_vol(enum mtk_regulator regulator);

#endif /* SOC_MEDIATEK_COMMON_REGULATOR_H */
