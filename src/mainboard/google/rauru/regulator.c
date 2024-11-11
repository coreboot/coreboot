/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <soc/mt6373.h>
#include <soc/regulator.h>

#define MTK_REGULATOR_INVALID -1

static int get_mt6373_regulator_id(enum mtk_regulator regulator)
{
	switch (regulator) {
	case MTK_REGULATOR_VMCH:
		return MT6373_VMCH;
	case MTK_REGULATOR_VMC:
		return MT6373_VMC;
	default:
		return MTK_REGULATOR_INVALID;
	}
}

void mainboard_set_regulator_voltage(enum mtk_regulator regulator, uint32_t voltage_uv)
{
	int id;

	id = get_mt6373_regulator_id(regulator);
	if (id < 0) {
		printk(BIOS_ERR, "Invalid regulator ID: %d\n", regulator);
		return;
	}

	switch (id) {
	case MT6373_VMCH:
		mt6373_set_vmch_voltage(voltage_uv);
		break;
	case MT6373_VMC:
		mt6373_set_vmc_voltage(voltage_uv);
		break;
	default:
		printk(BIOS_ERR, "Regulator ID %d: not supported\n", regulator);
		break;
	}
}

uint32_t mainboard_get_regulator_voltage(enum mtk_regulator regulator)
{
	int id;

	id = get_mt6373_regulator_id(regulator);
	if (id < 0) {
		printk(BIOS_ERR, "Invalid regulator ID: %d\n", regulator);
		return 0;
	}

	switch (id) {
	case MT6373_VMCH:
		return mt6373_get_vmch_voltage();
	case MT6373_VMC:
		return mt6373_get_vmc_voltage();
	default:
		printk(BIOS_ERR, "Regulator ID %d: not supported\n", regulator);
		return 0;
	}
}

int mainboard_enable_regulator(enum mtk_regulator regulator, bool enable)
{
	int id;

	id = get_mt6373_regulator_id(regulator);
	if (id < 0) {
		printk(BIOS_ERR, "Invalid regulator ID: %d\n", regulator);
		return -1;
	}

	switch (id) {
	case MT6373_VMCH:
		mt6373_enable_vmch(enable);
		break;
	case MT6373_VMC:
		mt6373_enable_vmc(enable);
		break;
	default:
		printk(BIOS_ERR, "Regulator ID %d: not supported\n", regulator);
		break;
	}

	return 0;
}
