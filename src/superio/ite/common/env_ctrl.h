/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2016 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SUPERIO_ITE_ENV_CTRL_H
#define SUPERIO_ITE_ENV_CTRL_H

#include "env_ctrl_chip.h"

#if IS_ENABLED(CONFIG_SUPERIO_ITE_ENV_CTRL_8BIT_PWM)
#define ITE_EC_FAN_MAX_PWM			0xff
#define ITE_EC_FAN_PWM_DEFAULT_CLOCK		ITE_EC_FAN_PWM_CLOCK_6MHZ
#else
#define ITE_EC_FAN_MAX_PWM			0x7f
#define ITE_EC_FAN_PWM_DEFAULT_CLOCK		ITE_EC_FAN_PWM_CLOCK_3MHZ
#endif

#define ITE_EC_CONFIGURATION			0x00
#define   ITE_EC_CONFIGURATION_START		(1 << 0)

#define ITE_EC_INTERFACE_SELECT			0x0a
#define   ITE_EC_INTERFACE_PSEUDO_EOC		(1 << 7)
#define   ITE_EC_INTERFACE_SMB_ENABLE		(1 << 6)
#define   ITE_EC_INTERFACE_SEL_DISABLE		(0 << 4)
#define   ITE_EC_INTERFACE_SEL_SST_SLAVE	(1 << 4)
#define   ITE_EC_INTERFACE_SEL_PECI		(2 << 4)
#define   ITE_EC_INTERFACE_SEL_SST_HOST		(3 << 4)
#define   ITE_EC_INTERFACE_CLOCK_32MHZ		(0 << 3)
#define   ITE_EC_INTERFACE_CLOCK_24MHZ		(1 << 3)
#define   ITE_EC_INTERFACE_SPEED_TOLERANCE	(1 << 2)
#define   ITE_EC_INTERFACE_PECI_AWFCS		(1 << 0)

#define ITE_EC_FAN_PWM_SMOOTHING_FREQ		0x0b
#define   ITE_EC_FAN_PWM_SMOOTHING_MASK		(3 << 6)
#define   ITE_EC_FAN_PWM_SMOOTHING_1KHZ		(0 << 6)
#define   ITE_EC_FAN_PWM_SMOOTHING_256HZ	(1 << 6)
#define   ITE_EC_FAN_PWM_SMOOTHING_64HZ		(2 << 6)
#define   ITE_EC_FAN_PWM_SMOOTHING_16HZ		(3 << 6)

#define ITE_EC_FAN_TAC_COUNTER_ENABLE		0x0c
#define   ITE_EC_FAN_TAC_16BIT_ENABLE(x)	(1 << ((x)-1))
#define ITE_EC_FAN_TAC_LIMIT(x)			(0x10 + ((x)-1))
#define ITE_EC_FAN_TAC_EXT_LIMIT(x)		(0x1b + ((x)-1))

#define ITE_EC_FAN_MAIN_CTL			0x13
#define   ITE_EC_FAN_MAIN_CTL_TAC_EN(x)		(1 << ((x)+3))
#define   ITE_EC_FAN_MAIN_CTL_COLL_FULL_SPEED	(1 << 3)
#define   ITE_EC_FAN_MAIN_CTL_SMART(x)		(1 << ((x)-1))
#define ITE_EC_FAN_CTL_MODE			0x14
#define   ITE_EC_FAN_CTL_POLARITY_HIGH		(1 << 7)
#define   ITE_EC_FAN_PWM_CLOCK_MASK		(7 << 4)
#define   ITE_EC_FAN_PWM_CLOCK_48MHZ		(0 << 4)
#define   ITE_EC_FAN_PWM_CLOCK_24MHZ		(1 << 4)
#define   ITE_EC_FAN_PWM_CLOCK_12MHZ		(2 << 4)
#define   ITE_EC_FAN_PWM_CLOCK_8MHZ		(3 << 4)
#define   ITE_EC_FAN_PWM_CLOCK_6MHZ		(4 << 4)
#define   ITE_EC_FAN_PWM_CLOCK_3MHZ		(5 << 4)
#define   ITE_EC_FAN_PWM_CLOCK_1_5MHZ		(6 << 4)
#define   ITE_EC_FAN_PWM_CLOCK_51KHZ		(7 << 4)
#define   ITE_EC_FAN_PWM_MIN_DUTY_20		(1 << 3)
#define   ITE_EC_FAN_CTL_ON(x)			(1 << ((x)-1))
#define ITE_EC_FAN_CTL_PWM_CONTROL(x)		(0x15 + ((x)-1))
#define   ITE_EC_FAN_CTL_PWM_MODE_SOFTWARE	(0 << 7)
#define   ITE_EC_FAN_CTL_PWM_MODE_AUTOMATIC	(1 << 7)
#define   ITE_EC_FAN_CTL_PWM_DUTY_MASK		(ITE_EC_FAN_MAX_PWM << 0)
#define   ITE_EC_FAN_CTL_PWM_DUTY(p)		\
	  ({					\
		const unsigned int _p = p;			\
		(_p >= 100)					\
			? ITE_EC_FAN_MAX_PWM			\
			: (_p * ITE_EC_FAN_MAX_PWM) / 100;	\
	  })
#define   ITE_EC_FAN_CTL_TEMPIN_MASK		(3 << 0)
#define   ITE_EC_FAN_CTL_TEMPIN(x)		(((x)-1) & 3)

#define ITE_EC_HIGH_TEMP_LIMIT(x)		(0x40 + ((x-1) * 2))
#define ITE_EC_LOW_TEMP_LIMIT(x)		(0x41 + ((x-1) * 2))

