/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <cpu/intel/microcode.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <fsp/api.h>
#include <fsp/fsp_debug_event.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <fsp/util.h>
#include <option.h>
#include <intelblocks/irq.h>
#include <intelblocks/lpss.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <intelblocks/tcss.h>
#include <soc/cpu.h>
#include <soc/gpio.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

/* THC assignment definition */
#define THC_NONE	0
#define THC_0		1
#define THC_1		2

/* SATA DEVSLP idle timeout default values */
#define DEF_DMVAL	15
#define DEF_DITOVAL	625

/* VccIn Aux Imon IccMax values in mA */
#define MILLIAMPS_TO_AMPS	1000
#define ICC_MAX_TDP_45W		34250
#define ICC_MAX_TDP_15W_28W	32000
#define ICC_MAX_ID_ADL_M_MA	12000
#define ICC_MAX_ID_ADL_N_MA	27000
#define ICC_MAX_ADL_S		33000

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

static const struct slot_irq_constraints irq_constraints[] = {
	{
		.slot = SA_DEV_SLOT_IGD,
		.fns = {
			/* INTERRUPT_PIN is RO/0x01 */
			FIXED_INT_ANY_PIRQ(SA_DEVFN_IGD, PCI_INT_A),
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
			/* INTERRUPT_PIN is RO/0x01, and INTERRUPT_LINE is RW,
			   but S0ix fails when not set to 16 (b/193434192) */
			FIXED_INT_PIRQ(SA_DEVFN_IPU, PCI_INT_A, PIRQ_A),
		},
	},
	{
		.slot = SA_DEV_SLOT_CPU_6,
		.fns = {
			FIXED_INT_PIRQ(SA_DEVFN_CPU_PCIE6_0, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(SA_DEVFN_CPU_PCIE6_2, PCI_INT_C, PIRQ_C),
		},
	},
	{
		.slot = SA_DEV_SLOT_TBT,
		.fns = {
			ANY_PIRQ(SA_DEVFN_TBT0),
			ANY_PIRQ(SA_DEVFN_TBT1),
			ANY_PIRQ(SA_DEVFN_TBT2),
			ANY_PIRQ(SA_DEVFN_TBT3),
		},
	},
	{
		.slot = SA_DEV_SLOT_GNA,
		.fns = {
			/* INTERRUPT_PIN is RO/0x01 */
			FIXED_INT_ANY_PIRQ(SA_DEVFN_GNA, PCI_INT_A),
		},
	},
	{
		.slot = SA_DEV_SLOT_TCSS,
		.fns = {
			ANY_PIRQ(SA_DEVFN_TCSS_XHCI),
			ANY_PIRQ(SA_DEVFN_TCSS_XDCI),
		},
	},
	{
		.slot = PCH_DEV_SLOT_SIO0,
		.fns = {
			DIRECT_IRQ(PCH_DEVFN_I2C6),
			DIRECT_IRQ(PCH_DEVFN_I2C7),
			ANY_PIRQ(PCH_DEVFN_THC0),
			ANY_PIRQ(PCH_DEVFN_THC1),
		},
	},
	{
		.slot = PCH_DEV_SLOT_SIO6,
		.fns = {
			DIRECT_IRQ(PCH_DEVFN_UART3),
			DIRECT_IRQ(PCH_DEVFN_UART4),
			DIRECT_IRQ(PCH_DEVFN_UART5),
			DIRECT_IRQ(PCH_DEVFN_UART6),
		},
	},
	{
		.slot = PCH_DEV_SLOT_ISH,
		.fns = {
			DIRECT_IRQ(PCH_DEVFN_ISH),
			DIRECT_IRQ(PCH_DEVFN_GSPI2),
			ANY_PIRQ(PCH_DEVFN_UFS),
		},
	},
	{
		.slot = PCH_DEV_SLOT_SIO2,
		.fns = {
			DIRECT_IRQ(PCH_DEVFN_GSPI3),
			DIRECT_IRQ(PCH_DEVFN_GSPI4),
			DIRECT_IRQ(PCH_DEVFN_GSPI5),
			DIRECT_IRQ(PCH_DEVFN_GSPI6),
		},
	},
	{
		.slot = PCH_DEV_SLOT_XHCI,
		.fns = {
			ANY_PIRQ(PCH_DEVFN_XHCI),
			DIRECT_IRQ(PCH_DEVFN_USBOTG),
			ANY_PIRQ(PCH_DEVFN_CNVI_WIFI),
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
#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
	{
		.slot = PCH_DEV_SLOT_EMMC,
		.fns = {
			ANY_PIRQ(PCH_DEVFN_EMMC),
		},
	},
#endif
	{
		.slot = PCH_DEV_SLOT_PCIE,
		.fns = {
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE1, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE2, PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE3, PCI_INT_C, PIRQ_C),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE4, PCI_INT_D, PIRQ_D),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE5, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE6, PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE7, PCI_INT_C, PIRQ_C),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE8, PCI_INT_D, PIRQ_D),
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
			/* UART0 shares an interrupt line with TSN0, so must use
			   a PIRQ */
			FIXED_INT_ANY_PIRQ(PCH_DEVFN_UART0, PCI_INT_A),
			/* UART1 shares an interrupt line with TSN1, so must use
			   a PIRQ */
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
			/* INTERRUPT_PIN is RO/0x01 */
			FIXED_INT_ANY_PIRQ(PCH_DEVFN_TRACEHUB, PCI_INT_A),
		},
	},
};

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
	if ((ctl > ASPM_AUTO) || (ctl == ASPM_DEFAULT))
		ctl = ASPM_AUTO;
	return ctl - 1;
}

