/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/mt6359p.h>
#include <soc/regulator.h>

#define MTK_REGULATOR_INVALID -1

static int get_mt6359p_regulator_id(enum mtk_regulator regulator)
{
	switch (regulator) {
	case MTK_REGULATOR_VCORE:
		return MT6359P_GPU11;
	case MTK_REGULATOR_VPROC11:
		return MT6359P_CORE;
	case MTK_REGULATOR_VSRAM_PROC11:
		return MT6359P_SRAM_PROC1;
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
		printk(BIOS_ERR, "Invalid regulator ID: %d\n", regulator);
		return;
	}

	if (id == MT6359P_SIM1)
		mt6359p_set_vsim1_voltage(voltage_uv);
	else
		mt6359p_buck_set_voltage(id, voltage_uv);
}

uint32_t mainboard_get_regulator_voltage(enum mtk_regulator regulator)
{
	int id;

	id = get_mt6359p_regulator_id(regulator);
	if (id < 0) {
		printk(BIOS_ERR, "Invalid regulator ID: %d\n", regulator);
		return 0;
	}

	if (id == MT6359P_SIM1)
		return mt6359p_get_vsim1_voltage();

	return mt6359p_buck_get_voltage(id);
}

int mainboard_enable_regulator(enum mtk_regulator regulator, bool enable)
{
	int id;

	id = get_mt6359p_regulator_id(regulator);
	if (id < 0) {
		printk(BIOS_ERR, "Invalid regulator ID: %d\n", regulator);
		return -1;
	}

	if (id == MT6359P_SIM1)
		mt6359p_enable_vsim1(enable);
	else if (id == MT6359P_PA)
		mt6359p_enable_vpa(enable);
	else
		printk(BIOS_INFO, "No need to enable regulator ID: %d\n", regulator);

	return 0;
}
