/* SPDX-License-Identifier: GPL-2.0-only */
#include <assert.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/device.h>
#include <fsp/api.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <fsp/util.h>
#include <option.h>
#include <intelblocks/lpss.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>
#include <types.h>

/* SATA DEVSLP idle timeout default values */
#define DEF_DMVAL		15
#define DEF_DITOVAL_MS		625

/* Native function controls pads termination */
#define GPIO_TERM_NATIVE	0x1F

/* PM related values */
/* Imon offset is defined in 1/1000 increments */
#define IMON_OFFSET	1
/* Policy Imon slope is defined in 1/100 increments */
#define IMON_SLOPE	100
/* Thermal Design Current current limit in 1/8A units */
#define TDC_CURRENT_LIMIT_MAX	112
/* AcLoadline in 1/100 mOhms */
#define AC_LOADLINE_LANE_0_MAX	112
#define AC_LOADLINE_LANE_1_MAX	3
/* DcLoadline in 1/100 mOhms */
#define DC_LOADLINE_LANE_0_MAX	92
#define DC_LOADLINE_LANE_1_MAX	3
/* VR Icc Max limit. 0-255A in 1/4 A units */
#define ICC_LIMIT_MAX	104
/* Core Ratio Limit: For overclocking part: LFM to Fused */
#define CORE_RATIO_LIMIT	0x13

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

static void fill_fsps_fivr_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_elkhartlake_config *config)
{
	s_cfg->PchFivrExtV1p05RailEnabledStates = config->fivr.v1p05_state;
	s_cfg->PchFivrExtV1p05RailSupportedVoltageStates = config->fivr.v1p05_rail;
	s_cfg->PchFivrExtVnnRailEnabledStates = config->fivr.vnn_state;
	s_cfg->PchFivrExtVnnRailSupportedVoltageStates = config->fivr.vnn_rail;
	s_cfg->PchFivrExtVnnRailSxEnabledStates = config->fivr.vnn_sx_state;
	s_cfg->PchFivrVccinAuxLowToHighCurModeVolTranTime = config->fivr.vcc_low_high_us;
	s_cfg->PchFivrVccinAuxRetToHighCurModeVolTranTime = config->fivr.vcc_ret_high_us;
	s_cfg->PchFivrVccinAuxRetToLowCurModeVolTranTime = config->fivr.vcc_ret_low_us;
	s_cfg->PchFivrVccinAuxOffToHighCurModeVolTranTime = config->fivr.vcc_off_high_us;
	/* Convert mV to number of 2.5 mV increments */
	s_cfg->PchFivrExtVnnRailSxVoltage = (config->fivr.vnn_sx_mv * 10) / 25;
	s_cfg->PchFivrExtV1p05RailIccMaximum = config->fivr.v1p05_icc_max_ma;
	s_cfg->FivrSpreadSpectrum = config->fivr.spread_spectrum;
}

