/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cbfs.h>
#include <console/console.h>
#include <cpu/intel/cpu_ids.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/fsp_debug_event.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <fsp/util.h>
#include <intelblocks/cse.h>
#include <intelblocks/lpss.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <security/vboot/vboot_common.h>
#include <soc/cpu.h>
#include <soc/gpio_soc_defs.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>
#include <soc/soc_info.h>
#include <string.h>

/* THC assignment definition */
#define THC_NONE	0
#define THC_0		1
#define THC_1		2

/* SATA DEVSLP idle timeout default values */
#define DEF_DMVAL	15
#define DEF_DITOVAL	625

/*
 * ME End of Post configuration
 * 0 - Disable EOP.
 * 1 - Send in PEI (Applicable for FSP in API mode)
 * 2 - Send in DXE (Not applicable for FSP in API mode)
 */
enum fsp_end_of_post {
	EOP_DISABLE = 0,
	EOP_PEI = 1,
	EOP_DXE = 2,
};

static const pci_devfn_t i2c_dev[] = {
	PCI_DEVFN_I2C0,
	PCI_DEVFN_I2C1,
	PCI_DEVFN_I2C2,
	PCI_DEVFN_I2C3,
	PCI_DEVFN_I2C4,
	PCI_DEVFN_I2C5,
};

static const pci_devfn_t gspi_dev[] = {
	PCI_DEVFN_GSPI0,
	PCI_DEVFN_GSPI1,
	PCI_DEVFN_GSPI2,
};

static const pci_devfn_t uart_dev[] = {
	PCI_DEVFN_UART0,
	PCI_DEVFN_UART1,
	PCI_DEVFN_UART2
};

/*
 * Chip config parameter PcieRpL1Substates uses (UPD value + 1)
 * because UPD value of 0 for PcieRpL1Substates means disabled for FSP.
 * In order to ensure that mainboard setting does not disable L1 substates
 * incorrectly, chip config parameter values are offset by 1 with 0 meaning
 * use FSP UPD default. get_l1_substate_control() ensures that the right UPD
 * value is set in fsp_params.
 * 0: Use FSP UPD default
 * 1: Disable L1 substates
 * 2: Use L1.1
 * 3: Use L1.2 (FSP UPD default)
 */
static int get_l1_substate_control(enum L1_substates_control ctl)
{
	if ((ctl > L1_SS_L1_2) || (ctl == L1_SS_FSP_DEFAULT))
		ctl = L1_SS_L1_2;
	return ctl - 1;
}

__weak void mainboard_update_soc_chip_config(struct soc_intel_meteorlake_config *config)
{
	/* Override settings per board. */
}

static void fill_fsps_lpss_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	int max_port, i;

	max_port = get_max_i2c_port();
	for (i = 0; i < max_port; i++) {
		s_cfg->SerialIoI2cMode[i] = is_devfn_enabled(i2c_dev[i]) ?
			config->serial_io_i2c_mode[i] : 0;
	}

	max_port = get_max_gspi_port();
	for (i = 0; i < max_port; i++) {
		s_cfg->SerialIoSpiCsMode[i] = config->serial_io_gspi_cs_mode[i];
		s_cfg->SerialIoSpiCsState[i] = config->serial_io_gspi_cs_state[i];
		s_cfg->SerialIoSpiMode[i] = is_devfn_enabled(gspi_dev[i]) ?
			config->serial_io_gspi_mode[i] : 0;
	}

	max_port = get_max_uart_port();
	for (i = 0; i < max_port; i++) {
		s_cfg->SerialIoUartMode[i] = is_devfn_enabled(uart_dev[i]) ?
			config->serial_io_uart_mode[i] : 0;
	}
}

static void fill_fsps_cpu_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	const struct microcode *microcode_file;
	size_t microcode_len;

	/* Locate microcode and pass to FSP-S for 2nd microcode loading */
	microcode_file = cbfs_map("cpu_microcode_blob.bin", &microcode_len);

	if ((microcode_file) && (microcode_len != 0)) {
		/* Update CPU Microcode patch base address/size */
		s_cfg->MicrocodeRegionBase = (uint32_t)microcode_file;
		s_cfg->MicrocodeRegionSize = (uint32_t)microcode_len;
	}

	if (CONFIG(MTL_USE_FSP_MP_INIT)) {
		/*
		 * Use FSP running MP PPI services to perform CPU feature programming
		 * if Kconfig is enabled
		 */
		s_cfg->CpuMpPpi = (uintptr_t)mp_fill_ppi_services_data();
	} else {
		/* Use coreboot native driver to perform MP init by default */
		s_cfg->CpuMpPpi = (uintptr_t)NULL;
		/*
		 * FIXME: Bring back SkipMpInit UPD in MTL FSP to let coreboot perform
		 * AP programming independently.
		 */
		// s_cfg->SkipMpInit = !CONFIG(USE_INTEL_FSP_MP_INIT);
	}
}


