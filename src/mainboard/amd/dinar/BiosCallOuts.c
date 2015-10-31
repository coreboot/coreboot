/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#include "AGESA.h"
#include "amdlib.h"
#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include "Ids.h"
#include "OptionsIds.h"
#include "heapManager.h"
#include "SB700.h"
#include "OEM.h"		/* SMBUS0_BASE_ADDRESS */
#include <stdlib.h>

#include <southbridge/amd/cimx/sb700/smbus_spd.h>

#ifdef __PRE_RAM__
/* This define is used when selecting the appropriate socket for the SPD read
 * because this is a multi-socket design.
 */
#define LTC4305_SMBUS_ADDR    (0x94)

static void select_socket(UINT8 socket_id)
{
	AMD_CONFIG_PARAMS StdHeader;
	UINT32            PciData32;
	UINT8             PciData8;
	PCI_ADDR          PciAddress;

	/* Set SMBus MMIO. */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 20, 0, 0x90);
	PciData32 = (SMBUS0_BASE_ADDRESS & 0xFFFFFFF0) | BIT0;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData32, &StdHeader);

	/* Enable SMBus MMIO. */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 20, 0, 0xD2);
	LibAmdPciRead(AccessWidth8, PciAddress, &PciData8, &StdHeader); ;
	PciData8 |= BIT0;
	LibAmdPciWrite(AccessWidth8, PciAddress, &PciData8, &StdHeader);

	switch (socket_id) {
	case 0:
		/* Switch onto the First CPU Socket SMBus */
		writeSmbusByte(SMBUS0_BASE_ADDRESS, LTC4305_SMBUS_ADDR, 0x80, 0x03);
		break;
	case 1:
		/* Switch onto the Second CPU Socket SMBus */
		writeSmbusByte(SMBUS0_BASE_ADDRESS, LTC4305_SMBUS_ADDR, 0x40, 0x03);
		break;
	default:
		/* Switch off two CPU Sockets SMBus */
		writeSmbusByte(SMBUS0_BASE_ADDRESS, LTC4305_SMBUS_ADDR, 0x00, 0x03);
		break;
	}
}

static void restore_socket(void)
{
	/* Switch off two CPU Sockets SMBus */
	writeSmbusByte(SMBUS0_BASE_ADDRESS, LTC4305_SMBUS_ADDR, 0x00, 0x03);
}
#endif

static AGESA_STATUS board_ReadSpd (UINT32 Func, UINTN Data, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_DO_RESET,			agesa_Reset },
	{AGESA_READ_SPD,			board_ReadSpd },
	{AGESA_READ_SPD_RECOVERY,		agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,			agesa_RunFuncOnAp },
	{AGESA_GNB_PCIE_SLOT_RESET,		agesa_NoopSuccess },
	{AGESA_GET_IDS_INIT_DATA,		agesa_EmptyIdsInitData },
	{AGESA_HOOKBEFORE_DRAM_INIT,		agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY,	agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_DQS_TRAINING,		agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF,	agesa_NoopSuccess },
};
const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);


static AGESA_STATUS board_ReadSpd (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status;
#ifdef __PRE_RAM__
	if (ConfigPtr == NULL)
		return AGESA_ERROR;

	select_socket(((AGESA_READ_SPD_PARAMS *)ConfigPtr)->SocketId);

	Status = agesa_ReadSpd (Func, Data, ConfigPtr);

	restore_socket();
#else
	Status = AGESA_UNSUPPORTED;
#endif

	return Status;
}

static AGESA_STATUS OemInitPost(AMD_POST_PARAMS *InitPost)
{
	InitPost->MemConfig.UmaMode = UMA_AUTO;
	InitPost->MemConfig.BottomIo = 0xE0;
	InitPost->MemConfig.UmaSize = 0xE0-0xC0;
	return AGESA_SUCCESS;
}

const struct OEM_HOOK OemCustomize = {
	.InitPost = OemInitPost,
};