#define ITE_EC_ADC_VOLTAGE_CHANNEL_ENABLE	0x50
#define ITE_EC_ADC_TEMP_CHANNEL_ENABLE		0x51
#define   ITE_EC_ADC_TEMP_EXT_REPORTS_TO_MASK	(3 << 6)
#define   ITE_EC_ADC_TEMP_EXT_REPORTS_TO(x)	(((x) & 3) << 6)
#define   ITE_EC_ADC_TEMP_RESISTOR_MODE(x)	(1 << ((x)+2))
#define   ITE_EC_ADC_TEMP_DIODE_MODE(x)		(1 << ((x)-1))
#define ITE_EC_ADC_TEMP_EXTRA_CHANNEL_ENABLE	0x55

/* Matches length of ITE_EC_TMPIN_CNT */
static const u8 ITE_EC_TEMP_ADJUST[] = { 0x56, 0x57, 0x59 };

#define ITE_EC_BEEP_ENABLE			0x5C
#define   ITE_EC_TEMP_ADJUST_WRITE_ENABLE	(1 << 7)
#define   ITE_EC_ADC_CLOCK_1MHZ			(6 << 4)
#define   ITE_EC_ADC_CLOCK_2MHZ			(7 << 4)
#define   ITE_EC_ADC_CLOCK_24MHZ		(5 << 4)
#define   ITE_EC_ADC_CLOCK_31KHZ		(4 << 4)
#define   ITE_EC_ADC_CLOCK_62KHZ		(3 << 4)
#define   ITE_EC_ADC_CLOCK_125KHZ		(2 << 4)
#define   ITE_EC_ADC_CLOCK_250KHZ		(1 << 4)
#define   ITE_EC_ADC_CLOCK_500KHZ		(0 << 4)
#define   ITE_EC_BEEP_ON_TMP_LIMIT		(1 << 2)
#define   ITE_EC_BEEP_ON_VIN_LIMIT		(1 << 1)
#define   ITE_EC_BEEP_ON_FAN_LIMIT		(1 << 0)
#define ITE_EC_BEEP_FREQ_DIV_OF_FAN		0x5D
#define ITE_EC_BEEP_FREQ_DIV_OF_VIN		0x5E
#define ITE_EC_BEEP_FREQ_DIV_OF_TMPIN		0x5F
#define   ITE_EC_BEEP_TONE_DIVISOR(x)		(((x) & 0x0f) << 4)
#define   ITE_EC_BEEP_FREQ_DIVISOR(x)		(((x) & 0x0f) << 0)

#define ITE_EC_FAN_CTL_TEMP_LIMIT_OFF(x)	(0x60 + ((x)-1) * 8)
#define ITE_EC_FAN_CTL_TEMP_LIMIT_START(x)	(0x61 + ((x)-1) * 8)
#define ITE_EC_FAN_CTL_TEMP_LIMIT_FULL(x)	(0x62 + ((x)-1) * 8)
#define ITE_EC_FAN_CTL_PWM_START(x)		(0x63 + ((x)-1) * 8)
#define   ITE_EC_FAN_CTL_PWM_SLOPE_BIT6(s)	(((s) & 0x40) << 1)
#define   ITE_EC_FAN_CTL_PWM_START_DUTY(p)	ITE_EC_FAN_CTL_PWM_DUTY(p)
#define ITE_EC_FAN_CTL_PWM_AUTO(x)		(0x64 + ((x)-1) * 8)
#define   ITE_EC_FAN_CTL_AUTO_SMOOTHING_EN	(1 << 7)
#define   ITE_EC_FAN_CTL_PWM_SLOPE_LOWER(s)	((s) & 0x3f)
#define ITE_EC_FAN_CTL_DELTA_TEMP(x)		(0x65 + ((x)-1) * 8)
#define   ITE_EC_FAN_CTL_DELTA_TEMP_INTRVL(c)	((c) & 0x1f)

#define ITE_EC_EXTEMP_STATUS			0x88
#define   ITE_EC_EXTEMP_STATUS_HOST_BUSY	(1 << 0)
#define ITE_EC_EXTEMP_ADDRESS			0x89
#define ITE_EC_EXTEMP_WRITE_LENGTH		0x8a
#define ITE_EC_EXTEMP_READ_LENGTH		0x8b
#define ITE_EC_EXTEMP_COMMAND			0x8c
#define ITE_EC_EXTEMP_WRITE_DATA_2		0x8d
#define ITE_EC_EXTEMP_CONTROL			0x8e
#define   ITE_EC_EXTEMP_CTRL_AUTO_32HZ		(0 << 6)
#define   ITE_EC_EXTEMP_CTRL_AUTO_16HZ		(1 << 6)
#define   ITE_EC_EXTEMP_CTRL_AUTO_8HZ		(2 << 6)
#define   ITE_EC_EXTEMP_CTRL_AUTO_4HZ		(3 << 6)
#define   ITE_EC_EXTEMP_CTRL_AUTO_START		(1 << 5)
#define   ITE_EC_EXTEMP_CTRL_AUTO_ABORT		(1 << 4)
#define   ITE_EC_EXTEMP_CTRL_AUTO_TWO_DOMAIN	(1 << 3)
#define   ITE_EC_EXTEMP_CTRL_CONTENTION		(1 << 2)
#define   ITE_EC_EXTEMP_CTRL_SST_IDLE_HIGH	(1 << 1)
#define   ITE_EC_EXTEMP_CTRL_START		(1 << 0)

/* Standard PECI GetTemp */
#define PECI_CLIENT_ADDRESS			0x30
#define PECI_GETTEMP_COMMAND			0x01
#define PECI_GETTEMP_WRITE_LENGTH		0x01
#define PECI_GETTEMP_READ_LENGTH		0x02

void ite_ec_init(u16 base, const struct ite_ec_config *conf);

#endif /* SUPERIO_ITE_ENV_CTRL_H */
