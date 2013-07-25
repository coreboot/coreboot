/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Integrated Debug library Routines
 *
 * Contains AMD AGESA debug macros and library functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 */
/*****************************************************************************
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
 ******************************************************************************
 */
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "Ids.h"
#include "amdlib.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuPstateTables.h"
#include "IdsLib.h"
#include "heapManager.h"

#include "mm.h"
#include "mn.h"
#include "cpuLateInit.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_IDS_LIBRARY_IDSLIB_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE            PstateFamilyServiceTable;

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
AGESA_STATUS
AmdGetIdsImagebase (
  IN OUT   UINT64 *IdsImageBase,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

/**
 *
 *  Get IDS NV table pointer in the AGESA Heap.
 *
 *  @param[in,out] IdsNvTable   The Pointer of IDS NV Table.
 *  @param[in,out] StdHeader    The Pointer of Standard Header.
 *
 *  @retval AGESA_SUCCESS       Success to get the pointer of NV Table.
v *  @retval AGESA_ERROR         Fail to get the pointer of NV Table.
 **/
AGESA_STATUS
AmdGetIdsNvTable (
  IN OUT   VOID  **IdsNvTable,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AGESA_STATUS status;
  LOCATE_HEAP_PTR LocateHeapStructPtr;
  IDS_CONTROL_STRUCT *IdsCtrlPtr;

  LocateHeapStructPtr.BufferHandle = IDS_CONTROL_HANDLE;
  LocateHeapStructPtr.BufferPtr = NULL;
  status = HeapLocateBuffer (&LocateHeapStructPtr, StdHeader);
  if (status == AGESA_SUCCESS) {
    IdsCtrlPtr = (IDS_CONTROL_STRUCT *) LocateHeapStructPtr.BufferPtr;
    *IdsNvTable = LocateHeapStructPtr.BufferPtr + IdsCtrlPtr->IdsNvTableOffset;
  }
  return status;
}

/**
 *
 *  Get IDS Override Image Base Address
 *
 *  @param[in,out] IdsImageBase The Base Address of IDS Override Image.
 *  @param[in,out] StdHeader    The Pointer of Standard Header.
 *
 *  @retval AGESA_SUCCESS       Success to get the pointer of NV Table.
 *  @retval AGESA_ERROR         Fail to get the pointer of NV Table.
 *
 **/
AGESA_STATUS
AmdGetIdsImagebase (
  IN OUT   UINT64 *IdsImageBase,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AGESA_STATUS status;
  LOCATE_HEAP_PTR LocateHeapStructPtr;
  IDS_CONTROL_STRUCT *IdsCtrlPtr;

  LocateHeapStructPtr.BufferHandle = IDS_CONTROL_HANDLE;
  LocateHeapStructPtr.BufferPtr = NULL;
  status = HeapLocateBuffer (&LocateHeapStructPtr, StdHeader);
  if (status == AGESA_SUCCESS) {
    IdsCtrlPtr = (IDS_CONTROL_STRUCT *) LocateHeapStructPtr.BufferPtr;
    *IdsImageBase = IdsCtrlPtr->IdsImageBase;
  }
  return status;
}

/**
 *
 * Read IDS NV value in NV table.
 *
 * It searches the table until the Nv Id is found and return the NV value
 * in the table. Otherwise, return IDS_UNSUPPORTED.
 *
 * @param[in] IdsNvId       IDS NV ID
 * @param[in] NvTablePtr    NV Table pointer.
 * @param[in,out] StdHeader    The Pointer of Standard Header.
 *
 * @retval IDS_UNSUPPORTED  NV ID is not found in the table
 *         Other Value      The NV value
 *
 **/
IDS_STATUS
AmdIdsNvReader (
  IN       UINT16 IdsNvId,
  IN       IDS_NV_ITEM *NvTablePtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  IDS_STATUS Status;
  IDS_NV_ITEM *NvPtr;
  BOOLEAN IgnoreIdsDefault;
  AGESA_STATUS status;
  LOCATE_HEAP_PTR LocateHeapStructPtr;
  IDS_CONTROL_STRUCT *IdsCtrlPtr;

  IgnoreIdsDefault = FALSE;
  Status = IDS_UNSUPPORTED;
  NvPtr = NvTablePtr;

  if (NvPtr != NULL) {
    while (NvPtr->IdsNvId != AGESA_IDS_NV_END) {
      if (NvPtr->IdsNvId == IdsNvId) {
        break;
      } else {
        NvPtr ++;
      }
    }
    if ((NvPtr->IdsNvId != AGESA_IDS_NV_END)) {
      //Get IgnoreIdsDefault from heap
      LocateHeapStructPtr.BufferHandle = IDS_CONTROL_HANDLE;
      LocateHeapStructPtr.BufferPtr = NULL;
      status = HeapLocateBuffer (&LocateHeapStructPtr, StdHeader);
      if (status == AGESA_SUCCESS) {
        IdsCtrlPtr = (IDS_CONTROL_STRUCT *) LocateHeapStructPtr.BufferPtr;
        IgnoreIdsDefault = IdsCtrlPtr->IgnoreIdsDefault;
      }

      if (IgnoreIdsDefault || (NvPtr->IdsNvValue != AGESA_IDS_DFT_VAL)) {
        Status = NvPtr->IdsNvValue;
      }
    }
  }
  return Status;
}

/**
 * IDS function for only return IDS_SUCCESS
 *
 *
 *  @param[in,out]   DataPtr      meaningless data pointer
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS          Always succeeds.
 *
 **/
IDS_STATUS
IdsCommonReturn (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  return IDS_SUCCESS;
}


/**
 * IDS function for ap run specific task after amdinitpost
 *
 *
 *  @param[in]   ApicIdOfCore      apic id of specific AP
 *  @param[in]   ApLateTaskPtr    The Pointer of IDSAPLATETASK.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *
 *  @retval AGESA_SUCCESS       Success
 *  @retval AGESA_ERROR         meet some error
 *
 **/
AGESA_STATUS
IdsAgesaRunFcnOnApLate  (
  IN       UINTN               ApicIdOfCore,
  IN       IDSAPLATETASK  *ApLateTaskPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AGESA_STATUS  Status;
  AP_EXE_PARAMS LaunchApParams;

//init AgesaRunFcnOnAp parameters
  LaunchApParams.FunctionNumber = IDS_LATE_RUN_AP_TASK_ID;
  LaunchApParams.RelatedBlockLength = SIZE_IN_DWORDS (IDSAPLATETASK);
  LaunchApParams.RelatedDataBlock = ApLateTaskPtr;
  LaunchApParams.StdHeader = *StdHeader;

  AGESA_TESTPOINT (TpIfBeforeRunApFromIds, StdHeader);
  Status = AgesaRunFcnOnAp ((UINTN) ApicIdOfCore, &LaunchApParams);
  AGESA_TESTPOINT (TpIfAfterRunApFromIds, StdHeader);

  return Status;
}

/**
 * IDS function force all cores run specific task after amdinitpost
 *
 *
 *  @param[in]   ApLateTaskPtr    The Pointer of IDSAPLATETASK.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *
 *  @retval AGESA_SUCCESS       Success
 *  @retval AGESA_ERROR         meet some error
 *
 **/
AGESA_STATUS
IdsAgesaRunFcnOnAllCoresLate  (
  IN       IDSAPLATETASK  *ApLateTaskPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AP_EXE_PARAMS LaunchApParams;
  AGESA_STATUS Status;

//init AgesaRunFcnOnAp parameters
  Status = AGESA_SUCCESS;
  LaunchApParams.FunctionNumber = IDS_LATE_RUN_AP_TASK_ID;
  LaunchApParams.RelatedBlockLength = SIZE_IN_DWORDS (IDSAPLATETASK);
  LaunchApParams.RelatedDataBlock = ApLateTaskPtr;
  LaunchApParams.StdHeader = *StdHeader;

  Status = RunLateApTaskOnAllAPs (&LaunchApParams, StdHeader);

//do it on Bsp
  Status = ApLateTaskPtr->ApTask (ApLateTaskPtr->ApTaskPara, StdHeader);
  return Status;
}

/**
 * IDS call-back function for ApDispatchTable
 *
 *  @param[in] AmdApExeParams  AP_EXE_PARAMS.
 *
 *  @retval AGESA_SUCCESS       Success
 *  @retval AGESA_ERROR         meet some error
 *
 **/
AGESA_STATUS
AmdIdsRunApTaskLate (
  IN       AP_EXE_PARAMS  *AmdApExeParams
  )
{
  IDSAPLATETASK *ApLateTaskPtr;
  AGESA_STATUS Status;

  ApLateTaskPtr = (IDSAPLATETASK *)AmdApExeParams->RelatedDataBlock;
  Status = ApLateTaskPtr->ApTask (ApLateTaskPtr->ApTaskPara, &AmdApExeParams->StdHeader);
  return Status;
}

/**
 *
 *
 *     IDS Common routine for run code on AP for both ealry & later stage
 *     This routine can only be used when AP service have been established
 *
 *     @param[in]   PIdsRuncodeParams     Point to parameters structure
 *     @param[in,out]   StdHeader    - The Pointer of AGESA Header
 *
 */
VOID
IdsRunCodeOnCores (
  IN       IDS_RUNCODE_PARAMS *PIdsRuncodeParams,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AP_TASK ApTask;
  IDSAPLATETASK IdsLateTask;
  UINT32 NumberOfCores;
  UINT8 StartSocket;
  UINT8 EndSocket;
  UINT8 Socket;
  UINT8 StartCore;
  UINT8 EndCore;
  UINT8 Core;
  UINT32 BscCoreNum;
  UINT32 BscSocket;
  UINT32 IgnoredModule;
  UINT32 ApicIdOfCore;
  AGESA_STATUS IgnoredSts;
  IDS_AP_RUN_CODE_TIMEPOINT TimePoint;

  TimePoint = PIdsRuncodeParams->TimePoint;
  ASSERT ((TimePoint == IDS_AP_RUN_CODE_EARLY) ||
          (TimePoint == IDS_AP_RUN_CODE_POST) ||
          (TimePoint == IDS_AP_RUN_CODE_LATE));

  IdentifyCore (StdHeader, &BscSocket, &IgnoredModule, &BscCoreNum, &IgnoredSts);
  IdsGetStartEndSocket (PIdsRuncodeParams->Socket, &StartSocket, &EndSocket);
  //TaskPtr for both IDS_AP_RUN_CODE_EARLY, IDS_AP_RUN_CODE_POST
  ApTask.FuncAddress.PfApTaskIO = PIdsRuncodeParams->ApTask;
  ApTask.ExeFlags = WAIT_FOR_CORE;
  ApTask.DataTransfer.DataSizeInDwords = PIdsRuncodeParams->ParamsDataSizeInDwords;
  ApTask.DataTransfer.DataPtr = PIdsRuncodeParams->ParamsDataPtr;
  ApTask.DataTransfer.DataTransferFlags = 0;

  for (Socket = StartSocket; Socket <= EndSocket; Socket++) {
    if (GetActiveCoresInGivenSocket (Socket, &NumberOfCores, StdHeader)) {
      if (PIdsRuncodeParams->Core == IDS_ALL_CORE) {
        StartCore = 0;
        EndCore = (UINT8)NumberOfCores - 1;
      } else {
        StartCore = PIdsRuncodeParams->Core;
        EndCore = PIdsRuncodeParams->Core;
      }
      for (Core = StartCore; (Core <= EndCore) && (Core <= (NumberOfCores - 1)); Core++) {
        if ((Core == BscCoreNum) && (Socket == BscSocket)) {
          //Call function directly
          PIdsRuncodeParams->ApTask (PIdsRuncodeParams->ParamsDataPtr, StdHeader);
        } else {
          if (IsProcessorPresent (Socket, StdHeader)) {
            if (TimePoint == IDS_AP_RUN_CODE_EARLY) {
              // At early stage, the AP's task has to be called by core 0, not by bsc
              IdsRunCodeOnCoreEarly (Socket, Core, &ApTask, StdHeader);
            } else if (TimePoint == IDS_AP_RUN_CODE_POST) {
              ApUtilRunCodeOnSocketCore (Socket, Core, &ApTask, StdHeader);
            } else if (TimePoint == IDS_AP_RUN_CODE_LATE) {
              IdsLateTask.ApTask = (PF_IDS_AP_TASK)PIdsRuncodeParams->ApTask;
              IdsLateTask.ApTaskPara = PIdsRuncodeParams->ParamsDataPtr;
              GetLocalApicIdForCore (Socket, Core, &ApicIdOfCore, StdHeader);
              IdsAgesaRunFcnOnApLate (ApicIdOfCore, &IdsLateTask, StdHeader);
            }
          }
        }
      }
    }
  }
}

/**
 *  Get the number of P-State to support
 *
 *  @param[in,out] StdHeader The Pointer of AMD_CONFIG_PARAMS.
 *
 *  @retval num    The number of P-State to support.
 *
 **/
UINT8
IdsGetNumPstatesFamCommon (
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8 pstatesnum;
  UINT8 i;
  UINT8 IgnoredByte;
  UINT32 Ignored;
  BOOLEAN PStateEnabled;
  UINT32 TempVar_c;
  PSTATE_CPU_FAMILY_SERVICES  *FamilyServices;

  pstatesnum = 0;
  GetFeatureServicesOfCurrentCore (&PstateFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
  ASSERT (FamilyServices != NULL);

  FamilyServices->GetPstateMaxState (FamilyServices, &TempVar_c, &IgnoredByte, StdHeader);
  for (i = 0; i <= TempVar_c; i++) {
    // Check if PState is enabled
    FamilyServices->GetPstateRegisterInfo (        FamilyServices,
                                                   (UINT32) i,
                                                   &PStateEnabled,
                                                   &Ignored,
                                                   &Ignored,
                                                   &Ignored,
                                                   StdHeader);
    if (PStateEnabled) {
      pstatesnum++;
    }
  }
  return pstatesnum;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Runs the given task on all cores (including self) on the socket of the executing
 * core 0.
 *
 * This function is used to invoke all APs on the socket of the executing core 0 to
 * run a specified AGESA procedure.
 *
 * @param[in]  TaskPtr      Function descriptor
 * @param[in]  StdHeader    Config handle for library and services
 *
 */
VOID
IdsApRunCodeOnAllLocalCores (
  IN       AP_TASK *TaskPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 Core;
  UINT32 BscCoreNum;
  UINT32 Socket;
  UINT32 BscSocket;
  UINT32 IgnoredModule;
  UINT32 NumberOfCores;
  UINT32 NumberOfSockets;
  AGESA_STATUS IgnoredSts;

  IdentifyCore (StdHeader, &BscSocket, &IgnoredModule, &BscCoreNum, &IgnoredSts);
  NumberOfSockets = GetPlatformNumberOfSockets ();


  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (GetActiveCoresInGivenSocket (Socket, &NumberOfCores, StdHeader)) {
      for (Core = 0; Core < NumberOfCores; Core++) {
        if ((Socket != (UINT32) BscSocket) || (Core != (UINT32) BscCoreNum)) {
          ApUtilRunCodeOnSocketCore ((UINT8) Socket, (UINT8) Core, TaskPtr, StdHeader);
        }
      }
    }
  }
 // BSP codes
  ApUtilTaskOnExecutingCore (TaskPtr, StdHeader, NULL);
}

/**
 *  IdsMakePciRegEntry
 *
 *
 *  @param[in,out]      TableEntry      The Pointer of TableEntry
*  @param[in]      Family      Family
*  @param[in]      Revision      Revision
*  @param[in]      PciAddr      PCI address
*  @param[in]      Data      Or Mask
*  @param[in]      Mask      And Mask
 *
 *
 */
VOID
IdsMakePciRegEntry (
  IN OUT   TABLE_ENTRY_FIELDS **TableEntry,
  IN       UINT64 Family,
  IN       UINT64 Revision,
  IN       UINT32 PciAddr,
  IN       UINT32 Data,
  IN       UINT32 Mask
  )
{
  (*TableEntry)->EntryType = PciRegister;
  (*TableEntry)->CpuRevision.Family = Family;
  (*TableEntry)->CpuRevision.Revision = Revision;
  (*TableEntry)->Features.PlatformValue = AMD_PF_ALL;
  (*TableEntry)->Entry.PciEntry.Address.AddressValue = PciAddr;
  (*TableEntry)->Entry.PciEntry.Data = Data;
  (*TableEntry)->Entry.PciEntry.Mask = Mask;
  (*TableEntry)++;
}

/**
 *  IdsMakeHtLinkPciRegEntry
 *
 *
 *  @param[in,out]      TableEntry      The Pointer of TableEntry
*  @param[in]      Family      Family
*  @param[in]      Revision      Revision
*  @param[in]      HtHostFeat      HtHostFeat
*  @param[in]      PciAddr      PCI address
*  @param[in]      Data      Or Mask
*  @param[in]      Mask      And Mask
 *
 *
 */
VOID
IdsMakeHtLinkPciRegEntry (
  IN OUT   TABLE_ENTRY_FIELDS **TableEntry,
  IN       UINT64 Family,
  IN       UINT64 Revision,
  IN       UINT32 HtHostFeat,
  IN       UINT32 PciAddr,
  IN       UINT32 Data,
  IN       UINT32 Mask
  )
{
  (*TableEntry)->EntryType = HtLinkPciRegister;
  (*TableEntry)->CpuRevision.Family = Family;
  (*TableEntry)->CpuRevision.Revision = Revision;
  (*TableEntry)->Features.PlatformValue = AMD_PF_ALL;
  (*TableEntry)->Entry.HtLinkPciEntry.LinkFeats.HtHostValue = HtHostFeat;
  (*TableEntry)->Entry.HtLinkPciEntry.PciEntry.Address.AddressValue = PciAddr;
  (*TableEntry)->Entry.HtLinkPciEntry.PciEntry.Data = Data;
  (*TableEntry)->Entry.HtLinkPciEntry.PciEntry.Mask = Mask;
  (*TableEntry)++;
}
/**
 *  IdsMakeHtFeatPciRegEntry
 *
 *
 *  @param[in,out]      TableEntry      The Pointer of TableEntry
*  @param[in]      Family      Family
*  @param[in]      Revision      Revision
*  @param[in]      HtHostFeat      HtHostFeat
*  @param[in]      PackageType      PackageType
*  @param[in]      PciAddr      PCI address
*  @param[in]      Data      Or Mask
*  @param[in]      Mask      And Mask
 *
 *
 */
VOID
IdsMakeHtFeatPciRegEntry (
  IN OUT   TABLE_ENTRY_FIELDS **TableEntry,
  IN       UINT64 Family,
  IN       UINT64 Revision,
  IN       UINT32 HtHostFeat,
  IN       UINT32 PackageType,
  IN       UINT32 PciAddr,
  IN       UINT32 Data,
  IN       UINT32 Mask
  )
{
  (*TableEntry)->EntryType = HtFeatPciRegister;
  (*TableEntry)->CpuRevision.Family = Family;
  (*TableEntry)->CpuRevision.Revision = Revision;
  (*TableEntry)->Features.PlatformValue = AMD_PF_ALL;
  (*TableEntry)->Entry.HtFeatPciEntry.LinkFeats.HtHostValue = HtHostFeat;
  (*TableEntry)->Entry.HtFeatPciEntry.PackageType.PackageTypeValue = PackageType;
  (*TableEntry)->Entry.HtFeatPciEntry.PciEntry.Address.AddressValue = PciAddr;
  (*TableEntry)->Entry.HtFeatPciEntry.PciEntry.Data = Data;
  (*TableEntry)->Entry.HtFeatPciEntry.PciEntry.Mask = Mask;
  (*TableEntry)++;
}
/**
 *  IdsMakeHostPciRegEntry
 *
 *
 *  @param[in,out]      TableEntry      The Pointer of TableEntry
*  @param[in]      Family      Family
*  @param[in]      Revision      Revision
*  @param[in]      HtHostFeat      HtHostFeat
*  @param[in]      PciAddr      PCI address
*  @param[in]      Data      Or Mask
*  @param[in]      Mask      And Mask
 *
 *
 */
VOID
IdsMakeHtHostPciRegEntry (
  IN OUT   TABLE_ENTRY_FIELDS **TableEntry,
  IN       UINT64 Family,
  IN       UINT64 Revision,
  IN       UINT32 HtHostFeat,
  IN       UINT32 PciAddr,
  IN       UINT32 Data,
  IN       UINT32 Mask
  )
{
  (*TableEntry)->EntryType = HtHostPciRegister;
  (*TableEntry)->CpuRevision.Family = Family;
  (*TableEntry)->CpuRevision.Revision = Revision;
  (*TableEntry)->Features.PlatformValue = AMD_PF_ALL;
  (*TableEntry)->Entry.HtHostEntry.TypeFeats.HtHostValue = HtHostFeat;
  (*TableEntry)->Entry.HtHostEntry.Address.AddressValue = PciAddr;
  (*TableEntry)->Entry.HtHostEntry.Data = Data;
  (*TableEntry)->Entry.HtHostEntry.Mask = Mask;
  (*TableEntry)++;
}
/**
 *  IdsMakeHtPhyRegEntry
 *
 *
 *  @param[in,out]      TableEntry      The Pointer of TableEntry
*  @param[in]      Family      Family
*  @param[in]      Revision      Revision
*  @param[in]      HtPhyLinkFeat      HtPhyLinkFeat
*  @param[in]      Address      PCI address
*  @param[in]      Data      Or Mask
*  @param[in]      Mask      And Mask
 *
 *
 */
VOID
IdsMakeHtPhyRegEntry (
  IN OUT   TABLE_ENTRY_FIELDS **TableEntry,
  IN       UINT64 Family,
  IN       UINT64 Revision,
  IN       UINT32 HtPhyLinkFeat,
  IN       UINT32 Address,
  IN       UINT32 Data,
  IN       UINT32 Mask
  )
{
  (*TableEntry)->EntryType = HtPhyRegister;
  (*TableEntry)->CpuRevision.Family = Family;
  (*TableEntry)->CpuRevision.Revision = Revision;
  (*TableEntry)->Features.PlatformValue = AMD_PF_ALL;
  (*TableEntry)->Entry.HtPhyEntry.TypeFeats.HtPhyLinkValue = HtPhyLinkFeat;
  (*TableEntry)->Entry.HtPhyEntry.Address = Address;
  (*TableEntry)->Entry.HtPhyEntry.Data = Data;
  (*TableEntry)->Entry.HtPhyEntry.Mask = Mask;
  (*TableEntry)++;
}

/**
 *  IdsOptionCallout
 *
 * Description
 *    Call the host environment interface to provide a user hook opportunity.
 *
 *  @param[in]      CallOutId         This parameter indicates the IDS Call-Out-function desired.
 *  @param[in,out]  DataPtr           The pointer for callout function use
 *  @param[in,out]  StdHeader    Config handle for library and services
 *
 *  @retval AGESA_SUCCESS       Success
 *  @retval AGESA_ERROR         meet some error
 *
 */
AGESA_STATUS
IdsOptionCallout (
  IN       UINTN                CallOutId,
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  IDS_CALLOUT_STRUCT IdsCalloutData;
  IDS_NV_ITEM NullEntry;

  NullEntry.IdsNvId = 0xFFFF;
  NullEntry.IdsNvValue = 0xFFFF;
  IdsCalloutData.StdHeader = *StdHeader;
  IdsCalloutData.IdsNvPtr = &NullEntry;
  IdsCalloutData.Reserved = (UINTN) DataPtr;

  return AgesaGetIdsData (CallOutId, &IdsCalloutData);

}

/**
 * Ids Write PCI register to All node
 *
 *
 * @param[in] PciAddress    Pci address
 * @param[in]   Highbit       High bit position of the field in DWORD
 * @param[in]   Lowbit        Low bit position of the field in DWORD
 * @param[in] Value         Pointer to input value
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
IdsLibPciWriteBitsToAllNode (
  IN       PCI_ADDR PciAddress,
  IN       UINT8 Highbit,
  IN       UINT8 Lowbit,
  IN       UINT32 *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 Socket;
  UINT32 Module;
  AGESA_STATUS IgnoreStatus;
  PCI_ADDR PciAddr;


  for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
    for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
      if (GetPciAddress (StdHeader, Socket, Module, &PciAddr, &IgnoreStatus)) {
        PciAddr.Address.Function = PciAddress.Address.Function;
        PciAddr.Address.Register = PciAddress.Address.Register;
        LibAmdPciWriteBits (PciAddr, Highbit, Lowbit, Value, StdHeader);
      }
    }
  }
}

/**
 *
 *
 *     Core 0 task to run local ap task at early
 *
 *     @param[in]   PEarlyApTask   - point to  IDS_EARLY_AP_TASK structure
 *     @param[in,out]   StdHeader    - The Pointer of AGESA Header
 *
 */

STATIC VOID
IdsCmnTaskCore0Early (
  IN       IDS_EARLY_AP_TASK *PEarlyApTask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 Socket;
  UINT32 IgnoredModule;
  UINT32 IgnoredCore;
  AGESA_STATUS IgnoredSts;

  ASSERT (PEarlyApTask->Ap_Task0.Core != 0);

  PEarlyApTask->Ap_Task0.ApTask.DataTransfer.DataPtr = &PEarlyApTask->Parameters[0];
  IdentifyCore (StdHeader, &Socket, &IgnoredModule, &IgnoredCore, &IgnoredSts);
  ApUtilRunCodeOnSocketCore ((UINT8)Socket, PEarlyApTask->Ap_Task0.Core, &PEarlyApTask->Ap_Task0.ApTask, StdHeader);
}

/**
 *
 *
 *     BSC task to run Core0 task at early, must only run on BSC
 *
 *     @param[in]   Socket   - Socket which run the task
 *     @param[in]   Core   - Core which run the task
 *     @param[in]   ApTask   - Task for AP
 *     @param[in,out]   StdHeader    - The Pointer of AGESA Header
 *
 */
VOID
IdsRunCodeOnCoreEarly (
  IN       UINT8 Socket,
  IN       UINT8 Core,
  IN       AP_TASK* ApTask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 BscSocket;
  UINT32 BscCoreNum;
  UINT32 IgnoredModule;
  AGESA_STATUS IgnoredSts;
  AP_TASK Core0Task;
  IDS_EARLY_AP_TASK IdsEarlyTask;

  IdentifyCore (StdHeader, &BscSocket, &IgnoredModule, &BscCoreNum, &IgnoredSts);
  ASSERT (~((Socket == BscSocket) && (Core == BscCoreNum)));
  if ((Socket == BscSocket) || (Core == 0)) {
    ApUtilRunCodeOnSocketCore (Socket, Core, ApTask, StdHeader);
  } else {
    //Init IDS_EARLY_AP_TASK for Core 0
    IdsEarlyTask.Ap_Task0.ApTask = *ApTask;
    IdsEarlyTask.Ap_Task0.Core = Core;
    //Init Parameter buffer, Target core can't get the parameter from pointer, which point to Host Core memory space
    ASSERT ((ApTask->DataTransfer.DataSizeInDwords * sizeof (UINT32)) <= IDS_EARLY_AP_TASK_PARA_NUM);
    LibAmdMemCopy (&IdsEarlyTask.Parameters[0], ApTask->DataTransfer.DataPtr, sizeof (UINT32) * ApTask->DataTransfer.DataSizeInDwords, StdHeader);
    if ((ApTask->DataTransfer.DataSizeInDwords * sizeof (UINT32)) <= IDS_EARLY_AP_TASK_PARA_NUM) {
      //Lauch Core0 1st
      Core0Task.FuncAddress.PfApTaskI = (PF_AP_TASK_I)IdsCmnTaskCore0Early;
      Core0Task.ExeFlags = WAIT_FOR_CORE;
      Core0Task.DataTransfer.DataSizeInDwords = SIZE_IN_DWORDS (IDS_EARLY_AP_TASK0) + ApTask->DataTransfer.DataSizeInDwords;
      Core0Task.DataTransfer.DataPtr = &IdsEarlyTask;
      Core0Task.DataTransfer.DataTransferFlags = 0;
      ApUtilRunCodeOnSocketCore (Socket, 0, &Core0Task, StdHeader);
    }
  }
}

/**
 *
 *
 *     This function get start end Module according to input ModuleId
 *
 *     @param[in]   ModuleId   - 0xFF means all nodes, other value Specifies real NodeId
 *     @param[in,out]   StartModule    - Point to start Node
 *     @param[in,out]   EndModule   - Point to end Node
 *
 */
VOID
IdsGetStartEndModule (
  IN       UINT8  ModuleId,
  IN OUT   UINT8 *StartModule,
  IN OUT   UINT8 *EndModule
  )
{
  if (ModuleId == 0xFF) {
    *StartModule = 0;
    *EndModule = (UINT8) (GetPlatformNumberOfSockets () * GetPlatformNumberOfModules () - 1);
    if (*EndModule > 7) {
      *EndModule = 7;
    }
  } else {
    *StartModule = ModuleId;
    *EndModule = ModuleId;
  }
}

/**
 *
 *
 *     This function get start end socket according to input SocketId
 *
 *     @param[in]   SocketId   - 0xFF means all sockets, other value Specifies real SokcetId
 *     @param[in,out]   StartSocket    - Point to start Socket
 *     @param[in,out]   EndSocket   - Point to end Socket
 *
 */
VOID
IdsGetStartEndSocket (
  IN       UINT8  SocketId,
  IN OUT   UINT8 *StartSocket,
  IN OUT   UINT8 *EndSocket
  )
{
  if (SocketId == IDS_ALL_SOCKET) {
    *StartSocket = 0;
    *EndSocket = (UINT8) (GetPlatformNumberOfSockets () - 1);
  } else {
    *StartSocket = SocketId;
    *EndSocket = SocketId;
  }
}

/**
 *
 *
 *     This function transfer input High low bit to Mask
 *
 *     @param[in]   RegVal   - Regval want to set
 *     @param[in]   Highbit   - (0~63)
 *     @param[in]   Lowbit    - (0~63)
 *     @param[in,out]   AndMask   - point value contain output AndMask
 *     @param[in,out]   OrMask   - point value contain output OrMask
 *
 */
VOID
IdsGetMask64bits (
  IN       UINT64 RegVal,
  IN       UINT8  Highbit,
  IN       UINT8  Lowbit,
  IN OUT   UINT64 *AndMask,
  IN OUT   UINT64 *OrMask
  )
{
  UINT64 Mask;

  if ((Highbit - Lowbit) != 63) {
    Mask = (((UINT64) 1 << (Highbit - Lowbit + 1)) - 1);
  } else {
    Mask = (UINT64) 0xFFFFFFFFFFFFFFFF;
  }
  *AndMask = ~(Mask << Lowbit);
  *OrMask = (RegVal & Mask) << Lowbit;
}
/**
 *
 *
 *     This function transfer input High low bit to Mask
 *
 *     @param[in]   RegVal   - Regval want to set
 *     @param[in]   Highbit   - (0~31)
 *     @param[in]   Lowbit    - (0~31)
 *     @param[in,out]   AndMask   - point value contain output AndMask
 *     @param[in,out]   OrMask   - point value contain output OrMask
 *
 */

VOID
IdsGetMask32bits (
  IN       UINT32 RegVal,
  IN       UINT8  Highbit,
  IN       UINT8  Lowbit,
  IN OUT   UINT32 *AndMask,
  IN OUT   UINT32 *OrMask
  )
{
  UINT32 Mask;

  if ((Highbit - Lowbit) != 31) {
    Mask = (((UINT32) 1 << (Highbit - Lowbit + 1)) - 1);
  } else {
    Mask = (UINT32) 0xFFFFFFFF;
  }
  *AndMask = ~(Mask << Lowbit);
  *OrMask = (RegVal & Mask) << Lowbit;

}
/**
 *
 *
 *     This function transfer input High low bit to Mask
 *
 *     @param[in]   RegVal   - Regval want to set
 *     @param[in]   Highbit   - (0~15)
 *     @param[in]   Lowbit    - (0~15)
 *     @param[in,out]   AndMask   - point value contain output AndMask
 *     @param[in,out]   OrMask   - point value contain output OrMask
 *
 */

VOID
IdsGetMask16bits (
  IN       UINT16 RegVal,
  IN       UINT8  Highbit,
  IN       UINT8  Lowbit,
  IN OUT   UINT32 *AndMask,
  IN OUT   UINT32 *OrMask
  )
{
  UINT16 Mask;

  if ((Highbit - Lowbit) != 15) {
    Mask = (((UINT16) 1 << (Highbit - Lowbit + 1)) - 1);
  } else {
    Mask = (UINT16) 0xFFFF;
  }
  *AndMask = ~(Mask << Lowbit);
  *OrMask = (RegVal & Mask) << Lowbit;
}

/**
 *
 *
 *     IdsCheckPciExisit
 *     Use to check is the PCI device exisit of given address
 *
 *     @param[in]   PciAddr   - Given PCI address
 *     @param[in,out]   StdHeader    - The Pointer of AGESA Header
 *
 *     @retval TRUE         The PCI device exisit
 *     @retval FALSE        The PCI device doesn't exisit
 *
 *
 */
BOOLEAN
IdsCheckPciExisit (
  IN       PCI_ADDR PciAddr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  PCI_ADDR _pciaddr;
  UINT32 _pcidata;
  BOOLEAN status;

  status = FALSE;
  _pciaddr = PciAddr;
  _pciaddr.Address.Register = 0;
  LibAmdPciRead (AccessWidth32, _pciaddr, &_pcidata, StdHeader);
  if (_pcidata != 0xFFFFFFFF || _pcidata != 0) {
    status = TRUE;
  }
  return status;
}

/**
 *
 *
 *     This function transfer input High low bit to Mask
 *
 *     @param[in,out]   Value   - Regval want to And Or with Mask
 *     @param[in]   AndMask   - AndMask
 *     @param[in]   OrMask   - OrMask
 *
 */

VOID
IdsLibDataMaskSet32 (
  IN OUT   UINT32 *Value,
  IN       UINT32  AndMask,
  IN       UINT32  OrMask
  )
{
  *Value &= AndMask;
  *Value |= OrMask;
}



