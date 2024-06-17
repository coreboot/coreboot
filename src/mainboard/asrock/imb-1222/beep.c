/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <pc80/i8254.h>
#include <gpio.h>
#include <mainboard/beep.h>

#define LED_GPIO_PAD	GPP_E8
#define SPKR_GPIO_PAD	GPP_B14

static const struct pad_config spkr_led_cfg_tbl[] = {
	PAD_CFG_GPO(LED_GPIO_PAD, 0, PLTRST),			/* GPIO */
	PAD_CFG_NF(SPKR_GPIO_PAD, DN_20K, PLTRST, NF1),		/* SPKR */
};

static const struct pad_config sata_led_cfg_tbl[] = {
	PAD_CFG_NF(LED_GPIO_PAD, UP_5K, PLTRST, NF1),		/* SATALED# */
};

static void config_gpio_spkr_led(void)
{
	gpio_configure_pads(spkr_led_cfg_tbl, ARRAY_SIZE(spkr_led_cfg_tbl));
}

static void config_gpio_sata_led(void)
{
	gpio_configure_pads(sata_led_cfg_tbl, ARRAY_SIZE(sata_led_cfg_tbl));
}

static void blink_sata_led(unsigned int on)
{
	gpio_set(LED_GPIO_PAD, on ^ 1);
}

void mainboard_beep_and_blink_on_panic_once(void)
{
	const unsigned int duration_msec = 300;
	static bool once = false;

	if (!once) {
		config_gpio_spkr_led();
		once = true;
	}

	blink_sata_led(1);
	if (CONFIG(BEEP_ON_PANIC))
		beep(800, duration_msec);
	else
		mdelay(duration_msec);
	blink_sata_led(0);
}

void mainboard_beep_and_blink_on_boot_once(void)
{
	const unsigned int duration_msec = 500;
	if (CONFIG(BEEP_ON_BOOT)) {
		config_gpio_spkr_led();
		blink_sata_led(1);
		beep(600, duration_msec);
		blink_sata_led(0);
		config_gpio_sata_led();
	}
}
