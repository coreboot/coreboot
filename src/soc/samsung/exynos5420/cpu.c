/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/device.h>
#include <ec/google/chromeec/ec.h>
#include <soc/dp.h>
#include <soc/fimd.h>
#include <soc/cpu.h>
#include <soc/clk.h>
#include <string.h>

#include "chip.h"

static unsigned int cpu_id;
static unsigned int cpu_rev;

static void set_cpu_id(void)
{
	u32 pro_id = (read32((void *)EXYNOS5_PRO_ID) & 0x00FFF000) >> 12;

	switch (pro_id) {
	case 0x200:
		/* Exynos4210 EVT0 */
		cpu_id = 0x4210;
		cpu_rev = 0;
		break;
	case 0x210:
		/* Exynos4210 EVT1 */
		cpu_id = 0x4210;
		break;
	case 0x412:
		/* Exynos4412 */
		cpu_id = 0x4412;
		break;
	case 0x520:
		/* Exynos5250 */
		cpu_id = 0x5250;
		break;
	case 0x420:
		/* Exynos5420 */
		cpu_id = 0x5420;
		break;
	}
}

/* we distinguish a display port device from a raw graphics device
 * because there are dramatic differences in startup depending on
 * graphics usage. To make startup fast and easier to understand and
 * debug we explicitly name this common case. The alternate approach,
 * involving lots of machine and callbacks, is hard to debug and
 * verify.
 */
static void exynos_displayport_init(struct device *dev, u32 lcdbase,
		unsigned long fb_size)
{
	struct soc_samsung_exynos5420_config *conf = dev->chip_info;
	/* put these on the stack. If, at some point, we want to move
	 * this code to a pre-ram stage, it will be much easier.
	 */
	struct exynos5_fimd_panel panel;
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
	panel.xres = conf->xres;
	panel.yres = conf->yres;

	printk(BIOS_SPEW, "LCD framebuffer @%p\n", (void *)(lcdbase));
	memset((void *)lcdbase, 0, fb_size);	/* clear the framebuffer */

	/*
	 * We need to clean and invalidate the framebuffer region and disable
	 * caching as well. We assume that our dcache <--> memory address
	 * space is identity-mapped in 1MB chunks, so align accordingly.
	 *
	 * Note: We may want to do something clever to ensure the framebuffer
	 * region is aligned such that we don't change dcache policy for other
	 * stuff inadvertently.
	 */
	uint32_t lower = ALIGN_DOWN(lcdbase, MiB);
	uint32_t upper = ALIGN_UP(lcdbase + fb_size, MiB);

	dcache_clean_invalidate_by_mva((void *)lower, upper - lower);
	mmu_config_range(lower / MiB, (upper - lower) / MiB, DCACHE_OFF);
}

static void tps65090_thru_ec_fet_disable(int index)
{
	uint8_t value = 0;

	if (google_chromeec_i2c_xfer(0x48, 0xe + index, 1, &value, 1, 0)) {
		printk(BIOS_ERR,
		       "Error sending i2c pass through command to EC.\n");
		return;
	}
}

static void cpu_enable(struct device *dev)
{
	unsigned long fb_size = FB_SIZE_KB * KiB;
	u32 lcdbase = get_fb_base_kb() * KiB;

	/*
	 * Disable LCD FETs before we do anything with the display.
	 * FIXME(dhendrix): This is a gross hack and should be done
	 * elsewhere (romstage?).
	 */
	tps65090_thru_ec_fet_disable(1);
	tps65090_thru_ec_fet_disable(6);

	exynos_displayport_init(dev, lcdbase, fb_size);

	set_cpu_id();
}

static void cpu_read_resources(struct device *dev)
{
	unsigned long fb_size = FB_SIZE_KB * KiB;
	u32 lcdbase = get_fb_base_kb() * KiB;

	ram_range(dev, 0, RAM_BASE_KB * KiB, (RAM_SIZE_KB - FB_SIZE_KB) * KiB);
	mmio_range(dev, 1, lcdbase, fb_size);
}

static void cpu_init(struct device *dev)
{
	printk(BIOS_INFO, "CPU:   S5P%X @ %ldMHz\n",
			cpu_id, get_arm_clk() / 1000000);
}

static struct device_operations cpu_ops = {
	.read_resources   = cpu_read_resources,
	.set_resources    = noop_set_resources,
	.enable_resources = cpu_enable,
	.init             = cpu_init,
};

static void enable_exynos5420_dev(struct device *dev)
{
	dev->ops = &cpu_ops;
}

struct chip_operations soc_samsung_exynos5420_ops = {
	CHIP_NAME("SOC Samsung Exynos 5420")
	.enable_dev = enable_exynos5420_dev,
};
