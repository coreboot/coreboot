/**
 * @file
 *
 * AMD Integrated Debug Option Backend Routines
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
#include "Topology.h"
#include "htFeat.h"
#include "IdsHt.h"
#include "amdlib.h"
#include "mm.h"
#include "mn.h"
#include "cpuRegisters.h"
#include "heapManager.h"
#include "cpuFamilyTranslation.h"
#include "GeneralServices.h"
#include "IdsLib.h"
#include "Filecode.h"
#define FILECODE PROC_IDS_CONTROL_IDSCTRL_FILECODE

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


extern CONST IDS_FEAT_STRUCT* ROMDATA IdsCommonFeats[];


/**
 * IDS option hooking function dispatcher.
 *
 * This is the top level interface for IDS option Backend code.
 *
 * @param[in]     IdsOption IDS indicator value, see @ref AGESA_IDS_OPTION
 * @param[in,out] DataPtr   Data pointer.
 * @param[in,out] StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 **/
IDS_STATUS
AmdIdsCtrlDispatcher (
  IN       AGESA_IDS_OPTION IdsOption,
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  IDS_NV_ITEM *IdsNvPtr;
  IDS_STATUS   ReturnFlag;
  IDS_STATUS ExtendedFlag;

  IdsNvPtr  = NULL;
  ReturnFlag = IDS_SUCCESS;

  ASSERT (StdHeader != NULL);
  if (AmdGetIdsNvTable ((VOID **)&IdsNvPtr, StdHeader) != AGESA_SUCCESS) {
    AmdIdsCtrlInitialize (StdHeader);
    AmdGetIdsNvTable ((VOID **)&IdsNvPtr, StdHeader);
  }

  if (IdsNvPtr != NULL) {
    ReturnFlag = IdsParseFeatTbl (IdsOption, IdsCommonFeats, DataPtr, IdsNvPtr, StdHeader);
    ExtendedFlag = IDS_EXTENDED_HOOK (IdsOption, DataPtr, IdsNvPtr, StdHeader);
    if (ExtendedFlag != IDS_SUCCESS) {
      ReturnFlag = IDS_UNSUPPORTED;
    }
  }
  return ReturnFlag;
}

/**
 * Ids code for parse IDS feat table.
 *
 * Feat table in IDS is used to decribe the IDS support feat and its according family,handler.
 *
 * @param[in]     PIdsFeatTbl point to Ids Feat table
 * @param[in]     IdsOption IDS indicator value, see @ref AGESA_IDS_OPTION
 * @param[in,out] DataPtr   Data pointer.
 * @param[in]     IdsNvPtr   Ids Nvram pointer.
 * @param[in,out] StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 **/
IDS_STATUS
IdsParseFeatTbl (
  IN       AGESA_IDS_OPTION IdsOption,
  IN       CONST IDS_FEAT_STRUCT * PIdsFeatTbl[],
  IN OUT   VOID *DataPtr,
  IN       IDS_NV_ITEM *IdsNvPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT16 i;
  AGESA_STATUS Tmpsts;
  CPU_LOGICAL_ID CpuLogicalId;
  BOOLEAN No_Check_Bsp;
  CONST IDS_FEAT_STRUCT *PIdsFeat;
  IDS_STATUS ReturnFlag;
  IDS_STATUS status;

  status = IDS_SUCCESS;
  ReturnFlag = IDS_SUCCESS;

  for (i = 0; PIdsFeatTbl[i] != NULL; i++) {
    PIdsFeat = PIdsFeatTbl[i];
    //Does specified IdsOption reached
    if (PIdsFeat->IdsOption == IdsOption) {
      //check if bsp only
      if (PIdsFeat->IsBsp) {
        No_Check_Bsp = 0;
      } else {
        No_Check_Bsp = 1;
      }
      if (No_Check_Bsp || IsBsp (StdHeader, &Tmpsts)) {
        //Does Family Match required
        GetLogicalIdOfCurrentCore (&CpuLogicalId, StdHeader);
        if ((CpuLogicalId.Family) & (PIdsFeat->CpuFamily)) {
          //Excute the code for specific Ids Feat
          status = PIdsFeat->pf_idsoption (DataPtr, StdHeader, IdsNvPtr);
          if (status != IDS_SUCCESS) {
            ReturnFlag = status;
          }
        }
      }
    }
  }
  return ReturnFlag;
}

/**
 *
 *  IDS Object Initialization
 *
 *  Initializer routine that will be invoked by the wrapper to initialize
 *  the data buffer in heap for the IDS object. It includes IDS control
 *  structure, IDS mem table and IDS GRA table.
 *
 *  @param[in,out] StdHeader    The Pointer of IDS Initial Parameter
 *
 *  @retval AGESA_SUCCESS       Success to init IDS Object.
 *  @retval AGESA_ERROR         Fail to init IDS Object.
 *
 **/

