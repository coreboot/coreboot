#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <delay.h>
#include <console/console.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/i2c.h>		/* FIXME: for backlight FET enable */
#include <cbmem.h>
#include <cpu/samsung/exynos5250/fimd.h>
#include <cpu/samsung/exynos5-common/s5p-dp-core.h>
#include "chip.h"

#define RAM_BASE_KB (CONFIG_SYS_SDRAM_BASE >> 10)
#define RAM_SIZE_KB (CONFIG_DRAM_SIZE_MB << 10UL)

/* we distinguish a display port device from a raw graphics device because there are
 * dramatic differences in startup depending on graphics usage. To make startup fast
 * and easier to understand and debug we explicitly name this common case. The alternate
 * approach, involving lots of machine and callbacks, is hard to debug and verify.
 */
static void exynos_displayport_init(device_t dev)
{
	int ret;
	struct cpu_samsung_exynos5250_config *conf = dev->chip_info;
	/* put these on the stack. If, at some point, we want to move this code to a
	 * pre-ram stage, it will be much easier.
	 */
	vidinfo_t vi;
	struct exynos5_fimd_panel panel;
	u32 lcdbase;

	printk(BIOS_SPEW, "%s: dev %p, conf %p\n", __func__, dev, conf);
	memset(&vi, 0, sizeof(vi));
	memset(&panel, 0, sizeof(panel));

	panel.is_dp = 1; /* Display I/F is eDP */
	/* while it is true that we did a memset to zero,
	 * we leave some 'set to zero' entries here to make
	 * it clear what's going on. Graphics is confusing.
	 */
	panel.is_mipi = 0;
	panel.fixvclk = 0;
	panel.ivclk = 0;
	panel.clkval_f = conf->clkval_f;
	panel.upper_margin = conf->upper_margin;
	panel.lower_margin = conf->lower_margin;
	panel.vsync = conf->vsync;
	panel.left_margin = conf->left_margin;
	panel.right_margin = conf->right_margin;
	panel.hsync = conf->hsync;

	vi.vl_col = conf->xres;
	vi.vl_row = conf->yres;
	vi.vl_bpix = conf->bpp;
	/* The size is a magic number from hardware. */
	lcdbase = (uintptr_t)cbmem_add(CBMEM_ID_CONSOLE, 16*MiB + 64*KiB);
	printk(BIOS_SPEW, "lcd colormap base is %p\n", (void *)(lcdbase));
//	mmio_resource(dev, 0, conf->lcdbase/KiB, 64);
//	vi.cmap = (void *)conf->lcdbase;
	mmio_resource(dev, 0, lcdbase/KiB, 64);
	vi.cmap = (void *)lcdbase;
//	lcdbase = conf->lcdbase + 64*KiB;

	/* FIXME: We had to do a single cbmem_add() above due to weirdness when
	 * the adjacent regions were being merged... */
//	lcdbase = (uintptr_t)cbmem_add(CBMEM_ID_CONSOLE, 16*MiB);
	lcdbase += 64*KiB;
	printk(BIOS_SPEW, "lcd framebuffer base is %p\n", (void *)(lcdbase));
	printk(BIOS_SPEW, "conf->xres is: %u\n", conf->xres);	/* FIXME: remove this */
	mmio_resource(dev, 1, lcdbase/KiB, (conf->xres*conf->yres*4 + (KiB-1))/KiB);
	printk(BIOS_DEBUG, "Initializing exynos VGA, base %p\n",(void *)lcdbase);
	ret = lcd_ctrl_init(&vi, &panel, (void *)lcdbase);

	/* enable backlight FETs (TODO: move all the stuff below to Snow) */
	i2c_set_bus_num(0);
	uint8_t val;

#define TPS69050_ADDR	0x48
#define FET6_CTRL	0x14
	printk(BIOS_DEBUG, "attempting to enable FET6\n");
	//ret = board_dp_lcd_vdd(blob, &wait_ms);
	i2c_read(TPS69050_ADDR, FET6_CTRL, 1, &val, sizeof(val));
	val |= 1;
	i2c_write(TPS69050_ADDR, FET6_CTRL, 1, &val, sizeof(val));

	exynos_pinmux_config(PERIPH_ID_DPHPD, 0);

	/* Setup the GPIOs */
#if 0
	ret = board_dp_bridge_setup(blob, &wait_ms);
#endif
	enum exynos5_gpio_pin dp_pd_l = GPIO_Y25;	/* active low */
	enum exynos5_gpio_pin dp_rst_l = GPIO_X15;	/* active low */
	enum exynos5_gpio_pin dp_hpd = GPIO_X07;	/* active high */

	/* De-assert PD (and possibly RST) to power up the bridge */
	gpio_set_value(dp_pd_l, 1);
	gpio_set_value(dp_rst_l, 1);

	/*
	 * We need to wait for 90ms after bringing up the bridge since there
	 * is a phantom "high" on the HPD chip during its bootup.  The phantom
	 * high comes within 7ms of de-asserting PD and persists for at least
	 * 15ms.  The real high comes roughly 50ms after PD is de-asserted. The
	 * phantom high makes it hard for us to know when the NXP chip is up.
	 */
	*wait_ms = 90;


	gpio_cfg_pin(dp_pd_l, EXYNOS_GPIO_OUTPUT);
	gpio_set_pull(dp_pd_l, EXYNOS_GPIO_PULL_NONE);

#if 0
	if (fdt_gpio_isvalid(&local.dp_rst)) {
		fdtdec_set_gpio(&local.dp_rst, 1);
		gpio_cfg_pin(local.dp_rst.gpio, EXYNOS_GPIO_OUTPUT);
		gpio_set_pull(local.dp_rst.gpio, EXYNOS_GPIO_PULL_NONE);
		udelay(10);
		fdtdec_set_gpio(&local.dp_rst, 0);
	}
#endif
	gpio_set_value(dp_rst_l, 0);
	gpio_cfg_pin(dp_rst_l, EXYNOS_GPIO_OUTPUT);
	gpio_set_pull(dp_rst_l, EXYNOS_GPIO_PULL_NONE);
	udelay(10);
	gpio_set_value(dp_rst, 1);

#if 0
	while (tries < 5) {
		ret = board_dp_bridge_init(blob, &wait_ms);
		ret = board_dp_hotplug(blob, &wait_ms);
		if (ret) {
			ret = board_dp_bridge_reset(blob, &wait_ms);
			continue;
		}
		ret = dp_controller_init(blob, &wait_ms);
		ret = board_dp_backlight_vdd(blob, &wait_ms);
		ret = board_dp_backlight_pwm(blob, &wait_ms);
		ret = board_dp_backlight_en(blob, &wait_ms);
	}
#endif
}

