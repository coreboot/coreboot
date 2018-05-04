/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2017 Intel Corporation.
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

#include <arch/acpi.h>
#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/util.h>
#include <intelblocks/xdci.h>
#include <soc/acpi.h>
#include <soc/interrupt.h>
#include <soc/irq.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <string.h>

void soc_init_pre_device(void *chip_info)
{
	/* Perform silicon specific init. */
	intel_silicon_init();
}

void soc_fsp_load(void)
{
	fsp_load();
}

static void pci_domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources   = &pci_domain_read_resources,
	.set_resources    = &pci_domain_set_resources,
	.scan_bus         = &pci_domain_scan_bus,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_name        = &soc_acpi_name,
#endif
};

static struct device_operations cpu_bus_ops = {
	.init             = DEVICE_NOOP,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_fill_ssdt_generator = generate_cpu_entries,
#endif
};

static void soc_enable(device_t dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
}

struct chip_operations soc_intel_skylake_ops = {
	CHIP_NAME("Intel Skylake")
	.enable_dev = &soc_enable,
	.init       = &soc_init_pre_device,
};

/* UPD parameters to be initialized before SiliconInit */
void soc_silicon_init_params(SILICON_INIT_UPD *params)
{
	struct device *dev = dev_find_slot(0, PCH_DEVFN_LPC);
	const struct soc_intel_skylake_config *config = dev->chip_info;
	int i;

	memcpy(params->SerialIoDevMode, config->SerialIoDevMode,
	       sizeof(params->SerialIoDevMode));

	for (i = 0; i < ARRAY_SIZE(config->usb2_ports); i++) {
		params->PortUsb20Enable[i] =
			config->usb2_ports[i].enable;
		params->Usb2OverCurrentPin[i] =
			config->usb2_ports[i].ocpin;
		params->Usb2AfePetxiset[i] =
			config->usb2_ports[i].pre_emp_bias;
		params->Usb2AfeTxiset[i] =
			config->usb2_ports[i].tx_bias;
		params->Usb2AfePredeemp[i] =
			config->usb2_ports[i].tx_emp_enable;
		params->Usb2AfePehalfbit[i] =
			config->usb2_ports[i].pre_emp_bit;
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

	memcpy(params->PcieRpEnable, config->PcieRpEnable,
			sizeof(params->PcieRpEnable));
	memcpy(params->PcieRpClkReqSupport, config->PcieRpClkReqSupport,
			sizeof(params->PcieRpClkReqSupport));
	memcpy(params->PcieRpClkReqNumber, config->PcieRpClkReqNumber,
			sizeof(params->PcieRpClkReqNumber));
	memcpy(params->PcieRpHotPlug, config->PcieRpHotPlug,
			sizeof(params->PcieRpHotPlug));

	params->EnableLan = config->EnableLan;
	params->Cio2Enable = config->Cio2Enable;
	params->SataSalpSupport = config->SataSalpSupport;
	memcpy(params->SataPortsEnable, config->SataPortsEnable,
			sizeof(params->SataPortsEnable));
	memcpy(params->SataPortsDevSlp, config->SataPortsDevSlp,
			sizeof(params->SataPortsDevSlp));
	params->SsicPortEnable = config->SsicPortEnable;
	params->SmbusEnable = config->SmbusEnable;
	params->ScsEmmcEnabled = config->ScsEmmcEnabled;
	params->ScsEmmcHs400Enabled = config->ScsEmmcHs400Enabled;
	params->ScsSdCardEnabled = config->ScsSdCardEnabled;
	params->IshEnable = 0;
	params->EnableAzalia = config->EnableAzalia;
	params->IoBufferOwnership = config->IoBufferOwnership;
	params->DspEnable = config->DspEnable;
	params->Device4Enable = config->Device4Enable;
	params->EnableSata = config->EnableSata;
	params->SataMode = config->SataMode;
	params->LockDownConfigGlobalSmi = config->LockDownConfigGlobalSmi;
	params->LockDownConfigRtcLock = config->LockDownConfigRtcLock;
	if (config->chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT) {
		params->LockDownConfigBiosInterface = 0;
		params->LockDownConfigBiosLock = 0;
		params->LockDownConfigSpiEiss = 0;
	}
	params->PchConfigSubSystemVendorId = config->PchConfigSubSystemVendorId;
	params->PchConfigSubSystemId = config->PchConfigSubSystemId;
	params->WakeConfigWolEnableOverride =
		config->WakeConfigWolEnableOverride;
	params->WakeConfigPcieWakeFromDeepSx =
		config->WakeConfigPcieWakeFromDeepSx;
	params->PmConfigDeepSxPol = config->PmConfigDeepSxPol;
	params->PmConfigSlpS3MinAssert = config->PmConfigSlpS3MinAssert;
	params->PmConfigSlpS4MinAssert = config->PmConfigSlpS4MinAssert;
	params->PmConfigSlpSusMinAssert = config->PmConfigSlpSusMinAssert;
	params->PmConfigSlpAMinAssert = config->PmConfigSlpAMinAssert;
	params->PmConfigPciClockRun = config->PmConfigPciClockRun;
	params->PmConfigSlpStrchSusUp = config->PmConfigSlpStrchSusUp;
	params->PmConfigPwrBtnOverridePeriod =
		config->PmConfigPwrBtnOverridePeriod;
	params->PmConfigPwrCycDur = config->PmConfigPwrCycDur;
	params->SerialIrqConfigSirqEnable = config->SerialIrqConfigSirqEnable;
	params->SerialIrqConfigSirqMode = config->SerialIrqConfigSirqMode;
	params->SerialIrqConfigStartFramePulse =
		config->SerialIrqConfigStartFramePulse;

	params->SkipMpInit = config->FspSkipMpInit;

	for (i = 0; i < ARRAY_SIZE(config->i2c); i++)
		params->SerialIoI2cVoltage[i] = config->i2c_voltage[i];

	/*
	 * To disable Heci, the Psf needs to be left unlocked
	 * by FSP after end of post sequence. Based on the devicetree
	 * setting, we set the appropriate PsfUnlock policy in Fsp,
	 * do the changes and then lock it back in coreboot
	 *
	 */
	if (config->HeciEnabled == 0)
		params->PsfUnlock = 1;
	else
		params->PsfUnlock = 0;

	for (i = 0; i < ARRAY_SIZE(config->domain_vr_config); i++)
		fill_vr_domain_config(params, i, &config->domain_vr_config[i]);

	/* Show SPI controller if enabled in devicetree.cb */
	dev = dev_find_slot(0, PCH_DEVFN_SPI);
	params->ShowSpiController = dev->enabled;

	/* Enable xDCI controller if enabled in devicetree and allowed */
	dev = dev_find_slot(0, PCH_DEVFN_USBOTG);
	if (!xdci_can_enable())
		dev->enabled = 0;
	params->XdciEnable = dev->enabled;

	params->SendVrMbxCmd = config->SendVrMbxCmd;

	/* Acoustic Noise Mitigation */
	params->AcousticNoiseMitigation = config->AcousticNoiseMitigation;
	params->SlowSlewRateForIa = config->SlowSlewRateForIa;
	params->SlowSlewRateForGt = config->SlowSlewRateForGt;
	params->SlowSlewRateForSa = config->SlowSlewRateForSa;
	params->FastPkgCRampDisable = config->FastPkgCRampDisable;

	soc_irq_settings(params);
}

void soc_display_silicon_init_params(const SILICON_INIT_UPD *original,
	SILICON_INIT_UPD *params)
{
	/* Display the parameters for SiliconInit */
	printk(BIOS_SPEW, "UPD values for SiliconInit:\n");
	fsp_display_upd_value("LogoPtr", 4,
			(uint32_t)original->LogoPtr,
			(uint32_t)params->LogoPtr);
	fsp_display_upd_value("LogoSize", 4,
		(uint32_t)original->LogoSize,
		(uint32_t)params->LogoSize);
	fsp_display_upd_value("GraphicsConfigPtr", 4,
		(uint32_t)original->GraphicsConfigPtr,
		(uint32_t)params->GraphicsConfigPtr);
	fsp_display_upd_value("MicrocodeRegionBase", 4,
		(uint32_t)original->MicrocodeRegionBase,
		(uint32_t)params->MicrocodeRegionBase);
	fsp_display_upd_value("MicrocodeRegionSize", 4,
		(uint32_t)original->MicrocodeRegionSize,
		(uint32_t)params->MicrocodeRegionSize);
	fsp_display_upd_value("TurboMode", 1,
		(uint32_t)original->TurboMode,
		(uint32_t)params->TurboMode);
	fsp_display_upd_value("Device4Enable", 1,
		original->Device4Enable,
		params->Device4Enable);
	fsp_display_upd_value("PcieRpEnable[0]", 1, original->PcieRpEnable[0],
		params->PcieRpEnable[0]);
	fsp_display_upd_value("PcieRpEnable[1]", 1, original->PcieRpEnable[1],
		params->PcieRpEnable[1]);
	fsp_display_upd_value("PcieRpEnable[2]", 1, original->PcieRpEnable[2],
		params->PcieRpEnable[2]);
	fsp_display_upd_value("PcieRpEnable[3]", 1, original->PcieRpEnable[3],
		params->PcieRpEnable[3]);
	fsp_display_upd_value("PcieRpEnable[4]", 1, original->PcieRpEnable[4],
		params->PcieRpEnable[4]);
	fsp_display_upd_value("PcieRpEnable[5]", 1, original->PcieRpEnable[5],
		params->PcieRpEnable[5]);
	fsp_display_upd_value("PcieRpEnable[6]", 1, original->PcieRpEnable[6],
		params->PcieRpEnable[6]);
	fsp_display_upd_value("PcieRpEnable[7]", 1, original->PcieRpEnable[7],
		params->PcieRpEnable[7]);
	fsp_display_upd_value("PcieRpEnable[8]", 1, original->PcieRpEnable[8],
		params->PcieRpEnable[8]);
	fsp_display_upd_value("PcieRpEnable[9]", 1, original->PcieRpEnable[9],
		params->PcieRpEnable[9]);
	fsp_display_upd_value("PcieRpEnable[10]", 1, original->PcieRpEnable[10],
		params->PcieRpEnable[10]);
	fsp_display_upd_value("PcieRpEnable[11]", 1, original->PcieRpEnable[11],
		params->PcieRpEnable[11]);
	fsp_display_upd_value("PcieRpEnable[12]", 1, original->PcieRpEnable[12],
		params->PcieRpEnable[12]);
	fsp_display_upd_value("PcieRpEnable[13]", 1, original->PcieRpEnable[13],
		params->PcieRpEnable[13]);
	fsp_display_upd_value("PcieRpEnable[14]", 1, original->PcieRpEnable[14],
		params->PcieRpEnable[14]);
	fsp_display_upd_value("PcieRpEnable[15]", 1, original->PcieRpEnable[15],
		params->PcieRpEnable[15]);
	fsp_display_upd_value("PcieRpEnable[16]", 1, original->PcieRpEnable[16],
		params->PcieRpEnable[16]);
	fsp_display_upd_value("PcieRpEnable[17]", 1, original->PcieRpEnable[17],
		params->PcieRpEnable[17]);
	fsp_display_upd_value("PcieRpEnable[18]", 1, original->PcieRpEnable[18],
		params->PcieRpEnable[18]);
	fsp_display_upd_value("PcieRpEnable[19]", 1, original->PcieRpEnable[19],
		params->PcieRpEnable[19]);
	fsp_display_upd_value("PcieRpClkReqSupport[0]", 1,
		original->PcieRpClkReqSupport[0],
		params->PcieRpClkReqSupport[0]);
	fsp_display_upd_value("PcieRpClkReqSupport[1]", 1,
		original->PcieRpClkReqSupport[1],
		params->PcieRpClkReqSupport[1]);
	fsp_display_upd_value("PcieRpClkReqSupport[2]", 1,
		original->PcieRpClkReqSupport[2],
		params->PcieRpClkReqSupport[2]);
	fsp_display_upd_value("PcieRpClkReqSupport[3]", 1,
		original->PcieRpClkReqSupport[3],
		params->PcieRpClkReqSupport[3]);
	fsp_display_upd_value("PcieRpClkReqSupport[4]", 1,
		original->PcieRpClkReqSupport[4],
		params->PcieRpClkReqSupport[4]);
	fsp_display_upd_value("PcieRpClkReqSupport[5]", 1,
		original->PcieRpClkReqSupport[5],
		params->PcieRpClkReqSupport[5]);
	fsp_display_upd_value("PcieRpClkReqSupport[6]", 1,
		original->PcieRpClkReqSupport[6],
		params->PcieRpClkReqSupport[6]);
	fsp_display_upd_value("PcieRpClkReqSupport[7]", 1,
		original->PcieRpClkReqSupport[7],
		params->PcieRpClkReqSupport[7]);
	fsp_display_upd_value("PcieRpClkReqSupport[8]", 1,
		original->PcieRpClkReqSupport[8],
		params->PcieRpClkReqSupport[8]);
	fsp_display_upd_value("PcieRpClkReqSupport[9]", 1,
		original->PcieRpClkReqSupport[9],
		params->PcieRpClkReqSupport[9]);
	fsp_display_upd_value("PcieRpClkReqSupport[10]", 1,
		original->PcieRpClkReqSupport[10],
		params->PcieRpClkReqSupport[10]);
	fsp_display_upd_value("PcieRpClkReqSupport[11]", 1,
		original->PcieRpClkReqSupport[11],
		params->PcieRpClkReqSupport[11]);
	fsp_display_upd_value("PcieRpClkReqSupport[12]", 1,
		original->PcieRpClkReqSupport[12],
		params->PcieRpClkReqSupport[12]);
	fsp_display_upd_value("PcieRpClkReqSupport[13]", 1,
		original->PcieRpClkReqSupport[13],
		params->PcieRpClkReqSupport[13]);
	fsp_display_upd_value("PcieRpClkReqSupport[14]", 1,
		original->PcieRpClkReqSupport[14],
		params->PcieRpClkReqSupport[14]);
	fsp_display_upd_value("PcieRpClkReqSupport[15]", 1,
		original->PcieRpClkReqSupport[15],
		params->PcieRpClkReqSupport[15]);
	fsp_display_upd_value("PcieRpClkReqSupport[16]", 1,
		original->PcieRpClkReqSupport[16],
		params->PcieRpClkReqSupport[16]);
	fsp_display_upd_value("PcieRpClkReqSupport[17]", 1,
		original->PcieRpClkReqSupport[17],
		params->PcieRpClkReqSupport[17]);
	fsp_display_upd_value("PcieRpClkReqSupport[18]", 1,
		original->PcieRpClkReqSupport[18],
		params->PcieRpClkReqSupport[18]);
	fsp_display_upd_value("PcieRpClkReqSupport[19]", 1,
		original->PcieRpClkReqSupport[19],
		params->PcieRpClkReqSupport[19]);
	fsp_display_upd_value("PcieRpClkReqNumber[0]", 1,
		original->PcieRpClkReqNumber[0],
		params->PcieRpClkReqNumber[0]);
	fsp_display_upd_value("PcieRpClkReqNumber[1]", 1,
		original->PcieRpClkReqNumber[1],
		params->PcieRpClkReqNumber[1]);
	fsp_display_upd_value("PcieRpClkReqNumber[2]", 1,
		original->PcieRpClkReqNumber[2],
		params->PcieRpClkReqNumber[2]);
	fsp_display_upd_value("PcieRpClkReqNumber[3]", 1,
		original->PcieRpClkReqNumber[3],
		params->PcieRpClkReqNumber[3]);
	fsp_display_upd_value("PcieRpClkReqNumber[4]", 1,
		original->PcieRpClkReqNumber[4],
		params->PcieRpClkReqNumber[4]);
	fsp_display_upd_value("PcieRpClkReqNumber[5]", 1,
		original->PcieRpClkReqNumber[5],
		params->PcieRpClkReqNumber[5]);
	fsp_display_upd_value("PcieRpClkReqNumber[6]", 1,
		original->PcieRpClkReqNumber[6],
		params->PcieRpClkReqNumber[6]);
	fsp_display_upd_value("PcieRpClkReqNumber[7]", 1,
		original->PcieRpClkReqNumber[7],
		params->PcieRpClkReqNumber[7]);
	fsp_display_upd_value("PcieRpClkReqNumber[8]", 1,
		original->PcieRpClkReqNumber[8],
		params->PcieRpClkReqNumber[8]);
	fsp_display_upd_value("PcieRpClkReqNumber[9]", 1,
		original->PcieRpClkReqNumber[9],
		params->PcieRpClkReqNumber[9]);
	fsp_display_upd_value("PcieRpClkReqNumber[10]", 1,
		original->PcieRpClkReqNumber[10],
		params->PcieRpClkReqNumber[10]);
	fsp_display_upd_value("PcieRpClkReqNumber[11]", 1,
		original->PcieRpClkReqNumber[11],
		params->PcieRpClkReqNumber[11]);
	fsp_display_upd_value("PcieRpClkReqNumber[12]", 1,
		original->PcieRpClkReqNumber[12],
		params->PcieRpClkReqNumber[12]);
	fsp_display_upd_value("PcieRpClkReqNumber[13]", 1,
		original->PcieRpClkReqNumber[13],
		params->PcieRpClkReqNumber[13]);
	fsp_display_upd_value("PcieRpClkReqNumber[14]", 1,
		original->PcieRpClkReqNumber[14],
		params->PcieRpClkReqNumber[14]);
	fsp_display_upd_value("PcieRpClkReqNumber[15]", 1,
		original->PcieRpClkReqNumber[15],
		params->PcieRpClkReqNumber[15]);
	fsp_display_upd_value("PcieRpClkReqNumber[16]", 1,
		original->PcieRpClkReqNumber[16],
		params->PcieRpClkReqNumber[16]);
	fsp_display_upd_value("PcieRpClkReqNumber[17]", 1,
		original->PcieRpClkReqNumber[17],
		params->PcieRpClkReqNumber[17]);
	fsp_display_upd_value("PcieRpClkReqNumber[18]", 1,
		original->PcieRpClkReqNumber[18],
		params->PcieRpClkReqNumber[18]);
	fsp_display_upd_value("PcieRpClkReqNumber[19]", 1,
		original->PcieRpClkReqNumber[19],
		params->PcieRpClkReqNumber[19]);
	fsp_display_upd_value("EnableLan", 1, original->EnableLan,
		params->EnableLan);
	fsp_display_upd_value("Cio2Enable", 1, original->Cio2Enable,
		params->Cio2Enable);
	fsp_display_upd_value("SataSalpSupport", 1, original->SataSalpSupport,
		params->SataSalpSupport);
	fsp_display_upd_value("SataPortsEnable[0]", 1,
		original->SataPortsEnable[0], params->SataPortsEnable[0]);
	fsp_display_upd_value("SataPortsEnable[1]", 1,
		original->SataPortsEnable[1], params->SataPortsEnable[1]);
	fsp_display_upd_value("SataPortsEnable[2]", 1,
		original->SataPortsEnable[2], params->SataPortsEnable[2]);
	fsp_display_upd_value("SataPortsEnable[3]", 1,
		original->SataPortsEnable[3], params->SataPortsEnable[3]);
	fsp_display_upd_value("SataPortsEnable[4]", 1,
		original->SataPortsEnable[4], params->SataPortsEnable[4]);
	fsp_display_upd_value("SataPortsEnable[5]", 1,
		original->SataPortsEnable[5], params->SataPortsEnable[5]);
	fsp_display_upd_value("SataPortsEnable[6]", 1,
		original->SataPortsEnable[6], params->SataPortsEnable[6]);
	fsp_display_upd_value("SataPortsEnable[7]", 1,
		original->SataPortsEnable[7], params->SataPortsEnable[7]);
	fsp_display_upd_value("SataPortsDevSlp[0]", 1,
		original->SataPortsDevSlp[0], params->SataPortsDevSlp[0]);
	fsp_display_upd_value("SataPortsDevSlp[1]", 1,
		original->SataPortsDevSlp[1], params->SataPortsDevSlp[1]);
	fsp_display_upd_value("SataPortsDevSlp[2]", 1,
		original->SataPortsDevSlp[2], params->SataPortsDevSlp[2]);
	fsp_display_upd_value("SataPortsDevSlp[3]", 1,
		original->SataPortsDevSlp[3], params->SataPortsDevSlp[3]);
	fsp_display_upd_value("SataPortsDevSlp[4]", 1,
		original->SataPortsDevSlp[4], params->SataPortsDevSlp[4]);
	fsp_display_upd_value("SataPortsDevSlp[5]", 1,
		original->SataPortsDevSlp[5], params->SataPortsDevSlp[5]);
	fsp_display_upd_value("SataPortsDevSlp[6]", 1,
		original->SataPortsDevSlp[6], params->SataPortsDevSlp[6]);
	fsp_display_upd_value("SataPortsDevSlp[7]", 1,
		original->SataPortsDevSlp[7], params->SataPortsDevSlp[7]);
	fsp_display_upd_value("EnableAzalia", 1,
		original->EnableAzalia,	params->EnableAzalia);
	fsp_display_upd_value("DspEnable", 1, original->DspEnable,
		params->DspEnable);
	fsp_display_upd_value("IoBufferOwnership", 1,
		original->IoBufferOwnership, params->IoBufferOwnership);
	fsp_display_upd_value("PortUsb20Enable[0]", 1,
		original->PortUsb20Enable[0], params->PortUsb20Enable[0]);
	fsp_display_upd_value("PortUsb20Enable[1]", 1,
		original->PortUsb20Enable[1], params->PortUsb20Enable[1]);
	fsp_display_upd_value("PortUsb20Enable[2]", 1,
		original->PortUsb20Enable[2], params->PortUsb20Enable[2]);
	fsp_display_upd_value("PortUsb20Enable[3]", 1,
		original->PortUsb20Enable[3], params->PortUsb20Enable[3]);
	fsp_display_upd_value("PortUsb20Enable[4]", 1,
		original->PortUsb20Enable[4], params->PortUsb20Enable[4]);
	fsp_display_upd_value("PortUsb20Enable[5]", 1,
		original->PortUsb20Enable[5], params->PortUsb20Enable[5]);
	fsp_display_upd_value("PortUsb20Enable[6]", 1,
		original->PortUsb20Enable[6], params->PortUsb20Enable[6]);
	fsp_display_upd_value("PortUsb20Enable[7]", 1,
		original->PortUsb20Enable[7], params->PortUsb20Enable[7]);
	fsp_display_upd_value("PortUsb20Enable[8]", 1,
		original->PortUsb20Enable[8], params->PortUsb20Enable[8]);
	fsp_display_upd_value("PortUsb20Enable[9]", 1,
		original->PortUsb20Enable[9], params->PortUsb20Enable[9]);
	fsp_display_upd_value("PortUsb20Enable[10]", 1,
		original->PortUsb20Enable[10], params->PortUsb20Enable[10]);
	fsp_display_upd_value("PortUsb20Enable[11]", 1,
		original->PortUsb20Enable[11], params->PortUsb20Enable[11]);
	fsp_display_upd_value("PortUsb20Enable[12]", 1,
		original->PortUsb20Enable[12], params->PortUsb20Enable[12]);
	fsp_display_upd_value("PortUsb20Enable[13]", 1,
		original->PortUsb20Enable[13], params->PortUsb20Enable[13]);
	fsp_display_upd_value("PortUsb20Enable[14]", 1,
		original->PortUsb20Enable[14], params->PortUsb20Enable[14]);
	fsp_display_upd_value("PortUsb20Enable[15]", 1,
		original->PortUsb20Enable[15], params->PortUsb20Enable[15]);
	fsp_display_upd_value("PortUsb30Enable[0]", 1,
		original->PortUsb30Enable[0], params->PortUsb30Enable[0]);
	fsp_display_upd_value("PortUsb30Enable[1]", 1,
		original->PortUsb30Enable[1], params->PortUsb30Enable[1]);
	fsp_display_upd_value("PortUsb30Enable[2]", 1,
		original->PortUsb30Enable[2], params->PortUsb30Enable[2]);
	fsp_display_upd_value("PortUsb30Enable[3]", 1,
		original->PortUsb30Enable[3], params->PortUsb30Enable[3]);
	fsp_display_upd_value("PortUsb30Enable[4]", 1,
		original->PortUsb30Enable[4], params->PortUsb30Enable[4]);
	fsp_display_upd_value("PortUsb30Enable[5]", 1,
		original->PortUsb30Enable[5], params->PortUsb30Enable[5]);
	fsp_display_upd_value("PortUsb30Enable[6]", 1,
		original->PortUsb30Enable[6], params->PortUsb30Enable[6]);
	fsp_display_upd_value("PortUsb30Enable[7]", 1,
		original->PortUsb30Enable[7], params->PortUsb30Enable[7]);
	fsp_display_upd_value("PortUsb30Enable[8]", 1,
		original->PortUsb30Enable[8], params->PortUsb30Enable[8]);
	fsp_display_upd_value("PortUsb30Enable[9]", 1,
		original->PortUsb30Enable[9], params->PortUsb30Enable[9]);
	fsp_display_upd_value("XdciEnable", 1, original->XdciEnable,
		params->XdciEnable);
	fsp_display_upd_value("SsicPortEnable", 1, original->SsicPortEnable,
		params->SsicPortEnable);
	fsp_display_upd_value("SmbusEnable", 1, original->SmbusEnable,
		params->SmbusEnable);
	fsp_display_upd_value("SerialIoDevMode[0]", 1,
		original->SerialIoDevMode[0], params->SerialIoDevMode[0]);
	fsp_display_upd_value("SerialIoDevMode[1]", 1,
		original->SerialIoDevMode[1], params->SerialIoDevMode[1]);
	fsp_display_upd_value("SerialIoDevMode[2]", 1,
		original->SerialIoDevMode[2], params->SerialIoDevMode[2]);
	fsp_display_upd_value("SerialIoDevMode[3]", 1,
		original->SerialIoDevMode[3], params->SerialIoDevMode[3]);
	fsp_display_upd_value("SerialIoDevMode[4]", 1,
		original->SerialIoDevMode[4], params->SerialIoDevMode[4]);
	fsp_display_upd_value("SerialIoDevMode[5]", 1,
		original->SerialIoDevMode[5], params->SerialIoDevMode[5]);
	fsp_display_upd_value("SerialIoDevMode[6]", 1,
		original->SerialIoDevMode[6], params->SerialIoDevMode[6]);
	fsp_display_upd_value("SerialIoDevMode[7]", 1,
		original->SerialIoDevMode[7], params->SerialIoDevMode[7]);
	fsp_display_upd_value("SerialIoDevMode[8]", 1,
		original->SerialIoDevMode[8], params->SerialIoDevMode[8]);
	fsp_display_upd_value("SerialIoDevMode[9]", 1,
		original->SerialIoDevMode[9], params->SerialIoDevMode[9]);
	fsp_display_upd_value("SerialIoDevMode[10]", 1,
		original->SerialIoDevMode[10], params->SerialIoDevMode[10]);
	fsp_display_upd_value("ScsEmmcEnabled", 1, original->ScsEmmcEnabled,
		params->ScsEmmcEnabled);
	fsp_display_upd_value("ScsEmmcHs400Enabled", 1,
		original->ScsEmmcHs400Enabled, params->ScsEmmcHs400Enabled);
	fsp_display_upd_value("ScsSdCardEnabled", 1, original->ScsSdCardEnabled,
		params->ScsSdCardEnabled);
	fsp_display_upd_value("IshEnable", 1, original->IshEnable,
		params->IshEnable);
	fsp_display_upd_value("ShowSpiController", 1,
		original->ShowSpiController, params->ShowSpiController);
	fsp_display_upd_value("HsioMessaging", 1, original->HsioMessaging,
		params->HsioMessaging);
	fsp_display_upd_value("Heci3Enabled", 1, original->Heci3Enabled,
		params->Heci3Enabled);
	fsp_display_upd_value("EnableSata", 1, original->EnableSata,
		params->EnableSata);
	fsp_display_upd_value("SataMode", 1, original->SataMode,
		params->SataMode);
	fsp_display_upd_value("NumOfDevIntConfig", 1,
		original->NumOfDevIntConfig,
		params->NumOfDevIntConfig);
	fsp_display_upd_value("PxRcConfig[PARC]", 1,
		original->PxRcConfig[PCH_PARC],
		params->PxRcConfig[PCH_PARC]);
	fsp_display_upd_value("PxRcConfig[PBRC]", 1,
		original->PxRcConfig[PCH_PBRC],
		params->PxRcConfig[PCH_PBRC]);
	fsp_display_upd_value("PxRcConfig[PCRC]", 1,
		original->PxRcConfig[PCH_PCRC],
		params->PxRcConfig[PCH_PCRC]);
	fsp_display_upd_value("PxRcConfig[PDRC]", 1,
		original->PxRcConfig[PCH_PDRC],
		params->PxRcConfig[PCH_PDRC]);
	fsp_display_upd_value("PxRcConfig[PERC]", 1,
		original->PxRcConfig[PCH_PERC],
		params->PxRcConfig[PCH_PERC]);
	fsp_display_upd_value("PxRcConfig[PFRC]", 1,
		original->PxRcConfig[PCH_PFRC],
		params->PxRcConfig[PCH_PFRC]);
	fsp_display_upd_value("PxRcConfig[PGRC]", 1,
		original->PxRcConfig[PCH_PGRC],
		params->PxRcConfig[PCH_PGRC]);
	fsp_display_upd_value("PxRcConfig[PHRC]", 1,
		original->PxRcConfig[PCH_PHRC],
		params->PxRcConfig[PCH_PHRC]);
	fsp_display_upd_value("GpioIrqRoute", 1,
		original->GpioIrqRoute,
		params->GpioIrqRoute);
	fsp_display_upd_value("SciIrqSelect", 1,
		original->SciIrqSelect,
		params->SciIrqSelect);
	fsp_display_upd_value("TcoIrqSelect", 1,
		original->TcoIrqSelect,
		params->TcoIrqSelect);
	fsp_display_upd_value("TcoIrqEnable", 1,
		original->TcoIrqEnable,
		params->TcoIrqEnable);
	fsp_display_upd_value("LockDownConfigGlobalSmi", 1,
		original->LockDownConfigGlobalSmi,
		params->LockDownConfigGlobalSmi);
	fsp_display_upd_value("LockDownConfigBiosInterface", 1,
		original->LockDownConfigBiosInterface,
		params->LockDownConfigBiosInterface);
	fsp_display_upd_value("LockDownConfigRtcLock", 1,
		original->LockDownConfigRtcLock,
		params->LockDownConfigRtcLock);
	fsp_display_upd_value("LockDownConfigBiosLock", 1,
		original->LockDownConfigBiosLock,
		params->LockDownConfigBiosLock);
	fsp_display_upd_value("LockDownConfigSpiEiss", 1,
		original->LockDownConfigSpiEiss,
		params->LockDownConfigSpiEiss);
	fsp_display_upd_value("PchConfigSubSystemVendorId", 1,
		original->PchConfigSubSystemVendorId,
		params->PchConfigSubSystemVendorId);
	fsp_display_upd_value("PchConfigSubSystemId", 1,
		original->PchConfigSubSystemId,
		params->PchConfigSubSystemId);
	fsp_display_upd_value("WakeConfigWolEnableOverride", 1,
		original->WakeConfigWolEnableOverride,
		params->WakeConfigWolEnableOverride);
	fsp_display_upd_value("WakeConfigPcieWakeFromDeepSx", 1,
		original->WakeConfigPcieWakeFromDeepSx,
		params->WakeConfigPcieWakeFromDeepSx);
	fsp_display_upd_value("PmConfigDeepSxPol", 1,
		original->PmConfigDeepSxPol,
		params->PmConfigDeepSxPol);
	fsp_display_upd_value("PmConfigSlpS3MinAssert", 1,
		original->PmConfigSlpS3MinAssert,
		params->PmConfigSlpS3MinAssert);
	fsp_display_upd_value("PmConfigSlpS4MinAssert", 1,
		original->PmConfigSlpS4MinAssert,
		params->PmConfigSlpS4MinAssert);
	fsp_display_upd_value("PmConfigSlpSusMinAssert", 1,
		original->PmConfigSlpSusMinAssert,
		params->PmConfigSlpSusMinAssert);
	fsp_display_upd_value("PmConfigSlpAMinAssert", 1,
		original->PmConfigSlpAMinAssert,
		params->PmConfigSlpAMinAssert);
	fsp_display_upd_value("PmConfigPciClockRun", 1,
		original->PmConfigPciClockRun,
		params->PmConfigPciClockRun);
	fsp_display_upd_value("PmConfigSlpStrchSusUp", 1,
		original->PmConfigSlpStrchSusUp,
		params->PmConfigSlpStrchSusUp);
	fsp_display_upd_value("PmConfigPwrBtnOverridePeriod", 1,
		original->PmConfigPwrBtnOverridePeriod,
		params->PmConfigPwrBtnOverridePeriod);
	fsp_display_upd_value("PmConfigPwrCycDur", 1,
		original->PmConfigPwrCycDur,
		params->PmConfigPwrCycDur);
	fsp_display_upd_value("SerialIrqConfigSirqEnable", 1,
		original->SerialIrqConfigSirqEnable,
		params->SerialIrqConfigSirqEnable);
	fsp_display_upd_value("SerialIrqConfigSirqMode", 1,
		original->SerialIrqConfigSirqMode,
		params->SerialIrqConfigSirqMode);
	fsp_display_upd_value("SerialIrqConfigStartFramePulse", 1,
		original->SerialIrqConfigStartFramePulse,
		params->SerialIrqConfigStartFramePulse);

	fsp_display_upd_value("Psi1Threshold[0]", 1,
		original->Psi1Threshold[0],
		params->Psi1Threshold[0]);
	fsp_display_upd_value("Psi1Threshold[1]", 1,
		original->Psi1Threshold[1],
		params->Psi1Threshold[1]);
	fsp_display_upd_value("Psi1Threshold[2]", 1,
		original->Psi1Threshold[2],
		params->Psi1Threshold[2]);
	fsp_display_upd_value("Psi1Threshold[3]", 1,
		original->Psi1Threshold[3],
		params->Psi1Threshold[3]);
	fsp_display_upd_value("Psi1Threshold[4]", 1,
		original->Psi1Threshold[4],
		params->Psi1Threshold[4]);
	fsp_display_upd_value("Psi2Threshold[0]", 1,
		original->Psi2Threshold[0],
		params->Psi2Threshold[0]);
	fsp_display_upd_value("Psi2Threshold[1]", 1,
		original->Psi2Threshold[1],
		params->Psi2Threshold[1]);
	fsp_display_upd_value("Psi2Threshold[2]", 1,
		original->Psi2Threshold[2],
		params->Psi2Threshold[2]);
	fsp_display_upd_value("Psi2Threshold[3]", 1,
		original->Psi2Threshold[3],
		params->Psi2Threshold[3]);
	fsp_display_upd_value("Psi2Threshold[4]", 1,
		original->Psi2Threshold[4],
		params->Psi2Threshold[4]);
	fsp_display_upd_value("Psi3Threshold[0]", 1,
		original->Psi3Threshold[0],
		params->Psi3Threshold[0]);
	fsp_display_upd_value("Psi3Threshold[1]", 1,
		original->Psi3Threshold[1],
		params->Psi3Threshold[1]);
	fsp_display_upd_value("Psi3Threshold[2]", 1,
		original->Psi3Threshold[2],
		params->Psi3Threshold[2]);
	fsp_display_upd_value("Psi3Threshold[3]", 1,
		original->Psi3Threshold[3],
		params->Psi3Threshold[3]);
	fsp_display_upd_value("Psi3Threshold[4]", 1,
		original->Psi3Threshold[4],
		params->Psi3Threshold[4]);
	fsp_display_upd_value("Psi3Enable[0]", 1,
		original->Psi3Enable[0],
		params->Psi3Enable[0]);
	fsp_display_upd_value("Psi3Enable[1]", 1,
		original->Psi3Enable[1],
		params->Psi3Enable[1]);
	fsp_display_upd_value("Psi3Enable[2]", 1,
		original->Psi3Enable[2],
		params->Psi3Enable[2]);
	fsp_display_upd_value("Psi3Enable[3]", 1,
		original->Psi3Enable[3],
		params->Psi3Enable[3]);
	fsp_display_upd_value("Psi3Enable[4]", 1,
		original->Psi3Enable[4],
		params->Psi3Enable[4]);
	fsp_display_upd_value("Psi4Enable[0]", 1,
		original->Psi4Enable[0],
		params->Psi4Enable[0]);
	fsp_display_upd_value("Psi4Enable[1]", 1,
		original->Psi4Enable[1],
		params->Psi4Enable[1]);
	fsp_display_upd_value("Psi4Enable[2]", 1,
		original->Psi4Enable[2],
		params->Psi4Enable[2]);
	fsp_display_upd_value("Psi4Enable[3]", 1,
		original->Psi4Enable[3],
		params->Psi4Enable[3]);
	fsp_display_upd_value("Psi4Enable[4]", 1,
		original->Psi4Enable[4],
		params->Psi4Enable[4]);
	fsp_display_upd_value("ImonSlope[0]", 1,
		original->ImonSlope[0],
		params->ImonSlope[0]);
	fsp_display_upd_value("ImonSlope[1]", 1,
		original->ImonSlope[1],
		params->ImonSlope[1]);
	fsp_display_upd_value("ImonSlope[2]", 1,
		original->ImonSlope[2],
		params->ImonSlope[2]);
	fsp_display_upd_value("ImonSlope[3]", 1,
		original->ImonSlope[3],
		params->ImonSlope[3]);
	fsp_display_upd_value("ImonSlope[4]", 1,
		original->ImonSlope[4],
		params->ImonSlope[4]);
	fsp_display_upd_value("ImonOffse[0]t", 1,
		original->ImonOffset[0],
		params->ImonOffset[0]);
	fsp_display_upd_value("ImonOffse[1]t", 1,
		original->ImonOffset[1],
		params->ImonOffset[1]);
	fsp_display_upd_value("ImonOffse[2]t", 1,
		original->ImonOffset[2],
		params->ImonOffset[2]);
	fsp_display_upd_value("ImonOffse[3]t", 1,
		original->ImonOffset[3],
		params->ImonOffset[3]);
	fsp_display_upd_value("ImonOffse[4]t", 1,
		original->ImonOffset[4],
		params->ImonOffset[4]);
	fsp_display_upd_value("IccMax[0]", 1,
		original->IccMax[0],
		params->IccMax[0]);
	fsp_display_upd_value("IccMax[1]", 1,
		original->IccMax[1],
		params->IccMax[1]);
	fsp_display_upd_value("IccMax[2]", 1,
		original->IccMax[2],
		params->IccMax[2]);
	fsp_display_upd_value("IccMax[3]", 1,
		original->IccMax[3],
		params->IccMax[3]);
	fsp_display_upd_value("IccMax[4]", 1,
		original->IccMax[4],
		params->IccMax[4]);
	fsp_display_upd_value("VrVoltageLimit[0]", 1,
		original->VrVoltageLimit[0],
		params->VrVoltageLimit[0]);
	fsp_display_upd_value("VrVoltageLimit[1]", 1,
		original->VrVoltageLimit[1],
		params->VrVoltageLimit[1]);
	fsp_display_upd_value("VrVoltageLimit[2]", 1,
		original->VrVoltageLimit[2],
		params->VrVoltageLimit[2]);
	fsp_display_upd_value("VrVoltageLimit[3]", 1,
		original->VrVoltageLimit[3],
		params->VrVoltageLimit[3]);
	fsp_display_upd_value("VrVoltageLimit[4]", 1,
		original->VrVoltageLimit[4],
		params->VrVoltageLimit[4]);
	fsp_display_upd_value("VrConfigEnable[0]", 1,
		original->VrConfigEnable[0],
		params->VrConfigEnable[0]);
	fsp_display_upd_value("VrConfigEnable[1]", 1,
		original->VrConfigEnable[1],
		params->VrConfigEnable[1]);
	fsp_display_upd_value("VrConfigEnable[2]", 1,
		original->VrConfigEnable[2],
		params->VrConfigEnable[2]);
	fsp_display_upd_value("VrConfigEnable[3]", 1,
		original->VrConfigEnable[3],
		params->VrConfigEnable[3]);
	fsp_display_upd_value("VrConfigEnable[4]", 1,
		original->VrConfigEnable[4],
		params->VrConfigEnable[4]);
	fsp_display_upd_value("SerialIoI2cVoltage[0]", 1,
		original->SerialIoI2cVoltage[0],
		params->SerialIoI2cVoltage[0]);
	fsp_display_upd_value("SerialIoI2cVoltage[1]", 1,
		original->SerialIoI2cVoltage[1],
		params->SerialIoI2cVoltage[1]);
	fsp_display_upd_value("SerialIoI2cVoltage[2]", 1,
		original->SerialIoI2cVoltage[2],
		params->SerialIoI2cVoltage[2]);
	fsp_display_upd_value("SerialIoI2cVoltage[3]", 1,
		original->SerialIoI2cVoltage[3],
		params->SerialIoI2cVoltage[3]);
	fsp_display_upd_value("SerialIoI2cVoltage[4]", 1,
		original->SerialIoI2cVoltage[4],
		params->SerialIoI2cVoltage[4]);
	fsp_display_upd_value("SerialIoI2cVoltage[5]", 1,
		original->SerialIoI2cVoltage[5],
		params->SerialIoI2cVoltage[5]);
	fsp_display_upd_value("SendVrMbxCmd", 1,
		original->SendVrMbxCmd,
		params->SendVrMbxCmd);
	fsp_display_upd_value("AcousticNoiseMitigation", 1,
		original->AcousticNoiseMitigation,
		params->AcousticNoiseMitigation);
	fsp_display_upd_value("SlowSlewRateForIa", 1,
		original->SlowSlewRateForIa,
		params->SlowSlewRateForIa);
	fsp_display_upd_value("SlowSlewRateForGt", 1,
		original->SlowSlewRateForGt,
		params->SlowSlewRateForGt);
	fsp_display_upd_value("SlowSlewRateForSa", 1,
		original->SlowSlewRateForSa,
		params->SlowSlewRateForSa);
	fsp_display_upd_value("FastPkgCRampDisable", 1,
		original->FastPkgCRampDisable,
		params->FastPkgCRampDisable);
}
