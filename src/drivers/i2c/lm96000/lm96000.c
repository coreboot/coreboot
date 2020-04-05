/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <delay.h>
#include <timer.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c_bus.h>

#include "lm96000.h"
#include "chip.h"

static inline int lm96000_read(struct device *const dev, const u8 reg)
{
	return i2c_dev_readb_at(dev, reg);
}

static inline int lm96000_write(struct device *const dev,
				const u8 reg, const u8 value)
{
	return i2c_dev_writeb_at(dev, reg, value);
}

static inline int lm96000_update(struct device *const dev, const u8 reg,
				 const u8 clear_mask, const u8 set_mask)
{
	const int val = i2c_dev_readb_at(dev, reg);
	if (val < 0)
		return val;
	return i2c_dev_writeb_at(dev, reg, (val & ~clear_mask) | set_mask);
}

static const unsigned int ref_mv[] = { 2500, 2250, 3300, 5000, 12000 };

static u8 lm96000_to_low_limit(const enum lm96000_vin ref, const u16 limit)
{
	const unsigned int reg =
		(unsigned int)limit * 0xc0 / ref_mv[ref];
	return reg < 0xff ? reg : 0xff;
}

static u8 lm96000_to_high_limit(const enum lm96000_vin ref, const u16 limit)
{
	const unsigned int reg =
		DIV_ROUND_UP((unsigned int)limit * 0xc0, ref_mv[ref]);
	return reg < 0xff ? reg : 0xff;
}

static void lm96000_set_vin_limits(struct device *const dev,
		const struct drivers_i2c_lm96000_config *const config)
{
	unsigned int i;

	for (i = 0; i < LM96000_VIN_CNT; ++i) {
		lm96000_write(dev, LM96000_VIN_LOW_LIMIT(i),
			      lm96000_to_low_limit(i, config->vin[i].low));
		if (config->vin[i].high > config->vin[i].low)
			lm96000_write(dev, LM96000_VIN_HIGH_LIMIT(i),
				lm96000_to_high_limit(i, config->vin[i].high));
		else
			lm96000_write(dev, LM96000_VIN_HIGH_LIMIT(i), 0xff);
	}
}

static void lm96000_set_temp_limits(struct device *const dev,
		const struct drivers_i2c_lm96000_config *const config)
{
	unsigned int i;

	for (i = 0; i < LM96000_TEMP_IN_CNT; ++i) {
		lm96000_write(dev, LM96000_TEMP_LOW_LIMIT(i),
			      config->temp_in[i].low);
		if (config->temp_in[i].high > config->temp_in[i].low)
			lm96000_write(dev, LM96000_TEMP_HIGH_LIMIT(i),
				      config->temp_in[i].high);
		else
			lm96000_write(dev, LM96000_TEMP_HIGH_LIMIT(i), 0x7f);
	}
}

static u16 lm96000_rpm_to_tach(const u16 rpm)
{
	return rpm ? (60 * 90000 / rpm) & 0xfffc : 0xfffc;
}

static void lm96000_set_fan_limits(struct device *const dev,
		const struct drivers_i2c_lm96000_config *const config)
{
	unsigned int i;

	for (i = 0; i < LM96000_FAN_IN_CNT; ++i) {
		const u16 tach = lm96000_rpm_to_tach(config->fan_in[i].low);
		lm96000_write(dev, LM96000_FAN_LOW_LIMIT(i), tach & 0xff);
		lm96000_write(dev, LM96000_FAN_LOW_LIMIT(i) + 1, tach >> 8);
	}
}

static u8 lm96000_to_duty(const u8 duty_cycle)
{
	return duty_cycle * 255 / 100;
}

