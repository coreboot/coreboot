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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
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
#include <device/pci_def.h>
#include <elog.h>
#include <reset.h>
#include <romstage_handoff.h>
#include <soc/pci_devs.h>
#include <soc/pei_wrapper.h>
#include <soc/pm.h>
#include <soc/serialio.h>
#include <soc/romstage.h>
#include <soc/spi.h>
#include <stage_cache.h>
#include <timestamp.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* SOC initialization before the console is enabled */
void soc_pre_console_init(struct romstage_params *params)
{
	/* System Agent Early Initialization */
	systemagent_early_init();

	if (IS_ENABLED(CONFIG_UART_DEBUG))
		pch_uart_init();
}

/* SOC initialization before RAM is enabled */
void soc_pre_ram_init(struct romstage_params *params)
{
	/* Prepare to initialize memory */
	soc_fill_pei_data(params->pei_data);
}

void soc_romstage_init(struct romstage_params *params)
{
	pch_early_init();
}

#if IS_ENABLED(CONFIG_CHROMEOS)
int vboot_get_sw_write_protect(void)
{
	u8 status;

	/* Return unprotected status if status read fails. */
	return early_spi_read_wpsr(&status) ? 0 : !!(status & 0x80);
}
#endif

/* UPD parameters to be initialized before MemoryInit */
void soc_memory_init_params(MEMORY_INIT_UPD *params)
{
	const struct device *dev;
	const struct soc_intel_skylake_config *config;
	int i;

	/* Set the parameters for MemoryInit */
	dev = dev_find_slot(0, PCI_DEVFN(PCH_DEV_SLOT_LPC, 0));
	config = dev->chip_info;

	for (i = 0; i < PchSerialIoIndexMax; i++)
		params->SerialIoDevMode[i] = config->SerialIoDevMode[i];

	memcpy(params->PcieRpEnable, config->PcieRpEnable,
		sizeof(params->PcieRpEnable));
	memcpy(params->PcieRpClkReqSupport, config->PcieRpClkReqSupport,
		sizeof(params->PcieRpClkReqSupport));
	memcpy(params->PcieRpClkReqNumber, config->PcieRpClkReqNumber,
		sizeof(params->PcieRpClkReqNumber));

	params->MmioSize = 0x800; /* 2GB in MB */
	params->TsegSize = CONFIG_SMM_TSEG_SIZE;
	params->IedSize = config->IedSize;
	params->ProbelessTrace = config->ProbelessTrace;
	params->EnableLan = config->EnableLan;
	params->EnableSata = config->EnableSata;
	params->SataMode = config->SataMode;
	params->SataSalpSupport = config->SataSalpSupport;
	params->SataPortsEnable[0] = config->SataPortsEnable[0];
	params->SsicPortEnable = config->SsicPortEnable;
	params->EnableTraceHub = config->EnableTraceHub;
	params->SmbusEnable = config->SmbusEnable;
	params->Cio2Enable = config->Cio2Enable;
	params->ScsEmmcEnabled = config->ScsEmmcEnabled;
	params->ScsEmmcHs400Enabled = config->ScsEmmcHs400Enabled;
	params->ScsSdCardEnabled = config->ScsSdCardEnabled;
	params->IshEnable = 0;
	params->EnableAzalia = config->EnableAzalia;
	params->IoBufferOwnership = config->IoBufferOwnership;
	params->DspEnable = config->DspEnable;
	params->XdciEnable = config->XdciEnable;

	/* Show SPI controller if enabled in devicetree.cb */
	dev = dev_find_slot(0, PCH_DEVFN_SPI);
	params->ShowSpiController = dev->enabled;
}

