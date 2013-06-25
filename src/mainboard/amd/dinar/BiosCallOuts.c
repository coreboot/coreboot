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


AGESA_STATUS BiosAllocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	UINT32              AvailableHeapSize;
	UINT8               *BiosHeapBaseAddr;
	UINT32              CurrNodeOffset;
	UINT32              PrevNodeOffset;
	UINT32              FreedNodeOffset;
	UINT32              BestFitNodeOffset;
	UINT32              BestFitPrevNodeOffset;
	UINT32              NextFreeOffset;
	BIOS_BUFFER_NODE   *CurrNodePtr;
	BIOS_BUFFER_NODE   *FreedNodePtr;
	BIOS_BUFFER_NODE   *BestFitNodePtr;
	BIOS_BUFFER_NODE   *BestFitPrevNodePtr;
	BIOS_BUFFER_NODE   *NextFreePtr;
	BIOS_HEAP_MANAGER  *BiosHeapBasePtr;
	AGESA_BUFFER_PARAMS *AllocParams;

	AllocParams = ((AGESA_BUFFER_PARAMS *) ConfigPtr);
	AllocParams->BufferPointer = NULL;

	AvailableHeapSize = BIOS_HEAP_SIZE - sizeof (BIOS_HEAP_MANAGER);
	BiosHeapBaseAddr = (UINT8 *) BIOS_HEAP_START_ADDRESS;
	BiosHeapBasePtr = (BIOS_HEAP_MANAGER *) BIOS_HEAP_START_ADDRESS;

	if (BiosHeapBasePtr->StartOfAllocatedNodes == 0) {
		/* First allocation */
		CurrNodeOffset = sizeof (BIOS_HEAP_MANAGER);
		CurrNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + CurrNodeOffset);
		CurrNodePtr->BufferHandle = AllocParams->BufferHandle;
		CurrNodePtr->BufferSize = AllocParams->BufferLength;
		CurrNodePtr->NextNodeOffset = 0;
		AllocParams->BufferPointer = (UINT8 *) CurrNodePtr + sizeof (BIOS_BUFFER_NODE);

		/* Update the remaining free space */
		FreedNodeOffset = CurrNodeOffset + CurrNodePtr->BufferSize + sizeof (BIOS_BUFFER_NODE);
		FreedNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + FreedNodeOffset);
		FreedNodePtr->BufferSize = AvailableHeapSize - sizeof (BIOS_BUFFER_NODE) - CurrNodePtr->BufferSize;
		FreedNodePtr->NextNodeOffset = 0;

		/* Update the offsets for Allocated and Freed nodes */
		BiosHeapBasePtr->StartOfAllocatedNodes = CurrNodeOffset;
		BiosHeapBasePtr->StartOfFreedNodes = FreedNodeOffset;
	} else {
		/* Find out whether BufferHandle has been allocated on the heap. */
		/* If it has, return AGESA_BOUNDS_CHK */
		CurrNodeOffset = BiosHeapBasePtr->StartOfAllocatedNodes;
		CurrNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + CurrNodeOffset);

		while (CurrNodeOffset != 0) {
			CurrNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + CurrNodeOffset);
			if (CurrNodePtr->BufferHandle == AllocParams->BufferHandle) {
				return AGESA_BOUNDS_CHK;
			}
			CurrNodeOffset = CurrNodePtr->NextNodeOffset;
			/* If BufferHandle has not been allocated on the heap, CurrNodePtr here points
			   to the end of the allocated nodes list.
			   */

		}
		/* Find the node that best fits the requested buffer size */
		FreedNodeOffset = BiosHeapBasePtr->StartOfFreedNodes;
		PrevNodeOffset = FreedNodeOffset;
		BestFitNodeOffset = 0;
		BestFitPrevNodeOffset = 0;
		while (FreedNodeOffset != 0) {
			FreedNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + FreedNodeOffset);
			if (FreedNodePtr->BufferSize >= (AllocParams->BufferLength + sizeof (BIOS_BUFFER_NODE))) {
				if (BestFitNodeOffset == 0) {
					/* First node that fits the requested buffer size */
					BestFitNodeOffset = FreedNodeOffset;
					BestFitPrevNodeOffset = PrevNodeOffset;
				} else {
					/* Find out whether current node is a better fit than the previous nodes */
					BestFitNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + BestFitNodeOffset);
					if (BestFitNodePtr->BufferSize > FreedNodePtr->BufferSize) {
						BestFitNodeOffset = FreedNodeOffset;
						BestFitPrevNodeOffset = PrevNodeOffset;
					}
				}
			}
			PrevNodeOffset = FreedNodeOffset;
			FreedNodeOffset = FreedNodePtr->NextNodeOffset;
		} /* end of while loop */


		if (BestFitNodeOffset == 0) {
			/* If we could not find a node that fits the requested buffer */
			/* size, return AGESA_BOUNDS_CHK */
			return AGESA_BOUNDS_CHK;
		} else {
			BestFitNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + BestFitNodeOffset);
			BestFitPrevNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + BestFitPrevNodeOffset);

			/* If BestFitNode is larger than the requested buffer, fragment the node further */
			if (BestFitNodePtr->BufferSize > (AllocParams->BufferLength + sizeof (BIOS_BUFFER_NODE))) {
				NextFreeOffset = BestFitNodeOffset + AllocParams->BufferLength + sizeof (BIOS_BUFFER_NODE);

				NextFreePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + NextFreeOffset);
				NextFreePtr->BufferSize = BestFitNodePtr->BufferSize - (AllocParams->BufferLength + sizeof (BIOS_BUFFER_NODE));
				NextFreePtr->NextNodeOffset = BestFitNodePtr->NextNodeOffset;
			} else {
				/* Otherwise, next free node is NextNodeOffset of BestFitNode */
				NextFreeOffset = BestFitNodePtr->NextNodeOffset;
			}

			/* If BestFitNode is the first buffer in the list, then update
			   StartOfFreedNodes to reflect the new free node
			   */
			if (BestFitNodeOffset == BiosHeapBasePtr->StartOfFreedNodes) {
				BiosHeapBasePtr->StartOfFreedNodes = NextFreeOffset;
			} else {
				BestFitPrevNodePtr->NextNodeOffset = NextFreeOffset;
			}

			/* Add BestFitNode to the list of Allocated nodes */
			CurrNodePtr->NextNodeOffset = BestFitNodeOffset;
			BestFitNodePtr->BufferSize = AllocParams->BufferLength;
			BestFitNodePtr->BufferHandle = AllocParams->BufferHandle;
			BestFitNodePtr->NextNodeOffset = 0;

			/* Remove BestFitNode from list of Freed nodes */
			AllocParams->BufferPointer = (UINT8 *) BestFitNodePtr + sizeof (BIOS_BUFFER_NODE);
		}
	}

	return AGESA_SUCCESS;
}

