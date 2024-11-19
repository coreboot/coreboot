/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include <cpu/intel/microcode.h>
#include <fsp/api.h>
#include <fsp/debug.h>
#include <fsp/fsp_debug_event.h>
#include <fsp/fsp_gop_blt.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <intelblocks/irq.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <option.h>
#include <soc/cpu.h>
#include <soc/intel/common/vbt.h>
#include <soc/pcie.h>
#include <soc/ramstage.h>
#include <static.h>

#define MAX_ONBOARD_PCIE_DEVICES 256

/* THC assignment definition */
enum {
	THC_NONE,
	THC_0,
	THC_1
};

/* LPSS UART Power Gating mode */
enum {
	LPSS_UART_PG_DISABLED,
	LPSS_UART_PG_ENABLED,
	LPSS_UART_PG_AUTO
};

static const pci_devfn_t i2c_dev[] = {
	PCI_DEVFN_I2C0,
	PCI_DEVFN_I2C1,
	PCI_DEVFN_I2C2,
	PCI_DEVFN_I2C3,
	PCI_DEVFN_I2C4,
	PCI_DEVFN_I2C5,
};

static const pci_devfn_t uart_dev[] = {
	PCI_DEVFN_UART0,
	PCI_DEVFN_UART1,
	PCI_DEVFN_UART2
};

static const pci_devfn_t gspi_dev[] = {
	PCI_DEVFN_GSPI0,
	PCI_DEVFN_GSPI1,
	PCI_DEVFN_GSPI2
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
	if (CONFIG(SOC_INTEL_COMPLIANCE_TEST_MODE))
		ctl = L1_SS_DISABLED;
	else if (ctl > L1_SS_L1_2 || ctl == L1_SS_FSP_DEFAULT)
		ctl = L1_SS_L1_2;
	return ctl - 1;
}

/*
 * Chip config parameter pcie_rp_aspm uses (UPD value + 1) because
 * a UPD value of 0 for pcie_rp_aspm means disabled. In order to ensure
 * that the mainboard setting does not disable ASPM incorrectly, chip
 * config parameter values are offset by 1 with 0 meaning use FSP UPD default.
 * get_aspm_control() ensures that the right UPD value is set in fsp_params.
 * 0: Use FSP UPD default
 * 1: Disable ASPM
 * 2: L0s only
 * 3: L1 only
 * 4: L0s and L1
 * 5: Auto configuration
 */
static unsigned int get_aspm_control(enum ASPM_control ctl)
{
	if (ctl > ASPM_AUTO || ctl == ASPM_DEFAULT)
		ctl = ASPM_AUTO;
	return ctl - 1;
}

__weak void mainboard_update_soc_chip_config(struct soc_intel_pantherlake_config *config)
{
	/* Override settings per board. */
}