static void fill_fsps_tsn_params(FSP_S_CONFIG *params,
		const struct soc_intel_elkhartlake_config *config)
{
	/*
	 * Currently EHL TSN GBE only supports link speed with 2 type of
	 * PCH XTAL frequency: 24 MHz and 38.4 MHz.
	 * These are the config values for PchTsnGbeLinkSpeed in FSP-S UPD:
	 * 0: 24MHz 2.5Gbps, 1: 24MHz 1Gbps, 2: 38.4MHz 2.5Gbps,
	 * 3: 38.4MHz 1Gbps
	 */
	int xtal_freq_enum = pmc_get_xtal_freq();
	if ((xtal_freq_enum != XTAL_24_MHZ) && (xtal_freq_enum != XTAL_38_4_MHZ)) {
		printk(BIOS_ERR, "XTAL not supported. Disabling All TSN GBE ports.\n");
		params->PchTsnEnable = 0;
		params->PchPseGbeEnable[0] = 0;
		params->PchPseGbeEnable[1] = 0;
		devfn_disable(pci_root_bus(), PCH_DEVFN_GBE);
		devfn_disable(pci_root_bus(), PCH_DEVFN_PSEGBE0);
		devfn_disable(pci_root_bus(), PCH_DEVFN_PSEGBE1);
	}
	/*
	 * PCH TSN settings:
	 * Due to EHL GBE comes with time sensitive networking (TSN)
	 * capability integrated, EHL FSP is using PchTsnEnable instead of
	 * usual PchLanEnable flag for GBE control. Hence, force
	 * PchLanEnable to disable to avoid it being used in the future.
	 */
	params->PchLanEnable = 0x0;
	params->PchTsnEnable = is_devfn_enabled(PCH_DEVFN_GBE);
	if (params->PchTsnEnable) {
		params->PchTsnGbeSgmiiEnable = config->PchTsnGbeSgmiiEnable;
		params->PchTsnGbeMultiVcEnable = config->PchTsnGbeMultiVcEnable;
		params->PchTsnGbeLinkSpeed = (config->PchTsnGbeLinkSpeed) + xtal_freq_enum;
	}

	/* PSE TSN settings */
	if (!CONFIG(PSE_ENABLE))
		return;
	for (unsigned int i = 0; i < MAX_PSE_TSN_PORTS; i++) {
		switch (i) {
		case 0:
			params->PchPseGbeEnable[i] = is_devfn_enabled(PCH_DEVFN_PSEGBE0) ?
				Host_Owned : config->PseGbeOwn[0];
			break;
		case 1:
			params->PchPseGbeEnable[i] = is_devfn_enabled(PCH_DEVFN_PSEGBE1) ?
				Host_Owned : config->PseGbeOwn[i];
			break;
		default:
			break;
		}
		if (params->PchPseGbeEnable[i]) {
			params->PseTsnGbeMultiVcEnable[i] = config->PseTsnGbeMultiVcEnable[i];
			params->PseTsnGbeSgmiiEnable[i] = config->PseTsnGbeSgmiiEnable[i];
			params->PseTsnGbePhyInterfaceType[i] =
				!config->PseTsnGbeSgmiiEnable[i] ?
				RGMII : config->PseTsnGbePhyType[i];
			params->PseTsnGbeLinkSpeed[i] =
				(params->PseTsnGbePhyInterfaceType[i] < SGMII_plus) ?
				xtal_freq_enum + 1 : xtal_freq_enum;
		}
	}
}

