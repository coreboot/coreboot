/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <drivers/parade/ps8625/ps8625.h>
#include <ec/google/chromeec/ec.h>
#include <soc/tmu.h>
#include <soc/clk.h>
#include <soc/cpu.h>
#include <soc/gpio.h>
#include <soc/power.h>
#include <soc/periph.h>
#include <soc/i2c.h>
#include <soc/dp.h>
#include <soc/fimd.h>
#include <soc/usb.h>
#include <string.h>
#include <symbols.h>
#include <vbe.h>
#include <framebuffer_info.h>

/* convenient shorthand (in MB) */
#define DRAM_START	((uintptr_t)_dram/MiB)
#define DRAM_SIZE	CONFIG_DRAM_SIZE_MB

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
	for (i = 0; i < 1000; i++){
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

static enum exynos5_gpio_pin usb_drd0_vbus = GPIO_H00;
static enum exynos5_gpio_pin usb_drd1_vbus = GPIO_H01;
/* static enum exynos5_gpio_pin hsic_reset_l = GPIO_X24; */

static void prepare_usb(void)
{
	/* Kick these resets off early so they get at least 100ms to settle */
	reset_usb_drd0_dwc3();
	reset_usb_drd1_dwc3();
}

static void setup_usb(void)
{
	/* HSIC and USB HOST port not needed in firmware on this board */
	setup_usb_drd0_phy();
	setup_usb_drd1_phy();

	setup_usb_drd0_dwc3();
	setup_usb_drd1_dwc3();

	gpio_direction_output(usb_drd0_vbus, 1);
	gpio_direction_output(usb_drd1_vbus, 1);
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
#define MAX_DP_TRIES	5

static void setup_storage(void)
{
	/* MMC0: Fixed, 8 bit mode, connected with GPIO. */
	if (clock_set_dwmci(PERIPH_ID_SDMMC0))
		printk(BIOS_CRIT, "%s: Failed to set MMC0 clock.\n", __func__);
	exynos_pinmux_sdmmc0();

	/* MMC2: Removable, 4 bit mode, no GPIO. */
	/* (Must be after romstage to avoid breaking SDMMC boot.) */
	clock_set_dwmci(PERIPH_ID_SDMMC2);
	exynos_pinmux_sdmmc2();
}

static void gpio_init(void)
{
	/* Set up the I2C buses. */
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

static void sdmmc_vdd(void)
{
	/* Enable FET4, P3.3V_SDCARD */
	tps65090_thru_ec_fet_set(4);
}

/* this happens after cpu_init where exynos resources are set */
static void mainboard_init(struct device *dev)
{
	/* we'll stick with the crummy u-boot struct for now.*/
	/* doing this as an auto since the struct has to be writeable */
	struct edp_device_info device_info;

	void *fb_addr = (void *)(get_fb_base_kb() * KiB);

	prepare_usb();
	gpio_init();
	setup_storage();
	tmu_init(&exynos5420_tmu_info);

	/* Clock Gating all the unused IP's to save power */
	clock_gate();

	sdmmc_vdd();

	fb_add_framebuffer_info((uintptr_t)fb_addr, 1366, 768, 2 * 1366, 16);

	/*
	 * The reset value for FIMD SYSMMU register MMU_CTRL:0x14640000
	 * should be 0 according to the datasheet, but has experimentally
	 * been found to come up as 3. This means FIMD SYSMMU is on by
	 * default on Exynos5420. For now we are disabling FIMD SYSMMU.
	 */
	write32p(0x14640000, 0x0);
	write32p(0x14680000, 0x0);

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
	device_info.disp_info.name = (char *)"Peach Pit display";
	device_info.disp_info.h_total = 1366;
	device_info.disp_info.v_total = 768;
	device_info.video_info = dp_video_info;
	device_info.raw_edid = panel_edid;
	exynos_init_dp(&device_info);

	backlight_vdd();
	backlight_pwm();
	backlight_en();

	setup_usb();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;

	/* set up caching for the DRAM */
	mmu_config_range(DRAM_START, DRAM_SIZE, DCACHE_WRITEBACK);
	mmu_config_range((uintptr_t)_dma_coherent/MiB,
			 REGION_SIZE(dma_coherent)/MiB, DCACHE_OFF);

	const unsigned int epll_hz = 192000000;
	const unsigned int sample_rate = 48000;
	const unsigned int lr_frame_size = 256;
	clock_epll_set_rate(epll_hz);
	clock_select_i2s_clk_source();
	clock_set_i2s_clk_prescaler(epll_hz, sample_rate * lr_frame_size);

	power_enable_xclkout();
}

struct chip_operations mainboard_ops = {
	.enable_dev	= mainboard_enable,
};

void lb_board(struct lb_header *header)
{
	struct lb_range *dma;

	dma = (struct lb_range *)lb_new_record(header);
	dma->tag = LB_TAG_DMA;
	dma->size = sizeof(*dma);
	dma->range_start = (uintptr_t)_dma_coherent;
	dma->range_size = REGION_SIZE(dma_coherent);
}