/* This function returns the VccIn Aux Imon IccMax values for ADL-P and ADL-S SKU's */
static uint16_t get_vccin_aux_imon_iccmax(void)
{
	uint16_t mch_id = 0;
	uint8_t tdp;

	if (!mch_id) {
		struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
		mch_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xffff;
	}

	switch (mch_id) {
	case PCI_DID_INTEL_ADL_P_ID_1:
	case PCI_DID_INTEL_ADL_P_ID_3:
	case PCI_DID_INTEL_ADL_P_ID_4:
	case PCI_DID_INTEL_ADL_P_ID_5:
	case PCI_DID_INTEL_ADL_P_ID_6:
	case PCI_DID_INTEL_ADL_P_ID_7:
	case PCI_DID_INTEL_ADL_P_ID_8:
	case PCI_DID_INTEL_ADL_P_ID_9:
	case PCI_DID_INTEL_ADL_P_ID_10:
		tdp = get_cpu_tdp();
		if (tdp == TDP_45W)
			return ICC_MAX_TDP_45W;
		return ICC_MAX_TDP_15W_28W;
	case PCI_DID_INTEL_ADL_M_ID_1:
	case PCI_DID_INTEL_ADL_M_ID_2:
		return ICC_MAX_ID_ADL_M_MA;
	case PCI_DID_INTEL_ADL_N_ID_1:
	case PCI_DID_INTEL_ADL_N_ID_2:
	case PCI_DID_INTEL_ADL_N_ID_3:
	case PCI_DID_INTEL_ADL_N_ID_4:
		return ICC_MAX_ID_ADL_N_MA;
	case PCI_DID_INTEL_ADL_S_ID_1:
	case PCI_DID_INTEL_ADL_S_ID_3:
	case PCI_DID_INTEL_ADL_S_ID_8:
	case PCI_DID_INTEL_ADL_S_ID_10:
		return ICC_MAX_ADL_S;
	default:
		printk(BIOS_ERR, "Unknown MCH ID: 0x%4x, skipping VccInAuxImonIccMax config\n",
			mch_id);
		return 0;
	}
}

__weak void mainboard_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	/* Override settings per board. */
}

static void fill_fsps_lpss_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	for (int i = 0; i < CONFIG_SOC_INTEL_I2C_DEV_MAX; i++)
		s_cfg->SerialIoI2cMode[i] = config->serial_io_i2c_mode[i];

	for (int i = 0; i < CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX; i++) {
		s_cfg->SerialIoSpiMode[i] = config->serial_io_gspi_mode[i];
		s_cfg->SerialIoSpiCsMode[i] = config->serial_io_gspi_cs_mode[i];
		s_cfg->SerialIoSpiCsState[i] = config->serial_io_gspi_cs_state[i];
	}

	for (int i = 0; i < CONFIG_SOC_INTEL_UART_DEV_MAX; i++)
		s_cfg->SerialIoUartMode[i] = config->serial_io_uart_mode[i];
}

