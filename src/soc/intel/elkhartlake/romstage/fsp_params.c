/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/device.h>
#include <fsp/util.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>
#include <string.h>

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_elkhartlake_config *config)
{
	unsigned int i;
	const struct device *dev;
	uint32_t mask = 0;

	/*
	 * If IGD is enabled, set IGD stolen size to 60MB.
	 * Otherwise, skip IGD init in FSP.
	 */
	dev = pcidev_path_on_root(SA_DEVFN_IGD);
	m_cfg->InternalGfx = !CONFIG(SOC_INTEL_DISABLE_IGD) && is_dev_enabled(dev);
	m_cfg->IgdDvmt50PreAlloc = m_cfg->InternalGfx ? 0xFE : 0;

	m_cfg->TsegSize = CONFIG_SMM_TSEG_SIZE;
	m_cfg->SaGv = config->SaGv;
	m_cfg->RMT = config->RMT;

	/* PCIe root port configuration */
	for (i = 0; i < ARRAY_SIZE(config->PcieRpEnable); i++) {
		if (config->PcieRpEnable[i])
			mask |= (1 << i);
	}

	m_cfg->PcieRpEnableMask = mask;

	_Static_assert(ARRAY_SIZE(m_cfg->PcieClkSrcUsage) >=
			ARRAY_SIZE(config->PcieClkSrcUsage), "copy buffer overflow!");
	memcpy(m_cfg->PcieClkSrcUsage, config->PcieClkSrcUsage,
			sizeof(config->PcieClkSrcUsage));

	_Static_assert(ARRAY_SIZE(m_cfg->PcieClkSrcClkReq) >=
			ARRAY_SIZE(config->PcieClkSrcClkReq), "copy buffer overflow!");
	memcpy(m_cfg->PcieClkSrcClkReq, config->PcieClkSrcClkReq,
			sizeof(config->PcieClkSrcClkReq));

	m_cfg->PrmrrSize = config->PrmrrSize;

	/* Disable BIOS Guard */
	m_cfg->BiosGuard = 0;

	/* Set CPU Ratio */
	m_cfg->CpuRatio = 0;

	/* Set debug interface flags */
	m_cfg->PcdDebugInterfaceFlags = CONFIG(DRIVERS_UART_8250IO) ?
		DEBUG_INTERFACE_UART_8250IO : DEBUG_INTERFACE_LPSS_SERIAL_IO;

	/* TraceHub configuration */
	dev = pcidev_path_on_root(PCH_DEVFN_TRACEHUB);
	if (is_dev_enabled(dev) && config->TraceHubMode) {
		m_cfg->PcdDebugInterfaceFlags |= DEBUG_INTERFACE_TRACEHUB;
		m_cfg->PchTraceHubMode = config->TraceHubMode;
		m_cfg->CpuTraceHubMode = config->TraceHubMode;
	}

	/* Change VmxEnable UPD value according to ENABLE_VMX Kconfig */
	m_cfg->VmxEnable = CONFIG(ENABLE_VMX);

	/* PCH Master Gating Control */
	m_cfg->PchMasterClockGating = 1;
	m_cfg->PchMasterPowerGating = 1;

	/* Enable SMBus controller based on config */
	m_cfg->SmbusEnable = config->SmbusEnable;

	/* Set debug probe type */
	m_cfg->PlatformDebugConsent = CONFIG_SOC_INTEL_ELKHARTLAKE_DEBUG_CONSENT;

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
	m_cfg->SerialIoUartDebugControllerNumber = CONFIG_UART_FOR_CONSOLE;

	/* Audio */
	dev = pcidev_path_on_root(PCH_DEVFN_HDA);
	m_cfg->PchHdaEnable = is_dev_enabled(dev);
	m_cfg->PchHdaDspEnable = config->PchHdaDspEnable;
	m_cfg->PchHdaAudioLinkHdaEnable = config->PchHdaAudioLinkHdaEnable;

	_Static_assert(ARRAY_SIZE(m_cfg->PchHdaSdiEnable) >=
			ARRAY_SIZE(config->PchHdaSdiEnable), "copy buffer overflow!");
	memcpy(m_cfg->PchHdaSdiEnable, config->PchHdaSdiEnable,
		sizeof(config->PchHdaSdiEnable));

	_Static_assert(ARRAY_SIZE(m_cfg->PchHdaAudioLinkDmicEnable) >=
			ARRAY_SIZE(config->PchHdaAudioLinkDmicEnable), "copy buffer overflow!");
	memcpy(m_cfg->PchHdaAudioLinkDmicEnable, config->PchHdaAudioLinkDmicEnable,
		sizeof(config->PchHdaAudioLinkDmicEnable));

	_Static_assert(ARRAY_SIZE(m_cfg->PchHdaAudioLinkSspEnable) >=
			ARRAY_SIZE(config->PchHdaAudioLinkSspEnable), "copy buffer overflow!");
	memcpy(m_cfg->PchHdaAudioLinkSspEnable, config->PchHdaAudioLinkSspEnable,
		sizeof(config->PchHdaAudioLinkSspEnable));

	_Static_assert(ARRAY_SIZE(m_cfg->PchHdaAudioLinkSndwEnable) >=
			ARRAY_SIZE(config->PchHdaAudioLinkSndwEnable), "copy buffer overflow!");
	memcpy(m_cfg->PchHdaAudioLinkSndwEnable, config->PchHdaAudioLinkSndwEnable,
		sizeof(config->PchHdaAudioLinkSndwEnable));

	/* Skip the CPU replacement check */
	m_cfg->SkipCpuReplacementCheck = config->SkipCpuReplacementCheck;

	/* Processor Early Power On Configuration FCLK setting */
	m_cfg->FClkFrequency = 0x1;
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const struct soc_intel_elkhartlake_config *config = config_of_soc();
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	soc_memory_init_params(m_cfg, config);

	mainboard_memory_init_params(mupd);
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
