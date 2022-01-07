/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <soc/dramc_common.h>
#include <soc/dramc_param.h>
#include <soc/emi.h>

struct dramc_param *dramc_params;

bool is_dvfs_enabled(void)
{
	dramc_info("dram_init: config_dvfs: %d\n",
		   dramc_params->dramc_datas.ddr_info.config_dvfs);
	return !!(dramc_params->dramc_datas.ddr_info.config_dvfs);
}

u32 get_ddr_geometry(void)
{
	dramc_info("dram_init: ddr_geometry: %d\n",
		   dramc_params->dramc_datas.ddr_info.sdram.ddr_geometry);
	return dramc_params->dramc_datas.ddr_info.sdram.ddr_geometry;
}

u32 get_ddr_type(void)
{
	dramc_info("dram_init: ddr_type: %d\n",
		   dramc_params->dramc_datas.ddr_info.sdram.ddr_type);
	return dramc_params->dramc_datas.ddr_info.sdram.ddr_type;
}

void init_dram_by_params(struct dramc_param *dparam)
{
	dramc_params = dparam;
	mt_set_emi(dramc_params);
}
