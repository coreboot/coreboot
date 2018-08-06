/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
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
#include <arch/cpu.h>
#include <lib.h>
#include <arch/io.h>
#include <arch/cbfs.h>
#include <arch/stages.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <romstage_handoff.h>
#include <timestamp.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/acpi.h>
#include <soc/baytrail.h>
#include <drivers/intel/fsp1_0/fsp_util.h>

/**
 * /brief mainboard call for setup that needs to be done before fsp init
 *
 */
void early_mainboard_romstage_entry()
{

}

/**
 * Get function disables - most of these will be done automatically
 * @param fd_mask
 * @param fd2_mask
 */
void get_func_disables(uint32_t *fd_mask, uint32_t *fd2_mask)
{

}

/**
 * /brief mainboard call for setup that needs to be done after fsp init
 *
 */
void late_mainboard_romstage_entry()
{

}

const uint32_t mAzaliaVerbTableData13[] = {
/*
 *ALC262 Verb Table - 10EC0262
 */
	/* Pin Complex (NID 0x11 ) */
	0x01171CF0,
	0x01171D11,
	0x01171E11,
	0x01171F41,
	/* Pin Complex (NID 0x12 ) */
	0x01271CF0,
	0x01271D11,
	0x01271E11,
	0x01271F41,
	/* Pin Complex (NID 0x14 ) */
	0x01471C10,
	0x01471D40,
	0x01471E01,
	0x01471F01,
	/* Pin Complex (NID 0x15 ) */
	0x01571CF0,
	0x01571D11,
	0x01571E11,
	0x01571F41,
	/* Pin Complex (NID 0x16 ) */
	0x01671CF0,
	0x01671D11,
	0x01671E11,
	0x01671F41,
	/* Pin Complex (NID 0x18 ) */
	0x01871C20,
	0x01871D98,
	0x01871EA1,
	0x01871F01,
	/* Pin Complex (NID 0x19 ) */
	0x01971C21,
	0x01971D98,
	0x01971EA1,
	0x01971F02,
	/* Pin Complex (NID 0x1A ) */
	0x01A71C2F,
	0x01A71D30,
	0x01A71E81,
	0x01A71F01,
	/* Pin Complex (NID 0x1B ) */
	0x01B71C1F,
	0x01B71D40,
	0x01B71E21,
	0x01B71F02,
	/* Pin Complex (NID 0x1C ) */
	0x01C71CF0,
	0x01C71D11,
	0x01C71E11,
	0x01C71F41,
	/* Pin Complex (NID 0x1D ) */
	0x01D71C01,
	0x01D71DC6,
	0x01D71E14,
	0x01D71F40,
	/* Pin Complex (NID 0x1E ) */
	0x01E71CF0,
	0x01E71D11,
	0x01E71E11,
	0x01E71F41,
	/* Pin Complex (NID 0x1F ) */
	0x01F71CF0,
	0x01F71D11,
	0x01F71E11,
	0x01F71F41 };

const PCH_AZALIA_VERB_TABLE mAzaliaVerbTable[] = { {
/*
 * VerbTable: (RealTek ALC262)
 *  Revision ID = 0xFF, support all steps
 *  Codec Verb Table For AZALIA
 *  Codec Address: CAd value (0/1/2)
 *  Codec Vendor:  0x10EC0262
 */
	{
		0x10EC0262,     /* Vendor ID/Device IDA */
		0x0000,         /* SubSystem ID */
		0xFF,           /* Revision IDA */
		0x01,           /* Front panel support (1=yes, 2=no) */
		0x000B,         /* Number of Rear Jacks = 11 */
		0x0002          /* Number of Front Jacks = 2 */
	},
	(uint32_t *)mAzaliaVerbTableData13 } };

const PCH_AZALIA_CONFIG mainboard_AzaliaConfig = {
	.Pme = 1,
	.DS = 1,
	.DA = 0,
	.HdmiCodec = 1,
	.AzaliaVCi = 1,
	.Rsvdbits = 0,
	.AzaliaVerbTableNum = 1,
	.AzaliaVerbTable = (PCH_AZALIA_VERB_TABLE *)mAzaliaVerbTable,
	.ResetWaitTimer = 300 };

/** /brief customize fsp parameters here if needed
 */
void romstage_fsp_rt_buffer_callback(FSP_INIT_RT_BUFFER *FspRtBuffer)
{
	UPD_DATA_REGION *UpdData = FspRtBuffer->Common.UpdDataRgnPtr;

	/* Initialize the Azalia Verb Tables to mainboard specific version */
	UpdData->AzaliaConfigPtr  = (UINT32)&mainboard_AzaliaConfig;

	/* Disable 2nd DIMM on Bakersport*/
#if IS_ENABLED(CONFIG_BOARD_INTEL_BAKERSPORT_FSP)
	UpdData->PcdMrcInitSPDAddr2 = 0x00; /* cannot use SPD_ADDR_DISABLED at this point */
#endif
}
