/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/apob_cache.h>
#include <amdblocks/ioapic.h>
#include <amdblocks/memmap.h>
#include <assert.h>
#include <console/uart.h>
#include <device/device.h>
#include <fsp/api.h>
#include <soc/platform_descriptors.h>
#include <soc/pci_devs.h>
#include <string.h>
#include <types.h>
#include "chip.h"

static const struct device_path gfx_hda_path[] = {
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = PCIE_ABC_A_DEVFN
	},
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = GFX_HDA_DEVFN
	},
};

static bool devtree_gfx_hda_dev_enabled(void)
{
	const struct device *gfx_hda_dev;

	gfx_hda_dev = find_dev_nested_path(pci_root_bus(), gfx_hda_path,
						ARRAY_SIZE(gfx_hda_path));
	if (!gfx_hda_dev)
		return false;

	return gfx_hda_dev->enabled;
}

static const struct device_path hda_path[] = {
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = PCIE_ABC_A_DEVFN
	},
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = HD_AUDIO_DEVFN
	},
};

static bool devtree_hda_dev_enabled(void)
{
	const struct device *hda_dev;

	hda_dev = find_dev_nested_path(pci_root_bus(), hda_path, ARRAY_SIZE(hda_path));

	if (!hda_dev)
		return false;

	return hda_dev->enabled;
}

static const struct device_path sata0_path[] = {
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = PCIE_GPP_B_DEVFN
	},
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = SATA0_DEVFN
	},
};

static const struct device_path sata1_path[] = {
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = PCIE_GPP_B_DEVFN
	},
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = SATA1_DEVFN
	},
};

static bool devtree_sata_dev_enabled(void)
{
	const struct device *ahci0_dev, *ahci1_dev;

	ahci0_dev = find_dev_nested_path(pci_root_bus(), sata0_path, ARRAY_SIZE(sata0_path));
	ahci1_dev = find_dev_nested_path(pci_root_bus(), sata1_path, ARRAY_SIZE(sata1_path));

	if (!ahci0_dev || !ahci1_dev)
		return false;

	return ahci0_dev->enabled || ahci1_dev->enabled;
}

__weak void mb_pre_fspm(void)
{
}

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

static void fsp_assign_ioapic_upds(FSP_M_CONFIG *mcfg)
{
	mcfg->gnb_ioapic_base = GNB_IO_APIC_ADDR;
	mcfg->gnb_ioapic_id = GNB_IOAPIC_ID;
	mcfg->fch_ioapic_id = FCH_IOAPIC_ID;
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSP_M_CONFIG *mcfg = &mupd->FspmConfig;
	const struct soc_amd_cezanne_config *config = config_of_soc();

	mupd->FspmArchUpd.NvsBufferPtr = (uintptr_t)soc_fill_apob_cache();

	mcfg->pci_express_base_addr = CONFIG_MMCONF_BASE_ADDRESS;
	mcfg->tseg_size = CONFIG_SMM_TSEG_SIZE;
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
	mcfg->stapm_time_constant = config->stapm_time_constant_s;
	mcfg->apu_only_sppt_limit = config->apu_only_sppt_limit;
	mcfg->sustained_power_limit = config->sustained_power_limit_mW;
	mcfg->fast_ppt_limit = config->fast_ppt_limit_mW;
	mcfg->slow_ppt_limit = config->slow_ppt_limit_mW;
	mcfg->slow_ppt_time_constant = config->slow_ppt_time_constant_s;
	mcfg->thermctl_limit = config->thermctl_limit_degreeC;

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

	/* S0i3 enable */
	mcfg->s0i3_enable = config->s0ix_enable;
	mcfg->iommu_support = is_devfn_enabled(IOMMU_DEVFN);

	/* voltage regulator telemetry settings */
	mcfg->telemetry_vddcrvddfull_scale_current =
		config->telemetry_vddcrvddfull_scale_current_mA;
	mcfg->telemetry_vddcrvddoffset =
		config->telemetry_vddcrvddoffset;
	mcfg->telemetry_vddcrsocfull_scale_current =
		config->telemetry_vddcrsocfull_scale_current_mA;
	mcfg->telemetry_vddcrsocOffset =
		config->telemetry_vddcrsocoffset;

	/* PCIe power vs. speed */
	mcfg->pspp_policy = config->pspp_policy;

	mcfg->enable_nb_azalia = devtree_gfx_hda_dev_enabled();
	mcfg->hda_enable = devtree_hda_dev_enabled();
	mcfg->sata_enable = devtree_sata_dev_enabled();

	if (config->usb_phy_custom) {
		mcfg->usb_phy = (struct usb_phy_config *)&config->usb_phy;
		mcfg->usb_phy->Version_Major = 0xd;
		mcfg->usb_phy->Version_Minor = 0x6;
		mcfg->usb_phy->TableLength = 100;
	}
	else
		mcfg->usb_phy = NULL;

	fsp_fill_pcie_ddi_descriptors(mcfg);
	fsp_assign_ioapic_upds(mcfg);
	mb_pre_fspm();
}
