/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
 * Foundation, Inc.
 */

#include <boardid.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <delay.h>
#include <drivers/i2c/ww_ring/ww_ring.h>
#include <gpio.h>
#include <soc/cdp.h>
#include <soc/gsbi.h>
#include <string.h>
#include <timer.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define DEV_SW 15
#define DEV_POL ACTIVE_LOW
#define REC_SW 16
#define REC_POL ACTIVE_LOW
#define WP_SW  17
#define WP_POL ACTIVE_LOW

static int read_gpio(gpio_t gpio_num)
{
	gpio_tlmm_config_set(gpio_num, GPIO_FUNC_DISABLE,
			     GPIO_NO_PULL, GPIO_2MA, GPIO_DISABLE);
	udelay(10); /* Should be enough to settle. */
	return gpio_get(gpio_num);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio *gpio;
	const int GPIO_COUNT = 5;

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	gpio = gpios->gpios;
	fill_lb_gpio(gpio++, DEV_SW, ACTIVE_LOW, "developer", read_gpio(DEV_SW));
	fill_lb_gpio(gpio++, REC_SW, ACTIVE_LOW, "recovery", read_gpio(REC_SW));
	fill_lb_gpio(gpio++, WP_SW, ACTIVE_LOW, "write protect", read_gpio(WP_SW));
	fill_lb_gpio(gpio++, -1, ACTIVE_LOW, "power", 1);
	fill_lb_gpio(gpio++, -1, ACTIVE_LOW, "lid", 0);
}

int get_developer_mode_switch(void)
{
	return 0;
}

/*
 * The recovery switch on storm is overloaded: it needs to be pressed for a
 * certain duration at startup to signal different requests:
 *
 * - keeping it pressed for 8 to 16 seconds after startup signals the need for
 *   factory reset (wipeout);
 * - keeping it pressed for longer than 16 seconds signals the need for Chrome
 *   OS recovery.
 *
 * The state is read once and cached for following inquiries. The below enum
 * lists possible states.
 */
enum switch_state {
	not_probed = -1,
	no_req,
	recovery_req,
	wipeout_req
};

static void display_pattern(int pattern)
{
	if (board_id() == BOARD_ID_WHIRLWIND_SP5)
		ww_ring_display_pattern(GSBI_ID_7, pattern);
}

#define WIPEOUT_MODE_DELAY_MS (8 * 1000)
#define RECOVERY_MODE_EXTRA_DELAY_MS (8 * 1000)

static enum switch_state get_switch_state(void)
{
	struct stopwatch sw;
	int sampled_value;
	static enum switch_state saved_state = not_probed;

	if (saved_state != not_probed)
		return saved_state;

	sampled_value = read_gpio(REC_SW) ^ !REC_POL;

	if (!sampled_value) {
		saved_state = no_req;
		display_pattern(WWR_NORMAL_BOOT);
		return saved_state;
	}

	display_pattern(WWR_RECOVERY_PUSHED);
	printk(BIOS_INFO, "recovery button pressed\n");

	stopwatch_init_msecs_expire(&sw, WIPEOUT_MODE_DELAY_MS);

	do {
		sampled_value = read_gpio(REC_SW) ^ !REC_POL;
		if (!sampled_value)
			break;
	} while (!stopwatch_expired(&sw));

	if (sampled_value) {
		display_pattern(WWR_WIPEOUT_REQUEST);
		printk(BIOS_INFO, "wipeout requested, checking recovery\n");
		stopwatch_init_msecs_expire(&sw, RECOVERY_MODE_EXTRA_DELAY_MS);
		do {
			sampled_value = read_gpio(REC_SW) ^ !REC_POL;
			if (!sampled_value)
				break;
		} while (!stopwatch_expired(&sw));

		if (sampled_value) {
			saved_state = recovery_req;
			display_pattern(WWR_RECOVERY_REQUEST);
			printk(BIOS_INFO, "recovery requested\n");
		} else {
			saved_state = wipeout_req;
		}
	} else {
		saved_state = no_req;
		display_pattern(WWR_NORMAL_BOOT);
	}

	return saved_state;
}

int get_recovery_mode_switch(void)
{
	return get_switch_state() == recovery_req;
}

int get_wipeout_mode_switch(void)
{
	return get_switch_state() == wipeout_req;
}

int get_write_protect_state(void)
{
	return read_gpio(WP_SW) ^ !WP_POL;
}
