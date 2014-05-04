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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "agesawrapper.h"
#include "amdlib.h"
#include "BiosCallOuts.h"
#include "Ids.h"
#include "OptionsIds.h"
#include "heapManager.h"
#include "SB700.h"
#include <northbridge/amd/agesa/family15/dimmSpd.h>
#include "OEM.h"		/* SMBUS0_BASE_ADDRESS */

#ifndef SB_GPIO_REG01
#define SB_GPIO_REG01   1
#endif

#ifndef SB_GPIO_REG24
#define SB_GPIO_REG24   24
#endif

#ifndef SB_GPIO_REG27
#define SB_GPIO_REG27   27
#endif

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

STATIC BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_ALLOCATE_BUFFER,
		BiosAllocateBuffer
	},

	{AGESA_DEALLOCATE_BUFFER,
		BiosDeallocateBuffer
	},

	{AGESA_DO_RESET,
		BiosReset
	},

	{AGESA_LOCATE_BUFFER,
		BiosLocateBuffer
	},

	{AGESA_READ_SPD,
		BiosReadSpd
	},

	{AGESA_READ_SPD_RECOVERY,
		BiosDefaultRet
	},

	{AGESA_RUNFUNC_ONAP,
		BiosRunFuncOnAp
	},

	{AGESA_GNB_PCIE_SLOT_RESET,
		BiosGnbPcieSlotReset
	},

	{AGESA_GET_IDS_INIT_DATA,
		BiosGetIdsInitData
	},

	{AGESA_HOOKBEFORE_DRAM_INIT,
		BiosHookBeforeDramInit
	},

	{AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY,
		BiosHookBeforeDramInitRecovery
	},

	{AGESA_HOOKBEFORE_DQS_TRAINING,
		BiosHookBeforeDQSTraining
	},

	{AGESA_HOOKBEFORE_EXIT_SELF_REF,
		BiosHookBeforeExitSelfRefresh
	},
};

AGESA_STATUS GetBiosCallout (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	UINTN i;
	AGESA_STATUS CalloutStatus;
	UINTN CallOutCount = sizeof (BiosCallouts) / sizeof (BiosCallouts [0]);

	for (i = 0; i < CallOutCount; i++)
	{
		if (BiosCallouts[i].CalloutName == Func)
		{
			break;
		}
	}

	if(i >= CallOutCount)
	{
		return AGESA_UNSUPPORTED;
	}

	CalloutStatus = BiosCallouts[i].CalloutPtr (Func, Data, ConfigPtr);

	return CalloutStatus;
}


CONST IDS_NV_ITEM IdsData[] =
{
	/*{
	  AGESA_IDS_NV_MAIN_PLL_CON,
	  0x1
	  },
	  {
	  AGESA_IDS_NV_MAIN_PLL_FID_EN,
	  0x1
	  },
	  {
	  AGESA_IDS_NV_MAIN_PLL_FID,
	  0x8
	  },

	  {
	  AGESA_IDS_NV_CUSTOM_NB_PSTATE,
	  },
	  {
	  AGESA_IDS_NV_CUSTOM_NB_P0_DIV_CTRL,
	  },
	  {
	  AGESA_IDS_NV_CUSTOM_NB_P1_DIV_CTRL,
	  },
	  {
	  AGESA_IDS_NV_FORCE_NB_PSTATE,
	  },
	  */
	{
		0xFFFF,
			0xFFFF
	}
};

#define   NUM_IDS_ENTRIES    (sizeof (IdsData) / sizeof (IDS_NV_ITEM))


AGESA_STATUS BiosGetIdsInitData (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	UINTN   i;
	IDS_NV_ITEM *IdsPtr;

	IdsPtr = ((IDS_CALLOUT_STRUCT *) ConfigPtr)->IdsNvPtr;

	if (Data == IDS_CALLOUT_INIT) {
		for (i = 0; i < NUM_IDS_ENTRIES; i++) {
			IdsPtr[i].IdsNvValue = IdsData[i].IdsNvValue;
			IdsPtr[i].IdsNvId = IdsData[i].IdsNvId;
		}
	}
	return AGESA_SUCCESS;
}


AGESA_STATUS BiosReadSpd (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status;
#ifdef __PRE_RAM__
	if (ConfigPtr == NULL)
		return AGESA_ERROR;

	select_socket(((AGESA_READ_SPD_PARAMS *)ConfigPtr)->SocketId);

	Status = agesa_ReadSPD (Func, Data, ConfigPtr);

	restore_socket();
#else
	Status = AGESA_UNSUPPORTED;
#endif

	return Status;
}

/*  Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeDQSTraining (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	return AGESA_SUCCESS;
}
/*  Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeDramInit (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	AGESA_STATUS      Status;
	UINTN             FcnData;
	MEM_DATA_STRUCT   *MemData;
	UINT32            AcpiMmioAddr;
	UINT32            GpioMmioAddr;
	UINT8             Data8;
	UINT16            Data16;

	FcnData = Data;
	MemData = ConfigPtr;

	Status  = AGESA_SUCCESS;
	/* Get SB MMIO Base (AcpiMmioAddr) */
	WriteIo8 (0xCD6, 0x27);
	Data8   = ReadIo8(0xCD7);
	Data16  = Data8<<8;
	WriteIo8 (0xCD6, 0x26);
	Data8   = ReadIo8(0xCD7);
	Data16  |= Data8;
	AcpiMmioAddr = (UINT32)Data16 << 16;
	GpioMmioAddr = AcpiMmioAddr + GPIO_BASE;
	Status = AGESA_SUCCESS;
	return Status;
}

/*  Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeDramInitRecovery (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	return AGESA_SUCCESS;
}
/*  Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeExitSelfRefresh (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	return AGESA_SUCCESS;
}
/* PCIE slot reset control */
AGESA_STATUS BiosGnbPcieSlotReset (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status;

	Status = AGESA_SUCCESS;
	return  Status;
}
