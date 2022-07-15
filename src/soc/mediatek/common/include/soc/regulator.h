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
	MTK_REGULATOR_VCC,
	MTK_REGULATOR_VCCQ,
	MTK_REGULATOR_VDRAM1,
	MTK_REGULATOR_VMCH,
	MTK_REGULATOR_VMC,
	MTK_REGULATOR_VPROC11,
	MTK_REGULATOR_VPROC12,
	MTK_REGULATOR_VSRAM_PROC11,
	MTK_REGULATOR_VSRAM_PROC12,
	MTK_REGULATOR_VRF12,
	MTK_REGULATOR_VCN33,
	MTK_REGULATOR_NUM,
};

void mainboard_set_regulator_voltage(enum mtk_regulator regulator, uint32_t voltage_uv);
uint32_t mainboard_get_regulator_voltage(enum mtk_regulator regulator);
int mainboard_enable_regulator(enum mtk_regulator regulator, bool enable);
bool mainboard_regulator_is_enabled(enum mtk_regulator regulator);

#endif /* SOC_MEDIATEK_COMMON_REGULATOR_H */
