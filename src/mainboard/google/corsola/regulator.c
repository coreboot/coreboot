/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <soc/mt6366.h>
#include <soc/regulator.h>

#define REGULATOR_NOT_SUPPORT -1

static const int regulator_id[] = {
	[MTK_REGULATOR_VDD1] = REGULATOR_NOT_SUPPORT,
	[MTK_REGULATOR_VDD2] = REGULATOR_NOT_SUPPORT,
	[MTK_REGULATOR_VDDQ] = MT6366_VDDQ,
	[MTK_REGULATOR_VMDDR] = REGULATOR_NOT_SUPPORT,
	[MTK_REGULATOR_VCORE] = MT6366_VCORE,
	[MTK_REGULATOR_VCC] = REGULATOR_NOT_SUPPORT,
	[MTK_REGULATOR_VCCQ] = REGULATOR_NOT_SUPPORT,
	[MTK_REGULATOR_VDRAM1] = MT6366_VDRAM1,
	[MTK_REGULATOR_VMCH] = MT6366_VMCH,
	[MTK_REGULATOR_VMC] = MT6366_VMC,
	[MTK_REGULATOR_VPROC12] = MT6366_VPROC12,
	[MTK_REGULATOR_VSRAM_PROC12] = MT6366_VSRAM_PROC12,
	[MTK_REGULATOR_VRF12] = MT6366_VRF12,
	[MTK_REGULATOR_VCN33] = MT6366_VCN33,
};

_Static_assert(ARRAY_SIZE(regulator_id) == MTK_REGULATOR_NUM, "regulator_id size error");

void mainboard_set_regulator_voltage(enum mtk_regulator regulator, uint32_t voltage_uv)
{
	assert(regulator < MTK_REGULATOR_NUM);

	if (regulator_id[regulator] < 0) {
		printk(BIOS_ERR, "Invalid regulator ID: %d\n", regulator);
		return;
	}
	mt6366_set_voltage(regulator_id[regulator], voltage_uv);
}

uint32_t mainboard_get_regulator_voltage(enum mtk_regulator regulator)
{
	assert(regulator < MTK_REGULATOR_NUM);

	if (regulator_id[regulator] < 0) {
		printk(BIOS_ERR, "Invalid regulator ID: %d\n", regulator);
		return 0;
	}
	return mt6366_get_voltage(regulator_id[regulator]);
}
