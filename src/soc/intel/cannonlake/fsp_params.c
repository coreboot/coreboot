/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <intelblocks/xdci.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <string.h>

static void parse_devicetree(FSP_S_CONFIG *params)
{
	struct device *dev = SA_DEV_ROOT;
	if (!dev) {
		printk(BIOS_ERR, "Could not find root device\n");
		return;
	}

	const config_t *config = dev->chip_info;
	const int SerialIoDev[] = {
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

	for (int i = 0; i < ARRAY_SIZE(SerialIoDev); i++) {
		dev = dev_find_slot(0, SerialIoDev[i]);
		if (!dev->enabled) {
			params->SerialIoDevMode[i] = PchSerialIoDisabled;
			continue;
		}
		params->SerialIoDevMode[i] = PchSerialIoPci;
		if (config->SerialIoDevMode[i] == PchSerialIoAcpi ||
		    config->SerialIoDevMode[i] == PchSerialIoHidden)
			params->SerialIoDevMode[i] = config->SerialIoDevMode[i];
	}
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	int i;
	FSP_S_CONFIG *params = &supd->FspsConfig;
	FSP_S_TEST_CONFIG *tconfig = &supd->FspsTestConfig;
	struct device *dev = SA_DEV_ROOT;
	config_t *config = dev->chip_info;

	/* Parse device tree and enable/disable devices */
	parse_devicetree(params);

	/* Load VBT before devicetree-specific config. */
	params->GraphicsConfigPtr = (uintptr_t)vbt_get();

	/* Set USB OC pin to 0 first */
	for (i = 0; i < ARRAY_SIZE(params->Usb2OverCurrentPin); i++) {
		params->Usb2OverCurrentPin[i] = 0;
	}

	for (i = 0; i < ARRAY_SIZE(params->Usb3OverCurrentPin); i++) {
		params->Usb3OverCurrentPin[i] = 0;
	}

	mainboard_silicon_init_params(params);

	/* Unlock upper 8 bytes of RTC RAM */
	params->PchLockDownRtcMemoryLock = 0;

	/* SATA */
	dev = dev_find_slot(0, PCH_DEVFN_SATA);
	if (!dev)
		params->SataEnable = 0;
	else {
		params->SataEnable = dev->enabled;
		params->SataMode = config->SataMode;
		params->SataSalpSupport = config->SataSalpSupport;
		memcpy(params->SataPortsEnable, config->SataPortsEnable,
			sizeof(params->SataPortsEnable));
		memcpy(params->SataPortsDevSlp, config->SataPortsDevSlp,
			sizeof(params->SataPortsDevSlp));
	}

	/* Lan */
	dev = dev_find_slot(0, PCH_DEVFN_GBE);
	if (!dev)
		params->PchLanEnable = 0;
	else
		params->PchLanEnable = dev->enabled;

	/* Audio */
	params->PchHdaDspEnable = config->PchHdaDspEnable;
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

	/* S0ix */
	params->PchPmSlpS0Enable = config->s0ix_enable;

	/* disable Legacy PME */
	memset(params->PcieRpPmSci, 0, sizeof(params->PcieRpPmSci));

	/* USB */
	for (i = 0; i < ARRAY_SIZE(config->usb2_ports); i++) {
		params->PortUsb20Enable[i] = config->usb2_ports[i].enable;
		params->Usb2OverCurrentPin[i] = config->usb2_ports[i].ocpin;
		params->Usb2AfePetxiset[i] = config->usb2_ports[i].pre_emp_bias;
		params->Usb2AfeTxiset[i] = config->usb2_ports[i].tx_bias;
		params->Usb2AfePredeemp[i] =
			config->usb2_ports[i].tx_emp_enable;
		params->Usb2AfePehalfbit[i] = config->usb2_ports[i].pre_emp_bit;
	}

	for (i = 0; i < ARRAY_SIZE(config->usb3_ports); i++) {
		params->PortUsb30Enable[i] = config->usb3_ports[i].enable;
		params->Usb3OverCurrentPin[i] = config->usb3_ports[i].ocpin;
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
	}

	/* Enable xDCI controller if enabled in devicetree and allowed */
	dev = dev_find_slot(0, PCH_DEVFN_USBOTG);
	if (!xdci_can_enable())
		dev->enabled = 0;
	params->XdciEnable = dev->enabled;

	/* Enable CNVi Wifi if enabled in device tree */
	dev = dev_find_slot(0, PCH_DEVFN_CNViWIFI);
	params->PchCnviMode = dev->enabled;

	/* PCI Express */
	for (i = 0; i < ARRAY_SIZE(config->PcieClkSrcUsage); i++) {
		if (config->PcieClkSrcUsage[i] == 0)
			config->PcieClkSrcUsage[i] = PCIE_CLK_NOTUSED;
	}
	memcpy(params->PcieClkSrcUsage, config->PcieClkSrcUsage,
	       sizeof(config->PcieClkSrcUsage));
	memcpy(params->PcieClkSrcClkReq, config->PcieClkSrcClkReq,
	       sizeof(config->PcieClkSrcClkReq));
	memcpy(params->PcieRpLtrEnable, config->PcieRpLtrEnable,
	       sizeof(config->PcieRpLtrEnable));

	/* eMMC and SD */
	dev = dev_find_slot(0, PCH_DEVFN_EMMC);
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

	dev = dev_find_slot(0, PCH_DEVFN_SDCARD);
	if (!dev)
		params->ScsSdCardEnabled = 0;
	else
		params->ScsSdCardEnabled = dev->enabled;

	dev = dev_find_slot(0, PCH_DEVFN_UFS);
	if (!dev)
		params->ScsUfsEnabled = 0;
	else
		params->ScsUfsEnabled = dev->enabled;

	params->Heci3Enabled = config->Heci3Enabled;
	params->Device4Enable = config->Device4Enable;

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

	/* Power Optimizer */
	params->PchPwrOptEnable = config->dmipwroptimize;
	params->SataPwrOptEnable = config->satapwroptimize;

	/* Apply minimum assertion width settings if non-zero */
	if (config->PchPmSlpS3MinAssert)
		params->PchPmSlpS3MinAssert = config->PchPmSlpS3MinAssert;
	if (config->PchPmSlpS4MinAssert)
		params->PchPmSlpS4MinAssert = config->PchPmSlpS4MinAssert;
	if (config->PchPmSlpSusMinAssert)
		params->PchPmSlpSusMinAssert = config->PchPmSlpSusMinAssert;
	if (config->PchPmSlpAMinAssert)
		params->PchPmSlpAMinAssert = config->PchPmSlpAMinAssert;

	/* Set TccActivationOffset */
	tconfig->TccActivationOffset = config->tcc_offset;
}

/* Mainboard GPIO Configuration */
__weak void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
