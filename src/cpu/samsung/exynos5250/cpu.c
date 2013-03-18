#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <delay.h>
#include <console/console.h>
#include <arch/io.h>
#include <device/device.h>
#include <cbmem.h>
#include <cpu/samsung/exynos5250/fimd.h>
#include <cpu/samsung/exynos5-common/s5p-dp-core.h>
#include "chip.h"

#define RAM_BASE_KB (CONFIG_SYS_SDRAM_BASE >> 10)
#define RAM_SIZE_KB (CONFIG_DRAM_SIZE_MB << 10UL)

static void domain_read_resources(device_t dev)
{
	ram_resource(dev, 0, RAM_BASE_KB, RAM_SIZE_KB);
}

static void domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

static unsigned int domain_scan_bus(device_t dev, unsigned int max)
{
	return max;
}


static struct device_operations domain_ops = {
	.read_resources   = domain_read_resources,
	.set_resources    = domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = domain_scan_bus,
};

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
	printk(BIOS_SPEW, "lcd base is %08ulx\n", conf->lcdbase);
	/* The size is a magic number from hardware. */
	mmio_resource(dev, 0, conf->lcdbase/KiB, 64);
	vi.cmap = (void *)conf->lcdbase;
	lcdbase = conf->lcdbase + 64*KiB;

	mmio_resource(dev, 1, lcdbase/KiB, (conf->xres*conf->yres*4 + (KiB-1))/KiB);
	printk(BIOS_DEBUG, "Initializing exynos VGA, base %p\n",(void *)lcdbase);
	ret = lcd_ctrl_init(&vi, &panel, (void *)lcdbase);
#if 0
	ret = board_dp_lcd_vdd(blob, &wait_ms);
	ret = board_dp_bridge_setup(blob, &wait_ms);
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
	printk(BIOS_SPEW, "%s\n", __func__);
	exynos_displayport_init(dev);
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
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		printk(BIOS_SPEW, "%s: DOMAIN\n", __func__);
		dev->ops = &domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		printk(BIOS_SPEW, "%s: CPU_CLUSTER\n", __func__);
		dev->ops = &cpu_ops;
	}
	printk(BIOS_SPEW, "%s: done\n", __func__);
}

struct chip_operations cpu_samsung_exynos5250_ops = {
	CHIP_NAME("CPU Samsung Exynos 5250")
	.enable_dev = enable_exynos5250_dev,
};
