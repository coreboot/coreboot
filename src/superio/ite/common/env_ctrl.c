/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <delay.h>
#include <stddef.h>
#include <superio/hwm5_conf.h>
#include <option.h>

#include "env_ctrl.h"
#include "env_ctrl_chip.h"

static void extemp_force_idle_status(const u16 base)
{
	u8 reg;
	int retries = 10;

	/* Wait up to 10ms for non-busy state. */
	while (retries > 0) {
		reg = pnp_read_hwm5_index(base, ITE_EC_EXTEMP_STATUS);

		if ((reg & ITE_EC_EXTEMP_STATUS_HOST_BUSY) == 0x0)
			break;

		retries--;

		mdelay(1);
	}

	if (retries == 0 && (reg & ITE_EC_EXTEMP_STATUS_HOST_BUSY) == 0x1) {
		/*
		 * SIO is busy due to unfinished peci transaction.
		 * Re-configure Register 0x8E to terminate processes.
		 */
		pnp_write_hwm5_index(base, ITE_EC_EXTEMP_CONTROL,
			ITE_EC_EXTEMP_CTRL_AUTO_4HZ | ITE_EC_EXTEMP_CTRL_AUTO_START);
	}
}

/*
 * Setup PECI interface
 */
static void enable_peci(const u16 base)
{
	/* Enable PECI interface */
	pnp_write_hwm5_index(base, ITE_EC_INTERFACE_SELECT,
			   ITE_EC_INTERFACE_SEL_PECI | ITE_EC_INTERFACE_SPEED_TOLERANCE);

	/* Setup External Temperature using PECI GetTemp */
	pnp_write_hwm5_index(base, ITE_EC_EXTEMP_ADDRESS, PECI_CLIENT_ADDRESS);
	pnp_write_hwm5_index(base, ITE_EC_EXTEMP_COMMAND, PECI_GETTEMP_COMMAND);
	pnp_write_hwm5_index(base, ITE_EC_EXTEMP_WRITE_LENGTH, PECI_GETTEMP_WRITE_LENGTH);
	pnp_write_hwm5_index(base, ITE_EC_EXTEMP_READ_LENGTH, PECI_GETTEMP_READ_LENGTH);
	pnp_write_hwm5_index(base, ITE_EC_EXTEMP_CONTROL,
		ITE_EC_EXTEMP_CTRL_AUTO_4HZ | ITE_EC_EXTEMP_CTRL_AUTO_START);
}

/*
 * Set up External Temperature to read via PECI or thermal diode/resistor
 * into TMPINx register
 */
