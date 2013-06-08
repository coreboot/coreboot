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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include "IdsLib.h"
#include "amdlib.h"
#include "heapManager.h"
#include "IdsNvToCmos.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_IDS_CONTROL_IDSNVTOCMOS_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
extern IDS_NV_TO_CMOS gIdsNVToCmos[];

/**
 *
 *  Read CMOS
 *
 *  @param[in] IndexPort   Index port of access CMOS
 *  @param[in] DataPort   Data port of access CMOS
 *  @param[in] Index   Index of CMOS
 *  @param[in,out] Value   Point to Value
 *  @param[in,out] StdHeader    The Pointer of Standard Header.
 *
 **/
VOID
IdsReadCmos (
  IN       UINT16       IndexPort,
  IN       UINT16       DataPort,
  IN       UINT16       Index,
  IN OUT   UINT8        *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdIoWrite (AccessWidth8, IndexPort, &Index, StdHeader);
  LibAmdIoRead (AccessWidth8, DataPort, Value, StdHeader);
}
/**
 *
 *  Write CMOS
 *
 *  @param[in] IndexPort   Index port of access CMOS
 *  @param[in] DataPort   Data port of access CMOS
 *  @param[in] Index   Index of CMOS
 *  @param[in,out] Value   Point to Value
 *  @param[in,out] StdHeader    The Pointer of Standard Header.
 *
 **/
VOID
IdsWriteCmos (
  IN       UINT16       IndexPort,
  IN       UINT16       DataPort,
  IN       UINT16       Index,
  IN OUT   UINT8        *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdIoWrite (AccessWidth8, IndexPort, &Index, StdHeader);
  LibAmdIoWrite (AccessWidth8, DataPort, Value, StdHeader);
}
/**
 *
 *  Get IDS CMOS save region in the AGESA Heap.
 *
 *  @param[in,out] IdsCmosRegion   The Pointer of IDS CMOS save address in heap.
 *  @param[in,out] StdHeader    The Pointer of Standard Header.
 *
 *  @retval AGESA_SUCCESS       Success to get the pointer of NV Table.
 *  @retval AGESA_ERROR         Fail to get the pointer of NV Table.
 **/
AGESA_STATUS
AmdGetIdsCmosSaveRegion (
  IN OUT   VOID  **IdsCmosRegion,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AGESA_STATUS status;
  LOCATE_HEAP_PTR LocateHeapStructPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  LocateHeapStructPtr.BufferHandle = IDS_NV_TO_CMOS_HANDLE;
  LocateHeapStructPtr.BufferPtr = NULL;
  status = HeapLocateBuffer (&LocateHeapStructPtr, StdHeader);
  if (status == AGESA_SUCCESS) {
    *IdsCmosRegion = LocateHeapStructPtr.BufferPtr;
  } else {
    //Allocated the heap when can't located
    AllocHeapParams.RequestedBufferSize = IDS_CMOS_REGION_END - IDS_CMOS_REGION_START + 1;
    AllocHeapParams.BufferHandle = IDS_NV_TO_CMOS_HANDLE;
    AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
    status = HeapAllocateBuffer (&AllocHeapParams, (AMD_CONFIG_PARAMS *) StdHeader);
    if (status == AGESA_SUCCESS) {
      *IdsCmosRegion = AllocHeapParams.BufferPtr;
    }
  }
  return status;
}


/**
 *  IDS Backend Function for save BSP's NV heap to CMOS
 *
 *
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsCheckCmosValid (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 CmosIndex;
  UINT8 TmpValue;
  UINT8 Len;
  UINT8 Sum;
  CmosIndex = IDS_CMOS_REGION_SIGNATURE_OFFSET;
  //Validate the Signature
  IdsReadCmos (IDS_CMOS_INDEX_PORT, IDS_CMOS_DATA_PORT, CmosIndex, &TmpValue, StdHeader);
  if (TmpValue != 'N') {
    return IDS_UNSUPPORTED;
  }

  CmosIndex++;
  IdsReadCmos (IDS_CMOS_INDEX_PORT, IDS_CMOS_DATA_PORT, CmosIndex, &TmpValue, StdHeader);
  if (TmpValue != 'V') {
    return IDS_UNSUPPORTED;
  }

  CmosIndex = IDS_CMOS_REGION_LENGTH_OFFSET;
  IdsReadCmos (IDS_CMOS_INDEX_PORT, IDS_CMOS_DATA_PORT, CmosIndex, &Len, StdHeader);
  if (Len > (IDS_CMOS_REGION_END - IDS_CMOS_REGION_START - IDS_NV_TO_CMOS_HEADER_SIZE + 1)) {
    return IDS_UNSUPPORTED;
  }
  Sum = 0;
  CmosIndex = IDS_CMOS_REGION_CHECKSUM_OFFSET;
  for (; CmosIndex < IDS_CMOS_REGION_DATA_OFFSET + Len; CmosIndex++) {
    IdsReadCmos (IDS_CMOS_INDEX_PORT, IDS_CMOS_DATA_PORT, CmosIndex, &TmpValue, StdHeader);
    Sum = (UINT8) (Sum + TmpValue);
  }
  if (Sum != 0) {
    return IDS_UNSUPPORTED;
  }
  return IDS_SUCCESS;
}

/**
 *
 *  AP get NV from CMOS
 *
 *  If Ap Can't get Nv Data from Callout, Try to Create NV heap via the
 *  CMOS data area save by BSP previous

 *
 *  @param[in,out] StdHeader    The Pointer of IDS Initial Parameter
 *
 *  @retval AGESA_SUCCESS       Success to get the NV from CMOS
 *  @retval AGESA_ERROR         Fail to get
 *
 **/
