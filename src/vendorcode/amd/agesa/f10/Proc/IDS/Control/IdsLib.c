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
 * @e \$Revision  $   @e \$Date  $
 */
/*****************************************************************************
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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
#include "Ids.h"
#include "amdlib.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "IdsLib.h"
#include "heapManager.h"

#include "mm.h"
#include "mn.h"
#include "cpuLateInit.h"
#include "Filecode.h"
#define FILECODE PROC_IDS_CONTROL_IDSLIB_FILECODE

/*----------------------------------------------------------------------------
 *                           EXPORTED FUNCTIONS
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
 *  @retval AGESA_ERROR         Fail to get the pointer of NV Table.
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
 *
 * @retval IDS_UNSUPPORTED  NV ID is not found in the table
 *         Other Value      The NV value
 *
 **/
IDS_STATUS
AmdIdsNvReader (
  IN      UINT16 IdsNvId,
  IN      IDS_NV_ITEM *NvTablePtr
  )
{
  IDS_STATUS Status;
  IDS_NV_ITEM *NvPtr;

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
    if ((NvPtr->IdsNvId != AGESA_IDS_NV_END) && (NvPtr->IdsNvValue != AGESA_IDS_DFT_VAL)) {
      Status = NvPtr->IdsNvValue;
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
 *
 *  set the MSR of AP
 *
 *  @param[in] PRegMsr  point to REG_MSR
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *
 *  @retval AGESA_SUCCESS       Success
 *  @retval AGESA_ERROR         meet some error
 *
 **/
AGESA_STATUS
AMDSetAPMsr (
  IN       REG_MSR  *PRegMsr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 value;

  LibAmdMsrRead (PRegMsr->msraddr, &value, StdHeader);
  value &= ((UINT64)PRegMsr->andmaskhi << 32) | ((UINT64)PRegMsr->andmasklo);
  value |= ((UINT64)PRegMsr->ormaskhi << 32) | ((UINT64)PRegMsr->ormasklo);
  LibAmdMsrWrite (PRegMsr->msraddr, &value, StdHeader);

  return AGESA_SUCCESS;
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
  UINT32 IddVal;
  UINT32 IddDiv;
  BOOLEAN PStateEnabled;
  UINT32 TempVar_c;
  CPU_SPECIFIC_SERVICES   *FamilySpecificServices;

  pstatesnum = 0;
  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);

  FamilySpecificServices->GetPstateMaxState (FamilySpecificServices, &TempVar_c, StdHeader);
  for (i = 0; i <= TempVar_c; i++) {
    // Check if PState is enabled
    FamilySpecificServices->GetPstateRegisterInfo (FamilySpecificServices,
                                                   (UINT32) i,
                                                   &PStateEnabled,
                                                   &IddVal,
                                                   &IddDiv,
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
  UINT32 BscCore;
  UINT32 Socket;
  UINT32 BscSocket;
  UINT32 IgnoredModule;
  UINT32 NumberOfCores;
  UINT32 NumberOfSockets;
  AGESA_STATUS IgnoredSts;

  IdentifyCore (StdHeader, &BscSocket, &IgnoredModule, &BscCore, &IgnoredSts);
  NumberOfSockets = GetPlatformNumberOfSockets ();


  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (GetActiveCoresInGivenSocket (Socket, &NumberOfCores, StdHeader)) {
      for (Core = 0; Core < NumberOfCores; Core++) {
        if ((Socket != (UINT32) BscSocket) || (Core != (UINT32) BscCore)) {
          ApUtilRunCodeOnSocketCore ((UINT8) Socket, (UINT8) Core, TaskPtr, StdHeader);
        }
      }
    }
  }
 // BSP codes
  ApUtilTaskOnExecutingCore (TaskPtr, StdHeader, NULL);
}