static void fill_fsps_cpu_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	const struct microcode *microcode_file;
	size_t microcode_len;

	/* Locate microcode and pass to FSP-S for 2nd microcode loading */
	microcode_file = intel_microcode_find();

	if (microcode_file != NULL) {
		microcode_len = get_microcode_size(microcode_file);
		if (microcode_len != 0) {
			/* Update CPU Microcode patch base address/size */
			s_cfg->MicrocodeRegionBase = (uint32_t)(uintptr_t)microcode_file;
			s_cfg->MicrocodeRegionSize = (uint32_t)microcode_len;
		}
	}

	if (CONFIG(USE_FSP_MP_INIT)) {
		/*
		 * Use FSP running MP PPI services to perform CPU feature programming
		 * if Kconfig is enabled
		 */
		s_cfg->CpuMpPpi = (uintptr_t) mp_fill_ppi_services_data();
	} else {
		/* Use coreboot native driver to perform MP init by default */
		s_cfg->CpuMpPpi = (uintptr_t)NULL;
		s_cfg->SkipMpInit = !CONFIG(USE_INTEL_FSP_MP_INIT);
	}
}

static void fill_fsps_igd_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* Load VBT before devicetree-specific config. */
	s_cfg->GraphicsConfigPtr = (uintptr_t)vbt_get();

	/* Check if IGD is present and fill Graphics init param accordingly */
	s_cfg->PeiGraphicsPeimInit = CONFIG(RUN_FSP_GOP) && is_devfn_enabled(SA_DEVFN_IGD);
	s_cfg->LidStatus = CONFIG(RUN_FSP_GOP);
	s_cfg->PavpEnable = CONFIG(PAVP);
}

static void fill_fsps_tcss_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
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

	/*
	 * Set FSPS UPD ITbtConnectTopologyTimeoutInMs with value 0. FSP will
	 * evaluate this UPD value and skip sending command. There will be no
	 * delay for command completion.
	 */
	s_cfg->ITbtConnectTopologyTimeoutInMs = 0;

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
		const struct soc_intel_alderlake_config *config)
{
	/* Chipset Lockdown */
	const bool lockdown_by_fsp = get_lockdown_config() == CHIPSET_LOCKDOWN_FSP;
	s_cfg->PchLockDownGlobalSmi = lockdown_by_fsp;
	s_cfg->PchLockDownBiosInterface = lockdown_by_fsp;
	s_cfg->PchUnlockGpioPads = lockdown_by_fsp;
	s_cfg->RtcMemoryLock = lockdown_by_fsp;
	s_cfg->SkipPamLock = !lockdown_by_fsp;

	/* coreboot will send EOP before loading payload */
	s_cfg->EndOfPostMessage = EOP_DISABLE;
}

static void fill_fsps_xhci_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	int i;
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

		if (config->usb2_ports[i].type_c)
			s_cfg->PortResetMessageEnable[i] = 1;
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

	s_cfg->PmcUsb2PhySusPgEnable = !config->usb2_phy_sus_pg_disable;
}

static void fill_fsps_xdci_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	s_cfg->XdciEnable = xdci_can_enable(PCH_DEVFN_USBOTG);
}

static void fill_fsps_uart_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	if (CONFIG(FSP_USES_CB_DEBUG_EVENT_HANDLER) && CONFIG(CONSOLE_SERIAL) &&
			 CONFIG(FSP_ENABLE_SERIAL_DEBUG))
		s_cfg->FspEventHandler = (UINT32)((FSP_EVENT_HANDLER *)
				fsp_debug_event_handler);
	/* PCH UART selection for FSP Debug */
	s_cfg->SerialIoDebugUartNumber = CONFIG_UART_FOR_CONSOLE;
	ASSERT(ARRAY_SIZE(s_cfg->SerialIoUartAutoFlow) > CONFIG_UART_FOR_CONSOLE);
	s_cfg->SerialIoUartAutoFlow[CONFIG_UART_FOR_CONSOLE] = 0;
}

static void fill_fsps_sata_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* SATA */
	s_cfg->SataEnable = is_devfn_enabled(PCH_DEVFN_SATA);
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
		const struct soc_intel_alderlake_config *config)
{
	/* Enable TCPU for processor thermal control */
	s_cfg->Device4Enable = is_devfn_enabled(SA_DEVFN_DPTF);

	/* Set TccActivationOffset */
	s_cfg->TccActivationOffset = config->tcc_offset;
}

static void fill_fsps_lan_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* LAN */
	s_cfg->PchLanEnable = is_devfn_enabled(PCH_DEVFN_GBE);
}

