/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <ec/acpi/ec.h>
#include <pc80/mc146818rtc.h>

#include "ec.h"
#include "chip.h"

typedef struct ec_kontron_it8516e_config config_t;

enum { /* EC commands */
	IT8516E_CMD_SET_SYSTEMP_TYPE	= 0x06,
	IT8516E_CMD_GET_SYSTEMP_TYPE	= 0x07,
	IT8516E_CMD_GET_FAN_MODE	= 0x10,
	IT8516E_CMD_SET_FAN_MODE	= 0x11,
	IT8516E_CMD_GET_FAN_PWM		= 0x12,
	IT8516E_CMD_SET_FAN_PWM		= 0x13,
	IT8516E_CMD_GET_FAN_SPEED	= 0x14,
	IT8516E_CMD_SET_FAN_SPEED	= 0x15,
	IT8516E_CMD_GET_FAN_TEMP	= 0x16,
	IT8516E_CMD_SET_FAN_TEMP	= 0x17,
	IT8516E_CMD_SET_FAN_LIMITS	= 0x1a,
};

/**
 * Sets the type of the external temperature sensor used
 *
 * @param type Type of sensor to set
 */
static void it8516e_set_systemp_type(const u8 type)
{
	if (send_ec_command(IT8516E_CMD_SET_SYSTEMP_TYPE))
		return;
	send_ec_data(type);
}

/**
 * Sets the operating mode of a fan
 *
 * @param idx Selects the fan; 0: CPU, 1: System
 * @param mode Mode to set
 */
static void it8516e_set_fan_mode(const u8 idx, const u8 mode)
{
	if (send_ec_command(IT8516E_CMD_SET_FAN_MODE))
		return;
	if (send_ec_data(idx))
		return;
	send_ec_data(mode);
}

/**
 * Sets the PWM rate of a fan in IT8516E_MODE_PWM
 *
 * @param idx Selects the fan; 0: CPU, 1: System
 * @param pwm PWM rate measured in 255ths
 */
static void it8516e_set_fan_pwm(const u8 idx, const u8 pwm)
{
	if (send_ec_command(IT8516E_CMD_SET_FAN_PWM))
		return;
	if (send_ec_data(idx))
		return;
	send_ec_data(pwm);
}

/**
 * Sets the target speed in RPM for a fan in IT8516E_MODE_SPEED
 *
 * @param idx Selects the fan; 0: CPU, 1: System
 * @param speed Speed in RPM
 */
static void it8516e_set_fan_speed(const u8 idx, const u16 speed)
{
	if (send_ec_command(IT8516E_CMD_SET_FAN_SPEED))
		return;
	if (send_ec_data(idx))
		return;
	if (send_ec_data(speed & 0xff))
		return;
	send_ec_data(speed >> 8);
}

/**
 * Sets the target temperature for a fan in IT8516E_MODE_THERMAL
 *
 * @param idx Selects the fan; 0: CPU, 1: System
 * @param temp Temperature in 64ths degree C
 */
static void it8516e_set_fan_temperature(const u8 idx, const u16 temp)
{
	if (send_ec_command(IT8516E_CMD_SET_FAN_TEMP))
		return;
	if (send_ec_data(idx))
		return;
	if (send_ec_data(temp & 0xff))
		return;
	send_ec_data(temp >> 8);
}

/**
 * Sets the minimum and maximum PWM rate of a fan in IT8516E_MODE_THERMAL
 *
 * @param idx Selects the fan; 0: CPU, 1: System
 * @param min Minimum PWM rate in %
 * @param max Maximum PWM rate in %
 */
static void it8516e_set_fan_limits(const u8 idx, const u8 min, const u8 max)
{
	if (send_ec_command(IT8516E_CMD_SET_FAN_LIMITS))
		return;
	if (send_ec_data(idx))
		return;
	if (send_ec_data(min))
		return;
	send_ec_data(max);
}

