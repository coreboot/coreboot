/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <console/console.h>
#include <delay.h>
#include <fmap.h>
#include <soc/clkbuf.h>
#include <soc/dramc_param.h>
#include <soc/emi.h>
#include <soc/mmu_operations.h>
#include <soc/mt6315.h>
#include <soc/mt6359p.h>
#include <soc/pll_common.h>
#include <soc/pmif.h>
#include <soc/rtc.h>
#include <soc/srclken_rc.h>

/* This must be defined in chromeos.fmd in same name and size. */
#define CALIBRATION_REGION		"RW_DDR_TRAINING"
#define CALIBRATION_REGION_SIZE		0x2000

_Static_assert(sizeof(struct dramc_param) <= CALIBRATION_REGION_SIZE,
	       "sizeof(struct dramc_param) exceeds " CALIBRATION_REGION);

static bool read_calibration_data_from_flash(struct dramc_param *dparam)
{
	const size_t length = sizeof(*dparam);
	size_t ret = fmap_read_area(CALIBRATION_REGION, dparam, length);
	printk(BIOS_DEBUG, "read data from flash, ret=%#zx, length=%#zx\n", ret, length);

	return ret == length;
}

static bool write_calibration_data_to_flash(const struct dramc_param *dparam)
{
	const size_t length = sizeof(*dparam);
	size_t ret = fmap_overwrite_area(CALIBRATION_REGION, dparam, length);
	printk(BIOS_DEBUG, "write data from flash, ret=%#zx, length=%#zx\n", ret, length);

	return ret == length;
}

/* dramc_param is ~2K and too large to fit in stack. */
static struct dramc_param dramc_parameter;

static struct dramc_param_ops dparam_ops = {
	.param = &dramc_parameter,
	.read_from_flash = &read_calibration_data_from_flash,
	.write_to_flash = &write_calibration_data_to_flash,
};

static void raise_little_cpu_freq(void)
{
	mt6359p_buck_set_voltage(MT6359P_SRAM_PROC2, 1000 * 1000);
	mt6315_buck_set_voltage(MT6315_CPU, MT6315_BUCK_3, 925 * 1000);
	udelay(200);
	mt_pll_raise_little_cpu_freq(2000 * MHz);
	mt_pll_raise_cci_freq(1400 * MHz);
}

void platform_romstage_main(void)
{
	mtk_pmif_init();
	mt6359p_init();
	mt6315_init();
	srclken_rc_init();
	clk_buf_init();
	rtc_boot();
	raise_little_cpu_freq();
	mt_mem_init(&dparam_ops);
	mtk_mmu_after_dram();
}
