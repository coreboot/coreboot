/*
 * Copyright (C) 2013 The ChromeOS Authors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/i2c.h>
#include <drivers/ti/tps65090/tps65090.h>
#include <cbmem.h>
#include <delay.h>
#include <arch/cache.h>
#include <arch/exception.h>
#include <arch/gpio.h>
#include <cpu/samsung/exynos5-common/exynos-tmu.h>
#include <cpu/samsung/exynos5250/clk.h>
#include <cpu/samsung/exynos5250/cpu.h>
#include <cpu/samsung/exynos5250/gpio.h>
#include <cpu/samsung/exynos5250/power.h>

#include <cpu/samsung/exynos5-common/i2c.h>
#include <cpu/samsung/exynos5-common/s5p-dp-core.h>


/* convenient shorthand (in MB) */
#define DRAM_START	(CONFIG_SYS_SDRAM_BASE >> 20)
#define DRAM_SIZE	CONFIG_DRAM_SIZE_MB
#define DRAM_END	(DRAM_START + DRAM_SIZE)	/* plus one... */

void hardwaremain(int boot_complete);
void main(void)
{
	console_init();
	printk(BIOS_INFO,
	       "hello from ramstage; now with deluxe exception handling.\n");

	/* set up coreboot tables */
	high_tables_size = CONFIG_COREBOOT_TABLES_SIZE;
	high_tables_base = CONFIG_SYS_SDRAM_BASE +
				((unsigned)CONFIG_DRAM_SIZE_MB << 20ULL) -
				CONFIG_COREBOOT_TABLES_SIZE;
	cbmem_init(high_tables_base, high_tables_size);

	/* set up dcache and MMU */
	/* FIXME: this should happen via resource allocator */
	exynos5250_config_l2_cache();
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

	hardwaremain(0);
}

/* TODO: transplanted DP stuff, clean up once we have something that works */
static enum exynos5_gpio_pin dp_pd_l = GPIO_Y25;	/* active low */
static enum exynos5_gpio_pin dp_rst_l = GPIO_X15;	/* active low */
static enum exynos5_gpio_pin dp_hpd = GPIO_X07;		/* active high */

static void exynos_dp_bridge_setup(void)
{
	exynos_pinmux_config(PERIPH_ID_DPHPD, 0);

	gpio_set_value(dp_pd_l, 1);
	gpio_cfg_pin(dp_pd_l, EXYNOS_GPIO_OUTPUT);
	gpio_set_pull(dp_pd_l, EXYNOS_GPIO_PULL_NONE);

	gpio_set_value(dp_rst_l, 0);
	gpio_cfg_pin(dp_rst_l, EXYNOS_GPIO_OUTPUT);
	gpio_set_pull(dp_rst_l, EXYNOS_GPIO_PULL_NONE);
	udelay(10);
	gpio_set_value(dp_rst_l, 1);

	udelay(90000);	/* FIXME: this might be unnecessary */
}

static void exynos_dp_bridge_init(void)
{
	/* De-assert PD (and possibly RST) to power up the bridge */
	gpio_set_value(dp_pd_l, 1);
	gpio_set_value(dp_rst_l, 1);

	/*
	 * We need to wait for 90ms after bringing up the bridge since
	 * there is a phantom "high" on the HPD chip during its
	 * bootup.  The phantom high comes within 7ms of de-asserting
	 * PD and persists for at least 15ms.  The real high comes
	 * roughly 50ms after PD is de-asserted. The phantom high
	 * makes it hard for us to know when the NXP chip is up.
	 */
	udelay(90000);	/* FIXME: this might be unnecessary */
}

static int exynos_dp_hotplug(void)
{
	int x = gpio_get_value(dp_hpd);
	/* Check HPD.  If it's high, we're all good. */
//	if (gpio_get_value(dp_hpd))
//		return 0;
	printk(BIOS_DEBUG, "%s: dp_hpd: 0x%02x\n", __func__, x);
	if (x)
		return 0;
	return -1;
}

static void exynos_dp_reset(void)
{
	gpio_set_value(dp_pd_l, 0);
	gpio_set_value(dp_rst_l, 0);
	/* paranoid delay period (300ms) */
	udelay(300 * 1000);
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

static void snow_backlight_pwm(void)
{
	/*Configure backlight PWM as a simple output high (100% brightness) */
	gpio_direction_output(GPIO_B20, 1);
	udelay(LCD_T6_DELAY_MS * 1000);
}

static void snow_backlight_en(void)
{
	/* * Configure GPIO for LCD_BL_EN */
	gpio_direction_output(GPIO_X30, 1);
}

#define TPS69050_BUS	4	/* Snow-specific */

#define FET1_CTRL	0x0f
#define FET6_CTRL	0x14

static void snow_lcd_vdd(void)
{
	/* Enable FET6, lcd panel */
	tps65090_fet_enable(TPS69050_BUS, FET6_CTRL);
}

static void snow_backlight_vdd(void)
{
	/* Enable FET1, backlight */
	tps65090_fet_enable(TPS69050_BUS, FET1_CTRL);
	udelay(LCD_T5_DELAY_MS * 1000);
}

//static struct video_info smdk5250_dp_config = {
static struct video_info snow_dp_video_info = {
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
#define EXYNOS5250_DP1_BASE	0x145b0000
#define SNOW_MAX_DP_TRIES	5

/* this happens after cpu_init where exynos resources are set */
static void mainboard_init(device_t dev)
{
	int dp_tries;
	struct s5p_dp_device dp_device = {
		.base = (struct exynos5_dp *)EXYNOS5250_DP1_BASE,
		.video_info = &snow_dp_video_info,
	};

	i2c_init(TPS69050_BUS, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	i2c_init(7, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

	tmu_init(&exynos5250_tmu_info);

	snow_lcd_vdd();
	do {
		udelay(50);
	} while (!exynos_dp_hotplug());

	exynos_dp_bridge_setup();
	for (dp_tries = 1; dp_tries <= SNOW_MAX_DP_TRIES; dp_tries++) {
		exynos_dp_bridge_init();
		if (exynos_dp_hotplug()) {
			exynos_dp_reset();
			continue;
		}

		if (dp_controller_init(&dp_device))
			continue;

		udelay(LCD_T3_DELAY_MS * 1000);

		snow_backlight_vdd();
		snow_backlight_pwm();
		snow_backlight_en();
		/* if we're here, we're successful */
		break;
	}

	if (dp_tries > SNOW_MAX_DP_TRIES)
		printk(BIOS_ERR, "%s: Failed to set up displayport\n", __func__);
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name	= "Samsung/Google ARM Chromebook",
	.enable_dev = mainboard_enable,
};
