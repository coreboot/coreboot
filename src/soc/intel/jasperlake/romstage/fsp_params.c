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

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_jasperlake_config *config)
{
	unsigned int i;

	/*
	 * If IGD is enabled, set IGD stolen size to 60MB.
	 * Otherwise, skip IGD init in FSP.
	 */
	m_cfg->InternalGfx = !CONFIG(SOC_INTEL_DISABLE_IGD) && is_devfn_enabled(SA_DEVFN_IGD);
	m_cfg->IgdDvmt50PreAlloc = m_cfg->InternalGfx ? 0xFE : 0;

	m_cfg->TsegSize = CONFIG_SMM_TSEG_SIZE;
	m_cfg->SaGv = config->SaGv;
	m_cfg->RMT = config->RMT;

	/* PCIe ModPhy configuration */
	for (i = 0; i < CONFIG_MAX_ROOT_PORTS; i++) {
		if (config->pcie_mp_cfg[i].tx_gen1_downscale_amp_override) {
			m_cfg->PchPcieHsioTxGen1DownscaleAmpEnable[i] = 1;
			m_cfg->PchPcieHsioTxGen1DownscaleAmp[i] =
				config->pcie_mp_cfg[i].tx_gen1_downscale_amp;
		}
		if (config->pcie_mp_cfg[i].tx_gen2_downscale_amp_override) {
			m_cfg->PchPcieHsioTxGen2DownscaleAmpEnable[i] = 1;
			m_cfg->PchPcieHsioTxGen2DownscaleAmp[i] =
				config->pcie_mp_cfg[i].tx_gen2_downscale_amp;
		}
		if (config->pcie_mp_cfg[i].tx_gen3_downscale_amp_override) {
			m_cfg->PchPcieHsioTxGen3DownscaleAmpEnable[i] = 1;
			m_cfg->PchPcieHsioTxGen3DownscaleAmp[i] =
				config->pcie_mp_cfg[i].tx_gen3_downscale_amp;
		}
		if (config->pcie_mp_cfg[i].tx_gen1_de_emph) {
			m_cfg->PchPcieHsioTxGen1DeEmphEnable[i] = 1;
			m_cfg->PchPcieHsioTxGen1DeEmph[i] =
				config->pcie_mp_cfg[i].tx_gen1_de_emph;
		}
		if (config->pcie_mp_cfg[i].tx_gen2_de_emph_3p5) {
			m_cfg->PchPcieHsioTxGen2DeEmph3p5Enable[i] = 1;
			m_cfg->PchPcieHsioTxGen2DeEmph3p5[i] =
				config->pcie_mp_cfg[i].tx_gen2_de_emph_3p5;
		}
		if (config->pcie_mp_cfg[i].tx_gen2_de_emph_6p0) {
			m_cfg->PchPcieHsioTxGen2DeEmph6p0Enable[i] = 1;
			m_cfg->PchPcieHsioTxGen2DeEmph6p0[i] =
				config->pcie_mp_cfg[i].tx_gen2_de_emph_6p0;
		}
	}

	m_cfg->PcieRpEnableMask = pcie_rp_enable_mask(pch_rp_groups);

	FSP_ARRAY_LOAD(m_cfg->PcieClkSrcUsage, config->PcieClkSrcUsage);
	FSP_ARRAY_LOAD(m_cfg->PcieClkSrcClkReq, config->PcieClkSrcClkReq);

	m_cfg->PrmrrSize = get_valid_prmrr_size();

	/* Disable BIOS Guard */
	m_cfg->BiosGuard = 0;

	/* Set CPU Ratio */
	m_cfg->CpuRatio = 0;

	/* Set debug interface flags */
	m_cfg->PcdDebugInterfaceFlags = CONFIG(DRIVERS_UART_8250IO) ?
		DEBUG_INTERFACE_UART_8250IO : DEBUG_INTERFACE_LPSS_SERIAL_IO;

	/* TraceHub configuration */
	if (is_devfn_enabled(PCH_DEVFN_TRACEHUB) && config->TraceHubMode) {
		m_cfg->PcdDebugInterfaceFlags |= DEBUG_INTERFACE_TRACEHUB;
		m_cfg->PchTraceHubMode = config->TraceHubMode;
		m_cfg->CpuTraceHubMode = config->TraceHubMode;
	}

	/* IPU configuration */
	m_cfg->SaIpuEnable = is_devfn_enabled(SA_DEVFN_IPU);

	/* Change VmxEnable UPD value according to ENABLE_VMX Kconfig */
	m_cfg->VmxEnable = CONFIG(ENABLE_VMX);

	/* Enable SMBus controller based on config */
	m_cfg->SmbusEnable = config->SmbusEnable;

	/* Set debug probe type */
	m_cfg->PlatformDebugConsent = CONFIG_SOC_INTEL_COMMON_DEBUG_CONSENT;

	/* VT-d config */
	m_cfg->VtdDisable = 0;
	m_cfg->VtdIopEnable = 0x1;

	if (m_cfg->InternalGfx) {
		m_cfg->VtdIgdEnable = 0x1;
		m_cfg->VtdBaseAddress[0] = GFXVT_BASE_ADDRESS;
	}

	if (m_cfg->SaIpuEnable) {
		m_cfg->VtdIpuEnable = 0x1;
		m_cfg->VtdBaseAddress[1] = IPUVT_BASE_ADDRESS;
	}
	m_cfg->VtdBaseAddress[2] = VTVC0_BASE_ADDRESS;

	m_cfg->SerialIoUartDebugControllerNumber = CONFIG_UART_FOR_CONSOLE;
	m_cfg->SerialIoUartDebugMode = config->SerialIoUartMode[CONFIG_UART_FOR_CONSOLE];

	/* Display */
	m_cfg->DdiPortAConfig = config->DdiPortAConfig;
	m_cfg->DdiPortBHpd = config->DdiPortBHpd;
	m_cfg->DdiPortCHpd = config->DdiPortCHpd;
	m_cfg->DdiPortBDdc = config->DdiPortBDdc;
	m_cfg->DdiPortCDdc = config->DdiPortCDdc;

	/* Audio */
	m_cfg->PchHdaEnable = is_devfn_enabled(PCH_DEVFN_HDA);

	m_cfg->PchHdaDspEnable = config->PchHdaDspEnable;
	m_cfg->PchHdaAudioLinkHdaEnable = config->PchHdaAudioLinkHdaEnable;

	FSP_ARRAY_LOAD(m_cfg->PchHdaAudioLinkDmicEnable, config->PchHdaAudioLinkDmicEnable);
	FSP_ARRAY_LOAD(m_cfg->PchHdaAudioLinkSspEnable, config->PchHdaAudioLinkSspEnable);
	FSP_ARRAY_LOAD(m_cfg->PchHdaAudioLinkSndwEnable, config->PchHdaAudioLinkSndwEnable);

	/* Skip the CPU replacement check */
	m_cfg->SkipCpuReplacementCheck = config->SkipCpuReplacementCheck;

	/*
	 * Set GpioOverride
	 * When GpioOverride is set FSP will not configure any GPIOs
	 * and rely on GPIO settings programmed before moved to FSP.
	 */
	m_cfg->GpioOverride = 1;

	/* crashLog config */
	if (CONFIG(SOC_INTEL_CRASHLOG)) {
		m_cfg->CpuCrashLogDevice = 1;
		m_cfg->CpuCrashLogEnable = 1;
	}
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const struct soc_intel_jasperlake_config *config = config_of_soc();
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	soc_memory_init_params(m_cfg, config);

	mainboard_memory_init_params(mupd);
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