static const struct slot_irq_constraints irq_constraints[] = {
	{
		.slot = PCI_DEV_SLOT_IGD,
		.fns = {
			/* INTERRUPT_PIN is RO/0x01 */
			FIXED_INT_ANY_PIRQ(PCI_DEVFN_IGD, PCI_INT_A),
		},
	},
	{
		.slot = PCI_DEV_SLOT_DPTF,
		.fns = {
			/* Dynamic Tuning Technology (DTT) device IRQ is not
			   programmable and is INT_A/PIRQ_A (IRQ 16) */
			FIXED_INT_PIRQ(PCI_DEVFN_DPTF, PCI_INT_A, PIRQ_A),
		},
	},
	{
		.slot = PCI_DEV_SLOT_IPU,
		.fns = {
			/* INTERRUPT_PIN is RO/0x01, and INTERRUPT_LINE is RW,
			   but S0ix fails when not set to 16 (b/193434192) */
			FIXED_INT_PIRQ(PCI_DEVFN_IPU, PCI_INT_A, PIRQ_A),
		},
	},
	{
		.slot = PCI_DEV_SLOT_PCIE_2,
		.fns = {
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE9, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE10, PCI_INT_B, PIRQ_B),
#if CONFIG(SOC_INTEL_PANTHERLAKE_H)
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE11, PCI_INT_C, PIRQ_C),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE12, PCI_INT_D, PIRQ_D),
#endif
		},
	},
	{
		.slot = PCI_DEV_SLOT_TBT,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_TBT0),
			ANY_PIRQ(PCI_DEVFN_TBT1),
			ANY_PIRQ(PCI_DEVFN_TBT2),
			ANY_PIRQ(PCI_DEVFN_TBT3),
		},
	},
	{
		.slot = PCI_DEV_SLOT_NPU,
		.fns = {
			/* INTERRUPT_PIN is RO/0x01 */
			FIXED_INT_ANY_PIRQ(PCI_DEVFN_NPU, PCI_INT_A),
		},
	},
	{
		.slot = PCI_DEV_SLOT_TCSS,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_TCSS_XHCI),
		},
	},
	{
		.slot = PCI_DEV_SLOT_THC,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_THC0),
			ANY_PIRQ(PCI_DEVFN_THC1),
		},
	},
	{
		.slot = PCI_DEV_SLOT_ISH,
		.fns = {
			DIRECT_IRQ(PCI_DEVFN_ISH),
		},
	},
	{
		.slot = PCI_DEV_SLOT_XHCI,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_XHCI),
			DIRECT_IRQ(PCI_DEVFN_USBOTG),
			ANY_PIRQ(PCI_DEVFN_CNVI_WIFI),
		},
	},
	{
		.slot = PCI_DEV_SLOT_SIO0,
		.fns = {
			DIRECT_IRQ(PCI_DEVFN_I2C0),
			DIRECT_IRQ(PCI_DEVFN_I2C1),
			DIRECT_IRQ(PCI_DEVFN_I2C2),
			DIRECT_IRQ(PCI_DEVFN_I2C3),
		},
	},
	{
		.slot = PCI_DEV_SLOT_CSE,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_CSE),
			ANY_PIRQ(PCI_DEVFN_CSE_2),
			ANY_PIRQ(PCI_DEVFN_CSE_IDER),
			ANY_PIRQ(PCI_DEVFN_CSE_KT),
			ANY_PIRQ(PCI_DEVFN_CSE_3),
			ANY_PIRQ(PCI_DEVFN_CSE_4),
		},
	},
#if CONFIG(SOC_INTEL_PANTHERLAKE_U_H)
	{
		.slot = PCI_DEV_SLOT_UFS,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_UFS),
		},
	},
#endif
	{
		.slot = PCI_DEV_SLOT_SIO1,
		.fns = {
			DIRECT_IRQ(PCI_DEVFN_I2C4),
			DIRECT_IRQ(PCI_DEVFN_I2C5),
			DIRECT_IRQ(PCI_DEVFN_UART2),
		},
	},
	{
		.slot = PCI_DEV_SLOT_PCIE_1,
		.fns = {
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE1, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE2, PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE3, PCI_INT_C, PIRQ_C),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE4, PCI_INT_D, PIRQ_D),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE5, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE6, PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE7, PCI_INT_C, PIRQ_C),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE8, PCI_INT_D, PIRQ_D),
		},
	},
	{
		.slot = PCI_DEV_SLOT_SIO2,
		.fns = {
			/* UART0 shares an interrupt line with TSN0, so must use
			   a PIRQ */
			FIXED_INT_ANY_PIRQ(PCI_DEVFN_UART0, PCI_INT_A),
			/* UART1 shares an interrupt line with TSN1, so must use
			   a PIRQ */
			FIXED_INT_ANY_PIRQ(PCI_DEVFN_UART1, PCI_INT_B),
			DIRECT_IRQ(PCI_DEVFN_GSPI0),
			DIRECT_IRQ(PCI_DEVFN_GSPI1),
		},
	},
	{
		.slot = PCI_DEV_SLOT_ESPI,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_HDA),
			ANY_PIRQ(PCI_DEVFN_SMBUS),
			ANY_PIRQ(PCI_DEVFN_GBE),
			/* INTERRUPT_PIN is RO/0x01 */
			FIXED_INT_ANY_PIRQ(PCI_DEVFN_NPK, PCI_INT_A),
		},
	},
};