static void fill_fsps_cnvi_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* CNVi */
	s_cfg->CnviMode = is_devfn_enabled(PCH_DEVFN_CNVI_WIFI);
	s_cfg->CnviBtCore = config->cnvi_bt_core;
	s_cfg->CnviBtAudioOffload = config->cnvi_bt_audio_offload;
	/* Assert if CNVi BT is enabled without CNVi being enabled. */
	assert(s_cfg->CnviMode || !s_cfg->CnviBtCore);
	/* Assert if CNVi BT offload is enabled without CNVi BT being enabled. */
	assert(s_cfg->CnviBtCore || !s_cfg->CnviBtAudioOffload);
}

static void fill_fsps_vmd_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* VMD */
	s_cfg->VmdEnable = is_devfn_enabled(SA_DEVFN_VMD);
}

static void fill_fsps_thc_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* THC */
	s_cfg->ThcPort0Assignment = is_devfn_enabled(PCH_DEVFN_THC0) ? THC_0 : THC_NONE;
	s_cfg->ThcPort1Assignment = is_devfn_enabled(PCH_DEVFN_THC1) ? THC_1 : THC_NONE;
}

static void fill_fsps_tbt_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* USB4/TBT */
	for (int i = 0; i < ARRAY_SIZE(s_cfg->ITbtPcieRootPortEn); i++)
		s_cfg->ITbtPcieRootPortEn[i] = is_devfn_enabled(SA_DEVFN_TBT(i));
}

static void fill_fsps_8254_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* Legacy 8254 timer support */
	bool use_8254 = get_uint_option("legacy_8254_timer", CONFIG(USE_LEGACY_8254_TIMER));
	s_cfg->Enable8254ClockGating = !use_8254;
	s_cfg->Enable8254ClockGatingOnS3 = !use_8254;
}

static void fill_fsps_pm_timer_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
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

static void fill_fsps_storage_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
	/* eMMC Configuration */
	s_cfg->ScsEmmcEnabled = is_devfn_enabled(PCH_DEVFN_EMMC);
	if (s_cfg->ScsEmmcEnabled)
		s_cfg->ScsEmmcHs400Enabled = config->emmc_enable_hs400_mode;
#endif

	/* UFS Configuration */
	s_cfg->UfsEnable[0] = 0; /* UFS Controller 0 is fuse disabled */
	s_cfg->UfsEnable[1] = is_devfn_enabled(PCH_DEVFN_UFS);

	/* Enable Hybrid storage auto detection */
	s_cfg->HybridStorageMode = config->hybrid_storage_mode;
}

static void fill_fsps_pcie_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	uint32_t enable_mask = pcie_rp_enable_mask(get_pch_pcie_rp_table());
	for (int i = 0; i < CONFIG_MAX_PCH_ROOT_PORTS; i++) {
		if (!(enable_mask & BIT(i)))
			continue;
		const struct pcie_rp_config *rp_cfg = &config->pch_pcie_rp[i];
		s_cfg->PcieRpL1Substates[i] =
				get_l1_substate_control(rp_cfg->PcieRpL1Substates);
		s_cfg->PcieRpLtrEnable[i] = !!(rp_cfg->flags & PCIE_RP_LTR);
		s_cfg->PcieRpAdvancedErrorReporting[i] = !!(rp_cfg->flags & PCIE_RP_AER);
		s_cfg->PcieRpHotPlug[i] = !!(rp_cfg->flags & PCIE_RP_HOTPLUG);
		s_cfg->PcieRpClkReqDetect[i] = !!(rp_cfg->flags & PCIE_RP_CLK_REQ_DETECT);
		if (rp_cfg->pcie_rp_aspm)
			s_cfg->PcieRpAspm[i] = get_aspm_control(rp_cfg->pcie_rp_aspm);
		/* PcieRpSlotImplemented default to 1 (slot implemented) in FSP; 0: built-in */
		if (!!(rp_cfg->flags & PCIE_RP_BUILT_IN))
			s_cfg->PcieRpSlotImplemented[i] = 0;
		s_cfg->PcieRpDetectTimeoutMs[i] = rp_cfg->pcie_rp_detect_timeout_ms;
	}
}

