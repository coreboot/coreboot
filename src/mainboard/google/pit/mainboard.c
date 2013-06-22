/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#include <console/console.h>
#include <device/device.h>
#include <device/i2c.h>
#include <cbmem.h>
#include <delay.h>
#include <edid.h>
#include <vbe.h>
#include <boot/coreboot_tables.h>
#include <arch/cache.h>
#include <arch/exception.h>
#include <cpu/samsung/exynos5420/tmu.h>
#include <cpu/samsung/exynos5420/clk.h>
#include <cpu/samsung/exynos5420/cpu.h>
#include <cpu/samsung/exynos5420/gpio.h>
#include <cpu/samsung/exynos5420/power.h>
#include <cpu/samsung/exynos5420/i2c.h>
#include <cpu/samsung/exynos5420/dp-core.h>
#include <drivers/parade/ps8625/ps8625.h>
#include <ec/google/chromeec/ec.h>
#include <stdlib.h>

#include "exynos5420.h"

/* convenient shorthand (in MB) */
#define DRAM_START	(CONFIG_SYS_SDRAM_BASE >> 20)
#define DRAM_SIZE	CONFIG_DRAM_SIZE_MB
#define DRAM_END	(DRAM_START + DRAM_SIZE)	/* plus one... */

static struct edid edid = {
	.ha = 1366,
	.va = 768,
	.bpp = 16,
};

/* TODO: transplanted DP stuff, clean up once we have something that works */
static enum exynos5_gpio_pin dp_pd_l = GPIO_X35;	/* active low */
static enum exynos5_gpio_pin dp_rst_l = GPIO_Y77;	/* active low */
static enum exynos5_gpio_pin dp_hpd = GPIO_X26;		/* active high */
static enum exynos5_gpio_pin bl_pwm = GPIO_B20;		/* active high */
static enum exynos5_gpio_pin bl_en = GPIO_X22;		/* active high */

static void parade_dp_bridge_setup(void)
{
	gpio_set_value(dp_pd_l, 1);
	gpio_cfg_pin(dp_pd_l, GPIO_OUTPUT);
	gpio_set_pull(dp_pd_l, GPIO_PULL_NONE);

	gpio_set_value(dp_rst_l, 0);
	gpio_cfg_pin(dp_rst_l, GPIO_OUTPUT);
	gpio_set_pull(dp_rst_l, GPIO_PULL_NONE);
	udelay(10);
	gpio_set_value(dp_rst_l, 1);

	gpio_cfg_pin(dp_hpd, GPIO_INPUT);

	/* De-assert PD (and possibly RST) to power up the bridge. */
	gpio_set_value(dp_pd_l, 1);
	gpio_set_value(dp_rst_l, 1);

	/* Hang around for the bridge to come up. */
	mdelay(40);

	/* Configure the bridge chip. */
	exynos_pinmux_i2c7();
	i2c_init(7, 100000, 0x00);

	parade_ps8625_bridge_setup(7, 0x48);
}

/*
 * This delay is T3 in the LCD timing spec (defined as >200ms). We set
 * this down to 60ms since that's the approximate maximum amount of time
 * it'll take a bridge to start outputting LVDS data. The delay of
 * >200ms is just a conservative value to avoid turning on the backlight
 * when there's random LCD data on the screen. Shaving 140ms off the
 * boot is an acceptable trade-off.
 */
#define LCD_T3_DELAY_MS	60

#define LCD_T5_DELAY_MS	10
#define LCD_T6_DELAY_MS	10

static void backlight_pwm(void)
{
	/*Configure backlight PWM as a simple output high (100% brightness) */
	gpio_direction_output(bl_pwm, 1);
	udelay(LCD_T6_DELAY_MS * 1000);
}

static void backlight_en(void)
{
	/* Configure GPIO for LCD_BL_EN */
	gpio_direction_output(bl_en, 1);
}

//static struct video_info smdk5420_dp_config = {
static struct video_info dp_video_info = {
	/* FIXME: fix video_info struct to use const for name */
	.name			= (char *)"eDP-LVDS NXP PTN3460",

	.h_sync_polarity	= 0,
	.v_sync_polarity	= 0,
	.interlaced		= 0,

	.color_space		= COLOR_RGB,
	.dynamic_range		= VESA,
	.ycbcr_coeff		= COLOR_YCBCR601,
	.color_depth		= COLOR_8,

