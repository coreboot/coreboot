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
	/* Prepare to initialize memory */
	soc_fill_pei_data(params->pei_data);
}

/* SOC initialization before the console is enabled. */
void car_soc_pre_console_init(void)
{
	/* System Agent Early Initialization */
	systemagent_early_init();

	if (IS_ENABLED(CONFIG_UART_DEBUG))
		pch_uart_init();
}

void car_soc_post_console_init(void)
{
	report_platform_info();
	set_max_freq();
	pch_early_init();
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

	memcpy(upd->PcieRpEnable, config->PcieRpEnable,
		sizeof(upd->PcieRpEnable));
	memcpy(upd->PcieRpClkReqSupport, config->PcieRpClkReqSupport,
		sizeof(upd->PcieRpClkReqSupport));
	memcpy(upd->PcieRpClkReqNumber, config->PcieRpClkReqNumber,
		sizeof(upd->PcieRpClkReqNumber));

	upd->MmioSize = 0x800; /* 2GB in MB */
	upd->TsegSize = CONFIG_SMM_TSEG_SIZE;
	upd->IedSize = CONFIG_IED_REGION_SIZE;
	upd->ProbelessTrace = config->ProbelessTrace;
	upd->EnableLan = config->EnableLan;
	upd->EnableTraceHub = config->EnableTraceHub;
	upd->SaGv = config->SaGv;
	upd->RMT = config->Rmt;
	upd->Cio2Enable = config->Cio2Enable;
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
	fsp_display_upd_value("EnableLan", 1, old->EnableLan, new->EnableLan);
	fsp_display_upd_value("EnableTraceHub", 1, old->EnableTraceHub,
		new->EnableTraceHub);
	fsp_display_upd_value("PcieRpEnable[0]", 1, old->PcieRpEnable[0],
		new->PcieRpEnable[0]);
	fsp_display_upd_value("PcieRpEnable[1]", 1, old->PcieRpEnable[1],
		new->PcieRpEnable[1]);
	fsp_display_upd_value("PcieRpEnable[2]", 1, old->PcieRpEnable[2],
		new->PcieRpEnable[2]);
	fsp_display_upd_value("PcieRpEnable[3]", 1, old->PcieRpEnable[3],
		new->PcieRpEnable[3]);
	fsp_display_upd_value("PcieRpEnable[4]", 1, old->PcieRpEnable[4],
		new->PcieRpEnable[4]);
	fsp_display_upd_value("PcieRpEnable[5]", 1, old->PcieRpEnable[5],
		new->PcieRpEnable[5]);
	fsp_display_upd_value("PcieRpEnable[6]", 1, old->PcieRpEnable[6],
		new->PcieRpEnable[6]);
	fsp_display_upd_value("PcieRpEnable[7]", 1, old->PcieRpEnable[7],
		new->PcieRpEnable[7]);
	fsp_display_upd_value("PcieRpEnable[8]", 1, old->PcieRpEnable[8],
		new->PcieRpEnable[8]);
	fsp_display_upd_value("PcieRpEnable[9]", 1, old->PcieRpEnable[9],
		new->PcieRpEnable[9]);
	fsp_display_upd_value("PcieRpEnable[10]", 1, old->PcieRpEnable[10],
		new->PcieRpEnable[10]);
	fsp_display_upd_value("PcieRpEnable[11]", 1, old->PcieRpEnable[11],
		new->PcieRpEnable[11]);
	fsp_display_upd_value("PcieRpEnable[12]", 1, old->PcieRpEnable[12],
		new->PcieRpEnable[12]);
	fsp_display_upd_value("PcieRpEnable[13]", 1, old->PcieRpEnable[13],
		new->PcieRpEnable[13]);
	fsp_display_upd_value("PcieRpEnable[14]", 1, old->PcieRpEnable[14],
		new->PcieRpEnable[14]);
	fsp_display_upd_value("PcieRpEnable[15]", 1, old->PcieRpEnable[15],
		new->PcieRpEnable[15]);
	fsp_display_upd_value("PcieRpEnable[16]", 1, old->PcieRpEnable[16],
		new->PcieRpEnable[16]);
	fsp_display_upd_value("PcieRpEnable[17]", 1, old->PcieRpEnable[17],
		new->PcieRpEnable[17]);
	fsp_display_upd_value("PcieRpEnable[18]", 1, old->PcieRpEnable[18],
		new->PcieRpEnable[18]);
	fsp_display_upd_value("PcieRpEnable[19]", 1, old->PcieRpEnable[19],
		new->PcieRpEnable[19]);
	fsp_display_upd_value("PcieRpClkReqSupport[0]", 1,
		old->PcieRpClkReqSupport[0],
		new->PcieRpClkReqSupport[0]);
	fsp_display_upd_value("PcieRpClkReqSupport[1]", 1,
		old->PcieRpClkReqSupport[1],
		new->PcieRpClkReqSupport[1]);
	fsp_display_upd_value("PcieRpClkReqSupport[2]", 1,
		old->PcieRpClkReqSupport[2],
		new->PcieRpClkReqSupport[2]);
	fsp_display_upd_value("PcieRpClkReqSupport[3]", 1,
		old->PcieRpClkReqSupport[3],
		new->PcieRpClkReqSupport[3]);
	fsp_display_upd_value("PcieRpClkReqSupport[4]", 1,
		old->PcieRpClkReqSupport[4],
		new->PcieRpClkReqSupport[4]);
	fsp_display_upd_value("PcieRpClkReqSupport[5]", 1,
		old->PcieRpClkReqSupport[5],
		new->PcieRpClkReqSupport[5]);
	fsp_display_upd_value("PcieRpClkReqSupport[6]", 1,
		old->PcieRpClkReqSupport[6],
		new->PcieRpClkReqSupport[6]);
	fsp_display_upd_value("PcieRpClkReqSupport[7]", 1,
		old->PcieRpClkReqSupport[7],
		new->PcieRpClkReqSupport[7]);
	fsp_display_upd_value("PcieRpClkReqSupport[8]", 1,
		old->PcieRpClkReqSupport[8],
		new->PcieRpClkReqSupport[8]);
	fsp_display_upd_value("PcieRpClkReqSupport[9]", 1,
		old->PcieRpClkReqSupport[9],
		new->PcieRpClkReqSupport[9]);
	fsp_display_upd_value("PcieRpClkReqSupport[10]", 1,
		old->PcieRpClkReqSupport[10],
		new->PcieRpClkReqSupport[10]);
	fsp_display_upd_value("PcieRpClkReqSupport[11]", 1,
		old->PcieRpClkReqSupport[11],
		new->PcieRpClkReqSupport[11]);
	fsp_display_upd_value("PcieRpClkReqSupport[12]", 1,
		old->PcieRpClkReqSupport[12],
		new->PcieRpClkReqSupport[12]);
	fsp_display_upd_value("PcieRpClkReqSupport[13]", 1,
		old->PcieRpClkReqSupport[13],
		new->PcieRpClkReqSupport[13]);
	fsp_display_upd_value("PcieRpClkReqSupport[14]", 1,
		old->PcieRpClkReqSupport[14],
		new->PcieRpClkReqSupport[14]);
	fsp_display_upd_value("PcieRpClkReqSupport[15]", 1,
		old->PcieRpClkReqSupport[15],
		new->PcieRpClkReqSupport[15]);
	fsp_display_upd_value("PcieRpClkReqSupport[16]", 1,
		old->PcieRpClkReqSupport[16],
		new->PcieRpClkReqSupport[16]);
	fsp_display_upd_value("PcieRpClkReqSupport[17]", 1,
		old->PcieRpClkReqSupport[17],
		new->PcieRpClkReqSupport[17]);
	fsp_display_upd_value("PcieRpClkReqSupport[18]", 1,
		old->PcieRpClkReqSupport[18],
		new->PcieRpClkReqSupport[18]);
	fsp_display_upd_value("PcieRpClkReqSupport[19]", 1,
		old->PcieRpClkReqSupport[19],
		new->PcieRpClkReqSupport[19]);
	fsp_display_upd_value("PcieRpClkReqNumber[0]", 1,
		old->PcieRpClkReqNumber[0],
		new->PcieRpClkReqNumber[0]);
	fsp_display_upd_value("PcieRpClkReqNumber[1]", 1,
		old->PcieRpClkReqNumber[1],
		new->PcieRpClkReqNumber[1]);
	fsp_display_upd_value("PcieRpClkReqNumber[2]", 1,
		old->PcieRpClkReqNumber[2],
		new->PcieRpClkReqNumber[2]);
	fsp_display_upd_value("PcieRpClkReqNumber[3]", 1,
		old->PcieRpClkReqNumber[3],
		new->PcieRpClkReqNumber[3]);
	fsp_display_upd_value("PcieRpClkReqNumber[4]", 1,
		old->PcieRpClkReqNumber[4],
		new->PcieRpClkReqNumber[4]);
	fsp_display_upd_value("PcieRpClkReqNumber[5]", 1,
		old->PcieRpClkReqNumber[5],
		new->PcieRpClkReqNumber[5]);
	fsp_display_upd_value("PcieRpClkReqNumber[6]", 1,
		old->PcieRpClkReqNumber[6],
		new->PcieRpClkReqNumber[6]);
	fsp_display_upd_value("PcieRpClkReqNumber[7]", 1,
		old->PcieRpClkReqNumber[7],
		new->PcieRpClkReqNumber[7]);
	fsp_display_upd_value("PcieRpClkReqNumber[8]", 1,
		old->PcieRpClkReqNumber[8],
		new->PcieRpClkReqNumber[8]);
	fsp_display_upd_value("PcieRpClkReqNumber[9]", 1,
		old->PcieRpClkReqNumber[9],
		new->PcieRpClkReqNumber[9]);
	fsp_display_upd_value("PcieRpClkReqNumber[10]", 1,
		old->PcieRpClkReqNumber[10],
		new->PcieRpClkReqNumber[10]);
	fsp_display_upd_value("PcieRpClkReqNumber[11]", 1,
		old->PcieRpClkReqNumber[11],
		new->PcieRpClkReqNumber[11]);
	fsp_display_upd_value("PcieRpClkReqNumber[12]", 1,
		old->PcieRpClkReqNumber[12],
		new->PcieRpClkReqNumber[12]);
	fsp_display_upd_value("PcieRpClkReqNumber[13]", 1,
		old->PcieRpClkReqNumber[13],
		new->PcieRpClkReqNumber[13]);
	fsp_display_upd_value("PcieRpClkReqNumber[14]", 1,
		old->PcieRpClkReqNumber[14],
		new->PcieRpClkReqNumber[14]);
	fsp_display_upd_value("PcieRpClkReqNumber[15]", 1,
		old->PcieRpClkReqNumber[15],
		new->PcieRpClkReqNumber[15]);
	fsp_display_upd_value("PcieRpClkReqNumber[16]", 1,
		old->PcieRpClkReqNumber[16],
		new->PcieRpClkReqNumber[16]);
	fsp_display_upd_value("PcieRpClkReqNumber[17]", 1,
		old->PcieRpClkReqNumber[17],
		new->PcieRpClkReqNumber[17]);
	fsp_display_upd_value("PcieRpClkReqNumber[18]", 1,
		old->PcieRpClkReqNumber[18],
		new->PcieRpClkReqNumber[18]);
	fsp_display_upd_value("PcieRpClkReqNumber[19]", 1,
		old->PcieRpClkReqNumber[19],
		new->PcieRpClkReqNumber[19]);
	fsp_display_upd_value("IgdDvmt50PreAlloc", 1, old->IgdDvmt50PreAlloc,
		new->IgdDvmt50PreAlloc);
	fsp_display_upd_value("InternalGfx", 1, old->InternalGfx,
		new->InternalGfx);
	fsp_display_upd_value("ApertureSize", 1, old->ApertureSize,
		new->ApertureSize);
	fsp_display_upd_value("SaGv", 1, old->SaGv, new->SaGv);
	fsp_display_upd_value("RMT", 1, old->RMT, new->RMT);
	fsp_display_upd_value("Cio2Enable", 1, old->Cio2Enable, new->Cio2Enable);
}

/* SOC initialization after RAM is enabled. */
void soc_after_ram_init(struct romstage_params *params)
{
	/* Set the DISB as soon as possible after DRAM
	 * init and MRC cache is saved.
	 */
	u32 disb_val = 0;
	device_t dev = PCH_DEV_PMC;
	disb_val = pci_read_config32(dev, GEN_PMCON_A);
	disb_val |= DISB;
	/* Preserve bits which get cleared up if written 1 */
	disb_val &= ~(GBL_RST_STS | MS4V);
	pci_write_config32(dev, GEN_PMCON_A, disb_val);
}