static void fill_fsps_cpu_pcie_params(FSP_S_CONFIG *s_cfg,
				      const struct soc_intel_alderlake_config *config)
{
	if (!CONFIG_MAX_CPU_ROOT_PORTS)
		return;

	const uint32_t enable_mask = pcie_rp_enable_mask(get_cpu_pcie_rp_table());
	for (int i = 0; i < CONFIG_MAX_CPU_ROOT_PORTS; i++) {
		if (!(enable_mask & BIT(i)))
			continue;

		const struct pcie_rp_config *rp_cfg = &config->cpu_pcie_rp[i];
		s_cfg->CpuPcieRpL1Substates[i] =
			get_l1_substate_control(rp_cfg->PcieRpL1Substates);
		s_cfg->CpuPcieRpLtrEnable[i] = !!(rp_cfg->flags & PCIE_RP_LTR);
		s_cfg->CpuPcieRpAdvancedErrorReporting[i] = !!(rp_cfg->flags & PCIE_RP_AER);
		s_cfg->CpuPcieRpHotPlug[i] = !!(rp_cfg->flags & PCIE_RP_HOTPLUG);
		s_cfg->PtmEnabled[i] = 0;
	}
}

static void fill_fsps_misc_power_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* Skip setting D0I3 bit for all HECI devices */
	s_cfg->DisableD0I3SettingForHeci = 1;
	/*
	 * Power Optimizer for DMI
	 * DmiPwrOptimizeDisable is default to 0.
	 * Boards not needing the optimizers explicitly disables them by setting
	 * these disable variables to 1 in devicetree overrides.
	 */
	s_cfg->PchPwrOptEnable = !(config->dmi_power_optimize_disable);
	s_cfg->PmSupport = 1;
	s_cfg->Hwp = 1;
	s_cfg->Cx = 1;
	s_cfg->PsOnEnable = 1;
	/* Enable the energy efficient turbo mode */
	s_cfg->EnergyEfficientTurbo = 1;
	s_cfg->PkgCStateLimit = LIMIT_AUTO;

	/* VccIn Aux Imon IccMax. Values are in 1/4 Amp increments and range is 0-512. */
	s_cfg->VccInAuxImonIccImax = get_vccin_aux_imon_iccmax() * 4 / MILLIAMPS_TO_AMPS;

	/* VrConfig Settings for IA and GT domains */
	for (size_t i = 0; i < ARRAY_SIZE(config->domain_vr_config); i++)
		fill_vr_domain_config(s_cfg, i, &config->domain_vr_config[i]);

	s_cfg->PmcLpmS0ixSubStateEnableMask = get_supported_lpm_mask();

	/* Apply minimum assertion width settings */
	if (config->pch_slp_s3_min_assertion_width == SLP_S3_ASSERTION_DEFAULT)
		s_cfg->PchPmSlpS3MinAssert = SLP_S3_ASSERTION_50_MS;
	else
		s_cfg->PchPmSlpS3MinAssert = config->pch_slp_s3_min_assertion_width;

	if (config->pch_slp_s4_min_assertion_width == SLP_S4_ASSERTION_DEFAULT)
		s_cfg->PchPmSlpS4MinAssert = SLP_S4_ASSERTION_1S;
	else
		s_cfg->PchPmSlpS4MinAssert = config->pch_slp_s4_min_assertion_width;

	if (config->pch_slp_sus_min_assertion_width == SLP_SUS_ASSERTION_DEFAULT)
		s_cfg->PchPmSlpSusMinAssert = SLP_SUS_ASSERTION_4_S;
	else
		s_cfg->PchPmSlpSusMinAssert = config->pch_slp_sus_min_assertion_width;

	if (config->pch_slp_a_min_assertion_width == SLP_A_ASSERTION_DEFAULT)
		s_cfg->PchPmSlpAMinAssert = SLP_A_ASSERTION_2_S;
	else
		s_cfg->PchPmSlpAMinAssert = config->pch_slp_a_min_assertion_width;

	unsigned int power_cycle_duration = config->pch_reset_power_cycle_duration;
	if (power_cycle_duration == POWER_CYCLE_DURATION_DEFAULT)
		power_cycle_duration = POWER_CYCLE_DURATION_4S;

	s_cfg->PchPmPwrCycDur = get_pm_pwr_cyc_dur(s_cfg->PchPmSlpS4MinAssert,
						   s_cfg->PchPmSlpS3MinAssert,
						   s_cfg->PchPmSlpAMinAssert,
						   power_cycle_duration);

	/* Set PsysPmax if it is available from DT */
	if (config->platform_pmax) {
		printk(BIOS_DEBUG, "PsysPmax = %dW\n", config->platform_pmax);
		/* PsysPmax is in unit of 1/8 Watt */
		s_cfg->PsysPmax = config->platform_pmax * 8;
	}

	s_cfg->C1StateAutoDemotion = !config->disable_c1_state_auto_demotion;

	s_cfg->VrPowerDeliveryDesign = config->vr_power_delivery_design;
}

