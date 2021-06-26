/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmem.h>
#include <bootmode.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <soc/nvidia/tegra/dc.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/cpu.h>
#include <soc/mc.h>
#include <soc/nvidia/tegra/apbmisc.h>
#include <soc/sdram.h>
#include <soc/sdram_configs.h>

#include "chip.h"

void bootmem_platform_add_ranges(void)
{
	uintptr_t begin;
	size_t size;
	carveout_range(CARVEOUT_TZ, &begin, &size);
	if (size == 0)
		return;
	bootmem_add_range(begin * MiB, size * MiB, BM_MEM_BL31);
}

static void soc_read_resources(struct device *dev)
{
	unsigned long index = 0;
	int i; uintptr_t begin, end;
	size_t size;

	for (i = CARVEOUT_TZ + 1; i < CARVEOUT_NUM; i++) {
		carveout_range(i, &begin, &size);
		if (size == 0)
			continue;
		reserved_ram_range(dev, index++, begin * MiB, size * MiB);
	}

	memory_in_range_below_4gb(&begin, &end);
	ram_from_to(dev, index++, begin * MiB, end * MiB);

	memory_in_range_above_4gb(&begin, &end);
	ram_from_to(dev, index++, begin * MiB, end * MiB);
}

static struct device_operations soc_ops = {
	.read_resources   = soc_read_resources,
	.set_resources    = noop_set_resources,
};

static void enable_tegra210_dev(struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &soc_ops;

	if (!CONFIG(MAINBOARD_DO_NATIVE_VGA_INIT))
		return;

	if (display_init_required())
		display_startup(dev);
	else
		printk(BIOS_INFO, "Skipping display init.\n");
}

static void tegra210_init(void *chip_info)
{
	struct tegra_revision rev;

	tegra_revision_info(&rev);

	printk(BIOS_INFO, "chip %x rev %02x.%x\n",
		rev.chip_id, rev.major, rev.minor);

	/* Save sdram parameters to scratch regs to be used in LP0 resume */
	sdram_lp0_save_params(get_sdram_config());
	printk(BIOS_INFO, "sdram params saved.\n");
}

struct chip_operations soc_nvidia_tegra210_ops = {
	CHIP_NAME("SOC Nvidia Tegra210")
	.init = tegra210_init,
	.enable_dev = enable_tegra210_dev,
};

static void enable_plld(void *unused)
{
	/*
	 * Configure a conservative 300MHz clock for PLLD. The kernel cannot
	 * handle PLLD not being configured so enable PLLD unconditionally
	 * with a default clock rate.
	 */
	clock_configure_plld(300 * MHz);
}

/*
 * The PLLD being enabled is done at BS_DEV_INIT  time because mainboard_init()
 * is the first thing called. This ensures PLLD is up and functional before
 * anything that mainboard can do that implicitly relies on PLLD.
 */
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, enable_plld, NULL);