static void enable_tmpin(const u16 base, const u8 tmpin,
			 const struct ite_ec_thermal_config *const conf)
{
	u8 reg;
	u8 reg_extra;

	reg = pnp_read_hwm5_index(base, ITE_EC_ADC_TEMP_CHANNEL_ENABLE);
	reg_extra = pnp_read_hwm5_index(base, ITE_EC_ADC_TEMP_EXTRA_CHANNEL_ENABLE);

	switch (conf->mode) {
	case THERMAL_MODE_DISABLED:
		return;
	case THERMAL_PECI:
		/* Some chips can set any TMPIN as the target for PECI readings
		   while others can only read to TMPIN3. In the latter case a
		   different register is used for enabling it. */
		if (CONFIG(SUPERIO_ITE_ENV_CTRL_EXT_ANY_TMPIN)) {
			/* IT8721F is an exception, it cannot use TMPIN2 for PECI. */
			if (CONFIG(SUPERIO_ITE_IT8721F) && tmpin == 2) {
				printk(BIOS_WARNING,
				       "PECI to TMPIN2 not supported on IT8721F\n");
				return;
			}
			u8 reg_new = (reg & ~ITE_EC_ADC_TEMP_EXT_REPORTS_TO_MASK)
					   | ITE_EC_ADC_TEMP_EXT_REPORTS_TO(tmpin);
			/* Registers stick on reboot and resume,
			   don't warn for correct reg values */
			if (reg & ITE_EC_ADC_TEMP_EXT_REPORTS_TO_MASK && reg != reg_new) {
				printk(BIOS_WARNING,
				       "PECI specified for another TMPIN, overwriting\n");
			}
			reg = reg_new;
		} else if (tmpin == 3) {
			reg_extra |= ITE_EC_ADC_TEMP_EXTRA_TMPIN3_EXT;
			pnp_write_hwm5_index(base, ITE_EC_ADC_TEMP_EXTRA_CHANNEL_ENABLE,
					     reg_extra);
		} else {
			printk(BIOS_WARNING, "PECI to TMPIN%d not supported on this Super I/O",
			       tmpin);
			return;
		}
		enable_peci(base);

		break;
	case THERMAL_DIODE:
		reg |= ITE_EC_ADC_TEMP_DIODE_MODE(tmpin);
		break;
	case THERMAL_RESISTOR:
		reg |= ITE_EC_ADC_TEMP_RESISTOR_MODE(tmpin);
		break;
	default:
		printk(BIOS_WARNING, "Unsupported thermal mode 0x%x on TMPIN%d\n",
		       conf->mode, tmpin);
		return;
	}

	pnp_write_hwm5_index(base, ITE_EC_ADC_TEMP_CHANNEL_ENABLE, reg);

	/* Set temperature offsets */
	if (conf->mode != THERMAL_RESISTOR) {
		reg = pnp_read_hwm5_index(base, ITE_EC_BEEP_ENABLE);
		reg |= ITE_EC_TEMP_ADJUST_WRITE_ENABLE;
		pnp_write_hwm5_index(base, ITE_EC_BEEP_ENABLE, reg);
		pnp_write_hwm5_index(base, ITE_EC_TEMP_ADJUST[tmpin-1], conf->offset);
	}

	/* Set temperature limits */
	u8 max = conf->max;
	pnp_write_hwm5_index(base, ITE_EC_HIGH_TEMP_LIMIT(tmpin), max ? max : 127);
	pnp_write_hwm5_index(base, ITE_EC_LOW_TEMP_LIMIT(tmpin), conf->min);

	/* Enable the startup of monitoring operation */
	reg = pnp_read_hwm5_index(base, ITE_EC_CONFIGURATION);
	reg |= ITE_EC_CONFIGURATION_START;
	pnp_write_hwm5_index(base, ITE_EC_CONFIGURATION, reg);
}

static void fan_smartconfig(const u16 base, const u8 fan,
			    const enum ite_ec_fan_mode mode,
			    const struct ite_ec_fan_smartconfig *const conf)
{
	u8 pwm_ctrl;
	u8 pwm_start = 0;
	u8 pwm_auto = 0;
	u8 delta_temp;

	if (mode == FAN_SMART_SOFTWARE) {
		pwm_ctrl = ITE_EC_FAN_CTL_PWM_MODE_SOFTWARE;

		/* 50% duty cycle by default */
		const u8 duty = conf->pwm_start ? conf->pwm_start : 50;
		if (CONFIG(SUPERIO_ITE_ENV_CTRL_8BIT_PWM))
			pwm_start = ITE_EC_FAN_CTL_PWM_DUTY(duty);
		else
			pwm_ctrl |= ITE_EC_FAN_CTL_PWM_DUTY(duty);
	} else {
		pwm_ctrl = ITE_EC_FAN_CTL_PWM_MODE_AUTOMATIC;
		pwm_ctrl |= ITE_EC_FAN_CTL_TEMPIN(conf->tmpin);

		if (conf->clsd_loop) {
			pwm_ctrl |= ITE_EC_FAN_PWM_CLSD_LOOP;
			pwm_start = ITE_EC_FAN_CTL_PWM_START_RPM(conf->rpm_start);
			pwm_auto = ITE_EC_FAN_CTL_PWM_SLOPE_LOWER(conf->slope);
		} else {
			pwm_start = ITE_EC_FAN_CTL_PWM_START_DUTY(conf->pwm_start);

			if (CONFIG(SUPERIO_ITE_ENV_CTRL_7BIT_SLOPE_REG)) {
				pwm_auto = conf->slope & 0x7f;
			} else {
				pwm_start |= ITE_EC_FAN_CTL_PWM_SLOPE_BIT6(conf->slope);
				pwm_auto = ITE_EC_FAN_CTL_PWM_SLOPE_LOWER(conf->slope);
			}
		}

		if (conf->smoothing)
			pwm_auto |= ITE_EC_FAN_CTL_AUTO_SMOOTHING_EN;

		pnp_write_hwm5_index(base, ITE_EC_FAN_CTL_TEMP_LIMIT_OFF(fan), conf->tmp_off);
		pnp_write_hwm5_index(base, ITE_EC_FAN_CTL_TEMP_LIMIT_START(fan),
			conf->tmp_start);
		/* Full speed above 127°C by default */
		pnp_write_hwm5_index(base, ITE_EC_FAN_CTL_TEMP_LIMIT_FULL(fan),
			conf->tmp_full ? conf->tmp_full : 127);

		delta_temp = ITE_EC_FAN_CTL_DELTA_TEMP_INTRVL(conf->tmp_delta);
		if (!CONFIG(SUPERIO_ITE_ENV_CTRL_NO_FULLSPEED_SETTING))
			delta_temp |= ITE_EC_FAN_CTL_FULL_AT_THRML_LMT(conf->full_lmt);
		pnp_write_hwm5_index(base, ITE_EC_FAN_CTL_DELTA_TEMP(fan),
			delta_temp);
	}

	pnp_write_hwm5_index(base, ITE_EC_FAN_CTL_PWM_CONTROL(fan), pwm_ctrl);
	pnp_write_hwm5_index(base, ITE_EC_FAN_CTL_PWM_START(fan), pwm_start);
	pnp_write_hwm5_index(base, ITE_EC_FAN_CTL_PWM_AUTO(fan), pwm_auto);
}

