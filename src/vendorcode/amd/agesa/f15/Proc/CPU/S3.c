/* $NoKeywords:$ */
/**
 * @file
 *
 * ACPI S3 Support routines
 *
 * Contains routines needed for supporting resume from the ACPI S3 sleep state.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Interface
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "S3.h"
#include "mfs3.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_S3_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
SaveDeviceContext (
  IN       DEVICE_BLOCK_HEADER *DeviceList,
  IN       CALL_POINTS         CallPoint,
     OUT   UINT32              *ActualBufferSize,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
SavePciDevice (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN       PCI_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS           CallPoint,
  IN OUT   VOID                  **OrMask
  );

VOID
SaveConditionalPciDevice (
  IN       AMD_CONFIG_PARAMS                 *StdHeader,
  IN       CONDITIONAL_PCI_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS                       CallPoint,
  IN OUT   VOID                              **OrMask
  );

VOID
SaveMsrDevice (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN       MSR_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS           CallPoint,
  IN OUT   UINT64                **OrMask
  );

VOID
SaveConditionalMsrDevice (
  IN       AMD_CONFIG_PARAMS                 *StdHeader,
  IN       CONDITIONAL_MSR_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS                       CallPoint,
  IN OUT   UINT64                            **OrMask
  );

VOID
RestorePciDevice (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN       PCI_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS           CallPoint,
  IN OUT   VOID                  **OrMask
  );

VOID
RestoreConditionalPciDevice (
  IN       AMD_CONFIG_PARAMS                 *StdHeader,
  IN       CONDITIONAL_PCI_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS                       CallPoint,
  IN OUT   VOID                              **OrMask
  );

VOID
RestoreMsrDevice (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN       MSR_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS           CallPoint,
  IN OUT   UINT64                **OrMask
  );

VOID
RestoreConditionalMsrDevice (
  IN       AMD_CONFIG_PARAMS                 *StdHeader,
  IN       CONDITIONAL_MSR_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS                       CallPoint,
  IN OUT   UINT64                            **OrMask
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Saves all devices in the given device list.
 *
 * This traverses the entire device list twice.  In the first pass, we save
 * all devices identified as Pre ESR.  In the second pass, we save devices
 * marked as post ESR.
 *
 * @param[in]     DeviceList        Beginning of the device list to save.
 * @param[in]     Storage           Beginning of the context buffer.
 * @param[in]     CallPoint         Indicates whether this is AMD_INIT_RESUME or
 *                                  AMD_S3LATE_RESTORE.
 * @param[out]    ActualBufferSize  Actual size used in saving the device list.
 * @param[in]     StdHeader         AMD standard header config param.
 *
 */