void soc_display_memory_init_params(const MEMORY_INIT_UPD *old,
	MEMORY_INIT_UPD *new)
{
	/* Display the parameters for MemoryInit */
	printk(BIOS_SPEW, "UPD values for MemoryInit:\n");

	soc_display_upd_value("PlatformMemorySize", 8,
		old->PlatformMemorySize, new->PlatformMemorySize);
	soc_display_upd_value("MemorySpdPtr00", 4, old->MemorySpdPtr00,
		new->MemorySpdPtr00);
	soc_display_upd_value("MemorySpdPtr01", 4, old->MemorySpdPtr01,
		new->MemorySpdPtr01);
	soc_display_upd_value("MemorySpdPtr10", 4, old->MemorySpdPtr10,
		new->MemorySpdPtr10);
	soc_display_upd_value("MemorySpdPtr11", 4, old->MemorySpdPtr11,
		new->MemorySpdPtr11);
	soc_display_upd_value("MemorySpdDataLen", 2, old->MemorySpdDataLen,
		new->MemorySpdDataLen);
	soc_display_upd_value("DqByteMapCh0[0]", 1, old->DqByteMapCh0[0],
		new->DqByteMapCh0[0]);
	soc_display_upd_value("DqByteMapCh0[1]", 1, old->DqByteMapCh0[1],
		new->DqByteMapCh0[1]);
	soc_display_upd_value("DqByteMapCh0[2]", 1, old->DqByteMapCh0[2],
		new->DqByteMapCh0[2]);
	soc_display_upd_value("DqByteMapCh0[3]", 1, old->DqByteMapCh0[3],
		new->DqByteMapCh0[3]);
	soc_display_upd_value("DqByteMapCh0[4]", 1, old->DqByteMapCh0[4],
		new->DqByteMapCh0[4]);
	soc_display_upd_value("DqByteMapCh0[5]", 1, old->DqByteMapCh0[5],
		new->DqByteMapCh0[5]);
	soc_display_upd_value("DqByteMapCh0[6]", 1, old->DqByteMapCh0[6],
		new->DqByteMapCh0[6]);
	soc_display_upd_value("DqByteMapCh0[7]", 1, old->DqByteMapCh0[7],
		new->DqByteMapCh0[7]);
	soc_display_upd_value("DqByteMapCh0[8]", 1, old->DqByteMapCh0[8],
		new->DqByteMapCh0[8]);
	soc_display_upd_value("DqByteMapCh0[9]", 1, old->DqByteMapCh0[9],
		new->DqByteMapCh0[9]);
	soc_display_upd_value("DqByteMapCh0[10]", 1, old->DqByteMapCh0[10],
		new->DqByteMapCh0[10]);
	soc_display_upd_value("DqByteMapCh0[11]", 1, old->DqByteMapCh0[11],
		new->DqByteMapCh0[11]);
	soc_display_upd_value("DqByteMapCh1[0]", 1, old->DqByteMapCh1[0],
		new->DqByteMapCh1[0]);
	soc_display_upd_value("DqByteMapCh1[1]", 1, old->DqByteMapCh1[1],
		new->DqByteMapCh1[1]);
	soc_display_upd_value("DqByteMapCh1[2]", 1, old->DqByteMapCh1[2],
		new->DqByteMapCh1[2]);
	soc_display_upd_value("DqByteMapCh1[3]", 1, old->DqByteMapCh1[3],
		new->DqByteMapCh1[3]);
	soc_display_upd_value("DqByteMapCh1[4]", 1, old->DqByteMapCh1[4],
		new->DqByteMapCh1[4]);
	soc_display_upd_value("DqByteMapCh1[5]", 1, old->DqByteMapCh1[5],
		new->DqByteMapCh1[5]);
	soc_display_upd_value("DqByteMapCh1[6]", 1, old->DqByteMapCh1[6],
		new->DqByteMapCh1[6]);
	soc_display_upd_value("DqByteMapCh1[7]", 1, old->DqByteMapCh1[7],
		new->DqByteMapCh1[7]);
	soc_display_upd_value("DqByteMapCh1[8]", 1, old->DqByteMapCh1[8],
		new->DqByteMapCh1[8]);
	soc_display_upd_value("DqByteMapCh1[9]", 1, old->DqByteMapCh1[9],
		new->DqByteMapCh1[9]);
	soc_display_upd_value("DqByteMapCh1[10]", 1, old->DqByteMapCh1[10],
		new->DqByteMapCh1[10]);
	soc_display_upd_value("DqByteMapCh1[11]", 1, old->DqByteMapCh1[11],
		new->DqByteMapCh1[11]);
	soc_display_upd_value("DqsMapCpu2DramCh0[0]", 1,
		old->DqsMapCpu2DramCh0[0], new->DqsMapCpu2DramCh0[0]);
	soc_display_upd_value("DqsMapCpu2DramCh0[1]", 1,
		old->DqsMapCpu2DramCh0[1], new->DqsMapCpu2DramCh0[1]);
	soc_display_upd_value("DqsMapCpu2DramCh0[2]", 1,
		old->DqsMapCpu2DramCh0[2], new->DqsMapCpu2DramCh0[2]);
	soc_display_upd_value("DqsMapCpu2DramCh0[3]", 1,
		old->DqsMapCpu2DramCh0[3], new->DqsMapCpu2DramCh0[3]);
	soc_display_upd_value("DqsMapCpu2DramCh0[4]", 1,
		old->DqsMapCpu2DramCh0[4], new->DqsMapCpu2DramCh0[4]);
	soc_display_upd_value("DqsMapCpu2DramCh0[5]", 1,
		old->DqsMapCpu2DramCh0[5], new->DqsMapCpu2DramCh0[5]);
	soc_display_upd_value("DqsMapCpu2DramCh0[6]", 1,
		old->DqsMapCpu2DramCh0[6], new->DqsMapCpu2DramCh0[6]);
	soc_display_upd_value("DqsMapCpu2DramCh0[7]", 1,
		old->DqsMapCpu2DramCh0[7], new->DqsMapCpu2DramCh0[7]);
	soc_display_upd_value("DqsMapCpu2DramCh1[0]", 1,
		old->DqsMapCpu2DramCh1[0], new->DqsMapCpu2DramCh1[0]);
	soc_display_upd_value("DqsMapCpu2DramCh1[1]", 1,
		old->DqsMapCpu2DramCh1[1], new->DqsMapCpu2DramCh1[1]);
	soc_display_upd_value("DqsMapCpu2DramCh1[2]", 1,
		old->DqsMapCpu2DramCh1[2], new->DqsMapCpu2DramCh1[2]);
	soc_display_upd_value("DqsMapCpu2DramCh1[3]", 1,
		old->DqsMapCpu2DramCh1[3], new->DqsMapCpu2DramCh1[3]);
	soc_display_upd_value("DqsMapCpu2DramCh1[4]", 1,
		old->DqsMapCpu2DramCh1[4], new->DqsMapCpu2DramCh1[4]);
	soc_display_upd_value("DqsMapCpu2DramCh1[5]", 1,
		old->DqsMapCpu2DramCh1[5], new->DqsMapCpu2DramCh1[5]);
	soc_display_upd_value("DqsMapCpu2DramCh1[6]", 1,
		old->DqsMapCpu2DramCh1[6], new->DqsMapCpu2DramCh1[6]);
	soc_display_upd_value("DqsMapCpu2DramCh1[7]", 1,
		old->DqsMapCpu2DramCh1[7], new->DqsMapCpu2DramCh1[7]);
	soc_display_upd_value("DqPinsInterleaved", 1,
		old->DqPinsInterleaved, new->DqPinsInterleaved);
	soc_display_upd_value("RcompResistor[0]", 2, old->RcompResistor[0],
		new->RcompResistor[0]);
	soc_display_upd_value("RcompResistor[1]", 2, old->RcompResistor[1],
		new->RcompResistor[1]);
	soc_display_upd_value("RcompResistor[2]", 2, old->RcompResistor[2],
		new->RcompResistor[2]);
	soc_display_upd_value("RcompTarget[0]", 1, old->RcompTarget[0],
		new->RcompTarget[0]);
	soc_display_upd_value("RcompTarget[1]", 1, old->RcompTarget[1],
		new->RcompTarget[1]);
	soc_display_upd_value("RcompTarget[2]", 1, old->RcompTarget[2],
		new->RcompTarget[2]);
	soc_display_upd_value("RcompTarget[3]", 1, old->RcompTarget[3],
		new->RcompTarget[3]);
	soc_display_upd_value("RcompTarget[4]", 1, old->RcompTarget[4],
		new->RcompTarget[4]);
	soc_display_upd_value("CaVrefConfig", 1, old->CaVrefConfig,
		new->CaVrefConfig);
	soc_display_upd_value("SmramMask", 1, old->SmramMask, new->SmramMask);
	soc_display_upd_value("MrcFastBoot", 1, old->MrcFastBoot,
		new->MrcFastBoot);
	soc_display_upd_value("IedSize", 4, old->IedSize, new->IedSize);
	soc_display_upd_value("TsegSize", 4, old->TsegSize, new->TsegSize);
	soc_display_upd_value("MmioSize", 2, old->MmioSize, new->MmioSize);
	soc_display_upd_value("ProbelessTrace", 1, old->ProbelessTrace,
		new->ProbelessTrace);
	soc_display_upd_value("EnableLan", 1, old->EnableLan, new->EnableLan);
	soc_display_upd_value("EnableSata", 1, old->EnableSata,
		new->EnableSata);
	soc_display_upd_value("SataMode", 1, old->SataMode, new->SataMode);
	soc_display_upd_value("SataSalpSupport", 1, old->SataSalpSupport,
		new->SataSalpSupport);
	soc_display_upd_value("SataPortsEnable[0]", 1, old->SataPortsEnable[0],
		new->SataPortsEnable[0]);
	soc_display_upd_value("SataPortsEnable[1]", 1, old->SataPortsEnable[1],
		new->SataPortsEnable[1]);
	soc_display_upd_value("SataPortsEnable[2]", 1, old->SataPortsEnable[2],
		new->SataPortsEnable[2]);
	soc_display_upd_value("SataPortsEnable[3]", 1, old->SataPortsEnable[3],
		new->SataPortsEnable[3]);
	soc_display_upd_value("SataPortsEnable[4]", 1, old->SataPortsEnable[4],
		new->SataPortsEnable[4]);
	soc_display_upd_value("SataPortsEnable[5]", 1, old->SataPortsEnable[5],
		new->SataPortsEnable[5]);
	soc_display_upd_value("SataPortsEnable[6]", 1, old->SataPortsEnable[6],
		new->SataPortsEnable[6]);
	soc_display_upd_value("SataPortsEnable[7]", 1, old->SataPortsEnable[7],
		new->SataPortsEnable[7]);
	soc_display_upd_value("SataPortsDevSlp[0]", 1, old->SataPortsDevSlp[0],
		new->SataPortsDevSlp[0]);
	soc_display_upd_value("SataPortsDevSlp[1]", 1, old->SataPortsDevSlp[1],
		new->SataPortsDevSlp[1]);
	soc_display_upd_value("SataPortsDevSlp[2]", 1, old->SataPortsDevSlp[2],
		new->SataPortsDevSlp[2]);
	soc_display_upd_value("SataPortsDevSlp[3]", 1, old->SataPortsDevSlp[3],
		new->SataPortsDevSlp[3]);
	soc_display_upd_value("SataPortsDevSlp[4]", 1, old->SataPortsDevSlp[4],
		new->SataPortsDevSlp[4]);
	soc_display_upd_value("SataPortsDevSlp[5]", 1, old->SataPortsDevSlp[5],
		new->SataPortsDevSlp[5]);
	soc_display_upd_value("SataPortsDevSlp[6]", 1, old->SataPortsDevSlp[6],
		new->SataPortsDevSlp[6]);
	soc_display_upd_value("SataPortsDevSlp[7]", 1, old->SataPortsDevSlp[7],
		new->SataPortsDevSlp[7]);
	soc_display_upd_value("EnableAzalia", 1, old->EnableAzalia,
		new->EnableAzalia);
	soc_display_upd_value("DspEnable", 1, old->DspEnable, new->DspEnable);
	soc_display_upd_value("IoBufferOwnership", 1, old->IoBufferOwnership,
		new->IoBufferOwnership);
	soc_display_upd_value("EnableTraceHub", 1, old->EnableTraceHub,
		new->EnableTraceHub);
	soc_display_upd_value("PcieRpEnable[0]", 1, old->PcieRpEnable[0],
		new->PcieRpEnable[0]);
	soc_display_upd_value("PcieRpEnable[1]", 1, old->PcieRpEnable[1],
		new->PcieRpEnable[1]);
	soc_display_upd_value("PcieRpEnable[2]", 1, old->PcieRpEnable[2],
		new->PcieRpEnable[2]);
	soc_display_upd_value("PcieRpEnable[3]", 1, old->PcieRpEnable[3],
		new->PcieRpEnable[3]);
	soc_display_upd_value("PcieRpEnable[4]", 1, old->PcieRpEnable[4],
		new->PcieRpEnable[4]);
	soc_display_upd_value("PcieRpEnable[5]", 1, old->PcieRpEnable[5],
		new->PcieRpEnable[5]);
	soc_display_upd_value("PcieRpEnable[6]", 1, old->PcieRpEnable[6],
		new->PcieRpEnable[6]);
	soc_display_upd_value("PcieRpEnable[7]", 1, old->PcieRpEnable[7],
		new->PcieRpEnable[7]);
	soc_display_upd_value("PcieRpEnable[8]", 1, old->PcieRpEnable[8],
		new->PcieRpEnable[8]);
	soc_display_upd_value("PcieRpEnable[9]", 1, old->PcieRpEnable[9],
		new->PcieRpEnable[9]);
	soc_display_upd_value("PcieRpEnable[10]", 1, old->PcieRpEnable[10],
		new->PcieRpEnable[10]);
	soc_display_upd_value("PcieRpEnable[11]", 1, old->PcieRpEnable[11],
		new->PcieRpEnable[11]);
	soc_display_upd_value("PcieRpEnable[12]", 1, old->PcieRpEnable[12],
		new->PcieRpEnable[12]);
	soc_display_upd_value("PcieRpEnable[13]", 1, old->PcieRpEnable[13],
		new->PcieRpEnable[13]);
	soc_display_upd_value("PcieRpEnable[14]", 1, old->PcieRpEnable[14],
		new->PcieRpEnable[14]);
	soc_display_upd_value("PcieRpEnable[15]", 1, old->PcieRpEnable[15],
		new->PcieRpEnable[15]);
	soc_display_upd_value("PcieRpEnable[16]", 1, old->PcieRpEnable[16],
		new->PcieRpEnable[16]);
	soc_display_upd_value("PcieRpEnable[17]", 1, old->PcieRpEnable[17],
		new->PcieRpEnable[17]);
	soc_display_upd_value("PcieRpEnable[18]", 1, old->PcieRpEnable[18],
		new->PcieRpEnable[18]);
	soc_display_upd_value("PcieRpEnable[19]", 1, old->PcieRpEnable[19],
		new->PcieRpEnable[19]);
	soc_display_upd_value("PcieRpClkReqSupport[0]", 1,
		old->PcieRpClkReqSupport[0],
		new->PcieRpClkReqSupport[0]);
	soc_display_upd_value("PcieRpClkReqSupport[1]", 1,
		old->PcieRpClkReqSupport[1],
		new->PcieRpClkReqSupport[1]);
	soc_display_upd_value("PcieRpClkReqSupport[2]", 1,
		old->PcieRpClkReqSupport[2],
		new->PcieRpClkReqSupport[2]);
	soc_display_upd_value("PcieRpClkReqSupport[3]", 1,
		old->PcieRpClkReqSupport[3],
		new->PcieRpClkReqSupport[3]);
	soc_display_upd_value("PcieRpClkReqSupport[4]", 1,
		old->PcieRpClkReqSupport[4],
		new->PcieRpClkReqSupport[4]);
	soc_display_upd_value("PcieRpClkReqSupport[5]", 1,
		old->PcieRpClkReqSupport[5],
		new->PcieRpClkReqSupport[5]);
	soc_display_upd_value("PcieRpClkReqSupport[6]", 1,
		old->PcieRpClkReqSupport[6],
		new->PcieRpClkReqSupport[6]);
	soc_display_upd_value("PcieRpClkReqSupport[7]", 1,
		old->PcieRpClkReqSupport[7],
		new->PcieRpClkReqSupport[7]);
	soc_display_upd_value("PcieRpClkReqSupport[8]", 1,
		old->PcieRpClkReqSupport[8],
		new->PcieRpClkReqSupport[8]);
	soc_display_upd_value("PcieRpClkReqSupport[9]", 1,
		old->PcieRpClkReqSupport[9],
		new->PcieRpClkReqSupport[9]);
	soc_display_upd_value("PcieRpClkReqSupport[10]", 1,
		old->PcieRpClkReqSupport[10],
		new->PcieRpClkReqSupport[10]);
	soc_display_upd_value("PcieRpClkReqSupport[11]", 1,
		old->PcieRpClkReqSupport[11],
		new->PcieRpClkReqSupport[11]);
	soc_display_upd_value("PcieRpClkReqSupport[12]", 1,
		old->PcieRpClkReqSupport[12],
		new->PcieRpClkReqSupport[12]);
	soc_display_upd_value("PcieRpClkReqSupport[13]", 1,
		old->PcieRpClkReqSupport[13],
		new->PcieRpClkReqSupport[13]);
	soc_display_upd_value("PcieRpClkReqSupport[14]", 1,
		old->PcieRpClkReqSupport[14],
		new->PcieRpClkReqSupport[14]);
	soc_display_upd_value("PcieRpClkReqSupport[15]", 1,
		old->PcieRpClkReqSupport[15],
		new->PcieRpClkReqSupport[15]);
	soc_display_upd_value("PcieRpClkReqSupport[16]", 1,
		old->PcieRpClkReqSupport[16],
		new->PcieRpClkReqSupport[16]);
	soc_display_upd_value("PcieRpClkReqSupport[17]", 1,
		old->PcieRpClkReqSupport[17],
		new->PcieRpClkReqSupport[17]);
	soc_display_upd_value("PcieRpClkReqSupport[18]", 1,
		old->PcieRpClkReqSupport[18],
		new->PcieRpClkReqSupport[18]);
	soc_display_upd_value("PcieRpClkReqSupport[19]", 1,
		old->PcieRpClkReqSupport[19],
		new->PcieRpClkReqSupport[19]);
	soc_display_upd_value("PcieRpClkReqNumber[0]", 1,
		old->PcieRpClkReqNumber[0],
		new->PcieRpClkReqNumber[0]);
	soc_display_upd_value("PcieRpClkReqNumber[1]", 1,
		old->PcieRpClkReqNumber[1],
		new->PcieRpClkReqNumber[1]);
	soc_display_upd_value("PcieRpClkReqNumber[2]", 1,
		old->PcieRpClkReqNumber[2],
		new->PcieRpClkReqNumber[2]);
	soc_display_upd_value("PcieRpClkReqNumber[3]", 1,
		old->PcieRpClkReqNumber[3],
		new->PcieRpClkReqNumber[3]);
	soc_display_upd_value("PcieRpClkReqNumber[4]", 1,
		old->PcieRpClkReqNumber[4],
		new->PcieRpClkReqNumber[4]);
	soc_display_upd_value("PcieRpClkReqNumber[5]", 1,
		old->PcieRpClkReqNumber[5],
		new->PcieRpClkReqNumber[5]);
	soc_display_upd_value("PcieRpClkReqNumber[6]", 1,
		old->PcieRpClkReqNumber[6],
		new->PcieRpClkReqNumber[6]);
	soc_display_upd_value("PcieRpClkReqNumber[7]", 1,
		old->PcieRpClkReqNumber[7],
		new->PcieRpClkReqNumber[7]);
	soc_display_upd_value("PcieRpClkReqNumber[8]", 1,
		old->PcieRpClkReqNumber[8],
		new->PcieRpClkReqNumber[8]);
	soc_display_upd_value("PcieRpClkReqNumber[9]", 1,
		old->PcieRpClkReqNumber[9],
		new->PcieRpClkReqNumber[9]);
	soc_display_upd_value("PcieRpClkReqNumber[10]", 1,
		old->PcieRpClkReqNumber[10],
		new->PcieRpClkReqNumber[10]);
	soc_display_upd_value("PcieRpClkReqNumber[11]", 1,
		old->PcieRpClkReqNumber[11],
		new->PcieRpClkReqNumber[11]);
	soc_display_upd_value("PcieRpClkReqNumber[12]", 1,
		old->PcieRpClkReqNumber[12],
		new->PcieRpClkReqNumber[12]);
	soc_display_upd_value("PcieRpClkReqNumber[13]", 1,
		old->PcieRpClkReqNumber[13],
		new->PcieRpClkReqNumber[13]);
	soc_display_upd_value("PcieRpClkReqNumber[14]", 1,
		old->PcieRpClkReqNumber[14],
		new->PcieRpClkReqNumber[14]);
	soc_display_upd_value("PcieRpClkReqNumber[15]", 1,
		old->PcieRpClkReqNumber[15],
		new->PcieRpClkReqNumber[15]);
	soc_display_upd_value("PcieRpClkReqNumber[16]", 1,
		old->PcieRpClkReqNumber[16],
		new->PcieRpClkReqNumber[16]);
	soc_display_upd_value("PcieRpClkReqNumber[17]", 1,
		old->PcieRpClkReqNumber[17],
		new->PcieRpClkReqNumber[17]);
	soc_display_upd_value("PcieRpClkReqNumber[18]", 1,
		old->PcieRpClkReqNumber[18],
		new->PcieRpClkReqNumber[18]);
	soc_display_upd_value("PcieRpClkReqNumber[19]", 1,
		old->PcieRpClkReqNumber[19],
		new->PcieRpClkReqNumber[19]);
	soc_display_upd_value("PortUsb20Enable[0]", 1, old->PortUsb20Enable[0],
		new->PortUsb20Enable[0]);
	soc_display_upd_value("PortUsb20Enable[1]", 1, old->PortUsb20Enable[1],
		new->PortUsb20Enable[1]);
	soc_display_upd_value("PortUsb20Enable[2]", 1, old->PortUsb20Enable[2],
		new->PortUsb20Enable[2]);
	soc_display_upd_value("PortUsb20Enable[3]", 1, old->PortUsb20Enable[3],
		new->PortUsb20Enable[3]);
	soc_display_upd_value("PortUsb20Enable[4]", 1, old->PortUsb20Enable[4],
		new->PortUsb20Enable[4]);
	soc_display_upd_value("PortUsb20Enable[5]", 1, old->PortUsb20Enable[5],
		new->PortUsb20Enable[5]);
	soc_display_upd_value("PortUsb20Enable[6]", 1, old->PortUsb20Enable[6],
		new->PortUsb20Enable[6]);
	soc_display_upd_value("PortUsb20Enable[7]", 1, old->PortUsb20Enable[7],
		new->PortUsb20Enable[7]);
	soc_display_upd_value("PortUsb20Enable[8]", 1, old->PortUsb20Enable[8],
		new->PortUsb20Enable[8]);
	soc_display_upd_value("PortUsb20Enable[9]", 1, old->PortUsb20Enable[9],
		new->PortUsb20Enable[9]);
	soc_display_upd_value("PortUsb20Enable[10]", 1,
		old->PortUsb20Enable[10],
		new->PortUsb20Enable[10]);
	soc_display_upd_value("PortUsb20Enable[11]", 1,
		old->PortUsb20Enable[11],
		new->PortUsb20Enable[11]);
	soc_display_upd_value("PortUsb20Enable[12]", 1,
		old->PortUsb20Enable[12],
		new->PortUsb20Enable[12]);
	soc_display_upd_value("PortUsb20Enable[13]", 1,
		old->PortUsb20Enable[13],
		new->PortUsb20Enable[13]);
	soc_display_upd_value("PortUsb20Enable[14]", 1,
		old->PortUsb20Enable[14],
		new->PortUsb20Enable[14]);
	soc_display_upd_value("PortUsb20Enable[15]", 1,
		old->PortUsb20Enable[15],
		new->PortUsb20Enable[15]);
	soc_display_upd_value("PortUsb30Enable[0]", 1, old->PortUsb30Enable[0],
		new->PortUsb30Enable[0]);
	soc_display_upd_value("PortUsb30Enable[1]", 1, old->PortUsb30Enable[1],
		new->PortUsb30Enable[1]);
	soc_display_upd_value("PortUsb30Enable[2]", 1, old->PortUsb30Enable[2],
		new->PortUsb30Enable[2]);
	soc_display_upd_value("PortUsb30Enable[3]", 1, old->PortUsb30Enable[3],
		new->PortUsb30Enable[3]);
	soc_display_upd_value("PortUsb30Enable[4]", 1, old->PortUsb30Enable[4],
		new->PortUsb30Enable[4]);
	soc_display_upd_value("PortUsb30Enable[5]", 1, old->PortUsb30Enable[5],
		new->PortUsb30Enable[5]);
	soc_display_upd_value("PortUsb30Enable[6]", 1, old->PortUsb30Enable[6],
		new->PortUsb30Enable[6]);
	soc_display_upd_value("PortUsb30Enable[7]", 1, old->PortUsb30Enable[7],
		new->PortUsb30Enable[7]);
	soc_display_upd_value("PortUsb30Enable[8]", 1, old->PortUsb30Enable[8],
		new->PortUsb30Enable[8]);
	soc_display_upd_value("PortUsb30Enable[9]", 1, old->PortUsb30Enable[9],
		new->PortUsb30Enable[9]);
	soc_display_upd_value("XdciEnable", 1, old->XdciEnable,
		new->XdciEnable);
	soc_display_upd_value("SsicPortEnable", 1, old->SsicPortEnable,
		new->SsicPortEnable);
	soc_display_upd_value("SmbusEnable", 1, old->SmbusEnable,
		new->SmbusEnable);
	soc_display_upd_value("SerialIoDevMode[0]", 1, old->SerialIoDevMode[0],
		new->SerialIoDevMode[0]);
	soc_display_upd_value("SerialIoDevMode[1]", 1, old->SerialIoDevMode[1],
		new->SerialIoDevMode[1]);
	soc_display_upd_value("SerialIoDevMode[2]", 1, old->SerialIoDevMode[2],
		new->SerialIoDevMode[2]);
	soc_display_upd_value("SerialIoDevMode[3]", 1, old->SerialIoDevMode[3],
		new->SerialIoDevMode[3]);
	soc_display_upd_value("SerialIoDevMode[4]", 1, old->SerialIoDevMode[4],
		new->SerialIoDevMode[4]);
	soc_display_upd_value("SerialIoDevMode[5]", 1, old->SerialIoDevMode[5],
		new->SerialIoDevMode[5]);
	soc_display_upd_value("SerialIoDevMode[6]", 1, old->SerialIoDevMode[6],
		new->SerialIoDevMode[6]);
	soc_display_upd_value("SerialIoDevMode[7]", 1, old->SerialIoDevMode[7],
		new->SerialIoDevMode[7]);
	soc_display_upd_value("SerialIoDevMode[8]", 1, old->SerialIoDevMode[8],
		new->SerialIoDevMode[8]);
	soc_display_upd_value("SerialIoDevMode[9]", 1, old->SerialIoDevMode[9],
		new->SerialIoDevMode[9]);
	soc_display_upd_value("SerialIoDevMode[10]", 1,
		old->SerialIoDevMode[10],
		new->SerialIoDevMode[10]);
	soc_display_upd_value("Cio2Enable", 1, old->Cio2Enable,
		new->Cio2Enable);
	soc_display_upd_value("ScsEmmcEnabled", 1, old->ScsEmmcEnabled,
		new->ScsEmmcEnabled);
	soc_display_upd_value("ScsEmmcHs400Enabled", 1,
		old->ScsEmmcHs400Enabled,
		new->ScsEmmcHs400Enabled);
	soc_display_upd_value("ScsSdCardEnabled", 1, old->ScsSdCardEnabled,
		new->ScsSdCardEnabled);
	soc_display_upd_value("IshEnable", 1, old->IshEnable, new->IshEnable);
	soc_display_upd_value("ShowSpiController", 1, old->ShowSpiController,
		new->ShowSpiController);
	soc_display_upd_value("PttSwitch", 1, old->PttSwitch, new->PttSwitch);
	soc_display_upd_value("HeciTimeouts", 1, old->HeciTimeouts,
		new->HeciTimeouts);
	soc_display_upd_value("HsioMessaging", 1, old->HsioMessaging,
		new->HsioMessaging);
	soc_display_upd_value("Heci3Enabled", 1, old->Heci3Enabled,
		new->Heci3Enabled);
	soc_display_upd_value("IgdDvmt50PreAlloc", 1, old->IgdDvmt50PreAlloc,
		new->IgdDvmt50PreAlloc);
	soc_display_upd_value("PrimaryDisplay", 1, old->PrimaryDisplay,
		new->PrimaryDisplay);
	soc_display_upd_value("InternalGfx", 1, old->InternalGfx,
		new->InternalGfx);
	soc_display_upd_value("ApertureSize", 1, old->ApertureSize,
		new->ApertureSize);
	soc_display_upd_value("SkipExtGfxScan", 1, old->SkipExtGfxScan,
		new->SkipExtGfxScan);
	soc_display_upd_value("ScanExtGfxForLegacyOpRom", 1,
		old->ScanExtGfxForLegacyOpRom,
		new->ScanExtGfxForLegacyOpRom);
}
