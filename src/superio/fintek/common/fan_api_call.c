/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include "fan_control.h"

static int check_status(int status)
{
	if (status < HWM_STATUS_SUCCESS)
		return status;
	return HWM_STATUS_SUCCESS;	/* positive values are warnings only */
}

int set_fan(struct fintek_fan *fan_init)
{
	int s;

	s = set_sensor_type(CONFIG_HWM_PORT, fan_init->sensor, fan_init->stype);
	if (check_status(s) != HWM_STATUS_SUCCESS)
		return s;

	s = set_fan_temperature_source(CONFIG_HWM_PORT, fan_init->fan, fan_init->temp_source);
	if (check_status(s) != HWM_STATUS_SUCCESS)
		return s;

	s = set_fan_type_mode(CONFIG_HWM_PORT, fan_init->fan, fan_init->ftype, fan_init->fmode);
	if (check_status(s) != HWM_STATUS_SUCCESS)
		return s;

	s = set_pwm_frequency(CONFIG_HWM_PORT, fan_init->fan, fan_init->fan_freq);
	if (check_status(s) != HWM_STATUS_SUCCESS)
		return s;

	s = set_fan_speed_change_rate(CONFIG_HWM_PORT, fan_init->fan, fan_init->rate_up,
				fan_init->rate_down);
	if (check_status(s) != HWM_STATUS_SUCCESS)
		return s;

	s = set_fan_follow(CONFIG_HWM_PORT, fan_init->fan, fan_init->follow);
	if (check_status(s) != HWM_STATUS_SUCCESS)
		return s;

	s = set_sections(CONFIG_HWM_PORT, fan_init->fan, fan_init->boundaries,
				fan_init->sections);
	if (check_status(s) != HWM_STATUS_SUCCESS)
		return s;

	printk(BIOS_DEBUG, "Fan %d completed\n", fan_init->fan);
	return HWM_STATUS_SUCCESS;
}
