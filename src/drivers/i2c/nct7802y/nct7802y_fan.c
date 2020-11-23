/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <device/device.h>

#include "nct7802y.h"
#include "chip.h"

static void init_fan(struct device *const dev,
		     const struct nct7802y_fan_config *const config,
		     const unsigned int fan)
{
	unsigned int temp;
	unsigned int i;

	nct7802y_update(dev, FAN_ENABLE, 0, FANx_ENABLE(fan));

	/* By default, do not map any temperature control to the fan. */
	for (temp = 0; temp < NCT7802Y_FAN_CNT; ++temp) {
		nct7802y_update(dev, TEMP_TO_FAN_MAP(temp),
				TEMPx_TO_FANy_MAP(temp, fan), 0);
	}

	if (config->mode == FAN_MANUAL) {
		nct7802y_write(dev, FAN_CTRL(fan), config->duty_cycle);
	} else {
		u8 set = 0, div, mul;
		if (config->smart.mode == SMART_FAN_RPM) {
			set |= CLOSE_LOOP_FANx_EN(fan);
			div = 50;
			if (config->smart.speed == FAN_SPPED_HIGHSPEED) {
				set |= CLOSE_LOOP_FANx_HIGH_RPM(fan);
				div = 100;
			}
			mul = 1;
		} else {
			/* SMART_FAN_DUTY is given in %, 100% == 255. */
			div = 100;
			mul = 255;
		}
		nct7802y_update(dev, CLOSE_LOOP_FAN_RPM_CTRL,
				CLOSE_LOOP_FANx_EN(fan) |
					CLOSE_LOOP_FANx_HIGH_RPM(fan),
				set);

		/* Map TEMPx to FANx to make things simple */
		nct7802y_update(dev, TEMP_TO_FAN_MAP(fan),
				TEMPx_TO_FAN_MAP_MASK(fan),
				TEMPx_TO_FANy_MAP(fan, fan));

		nct7802y_update(dev, FAN_CTRL_TEMP_SRC(fan),
				FAN_CTRL_TEMPx_SRC_MASK(fan),
				FAN_CTRL_TEMPx_SRCy(
					fan, config->smart.tempsrc));

		for (i = 0; i < ARRAY_SIZE(config->smart.table); ++i) {
			nct7802y_write(dev, TABLEx_TEMP_POINTy(fan, i),
				       config->smart.table[i].temp);
			nct7802y_write(dev, TABLEx_TARGET_POINTy(fan, i),
				(config->smart.table[i].target * mul) / div);
		}
		nct7802y_write(dev, TABLEx_TEMP_POINTy(fan, 4),
			       config->smart.critical_temp);
	}
}

void nct7802y_init_fan(struct device *const dev)
{
	const struct drivers_i2c_nct7802y_config *const config = dev->chip_info;
	unsigned int i;
	u8 value;

	if (nct7802y_select_bank(dev, 0) != CB_SUCCESS)
		return;

	for (i = 0; i < NCT7802Y_FAN_CNT; ++i) {
		if (config->fan[i].mode != FAN_IGNORE)
			init_fan(dev, &config->fan[i], i);
	}

	switch (config->on_pecierror) {
	case PECI_ERROR_KEEP:
		value = CLOSE_LOOP_FAN_PECI_ERR_CURR;
		break;
	case PECI_ERROR_VALUE:
		value = CLOSE_LOOP_FAN_PECI_ERR_VALUE;
		break;
	case PECI_ERROR_FULLSPEED:
		value = CLOSE_LOOP_FAN_PECI_ERR_MAX;
		break;
	default:
		value = 0;
		break;
	}
	nct7802y_update(dev, CLOSE_LOOP_FAN_RPM_CTRL, CLOSE_LOOP_FAN_PECI_ERR_MASK, value);
	nct7802y_write(dev, FAN_DUTY_ON_PECI_ERROR, config->pecierror_minduty);
}