static void fill_fsps_lpss_params(FSP_S_CONFIG *s_cfg,
				  const struct soc_intel_pantherlake_config *config)
{
	size_t i;

	for (i = 0; i < CONFIG_SOC_INTEL_I2C_DEV_MAX; i++)
		s_cfg->SerialIoI2cMode[i] =
			is_devfn_enabled(i2c_dev[i]) ? config->serial_io_i2c_mode[i] : 0;

	for (i = 0; i < CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX; i++) {
		s_cfg->SerialIoLpssSpiCsMode[i] = config->serial_io_gspi_cs_mode[i];
		s_cfg->SerialIoLpssSpiCsState[i] = config->serial_io_gspi_cs_state[i];
		s_cfg->SerialIoLpssSpiMode[i] =
			is_devfn_enabled(gspi_dev[i]) ? config->serial_io_gspi_mode[i] : 0;
	}

	for (i = 0; i < CONFIG_SOC_INTEL_UART_DEV_MAX; i++) {
		s_cfg->SerialIoUartMode[i] = is_devfn_enabled(uart_dev[i]) ?
			config->serial_io_uart_mode[i] : 0;
		s_cfg->SerialIoUartPowerGating[i] = is_devfn_enabled(uart_dev[i]) ?
			LPSS_UART_PG_ENABLED : LPSS_UART_PG_AUTO;
	}
}

static void fill_fsps_cpu_params(FSP_S_CONFIG *s_cfg,
				 const struct soc_intel_pantherlake_config *config)
{
	if (!CONFIG(USE_INTEL_FSP_TO_CALL_COREBOOT_PUBLISH_MP_PPI))
		return;

	s_cfg->CpuMpPpi = (uintptr_t)mp_fill_ppi_services_data();
}

static void fill_fsps_microcode_params(FSP_S_CONFIG *s_cfg,
				 const struct soc_intel_pantherlake_config *config)
{
	const struct microcode *microcode;
	size_t length;

	if (!CONFIG(USE_FSP_FEATURE_PROGRAM_ON_APS))
		return;

	/* Locate microcode and pass to FSP-S for 2nd microcode loading */
	microcode = intel_microcode_find();
	if (!microcode)
		return;

	length = get_microcode_size(microcode);
	if (!length)
		return;

	/* Update CPU Microcode patch base address/size */
	s_cfg->MicrocodeRegionBase = (uint32_t)(uintptr_t)microcode;
	s_cfg->MicrocodeRegionSize = (uint32_t)length;
}

static void fill_fsps_igd_params(FSP_S_CONFIG *s_cfg,
				 const struct soc_intel_pantherlake_config *config)
{
	/* Load VBT before devicetree-specific config. */
	s_cfg->GraphicsConfigPtr = (uintptr_t)vbt_get();

	/* Check if IGD is present and fill Graphics init param accordingly */
	s_cfg->PeiGraphicsPeimInit = CONFIG(RUN_FSP_GOP) && is_devfn_enabled(PCI_DEVFN_IGD);
	s_cfg->LidStatus = CONFIG(VBOOT_LID_SWITCH) ? get_lid_switch() : CONFIG(RUN_FSP_GOP);
	s_cfg->PavpEnable = CONFIG(PAVP);
}

static void fill_fsps_tcss_params(FSP_S_CONFIG *s_cfg,
				  const struct soc_intel_pantherlake_config *config)
{
	s_cfg->TcssAuxOri = config->tcss_aux_ori;

	/* Explicitly clear this field to avoid using defaults */
	memset(s_cfg->IomTypeCPortPadCfg, 0, sizeof(s_cfg->IomTypeCPortPadCfg));

	/* D3Cold for TCSS */
	s_cfg->D3ColdEnable = !config->tcss_d3_cold_disable;
	s_cfg->UsbTcPortEn = 0;

	for (size_t i = 0; i < MAX_TYPE_C_PORTS; i++)
		if (config->tcss_ports[i].enable)
			s_cfg->UsbTcPortEn |= BIT(i);
}