AGESA_STATUS
AmdIdsApGetNvFromCmos (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AGESA_STATUS status;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  IDS_CONTROL_STRUCT *IdsCtrlPtr;
  UINT8 CmosIndex;
  UINT8 TmpValue;
  UINT8 TmpU16Value;
  UINT8 k;
  UINT16 i;
  IDS_NV_ITEM *NvPtr;
  UINT8 Len;
  status = AGESA_ERROR;
  if (IdsCheckCmosValid (StdHeader) == IDS_SUCCESS) {
    AllocHeapParams.RequestedBufferSize = sizeof (IDS_CONTROL_STRUCT);
    AllocHeapParams.RequestedBufferSize += (IDS_CMOS_REGION_END - IDS_CMOS_REGION_START - IDS_NV_TO_CMOS_HEADER_SIZE + 2) * sizeof (IDS_NV_ITEM) ;
    AllocHeapParams.BufferHandle = IDS_CONTROL_HANDLE;
    AllocHeapParams.Persist = HEAP_LOCAL_CACHE;

    //
    // Allocate data buffer in heap
    //
    status = HeapAllocateBuffer (&AllocHeapParams, (AMD_CONFIG_PARAMS *) StdHeader);
    if (status == AGESA_SUCCESS) {
      IdsCtrlPtr = (IDS_CONTROL_STRUCT *) AllocHeapParams.BufferPtr;
      IdsCtrlPtr->IgnoreIdsDefault = TRUE;
      IdsCtrlPtr->IdsHeapMemSize = AllocHeapParams.RequestedBufferSize;
      IdsCtrlPtr->IdsNvTableOffset = sizeof (IDS_CONTROL_STRUCT);
      NvPtr = (IDS_NV_ITEM *) (AllocHeapParams.BufferPtr + IdsCtrlPtr->IdsNvTableOffset);
      i = 0;
      CmosIndex = IDS_CMOS_REGION_LENGTH_OFFSET;
      IdsReadCmos (IDS_CMOS_INDEX_PORT, IDS_CMOS_DATA_PORT, CmosIndex, &Len, StdHeader);
      CmosIndex = IDS_CMOS_REGION_DATA_OFFSET;
      while ((gIdsNVToCmos[i].Length != IDS_NV_TO_CMOS_LEN_END) &&
           (gIdsNVToCmos[i].IDS_NV_ID != IDS_NV_TO_CMOS_ID_END) &&
           (CmosIndex <= IDS_CMOS_REGION_END) &&
           (Len-- > 0)) {
        if (gIdsNVToCmos[i].Length == IDS_NV_TO_CMOS_LEN_BYTE || gIdsNVToCmos[i].Length == IDS_NV_TO_CMOS_LEN_WORD) {
          TmpU16Value = 0;
          for (k = 0; k < gIdsNVToCmos[i].Length; k++, CmosIndex++) {
            IdsReadCmos (IDS_CMOS_INDEX_PORT, IDS_CMOS_DATA_PORT, CmosIndex, &TmpValue, StdHeader);
            TmpU16Value |= (UINT16)TmpValue << (k * 8);
          }
          if ((TmpU16Value != IDS_NV_TO_CMOS_BYTE_IGNORED && (gIdsNVToCmos[i].Length == IDS_NV_TO_CMOS_LEN_BYTE)) ||
              (TmpU16Value != IDS_NV_TO_CMOS_WORD_IGNORED && (gIdsNVToCmos[i].Length == IDS_NV_TO_CMOS_LEN_WORD))) {
            NvPtr->IdsNvId = gIdsNVToCmos[i].IDS_NV_ID;
            NvPtr->IdsNvValue = TmpU16Value;
            NvPtr ++;
          }
        }
        i++;
      }
      NvPtr->IdsNvId = AGESA_IDS_NV_END;
    }
  }
  return status;
}

