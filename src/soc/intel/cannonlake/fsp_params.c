/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootsplash.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <drivers/usb/acpi/chip.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <option.h>
#include <intelblocks/irq.h>
#include <intelblocks/lpss.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <static.h>
#include <string.h>
#include <types.h>

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

static const struct slot_irq_constraints irq_constraints[] = {
	{
		.slot = SA_DEV_SLOT_PEG,
		.fns = {
			FIXED_INT_PIRQ(SA_DEVFN_PEG0, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(SA_DEVFN_PEG1, PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(SA_DEVFN_PEG2, PCI_INT_C, PIRQ_C),
			/*
			 * It looks like FSP does not apply this mapping properly to
			 * the PEG functions. The PINx to PIRQx mapping needs to be there
			 * in ACPI however in case PIN D is used.
			 */
			FIXED_INT_PIRQ(PCI_DEVFN(SA_DEV_SLOT_PEG, 3), PCI_INT_D, PIRQ_D),
		},
	},
	{
		.slot = SA_DEV_SLOT_IGD,
		.fns = {
			ANY_PIRQ(SA_DEVFN_IGD),
		},
	},
	{
		.slot = SA_DEV_SLOT_TS,
		.fns = {
			ANY_PIRQ(SA_DEVFN_TS),
		},
	},
	{
		.slot = SA_DEV_SLOT_IPU,
		.fns = {
			ANY_PIRQ(SA_DEVFN_IPU),
		},
	},
	{
		.slot = SA_DEV_SLOT_GNA,
		.fns = {
			ANY_PIRQ(SA_DEVFN_GNA),
		},
	},
	{
		.slot = PCH_DEV_SLOT_THERMAL,
		.fns = {
			ANY_PIRQ(PCH_DEVFN_THERMAL),
#if !CONFIG(SOC_INTEL_CANNONLAKE_PCH_H)
			ANY_PIRQ(PCH_DEVFN_UFS),
#endif
			DIRECT_IRQ(PCH_DEVFN_GSPI2),
		},
	},
	{
		.slot = PCH_DEV_SLOT_ISH,
		.fns = {
			DIRECT_IRQ(PCH_DEVFN_ISH),
		},
	},
	{
		.slot = PCH_DEV_SLOT_XHCI,
		.fns = {
			ANY_PIRQ(PCH_DEVFN_XHCI),
			ANY_PIRQ(PCH_DEVFN_USBOTG),
			ANY_PIRQ(PCH_DEVFN_CNViWIFI),
			ANY_PIRQ(PCH_DEVFN_SDCARD),
		},
	},
	{
		.slot = PCH_DEV_SLOT_SIO1,
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
		.slot = PCH_DEV_SLOT_SIO2,
		.fns = {
#if !CONFIG(SOC_INTEL_CANNONLAKE_PCH_H)
			DIRECT_IRQ(PCH_DEVFN_I2C4),
			DIRECT_IRQ(PCH_DEVFN_I2C5),
#endif
			DIRECT_IRQ(PCH_DEVFN_UART2),
		},
	},
#if !CONFIG(SOC_INTEL_CANNONLAKE_PCH_H)
	{
		.slot = PCH_DEV_SLOT_STORAGE,
		.fns = {
			ANY_PIRQ(PCH_DEVFN_EMMC),
		},
	},
#endif
#if CONFIG(SOC_INTEL_CANNONLAKE_PCH_H)
	{
		.slot = PCH_DEV_SLOT_PCIE_2,
		.fns = {
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE17, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE18, PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE19, PCI_INT_C, PIRQ_C),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE20, PCI_INT_D, PIRQ_D),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE21, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE22, PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE23, PCI_INT_C, PIRQ_C),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE24, PCI_INT_D, PIRQ_D),
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
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE13, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE14, PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE15, PCI_INT_C, PIRQ_C),
			FIXED_INT_PIRQ(PCH_DEVFN_PCIE16, PCI_INT_D, PIRQ_D),
		},
	},
	{
		.slot = PCH_DEV_SLOT_SIO3,
		.fns = {
			DIRECT_IRQ(PCH_DEVFN_UART0),
			DIRECT_IRQ(PCH_DEVFN_UART1),
			DIRECT_IRQ(PCH_DEVFN_GSPI0),
			DIRECT_IRQ(PCH_DEVFN_GSPI1),
		},
	},
	{
		.slot = PCH_DEV_SLOT_LPC,
		.fns = {
			ANY_PIRQ(PCH_DEVFN_HDA),
			ANY_PIRQ(PCH_DEVFN_SMBUS),
			ANY_PIRQ(PCH_DEVFN_GBE),
			FIXED_INT_ANY_PIRQ(PCH_DEVFN_TRACEHUB, PCI_INT_A)
		},
	},
};