static void fill_fsps_chipset_lockdown_params(FSP_S_CONFIG *s_cfg,
					      const struct soc_intel_pantherlake_config *config)
{
	/* Chipset Lockdown */
	const bool lockdown_by_fsp = get_lockdown_config() == CHIPSET_LOCKDOWN_FSP;

	s_cfg->PchLockDownGlobalSmi = lockdown_by_fsp;
	s_cfg->PchLockDownBiosInterface = lockdown_by_fsp;
	s_cfg->PchUnlockGpioPads = !lockdown_by_fsp;
	s_cfg->RtcMemoryLock = lockdown_by_fsp;
	s_cfg->SkipPamLock = !lockdown_by_fsp;

	/* coreboot will send EOP before loading payload */
	s_cfg->EndOfPostMessage = 0; /* EOP disable */

	s_cfg->CpuCrashLogEnable = CONFIG(SOC_INTEL_CRASHLOG);
}

static void fill_fsps_xhci_params(FSP_S_CONFIG *s_cfg,
				  const struct soc_intel_pantherlake_config *config)
{
	size_t i;

	for (i = 0; i < CONFIG_SOC_INTEL_USB2_DEV_MAX; i++) {
		s_cfg->PortUsb20Enable[i] = config->usb2_ports[i].enable;
		s_cfg->Usb2PhyPetxiset[i] = config->usb2_ports[i].pre_emp_bias;
		s_cfg->Usb2PhyTxiset[i] = config->usb2_ports[i].tx_bias;
		s_cfg->Usb2PhyPredeemp[i] = config->usb2_ports[i].tx_emp_enable;
		s_cfg->Usb2PhyPehalfbit[i] = config->usb2_ports[i].pre_emp_bit;

		if (config->usb2_ports[i].enable)
			s_cfg->Usb2OverCurrentPin[i] = config->usb2_ports[i].ocpin;
		else
			s_cfg->Usb2OverCurrentPin[i] = OC_SKIP;

		s_cfg->PortResetMessageEnable[i] = config->usb2_ports[i].type_c;
	}

	for (i = 0; i < CONFIG_SOC_INTEL_USB3_DEV_MAX; i++) {
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

	for (i = 0; i < MAX_TYPE_C_PORTS; i++)
		if (config->tcss_ports[i].enable)
			s_cfg->CpuUsb3OverCurrentPin[i] = config->tcss_ports[i].ocpin;
}

static void fill_fsps_xdci_params(FSP_S_CONFIG *s_cfg,
				  const struct soc_intel_pantherlake_config *config)
{
	s_cfg->XdciEnable = xdci_can_enable(PCI_DEVFN_USBOTG);
}

static void fill_fsps_thermal_params(FSP_S_CONFIG *s_cfg,
				     const struct soc_intel_pantherlake_config *config)
{
	s_cfg->Device4Enable = is_devfn_enabled(PCI_DEVFN_DPTF);
}

static const SI_PCH_DEVICE_INTERRUPT_CONFIG *pci_irq_to_fsp(size_t *out_count)
{
	const struct pci_irq_entry *entry = get_cached_pci_irqs();
	SI_PCH_DEVICE_INTERRUPT_CONFIG *config;
	size_t pch_total = 0, cfg_count = 0;

	if (!entry)
		return NULL;

	/* Count PCH devices */
	while (entry) {
		if (is_pch_slot(entry->devfn))
			pch_total++;
		entry = entry->next;
	}

	/* Convert PCH device entries to FSP format */
	config = calloc(pch_total, sizeof(*config));
	entry = get_cached_pci_irqs();
	while (entry) {
		if (!is_pch_slot(entry->devfn)) {
			entry = entry->next;
			continue;
		}

		config[cfg_count].Device = PCI_SLOT(entry->devfn);
		config[cfg_count].Function = PCI_FUNC(entry->devfn);
		config[cfg_count].IntX = (SI_PCH_INT_PIN)entry->pin;
		config[cfg_count].Irq = entry->irq;
		cfg_count++;

		entry = entry->next;
	}

	*out_count = cfg_count;

	return config;
}

static void fill_fsps_irq_params(FSP_S_CONFIG *s_cfg,
				 const struct soc_intel_pantherlake_config *config)
{
	if (!assign_pci_irqs(irq_constraints, ARRAY_SIZE(irq_constraints)))
		die("ERROR: Unable to assign PCI IRQs, and no _PRT table available\n");

	size_t pch_count = 0;
	const SI_PCH_DEVICE_INTERRUPT_CONFIG *upd_irqs = pci_irq_to_fsp(&pch_count);

	s_cfg->DevIntConfigPtr = (UINT32)((uintptr_t)upd_irqs);
	s_cfg->NumOfDevIntConfig = pch_count;
	printk(BIOS_INFO, "IRQ: Using dynamically assigned PCI IO-APIC IRQs\n");
}

static void evaluate_ssid(const struct device *dev, uint16_t *svid, uint16_t *ssid)
{
	if (!(dev && svid && ssid))
		return;

	*svid = CONFIG_SUBSYSTEM_VENDOR_ID ? : (dev->subsystem_vendor ? : 0x8086);
	*ssid = CONFIG_SUBSYSTEM_DEVICE_ID ? : (dev->subsystem_device ? : 0xfffe);
}

/*
 * Programming SSID before FSP-S is important because SSID registers of a few PCIE
 * devices (e.g. IPU, Crashlog, XHCI, TCSS_XHCI etc.) are locked after FSP-S hence
 * provide a custom SSID (same as DID by default) value via UPD.
 */
static void fill_fsps_pci_ssid_params(FSP_S_CONFIG *s_cfg,
				      const struct soc_intel_pantherlake_config *config)
{
	struct svid_ssid_init_entry {
		union {
			struct {
				uint64_t reg:12;
				uint64_t function:3;
				uint64_t device:5;
				uint64_t bus:8;
				uint64_t ignore1:4;
				uint64_t segment:16;
				uint64_t ignore2:16;
			};
			uint64_t data;
		};
		struct {
			uint16_t svid;
			uint16_t ssid;
		};
		uint32_t ignore3;
	};

	static struct svid_ssid_init_entry ssid_table[MAX_ONBOARD_PCIE_DEVICES];
	const struct device *dev;
	size_t i = 0;

	for (dev = all_devices; dev; dev = dev->next) {
		if (!(is_dev_enabled(dev) && dev->path.type == DEVICE_PATH_PCI &&
		    dev->upstream->secondary == 0))
			continue;

		if (dev->path.pci.devfn == PCI_DEVFN_ROOT) {
			evaluate_ssid(dev, &s_cfg->SiCustomizedSvid, &s_cfg->SiCustomizedSsid);
		} else {
			ssid_table[i].reg	= PCI_SUBSYSTEM_VENDOR_ID;
			ssid_table[i].device	= PCI_SLOT(dev->path.pci.devfn);
			ssid_table[i].function	= PCI_FUNC(dev->path.pci.devfn);
			evaluate_ssid(dev, &ssid_table[i].svid, &ssid_table[i].ssid);
			i++;
		}
	}

	s_cfg->SiSsidTablePtr = (uintptr_t)ssid_table;
	s_cfg->SiNumberOfSsidTableEntry = i;

	/* Ensure FSP will program the registers */
	s_cfg->SiSkipSsidProgramming = 0;
}

static void fill_fsps_lan_params(FSP_S_CONFIG *s_cfg,
				 const struct soc_intel_pantherlake_config *config)
{
	s_cfg->PchLanEnable = is_devfn_enabled(PCI_DEVFN_GBE);
}

static void fill_fsps_cnvi_params(FSP_S_CONFIG *s_cfg,
				  const struct soc_intel_pantherlake_config *config)
{
	s_cfg->CnviMode = is_devfn_enabled(PCI_DEVFN_CNVI_WIFI);
	s_cfg->CnviWifiCore = config->cnvi_wifi_core;
	s_cfg->CnviBtCore = config->cnvi_bt_core;
	s_cfg->CnviBtAudioOffload = config->cnvi_bt_audio_offload;

	if (!s_cfg->CnviMode && s_cfg->CnviWifiCore) {
		printk(BIOS_ERR, "CNVi WiFi is enabled without CNVi being enabled\n");
		s_cfg->CnviWifiCore = 0;
	}
	if (!s_cfg->CnviBtCore && s_cfg->CnviBtAudioOffload) {
		printk(BIOS_ERR, "BT offload is enabled without CNVi BT being enabled\n");
		s_cfg->CnviBtAudioOffload = 0;
	}
	if (!s_cfg->CnviMode && s_cfg->CnviBtCore) {
		printk(BIOS_ERR, "CNVi BT is enabled without CNVi being enabled\n");
		s_cfg->CnviBtCore = 0;
		s_cfg->CnviBtAudioOffload = 0;
	}

	s_cfg->CnviBtInterface = is_devfn_enabled(PCI_DEVFN_CNVI_BT) ? 2 : 1;
}

static void fill_fsps_vmd_params(FSP_S_CONFIG *s_cfg,
				 const struct soc_intel_pantherlake_config *config)
{
	s_cfg->VmdEnable = is_devfn_enabled(PCI_DEVFN_VMD);
}

static void fill_fsps_pmcpd_params(FSP_S_CONFIG *s_cfg,
				   const struct soc_intel_pantherlake_config *config)
{
	s_cfg->PmcPdEnable = 1;
}

static void fill_fsps_thc_params(FSP_S_CONFIG *s_cfg,
				 const struct soc_intel_pantherlake_config *config)
{
	for (size_t i = 0; i < NUM_THC; i++) {
		if (!is_devfn_enabled(_PCI_DEVFN(THC, i))) {
			s_cfg->ThcAssignment[i] = THC_NONE;
			continue;
		}
		s_cfg->ThcAssignment[i] = THC_0 + i;
		s_cfg->ThcMode[i] = config->thc_mode[i];
		s_cfg->ThcWakeOnTouch[i] = config->thc_wake_on_touch[i];
	}
}

static void fill_fsps_8254_params(FSP_S_CONFIG *s_cfg,
				  const struct soc_intel_pantherlake_config *config)
{
	bool use_8254 = get_uint_option("legacy_8254_timer", CONFIG(USE_LEGACY_8254_TIMER));
	s_cfg->Enable8254ClockGating = !use_8254;
	s_cfg->Enable8254ClockGatingOnS3 = !use_8254;
}

static void fill_fsps_pm_timer_params(FSP_S_CONFIG *s_cfg,
				      const struct soc_intel_pantherlake_config *config)
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
				  const struct soc_intel_pantherlake_config *config)
{
	uint32_t enable_mask = pcie_rp_enable_mask(get_pcie_rp_table());

	for (size_t i = 0; i < CONFIG_MAX_ROOT_PORTS; i++) {
		const struct pcie_rp_config *rp_cfg = &config->pcie_rp[i];
		if (!(enable_mask & BIT(i)))
			continue;
		s_cfg->PcieRpL1Substates[i] =
			get_l1_substate_control(rp_cfg->PcieRpL1Substates);
		s_cfg->PcieRpLtrEnable[i] = !!(rp_cfg->flags & PCIE_RP_LTR);
		s_cfg->PcieRpAdvancedErrorReporting[i] = !!(rp_cfg->flags & PCIE_RP_AER);
		s_cfg->PcieRpHotPlug[i] =
			!!(rp_cfg->flags & PCIE_RP_HOTPLUG) || CONFIG(SOC_INTEL_COMPLIANCE_TEST_MODE);
		s_cfg->PcieRpClkReqDetect[i] = !!(rp_cfg->flags & PCIE_RP_CLK_REQ_DETECT);
		if (rp_cfg->pcie_rp_aspm)
			s_cfg->PcieRpAspm[i] = get_aspm_control(rp_cfg->pcie_rp_aspm);
	}

	s_cfg->PcieComplianceTestMode = CONFIG(SOC_INTEL_COMPLIANCE_TEST_MODE);
}