static void fill_fsps_igd_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* Load VBT before devicetree-specific config. */
	s_cfg->GraphicsConfigPtr = (uintptr_t)vbt_get();

	/* Check if IGD is present and fill Graphics init param accordingly */
	s_cfg->PeiGraphicsPeimInit = CONFIG(RUN_FSP_GOP) && is_devfn_enabled(PCI_DEVFN_IGD);
	s_cfg->LidStatus = CONFIG(RUN_FSP_GOP);
	s_cfg->PavpEnable = CONFIG(PAVP);
}

static void fill_fsps_tcss_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	const struct device *tcss_port_arr[] = {
		DEV_PTR(tcss_usb3_port1),
		DEV_PTR(tcss_usb3_port2),
		DEV_PTR(tcss_usb3_port3),
		DEV_PTR(tcss_usb3_port4),
	};

	s_cfg->TcssAuxOri = config->tcss_aux_ori;

	/* Explicitly clear this field to avoid using defaults */
	memset(s_cfg->IomTypeCPortPadCfg, 0, sizeof(s_cfg->IomTypeCPortPadCfg));

	/* D3Hot and D3Cold for TCSS */
	s_cfg->D3HotEnable = !config->tcss_d3_hot_disable;
	s_cfg->D3ColdEnable = !config->tcss_d3_cold_disable;
	s_cfg->UsbTcPortEn = 0;

	for (int i = 0; i < MAX_TYPE_C_PORTS; i++) {
		if (is_dev_enabled(tcss_port_arr[i]))
			s_cfg->UsbTcPortEn |= BIT(i);
	}
}

static void fill_fsps_chipset_lockdown_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* Chipset Lockdown */
	const bool lockdown_by_fsp = get_lockdown_config() == CHIPSET_LOCKDOWN_FSP;
	s_cfg->PchLockDownGlobalSmi = lockdown_by_fsp;
	s_cfg->PchLockDownBiosInterface = lockdown_by_fsp;
	s_cfg->PchUnlockGpioPads = !lockdown_by_fsp;
	s_cfg->RtcMemoryLock = lockdown_by_fsp;
	s_cfg->SkipPamLock = !lockdown_by_fsp;

	/* coreboot will send EOP before loading payload */
	s_cfg->EndOfPostMessage = EOP_DISABLE;
}

static void fill_fsps_xhci_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	int i, max_port;

	max_port = get_max_usb20_port();
	for (i = 0; i < max_port; i++) {
		s_cfg->PortUsb20Enable[i] = config->usb2_ports[i].enable;
		s_cfg->Usb2PhyPetxiset[i] = config->usb2_ports[i].pre_emp_bias;
		s_cfg->Usb2PhyTxiset[i] = config->usb2_ports[i].tx_bias;
		s_cfg->Usb2PhyPredeemp[i] = config->usb2_ports[i].tx_emp_enable;
		s_cfg->Usb2PhyPehalfbit[i] = config->usb2_ports[i].pre_emp_bit;

		if (config->usb2_ports[i].enable)
			s_cfg->Usb2OverCurrentPin[i] = config->usb2_ports[i].ocpin;
		else
			s_cfg->Usb2OverCurrentPin[i] = OC_SKIP;
	}

	max_port = get_max_usb30_port();
	for (i = 0; i < max_port; i++) {
		s_cfg->PortUsb30Enable[i] = config->usb3_ports[i].enable;
		if (config->usb3_ports[i].enable)
			s_cfg->Usb3OverCurrentPin[i] = config->usb3_ports[i].ocpin;
		else
			s_cfg->Usb3OverCurrentPin[i] = OC_SKIP;

		if (config->usb3_ports[i].tx_de_emp) {
			s_cfg->Usb3HsioTxDeEmphEnable[i] = 1;
			s_cfg->Usb3HsioTxDeEmph[i] = config->usb3_ports[i].tx_de_emp;
		}
		if (config->usb3_ports[i].tx_downscale_amp) {
			s_cfg->Usb3HsioTxDownscaleAmpEnable[i] = 1;
			s_cfg->Usb3HsioTxDownscaleAmp[i] =
				config->usb3_ports[i].tx_downscale_amp;
		}
	}

	max_port = get_max_tcss_port();
	for (i = 0; i < max_port; i++) {
		s_cfg->PortUsb30Enable[i] = config->tcss_ports[i].enable;
		if (config->tcss_ports[i].enable)
			s_cfg->CpuUsb3OverCurrentPin[i] = config->tcss_ports[i].ocpin;
	}
}

static void fill_fsps_xdci_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	s_cfg->XdciEnable = xdci_can_enable(PCI_DEVFN_USBOTG);
}

