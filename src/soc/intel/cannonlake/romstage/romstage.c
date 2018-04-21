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

#include <arch/io.h>
#include <arch/early_variables.h>
#include <assert.h>
#include <chip.h>
#include <cpu/x86/mtrr.h>
#include <cbmem.h>
#include <compiler.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/cse.h>
#include <intelblocks/pmclib.h>
#include <memory_info.h>
#include <soc/intel/common/smbios.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <string.h>
#include <timestamp.h>

static struct chipset_power_state power_state CAR_GLOBAL;

#define FSP_SMBIOS_MEMORY_INFO_GUID	\
{	\
	0xd4, 0x71, 0x20, 0x9b, 0x54, 0xb0, 0x0c, 0x4e,	\
	0x8d, 0x09, 0x11, 0xcf, 0x8b, 0x9f, 0x03, 0x23	\
}

/* Save the DIMM information for SMBIOS table 17 */
static void save_dimm_info(void)
{
	int channel, dimm, dimm_max, index;
	size_t hob_size;
	const CONTROLLER_INFO *ctrlr_info;
	const CHANNEL_INFO *channel_info;
	const DIMM_INFO *src_dimm;
	struct dimm_info *dest_dimm;
	struct memory_info *mem_info;
	const MEMORY_INFO_DATA_HOB *memory_info_hob;
	const uint8_t smbios_memory_info_guid[16] =
			FSP_SMBIOS_MEMORY_INFO_GUID;

	/* Locate the memory info HOB, presence validated by raminit */
	memory_info_hob = fsp_find_extension_hob_by_guid(
						smbios_memory_info_guid,
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

			/* Populate the DIMM information */
			dimm_info_fill(dest_dimm,
				src_dimm->DimmCapacity,
				memory_info_hob->MemoryType,
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
	struct chipset_power_state *ps = car_get_var_ptr(&power_state);

	console_init();

	/* Program MCHBAR, DMIBAR, GDXBAR and EDRAMBAR */
	systemagent_early_init();
	/* initialize Heci interface */
	heci_init(HECI1_BASE_ADDRESS);

	timestamp_add_now(TS_START_ROMSTAGE);
	s3wake = pmc_fill_power_state(ps) == ACPI_S3;
	fsp_memory_init(s3wake);
	pmc_set_disb();
	if (!s3wake)
		save_dimm_info();
	if (postcar_frame_init(&pcf, 1 * KiB))
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

	/* Cache the ROM as WP just below 4GiB. */
	postcar_frame_add_mtrr(&pcf, 0xFFFFFFFF - CONFIG_ROM_SIZE + 1,
				CONFIG_ROM_SIZE, MTRR_TYPE_WRPROT);

	run_postcar_phase(&pcf);
}

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg, const config_t *config)
{
	unsigned int i;
	uint32_t mask = 0;

	/* Set IGD stolen size to 64MB. */
	m_cfg->IgdDvmt50PreAlloc = 2;
	m_cfg->TsegSize = CONFIG_SMM_TSEG_SIZE;
	m_cfg->IedSize = CONFIG_IED_REGION_SIZE;
	m_cfg->SaGv = config->SaGv;
	m_cfg->UserBd = BOARD_TYPE_ULT_ULX;
	m_cfg->RMT = config->RMT;

	for (i = 0; i < ARRAY_SIZE(config->PcieRpEnable); i++) {
		if (config->PcieRpEnable[i])
			mask |= (1 << i);
	}
	m_cfg->PcieRpEnableMask = mask;
	m_cfg->PrmrrSize = config->PrmrrSize;
	m_cfg->EnableC6Dram = config->enable_c6dram;
	/* Disable Cpu Ratio Override temporary. */
	m_cfg->CpuRatio = 0;
	m_cfg->PcdSerialIoUartNumber = CONFIG_UART_FOR_CONSOLE;
	/* Disable Vmx if Vt-d is already disabled */
	if (config->VtdDisable)
		m_cfg->VmxEnable = 0;
	else
		m_cfg->VmxEnable = config->VmxEnable;
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const struct device *dev = dev_find_slot(0, PCH_DEVFN_LPC);
	assert(dev != NULL);
	const config_t *config = dev->chip_info;
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	soc_memory_init_params(m_cfg, config);

	/* Enable SMBus controller based on config */
	m_cfg->SmbusEnable = config->SmbusEnable;
	/* Set debug probe type */
	m_cfg->PlatformDebugConsent = config->DebugConsent;

	mainboard_memory_init_params(mupd);
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Do nothing */
}
