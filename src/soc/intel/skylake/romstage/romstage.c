/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <stddef.h>
#include <stdint.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/cbfs.h>
#include <arch/stages.h>
#include <arch/early_variables.h>
#include <assert.h>
#include <cbmem.h>
#include <chip.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <elog.h>
#include <reset.h>
#include <romstage_handoff.h>
#include <soc/pci_devs.h>
#include <soc/pei_wrapper.h>
#include <soc/pm.h>
#include <soc/pmc.h>
#include <soc/serialio.h>
#include <soc/romstage.h>
#include <soc/spi.h>
#include <stage_cache.h>
#include <timestamp.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* SOC initialization before RAM is enabled */
void soc_pre_ram_init(struct romstage_params *params)
{
	/* Program MCHBAR and DMIBAR */
	systemagent_early_init();

	/* Prepare to initialize memory */
	soc_fill_pei_data(params->pei_data);
}

int get_sw_write_protect_state(void)
{
	u8 status;

	/* Return unprotected status if status read fails. */
	return early_spi_read_wpsr(&status) ? 0 : !!(status & 0x80);
}

/* UPD parameters to be initialized before MemoryInit */
void soc_memory_init_params(struct romstage_params *params,
			    MEMORY_INIT_UPD *upd)
{
	const struct device *dev;
	const struct soc_intel_skylake_config *config;

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
	upd->IgdDvmt50PreAlloc = 2;

	upd->MmioSize = 0x800; /* 2GB in MB */
	upd->TsegSize = CONFIG_SMM_TSEG_SIZE;
	upd->IedSize = CONFIG_IED_REGION_SIZE;
	upd->ProbelessTrace = config->ProbelessTrace;
	upd->EnableTraceHub = config->EnableTraceHub;
	if (vboot_recovery_mode_enabled())
		upd->SaGv = 0; /* Disable SaGv in recovery mode. */
	else
		upd->SaGv = config->SaGv;
	upd->RMT = config->Rmt;
	upd->DdrFreqLimit = config->DdrFreqLimit;
	if (IS_ENABLED(CONFIG_SKIP_FSP_CAR)) {
		upd->FspCarBase = CONFIG_DCACHE_RAM_BASE;
		upd->FspCarSize = CONFIG_DCACHE_RAM_SIZE;
	}
}

