/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <console/console.h>
#include <fmap.h>
#include <soc/dramc_param.h>
#include <soc/emi.h>
#include <soc/mmu_operations.h>
#include <soc/mt6358.h>
#include <soc/pll.h>
#include <soc/rtc.h>

#include "early_init.h"

/* This must be defined in chromeos.fmd in same name and size. */
#define CAL_REGION_RW_DDR_TRAINING		"RW_DDR_TRAINING"
#define CAL_REGION_RW_DDR_TRAINING_SIZE		0x2000

_Static_assert(sizeof(struct dramc_param) <= CAL_REGION_RW_DDR_TRAINING_SIZE,
	       "sizeof(struct dramc_param) exceeds " CAL_REGION_RW_DDR_TRAINING);

static bool read_calibration_data_from_flash(struct dramc_param *dparam)
{
	const size_t length = sizeof(*dparam);
	size_t ret = fmap_read_area(CAL_REGION_RW_DDR_TRAINING, dparam, length);
	printk(BIOS_DEBUG, "%s: ret=%#lx, length=%#lx\n",
	       __func__, ret, length);

	return ret == length;
}

static bool write_calibration_data_to_flash(const struct dramc_param *dparam)
{
	const size_t length = sizeof(*dparam);
	size_t ret = fmap_overwrite_area(CAL_REGION_RW_DDR_TRAINING, dparam, length);
	printk(BIOS_DEBUG, "%s: ret=%#lx, length=%#lx\n",
	       __func__, ret, length);

	return ret == length;
}

/* dramc_param is ~2K and too large to fit in stack. */
static struct dramc_param dramc_parameter;

static struct dramc_param_ops dparam_ops = {
	.param = &dramc_parameter,
	.read_from_flash = &read_calibration_data_from_flash,
	.write_to_flash = &write_calibration_data_to_flash,
};

void platform_romstage_main(void)
{
	/* This will be done in verstage if CONFIG_VBOOT is enabled. */
	if (!CONFIG(VBOOT))
		mainboard_early_init();

	mt6358_init();
	/* Adjust VSIM2 down to 2.7V because it is shared with IT6505. */
	pmic_set_vsim2_cali(2700);
	mt_pll_raise_little_cpu_freq(1989 * MHz);
	pmic_init_scp_voltage();
	rtc_boot();
	mt_mem_init(&dparam_ops);
	mtk_mmu_after_dram();
}
