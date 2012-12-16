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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "agesawrapper.h"
#include "amdlib.h"
#include "dimmSpd.h"
#include "BiosCallOuts.h"
#include "Ids.h"
#include "OptionsIds.h"
#include "heapManager.h"
#include "FchPlatform.h"
#include "cbfs.h"

STATIC CONST BIOS_CALLOUT_STRUCT BiosCallouts[] =
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

	{AGESA_GET_IDS_INIT_DATA,
	 BiosGetIdsInitData
	},

	{AGESA_HOOKBEFORE_DQS_TRAINING,
	 BiosHookBeforeDQSTraining
	},

	{AGESA_HOOKBEFORE_EXIT_SELF_REF,
	 BiosHookBeforeExitSelfRefresh
	},

	{AGESA_FCH_OEM_CALLOUT,
	 Fch_Oem_config
	},
	{AGESA_GNB_GFX_GET_VBIOS_IMAGE,
	 BiosHookGfxGetVbiosImage
	}
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
	BiosHeapBaseAddr = (UINT8 *) GetHeapBase(&(AllocParams->StdHeader));
	BiosHeapBasePtr = (BIOS_HEAP_MANAGER *) BiosHeapBaseAddr;

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

	AllocParams = (AGESA_BUFFER_PARAMS *) ConfigPtr;

	BiosHeapBaseAddr = (UINT8 *) GetHeapBase(&(AllocParams->StdHeader));
	BiosHeapBasePtr = (BIOS_HEAP_MANAGER *) BiosHeapBaseAddr;

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

	BiosHeapBaseAddr = (UINT8 *) GetHeapBase(&(AllocParams->StdHeader));
	BiosHeapBasePtr = (BIOS_HEAP_MANAGER *) BiosHeapBaseAddr;

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

	Status = agesawrapper_amdlaterunaptask (Func, Data, ConfigPtr);
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
	Status = AmdMemoryReadSPD (Func, Data, ConfigPtr);

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
AGESA_STATUS BiosHookBeforeExitSelfRefresh (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	return AGESA_SUCCESS;
}

/**
 * AMD Thatcher Platform ALC272 Verb Table
 */
const CODEC_ENTRY Thatcher_Alc272_VerbTbl[] = {
	{0x11, 0x411111F0},
	{0x12, 0x411111F0},
	{0x13, 0x411111F0},
	{0x14, 0x411111F0},
	{0x15, 0x411111F0},
	{0x16, 0x411111F0},
	{0x17, 0x411111F0},
	{0x18, 0x01a19840},
	{0x19, 0x411111F0},
	{0x1a, 0x01813030},
	{0x1b, 0x411111F0},
	{0x1d, 0x40130605},
	{0x1e, 0x01441120},
	{0x21, 0x01211010},
	{0xff, 0xffffffff}
};

const CODEC_TBL_LIST ThatcherCodecTableList[] =
{
	{0x10ec0272, (CODEC_ENTRY*)&Thatcher_Alc272_VerbTbl[0]},
	{(UINT32)0x0FFFFFFFF, (CODEC_ENTRY*)0x0FFFFFFFFUL}
};

/**
 * Fch Oem setting callback
 *
 *  Configure platform specific Hudson device,
 *   such Azalia, SATA, GEC, IMC etc.
 */
AGESA_STATUS Fch_Oem_config(UINT32 Func, UINT32 FchData, VOID *ConfigPtr)
{
	FCH_RESET_DATA_BLOCK *FchParams = (FCH_RESET_DATA_BLOCK *)FchData;

	if (FchParams->StdHeader->Func == AMD_INIT_RESET) {
		//FCH_RESET_DATA_BLOCK *FchParams_reset =  (FCH_RESET_DATA_BLOCK *) FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT RESET ");
		//FchParams_reset->EcChannel0 = TRUE; /* logical devicd 3 */
	} else if (FchParams->StdHeader->Func == AMD_INIT_ENV) {
		FCH_DATA_BLOCK *FchParams_env = (FCH_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT ENV ");

		/* Azalia Controller OEM Codec Table Pointer */
		FchParams_env->Azalia.AzaliaOemCodecTablePtr = (CODEC_TBL_LIST *)(&ThatcherCodecTableList[0]);
		/* Azalia Controller Front Panel OEM Table Pointer */
		FchParams_env->Imc.ImcEnable = FALSE;
		FchParams_env->Hwm.HwMonitorEnable = FALSE;
		FchParams_env->Hwm.HwmFchtsiAutoPoll = FALSE;/* 1 enable, 0 disable TSI Auto Polling */

		/* XHCI configuration */
		FchParams_env->Usb.Xhci0Enable = TRUE;
		FchParams_env->Usb.Xhci1Enable = TRUE;
	}
	printk(BIOS_DEBUG, "Done\n");

	return AGESA_SUCCESS;
}

AGESA_STATUS BiosHookGfxGetVbiosImage(UINT32 Func, UINT32 FchData, VOID *ConfigPrt)
{
	GFX_VBIOS_IMAGE_INFO  *pVbiosImageInfo = (GFX_VBIOS_IMAGE_INFO *)ConfigPrt;
	pVbiosImageInfo->ImagePtr = cbfs_find_file("pci"CONFIG_VGA_BIOS_ID".rom", CBFS_TYPE_OPTIONROM);
	/* printk(BIOS_DEBUG, "IMGptr=%x\n", pVbiosImageInfo->ImagePtr); */
	return pVbiosImageInfo->ImagePtr == NULL ? AGESA_WARNING : AGESA_SUCCESS;
}