	.link_rate		= LINK_RATE_2_70GBPS,
	.lane_count		= LANE_COUNT2,
};

/* FIXME: move some place more appropriate */
#define EXYNOS5420_DP1_BASE	0x145b0000
#define MAX_DP_TRIES	5

/*
 * This function disables the USB3.0 PLL to save power
 */
static void disable_usb30_pll(void)
{
	enum exynos5_gpio_pin usb3_pll_l = GPIO_Y11;

	gpio_direction_output(usb3_pll_l, 0);
}

static void gpio_init(void)
{
	/* Set up the I2C busses. */
	exynos_pinmux_i2c2();
	exynos_pinmux_i2c4();
	exynos_pinmux_i2c7();
	exynos_pinmux_i2c8();
	exynos_pinmux_i2c9();
	exynos_pinmux_i2c10();
}

enum {
	FET_CTRL_WAIT = 3 << 2,
	FET_CTRL_ADENFET = 1 << 1,
	FET_CTRL_ENFET = 1 << 0
};

static void tps65090_thru_ec_fet_set(int index)
{
	uint8_t value = FET_CTRL_ADENFET | FET_CTRL_WAIT | FET_CTRL_ENFET;

	if (google_chromeec_i2c_xfer(0x48, 0xe + index, 1, &value, 1, 0)) {
		printk(BIOS_ERR,
		       "Error sending i2c pass through command to EC.\n");
		return;
	}
}

static void lcd_vdd(void)
{
	/* Enable FET6, lcd panel */
	tps65090_thru_ec_fet_set(6);
}

static void backlight_vdd(void)
{
	/* Enable FET1, backlight */
	tps65090_thru_ec_fet_set(1);
}

/* this happens after cpu_init where exynos resources are set */
static void mainboard_init(device_t dev)
{
	struct s5p_dp_device dp_device = {
		.base = (struct exynos5_dp *)EXYNOS5420_DP1_BASE,
		.video_info = &dp_video_info,
	};
	void *fb_addr;

	gpio_init();

	tmu_init(&exynos5420_tmu_info);

	/* Clock Gating all the unused IP's to save power */
	clock_gate();

	/* Disable USB3.0 PLL to save 250mW of power */
	disable_usb30_pll();

	fb_addr = cbmem_find(CBMEM_ID_CONSOLE);
	set_vbe_mode_info_valid(&edid, (uintptr_t)fb_addr);

	lcd_vdd();

	parade_dp_bridge_setup();
	dp_controller_init(&dp_device);

	udelay(LCD_T3_DELAY_MS * 1000);

	backlight_vdd();
	backlight_pwm();
	backlight_en();

	// Uncomment to get excessive GPIO output:
	// gpio_info();
}

void get_cbmem_table(uint64_t *base, uint64_t *size)
{
	*size = CONFIG_COREBOOT_TABLES_SIZE;
	*base = CONFIG_SYS_SDRAM_BASE +
				((unsigned)CONFIG_DRAM_SIZE_MB << 20ULL) -
				CONFIG_COREBOOT_TABLES_SIZE;
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;

	/* set up coreboot tables */
	cbmem_initialize();

	/* set up dcache and MMU */
	/* FIXME: this should happen via resource allocator */
	exynos5420_config_l2_cache();
	mmu_init();
	mmu_config_range(0, DRAM_START, DCACHE_OFF);
	mmu_config_range(DRAM_START, DRAM_SIZE, DCACHE_WRITEBACK);
	mmu_config_range(DRAM_END, 4096 - DRAM_END, DCACHE_OFF);
	dcache_invalidate_all();
	dcache_mmu_enable();

	/* this is going to move, but we must have it now and we're
	 * not sure where */
	exception_init();

	const unsigned epll_hz = 192000000;
	const unsigned sample_rate = 48000;
	const unsigned lr_frame_size = 256;
	clock_epll_set_rate(epll_hz);
	clock_select_i2s_clk_source();
	clock_set_i2s_clk_prescaler(epll_hz, sample_rate * lr_frame_size);

	power_enable_xclkout();
}

struct chip_operations mainboard_ops = {
	.name	= "Samsung/Google ARM Chromebook",
	.enable_dev = mainboard_enable,
};