static void enable_fan(const u16 base, const u8 fan,
		       const struct ite_ec_fan_config *const conf)
{
	u8 reg;

	if (conf->mode == FAN_IGNORE ||
	    (CONFIG(SUPERIO_ITE_ENV_CTRL_NO_ONOFF) &&
	     conf->mode <= FAN_MODE_OFF))
		return;

	/* FAN_CTL2 might have its own frequency setting */
	if (CONFIG(SUPERIO_ITE_ENV_CTRL_PWM_FREQ2) && fan == 2) {
		reg = pnp_read_hwm5_index(base, ITE_EC_ADC_TEMP_EXTRA_CHANNEL_ENABLE);
		reg &= ~ITE_EC_FAN_PWM_CLOCK_MASK;
		reg |= ITE_EC_FAN_PWM_DEFAULT_CLOCK;
		pnp_write_hwm5_index(base, ITE_EC_ADC_TEMP_EXTRA_CHANNEL_ENABLE, reg);
	}

	if (conf->mode >= FAN_SMART_SOFTWARE) {
		fan_smartconfig(base, fan, conf->mode, &conf->smart);
	} else {
		reg = pnp_read_hwm5_index(base, ITE_EC_FAN_CTL_MODE);
		if (conf->mode == FAN_MODE_ON)
			reg |= ITE_EC_FAN_CTL_ON(fan);
		else
			reg &= ~ITE_EC_FAN_CTL_ON(fan);
		pnp_write_hwm5_index(base, ITE_EC_FAN_CTL_MODE, reg);
	}

	if (CONFIG(SUPERIO_ITE_ENV_CTRL_FAN16_CONFIG) && conf->mode >= FAN_MODE_ON) {
		reg = pnp_read_hwm5_index(base, ITE_EC_FAN_TAC_COUNTER_ENABLE);
		reg |= ITE_EC_FAN_TAC_16BIT_ENABLE(fan);
		pnp_write_hwm5_index(base, ITE_EC_FAN_TAC_COUNTER_ENABLE, reg);
	}

	if (CONFIG(SUPERIO_ITE_ENV_CTRL_5FANS) && fan > 3) {
		reg = pnp_read_hwm5_index(base, ITE_EC_FAN_SEC_CTL);
		if (conf->mode >= FAN_MODE_ON)
			reg |= ITE_EC_FAN_SEC_CTL_TAC_EN(fan);
		else
			reg &= ~ITE_EC_FAN_SEC_CTL_TAC_EN(fan);
		pnp_write_hwm5_index(base, ITE_EC_FAN_SEC_CTL, reg);
	} else {
		reg = pnp_read_hwm5_index(base, ITE_EC_FAN_MAIN_CTL);
		if (conf->mode >= FAN_MODE_ON)
			reg |= ITE_EC_FAN_MAIN_CTL_TAC_EN(fan);
		else
			reg &= ~ITE_EC_FAN_MAIN_CTL_TAC_EN(fan);

		/* Some ITEs have SmartGuardian always enabled */
		if (!CONFIG(SUPERIO_ITE_ENV_CTRL_NO_ONOFF)) {
			if (conf->mode >= FAN_SMART_SOFTWARE)
				reg |= ITE_EC_FAN_MAIN_CTL_SMART(fan);
			else
				reg &= ~ITE_EC_FAN_MAIN_CTL_SMART(fan);
		}
		pnp_write_hwm5_index(base, ITE_EC_FAN_MAIN_CTL, reg);
	}
}

