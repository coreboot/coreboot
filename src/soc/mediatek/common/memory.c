/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <bootmode.h>
#include <cbfs.h>
#include <console/console.h>
#include <ip_checksum.h>
#include <soc/emi.h>
#include <symbols.h>
#include <timer.h>

const char *get_dram_geometry_str(u32 ddr_geometry);
const char *get_dram_type_str(u32 ddr_type);

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
			}
			printk(BIOS_DEBUG, "[MEM] rank %u complex R/W mem test passed\n", rank);

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

const char *get_dram_geometry_str(u32 ddr_geometry)
{
	const char *s;

	switch (ddr_geometry) {
	case DDR_TYPE_2CH_2RK_4GB_2_2:
		s = "2CH_2RK_4GB_2_2";
		break;
	case DDR_TYPE_2CH_2RK_6GB_3_3:
		s = "2CH_2RK_6GB_3_3";
		break;
	case DDR_TYPE_2CH_2RK_8GB_4_4:
		s = "2CH_2RK_8GB_4_4";
		break;
	case DDR_TYPE_2CH_2RK_8GB_4_4_BYTE:
		s = "2CH_2RK_8GB_4_4_BYTE";
		break;
	case DDR_TYPE_2CH_1RK_4GB_4_0:
		s = "2CH_1RK_4GB_4_0";
		break;
	case DDR_TYPE_2CH_2RK_6GB_2_4:
		s = "2CH_2RK_6GB_2_4";
		break;
	default:
		s = "";
		break;
	}

	return s;
}

const char *get_dram_type_str(u32 ddr_type)
{
	const char *s;

	switch (ddr_type) {
	case DDR_TYPE_DISCRETE:
		s = "DSC";
		break;
	case DDR_TYPE_EMCP:
		s = "EMCP";
		break;
	default:
		s = "";
		break;
	}

	return s;
}

static int dram_run_fast_calibration(struct dramc_param *dparam)
{
	if (!is_valid_dramc_param(dparam)) {
		printk(BIOS_WARNING, "DRAM-K: Invalid DRAM calibration data from flash\n");
		dump_param_header((void *)dparam);
		return -1;
	}

	const u32 checksum = compute_checksum(dparam);
	if (dparam->header.checksum != checksum) {
		printk(BIOS_ERR,
		       "DRAM-K: Invalid DRAM calibration checksum from flash "
		       "(expected: %#x, saved: %#x)\n",
		       checksum, dparam->header.checksum);
		return DRAMC_ERR_INVALID_CHECKSUM;
	}

	const u16 config = CONFIG(MEDIATEK_DRAM_DVFS) ? DRAMC_ENABLE_DVFS : DRAMC_DISABLE_DVFS;
	if (dparam->dramc_datas.ddr_info.config_dvfs != config) {
		printk(BIOS_WARNING,
		       "DRAM-K: Incompatible config for calibration data from flash "
		       "(expected: %#x, saved: %#x)\n",
		       config, dparam->dramc_datas.ddr_info.config_dvfs);
		return -1;
	}

	printk(BIOS_INFO, "DRAM-K: DRAM calibration data valid pass\n");
	init_dram_by_params(dparam);
	if (mt_mem_test(&dparam->dramc_datas) == 0)
		return 0;

	return DRAMC_ERR_FAST_CALIBRATION;
}

static int dram_run_full_calibration(struct dramc_param *dparam)
{
	/* Load and run the provided blob for full-calibration if available */
	struct prog dram = PROG_INIT(PROG_REFCODE, CONFIG_CBFS_PREFIX "/dram");

	initialize_dramc_param(dparam);

	if (cbfs_prog_stage_load(&dram)) {
		printk(BIOS_ERR, "DRAM-K: CBFS load program failed\n");
		return -2;
	}

	dparam->do_putc = do_putchar;

	prog_set_entry(&dram, prog_entry(&dram), dparam);
	prog_run(&dram);
	if (dparam->header.status != DRAMC_SUCCESS) {
		printk(BIOS_ERR, "DRAM-K: Full calibration failed: status = %d\n",
		       dparam->header.status);
		return -3;
	}

	if (!(dparam->header.flags & DRAMC_FLAG_HAS_SAVED_DATA)) {
		printk(BIOS_ERR,
		       "DRAM-K: Full calibration executed without saving parameters. "
		       "Please ensure the blob is built properly.\n");
		return -4;
	}

	return 0;
}

static void mem_init_set_default_config(struct dramc_param *dparam,
					const struct sdram_info *dram_info)
{
	u32 type, geometry;
	memset(dparam, 0, sizeof(*dparam));

	type = dram_info->ddr_type;
	geometry = dram_info->ddr_geometry;

	dparam->dramc_datas.ddr_info.ddr_type = type;

	if (CONFIG(MEDIATEK_DRAM_DVFS))
		dparam->dramc_datas.ddr_info.config_dvfs = DRAMC_ENABLE_DVFS;

	dparam->dramc_datas.ddr_info.ddr_geometry = geometry;

	printk(BIOS_INFO, "DRAM-K: ddr_type: %s, config_dvfs: %d, ddr_geometry: %s\n",
	       get_dram_type_str(type),
	       dparam->dramc_datas.ddr_info.config_dvfs,
	       get_dram_geometry_str(geometry));
}

static void mt_mem_init_run(struct dramc_param_ops *dparam_ops,
			    const struct sdram_info *dram_info)
{
	struct dramc_param *dparam = dparam_ops->param;
	struct stopwatch sw;
	int ret;

	/* Load calibration params from flash and run fast calibration */
	mem_init_set_default_config(dparam, dram_info);
	if (dparam_ops->read_from_flash(dparam)) {
		printk(BIOS_INFO, "DRAM-K: Running fast calibration\n");
		stopwatch_init(&sw);

		ret = dram_run_fast_calibration(dparam);
		if (ret != 0) {
			printk(BIOS_ERR, "DRAM-K: Failed to run fast calibration "
			       "in %ld msecs, error: %d\n",
			       stopwatch_duration_msecs(&sw), ret);

			/* Erase flash data after fast calibration failed */
			memset(dparam, 0xa5, sizeof(*dparam));
			dparam_ops->write_to_flash(dparam);
		} else {
			printk(BIOS_INFO, "DRAM-K: Fast calibration passed in %ld msecs\n",
			       stopwatch_duration_msecs(&sw));
			return;
		}
	} else {
		printk(BIOS_WARNING, "DRAM-K: Failed to read calibration data from flash\n");
	}

	/* Run full calibration */
	printk(BIOS_INFO, "DRAM-K: Running full calibration\n");
	mem_init_set_default_config(dparam, dram_info);

	stopwatch_init(&sw);
	int err = dram_run_full_calibration(dparam);
	if (err == 0) {
		printk(BIOS_INFO, "DRAM-K: Full calibration passed in %ld msecs\n",
		       stopwatch_duration_msecs(&sw));

		dparam->header.checksum = compute_checksum(dparam);
		dparam_ops->write_to_flash(dparam);
		printk(BIOS_DEBUG, "DRAM-K: Calibration params saved to flash: "
		       "version=%#x, size=%#x\n",
		       dparam->header.version, dparam->header.size);
	} else {
		printk(BIOS_ERR, "DRAM-K: Full calibration failed in %ld msecs\n",
		       stopwatch_duration_msecs(&sw));
	}
}

void mt_mem_init(struct dramc_param_ops *dparam_ops)
{
	const struct sdram_info *sdram_param = get_sdram_config();

	mt_mem_init_run(dparam_ops, sdram_param);
}
