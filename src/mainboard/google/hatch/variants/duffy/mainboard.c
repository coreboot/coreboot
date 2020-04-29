/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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

/*
 * Psys_pmax considerations
 *
 * Given the hardware design in puff, the serial shunt resistor is 0.01ohm.
 * The full scale of hardware PSYS signal 0.8v maps to system current 9.6A
 * instead of real system power. The equation is shown below:
 * PSYS = 0.8v = (0.01ohm x Iinput) x 50 (INA213, gain 50V/V) x 15k/(15k + 75k)
 * Hence, Iinput (Amps) = 9.6A
 * Since there is no voltage information from PSYS, different voltage input
 * would map to different Psys_pmax settings:
 * For Type-C 15V, the Psys_pmax sholud be 15v x 9.6A = 144W
 * For Type-C 20V, the Psys_pmax should be 20v x 9.6A = 192W
 * For a barral jack, the Psys_pmax should be 19v x 9.6A = 182.4W
 */
#define PSYS_IMAX	9600
#define BJ_VOLTS_MV	19000

static void mainboard_set_power_limits(config_t *conf)
{
	enum usb_chg_type type;
	u32 watts;
	u16 volts_mv, current_ma;
	u32 psyspl2 = PUFF_PSYSPL2; // default barrel jack value for U22
	int rv = google_chromeec_get_usb_pd_power_info(&type, &current_ma, &volts_mv);

	/* use SoC default value for PsysPL3 and PL4 unless we're on USB-PD*/
	conf->tdp_psyspl3 = 0;
	conf->tdp_pl4 = 0;

	if (rv == 0 && type == USB_CHG_TYPE_PD) {
		/* Detected USB-PD.  Base on max value of adapter */
		watts = ((u32)current_ma * volts_mv) / 1000000;
		psyspl2 = watts;
		conf->tdp_psyspl3 = SET_PSYSPL2(psyspl2);
		/* set max possible time window */
		conf->tdp_psyspl3_time = PUFF_MAX_TIME_WINDOW;
		/* set minimum duty cycle */
		conf->tdp_psyspl3_dutycycle = PUFF_MIN_DUTYCYCLE;
		conf->tdp_pl4 = SET_PSYSPL2(psyspl2);
	} else {
		/* Input type is barrel jack */
		volts_mv = BJ_VOLTS_MV;
	}
	/* voltage unit is milliVolts and current is in milliAmps */
	conf->psys_pmax = (u16)(((u32)PSYS_IMAX * volts_mv) / 1000000);

	conf->tdp_pl2_override = PUFF_PL2;
	/* set psyspl2 to 90% of max adapter power */
	conf->tdp_psyspl2 = SET_PSYSPL2(psyspl2);
}

void variant_ramstage_init(void)
{
	static const long display_timeout_ms = 3000;
	config_t *conf = config_of_soc();

	/* This is reconfigured back to whatever FSP-S expects by gpio_configure_pads. */
	gpio_input(GPIO_HDMI_HPD);
	gpio_input(GPIO_DP_HPD);
	if (display_init_required()
		&& !gpio_get(GPIO_HDMI_HPD)
		&& !gpio_get(GPIO_DP_HPD)) {
		/* This has to be done before FSP-S runs. */
		if (google_chromeec_wait_for_displayport(display_timeout_ms))
			wait_for_hpd(GPIO_DP_HPD, display_timeout_ms);
	}
	/* Psys_pmax needs to be setup before FSP-S */
	mainboard_set_power_limits(conf);
}