static void fill_fsps_irq_params(FSP_S_CONFIG *s_cfg,
				 const struct soc_intel_alderlake_config *config)
{
	if (!assign_pci_irqs(irq_constraints, ARRAY_SIZE(irq_constraints)))
		die("ERROR: Unable to assign PCI IRQs, and no _PRT table available\n");

	size_t pch_count = 0;
	const SI_PCH_DEVICE_INTERRUPT_CONFIG *upd_irqs = pci_irq_to_fsp(&pch_count);

	s_cfg->DevIntConfigPtr = (UINT32)((uintptr_t)upd_irqs);
	s_cfg->NumOfDevIntConfig = pch_count;
	printk(BIOS_INFO, "IRQ: Using dynamically assigned PCI IO-APIC IRQs\n");
}

static void fill_fsps_fivr_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* PCH FIVR settings override */
	if (!config->ext_fivr_settings.configure_ext_fivr)
		return;

	s_cfg->PchFivrExtV1p05RailEnabledStates =
			config->ext_fivr_settings.v1p05_enable_bitmap;

	s_cfg->PchFivrExtV1p05RailSupportedVoltageStates =
			config->ext_fivr_settings.v1p05_supported_voltage_bitmap;

	s_cfg->PchFivrExtVnnRailEnabledStates =
			config->ext_fivr_settings.vnn_enable_bitmap;

	s_cfg->PchFivrExtVnnRailSupportedVoltageStates =
			config->ext_fivr_settings.vnn_supported_voltage_bitmap;

	s_cfg->PchFivrExtVnnRailSxEnabledStates =
			config->ext_fivr_settings.vnn_sx_enable_bitmap;

	/* Convert the voltages to increments of 2.5mv */
	s_cfg->PchFivrExtV1p05RailVoltage =
			(config->ext_fivr_settings.v1p05_voltage_mv * 10) / 25;

	s_cfg->PchFivrExtVnnRailVoltage =
			(config->ext_fivr_settings.vnn_voltage_mv * 10) / 25;

	s_cfg->PchFivrExtVnnRailSxVoltage =
			(config->ext_fivr_settings.vnn_sx_voltage_mv * 10 / 25);

	s_cfg->PchFivrExtV1p05RailIccMaximum =
			config->ext_fivr_settings.v1p05_icc_max_ma;

	s_cfg->PchFivrExtVnnRailIccMaximum =
			config->ext_fivr_settings.vnn_icc_max_ma;
}

static void fill_fsps_fivr_rfi_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	/* transform from Hz to 100 KHz */
	s_cfg->FivrRfiFrequency = config->fivr_rfi_frequency / (100 * KHz);
	s_cfg->FivrSpreadSpectrum = config->fivr_spread_spectrum;
}

static void fill_fsps_acoustic_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_alderlake_config *config)
{
	s_cfg->AcousticNoiseMitigation = config->acoustic_noise_mitigation;

	if (s_cfg->AcousticNoiseMitigation) {
		s_cfg->PreWake = config->PreWake;
		for (int i = 0; i < NUM_VR_DOMAINS; i++) {
			s_cfg->FastPkgCRampDisable[i] = config->fast_pkg_c_ramp_disable[i];
			s_cfg->SlowSlewRate[i] = config->slow_slew_rate[i];
		}
	}
}

static void soc_silicon_init_params(FSP_S_CONFIG *s_cfg,
		struct soc_intel_alderlake_config *config)
{
	/* Override settings per board if required. */
	mainboard_update_soc_chip_config(config);

	void (*const fill_fsps_params[])(FSP_S_CONFIG *s_cfg,
			const struct soc_intel_alderlake_config *config) = {
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
		fill_fsps_thc_params,
		fill_fsps_tbt_params,
		fill_fsps_8254_params,
		fill_fsps_pm_timer_params,
		fill_fsps_storage_params,
		fill_fsps_pcie_params,
		fill_fsps_cpu_pcie_params,
		fill_fsps_misc_power_params,
		fill_fsps_irq_params,
		fill_fsps_fivr_params,
		fill_fsps_fivr_rfi_params,
		fill_fsps_acoustic_params,
	};

	for (size_t i = 0; i < ARRAY_SIZE(fill_fsps_params); i++)
		fill_fsps_params[i](s_cfg, config);
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	struct soc_intel_alderlake_config *config;
	FSP_S_CONFIG *s_cfg = &supd->FspsConfig;

	config = config_of_soc();
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