#ifdef IDS_HEAP_2STAGES
  #define IDS_HEAP_PERSIST_EARLY  HEAP_LOCAL_CACHE
  #define IDS_HEAP_ASSERTION_LATE
#else
  #define IDS_HEAP_PERSIST_EARLY  HEAP_SYSTEM_MEM
  #define IDS_HEAP_ASSERTION_LATE ASSERT(FALSE)
#endif

AGESA_STATUS
AmdIdsCtrlInitialize (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AGESA_STATUS status;
  UINT16 NvTblSize;
  UINT16 i;
  IDS_NV_ITEM IdsNvTable[IDS_NUM_NV_ITEM];
  IDS_NV_ITEM *NvTable;
  IDS_NV_ITEM *NvPtr;
  IDS_CONTROL_STRUCT *IdsCtrlPtr;
  IDS_CALLOUT_STRUCT IdsCalloutData;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  UINT16 MemTblSize;
  UINT8 HeapPersist;

  NvTblSize = 0;
  MemTblSize = 0;
  HeapPersist = HEAP_SYSTEM_MEM;
  //Heap status with HEAP_LOCAL_CACHE, will allocate heap with HEAP_LOCAL_CACHE
  //with HEAP_TEMP_MEM  HEAP_SYSTEM_MEM  HEAP_DO_NOT_EXIST_ANYMORE  HEAP_S3_RESUME
  // with allocate with HEAP_SYSTEM_MEM
  if (StdHeader->HeapStatus == HEAP_LOCAL_CACHE) {
    MemTblSize = IDS_MAX_MEM_ITEMS;
    HeapPersist = IDS_HEAP_PERSIST_EARLY;
  } else if ((StdHeader->HeapStatus == HEAP_DO_NOT_EXIST_YET) || (StdHeader->HeapStatus == HEAP_DO_NOT_EXIST_ANYMORE)) {
    return AGESA_ERROR;
  } else {
    IDS_HEAP_ASSERTION_LATE;
  }

  IdsCalloutData.IdsNvPtr = IdsNvTable;
  IdsCalloutData.StdHeader = *StdHeader;
//init IDS_CALLOUT_STRUCT before calling out, give NVITEM default value
  for (i = AGESA_IDS_EXT_ID_START; i < IDS_NUM_NV_ITEM; i++) {
    IdsNvTable[i].IdsNvId = i;
    IdsNvTable[i].IdsNvValue = AGESA_IDS_DFT_VAL;
  }

  AGESA_TESTPOINT (TpIfBeforeGetIdsData, StdHeader);
  if (AgesaGetIdsData (IDS_CALLOUT_INIT, &IdsCalloutData) == AGESA_SUCCESS) {
    NvTable = IdsCalloutData.IdsNvPtr;
    NvPtr = NvTable;
    while (NvPtr->IdsNvId != AGESA_IDS_NV_END) {
      NvTblSize  ++;
      NvPtr ++;
    }
    NvTblSize  ++;

    AllocHeapParams.RequestedBufferSize = sizeof (IDS_CONTROL_STRUCT);
    AllocHeapParams.RequestedBufferSize += NvTblSize  * sizeof (IDS_NV_ITEM);
    AllocHeapParams.RequestedBufferSize += MemTblSize * sizeof (MEM_TABLE_ALIAS);
    AllocHeapParams.RequestedBufferSize += IDS_EXTENDED_HEAP_SIZE;
    AllocHeapParams.BufferHandle = IDS_CONTROL_HANDLE;
    AllocHeapParams.Persist = HeapPersist;

    //
    // Allocate data buffer in heap
    //
    if (HeapAllocateBuffer (&AllocHeapParams, (AMD_CONFIG_PARAMS *) StdHeader) == AGESA_SUCCESS) {
      //
      // Initialize IDS Date Buffer
      //
      IdsCtrlPtr = (IDS_CONTROL_STRUCT *) AllocHeapParams.BufferPtr;
      IdsCtrlPtr->IdsHeapMemSize = AllocHeapParams.RequestedBufferSize;
      IdsCtrlPtr->IdsNvTableOffset = sizeof (IDS_CONTROL_STRUCT);
      IdsCtrlPtr->IdsMemTableOffset = IdsCtrlPtr->IdsNvTableOffset + NvTblSize  * sizeof (IDS_NV_ITEM);
      IdsCtrlPtr->IdsExtendOffset = IdsCtrlPtr->IdsMemTableOffset + MemTblSize * sizeof (MEM_TABLE_ALIAS);

      NvPtr = (IDS_NV_ITEM *) (AllocHeapParams.BufferPtr + IdsCtrlPtr->IdsNvTableOffset);
      for (i = 0; i < NvTblSize ; i++) {
        NvPtr->IdsNvId = NvTable->IdsNvId;
        NvPtr->IdsNvValue = NvTable->IdsNvValue;
        NvPtr ++;
        NvTable ++;
      }
      status = AGESA_SUCCESS;
    } else {
      status = AGESA_ERROR;
    }
  } else {
    status = AGESA_ERROR;
  }
  AGESA_TESTPOINT (TpIfAfterGetIdsData, StdHeader);

  return status;
}
/**
 *  IDS Backend Function for Target Pstate
 *
 *
 *  @param[in,out]   DataPtr      The Pointer of AMD_CPU_EARLY_PARAMS.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubTargetPstate (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  IDS_STATUS tarpst;
  CPU_SPECIFIC_SERVICES   *FamilySpecificServices;

  IDS_NV_READ_SKIP (tarpst, AGESA_IDS_NV_TARGET_PSTATE, IdsNvPtr) {
    GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
    FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) tarpst, (BOOLEAN) FALSE, StdHeader);
  }
  return IDS_SUCCESS;
}

/**
 *  IDS Backend Function for HdtOut
 *
 *
 *  @param[in,out]   DataPtr      The Pointer of AMD_CPU_EARLY_PARAMS.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubHdtOut (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  IDS_STATUS idsvalue;
  AMD_EARLY_PARAMS *rptr;

  rptr = (AMD_EARLY_PARAMS *)DataPtr;
//set HDTOUT En/Dis
  IDS_NV_READ_SKIP (idsvalue, AGESA_IDS_NV_HDTOUT, IdsNvPtr) {
//if HDTOUT set to enable, set the corresponding DR2 flag to 0x99cc
    if (idsvalue == 1) {
      LibAmdWriteCpuReg (DR2_REG, 0x99cc);
    }
  }
  return IDS_SUCCESS;
}

/**
 *  IDS Backend Function for Power down mode
 *
 *  @param[in,out]   DataPtr      The Pointer of AMD_POST_PARAMS.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubPowerDownCtrl (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  AMD_POST_PARAMS *PostParamsPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  IDS_STATUS idsvalue;
  MEM_DATA_STRUCT * memdataptr;

  PostParamsPtr = (AMD_POST_PARAMS *)DataPtr;
  memdataptr = PostParamsPtr->MemConfig.MemData;
  RefPtr = memdataptr->ParameterListPtr;

  IDS_NV_READ_SKIP (idsvalue, AGESA_IDS_NV_MEMORY_POWER_DOWN, IdsNvPtr) {
    if (idsvalue < (IDS_STATUS)0x2) {
      RefPtr->EnablePowerDown = (BOOLEAN) idsvalue;
    }
  }
  return IDS_SUCCESS;
}

/**
 *  Backend Function for IDS Option Hook Point: IDS_UCODE
 *
 *  This function is used to disable UCode Installation if IDS Option disables ucode.
 *  The method is to force the number of total patches to ZERO.
 *
 *  @param[in,out]   DataPtr      The Pointer of Data to Override.
 *  @param[in,out]   StdHeader    The Pointer of AGESA Header.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubUCode (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  UINT8 *TotalPatches;

  TotalPatches = (UINT8 *) DataPtr;
  if (AmdIdsNvReader (AGESA_IDS_NV_UCODE, IdsNvPtr) == (IDS_STATUS)  0x0) {
    (*TotalPatches) = 0;
  }
  return IDS_SUCCESS;
}

/**
 *  IDS Backend Function for Post P-State
 *
 *  This function is used to set Post P-State which are CPU specifically.
 *
 *  @param[in,out]   DataPtr      The Pointer of AMD_CPU_EARLY_PARAMS.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubPostPState (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  AMD_CPU_EARLY_PARAMS *PCpuEarlyParams;
  IDS_STATUS idsvalue;
  UINT8 curpstatesnum;

  PCpuEarlyParams = (AMD_CPU_EARLY_PARAMS *)DataPtr;
  curpstatesnum = IdsGetNumPstatesFamCommon (StdHeader);
  idsvalue = AmdIdsNvReader (AGESA_IDS_NV_POSTPSTATE, IdsNvPtr);

  if (idsvalue < (IDS_STATUS) (curpstatesnum + 3)) {
    switch (idsvalue) {
    case (IDS_STATUS) 0x0:
      // Auto
      break;
    case (IDS_STATUS) 0x1:
      // Maximum Performance
      PCpuEarlyParams->MemInitPState = 0;
      break;
    case (IDS_STATUS) 0x2:
      // Minimum Performance
      PCpuEarlyParams->MemInitPState = curpstatesnum - 1;
      break;
    default:
      PCpuEarlyParams->MemInitPState = (UINT8) (idsvalue - 3);
      break;
    }
  }
  return IDS_SUCCESS;
}


/**
 *  IDS Backend Function for ECC symbol size
 *
 *  This function is used to override mem parameter
 *
 *  @param[in,out]   DataPtr      The Pointer of AMD_POST_PARAMS.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/

IDS_STATUS
IdsSubEccSymbolSize (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  BOOLEAN *peccsymbolsize;
  IDS_STATUS Status;
  peccsymbolsize = (BOOLEAN *)DataPtr;
  Status = AmdIdsNvReader (AGESA_IDS_NV_ECC_SYMBOL_SIZE, IdsNvPtr);
  if (Status != IDS_UNSUPPORTED) {
    if (Status == 0x1 || Status == 0x2) {
      *peccsymbolsize = (BOOLEAN)(Status - 1);
    }
  }
  return IDS_SUCCESS;
}

/**
 *  IDS Backend Function for Memory Mode Unganged
 *
 *  This function is used to override Memory Mode Unganged.
 *
 *  @param[in,out]   DataPtr      The Pointer of AMD_POST_PARAMS.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubGangingMode (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  IDS_STATUS GangingMode;
  IDS_NV_READ_SKIP (GangingMode, AGESA_IDS_NV_DCT_GANGING_MODE, IdsNvPtr) {
    *((UINT8 *)DataPtr) = (UINT8) GangingMode;
  }
  return IDS_SUCCESS;
}

/**
 *  IDS Backend Function for Power Down Mode
 *
 *  This function is used to override Power Down Mode.
 *
 *  @param[in,out]   DataPtr      The Pointer of AMD_POST_PARAMS.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubPowerDownMode (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  IDS_STATUS PowerDownMode;
  PowerDownMode = AmdIdsNvReader (AGESA_IDS_NV_MEMORY_POWER_DOWN_MODE, IdsNvPtr);
  if (PowerDownMode < (IDS_STATUS)0x2) {
    *((UINT8 *) DataPtr) = (UINT8)PowerDownMode;
  }
  return IDS_SUCCESS;
}

/**
 *  IDS Backend Function for Burst Length32
 *
 *  This function is used to override Burst Length32.
 *
 *  @param[in,out]   DataPtr      The Pointer of AMD_POST_PARAMS.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubBurstLength32 (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  IDS_STATUS BurstLength32;
  BurstLength32 = AmdIdsNvReader (AGESA_IDS_NV_DRAM_BURST_LENGTH32, IdsNvPtr);
  if (BurstLength32 < (IDS_STATUS)0x2) {
    *((UINT8 *) DataPtr) = (UINT8)BurstLength32;
  }
  return IDS_SUCCESS;
}

/**
 *  IDS Backend Function for All Memory Clks Enable
 *
 *  This function is used to override All Memory Clks Enable
 *
 *  @param[in,out]   DataPtr      The Pointer of AMD_POST_PARAMS.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubAllMemClkEn (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  IDS_STATUS AllMemClkEn;
  AllMemClkEn = AmdIdsNvReader (AGESA_IDS_NV_ALL_MEMCLKS , IdsNvPtr);
  if (AllMemClkEn < (IDS_STATUS)0x2) {
    *((UINT8 *) DataPtr) = (UINT8)AllMemClkEn;
  }
  return IDS_SUCCESS;
}

/**
 *  IDS Backend Function for Dll Shut Down
 *
 *  This function is used to override Dll Shut Down Option
 *
 *  @param[in,out]   DataPtr      The Pointer of AMD_POST_PARAMS.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubDllShutDownSR (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  IDS_STATUS DllShutDownSR;
  IDS_NV_READ_SKIP (DllShutDownSR, AGESA_IDS_NV_DLL_SHUT_DOWN , IdsNvPtr) {
    *((UINT8 *) DataPtr) = (UINT8)DllShutDownSR;
  }
  return IDS_SUCCESS;
}

/**
 *  IDS Backend Function for HT Link Configuration
 *
 *  Provide the nv settings to the HT code in the form of a port override list.
 *  Create the list on the heap, so the HT code doesn't have to keep asking for it.
 *
 *  @param[out] Data         A reference to the HT Port Override List
 *  @param[in]  StdHeader    Header for library and services.
 *  @param[in] IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 */