static void fill_fsps_pse_params(FSP_S_CONFIG *params,
		const struct soc_intel_elkhartlake_config *config)
{
	static char psefwbuf[(CONFIG_PSE_FW_FILE_SIZE_KIB +
		CONFIG_PSE_CONFIG_BUFFER_SIZE_KIB) * KiB];
	uint32_t pse_fw_base;
	size_t psefwsize = cbfs_load("pse.bin", psefwbuf, sizeof(psefwbuf));
	if (psefwsize > 0) {
		pse_fw_base = (uintptr_t)&psefwbuf;
		params->SiipRegionBase = pse_fw_base;
		params->SiipRegionSize = psefwsize;
		printk(BIOS_DEBUG, "PSE base: %08x size: %08zx\n", pse_fw_base, psefwsize);

		/* Configure PSE peripherals */
		FSP_ARRAY_LOAD(params->PchPseDmaEnable, config->PseDmaOwn);
		FSP_ARRAY_LOAD(params->PchPseDmaSbInterruptEnable, config->PseDmaSbIntEn);
		FSP_ARRAY_LOAD(params->PchPseUartEnable, config->PseUartOwn);
		FSP_ARRAY_LOAD(params->PchPseUartSbInterruptEnable, config->PseUartSbIntEn);
		FSP_ARRAY_LOAD(params->PchPseHsuartEnable, config->PseHsuartOwn);
		FSP_ARRAY_LOAD(params->PchPseQepEnable, config->PseQepOwn);
		FSP_ARRAY_LOAD(params->PchPseQepSbInterruptEnable, config->PseQepSbIntEn);
		FSP_ARRAY_LOAD(params->PchPseI2cEnable, config->PseI2cOwn);
		FSP_ARRAY_LOAD(params->PchPseI2cSbInterruptEnable, config->PseI2cSbIntEn);
		FSP_ARRAY_LOAD(params->PchPseI2sEnable, config->PseI2sOwn);
		FSP_ARRAY_LOAD(params->PchPseI2sSbInterruptEnable, config->PseI2sSbIntEn);
		FSP_ARRAY_LOAD(params->PchPseSpiEnable, config->PseSpiOwn);
		FSP_ARRAY_LOAD(params->PchPseSpiSbInterruptEnable, config->PseSpiSbIntEn);
		FSP_ARRAY_LOAD(params->PchPseSpiCs0Enable, config->PseSpiCs0Own);
		FSP_ARRAY_LOAD(params->PchPseSpiCs1Enable, config->PseSpiCs1Own);
		FSP_ARRAY_LOAD(params->PchPseCanEnable, config->PseCanOwn);
		FSP_ARRAY_LOAD(params->PchPseCanSbInterruptEnable, config->PseCanSbIntEn);
		params->PchPsePwmEnable = config->PsePwmOwn;
		params->PchPsePwmSbInterruptEnable = config->PsePwmSbIntEn;
		FSP_ARRAY_LOAD(params->PchPsePwmPinEnable, config->PsePwmPinEn);
		params->PchPseAdcEnable = config->PseAdcOwn;
		params->PchPseAdcSbInterruptEnable = config->PseAdcSbIntEn;
		params->PchPseLh2PseSbInterruptEnable = config->PseLh2PseSbIntEn;
		params->PchPseShellEnabled = config->PseShellEn;

		/*
		 * As a minimum requirement for PSE initialization, the configuration
		 * of devices below are required as shown.
		 * TODO: Help needed to find a better way to handle this part of code
		 * as the settings from devicetree are overwritten here.
		 *
		 * Set the ownership of these devices to PSE. These are hardcoded for now,
		 * if the PSE should be opened one day (hopefully), this can be handled
		 * much better.
		 */
		params->PchPseDmaEnable[0] = PSE_Owned;
		params->PchPseUartEnable[2] = PSE_Owned;
		params->PchPseHsuartEnable[2] = PSE_Owned;
		params->PchPseI2cEnable[2] = PSE_Owned;
		params->PchPseTimedGpioEnable[0] = PSE_Owned;
		params->PchPseTimedGpioEnable[1] = PSE_Owned;
		/* Disable PSE DMA Sideband Interrupt for DMA 0 */
		params->PchPseDmaSbInterruptEnable[0] = 0;
		/* Set the log output to PSE UART 2 */
		params->PchPseLogOutputChannel = 3;
	} else {
		die("PSE enabled but PSE FW not available!\n");
	}
}