static void enable_fan_vector(const u16 base, const u8 fan_vector,
			      const struct ite_ec_fan_vector_config *const conf)
{
	u8 reg;

	u8 start = conf->tmp_start;
	if (!start) {
		/* When tmp_start is not configured we would set the
		 * register to it's default of 0xFF here, which would
		 * effectively disable the vector functionality of the
		 * SuperIO altogether since that temperature will never
		 * be reached. We can therefore return here and don't
		 * need to set any other registers.
		 */
		return;
	}
	pnp_write_hwm5_index(base, ITE_EC_FAN_VEC_CTL_LIMIT_START(fan_vector), start);

	const s8 slope = conf->slope;
	const bool slope_neg = slope < 0;
	if (slope <= -128)
		reg = 127;
	else if (slope_neg)
		reg = -slope;
	else
		reg = slope;
	reg |= ITE_EC_FAN_VEC_CTL_SLOPE_TMPIN0(conf->tmpin);
	pnp_write_hwm5_index(base, ITE_EC_FAN_VEC_CTL_SLOPE(fan_vector), reg);

	reg = ITE_EC_FAN_VEC_CTL_DELTA_TEMP_INTRVL(conf->tmp_delta);
	reg |= ITE_EC_FAN_VEC_CTL_DELTA_FANOUT(conf->fanout);
	reg |= ITE_EC_FAN_VEC_CTL_DELTA_TMPIN1(conf->tmpin);
	pnp_write_hwm5_index(base, ITE_EC_FAN_VEC_CTL_DELTA(fan_vector), reg);

	if (CONFIG(SUPERIO_ITE_ENV_CTRL_FAN_VECTOR_RANGED)) {
		reg = conf->tmp_range & 0x7f;
		reg |= ITE_EC_FAN_VEC_CTL_RANGE_SLOPESIGN(slope_neg);
		pnp_write_hwm5_index(base, ITE_EC_FAN_VEC_CTL_RANGE(fan_vector), reg);
	} else if (slope_neg) {
		printk(BIOS_WARNING, "Unsupported negative slope on fan vector control\n");
	}
}

static void enable_beeps(const u16 base, const struct ite_ec_config *const conf)
{
	u8 reg = 0;
	u8 freq = ITE_EC_BEEP_TONE_DIVISOR(10) | ITE_EC_BEEP_FREQ_DIVISOR(10);

	if (conf->tmpin_beep) {
		reg |= ITE_EC_BEEP_ON_TMP_LIMIT;
		pnp_write_hwm5_index(base, ITE_EC_BEEP_FREQ_DIV_OF_TMPIN, freq);
	}
	if (conf->fan_beep) {
		reg |= ITE_EC_BEEP_ON_FAN_LIMIT;
		pnp_write_hwm5_index(base, ITE_EC_BEEP_FREQ_DIV_OF_FAN, freq);
	}
	if (conf->vin_beep) {
		reg |= ITE_EC_BEEP_ON_VIN_LIMIT;
		pnp_write_hwm5_index(base, ITE_EC_BEEP_FREQ_DIV_OF_VIN, freq);
	}

	if (reg) {
		reg |= pnp_read_hwm5_index(base, ITE_EC_BEEP_ENABLE);
		pnp_write_hwm5_index(base, ITE_EC_BEEP_ENABLE, reg);
	}
}

