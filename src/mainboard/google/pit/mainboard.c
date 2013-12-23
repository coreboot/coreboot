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

#include <string.h>
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
#include <cpu/samsung/exynos5420/dp.h>
#include <cpu/samsung/exynos5420/fimd.h>
#include <drivers/parade/ps8625/ps8625.h>
#include <ec/google/chromeec/ec.h>
#include <stdlib.h>

/* convenient shorthand (in MB) */
#define DRAM_START	(CONFIG_SYS_SDRAM_BASE >> 20)
#define DRAM_SIZE	CONFIG_DRAM_SIZE_MB
#define DRAM_END	(DRAM_START + DRAM_SIZE)	/* plus one... */

static struct edid edid = {
	.ha = 1366,
	.va = 768,
	.bpp = 16,
	.x_resolution = 1366,
	.y_resolution = 768,
	.bytes_per_line = 2 * 1366
};

/* from the fdt */
static struct vidinfo vidinfo = {
	.vl_freq = 60,
	.vl_col = 1366,
	.vl_row = 768,
	.vl_width = 1366,
	.vl_height = 768,
	.vl_clkp = 1,
	.vl_dp = 1,
	.vl_bpix = 4,
	.vl_hspw = 32,
	.vl_hbpd = 40,
	.vl_hfpd = 40,
	.vl_vspw = 6,
	.vl_vbpd = 10,
	.vl_vfpd = 12,
	.vl_cmd_allow_len = 0xf,
	.win_id = 3,
	.dp_enabled = 1,
	.dual_lcd_enabled = 0,
	.interface_mode = FIMD_RGB_INTERFACE,
};

static unsigned char panel_edid[] = {
	0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,
	0x06,0xaf,0x5c,0x31,0x00,0x00,0x00,0x00,
	0x00,0x16,0x01,0x03,0x80,0x1a,0x0e,0x78,
	0x0a,0x99,0x85,0x95,0x55,0x56,0x92,0x28,
	0x22,0x50,0x54,0x00,0x00,0x00,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0xa3,0x1b,
	0x56,0x7e,0x50,0x00,0x16,0x30,0x30,0x20,
	0x36,0x00,0x00,0x90,0x10,0x00,0x00,0x18,
	0x6d,0x12,0x56,0x7e,0x50,0x00,0x16,0x30,
	0x30,0x20,0x36,0x00,0x00,0x90,0x10,0x00,
	0x00,0x18,0x00,0x00,0x00,0xfe,0x00,0x41,
	0x55,0x4f,0x0a,0x20,0x20,0x20,0x20,0x20,
	0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xfe,
	0x00,0x42,0x31,0x31,0x36,0x58,0x57,0x30,
	0x33,0x20,0x56,0x31,0x20,0x0a,0x00,0x3d,
	0x00,0xc0,0x00,0x00,0x27,0xfd,0x00,0x20,
	0x02,0x59,0x07,0x00,0x64,0x3e,0x07,0x02,
	0x00,0x00,0xcd,0x12,0x59,0xff,0x10,0x03,
	0x00,0x00,0x00,0x00,0x64,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,
	0x9c,0x3f,0x07,0x02,0x31,0xf9,0x00,0x20,
	0x59,0xff,0x10,0x03,0x00,0x00,0x00,0x00,
	0xbc,0x3e,0x07,0x02,0xc0,0x9b,0x01,0x20,
	0x00,0x00,0x00,0x00,0xdb,0xf8,0x00,0x20,
	0x98,0x3e,0x07,0x02,0x8b,0xaf,0x00,0x20,
	0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xe5,0xcd,0x16,0x00,0xe9,0xcd,0x16,0x00,
	0xe8,0x03,0x00,0x00,0x6c,0x55,0x01,0x20,
	0x2c,0x01,0x00,0x00,0x85,0xbb,0x00,0x20,
	0xe8,0x03,0x00,0x00,0xe9,0xcd,0x16,0x00,
};

