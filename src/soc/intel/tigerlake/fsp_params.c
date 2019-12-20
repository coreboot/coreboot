/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <fsp/util.h>
#include <intelblocks/cse.h>
#include <intelblocks/lpss.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <security/vboot/vboot_common.h>
#include <soc/early_tcss.h>
#include <soc/gpio_soc_defs.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
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

static void parse_devicetree(FSP_S_CONFIG *params)
{
	const struct soc_intel_tigerlake_config *config;
	config = config_of_soc();

	for (int i = 0; i < CONFIG_SOC_INTEL_I2C_DEV_MAX; i++)
		params->SerialIoI2cMode[i] = config->SerialIoI2cMode[i];

	for (int i = 0; i < CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX; i++) {
		params->SerialIoSpiMode[i] = config->SerialIoGSpiMode[i];
		params->SerialIoSpiCsMode[i] = config->SerialIoGSpiCsMode[i];
		params->SerialIoSpiCsState[i] = config->SerialIoGSpiCsState[i];
	}

	for (int i = 0; i < CONFIG_SOC_INTEL_UART_DEV_MAX; i++)
		params->SerialIoUartMode[i] = config->SerialIoUartMode[i];
}

static const pci_devfn_t serial_io_dev[] = {
	PCH_DEVFN_I2C0,
	PCH_DEVFN_I2C1,
	PCH_DEVFN_I2C2,
	PCH_DEVFN_I2C3,
	PCH_DEVFN_I2C4,
	PCH_DEVFN_I2C5,
	PCH_DEVFN_GSPI0,
	PCH_DEVFN_GSPI1,
	PCH_DEVFN_GSPI2,
	PCH_DEVFN_GSPI3,
	PCH_DEVFN_UART0,
	PCH_DEVFN_UART1,
	PCH_DEVFN_UART2
};

