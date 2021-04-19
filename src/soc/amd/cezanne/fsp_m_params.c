/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/apob_cache.h>
#include <amdblocks/memmap.h>
#include <assert.h>
#include <console/uart.h>
#include <device/device.h>
#include <fsp/api.h>
#include <soc/platform_descriptors.h>
#include <string.h>
#include <types.h>
#include "chip.h"

static void fill_dxio_descriptors(FSP_M_CONFIG *mcfg,
			const fsp_dxio_descriptor *descs, size_t num)
{
	size_t i;

	ASSERT_MSG(num <= FSPM_UPD_DXIO_DESCRIPTOR_COUNT,
			"Too many DXIO descriptors provided.");

	for (i = 0; i < num; i++) {
		memcpy(mcfg->dxio_descriptor[i], &descs[i], sizeof(mcfg->dxio_descriptor[0]));
	}
}

static void fill_ddi_descriptors(FSP_M_CONFIG *mcfg,
			const fsp_ddi_descriptor *descs, size_t num)
{
	size_t i;

	ASSERT_MSG(num <= FSPM_UPD_DDI_DESCRIPTOR_COUNT,
			"Too many DDI descriptors provided.");

	for (i = 0; i < num; i++) {
		memcpy(&mcfg->ddi_descriptor[i], &descs[i], sizeof(mcfg->ddi_descriptor[0]));
	}
}

static void fsp_fill_pcie_ddi_descriptors(FSP_M_CONFIG *mcfg)
{
	const fsp_dxio_descriptor *fsp_dxio;
	const fsp_ddi_descriptor *fsp_ddi;
	size_t num_dxio;
	size_t num_ddi;

	mainboard_get_dxio_ddi_descriptors(&fsp_dxio, &num_dxio,
						&fsp_ddi, &num_ddi);
	fill_dxio_descriptors(mcfg, fsp_dxio, num_dxio);
	fill_ddi_descriptors(mcfg, fsp_ddi, num_ddi);
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSP_M_CONFIG *mcfg = &mupd->FspmConfig;
	const struct soc_amd_cezanne_config *config = config_of_soc();

	mupd->FspmArchUpd.NvsBufferPtr = (uintptr_t)soc_fill_apob_cache();

	mcfg->pci_express_base_addr = CONFIG_MMCONF_BASE_ADDRESS;
	mcfg->tseg_size = CONFIG_SMM_TSEG_SIZE;
	mcfg->bert_size = CONFIG_ACPI_BERT_SIZE;
	mcfg->serial_port_base = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	mcfg->serial_port_use_mmio = CONFIG(DRIVERS_UART_8250MEM);
	mcfg->serial_port_baudrate = get_uart_baudrate();
	mcfg->serial_port_refclk = uart_platform_refclk();

	/* 0 is default */
	mcfg->ccx_down_core_mode = config->downcore_mode;
	mcfg->ccx_disable_smt = config->disable_smt;

	/* when stt_control isn't 1, FSP will ignore the other stt values */
	mcfg->stt_control = config->stt_control;
	mcfg->stt_pcb_sensor_count = config->stt_pcb_sensor_count;
	mcfg->stt_min_limit = config->stt_min_limit;
	mcfg->stt_m1 = config->stt_m1;
	mcfg->stt_m2 = config->stt_m2;
	mcfg->stt_m3 = config->stt_m3;
	mcfg->stt_m4 = config->stt_m4;
	mcfg->stt_m5 = config->stt_m5;
	mcfg->stt_m6 = config->stt_m6;
	mcfg->stt_c_apu = config->stt_c_apu;
	mcfg->stt_c_gpu = config->stt_c_gpu;
	mcfg->stt_c_hs2 = config->stt_c_hs2;
	mcfg->stt_alpha_apu = config->stt_alpha_apu;
	mcfg->stt_alpha_gpu = config->stt_alpha_gpu;
	mcfg->stt_alpha_hs2 = config->stt_alpha_hs2;
	mcfg->stt_skin_temp_apu = config->stt_skin_temp_apu;
	mcfg->stt_skin_temp_gpu = config->stt_skin_temp_gpu;
	mcfg->stt_skin_temp_hs2 = config->stt_skin_temp_hs2;
	mcfg->stt_error_coeff = config->stt_error_coeff;
	mcfg->stt_error_rate_coefficient = config->stt_error_rate_coefficient;

	/* all following fields being 0 is a valid config */
	mcfg->stapm_boost = config->stapm_boost;
	mcfg->stapm_time_constant = config->stapm_time_constant;
	mcfg->apu_only_sppt_limit = config->apu_only_sppt_limit;
	mcfg->sustained_power_limit = config->sustained_power_limit;
	mcfg->fast_ppt_limit = config->fast_ppt_limit;
	mcfg->slow_ppt_limit = config->slow_ppt_limit;

	/* 0 is default */
	mcfg->smartshift_enable = config->smartshift_enable;

	/* 0 is default */
	mcfg->system_configuration = config->system_configuration;

	/* when cppc_ctrl is 0 the other values won't be used */
	mcfg->cppc_ctrl = config->cppc_ctrl;
	mcfg->cppc_perf_limit_max_range = config->cppc_perf_limit_max_range;
	mcfg->cppc_perf_limit_min_range = config->cppc_perf_limit_min_range;
	mcfg->cppc_epp_max_range = config->cppc_epp_max_range;
	mcfg->cppc_epp_min_range = config->cppc_epp_min_range;
	mcfg->cppc_preferred_cores = config->cppc_preferred_cores;

	fsp_fill_pcie_ddi_descriptors(mcfg);
}
