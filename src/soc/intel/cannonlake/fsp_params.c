/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <intelblocks/lpss.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <string.h>

#include "chip.h"

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
	PCH_DEVFN_UART0,
	PCH_DEVFN_UART1,
	PCH_DEVFN_UART2
};

/*
 * Given an enum for PCH_SERIAL_IO_MODE, 1 needs to be subtracted to get the FSP
 * UPD expected value for Serial IO since valid enum index starts from 1.
 */
#define PCH_SERIAL_IO_INDEX(x)		((x) - 1)

static uint8_t get_param_value(const config_t *config, uint32_t dev_offset)
{
	struct device *dev;

	dev = pcidev_path_on_root(serial_io_dev[dev_offset]);
	if (!dev || !dev->enabled)
		return PCH_SERIAL_IO_INDEX(PchSerialIoDisabled);

	if ((config->SerialIoDevMode[dev_offset] >= PchSerialIoMax) ||
	    (config->SerialIoDevMode[dev_offset] == PchSerialIoNotInitialized))
		return PCH_SERIAL_IO_INDEX(PchSerialIoPci);

	/*
	 * Correct Enum index starts from 1, so subtract 1 while returning value
	 */
	return PCH_SERIAL_IO_INDEX(config->SerialIoDevMode[dev_offset]);
}

#if CONFIG(SOC_INTEL_COMETLAKE)
static void parse_devicetree_param(const config_t *config, FSP_S_CONFIG *params)
{
	uint32_t dev_offset = 0;
	uint32_t i = 0;

	for (i = 0; i < CONFIG_SOC_INTEL_I2C_DEV_MAX; i++, dev_offset++) {
		params->SerialIoI2cMode[i] =
				get_param_value(config, dev_offset);
	}

	for (i = 0; i < CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX; i++,
	     dev_offset++) {
		params->SerialIoSpiMode[i] =
				get_param_value(config, dev_offset);
	}

	for (i = 0; i < SOC_INTEL_CML_UART_DEV_MAX; i++, dev_offset++) {
		params->SerialIoUartMode[i] =
				get_param_value(config, dev_offset);
	}
}
#else
static void parse_devicetree_param(const config_t *config, FSP_S_CONFIG *params)
{
	for (int i = 0; i < ARRAY_SIZE(serial_io_dev); i++)
		params->SerialIoDevMode[i] = get_param_value(config, i);
}
#endif

static void parse_devicetree(FSP_S_CONFIG *params)
{
	const config_t *config = config_of_soc();

	parse_devicetree_param(config, params);
}

/* Ignore LTR value for GBE devices */
static void ignore_gbe_ltr(void)
{
	uint8_t reg8;
	uint8_t *pmcbase = pmc_mmio_regs();

	reg8 = read8(pmcbase + LTR_IGN);
	reg8 |= IGN_GBE;
	write8(pmcbase + LTR_IGN, reg8);
}

