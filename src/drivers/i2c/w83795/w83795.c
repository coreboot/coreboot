/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Raptor Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <device/device.h>
#include "w83795.h"
#include <device/smbus.h>
#include "chip.h"

static int w83795_set_bank(struct device *dev, uint8_t bank)
{
	return smbus_write_byte(dev, W83795_REG_BANKSEL, bank);
}

static uint8_t w83795_read(struct device *dev, uint16_t reg)
{
	int ret;

	ret = w83795_set_bank(dev, reg >> 8);
	if (ret < 0) {
		printk(BIOS_DEBUG, "read failed to set bank %x\n", reg >> 8);
		return -1;
	}

	ret = smbus_read_byte(dev, reg & 0xff);
	return ret;
}

static uint8_t w83795_write(struct device *dev, uint16_t reg, uint8_t value)
{
	int err;

	err = w83795_set_bank(dev, reg >> 8);
	if (err < 0) {
		printk(BIOS_DEBUG, "write failed to set bank %x\n", reg >> 8);
		return -1;
	}

	err = smbus_write_byte(dev, reg & 0xff, value);
	return err;
}

/*
 * Configure Digital Temperature Sensor
 */
static void w83795_dts_configure(struct device *dev, uint8_t dts_src)
{
	u8 val;

	/* DIS */
	val = w83795_read(dev, W83795_REG_DTSC) & ~0x1;
	val |= dts_src & 0x1;
	w83795_write(dev, W83795_REG_DTSC, val);

	/* TODO
	 * Determine if DTS interface needs to be reset
	 * (W83795_REG_DTSC bit 7) before operation.
	 */
}

static u32 w83795_set_fan_mode(struct device *dev, w83795_fan_mode_t mode)
{
	if (mode == SPEED_CRUISE_MODE) {
		w83795_write(dev, W83795_REG_FCMS1, 0xFF);
		printk(BIOS_INFO, "W83795G/ADG work in Speed Cruise Mode\n");
	}  else {
		w83795_write(dev, W83795_REG_FCMS1, 0x00);
		if (mode == THERMAL_CRUISE_MODE) {
			w83795_write(dev, W83795_REG_FCMS2, 0x00);
			printk(BIOS_INFO, "W83795G/ADG work in Thermal Cruise Mode\n");
		} else if (mode == SMART_FAN_MODE) {
			w83795_write(dev, W83795_REG_FCMS2, 0x3F);
			printk(BIOS_INFO, "W83795G/ADG work in Smart Fan Mode\n");
		} else {
			printk(BIOS_INFO, "W83795G/ADG work in Manual Mode\n");
			return -1;
		}
	}

	return 0;
}

static void w83795_set_fan(struct device *dev, w83795_fan_mode_t mode)
{
	/* set fan output controlled mode (FCMS)*/
	w83795_set_fan_mode(dev, mode);

	if (mode == SMART_FAN_MODE) {
		/* Set the Relative Register-at SMART FAN IV Control Mode Table */
		//SFIV TODO
	}

	/* Set Hysteresis of Temperature (HT) */
	//TODO
}

static uint8_t fan_pct_to_cfg_val(uint8_t percent)
{
	uint16_t cfg = (((unsigned int)percent * 10000) / 3922);
	if (cfg > 0xff)
		cfg = 0xff;
	return cfg;
}

static uint8_t millivolts_to_limit_value_type1(int millivolts)
{
	/* Datasheet v1.41 pages 44 and 70 (VSEN1 - VSEN11, VTT) */
	return ((millivolts / 2) >> 2);
}

static uint8_t millivolts_to_limit_value_type2(int millivolts)
{
	/* Datasheet v1.41 page pages 44 and 70 (3VSB, 3VDD, VBAT) */
	return ((millivolts / 6) >> 2);
}

static uint16_t millivolts_to_limit_value_type3(int millivolts)
{
	/* Datasheet v1.41 page 45 (VSEN12, VSEN13, VDSEN14 - VDSEN17) */
	return (millivolts / 2);
}

