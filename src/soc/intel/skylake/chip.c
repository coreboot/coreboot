/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootsplash.h>
#include <fsp/api.h>
#include <acpi/acpi.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <fsp/util.h>
#include <gpio.h>
#include <option.h>
#include <intelblocks/acpi.h>
#include <intelblocks/cfg.h>
#include <intelblocks/itss.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcie_rp.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/xdci.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/systemagent.h>
#include <intelpch/lockdown.h>
#include <soc/intel/common/vbt.h>
#include <soc/interrupt.h>
#include <soc/iomap.h>
#include <soc/irq.h>
#include <soc/itss.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/systemagent.h>
#include <soc/usb.h>
#include <string.h>
#include <types.h>

#include "chip.h"

static const struct pcie_rp_group pch_lp_rp_groups[] = {
	{ .slot = PCH_DEV_SLOT_PCIE,	.count = 8, .lcap_port_base = 1 },
	{ .slot = PCH_DEV_SLOT_PCIE_1,	.count = 4, .lcap_port_base = 1 },
	{ 0 }
};

static const struct pcie_rp_group pch_h_rp_groups[] = {
	{ .slot = PCH_DEV_SLOT_PCIE,	.count = 8, .lcap_port_base = 1 },
	{ .slot = PCH_DEV_SLOT_PCIE_1,	.count = 8, .lcap_port_base = 1 },
	/* Sunrise Point PCH-H actually only has 4 ports in the
	   third group. But that would require a runtime check
	   and probing 4 non-existent ports shouldn't hurt. */
	{ .slot = PCH_DEV_SLOT_PCIE_2,	.count = 8, .lcap_port_base = 1 },
	{ 0 }
};

#if CONFIG(HAVE_ACPI_TABLES)
const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type == DEVICE_PATH_USB) {
		switch (dev->path.usb.port_type) {
		case 0:
			/* Root Hub */
			return "RHUB";
		case 2:
			/* USB2 ports */
			switch (dev->path.usb.port_id) {
			case 0: return "HS01";
			case 1: return "HS02";
			case 2: return "HS03";
			case 3: return "HS04";
			case 4: return "HS05";
			case 5: return "HS06";
			case 6: return "HS07";
			case 7: return "HS08";
			case 8: return "HS09";
			case 9: return "HS10";
			}
			break;
		case 3:
			/* USB3 ports */
			switch (dev->path.usb.port_id) {
			case 0: return "SS01";
			case 1: return "SS02";
			case 2: return "SS03";
			case 3: return "SS04";
			case 4: return "SS05";
			case 5: return "SS06";
			}
			break;
		}
		return NULL;
	}

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	/* Match functions 0 and 1 for possible GPUs on a secondary bus */
	if (dev->bus && dev->bus->secondary > 0) {
		switch (PCI_FUNC(dev->path.pci.devfn)) {
		case 0: return "DEV0";
		case 1: return "DEV1";
		}
		return NULL;
	}

	switch (dev->path.pci.devfn) {
	case SA_DEVFN_ROOT:	return "MCHC";
	case SA_DEVFN_PEG0:	return "PEGP";
	case SA_DEVFN_IGD:	return "GFX0";
	case PCH_DEVFN_ISH:	return "ISHB";
	case PCH_DEVFN_XHCI:	return "XHCI";
	case PCH_DEVFN_USBOTG:	return "XDCI";
	case PCH_DEVFN_THERMAL:	return "THRM";
	case PCH_DEVFN_CIO:	return "ICIO";
	case PCH_DEVFN_I2C0:	return "I2C0";
	case PCH_DEVFN_I2C1:	return "I2C1";
	case PCH_DEVFN_I2C2:	return "I2C2";
	case PCH_DEVFN_I2C3:	return "I2C3";
	case PCH_DEVFN_CSE:	return "CSE1";
	case PCH_DEVFN_CSE_2:	return "CSE2";
	case PCH_DEVFN_CSE_IDER:	return "CSED";
	case PCH_DEVFN_CSE_KT:	return "CSKT";
	case PCH_DEVFN_CSE_3:	return "CSE3";
	case PCH_DEVFN_SATA:	return "SATA";
	case PCH_DEVFN_UART2:	return "UAR2";
	case PCH_DEVFN_I2C4:	return "I2C4";
	case PCH_DEVFN_I2C5:	return "I2C5";
	case PCH_DEVFN_PCIE1:	return "RP01";
	case PCH_DEVFN_PCIE2:	return "RP02";
	case PCH_DEVFN_PCIE3:	return "RP03";
	case PCH_DEVFN_PCIE4:	return "RP04";
	case PCH_DEVFN_PCIE5:	return "RP05";
	case PCH_DEVFN_PCIE6:	return "RP06";
	case PCH_DEVFN_PCIE7:	return "RP07";
	case PCH_DEVFN_PCIE8:	return "RP08";
	case PCH_DEVFN_PCIE9:	return "RP09";
	case PCH_DEVFN_PCIE10:	return "RP10";
	case PCH_DEVFN_PCIE11:	return "RP11";
	case PCH_DEVFN_PCIE12:	return "RP12";
	case PCH_DEVFN_PCIE13:	return "RP13";
	case PCH_DEVFN_PCIE14:	return "RP14";
	case PCH_DEVFN_PCIE15:	return "RP15";
	case PCH_DEVFN_PCIE16:	return "RP16";
	case PCH_DEVFN_UART0:	return "UAR0";
	case PCH_DEVFN_UART1:	return "UAR1";
	case PCH_DEVFN_GSPI0:	return "SPI0";
	case PCH_DEVFN_GSPI1:	return "SPI1";
	case PCH_DEVFN_EMMC:	return "EMMC";
	case PCH_DEVFN_SDIO:	return "SDIO";
	case PCH_DEVFN_SDCARD:	return "SDXC";
	case PCH_DEVFN_P2SB:	return "P2SB";
	case PCH_DEVFN_PMC:	return "PMC_";
	case PCH_DEVFN_HDA:	return "HDAS";
	case PCH_DEVFN_SMBUS:	return "SBUS";
	case PCH_DEVFN_SPI:	return "FSPI";
	case PCH_DEVFN_GBE:	return "IGBE";
	case PCH_DEVFN_TRACEHUB:return "THUB";
	}

	return NULL;
}
#endif

