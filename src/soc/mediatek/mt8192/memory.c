/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <bootmode.h>
#include <cbfs.h>
#include <console/console.h>
#include <ip_checksum.h>
#include <soc/emi.h>
#include <symbols.h>

static int mt_mem_test(const struct dramc_data *dparam)
{
	if (CONFIG(MEMORY_TEST)) {
		u8 *addr = _dram;
		const struct ddr_base_info *ddr_info = &dparam->ddr_info;

		for (u8 rank = RANK_0; rank < ddr_info->support_ranks; rank++) {
			int result = complex_mem_test(addr, 0x2000);

			if (result != 0) {
				printk(BIOS_ERR,
				       "[MEM] complex R/W mem test failed: %d\n", result);
				return -1;
			} else {
				printk(BIOS_DEBUG, "[MEM] complex R/W mem test passed\n");
			}

			addr += ddr_info->rank_size[rank];
		}
	}

	return 0;
}

static u32 compute_checksum(const struct dramc_param *dparam)
{
	return (u32)compute_ip_checksum(&dparam->dramc_datas,
					sizeof(dparam->dramc_datas));
}

static int dram_run_fast_calibration(const struct dramc_param *dparam)
{
	if (!is_valid_dramc_param(dparam)) {
		printk(BIOS_WARNING, "Invalid DRAM calibration data from flash\n");
		dump_param_header((void *)dparam);
		return -1;
	}

	const u32 checksum = compute_checksum(dparam);
	if (dparam->header.checksum != checksum) {
		printk(BIOS_ERR,
		       "Invalid DRAM calibration checksum from flash "
		       "(expected: %#x, saved: %#x)\n",
		       checksum, dparam->header.checksum);
		return DRAMC_ERR_INVALID_CHECKSUM;
	}

	const u16 config = CONFIG(MT8192_DRAM_DVFS) ? DRAMC_ENABLE_DVFS : DRAMC_DISABLE_DVFS;
	if (dparam->dramc_datas.ddr_info.config_dvfs != config) {
		printk(BIOS_WARNING,
		       "Incompatible config for calibration data from flash "
		       "(expected: %#x, saved: %#x)\n",
		       config, dparam->dramc_datas.ddr_info.config_dvfs);
		return -1;
	}

	printk(BIOS_INFO, "DRAM calibration data valid pass\n");
	mt_set_emi(&dparam->dramc_datas);
	if (mt_mem_test(&dparam->dramc_datas) == 0)
		return 0;

	return DRAMC_ERR_FAST_CALIBRATION;
}

static void mem_init_set_default_config(struct dramc_param *dparam,
	u32 ddr_geometry)
{
	memset(dparam, 0, sizeof(*dparam));

	if (CONFIG(MT8192_DRAM_EMCP))
		dparam->dramc_datas.ddr_info.ddr_type = DDR_TYPE_EMCP;

	if (CONFIG(MT8192_DRAM_DVFS))
		dparam->dramc_datas.ddr_info.config_dvfs = DRAMC_ENABLE_DVFS;
	dparam->dramc_datas.ddr_info.ddr_geometry = ddr_geometry;

	printk(BIOS_INFO, "DRAM-K: ddr_type: %d, config_dvfs: %d, ddr_geometry: %d\n",
		dparam->dramc_datas.ddr_info.ddr_type,
		dparam->dramc_datas.ddr_info.config_dvfs,
		dparam->dramc_datas.ddr_info.ddr_geometry);
}

static void mt_mem_init_run(struct dramc_param_ops *dparam_ops, u32 ddr_geometry)
{
	struct dramc_param *dparam = dparam_ops->param;

	/* Load calibration params from flash and run fast calibration */
	mem_init_set_default_config(dparam, ddr_geometry);
	if (dparam_ops->read_from_flash(dparam)) {
		printk(BIOS_INFO, "DRAM-K: Running fast calibration\n");
		if (dram_run_fast_calibration(dparam) != 0) {
			printk(BIOS_ERR, "Failed to run fast calibration\n");

			/* Erase flash data after fast calibration failed */
			memset(dparam, 0xa5, sizeof(*dparam));
			dparam_ops->write_to_flash(dparam);
		} else {
			printk(BIOS_INFO, "Fast calibration passed\n");
			return;
		}
	} else {
		printk(BIOS_WARNING, "Failed to read calibration data from flash\n");
	}
}

void mt_mem_init(struct dramc_param_ops *dparam_ops)
{
	const struct sdram_info *sdram_param = get_sdram_config();

	mt_mem_init_run(dparam_ops, sdram_param->ddr_geometry);
}