static void w83795_init(struct device *dev, w83795_fan_mode_t mode, u8 dts_src)
{
	struct drivers_i2c_w83795_config *config = dev->chip_info;
	uint8_t i;
	uint8_t val;
	uint16_t limit_value;

#if IS_ENABLED(CONFIG_SMBUS_HAS_AUX_CHANNELS)
	uint8_t smbus_aux_channel_prev = smbus_get_current_channel();
	smbus_switch_to_channel(config->smbus_aux);
	printk(BIOS_DEBUG, "Set SMBUS controller to channel %d\n", config->smbus_aux);
#endif

	if (smbus_read_byte(dev, 0x00) < 0) {
#if IS_ENABLED(CONFIG_SMBUS_HAS_AUX_CHANNELS)
		/* Restore SMBUS channel setting */
		smbus_switch_to_channel(smbus_aux_channel_prev);
		printk(BIOS_DEBUG, "Set SMBUS controller to channel %d\n", smbus_aux_channel_prev);
#endif
		printk(BIOS_ERR, "W83795G/ADG Nuvoton H/W Monitor not found\n");
		return;
	}
	val = w83795_read(dev, W83795_REG_CONFIG);
	if ((val & W83795_REG_CONFIG_CONFIG48) == 0)
		printk(BIOS_INFO, "Found 64 pin W83795G Nuvoton H/W Monitor\n");
	else if ((val & W83795_REG_CONFIG_CONFIG48) == 1)
		printk(BIOS_INFO, "Found 48 pin W83795ADG Nuvoton H/W Monitor\n");

	/* Reset */
	val |= W83795_REG_CONFIG_INIT;
	w83795_write(dev, W83795_REG_CONFIG, val);

	/* Fan monitor settings */
	w83795_write(dev, W83795_REG_FANIN_CTRL1, config->fanin_ctl1);
	w83795_write(dev, W83795_REG_FANIN_CTRL2, config->fanin_ctl2);

	/* Temperature thresholds */
	w83795_write(dev, W83795_REG_TEMP_CRIT(0), config->tr1_critical_temperature);
	w83795_write(dev, W83795_REG_TEMP_CRIT_HYSTER(0), config->tr1_critical_hysteresis);
	w83795_write(dev, W83795_REG_TEMP_WARN(0), config->tr1_warning_temperature);
	w83795_write(dev, W83795_REG_TEMP_WARN_HYSTER(0), config->tr1_warning_hysteresis);
	w83795_write(dev, W83795_REG_TEMP_CRIT(1), config->tr2_critical_temperature);
	w83795_write(dev, W83795_REG_TEMP_CRIT_HYSTER(1), config->tr2_critical_hysteresis);
	w83795_write(dev, W83795_REG_TEMP_WARN(1), config->tr2_warning_temperature);
	w83795_write(dev, W83795_REG_TEMP_WARN_HYSTER(1), config->tr2_warning_hysteresis);
	w83795_write(dev, W83795_REG_TEMP_CRIT(2), config->tr3_critical_temperature);
	w83795_write(dev, W83795_REG_TEMP_CRIT_HYSTER(2), config->tr3_critical_hysteresis);
	w83795_write(dev, W83795_REG_TEMP_WARN(2), config->tr3_warning_temperature);
	w83795_write(dev, W83795_REG_TEMP_WARN_HYSTER(2), config->tr3_warning_hysteresis);
	w83795_write(dev, W83795_REG_TEMP_CRIT(3), config->tr4_critical_temperature);
	w83795_write(dev, W83795_REG_TEMP_CRIT_HYSTER(3), config->tr4_critical_hysteresis);
	w83795_write(dev, W83795_REG_TEMP_WARN(3), config->tr4_warning_temperature);
	w83795_write(dev, W83795_REG_TEMP_WARN_HYSTER(3), config->tr4_warning_hysteresis);
	w83795_write(dev, W83795_REG_TEMP_CRIT(4), config->tr5_critical_temperature);
	w83795_write(dev, W83795_REG_TEMP_CRIT_HYSTER(4), config->tr5_critical_hysteresis);
	w83795_write(dev, W83795_REG_TEMP_WARN(4), config->tr5_warning_temperature);
	w83795_write(dev, W83795_REG_TEMP_WARN_HYSTER(4), config->tr5_warning_hysteresis);
	w83795_write(dev, W83795_REG_TEMP_CRIT(5), config->tr6_critical_temperature);
	w83795_write(dev, W83795_REG_TEMP_CRIT_HYSTER(5), config->tr6_critical_hysteresis);
	w83795_write(dev, W83795_REG_TEMP_WARN(5), config->tr6_warning_temperature);
	w83795_write(dev, W83795_REG_TEMP_WARN_HYSTER(5), config->tr6_warning_hysteresis);

	/* DTS temperature thresholds */
	w83795_write(dev, W83795_REG_DTS_CRIT, config->dts_critical_temperature);
	w83795_write(dev, W83795_REG_DTS_CRIT_HYSTER, config->dts_critical_hysteresis);
	w83795_write(dev, W83795_REG_DTS_WARN, config->dts_warning_temperature);
	w83795_write(dev, W83795_REG_DTS_WARN_HYSTER, config->dts_warning_hysteresis);

	/* Configure DTS registers in bank3 before enabling DTS */
	w83795_dts_configure(dev, dts_src);

	/* DTS enable */
	w83795_write(dev, W83795_REG_DTSE, config->temp_dtse);

	/* Temperature monitor settings */
	w83795_write(dev, W83795_REG_TEMP_CTRL1, config->temp_ctl1);
	w83795_write(dev, W83795_REG_TEMP_CTRL2, config->temp_ctl2);

	/* Temperature to fan mappings */
	w83795_write(dev, W83795_REG_TFMR(0), config->temp1_fan_select);
	w83795_write(dev, W83795_REG_TFMR(1), config->temp2_fan_select);
	w83795_write(dev, W83795_REG_TFMR(2), config->temp3_fan_select);
	w83795_write(dev, W83795_REG_TFMR(3), config->temp4_fan_select);
	w83795_write(dev, W83795_REG_TFMR(4), config->temp5_fan_select);
	w83795_write(dev, W83795_REG_TFMR(5), config->temp6_fan_select);

	/* Temperature data source to temperature mappings */
	w83795_write(dev, W83795_REG_T12TSS, ((config->temp2_source_select & 0x0f) << 4) | (config->temp1_source_select & 0x0f));
	w83795_write(dev, W83795_REG_T34TSS, ((config->temp4_source_select & 0x0f) << 4) | (config->temp3_source_select & 0x0f));
	w83795_write(dev, W83795_REG_T56TSS, ((config->temp6_source_select & 0x0f) << 4) | (config->temp5_source_select & 0x0f));

	/* Set critical temperatures
	 * If any sensor exceeds the associated critical temperature,
	 * all fans will be forced to full speed.
	 */
	w83795_write(dev, W83795_REG_CTFS(0), config->temp1_critical_temperature);
	w83795_write(dev, W83795_REG_CTFS(1), config->temp2_critical_temperature);
	w83795_write(dev, W83795_REG_CTFS(2), config->temp3_critical_temperature);
	w83795_write(dev, W83795_REG_CTFS(3), config->temp4_critical_temperature);
	w83795_write(dev, W83795_REG_CTFS(4), config->temp5_critical_temperature);
	w83795_write(dev, W83795_REG_CTFS(5), config->temp6_critical_temperature);

	/* Set fan control target temperatures */
	w83795_write(dev, W83795_REG_TTTI(0), config->temp1_target_temperature);
	w83795_write(dev, W83795_REG_TTTI(1), config->temp2_target_temperature);
	w83795_write(dev, W83795_REG_TTTI(2), config->temp3_target_temperature);
	w83795_write(dev, W83795_REG_TTTI(3), config->temp4_target_temperature);
	w83795_write(dev, W83795_REG_TTTI(4), config->temp5_target_temperature);
	w83795_write(dev, W83795_REG_TTTI(5), config->temp6_target_temperature);

	/* Set fan stall prevention parameters */
	w83795_write(dev, W83795_REG_FAN_NONSTOP(0), config->fan1_nonstop);
	w83795_write(dev, W83795_REG_FAN_NONSTOP(1), config->fan2_nonstop);
	w83795_write(dev, W83795_REG_FAN_NONSTOP(2), config->fan3_nonstop);
	w83795_write(dev, W83795_REG_FAN_NONSTOP(3), config->fan4_nonstop);
	w83795_write(dev, W83795_REG_FAN_NONSTOP(4), config->fan5_nonstop);
	w83795_write(dev, W83795_REG_FAN_NONSTOP(5), config->fan6_nonstop);
	w83795_write(dev, W83795_REG_FAN_NONSTOP(6), config->fan7_nonstop);
	w83795_write(dev, W83795_REG_FAN_NONSTOP(7), config->fan8_nonstop);

	/* Set fan default speed */
	w83795_write(dev, W83795_REG_DFSP, fan_pct_to_cfg_val(config->default_speed));

	/* Set initial fan speeds */
	w83795_write(dev, W83795_REG_FAN_MANUAL_SPEED(0), fan_pct_to_cfg_val(config->fan1_duty));
	w83795_write(dev, W83795_REG_FAN_MANUAL_SPEED(1), fan_pct_to_cfg_val(config->fan2_duty));
	w83795_write(dev, W83795_REG_FAN_MANUAL_SPEED(2), fan_pct_to_cfg_val(config->fan3_duty));
	w83795_write(dev, W83795_REG_FAN_MANUAL_SPEED(3), fan_pct_to_cfg_val(config->fan4_duty));
	w83795_write(dev, W83795_REG_FAN_MANUAL_SPEED(4), fan_pct_to_cfg_val(config->fan5_duty));
	w83795_write(dev, W83795_REG_FAN_MANUAL_SPEED(5), fan_pct_to_cfg_val(config->fan6_duty));
	w83795_write(dev, W83795_REG_FAN_MANUAL_SPEED(6), fan_pct_to_cfg_val(config->fan7_duty));
	w83795_write(dev, W83795_REG_FAN_MANUAL_SPEED(7), fan_pct_to_cfg_val(config->fan8_duty));

	/* Voltage monitor settings */
	w83795_write(dev, W83795_REG_VOLT_CTRL1, config->volt_ctl1);
	w83795_write(dev, W83795_REG_VOLT_CTRL2, config->volt_ctl2);

	/* Voltage high/low limits */
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(0), millivolts_to_limit_value_type1(config->vcore1_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(0), millivolts_to_limit_value_type1(config->vcore1_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(1), millivolts_to_limit_value_type1(config->vcore2_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(1), millivolts_to_limit_value_type1(config->vcore2_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(2), millivolts_to_limit_value_type1(config->vsen3_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(2), millivolts_to_limit_value_type1(config->vsen3_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(3), millivolts_to_limit_value_type1(config->vsen4_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(3), millivolts_to_limit_value_type1(config->vsen4_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(4), millivolts_to_limit_value_type1(config->vsen5_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(4), millivolts_to_limit_value_type1(config->vsen5_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(5), millivolts_to_limit_value_type1(config->vsen6_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(5), millivolts_to_limit_value_type1(config->vsen6_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(6), millivolts_to_limit_value_type1(config->vsen7_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(6), millivolts_to_limit_value_type1(config->vsen7_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(7), millivolts_to_limit_value_type1(config->vsen8_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(7), millivolts_to_limit_value_type1(config->vsen8_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(8), millivolts_to_limit_value_type1(config->vsen9_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(8), millivolts_to_limit_value_type1(config->vsen9_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(9), millivolts_to_limit_value_type1(config->vsen10_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(9), millivolts_to_limit_value_type1(config->vsen10_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(10), millivolts_to_limit_value_type1(config->vsen11_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(10), millivolts_to_limit_value_type1(config->vsen11_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(11), millivolts_to_limit_value_type1(config->vtt_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(11), millivolts_to_limit_value_type1(config->vtt_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(12), millivolts_to_limit_value_type2(config->vdd_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(12), millivolts_to_limit_value_type2(config->vdd_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(13), millivolts_to_limit_value_type2(config->vsb_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(13), millivolts_to_limit_value_type2(config->vsb_low_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_HIGH(14), millivolts_to_limit_value_type2(config->vbat_high_limit_mv));
	w83795_write(dev, W83795_REG_VOLT_LIM_LOW(14), millivolts_to_limit_value_type2(config->vbat_low_limit_mv));

	/* VSEN12 limits */
	if (config->temp_ctl1 & 0x2) {
		limit_value = millivolts_to_limit_value_type3(config->vsen12_high_limit_mv);
		w83795_write(dev, W83795_REG_VOLT_LIM_HIGH_2_M(4), limit_value >> 2);
		w83795_write(dev, W83795_REG_VOLT_LIM_HIGH_2_L(4), limit_value & 0x3);
		limit_value = millivolts_to_limit_value_type3(config->vsen12_low_limit_mv);
		w83795_write(dev, W83795_REG_VOLT_LIM_LOW_2_M(4), limit_value >> 2);
		w83795_write(dev, W83795_REG_VOLT_LIM_LOW_2_L(4), limit_value & 0x3);
	}

	/* VSEN13 limits */
	if (config->temp_ctl1 & 0x8) {
		limit_value = millivolts_to_limit_value_type3(config->vsen13_high_limit_mv);
		w83795_write(dev, W83795_REG_VOLT_LIM_HIGH_2_M(5), limit_value >> 2);
		w83795_write(dev, W83795_REG_VOLT_LIM_HIGH_2_L(5), limit_value & 0x3);
		limit_value = millivolts_to_limit_value_type3(config->vsen13_low_limit_mv);
		w83795_write(dev, W83795_REG_VOLT_LIM_LOW_2_M(5), limit_value >> 2);
		w83795_write(dev, W83795_REG_VOLT_LIM_LOW_2_L(5), limit_value & 0x3);
	}

	w83795_set_fan(dev, mode);

	/* Show current fan control settings */
	printk(BIOS_INFO, "Fan\tCTFS(celsius)\tTTTI(celsius)\n");
	for (i = 0; i < 6; i++) {
		val = w83795_read(dev, W83795_REG_CTFS(i));
		printk(BIOS_INFO, " %x\t%d", i + 1, val);
		val = w83795_read(dev, W83795_REG_TTTI(i));
		printk(BIOS_INFO, "\t%d\n", val);
	}

	/* Show current temperatures */
	for (i = 0; i < 8; i++) {
		val = w83795_read(dev, W83795_REG_DTS(i));
		printk(BIOS_DEBUG, "DTS%x current value: %x\n", i + 1, val);
	}

	/* Start monitoring / fan control */
	val = w83795_read(dev, W83795_REG_CONFIG);
	val |= W83795_REG_CONFIG_START;
	w83795_write(dev, W83795_REG_CONFIG, val);

#if IS_ENABLED(CONFIG_SMBUS_HAS_AUX_CHANNELS)
	/* Restore SMBUS channel setting */
	smbus_switch_to_channel(smbus_aux_channel_prev);
	printk(BIOS_DEBUG, "Set SMBUS controller to channel %d\n", smbus_aux_channel_prev);
#endif
}

static void w83795_hwm_init(struct device *dev)
{
	struct device *cpu;
	struct cpu_info *info;

	info = cpu_info();
	cpu = info->cpu;
	if (!cpu)
		die("CPU: missing cpu device structure");

	if (cpu->vendor == X86_VENDOR_AMD)
		w83795_init(dev, THERMAL_CRUISE_MODE, DTS_SRC_AMD_SBTSI);
	else if (cpu->vendor == X86_VENDOR_INTEL)
		w83795_init(dev, THERMAL_CRUISE_MODE, DTS_SRC_INTEL_PECI);
	else
		printk(BIOS_ERR, "Neither AMD nor INTEL CPU detected\n");
}

static struct device_operations w83795_operations = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init = w83795_hwm_init,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &w83795_operations;
}

struct chip_operations drivers_i2c_w83795_ops = {
	CHIP_NAME("Nuvoton W83795G/ADG Hardware Monitor")
	.enable_dev = enable_dev,
};
