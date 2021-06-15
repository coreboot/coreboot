/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <fsp/util.h>
#include <intelblocks/lpss.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/tcss.h>
#include <soc/gpio_soc_defs.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>
#include <string.h>

/* THC assignment definition */
#define THC_NONE	0
#define THC_0		1
#define THC_1		2

/* SATA DEVSLP idle timeout default values */
#define DEF_DMVAL	15
#define DEF_DITOVAL	625

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

static void parse_devicetree(FSP_S_CONFIG *s_cfg)
{
	const struct soc_intel_alderlake_config *config;
	config = config_of_soc();

	for (int i = 0; i < CONFIG_SOC_INTEL_I2C_DEV_MAX; i++)
		s_cfg->SerialIoI2cMode[i] = config->SerialIoI2cMode[i];

	for (int i = 0; i < CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX; i++) {
		s_cfg->SerialIoSpiMode[i] = config->SerialIoGSpiMode[i];
		s_cfg->SerialIoSpiCsMode[i] = config->SerialIoGSpiCsMode[i];
		s_cfg->SerialIoSpiCsState[i] = config->SerialIoGSpiCsState[i];
	}

	for (int i = 0; i < CONFIG_SOC_INTEL_UART_DEV_MAX; i++)
		s_cfg->SerialIoUartMode[i] = config->SerialIoUartMode[i];
}

__weak void mainboard_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	/* Override settings per board. */
}

static void soc_silicon_init_params(FSP_S_CONFIG *s_cfg,
		struct soc_intel_alderlake_config *config)
{
	int i;
	const struct microcode *microcode_file;
	size_t microcode_len;
	uint32_t enable_mask;

	mainboard_update_soc_chip_config(config);

	/* Parse device tree and enable/disable Serial I/O devices */
	parse_devicetree(s_cfg);

	microcode_file = cbfs_map("cpu_microcode_blob.bin", &microcode_len);

	if ((microcode_file != NULL) && (microcode_len != 0)) {
		/* Update CPU Microcode patch base address/size */
		s_cfg->MicrocodeRegionBase = (uint32_t)microcode_file;
		s_cfg->MicrocodeRegionSize = (uint32_t)microcode_len;
	}

	/* Load VBT before devicetree-specific config. */
	s_cfg->GraphicsConfigPtr = (uintptr_t)vbt_get();

	/* Check if IGD is present and fill Graphics init param accordingly */
	s_cfg->PeiGraphicsPeimInit = CONFIG(RUN_FSP_GOP) && is_devfn_enabled(SA_DEVFN_IGD);
	s_cfg->LidStatus = CONFIG(RUN_FSP_GOP);

	/* Use coreboot MP PPI services if Kconfig is enabled */
	if (CONFIG(USE_INTEL_FSP_TO_CALL_COREBOOT_PUBLISH_MP_PPI))
		s_cfg->CpuMpPpi = (uintptr_t) mp_fill_ppi_services_data();

	/* D3Hot and D3Cold for TCSS */
	s_cfg->D3HotEnable = !config->TcssD3HotDisable;
	s_cfg->D3ColdEnable = !config->TcssD3ColdDisable;

	s_cfg->TcssAuxOri = config->TcssAuxOri;

	/* Explicitly clear this field to avoid using defaults */
	memset(s_cfg->IomTypeCPortPadCfg, 0, sizeof(s_cfg->IomTypeCPortPadCfg));

	/*
	 * Set FSPS UPD ITbtConnectTopologyTimeoutInMs with value 0. FSP will
	 * evaluate this UPD value and skip sending command. There will be no
	 * delay for command completion.
	 */
	s_cfg->ITbtConnectTopologyTimeoutInMs = 0;

	/* Chipset Lockdown */
	if (get_lockdown_config() == CHIPSET_LOCKDOWN_COREBOOT) {
		s_cfg->PchLockDownGlobalSmi = 0;
		s_cfg->PchLockDownBiosInterface = 0;
		s_cfg->PchUnlockGpioPads = 1;
		s_cfg->RtcMemoryLock = 0;
	} else {
		s_cfg->PchLockDownGlobalSmi = 1;
		s_cfg->PchLockDownBiosInterface = 1;
		s_cfg->PchUnlockGpioPads = 0;
		s_cfg->RtcMemoryLock = 1;
	}

