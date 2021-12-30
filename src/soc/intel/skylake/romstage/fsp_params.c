/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cpu/x86/msr.h>
#include <fsp/util.h>
#include <intelblocks/cpulib.h>
#include <option.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>

static void cpu_flex_override(FSP_M_CONFIG *m_cfg)
{
	msr_t flex_ratio;
	m_cfg->CpuRatioOverride = 1;
	/*
	 * Set cpuratio to that value set in bootblock, This will ensure FSPM
	 * knows the intended flex ratio.
	 */
	flex_ratio = rdmsr(MSR_FLEX_RATIO);
	m_cfg->CpuRatio = (flex_ratio.lo >> 8) & 0xff;
}

static void soc_peg_init_params(FSP_M_CONFIG *m_cfg,
			FSP_M_TEST_CONFIG *m_t_cfg,
			const struct soc_intel_skylake_config *config)
{
	/*
	 * To enable or disable the corresponding PEG root port you need to
	 * add to the devicetree.cb:
	 *
	 *     device pci 01.0 on  end # enable PEG0 root port
	 *     device pci 01.1 off end # do not configure PEG1
	 *
	 * If PEG port is not defined in the device tree, it will be disabled
	 * in FSP
	 */
	m_cfg->Peg0Enable = is_devfn_enabled(SA_DEVFN_PEG0);
	if (m_cfg->Peg0Enable) {
		m_cfg->Peg0Enable = 2;
		m_cfg->Peg0MaxLinkWidth = config->Peg0MaxLinkWidth;
		/* Use maximum possible link speed */
		m_cfg->Peg0MaxLinkSpeed = 0;
		/* Power down unused lanes based on the max possible width */
		m_cfg->Peg0PowerDownUnusedLanes = 1;
		/* Set [Auto] for options to enable equalization methods */
		m_t_cfg->Peg0Gen3EqPh2Enable = 2;
		m_t_cfg->Peg0Gen3EqPh3Method = 0;
	}

	m_cfg->Peg1Enable = is_devfn_enabled(SA_DEVFN_PEG1);
	if (m_cfg->Peg1Enable) {
		m_cfg->Peg1Enable = 2;
		m_cfg->Peg1MaxLinkWidth = config->Peg1MaxLinkWidth;
		m_cfg->Peg1MaxLinkSpeed = 0;
		m_cfg->Peg1PowerDownUnusedLanes = 1;
		m_t_cfg->Peg1Gen3EqPh2Enable = 2;
		m_t_cfg->Peg1Gen3EqPh3Method = 0;
	}

	m_cfg->Peg2Enable = is_devfn_enabled(SA_DEVFN_PEG2);
	if (m_cfg->Peg2Enable) {
		m_cfg->Peg2Enable = 2;
		m_cfg->Peg2MaxLinkWidth = config->Peg2MaxLinkWidth;
		m_cfg->Peg2MaxLinkSpeed = 0;
		m_cfg->Peg2PowerDownUnusedLanes = 1;
		m_t_cfg->Peg2Gen3EqPh2Enable = 2;
		m_t_cfg->Peg2Gen3EqPh3Method = 0;
	}
}

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg,
			const struct soc_intel_skylake_config *config)
{
	int i;
	uint32_t mask = 0;

	m_cfg->MmioSize = 0x800; /* 2GB in MB */
	m_cfg->TsegSize = CONFIG_SMM_TSEG_SIZE;
	m_cfg->IedSize = CONFIG_IED_REGION_SIZE;
	m_cfg->ProbelessTrace = 0;
	m_cfg->SaGv = config->SaGv;
	if (CONFIG(SKYLAKE_SOC_PCH_H))
		m_cfg->UserBd = BOARD_TYPE_DESKTOP;
	else
		m_cfg->UserBd = BOARD_TYPE_ULT_ULX;
	m_cfg->RMT = config->RMT;
	m_cfg->CmdTriStateDis = config->CmdTriStateDis;
	m_cfg->DdrFreqLimit = 0;
	m_cfg->VmxEnable = CONFIG(ENABLE_VMX);
	m_cfg->PrmrrSize = get_valid_prmrr_size();
	for (i = 0; i < ARRAY_SIZE(config->PcieRpEnable); i++) {
		if (config->PcieRpEnable[i])
			mask |= (1<<i);
	}
	m_cfg->PcieRpEnableMask = mask;

	cpu_flex_override(m_cfg);

	/* HPET BDF already handled in coreboot code, so tell FSP to ignore UPDs */
	m_cfg->PchHpetBdfValid = 0;

	m_cfg->HyperThreading = get_uint_option("hyper_threading", CONFIG(FSP_HYPERTHREADING));
}