AGESA_STATUS BiosDeallocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{

	UINT8               *BiosHeapBaseAddr;
	UINT32              AllocNodeOffset;
	UINT32              PrevNodeOffset;
	UINT32              NextNodeOffset;
	UINT32              FreedNodeOffset;
	UINT32              EndNodeOffset;
	BIOS_BUFFER_NODE   *AllocNodePtr;
	BIOS_BUFFER_NODE   *PrevNodePtr;
	BIOS_BUFFER_NODE   *FreedNodePtr;
	BIOS_BUFFER_NODE   *NextNodePtr;
	BIOS_HEAP_MANAGER  *BiosHeapBasePtr;
	AGESA_BUFFER_PARAMS *AllocParams;

	BiosHeapBaseAddr = (UINT8 *) BIOS_HEAP_START_ADDRESS;
	BiosHeapBasePtr = (BIOS_HEAP_MANAGER *) BIOS_HEAP_START_ADDRESS;

	AllocParams = (AGESA_BUFFER_PARAMS *) ConfigPtr;

	/* Find target node to deallocate in list of allocated nodes.
	   Return AGESA_BOUNDS_CHK if the BufferHandle is not found
	   */
	AllocNodeOffset = BiosHeapBasePtr->StartOfAllocatedNodes;
	AllocNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + AllocNodeOffset);
	PrevNodeOffset = AllocNodeOffset;

	while (AllocNodePtr->BufferHandle !=  AllocParams->BufferHandle) {
		if (AllocNodePtr->NextNodeOffset == 0) {
			return AGESA_BOUNDS_CHK;
		}
		PrevNodeOffset = AllocNodeOffset;
		AllocNodeOffset = AllocNodePtr->NextNodeOffset;
		AllocNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + AllocNodeOffset);
	}

	/* Remove target node from list of allocated nodes */
	PrevNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + PrevNodeOffset);
	PrevNodePtr->NextNodeOffset = AllocNodePtr->NextNodeOffset;

	/* Zero out the buffer, and clear the BufferHandle */
	LibAmdMemFill ((UINT8 *)AllocNodePtr + sizeof (BIOS_BUFFER_NODE), 0, AllocNodePtr->BufferSize, &(AllocParams->StdHeader));
	AllocNodePtr->BufferHandle = 0;
	AllocNodePtr->BufferSize += sizeof (BIOS_BUFFER_NODE);

	/* Add deallocated node in order to the list of freed nodes */
	FreedNodeOffset = BiosHeapBasePtr->StartOfFreedNodes;
	FreedNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + FreedNodeOffset);

	EndNodeOffset = AllocNodeOffset + AllocNodePtr->BufferSize;

	if (AllocNodeOffset < FreedNodeOffset) {
		/* Add to the start of the freed list */
		if (EndNodeOffset == FreedNodeOffset) {
			/* If the freed node is adjacent to the first node in the list, concatenate both nodes */
			AllocNodePtr->BufferSize += FreedNodePtr->BufferSize;
			AllocNodePtr->NextNodeOffset = FreedNodePtr->NextNodeOffset;

			/* Clear the BufferSize and NextNodeOffset of the previous first node */
			FreedNodePtr->BufferSize = 0;
			FreedNodePtr->NextNodeOffset = 0;

		} else {
			/* Otherwise, add freed node to the start of the list
			   Update NextNodeOffset and BufferSize to include the
			   size of BIOS_BUFFER_NODE
			   */
			AllocNodePtr->NextNodeOffset = FreedNodeOffset;
		}
		/* Update StartOfFreedNodes to the new first node */
		BiosHeapBasePtr->StartOfFreedNodes = AllocNodeOffset;
	} else {
		/* Traverse list of freed nodes to find where the deallocated node
		   should be place
		   */
		NextNodeOffset = FreedNodeOffset;
		NextNodePtr = FreedNodePtr;
		while (AllocNodeOffset > NextNodeOffset) {
			PrevNodeOffset = NextNodeOffset;
			if (NextNodePtr->NextNodeOffset == 0) {
				break;
			}
			NextNodeOffset = NextNodePtr->NextNodeOffset;
			NextNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + NextNodeOffset);
		}

		/* If deallocated node is adjacent to the next node,
		   concatenate both nodes
		   */
		if (NextNodeOffset == EndNodeOffset) {
			NextNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + NextNodeOffset);
			AllocNodePtr->BufferSize += NextNodePtr->BufferSize;
			AllocNodePtr->NextNodeOffset = NextNodePtr->NextNodeOffset;

			NextNodePtr->BufferSize = 0;
			NextNodePtr->NextNodeOffset = 0;
		} else {
			/*AllocNodePtr->NextNodeOffset = FreedNodePtr->NextNodeOffset; */
			AllocNodePtr->NextNodeOffset = NextNodeOffset;
		}
		/* If deallocated node is adjacent to the previous node,
		   concatenate both nodes
		   */
		PrevNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + PrevNodeOffset);
		EndNodeOffset = PrevNodeOffset + PrevNodePtr->BufferSize;
		if (AllocNodeOffset == EndNodeOffset) {
			PrevNodePtr->NextNodeOffset = AllocNodePtr->NextNodeOffset;
			PrevNodePtr->BufferSize += AllocNodePtr->BufferSize;

			AllocNodePtr->BufferSize = 0;
			AllocNodePtr->NextNodeOffset = 0;
		} else {
			PrevNodePtr->NextNodeOffset = AllocNodeOffset;
		}
	}
	return AGESA_SUCCESS;
}

