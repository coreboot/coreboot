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
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
#include "mm.h"
#include "mn.h"
#include "Ids.h"
#include "S3.h"
#include "mfs3.h"
#include "heapManager.h"
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
BOOLEAN
STATIC
MemMRestoreDqsTimings (
  IN       VOID                *Storage,
  IN       MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

BOOLEAN
STATIC
MemMSetCSRNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       PCI_SPECIAL_CASE *SpecialCases,
  IN       PCI_ADDR PciAddr,
  IN       UINT32 Value
  );

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
MemMS3Save (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );
/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/
extern MEM_NB_SUPPORT memNBInstalled[];

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
          AllocHeapParams.RequestedBufferSize = BufferSize;
          AllocHeapParams.BufferHandle = AMD_S3_SAVE_HANDLE;
          AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
          if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
            // Save memory context
            SaveDeviceListContext (DeviceList, AllocHeapParams.BufferPtr, INIT_RESUME, &BufferSize, StdHeader);
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
  MEM_NB_BLOCK  *NBArray;
  MEM_PARAMETER_STRUCT *RefPtr;
  S3_MEM_NB_BLOCK *S3NBPtr;

  NBArray = MemMainPtr->NBPtr;
  RefPtr = NBArray[BSP_DIE].RefPtr;

  IDS_HDT_CONSOLE (MEM_STATUS, "\nStart Mem Restore\n");
  if (RefPtr->MemRestoreCtl) {
    if (RefPtr->MemContext.NvStorage != NULL) {
      MemMCreateS3NbBlock (MemMainPtr, &S3NBPtr);
      if (S3NBPtr != NULL) {
        // Check DIMM config and restore DQS timings if possible
        if (!MemMRestoreDqsTimings (RefPtr->MemContext.NvStorage, MemMainPtr)) {
          RefPtr->MemRestoreCtl = FALSE;
        }
      } else {
        RefPtr->MemRestoreCtl = FALSE;
      }
      HeapDeallocateBuffer (AMD_MEM_S3_NB_HANDLE, &(MemMainPtr->MemPtr->StdHeader));
    } else {
      IEM_SKIP_CODE (IEM_MEM_RESTORE) {
        RefPtr->MemRestoreCtl = FALSE;
      }
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
 * Restores all devices that contains DQS timings
 *
 *     @param[in]     Storage        Beginning of the device list.
 *     @param[in,out]   MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 *
 */
BOOLEAN
STATIC
MemMRestoreDqsTimings (
  IN       VOID                *Storage,
  IN       MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  AMD_CONFIG_PARAMS *StdHeader;
  UINT8 *OrMask;
  DEVICE_DESCRIPTORS Device;
  INT16 i;
  INT16 j;
  DEVICE_BLOCK_HEADER *DeviceList;
  PCI_REGISTER_BLOCK_HEADER *Reg;
  CPCI_REGISTER_BLOCK_HEADER *CReg;
  MSR_REGISTER_BLOCK_HEADER *MsrReg;
  CMSR_REGISTER_BLOCK_HEADER *CMsrReg;
  PCI_ADDR PciAddress;
  MEM_NB_BLOCK  *NBArray;
  UINT8 State;
  UINT8 Node;
  UINT8 Dct;
  UINT8 MaxNode;

  NBArray = MemMainPtr->NBPtr;
  StdHeader = &(MemMainPtr->MemPtr->StdHeader);
  DeviceList = (DEVICE_BLOCK_HEADER *) Storage;
  Device.CommonDeviceHeader = (DEVICE_DESCRIPTOR *) &DeviceList[1];
  OrMask = (UINT8 *) DeviceList + DeviceList->RelativeOrMaskOffset;

  if (DeviceList->NumDevices == 0) {
    return FALSE;
  }

  MaxNode = 0;
  State = ST_PRE_ESR;
  for (i = 0; State != ST_DONE; i++) {
    if (((State == ST_PRE_ESR) && (Device.CommonDeviceHeader->Type == DEV_TYPE_PCI_PRE_ESR)) ||
        ((State == ST_POST_ESR) && (Device.CommonDeviceHeader->Type == DEV_TYPE_PCI))) {
      MemFS3GetPciDeviceRegisterList (Device.PciDevice, &Reg, StdHeader);
      Node = Device.PciDevice->Node;
      IDS_HDT_CONSOLE (MEM_STATUS, "Node %d\n", Node);
      PciAddress = NBArray[Node].PciAddr;
      for (j = 0; j < Reg->NumRegisters; j++) {
        PciAddress.Address.Function = Reg->RegisterList[j].Function;
        PciAddress.Address.Register = Reg->RegisterList[j].Offset;
        PciAddress.Address.Segment = (Reg->RegisterList[j].Type.SpecialCaseFlag != 0) ?
                                     0xF - Reg->RegisterList[j].Type.SpecialCaseIndex : 0;
        if (!MemMSetCSRNb (&NBArray[Node], Reg->SpecialCases, PciAddress, *((UINT32 *) OrMask) & Reg->RegisterList[j].AndMask)) {
          return FALSE;   // Restore fails
        }
        OrMask += (Reg->RegisterList[j].Type.RegisterSize == 0) ? 4 : Reg->RegisterList[j].Type.RegisterSize;
      }

      if (MaxNode < Node) {
        MaxNode = Node;
      }

    } else if (((State == ST_PRE_ESR) && (Device.CommonDeviceHeader->Type == DEV_TYPE_CPCI_PRE_ESR)) ||
               ((State == ST_POST_ESR) && (Device.CommonDeviceHeader->Type == DEV_TYPE_CPCI))) {
      MemFS3GetCPciDeviceRegisterList (Device.CPciDevice, &CReg, StdHeader);
      Node = Device.CPciDevice->Node;
      IDS_HDT_CONSOLE (MEM_STATUS, "Node %d\n", Node);
      PciAddress = NBArray[Node].PciAddr;
      for (j = 0; j < CReg->NumRegisters; j++) {
        if (((Device.CPciDevice->Mask1 & CReg->RegisterList[j].Mask1) != 0) &&
            ((Device.CPciDevice->Mask2 & CReg->RegisterList[j].Mask2) != 0)) {
          PciAddress.Address.Function = CReg->RegisterList[j].Function;
          PciAddress.Address.Register = CReg->RegisterList[j].Offset;
          PciAddress.Address.Segment = (CReg->RegisterList[j].Type.SpecialCaseFlag != 0) ?
                                       0xF - CReg->RegisterList[j].Type.SpecialCaseIndex : 0;
          if (!MemMSetCSRNb (&NBArray[Node], CReg->SpecialCases, PciAddress, *((UINT32 *) OrMask) & CReg->RegisterList[j].AndMask)) {
            return FALSE;   // Restore fails
          }
          OrMask += (CReg->RegisterList[j].Type.RegisterSize == 0) ? 4 : CReg->RegisterList[j].Type.RegisterSize;
        }
      }
    } else if (((State == ST_PRE_ESR) && (Device.CommonDeviceHeader->Type == DEV_TYPE_MSR_PRE_ESR)) ||
               ((State == ST_POST_ESR) && (Device.CommonDeviceHeader->Type == DEV_TYPE_MSR))) {
      MemFS3GetMsrDeviceRegisterList (Device.MsrDevice, &MsrReg, StdHeader);
      for (j = 0; j < MsrReg->NumRegisters; j++) {
        OrMask += 8;
      }
    } else if (((State == ST_PRE_ESR) && (Device.CommonDeviceHeader->Type == DEV_TYPE_CMSR_PRE_ESR)) ||
               ((State == ST_POST_ESR) && (Device.CommonDeviceHeader->Type == DEV_TYPE_CMSR))) {
      MemFS3GetCMsrDeviceRegisterList (Device.CMsrDevice, &CMsrReg, StdHeader);
      for (j = 0; j < CMsrReg->NumRegisters; j++) {
        if (((Device.CMsrDevice->Mask1 & CMsrReg->RegisterList[j].Mask1) != 0) &&
            ((Device.CMsrDevice->Mask2 & CMsrReg->RegisterList[j].Mask2) != 0)) {
          OrMask += 8;
        }
      }
    }

    switch (Device.CommonDeviceHeader->Type) {
    case DEV_TYPE_PCI_PRE_ESR:
      // Fall through to advance the pointer after restoring context
    case DEV_TYPE_PCI:
      Device.PciDevice++;
      break;
    case DEV_TYPE_CPCI_PRE_ESR:
      // Fall through to advance the pointer after restoring context
    case DEV_TYPE_CPCI:
      Device.CPciDevice++;
      break;
    case DEV_TYPE_MSR_PRE_ESR:
      // Fall through to advance the pointer after restoring context
    case DEV_TYPE_MSR:
      Device.MsrDevice++;
      break;
    case DEV_TYPE_CMSR_PRE_ESR:
      // Fall through to advance the pointer after restoring context
    case DEV_TYPE_CMSR:
      Device.CMsrDevice++;
      break;
    default:
      ASSERT (FALSE);
      break;
    }

    if (i == (DeviceList->NumDevices - 1)) {
      // Go to next state
      State++;
      i = -1;
      Device.CommonDeviceHeader = (DEVICE_DESCRIPTOR *) &DeviceList[1];

      // Check to see if processor or DIMM population has changed
      if ((MaxNode + 1) != MemMainPtr->DieCount) {
        IDS_HDT_CONSOLE (MEM_FLOW, "\tSTOP: Population changed\n");
        return FALSE;
      }

      // Perform MemClk frequency change
      for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
        if (NBArray[Node].MCTPtr->NodeMemSize != 0) {
          NBArray[Node].BeforeDqsTraining (&NBArray[Node]);
          if (NBArray[Node].DCTPtr->Timings.Speed < NBArray[Node].DCTPtr->Timings.TargetSpeed) {
            for (Dct = 0; Dct < NBArray[Node].DctCount; Dct++) {
              NBArray[Node].SwitchDCT (&NBArray[Node], Dct);
              NBArray[Node].DCTPtr->Timings.Speed = NBArray[Node].DCTPtr->Timings.TargetSpeed;
            }
            IDS_OPTION_HOOK (IDS_BEFORE_MEM_FREQ_CHG, &NBArray[Node], &(MemMainPtr->MemPtr->StdHeader));
            NBArray[Node].ChangeFrequency (&NBArray[Node]);
          }
        }
      }
    }
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function filters out other settings and only restores DQS timings.
 *
 *     @param[in,out]   NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   SpecialCases - Pointer to special cases array handlers
 *     @param[in]   PciAddr - address of the CSR register in PCI_ADDR format.
 *     @param[in]   Value - Value to be programmed
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 *
 */

BOOLEAN
STATIC
MemMSetCSRNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       PCI_SPECIAL_CASE *SpecialCases,
  IN       PCI_ADDR PciAddr,
  IN       UINT32 Value
  )
{
  UINT32 Offset;
  UINT8  Dct;
  UINT32 Temp;
  BOOLEAN RetVal;
  UINT32 BOffset;

  RetVal = TRUE;
  if (PciAddr.Address.Segment != 0) {
    if (PciAddr.Address.Segment == 0xF) {
      PciAddr.Address.Segment = 0;
      Dct = (UINT8) ((PciAddr.Address.Register >> 10) & 1);
      Offset = PciAddr.Address.Register & 0x3FF;
      BOffset = PciAddr.Address.Register & 0xFF;
      if ((PciAddr.Address.Register & 0x800) == 0) {
        if (((BOffset >= 1) && (BOffset <= 3)) ||
            ((BOffset >= 5) && (BOffset <= 7)) ||
            ((Offset >= 0x10) && (Offset <= 0x2B)) ||
            ((Offset >= 0x30) && (Offset <= 0x4A))) {
          IDS_HDT_CONSOLE (MEM_FLOW, "\t\tF2_%d9C_%03x = %08x\n", Dct, Offset, Value);
          //MemNS3SetCSR
          SpecialCases[0].Restore (AccessS3SaveWidth32, PciAddr, &Value, &NBPtr->MemPtr->StdHeader);
        }
      }
    }
  } else {
    Dct = (UINT8) ((PciAddr.Address.Register >> 8) & 1);
    Offset = PciAddr.Address.Register & 0xFF;

    if (PciAddr.Address.Function == 2) {
      if ((Offset >= 0x40) && (Offset < 0x60) && ((Value & 4) != 0)) {
        // If TestFail bit is set, set CsTestFail
        NBPtr->SwitchDCT (NBPtr, Dct);
        NBPtr->DCTPtr->Timings.CsTrainFail |= (UINT16)1 << ((Offset - 0x40) >> 2);
        IDS_HDT_CONSOLE (MEM_FLOW, "\tBad CS:%d\n", ((Offset - 0x40) >> 2));
      } else if (Offset == 0x80) {
        LibAmdPciRead (AccessWidth32, PciAddr, &Temp, &NBPtr->MemPtr->StdHeader);
        if (Temp != Value) {
          IDS_HDT_CONSOLE (MEM_FLOW, "\tSTOP: DIMM config changed\n");
          RetVal = FALSE;
        }
      } else if (Offset == 0x90) {
        LibAmdPciRead (AccessWidth32, PciAddr, &Temp, &NBPtr->MemPtr->StdHeader);
        if ((Temp & 0x0001F000) != (Value & 0x0001F000)) {
          IDS_HDT_CONSOLE (MEM_FLOW, "\tSTOP: DIMM config changed\n");
          RetVal = FALSE;
        }
      } else if (Offset == 0x94) {
        LibAmdPciRead (AccessWidth32, PciAddr, &Temp, &NBPtr->MemPtr->StdHeader);
        if ((Temp & 0x00061000) != (Value & 0x00061000)) {
          IDS_HDT_CONSOLE (MEM_FLOW, "\tSTOP: DIMM config changed\n");
          RetVal = FALSE;
        }
        if (((Value & 0x4000) == 0) && (NBPtr->GetMemClkFreqId (NBPtr, NBPtr->DCTPtr->Timings.TargetSpeed) != ((Value & 7) + 1))) {
          IDS_HDT_CONSOLE (MEM_FLOW, "\tSTOP: MemClk has changed\n");
          RetVal = FALSE;
        }
        // Restore ZqcsInterval
        Temp &= 0xFFFFF3FF;
        Temp |= (Value & 0x00000C00);
        LibAmdPciWrite (AccessWidth32, PciAddr, &Temp, &NBPtr->MemPtr->StdHeader);
      } else if (Offset == 0x78) {
        // Program MaxRdLat
        LibAmdPciRead (AccessWidth32, PciAddr, &Temp, &NBPtr->MemPtr->StdHeader);
        Temp &= 0x0009BF0F;
        Temp |= (Value & 0xFFC00000);
        LibAmdPciWrite (AccessWidth32, PciAddr, &Temp, &NBPtr->MemPtr->StdHeader);
      } else if (PciAddr.Address.Register == 0x110) {
        if ((NBPtr->MCTPtr->NodeMemSize != 0) && (Value == 0x00000100)) {
          IDS_HDT_CONSOLE (MEM_FLOW, "\tSTOP: DIMM config changed\n");
          RetVal = FALSE;
        }
      }
    }
  }

  if (RetVal == FALSE) {
    NBPtr->SwitchDCT (NBPtr, 0);
    NBPtr->DCTPtr->Timings.CsTrainFail = 0;
    NBPtr->SwitchDCT (NBPtr, 1);
    NBPtr->DCTPtr->Timings.CsTrainFail = 0;
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