static void fill_fsps_misc_power_params(FSP_S_CONFIG *s_cfg,
					const struct soc_intel_pantherlake_config *config)
{
	/* Skip setting D0I3 bit for all HECI devices */
	s_cfg->DisableD0I3SettingForHeci = 1;

	s_cfg->Hwp = 1;
	s_cfg->Cx = 1;
	/* Enable the energy efficient turbo mode */
	s_cfg->EnergyEfficientTurbo = 1;
	s_cfg->PmcLpmS0ixSubStateEnableMask = get_supported_lpm_mask();
	/* Un-Demotion from Demoted C1 need to be disable when
	   C1 auto demotion is disabled. */
	s_cfg->C1StateUnDemotion = !config->disable_c1_state_auto_demotion;
	s_cfg->C1StateAutoDemotion = !config->disable_c1_state_auto_demotion;
	s_cfg->PkgCStateDemotion = !config->disable_package_c_state_demotion;
	s_cfg->PkgCStateUnDemotion = !config->disable_package_c_state_demotion;
	s_cfg->PmcV1p05PhyExtFetControlEn = 1;

	/* Enable/Disable PCH to CPU energy report feature. */
	s_cfg->PchPmDisableEnergyReport = !config->pch_pm_energy_report_enable;
}

static void fill_fsps_npu_params(FSP_S_CONFIG *s_cfg,
				 const struct soc_intel_pantherlake_config *config)
{
	s_cfg->NpuEnable = is_devfn_enabled(PCI_DEVFN_NPU);
}

