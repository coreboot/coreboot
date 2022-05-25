/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/device.h>
#include <arch/pci_io_cfg.h>
#include <cpu/intel/cpu_ids.h>
#include <device/pci_ops.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <fsp/api.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <fsp/util.h>
#include <option.h>
#include <intelblocks/cse.h>
#include <intelblocks/irq.h>
#include <intelblocks/lpss.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/tcss.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <security/vboot/vboot_common.h>
#include <soc/gpio.h>
#include <soc/intel/common/vbt.h>
#include <soc/lpm.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>
#include <soc/tcss.h>
#include <string.h>
#include <types.h>

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
enum {
	EOP_DISABLE = 0,
	EOP_PEI = 1,
	EOP_DXE = 2,
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

/*
 * The FSP expects a certain list of PCI devices to be in the DevIntConfig table,
 * regardless of whether or not they are used by the mainboard.
 */
static const struct slot_irq_constraints irq_constraints[] = {
	{
		.slot = SA_DEV_SLOT_PEG,
		.fns = {
			FIXED_INT_PIRQ(SA_DEVFN_PEG1, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(SA_DEVFN_PEG2, PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(SA_DEVFN_PEG3, PCI_INT_C, PIRQ_C),
		},
	},
	{
		.slot = SA_DEV_SLOT_IGD,
		.fns = {
			ANY_PIRQ(SA_DEVFN_IGD),
		},
	},
	{
		.slot = SA_DEV_SLOT_DPTF,
		.fns = {
			ANY_PIRQ(SA_DEVFN_DPTF),
		},
	},
	{
		.slot = SA_DEV_SLOT_IPU,
		.fns = {
			ANY_PIRQ(SA_DEVFN_IPU),
		},
	},
	{
		.slot = SA_DEV_SLOT_CPU_PCIE,
		.fns = {
			ANY_PIRQ(SA_DEVFN_CPU_PCIE),
		},
	},
	{
		.slot = SA_DEV_SLOT_TBT,
		.fns = {
			FIXED_INT_ANY_PIRQ(SA_DEVFN_TBT0, PCI_INT_A),
			FIXED_INT_ANY_PIRQ(SA_DEVFN_TBT1, PCI_INT_B),
			FIXED_INT_ANY_PIRQ(SA_DEVFN_TBT2, PCI_INT_C),
			FIXED_INT_ANY_PIRQ(SA_DEVFN_TBT3, PCI_INT_D),
		},
	},
	{
		.slot = SA_DEV_SLOT_TCSS,
		.fns = {
			ANY_PIRQ(SA_DEVFN_TCSS_XHCI),
			ANY_PIRQ(SA_DEVFN_TCSS_DMA0),
			ANY_PIRQ(SA_DEVFN_TCSS_DMA1),
		},
	},
	{
		.slot = PCH_DEV_SLOT_SIO0,
		.fns = {
			ANY_PIRQ(PCH_DEVFN_THC0),
			ANY_PIRQ(PCH_DEVFN_THC1),
		},
	},
	{
		.slot = PCH_DEV_SLOT_SIO1,
		.fns = {
			DIRECT_IRQ(PCH_DEVFN_UART3),
		},
	},
	{
		.slot = PCH_DEV_SLOT_ISH,
		.fns = {
			DIRECT_IRQ(PCH_DEVFN_ISH),
			DIRECT_IRQ(PCH_DEVFN_GSPI2),
		},
	},
	{
		.slot = PCH_DEV_SLOT_SIO2,
		.fns = {
			DIRECT_IRQ(PCH_DEVFN_GSPI3),
		},
	},
	{
		.slot = PCH_DEV_SLOT_XHCI,
		.fns = {
			ANY_PIRQ(PCH_DEVFN_XHCI),
			FIXED_INT_ANY_PIRQ(PCH_DEVFN_CNVI_WIFI, PCI_INT_A),
		},
	},
	{
		.slot = PCH_DEV_SLOT_SIO3,
		.fns = {
			DIRECT_IRQ(PCH_DEVFN_I2C0),
			DIRECT_IRQ(PCH_DEVFN_I2C1),
			DIRECT_IRQ(PCH_DEVFN_I2C2),
			DIRECT_IRQ(PCH_DEVFN_I2C3),
		},
	},
	{
		.slot = PCH_DEV_SLOT_CSE,
		.fns = {
			ANY_PIRQ(PCH_DEVFN_CSE),
			ANY_PIRQ(PCH_DEVFN_CSE_2),
			ANY_PIRQ(PCH_DEVFN_CSE_IDER),
			ANY_PIRQ(PCH_DEVFN_CSE_KT),
			ANY_PIRQ(PCH_DEVFN_CSE_3),
			ANY_PIRQ(PCH_DEVFN_CSE_4),
		},
	},
	{
		.slot = PCH_DEV_SLOT_SATA,
		.fns = {
			ANY_PIRQ(PCH_DEVFN_SATA),
		},
	},
	{
		.slot = PCH_DEV_SLOT_SIO4,
		.fns = {
			DIRECT_IRQ(PCH_DEVFN_I2C4),
			DIRECT_IRQ(PCH_DEVFN_I2C5),
			DIRECT_IRQ(PCH_DEVFN_UART2),
		},
	},
	{
		.slot = PCH_DEV_SLOT_PCIE,
		.fns = {
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE1, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE2,	PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE3,	PCI_INT_C, PIRQ_C),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE4,	PCI_INT_D, PIRQ_D),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE5,	PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE6,	PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE7,	PCI_INT_C, PIRQ_C),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE8,	PCI_INT_D, PIRQ_D),
		},
	},
	{
		.slot = PCH_DEV_SLOT_PCIE_1,
		.fns = {
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE9,  PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE10, PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE11, PCI_INT_C, PIRQ_C),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE12, PCI_INT_D, PIRQ_D),
		},
	},
	{
		.slot = PCH_DEV_SLOT_SIO5,
		.fns = {
			FIXED_INT_ANY_PIRQ(PCH_DEVFN_UART0, PCI_INT_A),
			FIXED_INT_ANY_PIRQ(PCH_DEVFN_UART1, PCI_INT_B),
			DIRECT_IRQ(PCH_DEVFN_GSPI0),
			DIRECT_IRQ(PCH_DEVFN_GSPI1),
		},
	},
	{
		.slot = PCH_DEV_SLOT_ESPI,
		.fns = {
			ANY_PIRQ(PCH_DEVFN_HDA),
			ANY_PIRQ(PCH_DEVFN_SMBUS),
			ANY_PIRQ(PCH_DEVFN_GBE),
			FIXED_INT_ANY_PIRQ(PCH_DEVFN_TRACEHUB, PCI_INT_A),
		},
	},
};

