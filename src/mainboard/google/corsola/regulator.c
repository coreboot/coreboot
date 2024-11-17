/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/mt6366.h>
#include <soc/regulator.h>

#define MTK_REGULATOR_INVALID -1

static int get_mt6366_regulator_id(enum mtk_regulator regulator)
{
	switch (regulator) {
	case MTK_REGULATOR_VDDQ:
		return MT6366_VDDQ;
	case MTK_REGULATOR_VCORE:
		return MT6366_VCORE;
	case MTK_REGULATOR_VDRAM1:
		return MT6366_VDRAM1;
	case MTK_REGULATOR_VMCH:
		return MT6366_VMCH;
	case MTK_REGULATOR_VMC:
		return MT6366_VMC;
	case MTK_REGULATOR_VPROC12:
		return MT6366_VPROC12;
	case MTK_REGULATOR_VSRAM_PROC12:
		return MT6366_VSRAM_PROC12;
	case MTK_REGULATOR_VRF12:
		return MT6366_VRF12;
	case MTK_REGULATOR_VCN33:
		return MT6366_VCN33;
	case MTK_REGULATOR_VIO18:
		return MT6366_VIO18;
	default:
		return MTK_REGULATOR_INVALID;
	}
}

void mainboard_set_regulator_voltage(enum mtk_regulator regulator, uint32_t voltage_uv)
{
	int id;

	id = get_mt6366_regulator_id(regulator);
	if (id < 0) {
		printk(BIOS_ERR, "Invalid regulator ID: %d\n", regulator);
		return;
	}
	mt6366_set_voltage(id, voltage_uv);
}

uint32_t mainboard_get_regulator_voltage(enum mtk_regulator regulator)
{
	int id;

	id = get_mt6366_regulator_id(regulator);
	if (id < 0) {
		printk(BIOS_ERR, "Invalid regulator ID: %d\n", regulator);
		return 0;
	}
	return mt6366_get_voltage(id);
}