static void lm96000_configure_pwm(struct device *const dev,
				  const unsigned int fan,
				  const struct lm96000_fan_config *const config)
{
	lm96000_update(dev, LM96000_FAN_CFG(fan),
		       LM96000_FAN_CFG_MODE_MASK | LM96000_FAN_CFG_PWM_INVERT |
		       LM96000_FAN_CFG_SPINUP_MASK,
		       ((config->mode << LM96000_FAN_CFG_MODE_SHIFT)
			& LM96000_FAN_CFG_MODE_MASK) |
		       (config->invert ? LM96000_FAN_CFG_PWM_INVERT : 0) |
		       config->spinup);
	lm96000_update(dev, LM96000_FAN_FREQ(fan),
		       LM96000_FAN_FREQ_MASK, config->freq);
	lm96000_update(dev, LM96000_TACH_MONITOR_MODE,
		       LM96000_TACH_MODE_FAN_MASK(fan),
		       config->freq <= LM96000_PWM_94HZ
		       ? config->tach << LM96000_TACH_MODE_FAN_SHIFT(fan) : 0);

	switch (config->mode) {
	case LM96000_FAN_ZONE_1_AUTO:
	case LM96000_FAN_ZONE_2_AUTO:
	case LM96000_FAN_ZONE_3_AUTO:
	case LM96000_FAN_HOTTEST_23:
	case LM96000_FAN_HOTTEST_123:
		lm96000_write(dev, LM96000_FAN_MIN_PWM(fan),
			      lm96000_to_duty(config->min_duty));
		break;
	case LM96000_FAN_MANUAL:
		lm96000_write(dev, LM96000_FAN_DUTY(fan),
			      lm96000_to_duty(config->duty_cycle));
		break;
	default:
		break;
	}
}

static void lm96000_configure_temp_zone(struct device *const dev,
		const unsigned int zone,
		const struct lm96000_temp_zone *const config)
{
	static const u8 temp_range[] =
		{ 2, 3, 3, 4, 5, 7, 8, 10, 13, 16, 20, 27, 32, 40, 53, 80 };
	unsigned int i;

	/* find longest range that starts from `low_temp` */
	for (i = ARRAY_SIZE(temp_range) - 1; i > 0; --i) {
		if (config->low_temp + temp_range[i] <= config->target_temp)
			break;
	}

	lm96000_update(dev, LM96000_ZONE_RANGE(zone),
		       LM96000_ZONE_RANGE_MASK, i << LM96000_ZONE_RANGE_SHIFT);
	lm96000_write(dev, LM96000_ZONE_TEMP_LOW(zone),
		      config->target_temp >= temp_range[i]
		      ? config->target_temp - temp_range[i]
		      : 0);
	lm96000_write(dev, LM96000_ZONE_TEMP_PANIC(zone),
		      config->panic_temp ? config->panic_temp : 100);
	lm96000_update(dev, LM96000_ZONE_SMOOTH(zone),
		       LM96000_ZONE_SMOOTH_MASK(zone),
		       LM96000_ZONE_SMOOTH_EN(zone) | 0); /* 0: 35s */
	lm96000_update(dev, LM96000_FAN_MIN_OFF,
		       LM96000_FAN_MIN(zone),
		       config->min_off ? LM96000_FAN_MIN(zone) : 0);
	lm96000_update(dev, LM96000_ZONE_HYSTERESIS(zone),
		       LM96000_ZONE_HYST_MASK(zone),
		       config->hysteresis << LM96000_ZONE_HYST_SHIFT(zone)
			& LM96000_ZONE_HYST_MASK(zone));
}

static void lm96000_init(struct device *const dev)
{
	const struct drivers_i2c_lm96000_config *const config = dev->chip_info;
	unsigned int i;
	int lm_config;
	struct stopwatch sw;

	printk(BIOS_DEBUG, "lm96000: Initialization hardware monitoring.\n");

	stopwatch_init_msecs_expire(&sw, 1000);
	lm_config = lm96000_read(dev, LM96000_CONFIG);
	while ((lm_config < 0 || !((unsigned int)lm_config & LM96000_READY))) {
		mdelay(1);
		lm_config = lm96000_read(dev, LM96000_CONFIG);
		if (stopwatch_expired(&sw))
			break;
	}
	if (lm_config < 0 || !((unsigned int)lm_config & LM96000_READY)) {
		printk(BIOS_INFO, "lm96000: Not ready after 1s.\n");
		return;
	}

	lm96000_set_vin_limits(dev, config);
	lm96000_set_temp_limits(dev, config);
	lm96000_set_fan_limits(dev, config);
	for (i = 0; i < LM96000_PWM_CTL_CNT; ++i) {
		if (config->fan[i].mode != LM96000_FAN_IGNORE)
			lm96000_configure_pwm(dev, i, config->fan + i);
	}
	for (i = 0; i < LM96000_TEMP_ZONE_CNT; ++i)
		lm96000_configure_temp_zone(dev, i, config->zone + i);
	lm96000_update(dev, LM96000_CONFIG, 0, LM96000_START);
}

static struct device_operations lm96000_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.init			= lm96000_init,
};

static void lm96000_enable(struct device *const dev)
{
	dev->ops = &lm96000_ops;
}

struct chip_operations drivers_i2c_lm96000_ops = {
	CHIP_NAME("LM96000")
	.enable_dev = lm96000_enable
};