__weak void mainboard_update_soc_chip_config(struct soc_intel_tigerlake_config *config)
{
	/* Override settings per board. */
}

static const SI_PCH_DEVICE_INTERRUPT_CONFIG *pci_irq_to_fsp(size_t *out_count)
{
	const struct pci_irq_entry *entry = get_cached_pci_irqs();
	SI_PCH_DEVICE_INTERRUPT_CONFIG *config;
	size_t pch_total = 0;
	size_t cfg_count = 0;

	if (!entry)
		return NULL;

	/* Count PCH devices */
	while (entry) {
		if (PCI_SLOT(entry->devfn) >= MIN_PCH_SLOT)
			++pch_total;
		entry = entry->next;
	}

	/* Convert PCH device entries to FSP format */
	config = calloc(pch_total, sizeof(*config));
	entry = get_cached_pci_irqs();
	while (entry) {
		if (PCI_SLOT(entry->devfn) < MIN_PCH_SLOT) {
			entry = entry->next;
			continue;
		}

		config[cfg_count].Device = PCI_SLOT(entry->devfn);
		config[cfg_count].Function = PCI_FUNC(entry->devfn);
		config[cfg_count].IntX = (SI_PCH_INT_PIN)entry->pin;
		config[cfg_count].Irq = entry->irq;
		++cfg_count;

		entry = entry->next;
	}

	*out_count = cfg_count;

	return config;
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
	params->PeiGraphicsPeimInit = CONFIG(RUN_FSP_GOP) && is_devfn_enabled(SA_DEVFN_IGD);

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

	params->UsbTcPortEn = config->UsbTcPortEn;
	params->TcssAuxOri = config->TcssAuxOri;

	/* Explicitly clear this field to avoid using defaults */
	memset(params->IomTypeCPortPadCfg, 0, sizeof(params->IomTypeCPortPadCfg));


	/* Assign PCI IRQs */
	if (!assign_pci_irqs(irq_constraints, ARRAY_SIZE(irq_constraints)))
		die("ERROR: Unable to assign PCI IRQs, and no ACPI _PRT table is defined\n");

	size_t pch_count = 0;
	const SI_PCH_DEVICE_INTERRUPT_CONFIG *upd_irqs = pci_irq_to_fsp(&pch_count);
	params->DevIntConfigPtr = (UINT32)((uintptr_t)upd_irqs);
	params->NumOfDevIntConfig = pch_count;
	printk(BIOS_INFO, "IRQ: Using dynamically assigned PCI IO-APIC IRQs\n");

	/*
	 * Set FSPS UPD ITbtConnectTopologyTimeoutInMs with value 0. FSP will
	 * evaluate this UPD value and skip sending command. There will be no
	 * delay for command completion.
	 */
	params->ITbtConnectTopologyTimeoutInMs = 0;

	/* Disable TcColdOnUsbConnect */
	params->DisableTccoldOnUsbConnected = 1;

	/* Chipset Lockdown */
	const bool lockdown_by_fsp = get_lockdown_config() == CHIPSET_LOCKDOWN_FSP;
	params->PchLockDownGlobalSmi = lockdown_by_fsp;
	params->PchLockDownBiosInterface = lockdown_by_fsp;
	params->PchUnlockGpioPads = !lockdown_by_fsp;
	params->RtcMemoryLock = lockdown_by_fsp;
	params->SkipPamLock = !lockdown_by_fsp;

	/* coreboot will send EOP before loading payload */
	params->EndOfPostMessage = EOP_DISABLE;

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

		if (config->usb2_ports[i].type_c)
			params->PortResetMessageEnable[i] = 1;
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
		params->PciePtm[i] = config->PciePtm[i];
		params->PcieRpSlotImplemented[i] = config->PcieRpSlotImplemented[i];
	}

	/* Enable ClkReqDetect for enabled port */
	memcpy(params->PcieRpClkReqDetect, config->PcieRpClkReqDetect,
		sizeof(config->PcieRpClkReqDetect));

	for (i = 0; i < ARRAY_SIZE(config->tcss_ports); i++) {
		if (config->tcss_ports[i].enable)
			params->CpuUsb3OverCurrentPin[i] =
					config->tcss_ports[i].ocpin;
	}

	params->XdciEnable = xdci_can_enable(PCH_DEVFN_USBOTG);

	/* PCH UART selection for FSP Debug */
	params->SerialIoDebugUartNumber = CONFIG_UART_FOR_CONSOLE;
	ASSERT(ARRAY_SIZE(params->SerialIoUartAutoFlow) > CONFIG_UART_FOR_CONSOLE);
	params->SerialIoUartAutoFlow[CONFIG_UART_FOR_CONSOLE] = 0;

	/* SATA */
	params->SataEnable = is_devfn_enabled(PCH_DEVFN_SATA);
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
	params->PmcLpmS0ixSubStateEnableMask = get_supported_lpm_mask(config);

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
	params->FastPkgCRampDisable[0] = config->FastPkgCRampDisable;
	params->SlowSlewRate[0] = config->SlowSlewRate;

	/* Enable TCPU for processor thermal control */
	params->Device4Enable = is_devfn_enabled(SA_DEVFN_DPTF);

	/* Set TccActivationOffset */
	params->TccActivationOffset = config->tcc_offset;

	/* LAN */
	params->PchLanEnable = is_devfn_enabled(PCH_DEVFN_GBE);

	/* CNVi */
	params->CnviMode = is_devfn_enabled(PCH_DEVFN_CNVI_WIFI);
	params->CnviBtCore = config->CnviBtCore;
	params->CnviBtAudioOffload = config->CnviBtAudioOffload;
	/* Assert if CNVi BT is enabled without CNVi being enabled. */
	assert(params->CnviMode || !params->CnviBtCore);
	/* Assert if CNVi BT offload is enabled without CNVi BT being enabled. */
	assert(params->CnviBtCore || !params->CnviBtAudioOffload);

	/* VMD */
	params->VmdEnable = is_devfn_enabled(SA_DEVFN_VMD);

	/* THC */
	params->ThcPort0Assignment = is_devfn_enabled(PCH_DEVFN_THC0) ? THC_0 : THC_NONE;
	params->ThcPort1Assignment = is_devfn_enabled(PCH_DEVFN_THC1) ? THC_1 : THC_NONE;

	/* Legacy 8254 timer support */
	bool use_8254 = get_uint_option("legacy_8254_timer", CONFIG(USE_LEGACY_8254_TIMER));
	params->Enable8254ClockGating = !use_8254;
	params->Enable8254ClockGatingOnS3 = !use_8254;

	/*
	 * Legacy PM ACPI Timer (and TCO Timer)
	 * This *must* be 1 in any case to keep FSP from
	 *  1) enabling PM ACPI Timer emulation in uCode.
	 *  2) disabling the PM ACPI Timer.
	 * We handle both by ourself!
	 */
	params->EnableTcoTimer = 1;

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
		params->ITbtPcieRootPortEn[i] = is_dev_enabled(dev);
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

	/* Override EnableMultiPhaseSiliconInit prior calling MultiPhaseSiInit */
	params->EnableMultiPhaseSiliconInit = fsp_is_multi_phase_init_enabled();

	/* Disable C1 C-state Demotion */
	params->C1StateAutoDemotion = 0;

	/* USB2 Phy Sus power gating setting override */
	params->PmcUsb2PhySusPgEnable = !config->usb2_phy_sus_pg_disable;

	/*
	 * Prevent FSP from programming write-once subsystem IDs by providing
	 * a custom SSID table. Must have at least one entry for the FSP to
	 * use the table.
	 */
	struct svid_ssid_init_entry {
		union {
			struct {
				uint64_t reg:12;	/* Register offset */
				uint64_t function:3;
				uint64_t device:5;
				uint64_t bus:8;
				uint64_t :4;
				uint64_t segment:16;
				uint64_t :16;
			};
			uint64_t segbusdevfuncregister;
		};
		struct {
			uint16_t svid;
			uint16_t ssid;
		};
		uint32_t reserved;
	};

	/*
	 * The xHCI and HDA devices have RW/L rather than RW/O registers for
	 * subsystem IDs and so must be written before FspSiliconInit locks
	 * them with their default values.
	 */
	const pci_devfn_t devfn_table[] = { PCH_DEVFN_XHCI, PCH_DEVFN_HDA };
	static struct svid_ssid_init_entry ssid_table[ARRAY_SIZE(devfn_table)];

	for (i = 0; i < ARRAY_SIZE(devfn_table); i++) {
		ssid_table[i].reg	= PCI_SUBSYSTEM_VENDOR_ID;
		ssid_table[i].device	= PCI_SLOT(devfn_table[i]);
		ssid_table[i].function	= PCI_FUNC(devfn_table[i]);
		dev = pcidev_path_on_root(devfn_table[i]);
		if (dev) {
			ssid_table[i].svid = dev->subsystem_vendor;
			ssid_table[i].ssid = dev->subsystem_device;
		}
	}

	params->SiSsidTablePtr = (uintptr_t)ssid_table;
	params->SiNumberOfSsidTableEntry = ARRAY_SIZE(ssid_table);

	/*
	 * Replace the default SVID:SSID value with the values specified in
	 * the devicetree for the root device.
	 */
	dev = pcidev_path_on_root(SA_DEVFN_ROOT);
	params->SiCustomizedSvid = dev->subsystem_vendor;
	params->SiCustomizedSsid = dev->subsystem_device;

	/* Ensure FSP will program the registers */
	params->SiSkipSsidProgramming = 0;

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
__weak void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