void soc_update_memory_params_for_mma(MEMORY_INIT_UPD *memory_cfg,
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

void soc_display_memory_init_params(const MEMORY_INIT_UPD *old,
	MEMORY_INIT_UPD *new)
{
	/* Display the parameters for MemoryInit */
	printk(BIOS_SPEW, "UPD values for MemoryInit:\n");

	fsp_display_upd_value("PlatformMemorySize", 8,
		old->PlatformMemorySize, new->PlatformMemorySize);
	fsp_display_upd_value("MemorySpdPtr00", 4, old->MemorySpdPtr00,
		new->MemorySpdPtr00);
	fsp_display_upd_value("MemorySpdPtr01", 4, old->MemorySpdPtr01,
		new->MemorySpdPtr01);
	fsp_display_upd_value("MemorySpdPtr10", 4, old->MemorySpdPtr10,
		new->MemorySpdPtr10);
	fsp_display_upd_value("MemorySpdPtr11", 4, old->MemorySpdPtr11,
		new->MemorySpdPtr11);
	fsp_display_upd_value("MemorySpdDataLen", 2, old->MemorySpdDataLen,
		new->MemorySpdDataLen);
	fsp_display_upd_value("DqByteMapCh0[0]", 1, old->DqByteMapCh0[0],
		new->DqByteMapCh0[0]);
	fsp_display_upd_value("DqByteMapCh0[1]", 1, old->DqByteMapCh0[1],
		new->DqByteMapCh0[1]);
	fsp_display_upd_value("DqByteMapCh0[2]", 1, old->DqByteMapCh0[2],
		new->DqByteMapCh0[2]);
	fsp_display_upd_value("DqByteMapCh0[3]", 1, old->DqByteMapCh0[3],
		new->DqByteMapCh0[3]);
	fsp_display_upd_value("DqByteMapCh0[4]", 1, old->DqByteMapCh0[4],
		new->DqByteMapCh0[4]);
	fsp_display_upd_value("DqByteMapCh0[5]", 1, old->DqByteMapCh0[5],
		new->DqByteMapCh0[5]);
	fsp_display_upd_value("DqByteMapCh0[6]", 1, old->DqByteMapCh0[6],
		new->DqByteMapCh0[6]);
	fsp_display_upd_value("DqByteMapCh0[7]", 1, old->DqByteMapCh0[7],
		new->DqByteMapCh0[7]);
	fsp_display_upd_value("DqByteMapCh0[8]", 1, old->DqByteMapCh0[8],
		new->DqByteMapCh0[8]);
	fsp_display_upd_value("DqByteMapCh0[9]", 1, old->DqByteMapCh0[9],
		new->DqByteMapCh0[9]);
	fsp_display_upd_value("DqByteMapCh0[10]", 1, old->DqByteMapCh0[10],
		new->DqByteMapCh0[10]);
	fsp_display_upd_value("DqByteMapCh0[11]", 1, old->DqByteMapCh0[11],
		new->DqByteMapCh0[11]);
	fsp_display_upd_value("DqByteMapCh1[0]", 1, old->DqByteMapCh1[0],
		new->DqByteMapCh1[0]);
	fsp_display_upd_value("DqByteMapCh1[1]", 1, old->DqByteMapCh1[1],
		new->DqByteMapCh1[1]);
	fsp_display_upd_value("DqByteMapCh1[2]", 1, old->DqByteMapCh1[2],
		new->DqByteMapCh1[2]);
	fsp_display_upd_value("DqByteMapCh1[3]", 1, old->DqByteMapCh1[3],
		new->DqByteMapCh1[3]);
	fsp_display_upd_value("DqByteMapCh1[4]", 1, old->DqByteMapCh1[4],
		new->DqByteMapCh1[4]);
	fsp_display_upd_value("DqByteMapCh1[5]", 1, old->DqByteMapCh1[5],
		new->DqByteMapCh1[5]);
	fsp_display_upd_value("DqByteMapCh1[6]", 1, old->DqByteMapCh1[6],
		new->DqByteMapCh1[6]);
	fsp_display_upd_value("DqByteMapCh1[7]", 1, old->DqByteMapCh1[7],
		new->DqByteMapCh1[7]);
	fsp_display_upd_value("DqByteMapCh1[8]", 1, old->DqByteMapCh1[8],
		new->DqByteMapCh1[8]);
	fsp_display_upd_value("DqByteMapCh1[9]", 1, old->DqByteMapCh1[9],
		new->DqByteMapCh1[9]);
	fsp_display_upd_value("DqByteMapCh1[10]", 1, old->DqByteMapCh1[10],
		new->DqByteMapCh1[10]);
	fsp_display_upd_value("DqByteMapCh1[11]", 1, old->DqByteMapCh1[11],
		new->DqByteMapCh1[11]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[0]", 1,
		old->DqsMapCpu2DramCh0[0], new->DqsMapCpu2DramCh0[0]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[1]", 1,
		old->DqsMapCpu2DramCh0[1], new->DqsMapCpu2DramCh0[1]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[2]", 1,
		old->DqsMapCpu2DramCh0[2], new->DqsMapCpu2DramCh0[2]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[3]", 1,
		old->DqsMapCpu2DramCh0[3], new->DqsMapCpu2DramCh0[3]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[4]", 1,
		old->DqsMapCpu2DramCh0[4], new->DqsMapCpu2DramCh0[4]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[5]", 1,
		old->DqsMapCpu2DramCh0[5], new->DqsMapCpu2DramCh0[5]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[6]", 1,
		old->DqsMapCpu2DramCh0[6], new->DqsMapCpu2DramCh0[6]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[7]", 1,
		old->DqsMapCpu2DramCh0[7], new->DqsMapCpu2DramCh0[7]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[0]", 1,
		old->DqsMapCpu2DramCh1[0], new->DqsMapCpu2DramCh1[0]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[1]", 1,
		old->DqsMapCpu2DramCh1[1], new->DqsMapCpu2DramCh1[1]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[2]", 1,
		old->DqsMapCpu2DramCh1[2], new->DqsMapCpu2DramCh1[2]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[3]", 1,
		old->DqsMapCpu2DramCh1[3], new->DqsMapCpu2DramCh1[3]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[4]", 1,
		old->DqsMapCpu2DramCh1[4], new->DqsMapCpu2DramCh1[4]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[5]", 1,
		old->DqsMapCpu2DramCh1[5], new->DqsMapCpu2DramCh1[5]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[6]", 1,
		old->DqsMapCpu2DramCh1[6], new->DqsMapCpu2DramCh1[6]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[7]", 1,
		old->DqsMapCpu2DramCh1[7], new->DqsMapCpu2DramCh1[7]);
	fsp_display_upd_value("DqPinsInterleaved", 1,
		old->DqPinsInterleaved, new->DqPinsInterleaved);
	fsp_display_upd_value("RcompResistor[0]", 2, old->RcompResistor[0],
		new->RcompResistor[0]);
	fsp_display_upd_value("RcompResistor[1]", 2, old->RcompResistor[1],
		new->RcompResistor[1]);
	fsp_display_upd_value("RcompResistor[2]", 2, old->RcompResistor[2],
		new->RcompResistor[2]);
	fsp_display_upd_value("RcompTarget[0]", 1, old->RcompTarget[0],
		new->RcompTarget[0]);
	fsp_display_upd_value("RcompTarget[1]", 1, old->RcompTarget[1],
		new->RcompTarget[1]);
	fsp_display_upd_value("RcompTarget[2]", 1, old->RcompTarget[2],
		new->RcompTarget[2]);
	fsp_display_upd_value("RcompTarget[3]", 1, old->RcompTarget[3],
		new->RcompTarget[3]);
	fsp_display_upd_value("RcompTarget[4]", 1, old->RcompTarget[4],
		new->RcompTarget[4]);
	fsp_display_upd_value("CaVrefConfig", 1, old->CaVrefConfig,
		new->CaVrefConfig);
	fsp_display_upd_value("SmramMask", 1, old->SmramMask, new->SmramMask);
	fsp_display_upd_value("MrcFastBoot", 1, old->MrcFastBoot,
		new->MrcFastBoot);
	fsp_display_upd_value("IedSize", 4, old->IedSize, new->IedSize);
	fsp_display_upd_value("TsegSize", 4, old->TsegSize, new->TsegSize);
	fsp_display_upd_value("MmioSize", 2, old->MmioSize, new->MmioSize);
	fsp_display_upd_value("EnableTraceHub", 1, old->EnableTraceHub,
		new->EnableTraceHub);
	fsp_display_upd_value("IgdDvmt50PreAlloc", 1, old->IgdDvmt50PreAlloc,
		new->IgdDvmt50PreAlloc);
	fsp_display_upd_value("InternalGfx", 1, old->InternalGfx,
		new->InternalGfx);
	fsp_display_upd_value("ApertureSize", 1, old->ApertureSize,
		new->ApertureSize);
	fsp_display_upd_value("SaGv", 1, old->SaGv, new->SaGv);
	fsp_display_upd_value("RMT", 1, old->RMT, new->RMT);
	fsp_display_upd_value("FspCarBase", 1, old->FspCarBase,
		new->FspCarBase);
	fsp_display_upd_value("FspCarSize", 1, old->FspCarSize,
		new->FspCarSize);
}

/* SOC initialization after RAM is enabled. */
void soc_after_ram_init(struct romstage_params *params)
{
	/* Set the DISB as soon as possible after DRAM
	 * init and MRC cache is saved.
	 */
	pmc_set_disb();
}
