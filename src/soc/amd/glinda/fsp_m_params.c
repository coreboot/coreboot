/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Glinda */
/* TODO: See what can be moved to common */

#include <amdblocks/apob_cache.h>
#include <amdblocks/ioapic.h>
#include <amdblocks/memmap.h>
#include <assert.h>
#include <console/console.h>
#include <console/uart.h>
#include <device/device.h>
#include <fsp/api.h>
#include <soc/platform_descriptors.h>
#include <soc/pci_devs.h>
#include <string.h>
#include <types.h>
#include <vendorcode/amd/fsp/glinda/FspUsb.h>
#include "chip.h"

__weak void mb_pre_fspm(FSP_M_CONFIG *mcfg)
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
	const fsp_dxio_descriptor *fsp_dxio = NULL;
	const fsp_ddi_descriptor *fsp_ddi = NULL;
	size_t num_dxio = 0;
	size_t num_ddi = 0;

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
	const struct soc_amd_glinda_config *config = config_of_soc();

	mupd->FspmArchUpd.NvsBufferPtr = (uintptr_t)soc_fill_apob_cache();

	mcfg->pci_express_base_addr = CONFIG_ECAM_MMCONF_BASE_ADDRESS;
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

	mcfg->enable_nb_azalia = is_dev_enabled(DEV_PTR(gfx_hda));
	mcfg->hda_enable = is_dev_enabled(DEV_PTR(hda));

	if (config->usb_phy_custom) {
		/* devicetree config is const, use local copy */
		static struct usb_phy_config lcl_usb_phy;
		lcl_usb_phy = config->usb_phy;
		lcl_usb_phy.Version_Major = FSP_USB_STRUCT_MAJOR_VERSION;
		lcl_usb_phy.Version_Minor = FSP_USB_STRUCT_MINOR_VERSION;
		lcl_usb_phy.TableLength = sizeof(struct usb_phy_config);
		if ((uintptr_t)&lcl_usb_phy <= UINT32_MAX) {
			mcfg->usb_phy_ptr = (uint32_t)(uintptr_t)&lcl_usb_phy;
		} else {
			printk(BIOS_ERR, "USB PHY config struct above 4GB; can't pass USB PHY "
					 "configuration to 32 bit FSP.\n");
			mcfg->usb_phy_ptr = 0;
		}
	} else {
		mcfg->usb_phy_ptr = 0;
	}

	fsp_fill_pcie_ddi_descriptors(mcfg);
	fsp_assign_ioapic_upds(mcfg);
	mb_pre_fspm(mcfg);
}