static void fill_fsps_audio_params(FSP_S_CONFIG *s_cfg,
				   const struct soc_intel_pantherlake_config *config)
{
	if (!is_devfn_enabled(PCI_DEVFN_HDA))
		return;

	/* Fill MIC privacy settings */
	s_cfg->PchHdaMicPrivacyHwModeSoundWire0 = 1;
	s_cfg->PchHdaMicPrivacyHwModeSoundWire1 = 1;
	s_cfg->PchHdaMicPrivacyHwModeSoundWire2 = 1;
	s_cfg->PchHdaMicPrivacyHwModeSoundWire3 = 1;
	s_cfg->PchHdaMicPrivacyHwModeSoundWire4 = 1;
	s_cfg->PchHdaMicPrivacyHwModeDmic = 1;
}

static void fill_fsps_iax_params(FSP_S_CONFIG *s_cfg,
				 const struct soc_intel_pantherlake_config *config)
{
	s_cfg->IaxEnable = is_devfn_enabled(PCI_DEVFN_IAA);
}

static void fill_fsps_ufs_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_pantherlake_config *config)
{
#if CONFIG(SOC_INTEL_PANTHERLAKE_U_H)
	/* Setting FSP UPD (1,0) to enable controller 0 */
	s_cfg->UfsEnable[0] = is_devfn_enabled(PCI_DEVFN_UFS);
	s_cfg->UfsEnable[1] = 0;
#else
	/* Setting FSP UPD (0,0) to keep both controllers disabled */
	s_cfg->UfsEnable[0] = 0;
	s_cfg->UfsEnable[1] = 0;
#endif
}