static void fill_fsps_uart_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	ASSERT(ARRAY_SIZE(s_cfg->SerialIoUartAutoFlow) > CONFIG_UART_FOR_CONSOLE);
	s_cfg->SerialIoUartAutoFlow[CONFIG_UART_FOR_CONSOLE] = 0;
}

static void fill_fsps_sata_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* SATA */
	s_cfg->SataEnable = is_devfn_enabled(PCI_DEVFN_SATA);
	if (s_cfg->SataEnable) {
		s_cfg->SataMode = config->sata_mode;
		s_cfg->SataSalpSupport = config->sata_salp_support;
		memcpy(s_cfg->SataPortsEnable, config->sata_ports_enable,
			sizeof(s_cfg->SataPortsEnable));
		memcpy(s_cfg->SataPortsDevSlp, config->sata_ports_dev_slp,
			sizeof(s_cfg->SataPortsDevSlp));
	}

	/*
	 * Power Optimizer for SATA.
	 * SataPwrOptimizeDisable is default to 0.
	 * Boards not needing the optimizers explicitly disables them by setting
	 * these disable variables to 1 in devicetree overrides.
	 */
	s_cfg->SataPwrOptEnable = !(config->sata_pwr_optimize_disable);
	/*
	 *  Enable DEVSLP Idle Timeout settings DmVal and DitoVal.
	 *  SataPortsDmVal is the DITO multiplier. Default is 15.
	 *  SataPortsDitoVal is the DEVSLP Idle Timeout (DITO), Default is 625ms.
	 *  The default values can be changed from devicetree.
	 */
	for (size_t i = 0; i < ARRAY_SIZE(config->sata_ports_enable_dito_config); i++) {
		if (config->sata_ports_enable_dito_config[i]) {
			s_cfg->SataPortsDmVal[i] = config->sata_ports_dm_val[i];
			s_cfg->SataPortsDitoVal[i] = config->sata_ports_dito_val[i];
		}
	}
}

static void fill_fsps_thermal_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* Enable TCPU for processor thermal control */
	s_cfg->Device4Enable = is_devfn_enabled(PCI_DEVFN_DPTF);

	/* Set TccActivationOffset */
	s_cfg->TccActivationOffset = config->tcc_offset;
}

static void fill_fsps_lan_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* LAN */
	s_cfg->PchLanEnable = is_devfn_enabled(PCI_DEVFN_GBE);
}

static void fill_fsps_cnvi_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* CNVi */
	s_cfg->CnviMode = is_devfn_enabled(PCI_DEVFN_CNVI_WIFI);
	s_cfg->CnviBtCore = config->cnvi_bt_core;
	s_cfg->CnviBtAudioOffload = config->cnvi_bt_audio_offload;
	/* Assert if CNVi BT is enabled without CNVi being enabled. */
	assert(s_cfg->CnviMode || !s_cfg->CnviBtCore);
	/* Assert if CNVi BT offload is enabled without CNVi BT being enabled. */
	assert(s_cfg->CnviBtCore || !s_cfg->CnviBtAudioOffload);
}

static void fill_fsps_vmd_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* VMD */
	s_cfg->VmdEnable = is_devfn_enabled(PCI_DEVFN_VMD);
}

static void fill_fsps_tbt_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	for (int i = 0; i < ARRAY_SIZE(s_cfg->ITbtPcieRootPortEn); i++)
		s_cfg->ITbtPcieRootPortEn[i] = is_devfn_enabled(PCI_DEVFN_TBT(i));
}

static void fill_fsps_8254_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* Legacy 8254 timer support */
	s_cfg->Enable8254ClockGating = !CONFIG(USE_LEGACY_8254_TIMER);
	s_cfg->Enable8254ClockGatingOnS3 = !CONFIG(USE_LEGACY_8254_TIMER);
}

static void fill_fsps_pm_timer_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/*
	 * Legacy PM ACPI Timer (and TCO Timer)
	 * This *must* be 1 in any case to keep FSP from
	 *  1) enabling PM ACPI Timer emulation in uCode.
	 *  2) disabling the PM ACPI Timer.
	 * We handle both by ourself!
	 */
	s_cfg->EnableTcoTimer = 1;
}

static void fill_fsps_pcie_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	int max_port = get_max_pcie_port();
	uint32_t enable_mask = pcie_rp_enable_mask(get_pcie_rp_table());
	for (int i = 0; i < max_port; i++) {
		if (!(enable_mask & BIT(i)))
			continue;
		const struct pcie_rp_config *rp_cfg = &config->pcie_rp[i];
		s_cfg->PcieRpL1Substates[i] =
				get_l1_substate_control(rp_cfg->PcieRpL1Substates);
		s_cfg->PcieRpLtrEnable[i] = !!(rp_cfg->flags & PCIE_RP_LTR);
		s_cfg->PcieRpAdvancedErrorReporting[i] = !!(rp_cfg->flags & PCIE_RP_AER);
		s_cfg->PcieRpHotPlug[i] = !!(rp_cfg->flags & PCIE_RP_HOTPLUG);
		s_cfg->PcieRpClkReqDetect[i] = !!(rp_cfg->flags & PCIE_RP_CLK_REQ_DETECT);
	}
}