static void configure_gspi_cs(int idx, const config_t *config,
			      uint8_t *polarity, uint8_t *enable,
			      uint8_t *defaultcs)
{
	struct spi_cfg cfg;

	/* If speed_mhz is set, infer that the port should be configured */
	if (config->common_soc_config.gspi[idx].speed_mhz != 0) {
		if (gspi_get_soc_spi_cfg(idx, &cfg) == 0) {
			if (cfg.cs_polarity == SPI_POLARITY_LOW)
				*polarity = 0;
			else
				*polarity = 1;

			if (defaultcs != NULL)
				*defaultcs = 0;
			if (enable != NULL)
				*enable = 1;
		}
	}
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	int i;
	FSP_S_CONFIG *params = &supd->FspsConfig;
	FSP_S_TEST_CONFIG *tconfig = &supd->FspsTestConfig;
	struct device *dev;

	config_t *config = config_of_soc();

	/* Parse device tree and enable/disable devices */
	parse_devicetree(params);

	/* Load VBT before devicetree-specific config. */
	params->GraphicsConfigPtr = (uintptr_t)vbt_get();

	mainboard_silicon_init_params(params);

	const struct soc_power_limits_config *soc_config;
	soc_config = &config->power_limits_config;
	/* Set PsysPmax if it is available from DT */
	if (soc_config->psys_pmax) {
		printk(BIOS_DEBUG, "psys_pmax = %dW\n", soc_config->psys_pmax);
		/* PsysPmax is in unit of 1/8 Watt */
		tconfig->PsysPmax = soc_config->psys_pmax * 8;
	}

	/* Unlock upper 8 bytes of RTC RAM */
	params->PchLockDownRtcMemoryLock = 0;

	/* SATA */
	dev = pcidev_path_on_root(PCH_DEVFN_SATA);
	if (!dev)
		params->SataEnable = 0;
	else {
		params->SataEnable = dev->enabled;
		params->SataMode = config->SataMode;
		params->SataPwrOptEnable = config->satapwroptimize;
		params->SataSalpSupport = config->SataSalpSupport;
		memcpy(params->SataPortsEnable, config->SataPortsEnable,
			sizeof(params->SataPortsEnable));
		memcpy(params->SataPortsDevSlp, config->SataPortsDevSlp,
			sizeof(params->SataPortsDevSlp));
		memcpy(params->SataPortsHotPlug, config->SataPortsHotPlug,
			sizeof(params->SataPortsHotPlug));
#if CONFIG(SOC_INTEL_COMETLAKE)
		memcpy(params->SataPortsDevSlpResetConfig,
			config->SataPortsDevSlpResetConfig,
			sizeof(params->SataPortsDevSlpResetConfig));
#endif
	}
	params->SlpS0WithGbeSupport = 0;
	params->PchPmSlpS0VmRuntimeControl = config->PchPmSlpS0VmRuntimeControl;
	params->PchPmSlpS0Vm070VSupport = config->PchPmSlpS0Vm070VSupport;
	params->PchPmSlpS0Vm075VSupport = config->PchPmSlpS0Vm075VSupport;

	/* Lan */
	dev = pcidev_path_on_root(PCH_DEVFN_GBE);
	if (!dev)
		params->PchLanEnable = 0;
	else {
		params->PchLanEnable = dev->enabled;
		if (config->s0ix_enable && params->PchLanEnable) {
			/*
			 * The VmControl UPDs need to be set as per board
			 * design to allow voltage margining in S0ix to lower
			 * power consumption.
			 * But if GbE is enabled, voltage magining cannot be
			 * enabled, so the Vm control UPDs need to be set to 0.
			 */
			params->SlpS0WithGbeSupport = 1;
			params->PchPmSlpS0VmRuntimeControl = 0;
			params->PchPmSlpS0Vm070VSupport = 0;
			params->PchPmSlpS0Vm075VSupport = 0;
			ignore_gbe_ltr();
		}
	}

	/* Audio */
	params->PchHdaDspEnable = config->PchHdaDspEnable;
	params->PchHdaIDispCodecDisconnect = config->PchHdaIDispCodecDisconnect;
	params->PchHdaAudioLinkHda = config->PchHdaAudioLinkHda;
	params->PchHdaAudioLinkDmic0 = config->PchHdaAudioLinkDmic0;
	params->PchHdaAudioLinkDmic1 = config->PchHdaAudioLinkDmic1;
	params->PchHdaAudioLinkSsp0 = config->PchHdaAudioLinkSsp0;
	params->PchHdaAudioLinkSsp1 = config->PchHdaAudioLinkSsp1;
	params->PchHdaAudioLinkSsp2 = config->PchHdaAudioLinkSsp2;
	params->PchHdaAudioLinkSndw1 = config->PchHdaAudioLinkSndw1;
	params->PchHdaAudioLinkSndw2 = config->PchHdaAudioLinkSndw2;
	params->PchHdaAudioLinkSndw3 = config->PchHdaAudioLinkSndw3;
	params->PchHdaAudioLinkSndw4 = config->PchHdaAudioLinkSndw4;

	/* eDP device */
	params->DdiPortEdp = config->DdiPortEdp;

	/* HPD of DDI ports */
	params->DdiPortBHpd = config->DdiPortBHpd;
	params->DdiPortCHpd = config->DdiPortCHpd;
	params->DdiPortDHpd = config->DdiPortDHpd;
	params->DdiPortFHpd = config->DdiPortFHpd;

	/* DDC of DDI ports */
	params->DdiPortBDdc = config->DdiPortBDdc;
	params->DdiPortCDdc = config->DdiPortCDdc;
	params->DdiPortDDdc = config->DdiPortDDdc;
	params->DdiPortFDdc = config->DdiPortFDdc;

	/* WOL */
	params->PchPmPcieWakeFromDeepSx = config->LanWakeFromDeepSx;
	params->PchPmWolEnableOverride = config->WolEnableOverride;

	/* S0ix */
	params->PchPmSlpS0Enable = config->s0ix_enable;

	/* disable Legacy PME */
	memset(params->PcieRpPmSci, 0, sizeof(params->PcieRpPmSci));

	/* Legacy 8254 timer support */
	params->Enable8254ClockGating = !CONFIG(USE_LEGACY_8254_TIMER);
	params->Enable8254ClockGatingOnS3 = !CONFIG(USE_LEGACY_8254_TIMER);

	/* USB */
	for (i = 0; i < ARRAY_SIZE(config->usb2_ports); i++) {
		params->PortUsb20Enable[i] = config->usb2_ports[i].enable;
		params->Usb2AfePetxiset[i] = config->usb2_ports[i].pre_emp_bias;
		params->Usb2AfeTxiset[i] = config->usb2_ports[i].tx_bias;
		params->Usb2AfePredeemp[i] =
			config->usb2_ports[i].tx_emp_enable;
		params->Usb2AfePehalfbit[i] = config->usb2_ports[i].pre_emp_bit;

		if (config->usb2_ports[i].enable)
			params->Usb2OverCurrentPin[i] = config->usb2_ports[i].ocpin;
		else
			params->Usb2OverCurrentPin[i] = 0xff;
	}

	if (config->PchUsb2PhySusPgDisable)
		params->PchUsb2PhySusPgEnable = 0;

	for (i = 0; i < ARRAY_SIZE(config->usb3_ports); i++) {
		params->PortUsb30Enable[i] = config->usb3_ports[i].enable;
		if (config->usb3_ports[i].enable) {
			params->Usb3OverCurrentPin[i] = config->usb3_ports[i].ocpin;
		} else {
			params->Usb3OverCurrentPin[i] = 0xff;
		}
		if (config->usb3_ports[i].tx_de_emp) {
			params->Usb3HsioTxDeEmphEnable[i] = 1;
			params->Usb3HsioTxDeEmph[i] =
				config->usb3_ports[i].tx_de_emp;
		}
		if (config->usb3_ports[i].tx_downscale_amp) {
			params->Usb3HsioTxDownscaleAmpEnable[i] = 1;
			params->Usb3HsioTxDownscaleAmp[i] =
				config->usb3_ports[i].tx_downscale_amp;
		}
#if CONFIG(SOC_INTEL_COMETLAKE)
		if (config->usb3_ports[i].gen2_tx_rate0_uniq_tran_enable) {
			params->Usb3HsioTxRate0UniqTranEnable[i] = 1;
			params->Usb3HsioTxRate0UniqTran[i] =
				config->usb3_ports[i].gen2_tx_rate0_uniq_tran;
		}
		if (config->usb3_ports[i].gen2_tx_rate1_uniq_tran_enable) {
			params->Usb3HsioTxRate1UniqTranEnable[i] = 1;
			params->Usb3HsioTxRate1UniqTran[i] =
				config->usb3_ports[i].gen2_tx_rate1_uniq_tran;
		}
		if (config->usb3_ports[i].gen2_tx_rate2_uniq_tran_enable) {
			params->Usb3HsioTxRate2UniqTranEnable[i] = 1;
			params->Usb3HsioTxRate2UniqTran[i] =
				config->usb3_ports[i].gen2_tx_rate2_uniq_tran;
		}
		if (config->usb3_ports[i].gen2_tx_rate3_uniq_tran_enable) {
			params->Usb3HsioTxRate3UniqTranEnable[i] = 1;
			params->Usb3HsioTxRate3UniqTran[i] =
				config->usb3_ports[i].gen2_tx_rate3_uniq_tran;
		}
#endif
		if (config->usb3_ports[i].gen2_rx_tuning_enable) {
			params->PchUsbHsioRxTuningEnable[i] =
				config->usb3_ports[i].gen2_rx_tuning_enable;
			params->PchUsbHsioRxTuningParameters[i] =
				config->usb3_ports[i].gen2_rx_tuning_params;
			params->PchUsbHsioFilterSel[i] =
				config->usb3_ports[i].gen2_rx_filter_sel;
		}
	}

	/* Enable xDCI controller if enabled in devicetree and allowed */
	dev = pcidev_path_on_root(PCH_DEVFN_USBOTG);
	if (dev) {
		if (!xdci_can_enable())
			dev->enabled = 0;
		params->XdciEnable = dev->enabled;
	} else
		params->XdciEnable = 0;

	/* Set Debug serial port */
	params->SerialIoDebugUartNumber = CONFIG_UART_FOR_CONSOLE;
#if !CONFIG(SOC_INTEL_COMETLAKE)
	params->SerialIoEnableDebugUartAfterPost = CONFIG(INTEL_LPSS_UART_FOR_CONSOLE);
#endif

	/* Enable CNVi Wifi if enabled in device tree */
	dev = pcidev_path_on_root(PCH_DEVFN_CNViWIFI);
#if CONFIG(SOC_INTEL_COMETLAKE)
	if (dev)
		params->CnviMode = dev->enabled;
	else
		params->CnviMode = 0;
#else
	if (dev)
		params->PchCnviMode = dev->enabled;
	else
		params->PchCnviMode = 0;
#endif
	/* PCI Express */
	for (i = 0; i < ARRAY_SIZE(config->PcieClkSrcUsage); i++) {
		if (config->PcieClkSrcUsage[i] == 0)
			config->PcieClkSrcUsage[i] = PCIE_CLK_NOTUSED;
	}
	memcpy(params->PcieClkSrcUsage, config->PcieClkSrcUsage,
	       sizeof(config->PcieClkSrcUsage));
	memcpy(params->PcieClkSrcClkReq, config->PcieClkSrcClkReq,
	       sizeof(config->PcieClkSrcClkReq));

	memcpy(params->PcieRpAdvancedErrorReporting,
		config->PcieRpAdvancedErrorReporting,
		sizeof(params->PcieRpAdvancedErrorReporting));

	memcpy(params->PcieRpLtrEnable, config->PcieRpLtrEnable,
	       sizeof(config->PcieRpLtrEnable));
	memcpy(params->PcieRpSlotImplemented, config->PcieRpSlotImplemented,
	       sizeof(config->PcieRpSlotImplemented));
	memcpy(params->PcieRpHotPlug, config->PcieRpHotPlug,
	       sizeof(params->PcieRpHotPlug));

	for (i = 0; i < CONFIG_MAX_ROOT_PORTS; i++) {
		params->PcieRpMaxPayload[i] = config->PcieRpMaxPayload[i];
		if (config->PcieRpAspm[i])
			params->PcieRpAspm[i] = config->PcieRpAspm[i] - 1;
	};

	/* eMMC and SD */
	dev = pcidev_path_on_root(PCH_DEVFN_EMMC);
	if (!dev)
		params->ScsEmmcEnabled = 0;
	else {
		params->ScsEmmcEnabled = dev->enabled;
		params->ScsEmmcHs400Enabled = config->ScsEmmcHs400Enabled;
		params->PchScsEmmcHs400DllDataValid = config->EmmcHs400DllNeed;
		if (config->EmmcHs400DllNeed == 1) {
			params->PchScsEmmcHs400RxStrobeDll1 =
				config->EmmcHs400RxStrobeDll1;
			params->PchScsEmmcHs400TxDataDll =
				config->EmmcHs400TxDataDll;
		}
	}

	dev = pcidev_path_on_root(PCH_DEVFN_SDCARD);
	if (!dev) {
		params->ScsSdCardEnabled = 0;
	} else {
		params->ScsSdCardEnabled = dev->enabled;
		params->SdCardPowerEnableActiveHigh =
			CONFIG(MB_HAS_ACTIVE_HIGH_SD_PWR_ENABLE);
#if CONFIG(SOC_INTEL_COMETLAKE)
		params->ScsSdCardWpPinEnabled = config->ScsSdCardWpPinEnabled;
#endif
	}

	dev = pcidev_path_on_root(PCH_DEVFN_UFS);
	if (!dev)
		params->ScsUfsEnabled = 0;
	else
		params->ScsUfsEnabled = dev->enabled;

	dev = pcidev_path_on_root(PCH_DEVFN_CSE_3);
	params->Heci3Enabled = is_dev_enabled(dev);
#if !CONFIG(HECI_DISABLE_USING_SMM)
	dev = pcidev_path_on_root(PCH_DEVFN_CSE);
	params->Heci1Disabled = !is_dev_enabled(dev);
#endif
	params->Device4Enable = config->Device4Enable;

	/* Teton Glacier hybrid storage support */
	params->TetonGlacierMode = config->TetonGlacierMode;

	/* VrConfig Settings for 5 domains
	 * 0 = System Agent, 1 = IA Core, 2 = Ring,
	 * 3 = GT unsliced,  4 = GT sliced */
	for (i = 0; i < ARRAY_SIZE(config->domain_vr_config); i++)
		fill_vr_domain_config(params, i, &config->domain_vr_config[i]);

	/* Acoustic Noise Mitigation */
	params->AcousticNoiseMitigation = config->AcousticNoiseMitigation;
	params->SlowSlewRateForIa = config->SlowSlewRateForIa;
	params->SlowSlewRateForGt = config->SlowSlewRateForGt;
	params->SlowSlewRateForSa = config->SlowSlewRateForSa;
	params->SlowSlewRateForFivr = config->SlowSlewRateForFivr;
	params->FastPkgCRampDisableIa = config->FastPkgCRampDisableIa;
	params->FastPkgCRampDisableGt = config->FastPkgCRampDisableGt;
	params->FastPkgCRampDisableSa = config->FastPkgCRampDisableSa;
	params->FastPkgCRampDisableFivr = config->FastPkgCRampDisableFivr;

	/* Disable PCH ACPI timer */
	params->EnableTcoTimer = !config->PmTimerDisabled;

	/* Apply minimum assertion width settings if non-zero */
	if (config->PchPmSlpS3MinAssert)
		params->PchPmSlpS3MinAssert = config->PchPmSlpS3MinAssert;
	if (config->PchPmSlpS4MinAssert)
		params->PchPmSlpS4MinAssert = config->PchPmSlpS4MinAssert;
	if (config->PchPmSlpSusMinAssert)
		params->PchPmSlpSusMinAssert = config->PchPmSlpSusMinAssert;
	if (config->PchPmSlpAMinAssert)
		params->PchPmSlpAMinAssert = config->PchPmSlpAMinAssert;

#if CONFIG(SOC_INTEL_COMETLAKE)
	if (config->PchPmPwrCycDur)
		params->PchPmPwrCycDur = get_pm_pwr_cyc_dur(config->PchPmSlpS4MinAssert,
				config->PchPmSlpS3MinAssert, config->PchPmSlpAMinAssert,
				config->PchPmPwrCycDur);
#endif

	/* Set TccActivationOffset */
	tconfig->TccActivationOffset = config->tcc_offset;

	/* Unlock all GPIO pads */
	tconfig->PchUnlockGpioPads = config->PchUnlockGpioPads;

	/* Set correct Sirq mode based on config */
	params->PchSirqEnable = config->serirq_mode != SERIRQ_OFF;
	params->PchSirqMode = config->serirq_mode == SERIRQ_CONTINUOUS;

	/*
	 * GSPI Chip Select parameters
	 * The GSPI driver assumes that CS0 is the used chip-select line,
	 * therefore only CS0 is configured below.
	 */
#if CONFIG(SOC_INTEL_COMETLAKE)
	configure_gspi_cs(0, config, &params->SerialIoSpi0CsPolarity[0],
			&params->SerialIoSpi0CsEnable[0],
			&params->SerialIoSpiDefaultCsOutput[0]);
	configure_gspi_cs(1, config, &params->SerialIoSpi1CsPolarity[0],
			&params->SerialIoSpi1CsEnable[0],
			&params->SerialIoSpiDefaultCsOutput[1]);
	configure_gspi_cs(2, config, &params->SerialIoSpi2CsPolarity[0],
			&params->SerialIoSpi2CsEnable[0],
			&params->SerialIoSpiDefaultCsOutput[2]);
#else
	for (i = 0; i < CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX; i++)
		configure_gspi_cs(i, config,
				&params->SerialIoSpiCsPolarity[0], NULL, NULL);
#endif

	/* Chipset Lockdown */
	if (get_lockdown_config() == CHIPSET_LOCKDOWN_COREBOOT) {
		tconfig->PchLockDownGlobalSmi = 0;
		tconfig->PchLockDownBiosInterface = 0;
		params->PchLockDownBiosLock = 0;
		params->PchLockDownRtcMemoryLock = 0;
#if CONFIG(SOC_INTEL_COMETLAKE)
		/*
		 * Skip SPI Flash Lockdown from inside FSP.
		 * Making this config "0" means FSP won't set the FLOCKDN bit
		 * of SPIBAR + 0x04 (i.e., Bit 15 of BIOS_HSFSTS_CTL).
		 * So, it becomes coreboot's responsibility to set this bit
		 * before end of POST for security concerns.
		 */
		params->SpiFlashCfgLockDown = 0;
#endif
	} else {
		tconfig->PchLockDownGlobalSmi = 1;
		tconfig->PchLockDownBiosInterface = 1;
		params->PchLockDownBiosLock = 1;
		params->PchLockDownRtcMemoryLock = 1;
#if CONFIG(SOC_INTEL_COMETLAKE)
		/*
		 * Enable SPI Flash Lockdown from inside FSP.
		 * Making this config "1" means FSP will set the FLOCKDN bit
		 * of SPIBAR + 0x04 (i.e., Bit 15 of BIOS_HSFSTS_CTL).
		 */
		params->SpiFlashCfgLockDown = 1;
#endif
	}

#if !CONFIG(SOC_INTEL_COMETLAKE)
	params->VrPowerDeliveryDesign = config->VrPowerDeliveryDesign;
#endif

	dev = pcidev_path_on_root(SA_DEVFN_IGD);
	if (CONFIG(RUN_FSP_GOP) && dev && dev->enabled)
		params->PeiGraphicsPeimInit = 1;
	else
		params->PeiGraphicsPeimInit = 0;
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

/* Handle FSP logo params */
const struct cbmem_entry *soc_load_logo(FSPS_UPD *supd)
{
	return fsp_load_logo(&supd->FspsConfig.LogoPtr, &supd->FspsConfig.LogoSize);
}