static void soc_primary_gfx_config_params(FSP_M_CONFIG *m_cfg,
				const struct soc_intel_skylake_config *config)
{
	m_cfg->InternalGfx = !CONFIG(SOC_INTEL_DISABLE_IGD) && is_devfn_enabled(SA_DEVFN_IGD);

	/*
	 * If iGPU is enabled, set IGD stolen size to 64MB. The FBC
	 * hardware for skylake does not have access to the bios
	 * reserved range so it always assumes 8MB is used and so the
	 * kernel will avoid the last 8MB of the stolen window. With
	 * the default stolen size of 32MB(-8MB) there is not enough
	 * space for FBC to work with a high resolution panel.
	 *
	 * If disabled, don't reserve memory for it.
	 */
	m_cfg->IgdDvmt50PreAlloc = m_cfg->InternalGfx ? 2 : 0;

	m_cfg->PrimaryDisplay = config->PrimaryDisplay;
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const struct soc_intel_skylake_config *config;
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	FSP_M_TEST_CONFIG *m_t_cfg = &mupd->FspmTestConfig;

	config = config_of_soc();

	soc_memory_init_params(m_cfg, config);
	soc_peg_init_params(m_cfg, m_t_cfg, config);

	/* Skip creating Management Engine MBP HOB */
	m_t_cfg->SkipMbpHob = 0x01;

	/* Enable DMI Virtual Channel for ME */
	m_t_cfg->DmiVcm = 0x01;

	/* Enable Sending DID to ME */
	m_t_cfg->SendDidMsg = 0x01;
	m_t_cfg->DidInitStat = 0x01;

	/* DCI and TraceHub configs */
	m_t_cfg->PchDciEn = config->PchDciEn;

	m_cfg->EnableTraceHub = is_devfn_enabled(PCH_DEVFN_TRACEHUB);
	m_cfg->TraceHubMemReg0Size = config->TraceHubMemReg0Size;
	m_cfg->TraceHubMemReg1Size = config->TraceHubMemReg1Size;

	/* Enable SMBus controller */
	m_cfg->SmbusEnable = is_devfn_enabled(PCH_DEVFN_SMBUS);

	/* Set primary graphic device */
	soc_primary_gfx_config_params(m_cfg, config);
	m_t_cfg->SkipExtGfxScan = config->SkipExtGfxScan;

	mainboard_memory_init_params(mupd);
}

void soc_update_memory_params_for_mma(FSP_M_CONFIG *memory_cfg,
		struct mma_config_param *mma_cfg)
{
	/* Boot media is memory mapped for Skylake and Kabylake (SPI). */
	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));

	memory_cfg->MmaTestContentPtr = (uintptr_t)mma_cfg->test_content;
	memory_cfg->MmaTestContentSize = mma_cfg->test_content_size;
	memory_cfg->MmaTestConfigPtr = (uintptr_t)mma_cfg->test_param;
	memory_cfg->MmaTestConfigSize = mma_cfg->test_param_size;
	memory_cfg->MrcFastBoot = 0x00;
	memory_cfg->SaGv = 0x02;
}