VOID
SaveDeviceListContext (
  IN       DEVICE_BLOCK_HEADER *DeviceList,
  IN       VOID                *Storage,
  IN       CALL_POINTS         CallPoint,
     OUT   UINT32              *ActualBufferSize,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  // Copy device list over
  LibAmdMemCopy (Storage,
                 DeviceList,
                 (UINTN) DeviceList->RelativeOrMaskOffset,
                 StdHeader);
  SaveDeviceContext (Storage, CallPoint, ActualBufferSize, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Saves all devices in the given device list.
 *
 * This traverses the entire device list twice.  In the first pass, we save
 * all devices identified as Pre ESR.  In the second pass, we save devices
 * marked as post ESR.
 *
 * @param[in,out] DeviceList        Beginning of the device list to save.
 * @param[in]     CallPoint         Indicates whether this is AMD_INIT_RESUME or
 *                                  AMD_S3LATE_RESTORE.
 * @param[out]    ActualBufferSize  Actual size used in saving the device list.
 * @param[in]     StdHeader         AMD standard header config param.
 *
 */
VOID
SaveDeviceContext (
  IN       DEVICE_BLOCK_HEADER *DeviceList,
  IN       CALL_POINTS         CallPoint,
     OUT   UINT32              *ActualBufferSize,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  DEVICE_DESCRIPTORS Device;
  UINT16 i;
  UINT64 StartAddress;
  UINT64 EndAddress;
  VOID *OrMask;

  StartAddress = (UINT64)(intptr_t)DeviceList;
  Device.CommonDeviceHeader = (DEVICE_DESCRIPTOR *) &DeviceList[1];
  OrMask = (UINT8 *) DeviceList + DeviceList->RelativeOrMaskOffset;

  // Process Pre ESR List
  for (i = 0; i < DeviceList->NumDevices; i++) {
    switch (Device.CommonDeviceHeader->Type) {
    case DEV_TYPE_PCI_PRE_ESR:
      SavePciDevice (StdHeader, Device.PciDevice, CallPoint, &OrMask);
      // Fall through to advance the pointer after saving context
    case DEV_TYPE_PCI:
      Device.PciDevice++;
      break;
    case DEV_TYPE_CPCI_PRE_ESR:
      SaveConditionalPciDevice (StdHeader, Device.CPciDevice, CallPoint, &OrMask);
      // Fall through to advance the pointer after saving context
    case DEV_TYPE_CPCI:
      Device.CPciDevice++;
      break;
    case DEV_TYPE_MSR_PRE_ESR:
      SaveMsrDevice (StdHeader, Device.MsrDevice, CallPoint, (UINT64 **) &OrMask);
      // Fall through to advance the pointer after saving context
    case DEV_TYPE_MSR:
      Device.MsrDevice++;
      break;
    case DEV_TYPE_CMSR_PRE_ESR:
      SaveConditionalMsrDevice (StdHeader, Device.CMsrDevice, CallPoint, (UINT64 **) &OrMask);
      // Fall through to advance the pointer after saving context
    case DEV_TYPE_CMSR:
      Device.CMsrDevice++;
      break;
    }
  }

  Device.CommonDeviceHeader = (DEVICE_DESCRIPTOR *) &DeviceList[1];
  // Process Post ESR List
  for (i = 0; i < DeviceList->NumDevices; i++) {
    switch (Device.CommonDeviceHeader->Type) {
    case DEV_TYPE_PCI:
      SavePciDevice (StdHeader, Device.PciDevice, CallPoint, &OrMask);
      // Fall through to advance the pointer after saving context
    case DEV_TYPE_PCI_PRE_ESR:
      Device.PciDevice++;
      break;
    case DEV_TYPE_CPCI:
      SaveConditionalPciDevice (StdHeader, Device.CPciDevice, CallPoint, &OrMask);
      // Fall through to advance the pointer after saving context
    case DEV_TYPE_CPCI_PRE_ESR:
      Device.CPciDevice++;
      break;
    case DEV_TYPE_MSR:
      SaveMsrDevice (StdHeader, Device.MsrDevice, CallPoint, (UINT64 **) &OrMask);
      // Fall through to advance the pointer after saving context
    case DEV_TYPE_MSR_PRE_ESR:
      Device.MsrDevice++;
      break;
    case DEV_TYPE_CMSR:
      SaveConditionalMsrDevice (StdHeader, Device.CMsrDevice, CallPoint, (UINT64 **) &OrMask);
      // Fall through to advance the pointer after saving context
    case DEV_TYPE_CMSR_PRE_ESR:
      Device.CMsrDevice++;
      break;
    }
  }
  EndAddress = (UINT64)(intptr_t)OrMask;
  *ActualBufferSize = (UINT32) (EndAddress - StartAddress);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Saves the context of a PCI device.
 *
 * This traverses the provided register list saving PCI registers.
 *
 * @param[in]     StdHeader      AMD standard header config param.
 * @param[in]     Device         PCI device to restore.
 * @param[in]     CallPoint      Indicates whether this is AMD_INIT_RESUME or
 *                               AMD_S3LATE_RESTORE.
 * @param[in,out] OrMask         Current buffer pointer of raw register values.
 *
 */
VOID
SavePciDevice (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN       PCI_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS           CallPoint,
  IN OUT   VOID                  **OrMask
  )
{
  UINT8   RegSizeInBytes;
  UINT8   SpecialCaseIndex;
  UINT8   *IntermediatePtr;
  UINT16  i;
  UINT32  Socket;
  UINT32  Module;
  UINT32  AndMask;
  ACCESS_WIDTH AccessWidth;
  AGESA_STATUS IgnoredSts;
  PCI_ADDR PciAddress;
  PCI_REGISTER_BLOCK_HEADER *RegisterHdr;

  GetSocketModuleOfNode ((UINT32) Device->Node,
                               &Socket,
                               &Module,
                               StdHeader);
  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);

  if (CallPoint == INIT_RESUME) {
    MemFS3GetPciDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  } else {
    S3GetPciDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  }

  for (i = 0; i < RegisterHdr->NumRegisters; i++) {
    PciAddress.Address.Function = RegisterHdr->RegisterList[i].Function;
    PciAddress.Address.Register = RegisterHdr->RegisterList[i].Offset;
    RegSizeInBytes = RegisterHdr->RegisterList[i].Type.RegisterSize;
    switch (RegSizeInBytes) {
    case 1:
      AndMask = 0xFFFFFFFF & ((UINT8) RegisterHdr->RegisterList[i].AndMask);
      AccessWidth = AccessS3SaveWidth8;
      break;
    case 2:
      AndMask = 0xFFFFFFFF & ((UINT16) RegisterHdr->RegisterList[i].AndMask);
      AccessWidth = AccessS3SaveWidth16;
      break;
    case 3:
      // In this case, we don't need to save a register. We just need to call a special
      // function to do certain things in the save and resume sequence.
      // This should not be used in a non-special case.
      AndMask = 0;
      RegSizeInBytes = 0;
      AccessWidth = 0;
      break;
    default:
      AndMask = RegisterHdr->RegisterList[i].AndMask;
      RegSizeInBytes = 4;
      AccessWidth = AccessS3SaveWidth32;
      break;
    }
    if (RegisterHdr->RegisterList[i].Type.SpecialCaseFlag == 0) {
      ASSERT ((AndMask != 0) && (RegSizeInBytes != 0) && (AccessWidth != 0));
      LibAmdPciRead (AccessWidth, PciAddress, *OrMask, StdHeader);
    } else {
      SpecialCaseIndex = RegisterHdr->RegisterList[i].Type.SpecialCaseIndex;
      RegisterHdr->SpecialCases[SpecialCaseIndex].Save (AccessWidth, PciAddress, *OrMask, StdHeader);
    }
    if (AndMask != 0) {
      // If AndMask is 0, then it is a not-care. Don't need to apply it to the OrMask
      **((UINT32 **) OrMask) &= AndMask;
    }
    if ((RegSizeInBytes == 0) && (**((UINT32 **) OrMask) == RESTART_FROM_BEGINNING_LIST)) {
      // Restart from the beginning of the register list
      i = 0xFFFF;
    }
    IntermediatePtr = (UINT8 *) *OrMask;
    *OrMask = &IntermediatePtr[RegSizeInBytes]; // += RegSizeInBytes;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Saves the context of a 'conditional' PCI device.
 *
 * This traverses the provided register list saving PCI registers when appropriate.
 *
 * @param[in]     StdHeader      AMD standard header config param.
 * @param[in]     Device         'conditional' PCI device to restore.
 * @param[in]     CallPoint      Indicates whether this is AMD_INIT_RESUME or
 *                               AMD_S3LATE_RESTORE.
 * @param[in,out] OrMask         Current buffer pointer of raw register values.
 *
 */
VOID
SaveConditionalPciDevice (
  IN       AMD_CONFIG_PARAMS                 *StdHeader,
  IN       CONDITIONAL_PCI_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS                       CallPoint,
  IN OUT   VOID                              **OrMask
  )
{
  UINT8   RegSizeInBytes;
  UINT8   SpecialCaseIndex;
  UINT8   *IntermediatePtr;
  UINT16  i;
  UINT32  Socket;
  UINT32  Module;
  UINT32  AndMask;
  ACCESS_WIDTH AccessWidth;
  AGESA_STATUS IgnoredSts;
  PCI_ADDR PciAddress;
  CPCI_REGISTER_BLOCK_HEADER *RegisterHdr;

  GetSocketModuleOfNode ((UINT32) Device->Node,
                               &Socket,
                               &Module,
                               StdHeader);
  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);

  if (CallPoint == INIT_RESUME) {
    MemFS3GetCPciDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  } else {
    S3GetCPciDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  }

  for (i = 0; i < RegisterHdr->NumRegisters; i++) {
    if (((Device->Mask1 & RegisterHdr->RegisterList[i].Mask1) != 0) &&
        ((Device->Mask2 & RegisterHdr->RegisterList[i].Mask2) != 0)) {
      PciAddress.Address.Function = RegisterHdr->RegisterList[i].Function;
      PciAddress.Address.Register = RegisterHdr->RegisterList[i].Offset;
      RegSizeInBytes = RegisterHdr->RegisterList[i].Type.RegisterSize;
      switch (RegSizeInBytes) {
      case 1:
        AndMask = 0xFFFFFFFF & ((UINT8) RegisterHdr->RegisterList[i].AndMask);
        AccessWidth = AccessS3SaveWidth8;
        break;
      case 2:
        AndMask = 0xFFFFFFFF & ((UINT16) RegisterHdr->RegisterList[i].AndMask);
        AccessWidth = AccessS3SaveWidth16;
        break;
      case 3:
        // In this case, we don't need to save a register. We just need to call a special
        // function to do certain things in the save and resume sequence.
        // This should not be used in a non-special case.
        AndMask = 0;
        RegSizeInBytes = 0;
        AccessWidth = 0;
        break;
      default:
        AndMask = RegisterHdr->RegisterList[i].AndMask;
        RegSizeInBytes = 4;
        AccessWidth = AccessS3SaveWidth32;
        break;
      }
      if (RegisterHdr->RegisterList[i].Type.SpecialCaseFlag == 0) {
        ASSERT ((AndMask != 0) && (RegSizeInBytes != 0) && (AccessWidth != 0));
        LibAmdPciRead (AccessWidth, PciAddress, *OrMask, StdHeader);
      } else {
        SpecialCaseIndex = RegisterHdr->RegisterList[i].Type.SpecialCaseIndex;
        RegisterHdr->SpecialCases[SpecialCaseIndex].Save (AccessWidth, PciAddress, *OrMask, StdHeader);
      }
      if (AndMask != 0) {
        // If AndMask is 0, then it is a not-care. Don't need to apply it to the OrMask
        **((UINT32 **) OrMask) &= AndMask;
      }
      if ((RegSizeInBytes == 0) && (**((UINT32 **) OrMask) == RESTART_FROM_BEGINNING_LIST)) {
        // Restart from the beginning of the register list
        i = 0xFFFF;
      }
      IntermediatePtr = (UINT8 *) *OrMask;
      *OrMask = &IntermediatePtr[RegSizeInBytes]; // += RegSizeInBytes;
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Saves the context of an MSR device.
 *
 * This traverses the provided register list saving MSRs.
 *
 * @param[in]     StdHeader      AMD standard header config param.
 * @param[in]     Device         MSR device to restore.
 * @param[in]     CallPoint      Indicates whether this is AMD_INIT_RESUME or
 *                               AMD_S3LATE_RESTORE.
 * @param[in,out] OrMask         Current buffer pointer of raw register values.
 *
 */
VOID
SaveMsrDevice (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN       MSR_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS           CallPoint,
  IN OUT   UINT64                **OrMask
  )
{
  UINT8   SpecialCaseIndex;
  UINT16  i;
  MSR_REGISTER_BLOCK_HEADER *RegisterHdr;

  if (CallPoint == INIT_RESUME) {
    MemFS3GetMsrDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  } else {
    S3GetMsrDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  }

  for (i = 0; i < RegisterHdr->NumRegisters; i++) {
    if (RegisterHdr->RegisterList[i].Type.SpecialCaseFlag == 0) {
      LibAmdMsrRead (RegisterHdr->RegisterList[i].Address, *OrMask, StdHeader);
    } else {
      SpecialCaseIndex = RegisterHdr->RegisterList[i].Type.SpecialCaseIndex;
      RegisterHdr->SpecialCases[SpecialCaseIndex].Save (RegisterHdr->RegisterList[i].Address, *OrMask, StdHeader);
    }
    **OrMask &= RegisterHdr->RegisterList[i].AndMask;
    (*OrMask)++;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Saves the context of a 'conditional' MSR device.
 *
 * This traverses the provided register list saving MSRs when appropriate.
 *
 * @param[in]     StdHeader      AMD standard header config param.
 * @param[in]     Device         'conditional' MSR device to restore.
 * @param[in]     CallPoint      Indicates whether this is AMD_INIT_RESUME or
 *                               AMD_S3LATE_RESTORE.
 * @param[in,out] OrMask         Current buffer pointer of raw register values.
 *
 */
VOID
SaveConditionalMsrDevice (
  IN       AMD_CONFIG_PARAMS                 *StdHeader,
  IN       CONDITIONAL_MSR_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS                       CallPoint,
  IN OUT   UINT64                            **OrMask
  )
{
  UINT8   SpecialCaseIndex;
  UINT16  i;
  CMSR_REGISTER_BLOCK_HEADER *RegisterHdr;

  if (CallPoint == INIT_RESUME) {
    MemFS3GetCMsrDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  } else {
    S3GetCMsrDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  }

  for (i = 0; i < RegisterHdr->NumRegisters; i++) {
    if (((Device->Mask1 & RegisterHdr->RegisterList[i].Mask1) != 0) &&
        ((Device->Mask2 & RegisterHdr->RegisterList[i].Mask2) != 0)) {
      if (RegisterHdr->RegisterList[i].Type.SpecialCaseFlag == 0) {
        LibAmdMsrRead (RegisterHdr->RegisterList[i].Address, (UINT64 *) *OrMask, StdHeader);
      } else {
        SpecialCaseIndex = RegisterHdr->RegisterList[i].Type.SpecialCaseIndex;
        RegisterHdr->SpecialCases[SpecialCaseIndex].Save (RegisterHdr->RegisterList[i].Address, (UINT64 *) *OrMask, StdHeader);
      }
      **OrMask &= RegisterHdr->RegisterList[i].AndMask;
      (*OrMask)++;
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Determines the maximum amount of space required to store all raw register
 * values for the given device list.
 *
 * This traverses the entire device list, and calculates the worst case size
 * of each device in the device list.
 *
 * @param[in]     DeviceList     Beginning of the device list.
 * @param[in]     CallPoint      Indicates whether this is AMD_INIT_RESUME or
 *                               AMD_S3LATE_RESTORE.
 * @param[in]     StdHeader      AMD standard header config param.
 *
 * @retval        Size in bytes required for storing all registers.
 */
UINT32
GetWorstCaseContextSize (
  IN       DEVICE_BLOCK_HEADER *DeviceList,
  IN       CALL_POINTS         CallPoint,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32 WorstCaseSize;
  DEVICE_DESCRIPTORS Device;
  UINT16 i;
  REGISTER_BLOCK_HEADERS RegisterHdr;

  WorstCaseSize = DeviceList->RelativeOrMaskOffset;
  Device.CommonDeviceHeader = (DEVICE_DESCRIPTOR *) &DeviceList[1];

  // Process Device List
  for (i = 0; i < DeviceList->NumDevices; i++) {
    switch (Device.CommonDeviceHeader->Type) {
    case DEV_TYPE_PCI_PRE_ESR:
      // PRE_ESR and post ESR take the same amount of space
    case DEV_TYPE_PCI:
      if (CallPoint == INIT_RESUME) {
        MemFS3GetPciDeviceRegisterList (Device.PciDevice, &RegisterHdr.PciRegisters, StdHeader);
      } else {
        S3GetPciDeviceRegisterList (Device.PciDevice, &RegisterHdr.PciRegisters, StdHeader);
      }
      WorstCaseSize += (RegisterHdr.PciRegisters->NumRegisters * 4);
      Device.PciDevice++;
      break;
    case DEV_TYPE_CPCI_PRE_ESR:
      // PRE_ESR and post ESR take the same amount of space
    case DEV_TYPE_CPCI:
      if (CallPoint == INIT_RESUME) {
        MemFS3GetCPciDeviceRegisterList (Device.CPciDevice, &RegisterHdr.CPciRegisters, StdHeader);
      } else {
        S3GetCPciDeviceRegisterList (Device.CPciDevice, &RegisterHdr.CPciRegisters, StdHeader);
      }
      WorstCaseSize += (RegisterHdr.CPciRegisters->NumRegisters * 4);
      Device.CPciDevice++;
      break;
    case DEV_TYPE_MSR_PRE_ESR:
      // PRE_ESR and post ESR take the same amount of space
    case DEV_TYPE_MSR:
      if (CallPoint == INIT_RESUME) {
        MemFS3GetMsrDeviceRegisterList (Device.MsrDevice, &RegisterHdr.MsrRegisters, StdHeader);
      } else {
        S3GetMsrDeviceRegisterList (Device.MsrDevice, &RegisterHdr.MsrRegisters, StdHeader);
      }
      WorstCaseSize += (RegisterHdr.MsrRegisters->NumRegisters * 8);
      Device.MsrDevice++;
      break;
    case DEV_TYPE_CMSR_PRE_ESR:
      // PRE_ESR and post ESR take the same amount of space
    case DEV_TYPE_CMSR:
      if (CallPoint == INIT_RESUME) {
        MemFS3GetCMsrDeviceRegisterList (Device.CMsrDevice, &RegisterHdr.CMsrRegisters, StdHeader);
      } else {
        S3GetCMsrDeviceRegisterList (Device.CMsrDevice, &RegisterHdr.CMsrRegisters, StdHeader);
      }
      WorstCaseSize += (RegisterHdr.CMsrRegisters->NumRegisters * 8);
      Device.CMsrDevice++;
      break;
    default:
      ASSERT (FALSE);
    }
  }
  return (WorstCaseSize);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Restores all devices marked as 'before exiting self-refresh.'
 *
 * This traverses the entire device list, restoring all devices identified
 * as Pre ESR.
 *
 * @param[in,out] OrMaskPtr      Current buffer pointer of raw register values.
 * @param[in]     Storage        Beginning of the device list.
 * @param[in]     CallPoint      Indicates whether this is AMD_INIT_RESUME or
 *                               AMD_S3LATE_RESTORE.
 * @param[in]     StdHeader      AMD standard header config param.
 *
 */
VOID
RestorePreESRContext (
     OUT   VOID              **OrMaskPtr,
  IN       VOID              *Storage,
  IN       CALL_POINTS       CallPoint,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  DEVICE_DESCRIPTORS Device;
  UINT16 i;
  DEVICE_BLOCK_HEADER *DeviceList;

  DeviceList = (DEVICE_BLOCK_HEADER *) Storage;
  Device.CommonDeviceHeader = (DEVICE_DESCRIPTOR *) &DeviceList[1];
  *OrMaskPtr = (UINT8 *) DeviceList + DeviceList->RelativeOrMaskOffset;

  // Process Pre ESR List
  for (i = 0; i < DeviceList->NumDevices; i++) {
    switch (Device.CommonDeviceHeader->Type) {
    case DEV_TYPE_PCI_PRE_ESR:
      RestorePciDevice (StdHeader, Device.PciDevice, CallPoint, OrMaskPtr);
      // Fall through to advance the pointer after restoring context
    case DEV_TYPE_PCI:
      Device.PciDevice++;
      break;
    case DEV_TYPE_CPCI_PRE_ESR:
      RestoreConditionalPciDevice (StdHeader, Device.CPciDevice, CallPoint, OrMaskPtr);
      // Fall through to advance the pointer after restoring context
    case DEV_TYPE_CPCI:
      Device.CPciDevice++;
      break;
    case DEV_TYPE_MSR_PRE_ESR:
      RestoreMsrDevice (StdHeader, Device.MsrDevice, CallPoint, (UINT64 **) OrMaskPtr);
      // Fall through to advance the pointer after restoring context
    case DEV_TYPE_MSR:
      Device.MsrDevice++;
      break;
    case DEV_TYPE_CMSR_PRE_ESR:
      RestoreConditionalMsrDevice (StdHeader, Device.CMsrDevice, CallPoint, (UINT64 **) OrMaskPtr);
      // Fall through to advance the pointer after restoring context
    case DEV_TYPE_CMSR:
      Device.CMsrDevice++;
      break;
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Restores all devices marked as 'after exiting self-refresh.'
 *
 * This traverses the entire device list, restoring all devices identified
 * as Post ESR.
 *
 * @param[in]     OrMaskPtr      Current buffer pointer of raw register values.
 * @param[in]     Storage        Beginning of the device list.
 * @param[in]     CallPoint      Indicates whether this is AMD_INIT_RESUME or
 *                               AMD_S3LATE_RESTORE.
 * @param[in]     StdHeader      AMD standard header config param.
 *
 */
VOID
RestorePostESRContext (
  IN       VOID              *OrMaskPtr,
  IN       VOID              *Storage,
  IN       CALL_POINTS       CallPoint,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  DEVICE_DESCRIPTORS Device;
  UINT16 i;
  DEVICE_BLOCK_HEADER *DeviceList;

  DeviceList = (DEVICE_BLOCK_HEADER *) Storage;
  Device.CommonDeviceHeader = (DEVICE_DESCRIPTOR *) &DeviceList[1];

  // Process Pre ESR List
  for (i = 0; i < DeviceList->NumDevices; i++) {
    switch (Device.CommonDeviceHeader->Type) {
    case DEV_TYPE_PCI:
      RestorePciDevice (StdHeader, Device.PciDevice, CallPoint, &OrMaskPtr);
      // Fall through to advance the pointer after restoring context
    case DEV_TYPE_PCI_PRE_ESR:
      Device.PciDevice++;
      break;
    case DEV_TYPE_CPCI:
      RestoreConditionalPciDevice (StdHeader, Device.CPciDevice, CallPoint, &OrMaskPtr);
      // Fall through to advance the pointer after restoring context
    case DEV_TYPE_CPCI_PRE_ESR:
      Device.CPciDevice++;
      break;
    case DEV_TYPE_MSR:
      RestoreMsrDevice (StdHeader, Device.MsrDevice, CallPoint, (UINT64 **) &OrMaskPtr);
      // Fall through to advance the pointer after restoring context
    case DEV_TYPE_MSR_PRE_ESR:
      Device.MsrDevice++;
      break;
    case DEV_TYPE_CMSR:
      RestoreConditionalMsrDevice (StdHeader, Device.CMsrDevice, CallPoint, (UINT64 **) &OrMaskPtr);
      // Fall through to advance the pointer after restoring context
    case DEV_TYPE_CMSR_PRE_ESR:
      Device.CMsrDevice++;
      break;
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Restores the context of a PCI device.
 *
 * This traverses the provided register list restoring PCI registers.
 *
 * @param[in]     StdHeader      AMD standard header config param.
 * @param[in]     Device         'conditional' PCI device to restore.
 * @param[in]     CallPoint      Indicates whether this is AMD_INIT_RESUME or
 *                               AMD_S3LATE_RESTORE.
 * @param[in,out] OrMask         Current buffer pointer of raw register values.
 *
 */
VOID
RestorePciDevice (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN       PCI_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS           CallPoint,
  IN OUT   VOID                  **OrMask
  )
{
  UINT8   RegSizeInBytes;
  UINT8   SpecialCaseIndex;
  UINT8   *IntermediatePtr;
  UINT16  i;
  UINT32  Socket;
  UINT32  Module;
  UINT32  AndMask;
  UINT32  RegValueRead;
  UINT32  RegValueWrite;
  ACCESS_WIDTH AccessWidth;
  AGESA_STATUS IgnoredSts;
  PCI_ADDR PciAddress;
  PCI_REGISTER_BLOCK_HEADER *RegisterHdr;

  GetSocketModuleOfNode ((UINT32) Device->Node,
                               &Socket,
                               &Module,
                               StdHeader);
  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);

  if (CallPoint == INIT_RESUME) {
    MemFS3GetPciDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  } else {
    S3GetPciDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  }

  for (i = 0; i < RegisterHdr->NumRegisters; i++) {
    PciAddress.Address.Function = RegisterHdr->RegisterList[i].Function;
    PciAddress.Address.Register = RegisterHdr->RegisterList[i].Offset;
    RegSizeInBytes = RegisterHdr->RegisterList[i].Type.RegisterSize;
    switch (RegSizeInBytes) {
    case 1:
      AndMask = 0xFFFFFFFF & ((UINT8) RegisterHdr->RegisterList[i].AndMask);
      RegValueWrite = **(UINT8 **)OrMask;
      AccessWidth = AccessS3SaveWidth8;
      break;
    case 2:
      AndMask = 0xFFFFFFFF & ((UINT16) RegisterHdr->RegisterList[i].AndMask);
      RegValueWrite = **(UINT16 **)OrMask;
      AccessWidth = AccessS3SaveWidth16;
      break;
    case 3:
      // In this case, we don't need to restore a register. We just need to call a special
      // function to do certain things in the save and resume sequence.
      // This should not be used in a non-special case.
      AndMask = 0;
      RegValueWrite = 0;
      RegSizeInBytes = 0;
      AccessWidth = 0;
      break;
    default:
      AndMask = RegisterHdr->RegisterList[i].AndMask;
      RegSizeInBytes = 4;
      RegValueWrite = **(UINT32 **)OrMask;
      AccessWidth = AccessS3SaveWidth32;
      break;
    }
    if (RegisterHdr->RegisterList[i].Type.SpecialCaseFlag == 0) {
      ASSERT ((AndMask != 0) && (RegSizeInBytes != 0) && (AccessWidth != 0));
      LibAmdPciRead (AccessWidth, PciAddress, &RegValueRead, StdHeader);
      RegValueWrite |= RegValueRead & (~AndMask);
      LibAmdPciWrite (AccessWidth, PciAddress, &RegValueWrite, StdHeader);
    } else {
      SpecialCaseIndex = RegisterHdr->RegisterList[i].Type.SpecialCaseIndex;
      if (AndMask != 0) {
        RegisterHdr->SpecialCases[SpecialCaseIndex].Save (AccessWidth,
                                             PciAddress,
                                             &RegValueRead,
                                             StdHeader);
        RegValueWrite |= RegValueRead & (~AndMask);
      }
      RegisterHdr->SpecialCases[SpecialCaseIndex].Restore (AccessWidth,
                                             PciAddress,
                                             &RegValueWrite,
                                             StdHeader);
    }
    IntermediatePtr = (UINT8 *) *OrMask;
    *OrMask = &IntermediatePtr[RegSizeInBytes]; // += RegSizeInBytes;
    if ((RegSizeInBytes == 0) && (RegValueWrite == RESTART_FROM_BEGINNING_LIST)) {
      // Restart from the beginning of the register list
      i = 0xFFFF;
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Restores the context of a 'conditional' PCI device.
 *
 * This traverses the provided register list restoring PCI registers when appropriate.
 *
 * @param[in]     StdHeader      AMD standard header config param.
 * @param[in]     Device         'conditional' PCI device to restore.
 * @param[in]     CallPoint      Indicates whether this is AMD_INIT_RESUME or
 *                               AMD_S3LATE_RESTORE.
 * @param[in,out] OrMask         Current buffer pointer of raw register values.
 *
 */
VOID
RestoreConditionalPciDevice (
  IN       AMD_CONFIG_PARAMS                 *StdHeader,
  IN       CONDITIONAL_PCI_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS                       CallPoint,
  IN OUT   VOID                              **OrMask
  )
{
  UINT8   RegSizeInBytes;
  UINT8   SpecialCaseIndex;
  UINT8   *IntermediatePtr;
  UINT16  i;
  UINT32  Socket;
  UINT32  Module;
  UINT32  RegValueRead;
  UINT32  RegValueWrite;
  UINT32  AndMask;
  ACCESS_WIDTH AccessWidth;
  AGESA_STATUS IgnoredSts;
  PCI_ADDR PciAddress;
  CPCI_REGISTER_BLOCK_HEADER *RegisterHdr;

  GetSocketModuleOfNode ((UINT32) Device->Node,
                               &Socket,
                               &Module,
                               StdHeader);
  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);

  if (CallPoint == INIT_RESUME) {
    MemFS3GetCPciDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  } else {
    S3GetCPciDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  }

  for (i = 0; i < RegisterHdr->NumRegisters; i++) {
    if (((Device->Mask1 & RegisterHdr->RegisterList[i].Mask1) != 0) &&
        ((Device->Mask2 & RegisterHdr->RegisterList[i].Mask2) != 0)) {
      PciAddress.Address.Function = RegisterHdr->RegisterList[i].Function;
      PciAddress.Address.Register = RegisterHdr->RegisterList[i].Offset;
      RegSizeInBytes = RegisterHdr->RegisterList[i].Type.RegisterSize;
      switch (RegSizeInBytes) {
      case 1:
        AndMask = 0xFFFFFFFF & ((UINT8) RegisterHdr->RegisterList[i].AndMask);
        RegValueWrite = **(UINT8 **)OrMask;
        AccessWidth = AccessS3SaveWidth8;
        break;
      case 2:
        AndMask = 0xFFFFFFFF & ((UINT16) RegisterHdr->RegisterList[i].AndMask);
        RegValueWrite = **(UINT16 **)OrMask;
        AccessWidth = AccessS3SaveWidth16;
        break;
      case 3:
        // In this case, we don't need to restore a register. We just need to call a special
        //  function to do certain things in the save and resume sequence.
        // This should not be used in a non-special case.
        AndMask = 0;
        RegValueWrite = 0;
        RegSizeInBytes = 0;
        AccessWidth = 0;
        break;
      default:
        AndMask = RegisterHdr->RegisterList[i].AndMask;
        RegSizeInBytes = 4;
        RegValueWrite = **(UINT32 **)OrMask;
        AccessWidth = AccessS3SaveWidth32;
        break;
      }
      if (RegisterHdr->RegisterList[i].Type.SpecialCaseFlag == 0) {
        LibAmdPciRead (AccessWidth, PciAddress, &RegValueRead, StdHeader);
        RegValueWrite |= RegValueRead & (~AndMask);
        LibAmdPciWrite (AccessWidth, PciAddress, &RegValueWrite, StdHeader);
      } else {
        SpecialCaseIndex = RegisterHdr->RegisterList[i].Type.SpecialCaseIndex;
        if (AndMask != 0) {
          RegisterHdr->SpecialCases[SpecialCaseIndex].Save (AccessWidth,
                                             PciAddress,
                                             &RegValueRead,
                                             StdHeader);
          RegValueWrite |= RegValueRead & (~AndMask);
        }
        RegisterHdr->SpecialCases[SpecialCaseIndex].Restore (AccessWidth,
                                               PciAddress,
                                               &RegValueWrite,
                                               StdHeader);
      }
      IntermediatePtr = (UINT8 *) *OrMask;
      *OrMask = &IntermediatePtr[RegSizeInBytes];
      if ((RegSizeInBytes == 0) && (RegValueWrite == RESTART_FROM_BEGINNING_LIST)) {
        // Restart from the beginning of the register list
        i = 0xFFFF;
      }
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Restores the context of an MSR device.
 *
 * This traverses the provided register list restoring MSRs.
 *
 * @param[in]     StdHeader      AMD standard header config param.
 * @param[in]     Device         MSR device to restore.
 * @param[in]     CallPoint      Indicates whether this is AMD_INIT_RESUME or
 *                               AMD_S3LATE_RESTORE.
 * @param[in,out] OrMask         Current buffer pointer of raw register values.
 *
 */
VOID
RestoreMsrDevice (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN       MSR_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS           CallPoint,
  IN OUT   UINT64                **OrMask
  )
{
  UINT8   SpecialCaseIndex;
  UINT16  i;
  UINT64  RegValueRead;
  UINT64  RegValueWrite;
  MSR_REGISTER_BLOCK_HEADER *RegisterHdr;

  if (CallPoint == INIT_RESUME) {
    MemFS3GetMsrDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  } else {
    S3GetMsrDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  }

  for (i = 0; i < RegisterHdr->NumRegisters; i++) {
    RegValueWrite = **OrMask;
    if (RegisterHdr->RegisterList[i].Type.SpecialCaseFlag == 0) {
      LibAmdMsrRead (RegisterHdr->RegisterList[i].Address, &RegValueRead, StdHeader);
      RegValueWrite |= RegValueRead & (~RegisterHdr->RegisterList[i].AndMask);
      LibAmdMsrWrite (RegisterHdr->RegisterList[i].Address, &RegValueWrite, StdHeader);
    } else {
      SpecialCaseIndex = RegisterHdr->RegisterList[i].Type.SpecialCaseIndex;
      RegisterHdr->SpecialCases[SpecialCaseIndex].Save (RegisterHdr->RegisterList[i].Address,
                                                           &RegValueRead,
                                                           StdHeader);
      RegValueWrite |= RegValueRead & (~RegisterHdr->RegisterList[i].AndMask);
      RegisterHdr->SpecialCases[SpecialCaseIndex].Restore (RegisterHdr->RegisterList[i].Address,
                                                           &RegValueWrite,
                                                           StdHeader);
    }
    (*OrMask)++;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Restores the context of a 'conditional' MSR device.
 *
 * This traverses the provided register list restoring MSRs when appropriate.
 *
 * @param[in]     StdHeader      AMD standard header config param.
 * @param[in]     Device         'conditional' MSR device to restore.
 * @param[in]     CallPoint      Indicates whether this is AMD_INIT_RESUME or
 *                               AMD_S3LATE_RESTORE.
 * @param[in,out] OrMask         Current buffer pointer of raw register values.
 *
 */
VOID
RestoreConditionalMsrDevice (
  IN       AMD_CONFIG_PARAMS                 *StdHeader,
  IN       CONDITIONAL_MSR_DEVICE_DESCRIPTOR *Device,
  IN       CALL_POINTS                       CallPoint,
  IN OUT   UINT64                            **OrMask
  )
{
  UINT8   SpecialCaseIndex;
  UINT16  i;
  UINT64  RegValueRead;
  UINT64  RegValueWrite;
  CMSR_REGISTER_BLOCK_HEADER *RegisterHdr;

  if (CallPoint == INIT_RESUME) {
    MemFS3GetCMsrDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  } else {
    S3GetCMsrDeviceRegisterList (Device, &RegisterHdr, StdHeader);
  }

  for (i = 0; i < RegisterHdr->NumRegisters; i++) {
    if (((Device->Mask1 & RegisterHdr->RegisterList[i].Mask1) != 0) &&
        ((Device->Mask2 & RegisterHdr->RegisterList[i].Mask2) != 0)) {
      RegValueWrite = **OrMask;
      if (RegisterHdr->RegisterList[i].Type.SpecialCaseFlag == 0) {
        LibAmdMsrRead (RegisterHdr->RegisterList[i].Address, &RegValueRead, StdHeader);
        RegValueWrite |= RegValueRead & (~RegisterHdr->RegisterList[i].AndMask);
        LibAmdMsrWrite (RegisterHdr->RegisterList[i].Address, &RegValueWrite, StdHeader);
      } else {
        SpecialCaseIndex = RegisterHdr->RegisterList[i].Type.SpecialCaseIndex;
        RegisterHdr->SpecialCases[SpecialCaseIndex].Save (RegisterHdr->RegisterList[i].Address,
                                                           &RegValueRead,
                                                           StdHeader);
        RegValueWrite |= RegValueRead & (~RegisterHdr->RegisterList[i].AndMask);
        RegisterHdr->SpecialCases[SpecialCaseIndex].Restore (RegisterHdr->RegisterList[i].Address,
                                                             &RegValueWrite,
                                                             StdHeader);
      }
      (*OrMask)++;
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Unique device ID to PCI register list translator.
 *
 * This translates the given device header in storage to the appropriate list
 * of registers in the AGESA image.
 *
 * @param[out]   NonMemoryRelatedDeviceList  List of devices to save and restore
 *                                           during S3LateRestore.
 * @param[in]    StdHeader                   AMD standard header config param.
 *
 */
VOID
GetNonMemoryRelatedDeviceList (
     OUT   DEVICE_BLOCK_HEADER **NonMemoryRelatedDeviceList,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  *NonMemoryRelatedDeviceList = NULL;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Unique device ID to PCI register list translator.
 *
 * This translates the given device header in storage to the appropriate list
 * of registers in the AGESA image.
 *
 * @param[in]     Device         Device header containing the unique ID.
 * @param[out]    RegisterHdr    Output PCI register list pointer.
 * @param[in]     StdHeader      AMD standard header config param.
 *
 * @retval        AGESA_SUCCESS  Always succeeds.
 */
AGESA_STATUS
S3GetPciDeviceRegisterList (
  IN       PCI_DEVICE_DESCRIPTOR     *Device,
     OUT   PCI_REGISTER_BLOCK_HEADER **RegisterHdr,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  *RegisterHdr = NULL;
  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Unique device ID to 'conditional' PCI register list translator.
 *
 * This translates the given device header in storage to the appropriate list
 * of registers in the AGESA image.
 *
 * @param[in]     Device         Device header containing the unique ID.
 * @param[out]    RegisterHdr    Output 'conditional' PCI register list pointer.
 * @param[in]     StdHeader      AMD standard header config param.
 *
 * @retval        AGESA_SUCCESS  Always succeeds.
 */
AGESA_STATUS
S3GetCPciDeviceRegisterList (
  IN       CONDITIONAL_PCI_DEVICE_DESCRIPTOR *Device,
     OUT   CPCI_REGISTER_BLOCK_HEADER        **RegisterHdr,
  IN       AMD_CONFIG_PARAMS                 *StdHeader
  )
{
  *RegisterHdr = NULL;
  return AGESA_SUCCESS;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Unique device ID to MSR register list translator.
 *
 * This translates the given device header in storage to the appropriate list
 * of registers in the AGESA image.
 *
 * @param[in]     Device         Device header containing the unique ID.
 * @param[out]    RegisterHdr    Output MSR register list pointer.
 * @param[in]     StdHeader      AMD standard header config param.
 *
 * @retval        AGESA_SUCCESS  Always succeeds.
 */
AGESA_STATUS
S3GetMsrDeviceRegisterList (
  IN       MSR_DEVICE_DESCRIPTOR     *Device,
     OUT   MSR_REGISTER_BLOCK_HEADER **RegisterHdr,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  *RegisterHdr = NULL;
  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Unique device ID to 'conditional' MSR register list translator.
 *
 * This translates the given device header in storage to the appropriate list
 * of registers in the AGESA image.
 *
 * @param[in]     Device         Device header containing the unique ID.
 * @param[out]    RegisterHdr    Output 'conditional' MSR register list pointer.
 * @param[in]     StdHeader      AMD standard header config param.
 *
 * @retval        AGESA_SUCCESS  Always succeeds.
 */
AGESA_STATUS
S3GetCMsrDeviceRegisterList (
  IN       CONDITIONAL_MSR_DEVICE_DESCRIPTOR *Device,
     OUT   CMSR_REGISTER_BLOCK_HEADER        **RegisterHdr,
  IN       AMD_CONFIG_PARAMS                 *StdHeader
  )
{
  *RegisterHdr = NULL;
  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Constructor for the AMD_S3_PARAMS structure.
 *
 * This routine initializes failsafe values for the AMD_S3_PARAMS structure
 * to be used by the AMD_INIT_RESUME, AMD_S3_SAVE, and AMD_S3LATE_RESTORE
 * entry points.
 *
 * @param[in,out] S3Params       Required input parameter for the AMD_S3_SAVE,
 *                               AMD_INIT_RESUME, and AMD_S3_SAVE entry points.
 *
 */
VOID
AmdS3ParamsInitializer (
  OUT   AMD_S3_PARAMS *S3Params
  )
{
  S3Params->Signature = 0x52545341;
  S3Params->Version = 0x0000;
  S3Params->VolatileStorage = NULL;
  S3Params->VolatileStorageSize = 0x00000000;
  S3Params->Flags = 0x00000000;
  S3Params->NvStorage = NULL;
  S3Params->NvStorageSize = 0x00000000;
}