static const struct parade_write parade_writes[] = {
	{ 0x02, 0xa1, 0x01 },  /* HPD low */
	 /*
	  * SW setting
	  * [1:0] SW output 1.2V voltage is lower to 96%
	  */
	{ 0x04, 0x14, 0x01 },
	 /*
	  * RCO SS setting
	  * [5:4] = b01 0.5%, b10 1%, b11 1.5%
	  */
	{ 0x04, 0xe3, 0x20 },
	{ 0x04, 0xe2, 0x80 }, /* [7] RCO SS enable */
	 /*
	  *  RPHY Setting
	  * [3:2] CDR tune wait cycle before
	  * measure for fine tune b00: 1us,
	  * 01: 0.5us, 10:2us, 11:4us.
	  */
	{ 0x04, 0x8a, 0x0c },
	{ 0x04, 0x89, 0x08 }, /* [3] RFD always on */
	 /*
	  * CTN lock in/out:
	  * 20000ppm/80000ppm. Lock out 2
	  * times.
	  */
	{ 0x04, 0x71, 0x2d },
	 /*
	  * 2.7G CDR settings
	  * NOF=40LSB for HBR CDR setting
	  */
	{ 0x04, 0x7d, 0x07 },
	{ 0x04, 0x7b, 0x00 },  /* [1:0] Fmin=+4bands */
	{ 0x04, 0x7a, 0xfd },  /* [7:5] DCO_FTRNG=+-40% */
	 /*
	  * 1.62G CDR settings
	  * [5:2]NOF=64LSB [1:0]DCO scale is 2/5
	  */
	{ 0x04, 0xc0, 0x12 },
	{ 0x04, 0xc1, 0x92 },  /* Gitune=-37% */
	{ 0x04, 0xc2, 0x1c },  /* Fbstep=100% */
	{ 0x04, 0x32, 0x80 },  /* [7] LOS signal disable */
	 /*
	  * RPIO Setting
	  * [7:4] LVDS driver bias current :
	  * 75% (250mV swing)
	  */
	{ 0x04, 0x00, 0xb0 },
	 /*
	  * [7:6] Right-bar GPIO output strength is 8mA
	  */
	{ 0x04, 0x15, 0x40 },
	 /* EQ Training State Machine Setting */
	{ 0x04, 0x54, 0x10 },  /* RCO calibration start */
	 /* [4:0] MAX_LANE_COUNT set to one lane */
	{ 0x01, 0x02, 0x81 },
	 /* [4:0] LANE_COUNT_SET set to one lane */
	{ 0x01, 0x21, 0x81 },
	{ 0x00, 0x52, 0x20 },
	{ 0x00, 0xf1, 0x03 },  /* HPD CP toggle enable */
	{ 0x00, 0x62, 0x41 },
	 /* Counter number, add 1ms counter delay */
	{ 0x00, 0xf6, 0x01 },
	 /*
	  * [6]PWM function control by
	  * DPCD0040f[7], default is PWM
	  * block always works.
	  */
	{ 0x00, 0x77, 0x06 },
	 /*
	  * 04h Adjust VTotal tolerance to
	  * fix the 30Hz no display issue
	  */
	{ 0x00, 0x4c, 0x04 },
	 /* DPCD00400='h00, Parade OUI = 'h001cf8 */
	{ 0x01, 0xc0, 0x00 },
	{ 0x01, 0xc1, 0x1c },  /* DPCD00401='h1c */
	{ 0x01, 0xc2, 0xf8 },  /* DPCD00402='hf8 */
	 /*
	  * DPCD403~408 = ASCII code
	  * D2SLV5='h4432534c5635
	  */
	{ 0x01, 0xc3, 0x44 },
	{ 0x01, 0xc4, 0x32 },  /* DPCD404 */
	{ 0x01, 0xc5, 0x53 },  /* DPCD405 */
	{ 0x01, 0xc6, 0x4c },  /* DPCD406 */
	{ 0x01, 0xc7, 0x56 },  /* DPCD407 */
	{ 0x01, 0xc8, 0x35 },  /* DPCD408 */
	 /*
	  * DPCD40A, Initial Code major  revision
	  * '01'
	  */
	{ 0x01, 0xca, 0x01 },
	 /* DPCD40B, Initial Code minor revision '05' */
	{ 0x01, 0xcb, 0x05 },
	 /* DPCD720, Select external PWM */
	{ 0x01, 0xa5, 0x80 },
	 /*
	  * Set LVDS output as 6bit-VESA mapping,
	  * single LVDS channel
	  */
	{ 0x01, 0xcc, 0x13 },
	 /* Enable SSC set by register */
	{ 0x02, 0xb1, 0x20 },
	 /*
	  * Set SSC enabled and +/-1% central
	  * spreading
	  */
	{ 0x04, 0x10, 0x16 },
	 /* MPU Clock source: LC => RCO */
	{ 0x04, 0x59, 0x60 },
	{ 0x04, 0x54, 0x14 },  /* LC -> RCO */
	{ 0x02, 0xa1, 0x91 }  /* HPD high */
};