IDS_STATUS
IdsSubHtLinkControl (
     OUT   VOID  *Data,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  HTIDS_PORT_OVERRIDE_LIST   *ListReference;
  HTIDS_PORT_OVERRIDE_LIST   PortOverrideList;
  UINT32 HTlinkSocket;
  UINT32 HTlinkPort;
  UINT32 HTlinkFre;
  UINT32 HTlinkWidthIn;
  UINT32 HTlinkWidthOut;

  ASSERT (IdsNvPtr != NULL);
  ASSERT (Data != NULL);

  ListReference = Data;
  *ListReference = NULL;
  // Allocated the number of portlist override option sets supported (currently 1) plus 1 more for terminal.
  AllocHeapParams.RequestedBufferSize = (sizeof (HTIDS_PORT_OVERRIDE) * 2);
  AllocHeapParams.BufferHandle = IDS_HT_DATA_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
    PortOverrideList = (HTIDS_PORT_OVERRIDE_LIST) AllocHeapParams.BufferPtr;
    LibAmdMemFill (PortOverrideList, HT_LIST_TERMINAL, AllocHeapParams.RequestedBufferSize, StdHeader);
    *ListReference = PortOverrideList;

    HTlinkSocket = AmdIdsNvReader (AGESA_IDS_NV_HTLINKSOCKET, IdsNvPtr);
    if (HTlinkSocket != IDS_UNSUPPORTED) {
      switch (HTlinkSocket) {
      case (UINT32) 0xE:
        HTlinkSocket = 0xFE;
        break;
      case (UINT32) 0xF:
        HTlinkSocket = 0xFF;
        break;
      default:
        break;
      }
      PortOverrideList->Socket = (UINT8) HTlinkSocket;
    }
    HTlinkPort = AmdIdsNvReader (AGESA_IDS_NV_HTLINKPORT, IdsNvPtr);
    if (HTlinkPort != IDS_UNSUPPORTED) {
      switch (HTlinkPort) {
      case (UINT32) 0xC:
        HTlinkPort = 0xFC;
        break;
      case (UINT32) 0xD:
        HTlinkPort = 0xFD;
        break;
      case (UINT32) 0xE:
        HTlinkPort = 0xFE;
        break;
      case (UINT32) 0xF:
        HTlinkPort = 0xFF;
        break;
      default:
        break;
      }
      PortOverrideList->Link = (UINT8) HTlinkPort;
    }
    HTlinkFre = AmdIdsNvReader (AGESA_IDS_NV_HTLINKFREQ, IdsNvPtr);
    if (HTlinkFre != IDS_UNSUPPORTED) {
      switch (HTlinkFre) {
      case (UINT32) 0x1F:
        HTlinkFre = 0xFF;
        break;
      default:
        break;
      }
      PortOverrideList->Frequency = (UINT8) HTlinkFre;
    }
    HTlinkWidthIn = AmdIdsNvReader (AGESA_IDS_NV_HTLINKWIDTHIN , IdsNvPtr);
    if (HTlinkWidthIn != IDS_UNSUPPORTED) {
      switch (HTlinkWidthIn) {
      case (UINT32) 0x0:
        HTlinkWidthIn = 0x8;
        break;
      case (UINT32) 0x01:
        HTlinkWidthIn = 0x16;
        break;
      case (UINT32) 0x04:
        HTlinkWidthIn = 0x2;
        break;
      case (UINT32) 0x5:
        HTlinkWidthIn = 0x4;
        break;
      case (UINT32) 0x7:
        HTlinkWidthIn = 0xFF;
        break;
      default:
        break;
      }
      PortOverrideList->WidthIn = (UINT8) HTlinkWidthIn;
    }
    HTlinkWidthOut = AmdIdsNvReader (AGESA_IDS_NV_HTLINKWIDTHOUT, IdsNvPtr);
    if (HTlinkWidthOut != IDS_UNSUPPORTED) {
      switch (HTlinkWidthOut) {
      case (UINT32) 0x0:
        HTlinkWidthOut = 0x8;
        break;
      case (UINT32) 0x01:
        HTlinkWidthOut = 0x16;
        break;
      case (UINT32) 0x04:
        HTlinkWidthOut = 0x2;
        break;
      case (UINT32) 0x5:
        HTlinkWidthOut = 0x4;
        break;
      case (UINT32) 0x7:
        HTlinkWidthOut = 0xFF;
        break;
      default:
        break;
      }
      PortOverrideList->WidthOut = (UINT8) HTlinkWidthOut;
    }
  }
  return IDS_SUCCESS;
}

