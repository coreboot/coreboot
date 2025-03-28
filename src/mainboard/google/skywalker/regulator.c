/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <soc/mt6359p.h>
#include <soc/regulator.h>

#define MTK_REGULATOR_INVALID -1

static int get_mt6359p_regulator_id(enum mtk_regulator regulator)
{
	switch (regulator) {
	case MTK_REGULATOR_VPROC12:
		return MT6359P_VMODEM;
	case MTK_REGULATOR_VSRAM_PROC12:
		return MT6359P_VSRAM_MD;
	case MTK_REGULATOR_VMCH:
		return MT6359P_PA;
	case MTK_REGULATOR_VMC:
		return MT6359P_SIM1;
	default:
		return MTK_REGULATOR_INVALID;
	}
}

void mainboard_set_regulator_voltage(enum mtk_regulator regulator, uint32_t voltage_uv)
{
	int id;

	id = get_mt6359p_regulator_id(regulator);
	if (id < 0) {
		printk(BIOS_ERR, "%s: Invalid regulator ID: %d\n", __func__, regulator);
		return;
	}

	switch (id) {
	case MT6359P_VMODEM:
		mt6359p_buck_set_voltage(MT6359P_VMODEM, voltage_uv);
		break;
	case MT6359P_VSRAM_MD:
		mt6359p_buck_set_voltage(MT6359P_VSRAM_MD, voltage_uv);
		break;
	case MT6359P_PA:
		mt6359p_buck_set_voltage(MT6359P_PA, voltage_uv);
		break;
	case MT6359P_SIM1:
		mt6359p_set_vsim1_voltage(voltage_uv);
		break;
	default:
		printk(BIOS_ERR, "%s: Regulator ID %d: not supported\n", __func__, regulator);
		break;
	}
}

uint32_t mainboard_get_regulator_voltage(enum mtk_regulator regulator)
{
	int id;

	id = get_mt6359p_regulator_id(regulator);
	if (id < 0) {
		printk(BIOS_ERR, "%s: Invalid regulator ID: %d\n", __func__, regulator);
		return 0;
	}

	switch (id) {
	case MT6359P_VMODEM:
		return mt6359p_buck_get_voltage(MT6359P_VMODEM);
	case MT6359P_VSRAM_MD:
		return mt6359p_buck_get_voltage(MT6359P_VSRAM_MD);
	case MT6359P_PA:
		return mt6359p_buck_get_voltage(MT6359P_PA);
	case MT6359P_SIM1:
		return mt6359p_get_vsim1_voltage();
	default:
		printk(BIOS_ERR, "%s: Regulator ID %d: not supported\n", __func__, regulator);
		return 0;
	}
}

int mainboard_enable_regulator(enum mtk_regulator regulator, bool enable)
{
	int id;

	id = get_mt6359p_regulator_id(regulator);
	if (id < 0) {
		printk(BIOS_ERR, "%s: Invalid regulator ID: %d\n", __func__, regulator);
		return -1;
	}

	switch (id) {
	case MT6359P_PA:
		mt6359p_enable_vpa(enable);
		break;
	case MT6359P_SIM1:
		mt6359p_enable_vsim1(enable);
		break;
	default:
		printk(BIOS_ERR, "%s: Regulator ID %d: not supported\n", __func__, regulator);
		break;
	}

	return 0;
}
