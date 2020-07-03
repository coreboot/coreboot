/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <console/console.h>
#include <fmap.h>
#include <soc/dramc_param.h>
#include <soc/emi.h>
#include <soc/mmu_operations.h>

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

void platform_romstage_main(void)
{
	mt_mem_init(&dparam_ops);
	mtk_mmu_after_dram();
}