AGESA_STATUS BiosLocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	UINT32              AllocNodeOffset;
	UINT8               *BiosHeapBaseAddr;
	BIOS_BUFFER_NODE   *AllocNodePtr;
	BIOS_HEAP_MANAGER  *BiosHeapBasePtr;
	AGESA_BUFFER_PARAMS *AllocParams;

	AllocParams = (AGESA_BUFFER_PARAMS *) ConfigPtr;

	BiosHeapBaseAddr = (UINT8 *) BIOS_HEAP_START_ADDRESS;
	BiosHeapBasePtr = (BIOS_HEAP_MANAGER *) BIOS_HEAP_START_ADDRESS;

	AllocNodeOffset = BiosHeapBasePtr->StartOfAllocatedNodes;
	AllocNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + AllocNodeOffset);

	while (AllocParams->BufferHandle != AllocNodePtr->BufferHandle) {
		if (AllocNodePtr->NextNodeOffset == 0) {
			AllocParams->BufferPointer = NULL;
			AllocParams->BufferLength = 0;
			return AGESA_BOUNDS_CHK;
		} else {
			AllocNodeOffset = AllocNodePtr->NextNodeOffset;
			AllocNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr + AllocNodeOffset);
		}
	}

	AllocParams->BufferPointer = (UINT8 *) ((UINT8 *) AllocNodePtr + sizeof (BIOS_BUFFER_NODE));
	AllocParams->BufferLength = AllocNodePtr->BufferSize;

	return AGESA_SUCCESS;

}

AGESA_STATUS BiosRunFuncOnAp (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	AGESA_STATUS        Status;

	Status = agesawrapper_amdlaterunaptask (Data, ConfigPtr);
	return Status;
}

AGESA_STATUS BiosReset (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	AGESA_STATUS        Status;
	UINT8                 Value;
	UINTN               ResetType;
	AMD_CONFIG_PARAMS   *StdHeader;

	ResetType = Data;
	StdHeader = ConfigPtr;

	//
	// Perform the RESET based upon the ResetType. In case of
	// WARM_RESET_WHENVER and COLD_RESET_WHENEVER, the request will go to
	// AmdResetManager. During the critical condition, where reset is required
	// immediately, the reset will be invoked directly by writing 0x04 to port
	// 0xCF9 (Reset Port).
	//
	switch (ResetType) {
		case WARM_RESET_WHENEVER:
		case COLD_RESET_WHENEVER:
			break;

		case WARM_RESET_IMMEDIATELY:
		case COLD_RESET_IMMEDIATELY:
			Value = 0x06;
			LibAmdIoWrite (AccessWidth8, 0xCf9, &Value, StdHeader);
			break;

		default:
			break;
	}

	Status = 0;
	return Status;
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

AGESA_STATUS BiosDefaultRet (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	return AGESA_UNSUPPORTED;
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