void soc_init_pre_device(void *chip_info)
{
	/* Snapshot the current GPIO IRQ polarities. FSP is setting a
	 * default policy that doesn't honor boards' requirements. */
	itss_snapshot_irq_polarities(GPIO_IRQ_START, GPIO_IRQ_END);

	/* Perform silicon specific init. */
	fsp_silicon_init();

	/*
	 * Keep the P2SB device visible so it and the other devices are
	 * visible in coreboot for driver support and PCI resource allocation.
	 * There is no UPD setting for this.
	 */
	p2sb_unhide();

	/* Restore GPIO IRQ polarities back to previous settings. */
	itss_restore_irq_polarities(GPIO_IRQ_START, GPIO_IRQ_END);

	/* swap enabled PCI ports in device tree if needed */
	if (CONFIG(SKYLAKE_SOC_PCH_H))
		pcie_rp_update_devicetree(pch_h_rp_groups);
	else
		pcie_rp_update_devicetree(pch_lp_rp_groups);
}

struct device_operations pci_domain_ops = {
	.read_resources   = &pci_domain_read_resources,
	.set_resources    = &pci_domain_set_resources,
	.scan_bus         = &pci_domain_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name        = &soc_acpi_name,
	.acpi_fill_ssdt   = ssdt_set_above_4g_pci,
#endif
};

static struct device_operations cpu_bus_ops = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt   = generate_cpu_entries,
#endif
};

static void soc_enable(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
}