static void cpu_init(device_t dev)
{
	exynos_displayport_init(dev);
	ram_resource(dev, 0, RAM_BASE_KB, RAM_SIZE_KB);
}

static void cpu_noop(device_t dev)
{
}

static struct device_operations cpu_ops = {
	.read_resources   = cpu_noop,
	.set_resources    = cpu_noop,
	.enable_resources = cpu_noop,
	.init             = cpu_init,
	.scan_bus         = 0,
};

static void enable_exynos5250_dev(device_t dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		printk(BIOS_SPEW, "%s: CPU_CLUSTER\n", __func__);
		dev->ops = &cpu_ops;
	}
}

struct chip_operations cpu_samsung_exynos5250_ops = {
	CHIP_NAME("CPU Samsung Exynos 5250")
	.enable_dev = enable_exynos5250_dev,
};

void exynos5250_config_l2_cache(void)
{
	uint32_t val;

	/*
	 * Bit    9 - L2 tag RAM setup (1 cycle)
	 * Bits 8:6 - L2 tag RAM latency (3 cycles)
	 * Bit    5 - L2 data RAM setup (1 cycle)
	 * Bits 2:0 - L2 data RAM latency (3 cycles)
	 */
	val = (1 << 9) | (0x2 << 6) | (1 << 5) | (0x2);
	write_l2ctlr(val);
}