static void arch_silicon_init_params(FSPS_ARCH2_UPD *s_arch_cfg)
{
	/* Assign FspEventHandler arch Upd to use coreboot debug event handler */
	if (CONFIG(FSP_USES_CB_DEBUG_EVENT_HANDLER)
	    && CONFIG(CONSOLE_SERIAL)
	    && CONFIG(FSP_ENABLE_SERIAL_DEBUG) && fsp_get_pcd_debug_log_level())
		s_arch_cfg->FspEventHandler = (uintptr_t)((FSP_EVENT_HANDLER *)
							  fsp_debug_event_handler);

}

static void soc_silicon_init_params(FSP_S_CONFIG *s_cfg,
				    const struct soc_intel_pantherlake_config *config)
{
	void (*fill_fsps_params[])(FSP_S_CONFIG *s_cfg,
				   const struct soc_intel_pantherlake_config *config) = {
		fill_fsps_lpss_params,
		fill_fsps_cpu_params,
		fill_fsps_microcode_params,
		fill_fsps_igd_params,
		fill_fsps_tcss_params,
		fill_fsps_chipset_lockdown_params,
		fill_fsps_xhci_params,
		fill_fsps_xdci_params,
		fill_fsps_thermal_params,
		fill_fsps_irq_params,
		fill_fsps_pci_ssid_params,
		fill_fsps_lan_params,
		fill_fsps_cnvi_params,
		fill_fsps_vmd_params,
		fill_fsps_pmcpd_params,
		fill_fsps_thc_params,
		fill_fsps_8254_params,
		fill_fsps_pm_timer_params,
		fill_fsps_pcie_params,
		fill_fsps_misc_power_params,
		fill_fsps_npu_params,
		fill_fsps_audio_params,
		fill_fsps_iax_params,
		fill_fsps_ufs_params,
	};

	for (size_t i = 0; i < ARRAY_SIZE(fill_fsps_params); i++)
		fill_fsps_params[i](s_cfg, config);
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	struct soc_intel_pantherlake_config *config;
	FSP_S_CONFIG *s_cfg = &supd->FspsConfig;
	FSPS_ARCH2_UPD *s_arch_cfg = &supd->FspsArchUpd;

	config = config_of_soc();
	arch_silicon_init_params(s_arch_cfg);
	/* Override settings per board if required. */
	mainboard_update_soc_chip_config(config);
	soc_silicon_init_params(s_cfg, config);
	mainboard_silicon_init_params(s_cfg);
}