struct chip_operations soc_intel_skylake_ops = {
	CHIP_NAME("Intel 6th Gen")
	.enable_dev	= &soc_enable,
	.init		= &soc_init_pre_device,
};

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	FSP_S_CONFIG *params = &supd->FspsConfig;
	FSP_S_TEST_CONFIG *tconfig = &supd->FspsTestConfig;
	struct soc_intel_skylake_config *config;
	uintptr_t vbt_data = (uintptr_t)vbt_get();
	int i;

	config = config_of_soc();

	mainboard_silicon_init_params(params);

	struct soc_power_limits_config *soc_confg;
	config_t *confg = config_of_soc();
	soc_confg = &confg->power_limits_config;
	/* Set PsysPmax if it is available from DT */
	if (soc_confg->psys_pmax) {
		/* PsysPmax is in unit of 1/8 Watt */
		tconfig->PsysPmax = soc_confg->psys_pmax * 8;
		printk(BIOS_DEBUG, "psys_pmax = %d\n", tconfig->PsysPmax);
	}

	params->GraphicsConfigPtr = (u32)vbt_data;

	for (i = 0; i < ARRAY_SIZE(config->usb2_ports); i++) {
		params->PortUsb20Enable[i] =
				config->usb2_ports[i].enable;
		params->Usb2AfePetxiset[i] =
				config->usb2_ports[i].pre_emp_bias;
		params->Usb2AfeTxiset[i] =
				config->usb2_ports[i].tx_bias;
		params->Usb2AfePredeemp[i] =
				config->usb2_ports[i].tx_emp_enable;
		params->Usb2AfePehalfbit[i] =
				config->usb2_ports[i].pre_emp_bit;

		if (config->usb2_ports[i].enable)
			params->Usb2OverCurrentPin[i] = config->usb2_ports[i].ocpin;
		else
			params->Usb2OverCurrentPin[i] = OC_SKIP;
	}

	for (i = 0; i < ARRAY_SIZE(config->usb3_ports); i++) {
		params->PortUsb30Enable[i] = config->usb3_ports[i].enable;
		if (config->usb3_ports[i].enable)
			params->Usb3OverCurrentPin[i] = config->usb3_ports[i].ocpin;
		else
			params->Usb3OverCurrentPin[i] = OC_SKIP;

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

	params->SataEnable = is_devfn_enabled(PCH_DEVFN_SATA);
	if (params->SataEnable) {
		memcpy(params->SataPortsEnable, config->SataPortsEnable,
				sizeof(params->SataPortsEnable));
		memcpy(params->SataPortsDevSlp, config->SataPortsDevSlp,
				sizeof(params->SataPortsDevSlp));
		memcpy(params->SataPortsHotPlug, config->SataPortsHotPlug,
				sizeof(params->SataPortsHotPlug));
		memcpy(params->SataPortsSpinUp, config->SataPortsSpinUp,
				sizeof(params->SataPortsSpinUp));

		params->SataSalpSupport = config->SataSalpSupport;
		params->SataMode = config->SataMode;
		params->SataSpeedLimit = config->SataSpeedLimit;
		/*
		 * For unknown reasons FSP skips writing some essential SATA init registers
		 * (SIR) when SataPwrOptEnable=0. This results in link errors, "unaligned
		 * write" errors and others. Enabling this option solves these problems.
		 */
		params->SataPwrOptEnable = 1;
		tconfig->SataTestMode = CONFIG(ENABLE_SATA_TEST_MODE);
	}

	memcpy(params->PcieRpClkReqSupport, config->PcieRpClkReqSupport,
	       sizeof(params->PcieRpClkReqSupport));
	memcpy(params->PcieRpClkReqNumber, config->PcieRpClkReqNumber,
	       sizeof(params->PcieRpClkReqNumber));
	memcpy(params->PcieRpAdvancedErrorReporting,
		config->PcieRpAdvancedErrorReporting,
			sizeof(params->PcieRpAdvancedErrorReporting));
	memcpy(params->PcieRpLtrEnable, config->PcieRpLtrEnable,
	       sizeof(params->PcieRpLtrEnable));
	memcpy(params->PcieRpHotPlug, config->PcieRpHotPlug,
	       sizeof(params->PcieRpHotPlug));
	for (i = 0; i < CONFIG_MAX_ROOT_PORTS; i++) {
		params->PcieRpMaxPayload[i] = config->PcieRpMaxPayload[i];
		if (config->pcie_rp_aspm[i])
			params->PcieRpAspm[i] = config->pcie_rp_aspm[i] - 1;
		if (config->pcie_rp_l1substates[i])
			params->PcieRpL1Substates[i] = config->pcie_rp_l1substates[i] - 1;
	}

	/*
	 * PcieRpClkSrcNumber UPD is set to clock source number(0-6) for
	 * all the enabled PCIe root ports, invalid(0x1F) is set for
	 * disabled PCIe root ports.
	 */
	for (i = 0; i < CONFIG_MAX_ROOT_PORTS; i++) {
		if (config->PcieRpClkReqSupport[i])
			params->PcieRpClkSrcNumber[i] =
				config->PcieRpClkSrcNumber[i];
		else
			params->PcieRpClkSrcNumber[i] = 0x1F;
	}

	/* disable Legacy PME */
	memset(params->PcieRpPmSci, 0, sizeof(params->PcieRpPmSci));

	/* Legacy 8254 timer support */
	bool use_8254 = get_uint_option("legacy_8254_timer", CONFIG(USE_LEGACY_8254_TIMER));
	params->Early8254ClockGatingEnable = !use_8254;

	/*
	 * Legacy PM ACPI Timer (and TCO Timer)
	 * This *must* be 1 in any case to keep FSP from
	 *  1) enabling PM ACPI Timer emulation in uCode.
	 *  2) disabling the PM ACPI Timer.
	 * We handle both by ourself!
	 */
	params->EnableTcoTimer = 1;

	memcpy(params->SerialIoDevMode, config->SerialIoDevMode,
	       sizeof(params->SerialIoDevMode));

	params->PchCio2Enable = is_devfn_enabled(PCH_DEVFN_CIO);

	params->SaImguEnable = is_devfn_enabled(SA_DEVFN_IMGU);

	tconfig->ChapDeviceEnable = is_devfn_enabled(SA_DEVFN_CHAP);

	params->Heci3Enabled = is_devfn_enabled(PCH_DEVFN_CSE_3);

	params->CpuConfig.Bits.VmxEnable = CONFIG(ENABLE_VMX);

	params->PchPmWoWlanEnable = config->PchPmWoWlanEnable;
	params->PchPmWoWlanDeepSxEnable = config->PchPmWoWlanDeepSxEnable;
	params->PchPmLanWakeFromDeepSx = config->WakeConfigPcieWakeFromDeepSx;

	params->PchLanEnable = is_devfn_enabled(PCH_DEVFN_GBE);
	if (params->PchLanEnable) {
		params->PchLanLtrEnable = config->EnableLanLtr;
		params->PchLanK1OffEnable = config->EnableLanK1Off;
		params->PchLanClkReqSupported = config->LanClkReqSupported;
		params->PchLanClkReqNumber = config->LanClkReqNumber;
	}
	params->SsicPortEnable = config->SsicPortEnable;

	params->ScsEmmcEnabled = is_devfn_enabled(PCH_DEVFN_EMMC);
	params->ScsEmmcHs400Enabled = config->ScsEmmcHs400Enabled;

	params->ScsSdCardEnabled = is_devfn_enabled(PCH_DEVFN_SDCARD);

	if (!!params->ScsEmmcHs400Enabled && !!config->EmmcHs400DllNeed) {
		params->PchScsEmmcHs400DllDataValid =
			!!config->EmmcHs400DllNeed;
		params->PchScsEmmcHs400RxStrobeDll1 =
			config->ScsEmmcHs400RxStrobeDll1;
		params->PchScsEmmcHs400TxDataDll =
			config->ScsEmmcHs400TxDataDll;
	}

	/* If ISH is enabled, enable ISH elements */
	params->PchIshEnable = is_devfn_enabled(PCH_DEVFN_ISH);

	params->PchHdaEnable = is_devfn_enabled(PCH_DEVFN_HDA);

	params->PchHdaVcType = config->PchHdaVcType;
	params->PchHdaIoBufferOwnership = config->IoBufferOwnership;
	params->PchHdaDspEnable = config->DspEnable;

	params->Device4Enable = is_devfn_enabled(SA_DEVFN_TS);
	params->PchThermalDeviceEnable = is_devfn_enabled(PCH_DEVFN_THERMAL);

	tconfig->PchLockDownGlobalSmi = config->LockDownConfigGlobalSmi;
	tconfig->PchLockDownRtcLock = config->LockDownConfigRtcLock;
	tconfig->PowerLimit4 = 0;
	/*
	 * To disable HECI, the Psf needs to be left unlocked
	 * by FSP till end of post sequence. Based on the config
	 * setting, we set the appropriate PsfUnlock policy in FSP,
	 * do the changes and then lock it back in coreboot during finalize.
	 */
	tconfig->PchSbAccessUnlock = CONFIG(DISABLE_HECI1_AT_PRE_BOOT);

	const bool lockdown_by_fsp = get_lockdown_config() == CHIPSET_LOCKDOWN_FSP;
	tconfig->PchLockDownBiosInterface = lockdown_by_fsp;
	params->PchLockDownBiosLock = lockdown_by_fsp;
	params->PchLockDownSpiEiss = lockdown_by_fsp;
	/*
	 * Making this config "0" means FSP won't set the FLOCKDN bit
	 * of SPIBAR + 0x04 (i.e., Bit 15 of BIOS_HSFSTS_CTL).
	 * So, it becomes coreboot's responsibility to set this bit
	 * before end of POST for security concerns.
	 */
	params->SpiFlashCfgLockDown = lockdown_by_fsp;

	/* FSP should let coreboot set subsystem IDs, which are read/write-once */
	params->DefaultSvid = 0;
	params->PchSubSystemVendorId = 0;
	params->DefaultSid = 0;
	params->PchSubSystemId = 0;

	params->PchPmWolEnableOverride = config->WakeConfigWolEnableOverride;
	params->PchPmPcieWakeFromDeepSx = config->WakeConfigPcieWakeFromDeepSx;
	params->PchPmDeepSxPol = config->PmConfigDeepSxPol;
	params->PchPmSlpS0Enable = config->s0ix_enable;
	params->PchPmSlpS3MinAssert = config->PmConfigSlpS3MinAssert;
	params->PchPmSlpS4MinAssert = config->PmConfigSlpS4MinAssert;
	params->PchPmSlpSusMinAssert = config->PmConfigSlpSusMinAssert;
	params->PchPmSlpAMinAssert = config->PmConfigSlpAMinAssert;
	params->PchPmSlpStrchSusUp = config->PmConfigSlpStrchSusUp;
	params->PchPmPwrBtnOverridePeriod =
				config->PmConfigPwrBtnOverridePeriod;
	params->PchPmPwrCycDur = config->PmConfigPwrCycDur;

	/* Indicate whether platform supports Voltage Margining */
	params->PchPmSlpS0VmEnable = config->PchPmSlpS0VmEnable;

	params->PchSirqEnable = config->serirq_mode != SERIRQ_OFF;
	params->PchSirqMode = config->serirq_mode == SERIRQ_CONTINUOUS;

	params->CpuConfig.Bits.SkipMpInit = !CONFIG(USE_INTEL_FSP_MP_INIT);

	for (i = 0; i < ARRAY_SIZE(config->i2c_voltage); i++)
		params->SerialIoI2cVoltage[i] = config->i2c_voltage[i];

	for (i = 0; i < ARRAY_SIZE(config->domain_vr_config); i++)
		fill_vr_domain_config(params, i, &config->domain_vr_config[i]);

	/* Show SPI controller if enabled in devicetree.cb */
	params->ShowSpiController = is_devfn_enabled(PCH_DEVFN_SPI);

	params->XdciEnable = xdci_can_enable(PCH_DEVFN_USBOTG);

	/* Enable or disable Gaussian Mixture Model in devicetree */
	params->GmmEnable = is_devfn_enabled(SA_DEVFN_GMM);

	/*
	 * Send VR specific mailbox commands:
	 * 000b - no VR specific command sent
	 * 001b - VR mailbox command specifically for the MPS IMPV8 VR
	 *	  will be sent
	 * 010b - VR specific command sent for PS4 exit issue
	 * 100b - VR specific command sent for MPS VR decay issue
	 */
	params->SendVrMbxCmd1 = config->SendVrMbxCmd;

	/*
	 * Activates VR mailbox command for Intersil VR C-state issues.
	 * 0 - no mailbox command sent.
	 * 1 - VR mailbox command sent for IA/GT rails only.
	 * 2 - VR mailbox command sent for IA/GT/SA rails.
	 */
	params->IslVrCmd = config->IslVrCmd;

	/* Acoustic Noise Mitigation */
	params->AcousticNoiseMitigation = config->AcousticNoiseMitigation;
	params->SlowSlewRateForIa = config->SlowSlewRateForIa;
	params->SlowSlewRateForGt = config->SlowSlewRateForGt;
	params->SlowSlewRateForSa = config->SlowSlewRateForSa;
	params->FastPkgCRampDisableIa = config->FastPkgCRampDisableIa;
	params->FastPkgCRampDisableGt = config->FastPkgCRampDisableGt;
	params->FastPkgCRampDisableSa = config->FastPkgCRampDisableSa;

	/* Enable PMC XRAM read */
	tconfig->PchPmPmcReadDisable = config->PchPmPmcReadDisable;

	/* Enable/Disable EIST */
	tconfig->Eist = config->eist_enable;

	/* Set TccActivationOffset */
	tconfig->TccActivationOffset = config->tcc_offset;

	/* Already handled in coreboot code, so tell FSP to ignore UPDs */
	params->PchIoApicBdfValid = 0;

	/* Enable VT-d and X2APIC */
	if (soc_vtd_enabled()) {
		params->VtdBaseAddress[0] = GFXVT_BASE_ADDRESS;
		params->VtdBaseAddress[1] = VTVC0_BASE_ADDRESS;
		params->X2ApicOptOut = 0;
		tconfig->VtdDisable = 0;
	}

	params->PeiGraphicsPeimInit = CONFIG(RUN_FSP_GOP) && is_devfn_enabled(SA_DEVFN_IGD);

	params->PavpEnable = CONFIG(PAVP);

	soc_irq_settings(params);
}

/* Mainboard FSP Configuration */
__weak void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

/* Handle FSP logo params */
void soc_load_logo(FSPS_UPD *supd)
{
	bmp_load_logo(&supd->FspsConfig.LogoPtr, &supd->FspsConfig.LogoSize);
}
