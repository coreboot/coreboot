/*
 * This file is part of the coreboot project.
 *
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

#include <baseboard/variants.h>
#include <chip.h>
#include <delay.h>
#include <device/device.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <timer.h>

#define GPIO_HDMI_HPD		GPP_E13
#define GPIO_DP_HPD		GPP_E14

/* TODO: This can be moved to common directory */
static void wait_for_hpd(gpio_t gpio, long timeout)
{
	struct stopwatch sw;

	printk(BIOS_INFO, "Waiting for HPD\n");
	stopwatch_init_msecs_expire(&sw, timeout);
	while (!gpio_get(gpio)) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_WARNING,
			       "HPD not ready after %ldms. Abort.\n", timeout);
			return;
		}
		mdelay(200);
	}
	printk(BIOS_INFO, "HPD ready after %lu ms\n",
	       stopwatch_duration_msecs(&sw));
}

void variant_ramstage_init(void)
{
	static const long display_timeout_ms = 3000;

	/* This is reconfigured back to whatever FSP-S expects by
	   gpio_configure_pads. */
	gpio_input(GPIO_HDMI_HPD);
	gpio_input(GPIO_DP_HPD);
	if (display_init_required()
		&& !gpio_get(GPIO_HDMI_HPD)
		&& !gpio_get(GPIO_DP_HPD)) {
		/* This has to be done before FSP-S runs. */
		if (google_chromeec_wait_for_displayport(display_timeout_ms))
			wait_for_hpd(GPIO_DP_HPD, display_timeout_ms);
	}
}

/*
 * For type-C chargers, set PL2 to 90% of max power to account for
 * cable loss and FET Rdson loss in the path from the source.
 */
#define SET_PSYSPL2(w)     (9 * (w) / 10)

#define PUFF_PL2   (35)

#define PUFF_PSYSPL2 (58)

#define PUFF_MAX_TIME_WINDOW 6
#define PUFF_MIN_DUTYCYCLE   4

/*
 * mainboard_set_power_limits
 *
 * Set Pl2 and SysPl2 values based on detected charger.
 * Values are defined below but we use U22 value for all SKUs for now.
 * definitions:
 * x = no value entered. Use default value in parenthesis.
 *     will set 0 to anything that shouldn't be set.
 * n = max value of power adapter.
 * +-------------+-----+---------+-----------+-------+
 * | sku_id      | PL2 | PsysPL2 |  PsysPL3  |  PL4  |
 * +-------------+-----+---------+-----------+-------+
 * | i7 U42      |  51 |   81    | x(.85PL4) | x(82) |
 * | celeron U22 |  35 |   58    | x(.85PL4) | x(51) |
 * +-------------+-----+---------+-----------+-------+
 * For USB C charger:
 * +-------------+-----+---------+---------+-------+
 * | Max Power(W)| PL2 | PsysPL2 | PsysPL3 |  PL4  |
 * +-------------+-----+---------+---------+-------+
 * | 60 (U42)    |  44 |   54    |    54   |   54  |
 * | 60 (U22)    |  29 |   54    |    54   | x(43) |
 * | n  (U42)    |  44 |   .9n   |   .9n   |  .9n  |
 * | n  (U22)    |  29 |   .9n   |   .9n   | x(43) |
 * +-------------+-----+---------+---------+-------+
 */
static void mainboard_set_power_limits(config_t *conf)
{
	enum usb_chg_type type;
	u32 watts;
	u32 psyspl2 = PUFF_PSYSPL2; // default barrel jack value for U22
	int rv = google_chromeec_get_usb_pd_power_info(&type, &watts);

	/* use SoC default value for PsysPL3 and PL4 unless we're on USB-PD*/
	conf->tdp_psyspl3 = 0;
	conf->tdp_pl4 = 0;

	if (rv == 0 && type == USB_CHG_TYPE_PD) {
		/* Detected USB-PD.  Base on max value of adapter */
		psyspl2 = watts;
		conf->tdp_psyspl3 = SET_PSYSPL2(psyspl2);
		/* set max possible time window */
		conf->tdp_psyspl3_time = PUFF_MAX_TIME_WINDOW;
		/* set minimum duty cycle */
		conf->tdp_psyspl3_dutycycle = PUFF_MIN_DUTYCYCLE;
		conf->tdp_pl4 = SET_PSYSPL2(psyspl2);
	}

	conf->tdp_pl2_override = PUFF_PL2;
	/* set psyspl2 to 90% of max adapter power */
	conf->tdp_psyspl2 = SET_PSYSPL2(psyspl2);
}

void variant_mainboard_enable(struct device *dev)
{
	config_t *conf = config_of_soc();
	mainboard_set_power_limits(conf);
}