/* TODO: transplanted DP stuff, clean up once we have something that works */
static enum exynos5_gpio_pin dp_pd_l = GPIO_X35;	/* active low */
static enum exynos5_gpio_pin dp_rst_l = GPIO_Y77;	/* active low */
static enum exynos5_gpio_pin dp_hpd = GPIO_X26;		/* active high */
static enum exynos5_gpio_pin bl_pwm = GPIO_B20;		/* active high */
static enum exynos5_gpio_pin bl_en = GPIO_X22;		/* active high */

static void parade_dp_bridge_setup(void)
{
	int i;

	gpio_set_value(dp_pd_l, 1);
	gpio_cfg_pin(dp_pd_l, GPIO_OUTPUT);
	gpio_set_pull(dp_pd_l, GPIO_PULL_NONE);

	gpio_set_value(dp_rst_l, 0);
	gpio_cfg_pin(dp_rst_l, GPIO_OUTPUT);
	gpio_set_pull(dp_rst_l, GPIO_PULL_NONE);
	udelay(10);
	gpio_set_value(dp_rst_l, 1);


	gpio_set_pull(dp_hpd, GPIO_PULL_NONE);
	gpio_cfg_pin(dp_hpd, GPIO_INPUT);

	/* De-assert PD (and possibly RST) to power up the bridge. */
	gpio_set_value(dp_pd_l, 1);
	gpio_set_value(dp_rst_l, 1);

	/* Hang around for the bridge to come up. */
	mdelay(40);

	/* Configure the bridge chip. */
	exynos_pinmux_i2c7();
	i2c_init(7, 100000, 0x00);

	parade_ps8625_bridge_setup(7, 0x48, parade_writes,
				   ARRAY_SIZE(parade_writes));
	/* Spin until the display is ready.
	 * It's quite important to try really hard to get the display up,
	 * so be generous. It will typically be ready in only 5 ms. and
	 * we're out of here.
	 * If it's not ready after a second, then we're in big trouble.
	 */
	for(i = 0; i < 1000; i++){
		if (gpio_get_value(dp_hpd))
			break;
		mdelay(1);
	}
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



static struct edp_video_info dp_video_info = {
	.master_mode = 0,
	.h_sync_polarity	= 0,
	.v_sync_polarity	= 0,
	.interlaced		= 0,
	.color_space		= COLOR_RGB,
	.dynamic_range		= VESA,
	.ycbcr_coeff		= COLOR_YCBCR601,
	.color_depth		= COLOR_8,
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
	/* we'll stick with the crummy u-boot struct for now.*/
	/* doing this as an auto since the struct has to be writeable */
	struct edp_device_info device_info;

	void *fb_addr = (void *)(get_fb_base_kb() * KiB);

	gpio_init();
	tmu_init(&exynos5420_tmu_info);

	/* Clock Gating all the unused IP's to save power */
	clock_gate();

	/* Disable USB3.0 PLL to save 250mW of power */
	disable_usb30_pll();

	set_vbe_mode_info_valid(&edid, (uintptr_t)fb_addr);

	/*
	 * The reset value for FIMD SYSMMU register MMU_CTRL:0x14640000
	 * should be 0 according to the datasheet, but has experimentally
	 * been found to come up as 3. This means FIMD SYSMMU is on by
	 * default on Exynos5420. For now we are disabling FIMD SYSMMU.
	 */
	writel(0x0, (void *)0x14640000);
	writel(0x0, (void *)0x14680000);

	lcd_vdd();

	/* Start the fimd running before you do the phy and lcd setup.
	 * why do fimd before training etc?
	 * because we need a data stream from
	 * the fimd or the clock recovery step fails.
	 */
	vidinfo.screen_base = fb_addr;
	exynos_fimd_lcd_init(&vidinfo);

	parade_dp_bridge_setup();

	/* this might get more dynamic in future ... */
	memset(&device_info, 0, sizeof(device_info));
	device_info.disp_info.name = (char *)"Pit display";
	device_info.disp_info.h_total = 1366;
	device_info.disp_info.v_total = 768;
	device_info.video_info = dp_video_info;
	device_info.raw_edid = panel_edid;
	exynos_init_dp(&device_info);

	udelay(LCD_T3_DELAY_MS * 1000);

	backlight_vdd();
	backlight_pwm();
	backlight_en();
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;

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
