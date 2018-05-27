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
#include <compiler.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cbmem.h>
#include <chip.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <fsp/util.h>
#include <fsp/memmap.h>
#include <intelblocks/pmclib.h>
#include <memory_info.h>
#include <smbios.h>
#include <soc/intel/common/smbios.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <string.h>
#include <timestamp.h>
#include <security/vboot/vboot_common.h>

#define FSP_SMBIOS_MEMORY_INFO_GUID	\
{	\
	0xd4, 0x71, 0x20, 0x9b, 0x54, 0xb0, 0x0c, 0x4e,	\
	0x8d, 0x09, 0x11, 0xcf, 0x8b, 0x9f, 0x03, 0x23	\
}

/* Memory Channel Present Status */
enum {
	CHANNEL_NOT_PRESENT,
	CHANNEL_DISABLED,
	CHANNEL_PRESENT
};

/* Save the DIMM information for SMBIOS table 17 */
static void save_dimm_info(void)
{
	int channel, dimm, dimm_max, index;
	size_t hob_size;
	uint8_t ddr_type;
	const CONTROLLER_INFO *ctrlr_info;
	const CHANNEL_INFO *channel_info;
	const DIMM_INFO *src_dimm;
	struct dimm_info *dest_dimm;
	struct memory_info *mem_info;
	const MEMORY_INFO_DATA_HOB *memory_info_hob;
	const uint8_t smbios_memory_info_guid[16] =
			FSP_SMBIOS_MEMORY_INFO_GUID;

	/* Locate the memory info HOB, presence validated by raminit */
	memory_info_hob =
		fsp_find_extension_hob_by_guid(smbios_memory_info_guid,
						&hob_size);
	if (memory_info_hob == NULL || hob_size == 0) {
		printk(BIOS_ERR, "SMBIOS MEMORY_INFO_DATA_HOB not found\n");
		return;
	}

	/*
	 * Allocate CBMEM area for DIMM information used to populate SMBIOS
	 * table 17
	 */
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	if (mem_info == NULL) {
		printk(BIOS_ERR, "CBMEM entry for DIMM info missing\n");
		return;
	}
	memset(mem_info, 0, sizeof(*mem_info));

	/* Describe the first N DIMMs in the system */
	index = 0;
	dimm_max = ARRAY_SIZE(mem_info->dimm);
	ctrlr_info = &memory_info_hob->Controller[0];
	for (channel = 0; channel < MAX_CH && index < dimm_max; channel++) {
		channel_info = &ctrlr_info->ChannelInfo[channel];
		if (channel_info->Status != CHANNEL_PRESENT)
			continue;
		for (dimm = 0; dimm < MAX_DIMM && index < dimm_max; dimm++) {
			src_dimm = &channel_info->DimmInfo[dimm];
			dest_dimm = &mem_info->dimm[index];

			if (src_dimm->Status != DIMM_PRESENT)
				continue;

			switch(memory_info_hob->MemoryType) {
			case MRC_DDR_TYPE_DDR4:
				ddr_type = MEMORY_DEVICE_DDR4;
				break;
			case MRC_DDR_TYPE_DDR3:
				ddr_type = MEMORY_DEVICE_DDR3;
				break;
			case MRC_DDR_TYPE_LPDDR3:
				ddr_type = MEMORY_DEVICE_LPDDR3;
				break;
			default:
				ddr_type = MEMORY_DEVICE_UNKNOWN;
				break;
			}

			/* Populate the DIMM information */
			dimm_info_fill(dest_dimm,
				src_dimm->DimmCapacity,
				ddr_type,
				memory_info_hob->ConfiguredMemoryClockSpeed,
				channel_info->ChannelId,
				src_dimm->DimmId,
				(const char *)src_dimm->ModulePartNum,
				sizeof(src_dimm->ModulePartNum),
				memory_info_hob->DataWidth);
			index++;
		}
	}
	mem_info->dimm_cnt = index;
	printk(BIOS_DEBUG, "%d DIMMs found\n", mem_info->dimm_cnt);
}