	/* USB */
	for (i = 0; i < ARRAY_SIZE(config->usb2_ports); i++) {
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

	for (i = 0; i < ARRAY_SIZE(config->usb3_ports); i++) {
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

	for (i = 0; i < ARRAY_SIZE(config->tcss_ports); i++) {
		if (config->tcss_ports[i].enable)
			s_cfg->CpuUsb3OverCurrentPin[i] = config->tcss_ports[i].ocpin;
	}

	/* Enable xDCI controller if enabled in devicetree and allowed */
	if (!xdci_can_enable())
		devfn_disable(pci_root_bus(), PCH_DEVFN_USBOTG);
	s_cfg->XdciEnable = is_devfn_enabled(PCH_DEVFN_USBOTG);

	/* PCH UART selection for FSP Debug */
	s_cfg->SerialIoDebugUartNumber = CONFIG_UART_FOR_CONSOLE;
	ASSERT(ARRAY_SIZE(s_cfg->SerialIoUartAutoFlow) > CONFIG_UART_FOR_CONSOLE);
	s_cfg->SerialIoUartAutoFlow[CONFIG_UART_FOR_CONSOLE] = 0;

	/* SATA */
	s_cfg->SataEnable = is_devfn_enabled(PCH_DEVFN_SATA);
	if (s_cfg->SataEnable) {
		s_cfg->SataMode = config->SataMode;
		s_cfg->SataSalpSupport = config->SataSalpSupport;
		memcpy(s_cfg->SataPortsEnable, config->SataPortsEnable,
			sizeof(s_cfg->SataPortsEnable));
		memcpy(s_cfg->SataPortsDevSlp, config->SataPortsDevSlp,
			sizeof(s_cfg->SataPortsDevSlp));
	}

	/*
	 * Power Optimizer for DMI and SATA.
	 * DmiPwrOptimizeDisable and SataPwrOptimizeDisable is default to 0.
	 * Boards not needing the optimizers explicitly disables them by setting
	 * these disable variables to 1 in devicetree overrides.
	 */
	s_cfg->PchPwrOptEnable = !(config->DmiPwrOptimizeDisable);
	s_cfg->SataPwrOptEnable = !(config->SataPwrOptimizeDisable);

	/*
	 *  Enable DEVSLP Idle Timeout settings DmVal and DitoVal.
	 *  SataPortsDmVal is the DITO multiplier. Default is 15.
	 *  SataPortsDitoVal is the DEVSLP Idle Timeout (DITO), Default is 625ms.
	 *  The default values can be changed from devicetree.
	 */
	for (i = 0; i < ARRAY_SIZE(config->SataPortsEnableDitoConfig); i++) {
		if (config->SataPortsEnableDitoConfig[i]) {
			s_cfg->SataPortsDmVal[i] = config->SataPortsDmVal[i];
			s_cfg->SataPortsDitoVal[i] = config->SataPortsDitoVal[i];
		}
	}

	/* Enable TCPU for processor thermal control */
	s_cfg->Device4Enable = is_devfn_enabled(SA_DEVFN_DPTF);

	/* Set TccActivationOffset */
	s_cfg->TccActivationOffset = config->tcc_offset;

	/* LAN */
	s_cfg->PchLanEnable = is_devfn_enabled(PCH_DEVFN_GBE);

	/* CNVi */
	s_cfg->CnviMode = is_devfn_enabled(PCH_DEVFN_CNVI_WIFI);
	s_cfg->CnviBtCore = config->CnviBtCore;
	s_cfg->CnviBtAudioOffload = config->CnviBtAudioOffload;
	/* Assert if CNVi BT is enabled without CNVi being enabled. */
	assert(s_cfg->CnviMode || !s_cfg->CnviBtCore);
	/* Assert if CNVi BT offload is enabled without CNVi BT being enabled. */
	assert(s_cfg->CnviBtCore || !s_cfg->CnviBtAudioOffload);

	/* VMD */
	s_cfg->VmdEnable = is_devfn_enabled(SA_DEVFN_VMD);

	/* THC */
	s_cfg->ThcPort0Assignment = is_devfn_enabled(PCH_DEVFN_THC0) ? THC_0 : THC_NONE;
	s_cfg->ThcPort1Assignment = is_devfn_enabled(PCH_DEVFN_THC1) ? THC_1 : THC_NONE;

	/* USB4/TBT */
	for (i = 0; i < ARRAY_SIZE(s_cfg->ITbtPcieRootPortEn); i++)
		s_cfg->ITbtPcieRootPortEn[i] = is_devfn_enabled(SA_DEVFN_TBT(i));

	/* Legacy 8254 timer support */
	s_cfg->Enable8254ClockGating = !CONFIG(USE_LEGACY_8254_TIMER);
	s_cfg->Enable8254ClockGatingOnS3 = !CONFIG(USE_LEGACY_8254_TIMER);

	/* Enable Hybrid storage auto detection */
	s_cfg->HybridStorageMode = config->HybridStorageMode;

	enable_mask = pcie_rp_enable_mask(get_pch_pcie_rp_table());
	for (i = 0; i < CONFIG_MAX_PCH_ROOT_PORTS; i++) {
		if (!(enable_mask & BIT(i)))
			continue;
		const struct pcie_rp_config *rp_cfg = &config->pch_pcie_rp[i];
		s_cfg->PcieRpL1Substates[i] =
				get_l1_substate_control(rp_cfg->PcieRpL1Substates);
		s_cfg->PcieRpLtrEnable[i] = !!(rp_cfg->flags & PCIE_RP_LTR);
		s_cfg->PcieRpAdvancedErrorReporting[i] = !!(rp_cfg->flags & PCIE_RP_AER);
		s_cfg->PcieRpHotPlug[i] = !!(rp_cfg->flags & PCIE_RP_HOTPLUG);
		s_cfg->PcieRpClkReqDetect[i] = !!(rp_cfg->flags & PCIE_RP_CLK_REQ_DETECT);
	}

	s_cfg->PmSupport = 1;
	s_cfg->Hwp = 1;
	s_cfg->Cx = 1;
	s_cfg->PsOnEnable = 1;
}

static void arch_silicon_init_params(FSPS_ARCH_UPD *s_arch_cfg)
{
	/* EnableMultiPhaseSiliconInit for running MultiPhaseSiInit */
	s_arch_cfg->EnableMultiPhaseSiliconInit = 1;
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	struct soc_intel_alderlake_config *config;
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
