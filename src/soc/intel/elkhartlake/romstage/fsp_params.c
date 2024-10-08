/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/device.h>
#include <fsp/util.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pcie_rp.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>
#include <static.h>

/* ISA Serial Base selection. */
enum {
	ISA_SERIAL_BASE_ADDR_3F8,
	ISA_SERIAL_BASE_ADDR_2F8,
} isa_serial_uart_base;

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_elkhartlake_config *config)
{
	/*
	 * If IGD is enabled, set IGD stolen size to 60MB.
	 * Otherwise, skip IGD init in FSP.
	 */
	m_cfg->InternalGfx = !CONFIG(SOC_INTEL_DISABLE_IGD) && is_devfn_enabled(SA_DEVFN_IGD);
	m_cfg->IgdDvmt50PreAlloc = m_cfg->InternalGfx ? 0xFE : 0;

	m_cfg->TsegSize = CONFIG_SMM_TSEG_SIZE;
	m_cfg->SaGv = config->SaGv;
	m_cfg->RMT = config->RMT;

	m_cfg->PcieRpEnableMask = pcie_rp_enable_mask(pch_rp_groups);

	FSP_ARRAY_LOAD(m_cfg->PcieClkSrcUsage, config->PcieClkSrcUsage);
	FSP_ARRAY_LOAD(m_cfg->PcieClkSrcClkReq, config->PcieClkSrcClkReq);

	m_cfg->PrmrrSize = get_valid_prmrr_size();

	/* Disable BIOS Guard */
	m_cfg->BiosGuard = 0;

	/* Set CPU Ratio */
	m_cfg->CpuRatio = 0;

	/* Change VmxEnable UPD value according to ENABLE_VMX Kconfig */
	m_cfg->VmxEnable = CONFIG(ENABLE_VMX);

	/* PCH Master Gating Control */
	if (config->realtime_tuning_enable) {
		m_cfg->PchMasterClockGating = 0;
		m_cfg->PchMasterPowerGating = 0;
		m_cfg->DisPgCloseIdleTimeout = 0;
		m_cfg->PowerDownMode = 0;
	} else {
		m_cfg->PchMasterClockGating = 1;
		m_cfg->PchMasterPowerGating = 1;
	}

	m_cfg->SmbusEnable = is_devfn_enabled(PCH_DEVFN_SMBUS);

	/* DMAR related config */
	m_cfg->VtdDisable = 0;
	m_cfg->X2ApicOptOut = 0x1;
	if (m_cfg->InternalGfx) {
		m_cfg->VtdIgdEnable = 0x1;
		m_cfg->DisableTeIgd = 0x1;
		m_cfg->VtdBaseAddress[0] = GFXVT_BASE_ADDRESS;
	}
	m_cfg->VtdBaseAddress[2] = VTVC0_BASE_ADDRESS;

	/* DllBwEn0/1/2/3, per frequency */
	m_cfg->DllBwEn0 = 0;
	m_cfg->DllBwEn1 = 0;
	m_cfg->DllBwEn2 = 0;
	m_cfg->DllBwEn3 = 0;

	/* Disable and Lock Watch Dog Register */
	m_cfg->WdtDisableAndLock = 0x1;

	m_cfg->HeciCommunication2 = config->Heci2Enable;

	/* Audio */
	m_cfg->PchHdaEnable = is_devfn_enabled(PCH_DEVFN_HDA);
	m_cfg->PchHdaDspEnable = config->PchHdaDspEnable;
	m_cfg->PchHdaAudioLinkHdaEnable = config->PchHdaAudioLinkHdaEnable;

	FSP_ARRAY_LOAD(m_cfg->PchHdaSdiEnable, config->PchHdaSdiEnable);
	FSP_ARRAY_LOAD(m_cfg->PchHdaAudioLinkDmicEnable, config->PchHdaAudioLinkDmicEnable);
	FSP_ARRAY_LOAD(m_cfg->PchHdaAudioLinkSspEnable, config->PchHdaAudioLinkSspEnable);
	FSP_ARRAY_LOAD(m_cfg->PchHdaAudioLinkSndwEnable, config->PchHdaAudioLinkSndwEnable);

	/* Skip the CPU replacement check */
	m_cfg->SkipCpuReplacementCheck = config->SkipCpuReplacementCheck;

	/* Processor Early Power On Configuration FCLK setting */
	m_cfg->FClkFrequency = 0x1;

	/* Ib-Band ECC configuration */
	if (config->ibecc.enable) {
		m_cfg->Ibecc = !!config->ibecc.enable;
		m_cfg->IbeccParity = !!config->ibecc.parity_en;
		m_cfg->IbeccOperationMode = config->ibecc.mode;
		if (m_cfg->IbeccOperationMode == IBECC_PER_REGION) {
			FSP_ARRAY_LOAD(m_cfg->IbeccProtectedRegionEnable,
				       config->ibecc.region_enable);
			FSP_ARRAY_LOAD(m_cfg->IbeccProtectedRegionBase,
				       config->ibecc.region_base);
			FSP_ARRAY_LOAD(m_cfg->IbeccProtectedRegionMask,
				       config->ibecc.region_mask);
		}
	}
	/* PSE (Intel Programmable Services Engine) switch */
	m_cfg->PchPseEnable = CONFIG(PSE_ENABLE) && cbfs_file_exists("pse.bin");

	/* DDR Frequency Limit */
	if (config->max_dram_speed_mts) {
		m_cfg->DdrFreqLimit = config->max_dram_speed_mts;
		m_cfg->DdrSpeedControl = 1;
	}
}