/**
 *  IDS Backend Function for Restore  CMOS
 *
 *
 *  @param[in,out]   DataPtr      NULL
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubRestoreCmos (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  UINT8 *PCmosSave;
  UINT8 CmosIndex;

  //Save CMOS to BSP heap
  if (AmdGetIdsCmosSaveRegion ((VOID **) &PCmosSave, StdHeader) == AGESA_SUCCESS) {
    for (CmosIndex = IDS_CMOS_REGION_START; CmosIndex <= IDS_CMOS_REGION_END; CmosIndex++, PCmosSave++) {
      IdsWriteCmos (IDS_CMOS_INDEX_PORT, IDS_CMOS_DATA_PORT, CmosIndex, PCmosSave, StdHeader);
    }
  }
  return IDS_SUCCESS;
}
/**
 *  IDS Backend Function for save BSP's NV heap to CMOS
 *
 *
 *  @param[in,out]   DataPtr      NULL
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubSaveBspNvHeapToCmos (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  UINT8 i;
  UINT8 k;
  UINT8 CmosIndex;
  UINT8 TmpValue;
  UINT8 Sum;
  UINT8 Len;
  UINT8 *PCmosSave;
  IDS_STATUS IdsNvValue;

  //Save CMOS to BSP heap
  if (AmdGetIdsCmosSaveRegion ((VOID **) &PCmosSave, StdHeader) == AGESA_SUCCESS) {
    for (CmosIndex = IDS_CMOS_REGION_START; CmosIndex <= IDS_CMOS_REGION_END; CmosIndex++) {
      IdsReadCmos (IDS_CMOS_INDEX_PORT, IDS_CMOS_DATA_PORT, CmosIndex, &TmpValue, StdHeader);
      *(PCmosSave++) = TmpValue;
    }
    //The CMOS Region is saved to heap,Now we can save BSP NV to CMOS
    i = 0;
    CmosIndex = IDS_CMOS_REGION_SIGNATURE_OFFSET;
    //CMOS Map
    // **********************************************************************************
    // Field          |   Offset   | Description
    // **********************************************************************************
    // Signature      |     0      | 'NV' specify the IDS Cmos save region
    // **********************************************************************************
    // Length         |     2      | Actual Length of all save NV, may less than platform
    //               |            | define
    // **********************************************************************************
    // CheckSum       |     3      | CheckSum of all NV fields exclue Signature & Length
    // **********************************************************************************
    // NVSaveRegion   |     4      | Nv Save Region
    // **********************************************************************************
    // Set Signature 'NV';
    TmpValue = 'N';
    IdsWriteCmos (IDS_CMOS_INDEX_PORT, IDS_CMOS_DATA_PORT, CmosIndex, &TmpValue, StdHeader);
    CmosIndex++;
    TmpValue = 'V';
    IdsWriteCmos (IDS_CMOS_INDEX_PORT, IDS_CMOS_DATA_PORT, CmosIndex, &TmpValue, StdHeader);

    CmosIndex = IDS_CMOS_REGION_DATA_OFFSET;
    Sum = 0;
    Len = 0;
    while ((gIdsNVToCmos[i].Length != IDS_NV_TO_CMOS_LEN_END) &&
               (gIdsNVToCmos[i].IDS_NV_ID != IDS_NV_TO_CMOS_ID_END) &&
               (CmosIndex <= IDS_CMOS_REGION_END)) {
      //Get NV Value length
      if (gIdsNVToCmos[i].Length == IDS_NV_TO_CMOS_LEN_BYTE || gIdsNVToCmos[i].Length == IDS_NV_TO_CMOS_LEN_WORD) {
        IdsNvValue = AmdIdsNvReader (gIdsNVToCmos[i].IDS_NV_ID, IdsNvPtr, StdHeader);
        for (k = 0; k < gIdsNVToCmos[i].Length; k++) {
          TmpValue = (UINT8) ((IdsNvValue >> (8 * k)) & 0xFF);
          Sum = (UINT8) (Sum + TmpValue);
          Len++;
          IdsWriteCmos (IDS_CMOS_INDEX_PORT, IDS_CMOS_DATA_PORT, CmosIndex, &TmpValue, StdHeader);
          CmosIndex++;
        }
      } else {
        ASSERT (FALSE);
      }
      i++;
    }
    CmosIndex = IDS_CMOS_REGION_LENGTH_OFFSET;
    IdsWriteCmos (IDS_CMOS_INDEX_PORT, IDS_CMOS_DATA_PORT, CmosIndex, &Len, StdHeader);
    CmosIndex = IDS_CMOS_REGION_CHECKSUM_OFFSET;
    TmpValue = (UINT8) (0x100 - Sum);
    IdsWriteCmos (IDS_CMOS_INDEX_PORT, IDS_CMOS_DATA_PORT, CmosIndex, &TmpValue, StdHeader);
  }
  return IDS_SUCCESS;
}

CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatNvToCmosSaveBlock =
{
  IDS_FEAT_NV_TO_CMOS,
  IDS_BSP_ONLY,
  IDS_CPU_Early_Override,
  IDS_FAMILY_ALL,
  IdsSubSaveBspNvHeapToCmos
};

CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatNvToCmosRestoreBlock =
{
  IDS_FEAT_NV_TO_CMOS,
  IDS_BSP_ONLY,
  IDS_BEFORE_AP_EARLY_HALT,
  IDS_FAMILY_ALL,
  IdsSubRestoreCmos
};


