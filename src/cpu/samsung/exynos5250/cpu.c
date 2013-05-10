#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <delay.h>
#include <console/console.h>
#include <device/device.h>
#include <cbmem.h>
#include <arch/cache.h>
#include <cpu/samsung/exynos5250/fimd.h>
#include <cpu/samsung/exynos5-common/s5p-dp-core.h>
#include <cpu/samsung/exynos5-common/cpu.h>
#include "chip.h"
#include "cpu.h"

#define RAM_BASE_KB (CONFIG_SYS_SDRAM_BASE >> 10)
#define RAM_SIZE_KB (CONFIG_DRAM_SIZE_MB << 10UL)

/* we distinguish a display port device from a raw graphics device
 * because there are dramatic differences in startup depending on
 * graphics usage. To make startup fast and easier to understand and
 * debug we explicitly name this common case. The alternate approach,
 * involving lots of machine and callbacks, is hard to debug and
 * verify.
 */
static void exynos_displayport_init(device_t dev)
{
	int ret;
	struct cpu_samsung_exynos5250_config *conf = dev->chip_info;
	/* put these on the stack. If, at some point, we want to move
	 * this code to a pre-ram stage, it will be much easier.
	 */
	vidinfo_t vi;
	struct exynos5_fimd_panel panel;
	unsigned long int fb_size;
	u32 lcdbase;

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
	panel.xres = conf->xres;
	panel.yres = conf->yres;

	vi.vl_col = conf->xres;
	vi.vl_row = conf->yres;
	vi.vl_bpix = conf->bpp;
	/*
	 * The size is a magic number from hardware. Allocate enough for the
	 * frame buffer and color map.
	 */
	fb_size = conf->xres * conf->yres * (conf->bpp / 8);
	lcdbase = (uintptr_t)cbmem_add(CBMEM_ID_CONSOLE, fb_size + 64*KiB);
	printk(BIOS_SPEW, "lcd colormap base is %p\n", (void *)(lcdbase));
	mmio_resource(dev, 0, lcdbase/KiB, 64);
	vi.cmap = (void *)lcdbase;

	/*
	 * We need to clean and invalidate the framebuffer region and disable
	 * caching as well. We assume that our dcache <--> memory address
	 * space is identity-mapped in 1MB chunks, so align accordingly.
	 *
	 * Note: We may want to do something clever to ensure the framebuffer
	 * region is aligned such that we don't change dcache policy for other
	 * stuff inadvertantly.
	 *
	 * FIXME: Is disabling/re-enabling the MMU entirely necessary?
	 */
	uint32_t lower = ALIGN_DOWN(lcdbase, MiB);
	uint32_t upper = ALIGN_UP(lcdbase + fb_size + 64*KiB, MiB);
	dcache_clean_invalidate_by_mva(lower, upper - lower);
	dcache_mmu_disable();
	mmu_config_range(lower/MiB, (upper - lower)/MiB, DCACHE_OFF);
	dcache_mmu_enable();

	lcdbase += 64*KiB;
	mmio_resource(dev, 1, lcdbase/KiB, (fb_size + KiB - 1)/KiB);
	printk(BIOS_DEBUG,
	       "Initializing exynos VGA, base %p\n", (void *)lcdbase);
	memset((void *)lcdbase, 0, fb_size);	/* clear the framebuffer */
	ret = lcd_ctrl_init(&vi, &panel, (void *)lcdbase);
}

static void cpu_init(device_t dev)
{
	exynos_displayport_init(dev);
	ram_resource(dev, 0, RAM_BASE_KB, RAM_SIZE_KB);

	arch_cpu_init();
}

static void cpu_noop(device_t dev)
{
}

static struct device_operations cpu_ops = {
	.read_resources   = cpu_noop,
	.set_resources    = cpu_noop,
	.enable_resources = cpu_init,
	.init             = cpu_noop,
	.scan_bus         = 0,
};

static void enable_exynos5250_dev(device_t dev)
{
	dev->ops = &cpu_ops;
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