static void fill_fsp_debug_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_elkhartlake_config *config)
{
	/* Set debug interface flags */
	m_cfg->PcdDebugInterfaceFlags = CONFIG(DRIVERS_UART_8250IO) ?
		DEBUG_INTERFACE_UART_8250IO : DEBUG_INTERFACE_LPSS_SERIAL_IO;
	m_cfg->SerialIoUartDebugControllerNumber = CONFIG_UART_FOR_CONSOLE;

	/* Set debug probe type */
	m_cfg->PlatformDebugConsent = CONFIG_SOC_INTEL_COMMON_DEBUG_CONSENT;

	/* TraceHub configuration */
	if (is_devfn_enabled(PCH_DEVFN_TRACEHUB) && config->TraceHubMode) {
		m_cfg->PcdDebugInterfaceFlags |= DEBUG_INTERFACE_TRACEHUB;
		m_cfg->PchTraceHubMode = config->TraceHubMode;
		m_cfg->CpuTraceHubMode = config->TraceHubMode;
	}

	if (CONFIG(CONSOLE_SERIAL)) {
		enum fsp_log_level log_level = fsp_map_console_log_level();
		/* Set Serial debug message level */
		m_cfg->PcdSerialDebugLevel = log_level;
		/* Set MRC debug level */
		m_cfg->MrcTaskDebugEnable = 0xff;
	} else {
		/* Disable Serial debug message */
		m_cfg->PcdSerialDebugLevel = 0;
		/* Disable MRC debug message */
		m_cfg->MrcTaskDebugEnable = 0;
	}

	if (CONFIG(DRIVERS_UART_8250IO)) {
		if (CONFIG_TTYS0_BASE == 0x3f8)
			m_cfg->PcdIsaSerialUartBase = ISA_SERIAL_BASE_ADDR_3F8;
		else if (CONFIG_TTYS0_BASE == 0x2f8)
			m_cfg->PcdIsaSerialUartBase = ISA_SERIAL_BASE_ADDR_2F8;
		else {
			printk(BIOS_ERR, "Unsupported UART I/O base, "
				"skipping FSP UART base UPD setting\n");
		}
	}
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const struct soc_intel_elkhartlake_config *config = config_of_soc();
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	soc_memory_init_params(m_cfg, config);
	fill_fsp_debug_params(m_cfg, config);

	mainboard_memory_init_params(mupd);
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