asmlinkage void car_stage_entry(void)
{
	bool s3wake;
	struct postcar_frame pcf;
	uintptr_t top_of_ram;
	struct chipset_power_state *ps;

	console_init();

	/* Program MCHBAR, DMIBAR, GDXBAR and EDRAMBAR */
	systemagent_early_init();

	ps = pmc_get_power_state();
	timestamp_add_now(TS_START_ROMSTAGE);
	s3wake = pmc_fill_power_state(ps) == ACPI_S3;
	fsp_memory_init(s3wake);
	pmc_set_disb();
	if (!s3wake)
		save_dimm_info();
	if (postcar_frame_init(&pcf, 1*KiB))
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
	postcar_frame_add_mtrr(&pcf, CACHE_ROM_BASE, CACHE_ROM_SIZE,
			       MTRR_TYPE_WRPROT);

	run_postcar_phase(&pcf);
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

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg,
			const struct soc_intel_skylake_config *config)
{
	int i;
	uint32_t mask = 0;

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
	m_cfg->SaGv = config->SaGv;
	m_cfg->UserBd = BOARD_TYPE_ULT_ULX;
	m_cfg->RMT = config->Rmt;
	m_cfg->DdrFreqLimit = config->DdrFreqLimit;
	m_cfg->VmxEnable = config->VmxEnable;
	m_cfg->PrmrrSize = config->PrmrrSize;
	for (i = 0; i < ARRAY_SIZE(config->PcieRpEnable); i++) {
		if (config->PcieRpEnable[i])
			mask |= (1<<i);
	}
	m_cfg->PcieRpEnableMask = mask;

	cpu_flex_override(m_cfg);

	if (!config->ignore_vtd) {
		m_cfg->PchHpetBdfValid = 1;
		m_cfg->PchHpetBusNumber = 250;
		m_cfg->PchHpetDeviceNumber = 15;
		m_cfg->PchHpetFunctionNumber = 0;
	}
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const struct device *dev;
	const struct soc_intel_skylake_config *config;
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	FSP_M_TEST_CONFIG *m_t_cfg = &mupd->FspmTestConfig;

	dev = dev_find_slot(0, PCI_DEVFN(PCH_DEV_SLOT_LPC, 0));
	config = dev->chip_info;

	soc_memory_init_params(m_cfg, config);

	/* Skip creating Management Engine MBP HOB */
	m_t_cfg->SkipMbpHob = 0x01;

	/* Enable DMI Virtual Channel for ME */
	m_t_cfg->DmiVcm = 0x01;

	/* Enable Sending DID to ME */
	m_t_cfg->SendDidMsg = 0x01;
	m_t_cfg->DidInitStat = 0x01;

	/* DCI and TraceHub configs */
	m_t_cfg->PchDciEn = config->PchDciEn;
	m_cfg->EnableTraceHub = config->EnableTraceHub;
	m_cfg->TraceHubMemReg0Size = config->TraceHubMemReg0Size;
	m_cfg->TraceHubMemReg1Size = config->TraceHubMemReg1Size;

	/* Enable SMBus controller based on config */
	m_cfg->SmbusEnable = config->SmbusEnable;

	mainboard_memory_init_params(mupd);
}

void soc_update_memory_params_for_mma(FSP_M_CONFIG *memory_cfg,
		struct mma_config_param *mma_cfg)
{
	/* Boot media is memory mapped for Skylake and Kabylake (SPI). */
	assert(IS_ENABLED(CONFIG_BOOT_DEVICE_MEMORY_MAPPED));

	memory_cfg->MmaTestContentPtr =
			(uintptr_t) rdev_mmap_full(&mma_cfg->test_content);
	memory_cfg->MmaTestContentSize =
			region_device_sz(&mma_cfg->test_content);
	memory_cfg->MmaTestConfigPtr =
			(uintptr_t) rdev_mmap_full(&mma_cfg->test_param);
	memory_cfg->MmaTestConfigSize =
			region_device_sz(&mma_cfg->test_param);
	memory_cfg->MrcFastBoot = 0x00;
	memory_cfg->SaGv = 0x02;
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Do nothing */
}