static void fill_fsps_misc_power_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* Skip setting D0I3 bit for all HECI devices */
	s_cfg->DisableD0I3SettingForHeci = 1;

	s_cfg->Hwp = 1;
	s_cfg->Cx = 1;
	s_cfg->PsOnEnable = 1;
	/* Enable the energy efficient turbo mode */
	s_cfg->EnergyEfficientTurbo = 1;
	s_cfg->PmcLpmS0ixSubStateEnableMask = get_supported_lpm_mask();
}


static void fill_fsps_ufs_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	s_cfg->UfsEnable[0] = is_devfn_enabled(PCI_DEVFN_UFS);
}

static void fill_fsps_ai_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	s_cfg->GnaEnable = is_devfn_enabled(PCI_DEVFN_GNA);
	s_cfg->VpuEnable = is_devfn_enabled(PCI_DEVFN_VPU);
}

static void arch_silicon_init_params(FSPS_ARCH_UPD *s_arch_cfg)
{
	/*
	 * EnableMultiPhaseSiliconInit for running MultiPhaseSiInit
	 */
	s_arch_cfg->EnableMultiPhaseSiliconInit = 1;

	/* Assign FspEventHandler arch Upd to use coreboot debug event handler */
	if (CONFIG(FSP_USES_CB_DEBUG_EVENT_HANDLER) && CONFIG(CONSOLE_SERIAL) &&
			 CONFIG(FSP_ENABLE_SERIAL_DEBUG))
		s_arch_cfg->FspEventHandler = (FSP_EVENT_HANDLER)
				fsp_debug_event_handler;
}

static void soc_silicon_init_params(FSP_S_CONFIG *s_cfg,
		struct soc_intel_meteorlake_config *config)
{
	/* Override settings per board if required. */
	mainboard_update_soc_chip_config(config);

	 void (*fill_fsps_params[])(FSP_S_CONFIG *s_cfg,
			const struct soc_intel_meteorlake_config *config) = {
		fill_fsps_lpss_params,
		fill_fsps_cpu_params,
		fill_fsps_igd_params,
		fill_fsps_tcss_params,
		fill_fsps_chipset_lockdown_params,
		fill_fsps_xhci_params,
		fill_fsps_xdci_params,
		fill_fsps_uart_params,
		fill_fsps_sata_params,
		fill_fsps_thermal_params,
		fill_fsps_lan_params,
		fill_fsps_cnvi_params,
		fill_fsps_vmd_params,
		fill_fsps_tbt_params,
		fill_fsps_8254_params,
		fill_fsps_pm_timer_params,
		fill_fsps_pcie_params,
		fill_fsps_misc_power_params,
		fill_fsps_ufs_params,
		fill_fsps_ai_params,
	};

	for (size_t i = 0; i < ARRAY_SIZE(fill_fsps_params); i++)
		fill_fsps_params[i](s_cfg, config);
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	struct soc_intel_meteorlake_config *config;
	FSP_S_CONFIG *s_cfg = &supd->FspsConfig;
	FSPS_ARCH_UPD *s_arch_cfg = &supd->FspsArchUpd;

	config = config_of_soc();
	arch_silicon_init_params(s_arch_cfg);
	soc_silicon_init_params(s_cfg, config);
	mainboard_silicon_init_params(s_cfg);
}

/*
 * Callbacks for SoC/Mainboard specific overrides for FspMultiPhaseSiInit
 * This platform supports below MultiPhaseSIInit Phase(s):
 * Phase   |  FSP return point                                |  Purpose
 * ------- + ------------------------------------------------ + -------------------------------
 *   1     |  After TCSS initialization completed             |  for TCSS specific init
 */
void platform_fsp_multi_phase_init_cb(uint32_t phase_index)
{
	switch (phase_index) {
	case 1:
		/* TCSS specific initialization here */
		printk(BIOS_DEBUG, "FSP MultiPhaseSiInit %s/%s called\n",
			__FILE__, __func__);

		if (CONFIG(SOC_INTEL_COMMON_BLOCK_TCSS)) {
			const config_t *config = config_of_soc();
			tcss_configure(config->typec_aux_bias_pads);
		}
		break;
	default:
		break;
	}
}

/* Mainboard GPIO Configuration */
__weak void mainboard_silicon_init_params(FSP_S_CONFIG *s_cfg)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