static void parse_devicetree(FSP_S_CONFIG *params)
{
	const struct soc_intel_elkhartlake_config *config = config_of_soc();

	/* LPSS controllers configuration */

	/* I2C */
	FSP_ARRAY_LOAD(params->SerialIoI2cMode, config->SerialIoI2cMode);
	FSP_ARRAY_LOAD(params->PchSerialIoI2cPadsTermination,
		config->SerialIoI2cPadsTermination);

	params->PchSerialIoI2cSclPinMux[4] = 0x1B44AC09;	//GPIO native mode for GPP_H9
	params->PchSerialIoI2cSdaPinMux[4] = 0x1B44CC08;	//GPIO native mode for GPP_H8

	/* GSPI */
	FSP_ARRAY_LOAD(params->SerialIoSpiMode, config->SerialIoGSpiMode);
	FSP_ARRAY_LOAD(params->SerialIoSpiCsEnable, config->SerialIoGSpiCsEnable);
	FSP_ARRAY_LOAD(params->SerialIoSpiCsMode, config->SerialIoGSpiCsMode);
	FSP_ARRAY_LOAD(params->SerialIoSpiCsState, config->SerialIoGSpiCsState);
	params->SerialIoSpiCsPolarity[2] = 0;

	/* UART */
	FSP_ARRAY_LOAD(params->SerialIoUartMode, config->SerialIoUartMode);
	FSP_ARRAY_LOAD(params->SerialIoUartDmaEnable, config->SerialIoUartDmaEnable);

	params->SerialIoUartCtsPinMuxPolicy[0] = 0x2B01320F;	//GPIO native mode for GPP_T15
	params->SerialIoUartRtsPinMuxPolicy[0] = 0x2B01220E;	//GPIO native mode for GPP_T14
	params->SerialIoUartRxPinMuxPolicy[0] = 0x2B01020C;	//GPIO native mode for GPP_T12
	params->SerialIoUartTxPinMuxPolicy[0] = 0x2B01120D;	//GPIO native mode for GPP_T13

	/* Provide correct UART number for FSP debug logs */
	params->SerialIoDebugUartNumber = CONFIG_UART_FOR_CONSOLE;
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	unsigned int i;

	FSP_S_CONFIG *params = &supd->FspsConfig;
	struct soc_intel_elkhartlake_config *config = config_of_soc();

	/* Parse device tree and fill in FSP UPDs */
	parse_devicetree(params);

	/* Load VBT before devicetree-specific config. */
	params->GraphicsConfigPtr = (uintptr_t)vbt_get();

	/* Check if IGD is present and fill Graphics init param accordingly */
	params->PeiGraphicsPeimInit = CONFIG(RUN_FSP_GOP) && is_devfn_enabled(SA_DEVFN_IGD);

	/* Display config */
	params->DdiPortAHpd = config->DdiPortAHpd;
	params->DdiPortADdc = config->DdiPortADdc;
	params->DdiPortCHpd = config->DdiPortCHpd;
	params->DdiPortCDdc = config->DdiPortCDdc;

	/* Intel Speed Step */
	params->Eist = config->eist_enable;

	/* Use coreboot MP PPI services if Kconfig is enabled */
	if (CONFIG(USE_INTEL_FSP_TO_CALL_COREBOOT_PUBLISH_MP_PPI))
		params->CpuMpPpi = (uintptr_t)mp_fill_ppi_services_data();

	/* Chipset Lockdown */
	if (get_lockdown_config() == CHIPSET_LOCKDOWN_COREBOOT) {
		params->PchLockDownGlobalSmi = 0;
		params->PchLockDownBiosLock = 0;
		params->PchLockDownBiosInterface = 0;
		params->PchWriteProtectionEnable[0] = 0;
		params->PchUnlockGpioPads = 1;
		params->RtcMemoryLock = 0;
		params->SkipPamLock = 1;
	} else {
		params->PchLockDownGlobalSmi = 1;
		params->PchLockDownBiosLock = 1;
		params->PchLockDownBiosInterface = 1;
		params->PchWriteProtectionEnable[0] = 1;
		params->PchUnlockGpioPads = 0;
		params->RtcMemoryLock = 1;
		params->SkipPamLock = 0;
	}

	/* Disable PAVP */
	params->PavpEnable = 0;

	/* Legacy 8254 timer support */
	bool use_8254 = get_uint_option("legacy_8254_timer", CONFIG(USE_LEGACY_8254_TIMER));
	params->Enable8254ClockGating = !use_8254;
	params->Enable8254ClockGatingOnS3 = 1;

	/*
	 * Legacy PM ACPI Timer (and TCO Timer)
	 * This *must* be 1 in any case to keep FSP from
	 *  1) enabling PM ACPI Timer emulation in uCode.
	 *  2) disabling the PM ACPI Timer.
	 * We handle both by ourself!
	 */
	params->EnableTcoTimer = 1;

	/* PCH Master Gating Control */
	params->PchPostMasterClockGating = 1;
	params->PchPostMasterPowerGating = 1;

	/* HECI */
	params->Heci3Enabled = config->Heci3Enable;

	/* USB configuration */
	for (i = 0; i < ARRAY_SIZE(config->usb2_ports); i++) {
		params->PortUsb20Enable[i] = config->usb2_ports[i].enable;
		params->Usb2PhyPetxiset[i] = config->usb2_ports[i].pre_emp_bias;
		params->Usb2PhyTxiset[i] = config->usb2_ports[i].tx_bias;
		params->Usb2PhyPredeemp[i] = config->usb2_ports[i].tx_emp_enable;
		params->Usb2PhyPehalfbit[i] = config->usb2_ports[i].pre_emp_bit;
		params->Usb2OverCurrentPin[i] = config->usb2_ports[i].enable ?
			config->usb2_ports[i].ocpin : 0xff;
	}

	for (i = 0; i < ARRAY_SIZE(config->usb3_ports); i++) {
		params->PortUsb30Enable[i] = config->usb3_ports[i].enable;
		params->Usb3OverCurrentPin[i] = config->usb3_ports[i].enable ?
			config->usb3_ports[i].ocpin : 0xff;
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

	params->UsbClockGatingEnable = 1;
	params->UsbPowerGatingEnable = 1;

	params->XdciEnable = xdci_can_enable(PCH_DEVFN_USBOTG);

	/* PCIe root ports config */
	for (i = 0; i < CONFIG_MAX_ROOT_PORTS; i++) {
		params->PcieRpClkReqDetect[i] =
			!config->PcieRpClkReqDetectDisable[i];
		params->PcieRpL1Substates[i] =
			get_l1_substate_control(config->PcieRpL1Substates[i]);
		params->PcieRpLtrEnable[i] = !config->PcieRpLtrDisable[i];
		params->PcieRpAdvancedErrorReporting[i] =
			!config->PcieRpAdvancedErrorReportingDisable[i];
		params->PcieRpHotPlug[i] = config->PcieRpHotPlug[i];
		params->PciePtm[i] = config->PciePtm[i];
		params->PcieRpLtrMaxNoSnoopLatency[i] = 0x1003;
		params->PcieRpLtrMaxSnoopLatency[i] = 0x1003;
		/* Virtual Channel 1 to Traffic Class mapping */
		params->PcieRpVc1TcMap[i] = 0x60;
	}

	/* SATA config */
	params->SataEnable = is_devfn_enabled(PCH_DEVFN_SATA);
	if (params->SataEnable) {
		params->SataMode = config->SataMode;
		params->SataSalpSupport = config->SataSalpSupport;
		params->SataPwrOptEnable = !(config->SataPwrOptimizeDisable);

		for (i = 0; i < CONFIG_MAX_SATA_PORTS; i++) {
			params->SataPortsEnable[i] = config->SataPortsEnable[i];
			params->SataPortsDevSlp[i] = config->SataPortsDevSlp[i];
			if (config->SataPortsEnableDitoConfig[i]) {
				params->SataPortsDmVal[i] =
					config->SataPortsDmVal[i] ? : DEF_DMVAL;
				params->SataPortsDitoVal[i] =
					config->SataPortsDitoVal[i] ? : DEF_DITOVAL_MS;
			}
		}
	}

	/* SDCard config */
	params->ScsSdCardEnabled = is_devfn_enabled(PCH_DEVFN_SDCARD);
	if (params->ScsSdCardEnabled) {
		params->SdCardPowerEnableActiveHigh = config->SdCardPowerEnableActiveHigh;
		params->SdCardGpioCmdPadTermination = GPIO_TERM_NATIVE;
		params->SdCardGpioDataPadTermination[0] = GPIO_TERM_NATIVE;
		params->SdCardGpioDataPadTermination[1] = GPIO_TERM_NATIVE;
		params->SdCardGpioDataPadTermination[2] = GPIO_TERM_NATIVE;
		params->SdCardGpioDataPadTermination[3] = GPIO_TERM_NATIVE;
	}

	/* eMMC config */
	params->ScsEmmcEnabled = is_devfn_enabled(PCH_DEVFN_EMMC);
	if (params->ScsEmmcEnabled) {
		params->ScsEmmcHs400Enabled = config->ScsEmmcHs400Enabled;
		params->ScsEmmcDdr50Enabled = config->ScsEmmcDdr50Enabled;
	}

	/* Thermal config */
	params->Device4Enable = is_devfn_enabled(SA_DEVFN_DPTF);
	params->ProcHotResponse = 0x0;	//Disable PROCHOT response
	/* Thermal sensor (TS) target width */
	params->DmiTS0TW = 3;
	params->DmiTS1TW = 2;
	params->DmiTS2TW = 1;
	/* Enable memory thermal throttling by default */
	if (!config->MemoryThermalThrottlingDisable) {
		params->PchMemoryPmsyncEnable[0] = 1;
		params->PchMemoryPmsyncEnable[1] = 1;
		params->PchMemoryC0TransmitEnable[0] = 1;
		params->PchMemoryC0TransmitEnable[1] = 1;
	}

	/* TccActivationOffset config */
	params->TccActivationOffset = config->tcc_offset;
	params->TccOffsetClamp = config->tcc_offset_clamp;
	params->TccOffsetLock = 0x1;	//lock Tcc Offset register

	/* Power management config */
	params->ImonSlope[0] = IMON_SLOPE;
	params->ImonOffset[0] = IMON_OFFSET;
	params->TdcCurrentLimit[0] = TDC_CURRENT_LIMIT_MAX;
	params->AcLoadline[0] = AC_LOADLINE_LANE_0_MAX;
	params->DcLoadline[0] = DC_LOADLINE_LANE_0_MAX;
	params->AcLoadline[1] = AC_LOADLINE_LANE_1_MAX;
	params->DcLoadline[1] = DC_LOADLINE_LANE_1_MAX;
	params->IccMax[0] = ICC_LIMIT_MAX;
	params->OneCoreRatioLimit = CORE_RATIO_LIMIT;
	params->TwoCoreRatioLimit = CORE_RATIO_LIMIT;
	params->ThreeCoreRatioLimit = CORE_RATIO_LIMIT;
	params->FourCoreRatioLimit = CORE_RATIO_LIMIT;
	params->FiveCoreRatioLimit = CORE_RATIO_LIMIT;
	params->SixCoreRatioLimit = CORE_RATIO_LIMIT;
	params->SevenCoreRatioLimit = CORE_RATIO_LIMIT;
	params->EightCoreRatioLimit = CORE_RATIO_LIMIT;
	params->PsysPmax = 0;	//Set max platform power to auto profile
	params->Custom1TurboActivationRatio = 0;
	params->Custom2TurboActivationRatio = 0;
	params->Custom3TurboActivationRatio = 0;
	params->PchPwrOptEnable = 0x1;	//Enable PCH DMI Power Optimizer
	params->TStates = 0x0;	//Disable T state
	params->PkgCStateLimit = 0x7;	//Set C state limit to C9
	params->FastPkgCRampDisable[0] = 0x1;
	params->SlowSlewRate[0] = 0x1;
	params->MaxRatio = 0x8;	//Set max P state ratio
	params->PchEspiLgmrEnable = 0;
	params->PchPmPwrBtnOverridePeriod = config->PchPmPwrBtnOverridePeriod;
	params->PchS0ixAutoDemotion = 0;
	params->PmcV1p05PhyExtFetControlEn = 0x1;
	params->PmcV1p05IsExtFetControlEn = 0x1;
	/* FIVR config */
	if (config->fivr.fivr_config_en) {
		fill_fsps_fivr_params(params, config);
	}

	/* FuSa (Functional Safety) config */
	if (!config->FuSaEnable) {
		params->DisplayFusaConfigEnable = 0;
		params->GraphicFusaConfigEnable = 0;
		params->OpioFusaConfigEnable = 0;
		params->PsfFusaConfigEnable = 0;
	}

	/* PSE (Intel Programmable Services Engine) config */
	if (CONFIG(PSE_ENABLE) && cbfs_file_exists("pse.bin"))
		fill_fsps_pse_params(params, config);

	/* TSN GBE config */
	fill_fsps_tsn_params(params, config);

	/* Override/Fill FSP Silicon Param for mainboard */
	mainboard_silicon_init_params(params);
}

/* Mainboard GPIO Configuration */
__weak void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
