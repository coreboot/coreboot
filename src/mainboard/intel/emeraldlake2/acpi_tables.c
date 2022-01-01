/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <device/device.h>
#include <soc/nvs.h>

#include "thermal.h"

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	/* Enable USB ports in S3 */
	gnvs->s3u0 = 1;
	gnvs->s3u1 = 1;

	/*
	 * Enable Front USB ports in S5 by default
	 * to be consistent with back port behavior
	 */
	gnvs->s5u0 = 1;
	gnvs->s5u1 = 1;

	gnvs->f4of = FAN4_THRESHOLD_OFF;
	gnvs->f4on = FAN4_THRESHOLD_ON;
	gnvs->f4pw = FAN4_PWM;

	gnvs->f3of = FAN3_THRESHOLD_OFF;
	gnvs->f3on = FAN3_THRESHOLD_ON;
	gnvs->f3pw = FAN3_PWM;

	gnvs->f2of = FAN2_THRESHOLD_OFF;
	gnvs->f2on = FAN2_THRESHOLD_ON;
	gnvs->f2pw = FAN2_PWM;

	gnvs->f1of = FAN1_THRESHOLD_OFF;
	gnvs->f1on = FAN1_THRESHOLD_ON;
	gnvs->f1pw = FAN1_PWM;

	gnvs->f0of = FAN0_THRESHOLD_OFF;
	gnvs->f0on = FAN0_THRESHOLD_ON;
	gnvs->f0pw = FAN0_PWM;

	gnvs->tcrt = CRITICAL_TEMPERATURE;
	gnvs->tpsv = PASSIVE_TEMPERATURE;
	gnvs->tmax = MAX_TEMPERATURE;
}