/*
 * Given an enum for PCH_SERIAL_IO_MODE, 1 needs to be subtracted to get the FSP
 * UPD expected value for Serial IO since valid enum index starts from 1.
 */
#define PCH_SERIAL_IO_INDEX(x)		((x) - 1)

static uint8_t get_param_value(const config_t *config, uint32_t dev_offset)
{
	if (!is_devfn_enabled(serial_io_dev[dev_offset]))
		return PCH_SERIAL_IO_INDEX(PchSerialIoDisabled);

	if ((config->SerialIoDevMode[dev_offset] >= PchSerialIoMax) ||
	    (config->SerialIoDevMode[dev_offset] == PchSerialIoNotInitialized))
		return PCH_SERIAL_IO_INDEX(PchSerialIoPci);

	/*
	 * Correct Enum index starts from 1, so subtract 1 while returning value
	 */
	return PCH_SERIAL_IO_INDEX(config->SerialIoDevMode[dev_offset]);
}

static void parse_devicetree(const config_t *config, FSP_S_CONFIG *s_cfg)
{
#if CONFIG(SOC_INTEL_COMETLAKE)
	uint32_t dev_offset = 0;
	uint32_t i = 0;

	for (i = 0; i < CONFIG_SOC_INTEL_I2C_DEV_MAX; i++, dev_offset++) {
		s_cfg->SerialIoI2cMode[i] =
				get_param_value(config, dev_offset);
	}

	for (i = 0; i < CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX; i++,
	     dev_offset++) {
		s_cfg->SerialIoSpiMode[i] =
				get_param_value(config, dev_offset);
	}

	for (i = 0; i < SOC_INTEL_CML_UART_DEV_MAX; i++, dev_offset++) {
		s_cfg->SerialIoUartMode[i] =
				get_param_value(config, dev_offset);
	}
#else
	for (int i = 0; i < ARRAY_SIZE(serial_io_dev); i++)
		s_cfg->SerialIoDevMode[i] = get_param_value(config, i);
#endif
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
		if (is_pch_slot(entry->devfn))
			++pch_total;
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
	FSP_S_CONFIG *s_cfg = &supd->FspsConfig;
	FSP_S_TEST_CONFIG *tconfig = &supd->FspsTestConfig;
	struct device *dev;

	config_t *config = config_of_soc();

	/* Parse device tree and enable/disable devices */
	parse_devicetree(config, s_cfg);

	/* Load VBT before devicetree-specific config. */
	s_cfg->GraphicsConfigPtr = (uintptr_t)vbt_get();

	mainboard_silicon_init_params(supd);

	const struct soc_power_limits_config *soc_config;
	soc_config = &config->power_limits_config;
	/* Set PsysPmax if it is available from DT */
	if (soc_config->psys_pmax) {
		printk(BIOS_DEBUG, "psys_pmax = %dW\n", soc_config->psys_pmax);
		/* PsysPmax is in unit of 1/8 Watt */
		tconfig->PsysPmax = soc_config->psys_pmax * 8;
	}

	/* Unlock upper 8 bytes of RTC RAM */
	s_cfg->PchLockDownRtcMemoryLock = 0;

	/* SATA */
	s_cfg->SataEnable = is_devfn_enabled(PCH_DEVFN_SATA);
	if (s_cfg->SataEnable) {
		s_cfg->SataMode = config->SataMode;
		s_cfg->SataPwrOptEnable = config->satapwroptimize;
		s_cfg->SataSalpSupport = config->SataSalpSupport;
		memcpy(s_cfg->SataPortsEnable, config->SataPortsEnable,
			sizeof(s_cfg->SataPortsEnable));
		memcpy(s_cfg->SataPortsDevSlp, config->SataPortsDevSlp,
			sizeof(s_cfg->SataPortsDevSlp));
		memcpy(s_cfg->SataPortsHotPlug, config->SataPortsHotPlug,
			sizeof(s_cfg->SataPortsHotPlug));
#if CONFIG(SOC_INTEL_COMETLAKE)
		memcpy(s_cfg->SataPortsDevSlpResetConfig,
			config->SataPortsDevSlpResetConfig,
			sizeof(s_cfg->SataPortsDevSlpResetConfig));
#endif
	}
	s_cfg->SlpS0WithGbeSupport = 0;
	s_cfg->PchPmSlpS0VmRuntimeControl = config->PchPmSlpS0VmRuntimeControl;
	s_cfg->PchPmSlpS0Vm070VSupport = config->PchPmSlpS0Vm070VSupport;
	s_cfg->PchPmSlpS0Vm075VSupport = config->PchPmSlpS0Vm075VSupport;

	/* S0ix */
	config->s0ix_enable = get_uint_option("s0ix_enable", config->s0ix_enable);
	s_cfg->PchPmSlpS0Enable = config->s0ix_enable;

	/* Lan */
	s_cfg->PchLanEnable = is_devfn_enabled(PCH_DEVFN_GBE);
	if (s_cfg->PchLanEnable) {
		if (config->s0ix_enable) {
			/*
			 * The VmControl UPDs need to be set as per board
			 * design to allow voltage margining in S0ix to lower
			 * power consumption.
			 * But if GbE is enabled, voltage magining cannot be
			 * enabled, so the Vm control UPDs need to be set to 0.
			 */
			s_cfg->SlpS0WithGbeSupport = 1;
			s_cfg->PchPmSlpS0VmRuntimeControl = 0;
			s_cfg->PchPmSlpS0Vm070VSupport = 0;
			s_cfg->PchPmSlpS0Vm075VSupport = 0;
			ignore_gbe_ltr();
		}
	}

	/* Audio */
	s_cfg->PchHdaDspEnable = config->PchHdaDspEnable;
	s_cfg->PchHdaIDispCodecDisconnect = config->PchHdaIDispCodecDisconnect;
	s_cfg->PchHdaAudioLinkHda = config->PchHdaAudioLinkHda;
	s_cfg->PchHdaAudioLinkDmic0 = config->PchHdaAudioLinkDmic0;
	s_cfg->PchHdaAudioLinkDmic1 = config->PchHdaAudioLinkDmic1;
	s_cfg->PchHdaAudioLinkSsp0 = config->PchHdaAudioLinkSsp0;
	s_cfg->PchHdaAudioLinkSsp1 = config->PchHdaAudioLinkSsp1;
	s_cfg->PchHdaAudioLinkSsp2 = config->PchHdaAudioLinkSsp2;
	s_cfg->PchHdaAudioLinkSndw1 = config->PchHdaAudioLinkSndw1;
	s_cfg->PchHdaAudioLinkSndw2 = config->PchHdaAudioLinkSndw2;
	s_cfg->PchHdaAudioLinkSndw3 = config->PchHdaAudioLinkSndw3;
	s_cfg->PchHdaAudioLinkSndw4 = config->PchHdaAudioLinkSndw4;

	/* eDP device */
	s_cfg->DdiPortEdp = config->DdiPortEdp;

	/* HPD of DDI ports */
	s_cfg->DdiPortBHpd = config->DdiPortBHpd;
	s_cfg->DdiPortCHpd = config->DdiPortCHpd;
	s_cfg->DdiPortDHpd = config->DdiPortDHpd;
	s_cfg->DdiPortFHpd = config->DdiPortFHpd;

	/* DDC of DDI ports */
	s_cfg->DdiPortBDdc = config->DdiPortBDdc;
	s_cfg->DdiPortCDdc = config->DdiPortCDdc;
	s_cfg->DdiPortDDdc = config->DdiPortDDdc;
	s_cfg->DdiPortFDdc = config->DdiPortFDdc;

	/* WOL */
	s_cfg->PchPmPcieWakeFromDeepSx = config->LanWakeFromDeepSx;
	s_cfg->PchPmWolEnableOverride = config->WolEnableOverride;

	/* disable Legacy PME */
	memset(s_cfg->PcieRpPmSci, 0, sizeof(s_cfg->PcieRpPmSci));

	/* Legacy 8254 timer support */
	bool use_8254 = get_uint_option("legacy_8254_timer", CONFIG(USE_LEGACY_8254_TIMER));
	s_cfg->Enable8254ClockGating = !use_8254;
	s_cfg->Enable8254ClockGatingOnS3 = !use_8254;

	/*
	 * Legacy PM ACPI Timer (and TCO Timer)
	 * This *must* be 1 in any case to keep FSP from
	 *  1) enabling PM ACPI Timer emulation in uCode.
	 *  2) disabling the PM ACPI Timer.
	 * We handle both by ourself!
	 */
	s_cfg->EnableTcoTimer = 1;

	/* USB */
	for (i = 0; i < ARRAY_SIZE(config->usb2_ports); i++) {
		s_cfg->PortUsb20Enable[i] = config->usb2_ports[i].enable;
		s_cfg->Usb2AfePetxiset[i] = config->usb2_ports[i].pre_emp_bias;
		s_cfg->Usb2AfeTxiset[i] = config->usb2_ports[i].tx_bias;
		s_cfg->Usb2AfePredeemp[i] =
			config->usb2_ports[i].tx_emp_enable;
		s_cfg->Usb2AfePehalfbit[i] = config->usb2_ports[i].pre_emp_bit;

		if (config->usb2_ports[i].enable)
			s_cfg->Usb2OverCurrentPin[i] = config->usb2_ports[i].ocpin;
		else
			s_cfg->Usb2OverCurrentPin[i] = 0xff;
	}

	if (config->PchUsb2PhySusPgDisable)
		s_cfg->PchUsb2PhySusPgEnable = 0;

	for (i = 0; i < ARRAY_SIZE(config->usb3_ports); i++) {
		s_cfg->PortUsb30Enable[i] = config->usb3_ports[i].enable;
		if (config->usb3_ports[i].enable) {
			s_cfg->Usb3OverCurrentPin[i] = config->usb3_ports[i].ocpin;
		} else {
			s_cfg->Usb3OverCurrentPin[i] = 0xff;
		}
		if (config->usb3_ports[i].tx_de_emp) {
			s_cfg->Usb3HsioTxDeEmphEnable[i] = 1;
			s_cfg->Usb3HsioTxDeEmph[i] =
				config->usb3_ports[i].tx_de_emp;
		}
		if (config->usb3_ports[i].tx_downscale_amp) {
			s_cfg->Usb3HsioTxDownscaleAmpEnable[i] = 1;
			s_cfg->Usb3HsioTxDownscaleAmp[i] =
				config->usb3_ports[i].tx_downscale_amp;
		}
#if CONFIG(SOC_INTEL_COMETLAKE)
		if (config->usb3_ports[i].gen2_tx_rate0_uniq_tran_enable) {
			s_cfg->Usb3HsioTxRate0UniqTranEnable[i] = 1;
			s_cfg->Usb3HsioTxRate0UniqTran[i] =
				config->usb3_ports[i].gen2_tx_rate0_uniq_tran;
		}
		if (config->usb3_ports[i].gen2_tx_rate1_uniq_tran_enable) {
			s_cfg->Usb3HsioTxRate1UniqTranEnable[i] = 1;
			s_cfg->Usb3HsioTxRate1UniqTran[i] =
				config->usb3_ports[i].gen2_tx_rate1_uniq_tran;
		}
		if (config->usb3_ports[i].gen2_tx_rate2_uniq_tran_enable) {
			s_cfg->Usb3HsioTxRate2UniqTranEnable[i] = 1;
			s_cfg->Usb3HsioTxRate2UniqTran[i] =
				config->usb3_ports[i].gen2_tx_rate2_uniq_tran;
		}
		if (config->usb3_ports[i].gen2_tx_rate3_uniq_tran_enable) {
			s_cfg->Usb3HsioTxRate3UniqTranEnable[i] = 1;
			s_cfg->Usb3HsioTxRate3UniqTran[i] =
				config->usb3_ports[i].gen2_tx_rate3_uniq_tran;
		}
#endif
		if (config->usb3_ports[i].gen2_rx_tuning_enable) {
			s_cfg->PchUsbHsioRxTuningEnable[i] =
				config->usb3_ports[i].gen2_rx_tuning_enable;
			s_cfg->PchUsbHsioRxTuningParameters[i] =
				config->usb3_ports[i].gen2_rx_tuning_params;
			s_cfg->PchUsbHsioFilterSel[i] =
				config->usb3_ports[i].gen2_rx_filter_sel;
		}
	}

	s_cfg->XdciEnable = xdci_can_enable(PCH_DEVFN_USBOTG);

	/* Set Debug serial port */
	s_cfg->SerialIoDebugUartNumber = CONFIG_UART_FOR_CONSOLE;
#if !CONFIG(SOC_INTEL_COMETLAKE)
	s_cfg->SerialIoEnableDebugUartAfterPost = CONFIG(INTEL_LPSS_UART_FOR_CONSOLE);
#endif

	/* Enable CNVi Wifi if enabled in device tree */
#if CONFIG(SOC_INTEL_COMETLAKE)
	struct device *port = NULL;
	struct drivers_usb_acpi_config *usb_cfg = NULL;
	bool usb_audio_offload = false;

	/* Search through the devicetree for matching USB devices */
	while ((port = dev_find_path(port, DEVICE_PATH_USB)) != NULL) {
		/* Skip ports that are not enabled or not of USB type */
		if (!port->enabled || port->path.type != DEVICE_PATH_USB)
			continue;

		usb_cfg = port->chip_info;
		if (usb_cfg && usb_cfg->cnvi_bt_audio_offload) {
			usb_audio_offload = true;
			break;
		}
	}

	/* CNVi */
	s_cfg->CnviMode = is_devfn_enabled(PCH_DEVFN_CNViWIFI);
	s_cfg->CnviBtCore = config->CnviBtCore;
	s_cfg->CnviBtAudioOffload = config->CnviBtAudioOffload;

	if (!s_cfg->CnviBtCore && s_cfg->CnviBtAudioOffload) {
		printk(BIOS_ERR, "BT offload is enabled without CNVi BT being enabled\n");
		s_cfg->CnviBtAudioOffload = 0;
	}
	if (!s_cfg->CnviMode && s_cfg->CnviBtCore) {
		printk(BIOS_ERR, "CNVi BT is enabled without CNVi being enabled\n");
		s_cfg->CnviBtCore = 0;
		s_cfg->CnviBtAudioOffload = 0;
	}
	if (s_cfg->CnviBtAudioOffload && !usb_audio_offload) {
		printk(BIOS_WARNING, "CNVi BT Audio offload enabled but not in USB driver.\n");
	}
	if (!s_cfg->CnviBtAudioOffload && usb_cfg && usb_audio_offload) {
		printk(BIOS_ERR, "USB BT Audio offload enabled but CNVi BT offload disabled\n");
		usb_cfg->cnvi_bt_audio_offload = 0;
	}
#else
	s_cfg->PchCnviMode = is_devfn_enabled(PCH_DEVFN_CNViWIFI);
#endif
	/* PCI Express */
	for (i = 0; i < ARRAY_SIZE(config->PcieClkSrcUsage); i++) {
		if (config->PcieClkSrcUsage[i] == 0)
			config->PcieClkSrcUsage[i] = PCIE_CLK_NOTUSED;
		else if (config->PcieClkSrcUsage[i] == PCIE_CLK_RP0)
			config->PcieClkSrcUsage[i] = 0;
	}
	memcpy(s_cfg->PcieClkSrcUsage, config->PcieClkSrcUsage,
	       sizeof(config->PcieClkSrcUsage));
	memcpy(s_cfg->PcieClkSrcClkReq, config->PcieClkSrcClkReq,
	       sizeof(config->PcieClkSrcClkReq));

	memcpy(s_cfg->PcieRpAdvancedErrorReporting,
		config->PcieRpAdvancedErrorReporting,
		sizeof(config->PcieRpAdvancedErrorReporting));

	memcpy(s_cfg->PcieRpLtrEnable, config->PcieRpLtrEnable,
	       sizeof(config->PcieRpLtrEnable));
	memcpy(s_cfg->PcieRpSlotImplemented, config->PcieRpSlotImplemented,
	       sizeof(config->PcieRpSlotImplemented));
	memcpy(s_cfg->PcieRpHotPlug, config->PcieRpHotPlug,
	       sizeof(config->PcieRpHotPlug));

	for (i = 0; i < CONFIG_MAX_ROOT_PORTS; i++) {
		s_cfg->PcieRpMaxPayload[i] = config->PcieRpMaxPayload[i];
		if (config->PcieRpAspm[i])
			s_cfg->PcieRpAspm[i] = config->PcieRpAspm[i] - 1;
	};

	/* eMMC and SD */
	s_cfg->ScsEmmcEnabled = is_devfn_enabled(PCH_DEVFN_EMMC);
	if (s_cfg->ScsEmmcEnabled) {
		s_cfg->ScsEmmcHs400Enabled = config->ScsEmmcHs400Enabled;
		s_cfg->PchScsEmmcHs400DllDataValid = config->EmmcHs400DllNeed;
		if (config->EmmcHs400DllNeed == 1) {
			s_cfg->PchScsEmmcHs400RxStrobeDll1 =
				config->EmmcHs400RxStrobeDll1;
			s_cfg->PchScsEmmcHs400TxDataDll =
				config->EmmcHs400TxDataDll;
		}
	}

	s_cfg->ScsSdCardEnabled = is_devfn_enabled(PCH_DEVFN_SDCARD);
	if (s_cfg->ScsSdCardEnabled) {
		s_cfg->SdCardPowerEnableActiveHigh = CONFIG(MB_HAS_ACTIVE_HIGH_SD_PWR_ENABLE);
#if CONFIG(SOC_INTEL_COMETLAKE)
		s_cfg->ScsSdCardWpPinEnabled = config->ScsSdCardWpPinEnabled;
#endif
	}

	s_cfg->ScsUfsEnabled = is_devfn_enabled(PCH_DEVFN_UFS);

	s_cfg->Heci3Enabled = is_devfn_enabled(PCH_DEVFN_CSE_3);
	/*
	 * coreboot will handle disabling of HECI1 device if `DISABLE_HECI1_AT_PRE_BOOT`
	 * config is selected hence, don't let FSP to disable the HECI1 device and set
	 * the `Heci1Disabled` UPD to `0`.
	 */
	s_cfg->Heci1Disabled = 0;
	s_cfg->Device4Enable = config->Device4Enable;

	/* Teton Glacier hybrid storage support */
	s_cfg->TetonGlacierMode = config->TetonGlacierMode;

	/* VrConfig Settings for 5 domains
	 * 0 = System Agent, 1 = IA Core, 2 = Ring,
	 * 3 = GT unsliced,  4 = GT sliced */
	for (i = 0; i < ARRAY_SIZE(config->domain_vr_config); i++)
		fill_vr_domain_config(s_cfg, i, &config->domain_vr_config[i]);

	/* Acoustic Noise Mitigation */
	s_cfg->AcousticNoiseMitigation = config->AcousticNoiseMitigation;
	s_cfg->SlowSlewRateForIa = config->SlowSlewRateForIa;
	s_cfg->SlowSlewRateForGt = config->SlowSlewRateForGt;
	s_cfg->SlowSlewRateForSa = config->SlowSlewRateForSa;
	s_cfg->SlowSlewRateForFivr = config->SlowSlewRateForFivr;
	s_cfg->FastPkgCRampDisableIa = config->FastPkgCRampDisableIa;
	s_cfg->FastPkgCRampDisableGt = config->FastPkgCRampDisableGt;
	s_cfg->FastPkgCRampDisableSa = config->FastPkgCRampDisableSa;
	s_cfg->FastPkgCRampDisableFivr = config->FastPkgCRampDisableFivr;

	/* Apply minimum assertion width settings if non-zero */
	if (config->PchPmSlpS3MinAssert)
		s_cfg->PchPmSlpS3MinAssert = config->PchPmSlpS3MinAssert;
	if (config->PchPmSlpS4MinAssert)
		s_cfg->PchPmSlpS4MinAssert = config->PchPmSlpS4MinAssert;
	if (config->PchPmSlpSusMinAssert)
		s_cfg->PchPmSlpSusMinAssert = config->PchPmSlpSusMinAssert;
	if (config->PchPmSlpAMinAssert)
		s_cfg->PchPmSlpAMinAssert = config->PchPmSlpAMinAssert;

#if CONFIG(SOC_INTEL_COMETLAKE)
	if (config->PchPmPwrCycDur)
		s_cfg->PchPmPwrCycDur = get_pm_pwr_cyc_dur(config->PchPmSlpS4MinAssert,
				config->PchPmSlpS3MinAssert, config->PchPmSlpAMinAssert,
				config->PchPmPwrCycDur);
#endif

	/* Set TccActivationOffset */
	tconfig->TccActivationOffset = config->tcc_offset;
	tconfig->TccOffsetClamp = config->tcc_offset > 0;

	/* Unlock all GPIO pads */
	tconfig->PchUnlockGpioPads = config->PchUnlockGpioPads;

	/* Set correct Sirq mode based on config */
	s_cfg->PchSirqEnable = config->serirq_mode != SERIRQ_OFF;
	s_cfg->PchSirqMode = config->serirq_mode == SERIRQ_CONTINUOUS;

	/*
	 * GSPI Chip Select parameters
	 * The GSPI driver assumes that CS0 is the used chip-select line,
	 * therefore only CS0 is configured below.
	 */
#if CONFIG(SOC_INTEL_COMETLAKE)
	configure_gspi_cs(0, config, &s_cfg->SerialIoSpi0CsPolarity[0],
			&s_cfg->SerialIoSpi0CsEnable[0],
			&s_cfg->SerialIoSpiDefaultCsOutput[0]);
	configure_gspi_cs(1, config, &s_cfg->SerialIoSpi1CsPolarity[0],
			&s_cfg->SerialIoSpi1CsEnable[0],
			&s_cfg->SerialIoSpiDefaultCsOutput[1]);
	configure_gspi_cs(2, config, &s_cfg->SerialIoSpi2CsPolarity[0],
			&s_cfg->SerialIoSpi2CsEnable[0],
			&s_cfg->SerialIoSpiDefaultCsOutput[2]);
#else
	for (i = 0; i < CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX; i++)
		configure_gspi_cs(i, config,
				&s_cfg->SerialIoSpiCsPolarity[0], NULL, NULL);
#endif

	/* Chipset Lockdown */
	const bool lockdown_by_fsp = get_lockdown_config() == CHIPSET_LOCKDOWN_FSP;
	tconfig->PchLockDownGlobalSmi = lockdown_by_fsp;
	tconfig->PchLockDownBiosInterface = lockdown_by_fsp;
	s_cfg->PchLockDownBiosLock = lockdown_by_fsp;
	s_cfg->PchLockDownRtcMemoryLock = lockdown_by_fsp;
	tconfig->SkipPamLock = !lockdown_by_fsp;
#if CONFIG(SOC_INTEL_COMETLAKE)
	/*
	 * Making this config "0" means FSP won't set the FLOCKDN bit
	 * of SPIBAR + 0x04 (i.e., Bit 15 of BIOS_HSFSTS_CTL).
	 * So, it becomes coreboot's responsibility to set this bit
	 * before end of POST for security concerns.
	 */
	s_cfg->SpiFlashCfgLockDown = lockdown_by_fsp;
#endif
	/*
	 * IA32_DEBUG_INTERFACE_MSR has to be locked by coreboot,
	 * because FSP does not do it unless DebugInterfaceEnable is 1.
	 * But to use Intel TXT, the debug interface has to be disabled,
	 * so let coreboot handle the IA32_DEBUG_INTERFACE_MSR programming.
	 */
	supd->FspsConfig.DebugInterfaceEnable = 0;
	supd->FspsTestConfig.DebugInterfaceEnable = 0;
	supd->FspsTestConfig.DebugInterfaceLockEnable = 0;

#if !CONFIG(SOC_INTEL_COMETLAKE)
	s_cfg->VrPowerDeliveryDesign = config->VrPowerDeliveryDesign;
#endif

	s_cfg->PeiGraphicsPeimInit = CONFIG(RUN_FSP_GOP) && is_devfn_enabled(SA_DEVFN_IGD);

	s_cfg->PavpEnable = CONFIG(PAVP);

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

	s_cfg->SiSsidTablePtr = (uintptr_t)ssid_table;
	s_cfg->SiNumberOfSsidTableEntry = ARRAY_SIZE(ssid_table);

	/* Assign PCI IRQs */
	if (!assign_pci_irqs(irq_constraints, ARRAY_SIZE(irq_constraints)))
		die("ERROR: Unable to assign PCI IRQs, and no ACPI _PRT table is defined\n");

	size_t pch_count = 0;
	const SI_PCH_DEVICE_INTERRUPT_CONFIG *upd_irqs = pci_irq_to_fsp(&pch_count);
	s_cfg->DevIntConfigPtr = (UINT32)((uintptr_t)upd_irqs);
	s_cfg->NumOfDevIntConfig = pch_count;
	printk(BIOS_INFO, "IRQ: Using dynamically assigned PCI IO-APIC IRQs\n");
}

/* Mainboard GPIO Configuration */
__weak void mainboard_silicon_init_params(FSPS_UPD *supd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

/* Handle FSP logo params */
void soc_load_logo(FSPS_UPD *supd)
{
	size_t logo_size;
	supd->FspsConfig.LogoPtr = (uintptr_t)bmp_load_logo(&logo_size);
	supd->FspsConfig.LogoSize = (uint32_t)logo_size;
}
