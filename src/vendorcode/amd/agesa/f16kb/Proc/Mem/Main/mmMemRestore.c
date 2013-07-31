/* $NoKeywords:$ */
/**
 * @file
 *
 * mmMemRestore.c
 *
 * Main Memory Feature implementation file for Node Interleaving
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 86704 $ @e \$Date: 2013-01-24 17:29:29 -0600 (Thu, 24 Jan 2013) $
 *
 **/
/*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ***************************************************************************
*
*/

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "amdlib.h"
#include "OptionMemory.h"
#include "GeneralServices.h"
#include "mm.h"
#include "mn.h"
#include "Ids.h"
#include "S3.h"
#include "mfs3.h"
#include "heapManager.h"
#include "cpuFeatures.h"
#include "cpuRegisters.h"
#include "cpuPostInit.h"
#include "cpuApicUtilities.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_MAIN_MMMEMRESTORE_FILECODE

#define ST_PRE_ESR  0
#define ST_POST_ESR 1
#define ST_DONE     2

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

VOID
STATIC
MemMCreateS3NbBlock (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr,
     OUT   S3_MEM_NB_BLOCK **S3NBPtr
  );

VOID
MemMContextSave (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

BOOLEAN
MemMContextRestore (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

BOOLEAN
STATIC
MemMDramInit (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

UINT32
GetReducedMemBlockSize (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

VOID
MemMReducedMemBlockSave (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr,
  IN       VOID                *Storage,
     OUT   UINT32              *ActualBufferSize
  );

VOID
MemMReducedMemBlockRestore (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr,
  IN       VOID                *Storage
  );

BOOLEAN
MemMS3Save (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/
extern MEM_NB_SUPPORT memNBInstalled[];
extern MEM_FEAT_BLOCK_MAIN MemFeatMain;

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Determines the maximum amount of space required to store all reduced NB block
 *  and DCT block.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
UINT32
GetReducedMemBlockSize (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8 Node;
  UINT8 Dct;
  UINT32 MemBlockSize;
  MEM_NB_BLOCK  *NBPtr;

  MemBlockSize = sizeof (REDUCED_MEM_BLOCK_HEAP_HEADER);

  for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
    // Increase the size if the node is present
    MemBlockSize += sizeof (REDUCED_NB_BLOCK);
    NBPtr = &MemMainPtr->NBPtr[Node];

    // Sync DCT Select bit with main NB block status
    NBPtr->Dct = 0;
    NBPtr->SetBitField (NBPtr, BFDctCfgSel, 0);
    for (Dct = 0; Dct < NBPtr->DctCount; Dct ++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      // Increase the size if memory is present on the DCT
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        MemBlockSize += sizeof (REDUCED_DCT_BLOCK);
      }
      // Switch back to DCT 0
      NBPtr->SwitchDCT (NBPtr, 0);
    }
  }

  return MemBlockSize;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Save all the required reduced NB blocks and DCT blocks.
 *
 *     @param[in,out]   *MemMainPtr       - Pointer to the MEM_MAIN_DATA_BLOCK
 *     @param[in]       Storage           - Beginning of the context buffer.
 *     @param[out]      ActualBufferSize  - Actual size used in saving the device list.
 *
 */
VOID
MemMReducedMemBlockSave (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr,
  IN       VOID                *Storage,
     OUT   UINT32              *ActualBufferSize
  )
{
  UINT8 Node;
  UINT8 Dct;
  MEM_NB_BLOCK  *NBPtr;
  REDUCED_MEM_BLOCK_HEAP_HEADER *MemBlockHeader;
  REDUCED_NB_BLOCK *ReducedNBPtr;
  REDUCED_DCT_BLOCK *ReducedDCTPtr;

  // Initialzie the block pointers
  MemBlockHeader = (REDUCED_MEM_BLOCK_HEAP_HEADER *)Storage;
  MemBlockHeader->NumNodes = MemMainPtr->DieCount;
  ReducedNBPtr = (REDUCED_NB_BLOCK *)&MemBlockHeader[1];
  ReducedDCTPtr = (REDUCED_DCT_BLOCK *)&ReducedNBPtr[MemBlockHeader->NumNodes];

  // Construct the reduced NB/DCT blocks
  for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
    NBPtr = &MemMainPtr->NBPtr[Node];
    ReducedNBPtr->NodeMemSize = NBPtr->MCTPtr->NodeMemSize;
    ReducedNBPtr->NodeSysBase = NBPtr->MCTPtr->NodeSysBase;
    ReducedNBPtr->NumDcts = 0;

    for (Dct = 0; Dct < NBPtr->DctCount; Dct ++) {
      // Sync DCT Select bit with main NB block status
      NBPtr->SetBitField (NBPtr, BFDctCfgSel, NBPtr->Dct);
      NBPtr->SwitchDCT (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        // Create a reduced block only if memory is present on the DCT
        ReducedNBPtr->NumDcts ++;
        ReducedDCTPtr->Dct = NBPtr->Dct;
        ReducedDCTPtr->DctMemSize = NBPtr->DCTPtr->Timings.DctMemSize;
        ReducedDCTPtr->EnabledChipSels = NBPtr->DCTPtr->EnabledChipSels;
        ReducedDCTPtr->BankAddrMap = NBPtr->DCTPtr->BankAddrMap;
        ReducedDCTPtr->BkIntDis = NBPtr->DCTPtr->BkIntDis;
        ReducedDCTPtr ++;
      }
      // Switch back to DCT 0
      NBPtr->SwitchDCT (NBPtr, 0);
    }
    ReducedNBPtr ++;
  }

  ASSERT ((UINT32) ((UINT8 *)ReducedDCTPtr - (UINT8 *)Storage) == GetReducedMemBlockSize (MemMainPtr));
  *ActualBufferSize += (UINT32) ((UINT8 *)ReducedDCTPtr - (UINT8 *)Storage);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Restore all the required reduced NB blocks and DCT blocks.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *     @param[in]       Storage       - Beginning of the context buffer.
 *
 */
VOID
MemMReducedMemBlockRestore (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr,
  IN       VOID                *Storage
  )
{
  UINT8 Node;
  UINT8 Dct;
  UINT8 NumDcts;
  MEM_NB_BLOCK  *NBPtr;
  DEVICE_BLOCK_HEADER *DeviceList;
  REDUCED_MEM_BLOCK_HEAP_HEADER *MemBlockHeader;
  REDUCED_NB_BLOCK *ReducedNBPtr;
  REDUCED_DCT_BLOCK *ReducedDCTPtr;

  // Initialzie the block pointers
  DeviceList = (DEVICE_BLOCK_HEADER *) Storage;
  MemBlockHeader = (REDUCED_MEM_BLOCK_HEAP_HEADER *) ((UINT8 *) DeviceList + DeviceList->NextBlockOffset);
  ReducedNBPtr = (REDUCED_NB_BLOCK *)&MemBlockHeader[1];
  ReducedDCTPtr = (REDUCED_DCT_BLOCK *)&ReducedNBPtr[MemBlockHeader->NumNodes];

  // Restore the memory data from the reduced NB/DCT blocks
  for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
    NBPtr = &MemMainPtr->NBPtr[Node];
    NBPtr->MCTPtr->NodeMemSize = ReducedNBPtr->NodeMemSize;
    NBPtr->MCTPtr->NodeSysBase = ReducedNBPtr->NodeSysBase;
    NumDcts = ReducedNBPtr->NumDcts;
    ASSERT (NumDcts <= NBPtr->DctCount);

    for (Dct = 0; Dct < NBPtr->DctCount; Dct ++) {
      // Sync DCT Select bit with main NB block status
      NBPtr->SetBitField (NBPtr, BFDctCfgSel, NBPtr->Dct);
      NBPtr->SwitchDCT (NBPtr, Dct);

      if (NumDcts > 0 && NBPtr->Dct == ReducedDCTPtr->Dct) {
        NBPtr->DCTPtr->Timings.DctMemSize = ReducedDCTPtr->DctMemSize;
        NBPtr->DCTPtr->EnabledChipSels = ReducedDCTPtr->EnabledChipSels;
        NBPtr->DCTPtr->BankAddrMap = ReducedDCTPtr->BankAddrMap;
        NBPtr->DCTPtr->BkIntDis = ReducedDCTPtr->BkIntDis;
        ReducedDCTPtr ++;
        NumDcts --;
      } else {
        NBPtr->DCTPtr->Timings.DctMemSize = 0;
        NBPtr->DCTPtr->EnabledChipSels = 0;
        NBPtr->DCTPtr->BankAddrMap = 0;
        NBPtr->DCTPtr->BkIntDis = FALSE;
      }
      // Switch back to DCT 0
      NBPtr->SwitchDCT (NBPtr, 0);
    }
    ReducedNBPtr ++;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Check and save memory context if possible.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
VOID
MemMContextSave (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8 Node;
  UINT8 i;
  MEM_PARAMETER_STRUCT *RefPtr;
  LOCATE_HEAP_PTR LocHeap;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  DEVICE_BLOCK_HEADER *DeviceList;
  AMD_CONFIG_PARAMS *StdHeader;
  UINT32 BufferSize;
  VOID *BufferOffset;
  MEM_NB_BLOCK  *NBArray;
  S3_MEM_NB_BLOCK *S3NBPtr;
  DESCRIPTOR_GROUP DeviceDescript[MAX_NODES_SUPPORTED];

  NBArray = MemMainPtr->NBPtr;
  RefPtr = NBArray[BSP_DIE].RefPtr;

  if (RefPtr->SaveMemContextCtl) {
    MemMDisableScrubber (MemMainPtr);

    RefPtr->MemContext.NvStorage = NULL;
    RefPtr->MemContext.NvStorageSize = 0;

    // Make sure DQS training has occurred before saving memory context
    if (!RefPtr->MemRestoreCtl) {
      StdHeader = &MemMainPtr->MemPtr->StdHeader;

      MemMCreateS3NbBlock (MemMainPtr, &S3NBPtr);
      if (S3NBPtr != NULL) {
        // Get the mask bit and the register list for node that presents
        BufferSize = 0;
        for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
          S3NBPtr->MemS3GetConPCIMask (S3NBPtr[Node].NBPtr, (VOID *)&DeviceDescript[Node]);
          S3NBPtr->MemS3GetConMSRMask (S3NBPtr[Node].NBPtr, (VOID *)&DeviceDescript[Node]);
          BufferSize += S3NBPtr->MemS3GetRegLstPtr (S3NBPtr[Node].NBPtr, (VOID *)&DeviceDescript[Node]);
        }

        // Base on the size of the device list, apply for a buffer for it.
        AllocHeapParams.RequestedBufferSize = (UINT32) (BufferSize + sizeof (DEVICE_BLOCK_HEADER));
        AllocHeapParams.BufferHandle = AMD_MEM_S3_DATA_HANDLE;
        AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
        if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
          DeviceList = (DEVICE_BLOCK_HEADER *) AllocHeapParams.BufferPtr;
          DeviceList->RelativeOrMaskOffset = (UINT16) AllocHeapParams.RequestedBufferSize;

          // Copy device list on the stack to the heap.
          BufferOffset = sizeof (DEVICE_BLOCK_HEADER) + AllocHeapParams.BufferPtr;
          for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
            for (i = PRESELFREF; i <= POSTSELFREF; i ++) {
              // Copy PCI device descriptor to the heap if it exists.
              if (DeviceDescript[Node].PCIDevice[i].RegisterListID != 0xFFFFFFFF) {
                LibAmdMemCopy (BufferOffset, &(DeviceDescript[Node].PCIDevice[i]), sizeof (PCI_DEVICE_DESCRIPTOR), StdHeader);
                DeviceList->NumDevices ++;
                BufferOffset = sizeof (PCI_DEVICE_DESCRIPTOR) + (UINT8 *)BufferOffset;
              }
              // Copy conditional PCI device descriptor to the heap if it exists.
              if (DeviceDescript[Node].CPCIDevice[i].RegisterListID != 0xFFFFFFFF) {
                LibAmdMemCopy (BufferOffset, &(DeviceDescript[Node].CPCIDevice[i]), sizeof (CONDITIONAL_PCI_DEVICE_DESCRIPTOR), StdHeader);
                DeviceList->NumDevices ++;
                BufferOffset = sizeof (CONDITIONAL_PCI_DEVICE_DESCRIPTOR) + (UINT8 *)BufferOffset;
              }
              // Copy MSR device descriptor to the heap if it exists.
              if (DeviceDescript[Node].MSRDevice[i].RegisterListID != 0xFFFFFFFF) {
                LibAmdMemCopy (BufferOffset, &(DeviceDescript[Node].MSRDevice[i]), sizeof (MSR_DEVICE_DESCRIPTOR), StdHeader);
                DeviceList->NumDevices ++;
                BufferOffset = sizeof (MSR_DEVICE_DESCRIPTOR) + (UINT8 *)BufferOffset;
              }
              // Copy conditional MSR device descriptor to the heap if it exists.
              if (DeviceDescript[Node].CMSRDevice[i].RegisterListID != 0xFFFFFFFF) {
                LibAmdMemCopy (BufferOffset, &(DeviceDescript[Node].PCIDevice[i]), sizeof (CONDITIONAL_MSR_DEVICE_DESCRIPTOR), StdHeader);
                DeviceList->NumDevices ++;
                BufferOffset = sizeof (CONDITIONAL_MSR_DEVICE_DESCRIPTOR) + (UINT8 *)BufferOffset;
              }
            }
          }

          // Determine size needed
          BufferSize = GetWorstCaseContextSize (DeviceList, INIT_RESUME, StdHeader);
          BufferSize += GetReducedMemBlockSize (MemMainPtr);
          AllocHeapParams.RequestedBufferSize = BufferSize;
          AllocHeapParams.BufferHandle = AMD_S3_SAVE_HANDLE;
          AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
          if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
            // Save memory context
            SaveDeviceListContext (DeviceList, AllocHeapParams.BufferPtr, INIT_RESUME, &BufferSize, StdHeader);
            // Save pointer to the next block, namely the reduced memory block
            DeviceList = (DEVICE_BLOCK_HEADER *)AllocHeapParams.BufferPtr;
            DeviceList->NextBlockOffset = BufferSize;
            // Save reduced memory block
            MemMReducedMemBlockSave (MemMainPtr, AllocHeapParams.BufferPtr + BufferSize, &BufferSize);
            RefPtr->MemContext.NvStorageSize = BufferSize;
          }

          HeapDeallocateBuffer (AMD_MEM_S3_DATA_HANDLE, StdHeader);
        }
      }
      HeapDeallocateBuffer (AMD_MEM_S3_NB_HANDLE, StdHeader);

      // Locate MemContext since it might have been shifted after deallocating
      LocHeap.BufferHandle = AMD_S3_SAVE_HANDLE;
      if (HeapLocateBuffer (&LocHeap, StdHeader) == AGESA_SUCCESS) {
        RefPtr->MemContext.NvStorage = LocHeap.BufferPtr;
      }
    }
    MemMRestoreScrubber (MemMainPtr);
  }

  for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
    NBArray[Node].FamilySpecificHook[AfterSaveRestore] (&NBArray[Node], &NBArray[Node]);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Check and restore memory context if possible.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  DQS timing restore succeeds.
 *     @return          FALSE - DQS timing restore fails.
 */
BOOLEAN
MemMContextRestore (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8 Node;
  UINT8 Mode;
  MEM_NB_BLOCK  *NBArray;
  MEM_PARAMETER_STRUCT *RefPtr;
  MEM_DATA_STRUCT *MemPtr;
  S3_MEM_NB_BLOCK *S3NBPtr;
  VOID   *OrMaskPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  NBArray = MemMainPtr->NBPtr;
  RefPtr = NBArray[BSP_DIE].RefPtr;
  MemPtr = MemMainPtr->MemPtr;
  Mode = 0;

  IDS_HDT_CONSOLE (MEM_FLOW, "\nMemRestoreCtl: %d\n", RefPtr->MemRestoreCtl);
  IDS_HDT_CONSOLE (MEM_FLOW, "\nSaveMemContextCtl : %d\n", RefPtr->SaveMemContextCtl);

  if (RefPtr->MemRestoreCtl) {
    if (RefPtr->MemContext.NvStorage != NULL) {
      IDS_HDT_CONSOLE (MEM_STATUS, "\nStart Mem Restore\n");
      MemMCreateS3NbBlock (MemMainPtr, &S3NBPtr);
      if (S3NBPtr != NULL) {
        // Restore registers before exiting self refresh
        RestorePreESRContext (&OrMaskPtr,
                              RefPtr->MemContext.NvStorage,
                              INIT_RESUME,
                              &(MemPtr->StdHeader));
        MemMReducedMemBlockRestore (MemMainPtr, RefPtr->MemContext.NvStorage);
        // Exit self refresh
        for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
          // Call this function to sync DctSelCfg and NBPtr->Dct to avoid assert.
          NBArray[Node].FamilySpecificHook[AfterSaveRestore] (&NBArray[Node], &NBArray[Node]);
        }
        if ((RefPtr->IsCapsuleMode && (AmdMemS3Resume (&(MemPtr->StdHeader)) == AGESA_SUCCESS)) ||
            (!RefPtr->IsCapsuleMode && (MemMDramInit (MemMainPtr) == TRUE))) {
          // Restore registers after exiting self refresh
          RestorePostESRContext (OrMaskPtr,
                                RefPtr->MemContext.NvStorage,
                                INIT_RESUME,
                                &(MemPtr->StdHeader));

          // If context restore is enabled, do memclr if ECC is enabled
          if (!RefPtr->IsCapsuleMode && RefPtr->EnableEccFeature) {
            for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
              if ((NBArray[Node].MCTPtr->NodeMemSize != 0) && (NBArray[Node].GetBitField (&NBArray[Node], BFDramEccEn) == 0)) {
                break;
              }
            }
            if (Node == MemMainPtr->DieCount) {
              AGESA_TESTPOINT (TpProcMemMemClr, &(MemMainPtr->MemPtr->StdHeader));
              MemFeatMain.MemClr (MemMainPtr);
            }
          }
          // Set LockDramCfg, which must be done after Memory Clear
          for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
            if (IsFeatureEnabled (C6Cstate, MemMainPtr->MemPtr->PlatFormConfig, &(MemMainPtr->MemPtr->StdHeader))) {
              IDS_SKIP_HOOK (IDS_LOCK_DRAM_CFG, &NBArray[Node], &MemMainPtr->MemPtr->StdHeader) {
                NBArray[Node].SetBitField (&NBArray[Node], BFLockDramCfg, 1);
              }
            }
          }
        } else {
          RefPtr->MemRestoreCtl = FALSE;
        }
      } else {
        RefPtr->MemRestoreCtl = FALSE;
      }
      HeapDeallocateBuffer (AMD_MEM_S3_NB_HANDLE, &(MemPtr->StdHeader));
    } else {
      IEM_SKIP_CODE (IEM_MEM_RESTORE) {
        RefPtr->MemRestoreCtl = FALSE;
      }
    }
  }

  // Set the flag to skip memory S3 save when memory data is being restored
  if (RefPtr->MemRestoreCtl) {
    AllocHeapParams.RequestedBufferSize = 1;
    AllocHeapParams.BufferHandle = AMD_SKIP_MEM_S3_SAVE;
    AllocHeapParams.Persist = HEAP_SYSTEM_MEM;

    if (HeapAllocateBuffer (&AllocHeapParams, &(MemPtr->StdHeader)) != AGESA_SUCCESS) {
      ASSERT (FALSE);
    }

    if (RefPtr->IsCapsuleMode) {
      PutEventLog (AGESA_SUCCESS, MEM_EVENT_CAPSULE_IN_EFFECT, 0, 0, 0, 0, &(MemPtr->StdHeader));
    } else {
      PutEventLog (AGESA_SUCCESS, MEM_EVENT_CONTEXT_RESTORE_IN_EFFECT, 0, 0, 0, 0, &(MemPtr->StdHeader));
    }
  }

  for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
    NBArray[Node].FamilySpecificHook[AfterSaveRestore] (&NBArray[Node], &NBArray[Node]);
  }
  IDS_HDT_CONSOLE (MEM_FLOW, RefPtr->MemRestoreCtl ? "Mem Restore Succeeds!\n" : "Mem Restore Fails!\n");
  return RefPtr->MemRestoreCtl;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Save all memory related data for S3.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMS3Save (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  MEM_PARAMETER_STRUCT *RefPtr;
  BOOLEAN SaveMemContextCtl;
  BOOLEAN MemRestoreCtl;

  RefPtr = MemMainPtr->NBPtr[BSP_DIE].RefPtr;

  // If memory context has not been saved
  if (RefPtr->MemContext.NvStorage == NULL) {
    // Change memory context save and restore control to allow memory context to happen
    SaveMemContextCtl = RefPtr->SaveMemContextCtl;
    MemRestoreCtl = RefPtr->MemRestoreCtl;
    RefPtr->SaveMemContextCtl = TRUE;
    RefPtr->MemRestoreCtl = FALSE;

    MemMContextSave (MemMainPtr);

    // Restore the original control
    RefPtr->SaveMemContextCtl = SaveMemContextCtl;
    RefPtr->MemRestoreCtl = MemRestoreCtl;

    if (RefPtr->MemContext.NvStorage == NULL) {
      // Memory context cannot be saved succesfully
      ASSERT (FALSE);
      return FALSE;
    }
  }

  // Allocate heap for memory S3 data to pass to main AMDS3Save
  // Apply for 4 bytes more than the size of the data buffer to store the size of data buffer
  IDS_HDT_CONSOLE (MEM_FLOW, "\nSave memory S3 data in heap\n");
  AllocHeapParams.RequestedBufferSize = RefPtr->MemContext.NvStorageSize + 4;
  AllocHeapParams.BufferHandle = AMD_MEM_S3_SAVE_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;

  if (HeapAllocateBuffer (&AllocHeapParams, &(MemMainPtr->MemPtr->StdHeader)) == AGESA_SUCCESS) {
    LibAmdMemCopy (AllocHeapParams.BufferPtr + 4, RefPtr->MemContext.NvStorage, RefPtr->MemContext.NvStorageSize, &(MemMainPtr->MemPtr->StdHeader));
    *(UINT32 *) AllocHeapParams.BufferPtr = RefPtr->MemContext.NvStorageSize;
    return TRUE;
  } else {
    ASSERT (FALSE);
    return FALSE;
  }
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * This function does dram init based on register value
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 *
 */
BOOLEAN
STATIC
MemMDramInit (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8 Node;
  UINT8 Dct;
  MEM_NB_BLOCK  *NBArray;
  MEM_NB_BLOCK  *NBPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  BIT_FIELD_NAME CsBitField;
  ID_INFO CallOutIdInfo;
  BOOLEAN RetVal;

  RefPtr = MemMainPtr->NBPtr[BSP_DIE].RefPtr;
  NBArray = MemMainPtr->NBPtr;
  RetVal = TRUE;

  for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
    NBPtr = &NBArray[Node];

    // When memory pstate is enabled, change MemPstateStage flag so registers won't be programmed twice
    if (NBPtr->MemPstateStage == MEMORY_PSTATE_1ST_STAGE) {
      NBPtr->MemPstateStage = MEMORY_PSTATE_S3_STAGE;
    }

    // Get dimm population info
    NBPtr->TechPtr->DimmPresence (NBPtr->TechPtr);
    NBPtr->MemNPlatformSpecificFormFactorInitNb (NBPtr);

    for (Dct = 0; Dct < NBPtr->DctCount; Dct ++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      for (CsBitField = BFCSBaseAddr0Reg; CsBitField <= BFCSBaseAddr7Reg; CsBitField ++) {
        if ((NBPtr->GetBitField (NBPtr, CsBitField) & 5) != 0) {
          // Need this variable for Dram init
          NBPtr->DCTPtr->Timings.CsPresent |= (UINT16)1 << (CsBitField - BFCSBaseAddr0Reg);
        }
      }

      // Enable memory clock
      if (NBPtr->GetBitField (NBPtr, BFDisDramInterface) == 0) {
        NBPtr->SetBitField (NBPtr, BFMemClkFreqVal, 1);
        while (NBPtr->GetBitField (NBPtr, BFFreqChgInProg) != 0) {}
      }

      // Get memory clock speed
      NBPtr->DCTPtr->Timings.Speed = MemNGetMemClkFreqUnb (NBPtr, (UINT8) NBPtr->GetBitField (NBPtr, BFMemClkFreq));
      NBPtr->DCTPtr->Timings.TargetSpeed = NBPtr->DCTPtr->Timings.Speed;

      // Call platform specific parameter processing function as lots of variable populated here
      // will be used during dram init
      if (NBPtr->GetBitField (NBPtr, BFDisDramInterface) == 0) {
        if (!NBPtr->PsPtr->MemPDoPs (NBPtr)) {
          IDS_ERROR_TRAP;
        }
      }
    }

    // Callout before Dram Init
    AGESA_TESTPOINT (TpProcMemBeforeAgesaHookBeforeDramInit, &(MemMainPtr->MemPtr->StdHeader));
    CallOutIdInfo.IdField.SocketId = NBPtr->MCTPtr->SocketId;
    CallOutIdInfo.IdField.ModuleId = NBPtr->MCTPtr->DieId;
    IDS_HDT_CONSOLE (MEM_FLOW, "\nCalling out to Platform BIOS on Socket %d, Module %d...\n", CallOutIdInfo.IdField.SocketId, CallOutIdInfo.IdField.ModuleId);
    AgesaHookBeforeDramInit ((UINTN) CallOutIdInfo.IdInformation, MemMainPtr->MemPtr);
    NBPtr->FamilySpecificHook[AmpVoltageDisp] (NBPtr, NULL);
    IDS_HDT_CONSOLE (MEM_FLOW, "\nVDDIO = 1.%dV\n", (NBPtr->RefPtr->DDR3Voltage == VOLT1_5) ? 5 :
                                          (NBPtr->RefPtr->DDR3Voltage == VOLT1_35) ? 35 :
                                          (NBPtr->RefPtr->DDR3Voltage == VOLT1_25) ? 25 : 999);
    AGESA_TESTPOINT (TpProcMemAfterAgesaHookBeforeDramInit, &(NBPtr->MemPtr->StdHeader));

    // Do Dram init
    AGESA_TESTPOINT (TpProcMemDramInit, &(NBPtr->MemPtr->StdHeader));
    IDS_HDT_CONSOLE (MEM_FLOW, "\nMemClkFreq: %d MHz\n", NBPtr->DCTPtr->Timings.Speed);
    NBPtr->FeatPtr->DramInit (NBPtr->TechPtr);

    // Print out hob info
    IDS_HDT_CONSOLE (MEM_FLOW, "UmaSize: %x\n", NBPtr->RefPtr->UmaSize);
    IDS_HDT_CONSOLE (MEM_FLOW, "UmaBase: %x\n", NBPtr->RefPtr->UmaBase);
    IDS_HDT_CONSOLE (MEM_FLOW, "UmaMode: %x\n", NBPtr->RefPtr->UmaMode);
    IDS_HDT_CONSOLE (MEM_FLOW, "Sub4GCacheTop: %x\n", NBPtr->RefPtr->Sub4GCacheTop);
    IDS_HDT_CONSOLE (MEM_FLOW, "SysLimit: %x\n\n", NBPtr->RefPtr->SysLimit);

    if (NBPtr->MCTPtr->ErrCode == AGESA_FATAL) {
      RetVal = FALSE;
    }
  }

  return RetVal;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *  Create S3 NB Block.
 *
 *     @param[in,out]   MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *     @param[out]      S3NBPtr   - Pointer to the S3 NB Block pointer
 *
 */
VOID
STATIC
MemMCreateS3NbBlock (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr,
     OUT   S3_MEM_NB_BLOCK **S3NBPtr
  )
{
  UINT8 Node;
  UINT8 i;
  MEM_NB_BLOCK  *NBArray;
  MEM_NB_BLOCK  *DummyNBs;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  NBArray = MemMainPtr->NBPtr;

  *S3NBPtr = NULL;

  // Allocate heap for S3 NB Blocks
  AllocHeapParams.RequestedBufferSize = (MemMainPtr->DieCount * (sizeof (S3_MEM_NB_BLOCK) + sizeof (MEM_NB_BLOCK)));
  AllocHeapParams.BufferHandle = AMD_MEM_S3_NB_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &(MemMainPtr->MemPtr->StdHeader)) == AGESA_SUCCESS) {
    *S3NBPtr = (S3_MEM_NB_BLOCK *) AllocHeapParams.BufferPtr;
    DummyNBs = (MEM_NB_BLOCK *) (AllocHeapParams.BufferPtr + MemMainPtr->DieCount * sizeof (S3_MEM_NB_BLOCK));

    // Initialize S3 NB Blocks
    for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
      (*S3NBPtr)[Node].NBPtr = &DummyNBs[Node];

      for (i = 0; memNBInstalled[i].MemS3ResumeConstructNBBlock != 0; i++) {
        if (memNBInstalled[i].MemS3ResumeConstructNBBlock (&(*S3NBPtr)[Node], NBArray[BSP_DIE].MemPtr, Node)) {
          (*S3NBPtr)[Node].NBPtr->RefPtr = NBArray[Node].RefPtr;
          break;
        }
      };
      if (memNBInstalled[i].MemS3ResumeConstructNBBlock == 0) {
        *S3NBPtr = NULL;
        break;
      }
    }
  }
}