__weak void mainboard_update_soc_chip_config(struct soc_intel_tigerlake_config *config)
{
	/* Override settings per board. */
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	int i;
	uint32_t cpu_id;
	FSP_S_CONFIG *params = &supd->FspsConfig;

	struct device *dev;
	struct soc_intel_tigerlake_config *config;
	config = config_of_soc();
	mainboard_update_soc_chip_config(config);

	/* Parse device tree and enable/disable Serial I/O devices */
	parse_devicetree(params);

	/* Load VBT before devicetree-specific config. */
	params->GraphicsConfigPtr = (uintptr_t)vbt_get();

	/* Check if IGD is present and fill Graphics init param accordingly */
	dev = pcidev_path_on_root(SA_DEVFN_IGD);
	params->PeiGraphicsPeimInit = CONFIG(RUN_FSP_GOP) && is_dev_enabled(dev);

	/* Use coreboot MP PPI services if Kconfig is enabled */
	if (CONFIG(USE_INTEL_FSP_TO_CALL_COREBOOT_PUBLISH_MP_PPI))
		params->CpuMpPpi = (uintptr_t) mp_fill_ppi_services_data();

	/* D3Hot and D3Cold for TCSS */
	params->D3HotEnable = !config->TcssD3HotDisable;
	cpu_id = cpu_get_cpuid();
	if (cpu_id == CPUID_TIGERLAKE_A0)
		params->D3ColdEnable = 0;
	else
		params->D3ColdEnable = !config->TcssD3ColdDisable;

	params->TcssAuxOri = config->TcssAuxOri;
	for (i = 0; i < 8; i++)
		params->IomTypeCPortPadCfg[i] = config->IomTypeCPortPadCfg[i];

	/*
	 * Set FSPS UPD ITbtConnectTopologyTimeoutInMs with value 0. FSP will
	 * evaluate this UPD value and skip sending command. There will be no
	 * delay for command completion.
	 */
	params->ITbtConnectTopologyTimeoutInMs = 0;

	/* Chipset Lockdown */
	if (get_lockdown_config() == CHIPSET_LOCKDOWN_COREBOOT) {
		params->PchLockDownGlobalSmi = 0;
		params->PchLockDownBiosInterface = 0;
		params->PchUnlockGpioPads = 1;
		params->RtcMemoryLock = 0;
	} else {
		params->PchLockDownGlobalSmi = 1;
		params->PchLockDownBiosInterface = 1;
		params->PchUnlockGpioPads = 0;
		params->RtcMemoryLock = 1;
	}

	/* USB */
	for (i = 0; i < ARRAY_SIZE(config->usb2_ports); i++) {
		params->PortUsb20Enable[i] = config->usb2_ports[i].enable;
		params->Usb2PhyPetxiset[i] = config->usb2_ports[i].pre_emp_bias;
		params->Usb2PhyTxiset[i] = config->usb2_ports[i].tx_bias;
		params->Usb2PhyPredeemp[i] = config->usb2_ports[i].tx_emp_enable;
		params->Usb2PhyPehalfbit[i] = config->usb2_ports[i].pre_emp_bit;

		if (config->usb2_ports[i].enable)
			params->Usb2OverCurrentPin[i] = config->usb2_ports[i].ocpin;
		else
			params->Usb2OverCurrentPin[i] = 0xff;
	}

	for (i = 0; i < ARRAY_SIZE(config->usb3_ports); i++) {
		params->PortUsb30Enable[i] = config->usb3_ports[i].enable;
		if (config->usb3_ports[i].enable) {
			params->Usb3OverCurrentPin[i] = config->usb3_ports[i].ocpin;
		} else {
			params->Usb3OverCurrentPin[i] = 0xff;
		}
		if (config->usb3_ports[i].tx_de_emp) {
			params->Usb3HsioTxDeEmphEnable[i] = 1;
			params->Usb3HsioTxDeEmph[i] = config->usb3_ports[i].tx_de_emp;
		}
		if (config->usb3_ports[i].tx_downscale_amp) {
			params->Usb3HsioTxDownscaleAmpEnable[i] = 1;
			params->Usb3HsioTxDownscaleAmp[i] =
				config->usb3_ports[i].tx_downscale_amp;
		}
	}

	/* RP Configs */
	for (i = 0; i < CONFIG_MAX_ROOT_PORTS; i++) {
		params->PcieRpL1Substates[i] =
			get_l1_substate_control(config->PcieRpL1Substates[i]);
		params->PcieRpLtrEnable[i] = config->PcieRpLtrEnable[i];
		params->PcieRpAdvancedErrorReporting[i] =
			config->PcieRpAdvancedErrorReporting[i];
		params->PcieRpHotPlug[i] = config->PcieRpHotPlug[i];
	}

	/* Enable ClkReqDetect for enabled port */
	memcpy(params->PcieRpClkReqDetect, config->PcieRpClkReqDetect,
		sizeof(config->PcieRpClkReqDetect));

	/* Enable xDCI controller if enabled in devicetree and allowed */
	dev = pcidev_path_on_root(PCH_DEVFN_USBOTG);
	if (dev) {
		if (!xdci_can_enable())
			dev->enabled = 0;
		params->XdciEnable = dev->enabled;
	} else {
		params->XdciEnable = 0;
	}

	/* PCH UART selection for FSP Debug */
	params->SerialIoDebugUartNumber = CONFIG_UART_FOR_CONSOLE;
	ASSERT(ARRAY_SIZE(params->SerialIoUartAutoFlow) > CONFIG_UART_FOR_CONSOLE);
	params->SerialIoUartAutoFlow[CONFIG_UART_FOR_CONSOLE] = 0;

	/* SATA */
	dev = pcidev_path_on_root(PCH_DEVFN_SATA);
	params->SataEnable = is_dev_enabled(dev);
	if (params->SataEnable) {
		params->SataMode = config->SataMode;
		params->SataSalpSupport = config->SataSalpSupport;
		memcpy(params->SataPortsEnable, config->SataPortsEnable,
			sizeof(params->SataPortsEnable));
		memcpy(params->SataPortsDevSlp, config->SataPortsDevSlp,
			sizeof(params->SataPortsDevSlp));
	}

	/* S0iX: Selectively enable individual sub-states,
	 * by default all are enabled.
	 *
	 * LPM0-s0i2.0, LPM1-s0i2.1, LPM2-s0i2.2, LPM3-s0i3.0,
	 * LPM4-s0i3.1, LPM5-s0i3.2, LPM6-s0i3.3, LPM7-s0i3.4
	 */
	params->LpmStateEnableMask = LPM_S0iX_ALL & ~config->LpmStateDisableMask;

	/*
	 * Power Optimizer for DMI and SATA.
	 * DmiPwrOptimizeDisable and SataPwrOptimizeDisable is default to 0.
	 * Boards not needing the optimizers explicitly disables them by setting
	 * these disable variables to 1 in devicetree overrides.
	 */
	params->PchPwrOptEnable = !(config->DmiPwrOptimizeDisable);
	params->SataPwrOptEnable = !(config->SataPwrOptimizeDisable);

	/*
	 *  Enable DEVSLP Idle Timeout settings DmVal and DitoVal.
	 *  SataPortsDmVal is the DITO multiplier. Default is 15.
	 *  SataPortsDitoVal is the DEVSLP Idle Timeout (DITO), Default is 625ms.
	 *  The default values can be changed from devicetree.
	 */
	for (i = 0; i < ARRAY_SIZE(config->SataPortsEnableDitoConfig); i++) {
		if (config->SataPortsEnableDitoConfig[i]) {
			if (config->SataPortsDmVal[i])
				params->SataPortsDmVal[i] = config->SataPortsDmVal[i];
			else
				params->SataPortsDmVal[i] = DEF_DMVAL;

			if (config->SataPortsDitoVal[i])
				params->SataPortsDitoVal[i] = config->SataPortsDitoVal[i];
			else
				params->SataPortsDitoVal[i] = DEF_DITOVAL;
		}
	}

	params->AcousticNoiseMitigation = config->AcousticNoiseMitigation;
	memcpy(&params->SlowSlewRate, &config->SlowSlewRate,
		ARRAY_SIZE(config->SlowSlewRate) * sizeof(config->SlowSlewRate[0]));

	memcpy(&params->FastPkgCRampDisable, &config->FastPkgCRampDisable,
		ARRAY_SIZE(config->FastPkgCRampDisable) *
			sizeof(config->FastPkgCRampDisable[0]));

	/* Enable TCPU for processor thermal control */
	params->Device4Enable = config->Device4Enable;

	/* Set TccActivationOffset */
	params->TccActivationOffset = config->tcc_offset;

	/* LAN */
	dev = pcidev_path_on_root(PCH_DEVFN_GBE);
	params->PchLanEnable = is_dev_enabled(dev);

	/* CNVi */
	dev = pcidev_path_on_root(PCH_DEVFN_CNVI_WIFI);
	params->CnviMode = is_dev_enabled(dev);

	/* CNVi BT Core */
	dev = pcidev_path_on_root(PCH_DEVFN_CNVI_BT);
	params->CnviBtCore = is_dev_enabled(dev);

	/* CNVi BT Audio Offload */
	params->CnviBtAudioOffload = config->CnviBtAudioOffload;

	/* VMD */
	dev = pcidev_path_on_root(SA_DEVFN_VMD);
	params->VmdEnable = is_dev_enabled(dev);

	/* THC */
	dev = pcidev_path_on_root(PCH_DEVFN_THC0);
	params->ThcPort0Assignment = is_dev_enabled(dev) ? THC_0 : THC_NONE;

	dev =  pcidev_path_on_root(PCH_DEVFN_THC1);
	params->ThcPort1Assignment = is_dev_enabled(dev) ? THC_1 : THC_NONE;

	/* Legacy 8254 timer support */
	params->Enable8254ClockGating = !CONFIG(USE_LEGACY_8254_TIMER);
	params->Enable8254ClockGatingOnS3 = !CONFIG(USE_LEGACY_8254_TIMER);

	/* Enable Hybrid storage auto detection */
	if (CONFIG(SOC_INTEL_CSE_LITE_SKU) && cse_is_hfs3_fw_sku_lite()
		&& vboot_recovery_mode_enabled() && !cse_is_hfs1_com_normal()) {
		/*
		 * CSE Lite SKU does not support hybrid storage dynamic configuration
		 * in CSE RO boot, and FSP does not allow to send the strap override
		 * HECI commands if CSE is not in normal mode; hence, hybrid storage
		 * mode is disabled on CSE RO boot in recovery boot mode.
		 */
		printk(BIOS_INFO, "cse_lite: CSE RO boot. HybridStorageMode disabled\n");
		params->HybridStorageMode = 0;
	} else {
		params->HybridStorageMode = config->HybridStorageMode;
	}

	/* USB4/TBT */
	for (i = 0; i < ARRAY_SIZE(params->ITbtPcieRootPortEn); i++) {
		dev = pcidev_on_root(SA_DEV_SLOT_TBT, i);
		if (dev)
			params->ITbtPcieRootPortEn[i] = dev->enabled;
		else
			params->ITbtPcieRootPortEn[i] = 0;
	}

	/* PCH FIVR settings override */
	if (config->ext_fivr_settings.configure_ext_fivr) {
		params->PchFivrExtV1p05RailEnabledStates =
			config->ext_fivr_settings.v1p05_enable_bitmap;

		params->PchFivrExtV1p05RailSupportedVoltageStates =
			config->ext_fivr_settings.v1p05_supported_voltage_bitmap;

		params->PchFivrExtVnnRailEnabledStates =
			config->ext_fivr_settings.vnn_enable_bitmap;

		params->PchFivrExtVnnRailSupportedVoltageStates =
			config->ext_fivr_settings.vnn_supported_voltage_bitmap;

		/* convert mV to number of 2.5 mV increments */
		params->PchFivrExtVnnRailSxVoltage =
			(config->ext_fivr_settings.vnn_sx_voltage_mv * 10) / 25;

		params->PchFivrExtV1p05RailIccMaximum =
			config->ext_fivr_settings.v1p05_icc_max_ma;

	}

	/* Apply minimum assertion width settings if non-zero */
	if (config->PchPmSlpS3MinAssert)
		params->PchPmSlpS3MinAssert = config->PchPmSlpS3MinAssert;
	if (config->PchPmSlpS4MinAssert)
		params->PchPmSlpS4MinAssert = config->PchPmSlpS4MinAssert;
	if (config->PchPmSlpSusMinAssert)
		params->PchPmSlpSusMinAssert = config->PchPmSlpSusMinAssert;
	if (config->PchPmSlpAMinAssert)
		params->PchPmSlpAMinAssert = config->PchPmSlpAMinAssert;

	/* Set Power Cycle Duration */
	if (config->PchPmPwrCycDur)
		params->PchPmPwrCycDur = get_pm_pwr_cyc_dur(config->PchPmSlpS4MinAssert,
				config->PchPmSlpS3MinAssert, config->PchPmSlpAMinAssert,
				config->PchPmPwrCycDur);

	/* EnableMultiPhaseSiliconInit for running MultiPhaseSiInit */
	params->EnableMultiPhaseSiliconInit = 1;

	/* Disable C1 C-state Demotion */
	params->C1StateAutoDemotion = 0;

	mainboard_silicon_init_params(params);
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
		if (CONFIG(EARLY_TCSS_DISPLAY) && (vboot_recovery_mode_enabled() ||
			vboot_developer_mode_enabled()))
			mainboard_early_tcss_enable();
		break;
	default:
		break;
	}
}

/* Mainboard GPIO Configuration */
__weak void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

/* Return list of SOC LPSS controllers */
const pci_devfn_t *soc_lpss_controllers_list(size_t *size)
{
	*size = ARRAY_SIZE(serial_io_dev);
	return serial_io_dev;
}
