/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/cpu.h>
#include <arch/early_variables.h>
#include <arch/io.h>
#include <arch/symbols.h>
#include <assert.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cbmem.h>
#include <chip.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <fsp/util.h>
#include <fsp/memmap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <timestamp.h>
#include <vboot/vboot_common.h>

/*
 * Romstage needs some stack for decompressing ramstage images, since the lzma
 * lib keeps its state on the stack during romstage.
 */
#define ROMSTAGE_RAM_STACK_SIZE 0x5000

asmlinkage void *car_stage_c_entry(void)
{
	bool s3wake;
	struct postcar_frame pcf;
	uintptr_t top_of_ram;
	struct chipset_power_state *ps;

	console_init();

	/* Program MCHBAR, DMIBAR, GDXBAR and EDRAMBAR */
	systemagent_early_init();

	ps = fill_power_state();
	timestamp_add_now(TS_START_ROMSTAGE);
	s3wake = ps->prev_sleep_state == ACPI_S3;
	fsp_memory_init(s3wake);
	pmc_set_disb();

	if (postcar_frame_init(&pcf, ROMSTAGE_RAM_STACK_SIZE))
		die("Unable to initialize postcar frame.\n");

	/*
	 * We need to make sure ramstage will be run cached. At this
	 * point exact location of ramstage in cbmem is not known.
	 * Instruct postcar to cache 16 megs under cbmem top which is
	 * a safe bet to cover ramstage.
	 */
	top_of_ram = (uintptr_t) cbmem_top();
	printk(BIOS_DEBUG, "top_of_ram = 0x%lx\n", top_of_ram);
	top_of_ram -= 16*MiB;
	postcar_frame_add_mtrr(&pcf, top_of_ram, 16*MiB, MTRR_TYPE_WRBACK);

	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)) {
		void *smm_base;
		size_t smm_size;
		uintptr_t tseg_base;

		/*
		 * Cache the TSEG region at the top of ram. This region is
		 * not restricted to SMM mode until SMM has been relocated.
		 * By setting the region to cacheable it provides faster access
		 * when relocating the SMM handler as well as using the TSEG
		 * region for other purposes.
		 */
		smm_region(&smm_base, &smm_size);
		tseg_base = (uintptr_t)smm_base;
		postcar_frame_add_mtrr(&pcf, tseg_base, smm_size,
					MTRR_TYPE_WRBACK);
	}

	/* Cache the ROM as WP just below 4GiB. */
	postcar_frame_add_mtrr(&pcf, 0xFFFFFFFF - CONFIG_ROM_SIZE + 1,
				CONFIG_ROM_SIZE, MTRR_TYPE_WRPROT);

	return postcar_commit_mtrrs(&pcf);
}

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

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg)
{
	const struct device *dev;
	const struct soc_intel_skylake_config *config;
	int i;
	uint32_t mask = 0;

	/* Set the parameters for MemoryInit */
	dev = dev_find_slot(0, PCI_DEVFN(PCH_DEV_SLOT_LPC, 0));
	config = dev->chip_info;
	/*
	 * Set IGD stolen size to 64MB.  The FBC hardware for skylake does not
	 * have access to the bios_reserved range so it always assumes 8MB is
	 * used and so the kernel will avoid the last 8MB of the stolen window.
	 * With the default stolen size of 32MB(-8MB) there is not enough space
	 * for FBC to work with a high resolution panel.
	 */
	m_cfg->IgdDvmt50PreAlloc = 2;
	m_cfg->MmioSize = 0x800; /* 2GB in MB */
	m_cfg->TsegSize = CONFIG_SMM_TSEG_SIZE;
	m_cfg->IedSize = CONFIG_IED_REGION_SIZE;
	m_cfg->ProbelessTrace = config->ProbelessTrace;
	m_cfg->EnableTraceHub = config->EnableTraceHub;
	if (vboot_recovery_mode_enabled())
		m_cfg->SaGv = 0; /* Disable SaGv in recovery mode. */
	else
		m_cfg->SaGv = config->SaGv;
	m_cfg->UserBd = BOARD_TYPE_ULT_ULX;
	m_cfg->RMT = config->Rmt;
	m_cfg->DdrFreqLimit = config->DdrFreqLimit;
	m_cfg->VmxEnable = config->VmxEnable;
	for (i = 0; i < ARRAY_SIZE(config->PcieRpEnable); i++) {
		if (config->PcieRpEnable[i])
			mask |= (1<<i);
	}
	m_cfg->PcieRpEnableMask = mask;

	cpu_flex_override(m_cfg);
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	FSP_M_TEST_CONFIG *m_t_cfg = &mupd->FspmTestConfig;

	soc_memory_init_params(m_cfg);

	/* Enable DMI Virtual Channel for ME */
	m_t_cfg->DmiVcm = 0x01;

	/* Enable Sending DID to ME */
	m_t_cfg->SendDidMsg = 0x01;
	m_t_cfg->DidInitStat = 0x01;

	mainboard_memory_init_params(mupd);
}

__attribute__((weak)) void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Do nothing */
}