static void it8516e_set_fan_from_options(const config_t *const config,
					 const u8 fan_idx)
{
	static char fanX_mode[]		= "fanX_mode";
	static char fanX_target[]	= "fanX_target";
	static char fanX_min[]		= "fanX_min";
	static char fanX_max[]		= "fanX_max";

	u8 fan_mode	= config->default_fan_mode[fan_idx];
	u16 fan_target	= config->default_fan_target[fan_idx];
	u8 fan_min	= config->default_fan_min[fan_idx];
	u8 fan_max	= config->default_fan_max[fan_idx];

	fanX_mode[3] = '1' + fan_idx;
	get_option(&fan_mode, fanX_mode);
	if (!fan_mode)
		fan_mode = IT8516E_MODE_AUTO;
	it8516e_set_fan_mode(fan_idx, fan_mode);

	fanX_target[3] = '1' + fan_idx;
	get_option(&fan_target, fanX_target);
	switch (fan_mode) {
	case IT8516E_MODE_AUTO:
		printk(BIOS_DEBUG,
		       "Setting it8516e fan%d "
		       "control to auto.\n",
		       fan_idx + 1);
		break;
	case IT8516E_MODE_PWM:
		printk(BIOS_DEBUG,
		       "Setting it8516e fan%d "
		       "control to %d%% PWM.\n",
		       fan_idx + 1, fan_target);
		if (fan_target > 100)		/* Constrain to 100% */
			fan_target = 100;
		it8516e_set_fan_pwm(fan_idx, (fan_target * 255) / 100);
		break;
	case IT8516E_MODE_SPEED:
		printk(BIOS_DEBUG,
		       "Setting it8516e fan%d "
		       "control to %d RPMs.\n",
		       fan_idx + 1, fan_target);
		it8516e_set_fan_speed(fan_idx, fan_target);
		break;
	case IT8516E_MODE_THERMAL:
		printk(BIOS_DEBUG,
		       "Setting it8516e fan%d control to %d C.\n",
		       fan_idx + 1, fan_target);
		if (fan_target > 1024)		/* Constrain to 1K */
			fan_target = 1024;
		it8516e_set_fan_temperature(fan_idx, fan_target * 64);

		fanX_min[3] = '1' + fan_idx;
		fanX_max[3] = '1' + fan_idx;
		get_option(&fan_min, fanX_min);
		get_option(&fan_max, fanX_max);

		if (!fan_max || fan_max > 100)	/* Constrain fan_max to 100% */
			fan_max = 100;
		if (fan_min >= 100)		/* Constrain fan_min to  99% */
			fan_min = 99;
		if (fan_max <= fan_min)	/* If fan_min is the higher of the two,
					   it's safer for the hardware to keep
					   its value. Therefore, update fan_max. */
			fan_max = fan_min + 1;

		printk(BIOS_DEBUG,
		       "Setting it8516e fan%d limits to %d%% - %d%% PWM.\n",
		       fan_idx + 1, fan_min, fan_max);
		it8516e_set_fan_limits(fan_idx, fan_min, fan_max);
		break;
	}
}

static void it8516e_pm2_init(const device_t dev)
{
	const config_t *const config = dev->chip_info;

	/* TODO: Set frequency / divider? */

	ec_set_ports(find_resource(dev, PNP_IDX_IO1)->base,
		     find_resource(dev, PNP_IDX_IO0)->base);

	u8 systemp_type = config->default_systemp;
	get_option(&systemp_type, "systemp_type");
	if (systemp_type >= IT8516E_SYSTEMP_LASTPLUSONE)
		systemp_type = IT8516E_SYSTEMP_NONE;
	it8516e_set_systemp_type(systemp_type);

	it8516e_set_fan_from_options(config, 0);
	it8516e_set_fan_from_options(config, 1);
}

static struct device_operations it8516e_pm2_ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
	.init             = it8516e_pm2_init
};

static struct pnp_info it8516e_dev_infos[] = {
	{ NULL,             IT8516E_LDN_UART1, PNP_IO0 | PNP_IRQ0, { 0x07f8, }, },
	{ NULL,             IT8516E_LDN_UART2, PNP_IO0 | PNP_IRQ0, { 0x07f8, }, },
	{ NULL,             IT8516E_LDN_SWUC,  PNP_IO0 | PNP_IRQ0, { 0xff7e0, }, },
	{ NULL,             IT8516E_LDN_MOUSE, PNP_IRQ0, },
	{ NULL,             IT8516E_LDN_KBD,   PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x07ff, }, { 0x07ff, }, },
	{ NULL,             IT8516E_LDN_SMFI,  PNP_IO0 | PNP_IRQ0, { 0xfff0, }, },
	{ NULL,             IT8516E_LDN_BRAM,  PNP_IO0 | PNP_IO1, { 0xfffe, }, { 0xfffe, }, },
	{ NULL,             IT8516E_LDN_PM1,   PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x07ff, }, { 0x07ff, }, },
	{ &it8516e_pm2_ops, IT8516E_LDN_PM2,   PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x07ff, }, { 0x07ff, }, },
	{ NULL,             IT8516E_LDN_PM3,   PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x07ff, }, { 0x07ff, }, },
};

static void it8516e_enable(const device_t dev)
{
	pnp_enable_devices(dev, &pnp_ops,
			   ARRAY_SIZE(it8516e_dev_infos), it8516e_dev_infos);
}

const struct chip_operations ec_kontron_it8516e_ops = {
	CHIP_NAME("Kontron (Fintec/ITE) IT8516E EC")
	.enable_dev = it8516e_enable
};