/*
 * Callbacks for SoC/Mainboard specific overrides for FspMultiPhaseSiInit
 * This platform supports below MultiPhaseSIInit Phase(s):
 *
 * Phase |  FSP return point                    |  Purpose
 * ----- + -------------------------------------+ ------------------------
 *   1   |  After TCSS initialization completed |  for TCSS specific init
 */
void platform_fsp_silicon_multi_phase_init_cb(uint32_t phase_index)
{
	switch (phase_index) {
	case 1:
		/* TCSS specific initialization here */
		printk(BIOS_DEBUG, "FSP MultiPhaseSiInit %s/%s called\n",
		       __FILE__, __func__);

		if (CONFIG(SOC_INTEL_COMMON_BLOCK_TCSS)) {
			const struct soc_intel_pantherlake_config *config = config_of_soc();
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

/* Handle FSP logo params */
void soc_load_logo(FSPS_UPD *supd)
{
	efi_uintn_t logo, blt_size;
	uint32_t logo_size;
	struct soc_intel_common_config *config = chip_get_common_soc_structure();
	FSP_S_CONFIG *s_cfg = &supd->FspsConfig;

	/*
	 * Adjusts panel orientation for external display when the lid is closed.
	 *
	 * When the lid is closed (LidStatus == 0), indicating the onboard display is inactive,
	 * this function forces the panel orientation to normal. This ensures proper display
	 * on an external monitor, as rotated orientations are typically not suitable in
	 * such state.
	 */
	if (s_cfg->LidStatus == 0)
		config->panel_orientation = LB_FB_ORIENTATION_NORMAL;

	fsp_convert_bmp_to_gop_blt(&logo, &logo_size,
				   &supd->FspsConfig.BltBufferAddress,
				   &blt_size,
				   &supd->FspsConfig.LogoPixelHeight,
				   &supd->FspsConfig.LogoPixelWidth,
				   config->panel_orientation);
}