void ite_ec_init(const u16 base, const struct ite_ec_config *const conf)
{
	size_t i;

	/* Configure 23.43kHz PWM active high output */
	u8 fan_ctl = pnp_read_hwm5_index(base, ITE_EC_FAN_CTL_MODE);
	fan_ctl &= ~ITE_EC_FAN_PWM_CLOCK_MASK;
	fan_ctl |= ITE_EC_FAN_PWM_DEFAULT_CLOCK;
	fan_ctl |= ITE_EC_FAN_CTL_POLARITY_HIGH;
	pnp_write_hwm5_index(base, ITE_EC_FAN_CTL_MODE, fan_ctl);

	/* Enable HWM if configured */
	for (i = 0; i < ITE_EC_TMPIN_CNT; ++i)
		enable_tmpin(base, i + 1, &conf->tmpin[i]);

	/* Enable External Sensor SMBus Host if configured */
	if (conf->smbus_en) {
		pnp_write_hwm5_index(base, ITE_EC_INTERFACE_SELECT,
			pnp_read_hwm5_index(base, ITE_EC_INTERFACE_SELECT) |
				ITE_EC_INTERFACE_SMB_ENABLE);
	}

	/* Set SST/PECI Host Controller Clock to either 24 MHz or internal 32 MHz */
	if (conf->smbus_24mhz) {
		pnp_write_hwm5_index(base, ITE_EC_INTERFACE_SELECT,
			pnp_read_hwm5_index(base, ITE_EC_INTERFACE_SELECT) |
				ITE_EC_INTERFACE_CLOCK_24MHZ);
	}

	/* Enable reading of voltage pins */
	pnp_write_hwm5_index(base, ITE_EC_ADC_VOLTAGE_CHANNEL_ENABLE, conf->vin_mask);

	/* Enable FANx if configured */
	for (i = 0; i < ITE_EC_FAN_CNT; ++i)
		enable_fan(base, i + 1, &conf->fan[i]);

	if (CONFIG(SUPERIO_ITE_ENV_CTRL_FAN_VECTOR)) {
		/* Enable Special FAN Vector X if configured */
		for (i = 0; i < ITE_EC_FAN_VECTOR_CNT; ++i)
			enable_fan_vector(base, i, &conf->fan_vector[i]);
	}

	/* Enable beeps if configured */
	enable_beeps(base, conf);

	/*
	 * System may get wrong temperature data when SIO is in
	 * busy state. Therefore, check the status and terminate
	 * processes if needed.
	 */
	for (i = 0; i < ITE_EC_TMPIN_CNT; ++i)
		if (conf->tmpin[i].mode == THERMAL_PECI)
			extemp_force_idle_status(base);
}

void ite_ec_set_power_state(struct device *dev)
{
	uint8_t power_status;
	uint8_t reg_pcr1, reg_pcr2;

	/* Set power state after power fail */
	power_status = get_uint_option("power_on_after_fail",
					CONFIG_MAINBOARD_POWER_FAILURE_STATE);
	pnp_enter_conf_mode(dev);
	pnp_set_logical_device(dev);
	reg_pcr1 = pnp_read_config(dev, ITE_EC_REG_PCR1);
	reg_pcr2 = pnp_read_config(dev, ITE_EC_REG_PCR2);
	if (power_status == MAINBOARD_POWER_ON) {
		reg_pcr2 |= (1 << 5);
	} else if (power_status == MAINBOARD_POWER_KEEP) {
		reg_pcr2 &= ~(1 << 5);
		reg_pcr1 |=  (1 << 5);
	} else {
		reg_pcr2 &= ~(1 << 5);
		reg_pcr1 &= ~(1 << 5);
	}
	pnp_write_config(dev, ITE_EC_REG_PCR1, reg_pcr1);
	pnp_write_config(dev, ITE_EC_REG_PCR2, reg_pcr2);
	pnp_exit_conf_mode(dev);
	printk(BIOS_INFO, "set power %u after power fail\n", power_status);
}
