/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <soc/mt6359p.h>
#include <soc/mt6360.h>
#include <soc/mt6691.h>
#include <soc/regulator.h>

#define MT6691_I2C_NUM	7

static int get_mt6360_regulator_id(enum mtk_regulator regulator)
{
	switch (regulator) {
	case MTK_REGULATOR_VDD2:
		return MT6360_BUCK1;
	case MTK_REGULATOR_VDDQ:
		return MT6360_BUCK2;
	case MTK_REGULATOR_VCC:
		return MT6360_LDO5;
	case MTK_REGULATOR_VCCQ:
		return MT6360_LDO3;
	default:
		break;
	}

	return -1;
}

static int get_mt6359p_regulator_id(enum mtk_regulator regulator)
{
	return regulator == MTK_REGULATOR_VCORE ? MT6359P_GPU11 : -1;
}

static int get_mt6691_regulator_id(enum mtk_regulator regulator)
{
	return regulator == MTK_REGULATOR_VMDDR ? MT6691_I2C_NUM : -1;
}

static int check_regulator_control(enum mtk_regulator regulator)
{
	/*
	 * MT6880 is not controlled by SW.
	 * No need to control it.
	 */
	if (regulator == MTK_REGULATOR_VDD1) {
		printk(BIOS_WARNING,
		       "[%d] MT6880 is not controlled by SW.\n", regulator);
		return -1;
	}
	return 0;
}

void mainboard_set_regulator_voltage(enum mtk_regulator regulator, uint32_t voltage_uv)
{
	if (check_regulator_control(regulator) < 0)
		return;

	int id;

	id = get_mt6360_regulator_id(regulator);
	if (id >= 0) {
		if (CONFIG(BOARD_GOOGLE_CHERRY)) {
			mt6360_set_voltage(id, voltage_uv);
		} else {
			uint32_t voltage_mv = voltage_uv / 1000;
			if (google_chromeec_regulator_set_voltage(id, voltage_mv,
								  voltage_mv) < 0) {
				printk(BIOS_WARNING,
				       "Failed to set voltage by ec: %d\n", regulator);
			}
		}
		return;
	}

	id = get_mt6359p_regulator_id(regulator);
	if (id >= 0) {
		mt6359p_buck_set_voltage(id, voltage_uv);
		return;
	}

	id = get_mt6691_regulator_id(regulator);
	if (id >= 0) {
		mt6691_set_voltage(id, voltage_uv);
		return;
	}

	printk(BIOS_ERR, "Invalid regulator ID: %d\n", regulator);
}

uint32_t mainboard_get_regulator_voltage(enum mtk_regulator regulator)
{
	if (check_regulator_control(regulator) < 0)
		return 0;

	int id;

	id = get_mt6360_regulator_id(regulator);
	if (id >= 0) {
		if (CONFIG(BOARD_GOOGLE_CHERRY)) {
			return mt6360_get_voltage(id);
		} else {
			uint32_t voltage_mv = 0;
			if (google_chromeec_regulator_get_voltage(id, &voltage_mv) < 0) {
				printk(BIOS_WARNING,
				       "Failed to get voltage by ec: %d\n", regulator);
				return 0;
			}
			return voltage_mv * 1000;
		}
	}

	id = get_mt6359p_regulator_id(regulator);
	if (id >= 0)
		return mt6359p_buck_get_voltage(id);

	id = get_mt6691_regulator_id(regulator);
	if (id >= 0)
		return mt6691_get_voltage(id);

	printk(BIOS_ERR, "Invalid regulator ID: %d\n", regulator);

	return 0;
}

int mainboard_enable_regulator(enum mtk_regulator regulator, uint8_t enable)
{
	if (check_regulator_control(regulator) < 0)
		return 0;

	/* Return 0 if the regulator is already enabled or disabled. */
	if (mainboard_regulator_is_enabled(regulator) == enable)
		return 0;

	int id;

	id = get_mt6360_regulator_id(regulator);
	if (id >= 0) {
		if (CONFIG(BOARD_GOOGLE_CHERRY)) {
			mt6360_enable(id, enable);
			return 0;
		} else {
			if (google_chromeec_regulator_enable(id, enable) < 0) {
				printk(BIOS_WARNING,
				       "Failed to enable regulator by ec: %d\n", regulator);
				return -1;
			}
			return 0;
		}
	}

	printk(BIOS_ERR, "Invalid regulator ID: %d\n", regulator);

	return -1;
}

uint8_t mainboard_regulator_is_enabled(enum mtk_regulator regulator)
{
	if (check_regulator_control(regulator) < 0)
		return 0;

	int id;

	id = get_mt6360_regulator_id(regulator);
	if (id >= 0) {
		if (CONFIG(BOARD_GOOGLE_CHERRY)) {
			return mt6360_is_enabled(id);
		} else {
			uint8_t enabled;
			if (google_chromeec_regulator_is_enabled(id, &enabled) < 0) {
				printk(BIOS_WARNING,
				       "Failed to retrieve is_enabled by ec; assuming disabled\n");
				return 0;
			}
			return enabled;
		}

	}

	printk(BIOS_ERR, "Invalid regulator ID: %d\n; assuming disabled", regulator);

	return 0;
}
