/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/cpulib.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_icelake_config *config)
{
	unsigned int i;
	uint32_t mask = 0;

	/*
	 * If IGD is enabled, set IGD stolen size to 60MB.
	 * Otherwise, skip IGD init in FSP.
	 */
	m_cfg->InternalGfx = !CONFIG(SOC_INTEL_DISABLE_IGD) && is_devfn_enabled(SA_DEVFN_IGD);
	m_cfg->IgdDvmt50PreAlloc = m_cfg->InternalGfx ? 0xFE : 0;

	m_cfg->TsegSize = CONFIG_SMM_TSEG_SIZE;
	m_cfg->IedSize = CONFIG_IED_REGION_SIZE;
	m_cfg->SaGv = config->SaGv;
	m_cfg->UserBd = BOARD_TYPE_ULT_ULX;
	m_cfg->RMT = config->RMT;
	m_cfg->SkipMbpHob = 1;

	/* If Audio Codec is enabled, enable FSP UPD */
	m_cfg->PchHdaEnable = is_devfn_enabled(PCH_DEVFN_HDA);

	for (i = 0; i < ARRAY_SIZE(config->PcieRpEnable); i++) {
		if (config->PcieRpEnable[i])
			mask |= (1 << i);
	}
	m_cfg->PcieRpEnableMask = mask;
	m_cfg->PrmrrSize = get_valid_prmrr_size();
	m_cfg->EnableC6Dram = config->enable_c6dram;
	/* Disable BIOS Guard */
	m_cfg->BiosGuard = 0;
	/* Disable Cpu Ratio Override temporary. */
	m_cfg->CpuRatio = 0;
	m_cfg->SerialIoUartDebugControllerNumber = CONFIG_UART_FOR_CONSOLE;
	m_cfg->PcdDebugInterfaceFlags =
		CONFIG(DRIVERS_UART_8250IO) ? 0x02 : 0x10;

	/* Change VmxEnable UPD value according to ENABLE_VMX Kconfig */
	m_cfg->VmxEnable = CONFIG(ENABLE_VMX);
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const struct soc_intel_icelake_config *config;
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	config = config_of_soc();

	soc_memory_init_params(m_cfg, config);

	/* Enable SMBus controller based on config */
	m_cfg->SmbusEnable = config->SmbusEnable;
	/* Set debug probe type */
	m_cfg->PlatformDebugConsent = CONFIG_SOC_INTEL_ICELAKE_DEBUG_CONSENT;

	/* Vt-D config */
	m_cfg->VtdDisable = 0;

	mainboard_memory_init_params(mupd);
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
