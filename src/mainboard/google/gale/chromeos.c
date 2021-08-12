/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <delay.h>
#include <drivers/i2c/ww_ring/ww_ring.h>
#include <gpio.h>
#include <soc/cdp.h>
#include <soc/blsp.h>
#include <timer.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define PP_SW   41

static int get_rec_sw_gpio_pin(void)
{
	uint8_t board_rev = board_id();

	switch (board_rev) {
	case BOARD_ID_GALE_PROTO:
	case BOARD_ID_GALE_EVT:
	case BOARD_ID_GALE_EVT2_0:
	case BOARD_ID_GALE_EVT2_1:
		return 7;
	case BOARD_ID_GALE_EVT3:
	default:
		return 57;
	}
}

static int get_wp_status_gpio_pin(void)
{
	uint8_t board_rev = board_id();
	switch (board_rev) {
	case BOARD_ID_GALE_PROTO:
	case BOARD_ID_GALE_EVT:
	case BOARD_ID_GALE_EVT2_0:
	case BOARD_ID_GALE_EVT2_1:
		return 6;
	case BOARD_ID_GALE_EVT3:
	default:
		return 53;
	}
}
static int read_gpio(gpio_t gpio_num)
{
	gpio_tlmm_config_set(gpio_num, GPIO_FUNC_DISABLE,
			     GPIO_NO_PULL, GPIO_2MA, GPIO_DISABLE);
	udelay(10); /* Should be enough to settle. */
	return gpio_get(gpio_num);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{PP_SW, ACTIVE_LOW, read_gpio(PP_SW), "presence"},
		{-1, ACTIVE_LOW, 1, "power"},
		{-1, ACTIVE_LOW, 0, "lid"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
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
	ww_ring_display_pattern(BLSP_QUP_ID_3, pattern);
}

#define WIPEOUT_MODE_DELAY_MS (8 * 1000)
#define RECOVERY_MODE_EXTRA_DELAY_MS (8 * 1000)

static enum switch_state get_switch_state(void)
{
	struct stopwatch sw;
	int sampled_value;
	uint8_t rec_sw;
	static enum switch_state saved_state = not_probed;

	if (saved_state != not_probed)
		return saved_state;

	rec_sw = get_rec_sw_gpio_pin();
	sampled_value = !read_gpio(rec_sw);

	if (!sampled_value) {
		saved_state = no_req;
		display_pattern(WWR_NORMAL_BOOT);
		return saved_state;
	}

	display_pattern(WWR_RECOVERY_PUSHED);
	printk(BIOS_INFO, "recovery button pressed\n");

	stopwatch_init_msecs_expire(&sw, WIPEOUT_MODE_DELAY_MS);

	do {
		sampled_value = !read_gpio(rec_sw);
		if (!sampled_value)
			break;
	} while (!stopwatch_expired(&sw));

	if (sampled_value) {
		display_pattern(WWR_WIPEOUT_REQUEST);
		printk(BIOS_INFO, "wipeout requested, checking recovery\n");
		stopwatch_init_msecs_expire(&sw, RECOVERY_MODE_EXTRA_DELAY_MS);
		do {
			sampled_value = !read_gpio(rec_sw);
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
	return !read_gpio(get_wp_status_gpio_pin());
}

int get_ec_is_trusted(void)
{
	/* Do not have a Chrome EC involved in entering recovery mode;
	   Always return trusted. */
	return 1;
}
