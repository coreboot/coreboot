/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Integrated Debug Routines for performance analysis
 *
 * Contains AMD AGESA debug macros and functions for performance analysis
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
#include "IdsDpHdtout.h"
#include "heapManager.h"
#include "cpuFamilyTranslation.h"
#include "amdlib.h"

#define FILECODE PROC_IDS_PERF_IDSPERF_FILECODE
/**
 *
 *  IDS Performance function for Output to HDT.
 *
 *  Invoke communications with the HDT environment to allow the user to issue
 *  debug commands. If the sign = 0x0, HDT Control Register will be initialized to
 *  catch the special I/O for HDT_OUT. Otherwise, it will inform HDT script
 *  function what is meaning for the value to output to HDT.
 *
 *  @param[in] Command    HDT_OUT Command.
 *  @param[in] Data       The Data to output to HDT.
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
#define HDTOUT_COMMAND        0x99cc
#define HDTOUT_TIME_ANALYSE   (0xD0BF0000ul | HDTOUT_COMMAND)

VOID
IdsPerfHdtOut (
  IN       UINT16 Command,
  IN       UINT32 Data,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  IdsOutPort (HDTOUT_TIME_ANALYSE, Data, 0);
}

/**
 *
 *  Get Ids Performance analysis table pointer in the AGESA Heap.
 *
 *  @param[in] LineInFile  ((FILECODE) shift 16)+ Line number
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 *  @retval AGESA_SUCCESS       Success to get the pointer of Performance analysis Table.
 *  @retval AGESA_ERROR         Fail to get the pointer of Performance analysis Table.
 *  @retval AGESA_UNSUPPORTED   Get an exclude testpoint
 *
 **/
AGESA_STATUS
IdsPerfTimestamp (
  IN       UINT32 LineInFile,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AGESA_STATUS status;
  UINT32 Index;
  TP_Perf_STRUCT *PerfTableEntry;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  LOCATE_HEAP_PTR LocateHeapStructPtr;
  UINT64 CurrentTsc;

  //if heap is not ready yet, don't invoke locate buffer, or else will cause event log & locate heap dead loop
  if (StdHeader->HeapStatus != HEAP_DO_NOT_EXIST_YET ) {
    LibAmdMsrRead (TSC, &CurrentTsc, StdHeader);

    LocateHeapStructPtr.BufferHandle = IDS_CHECK_POINT_PERF_HANDLE;
    LocateHeapStructPtr.BufferPtr = NULL;
    status = HeapLocateBuffer (&LocateHeapStructPtr, StdHeader);
    if (status == AGESA_SUCCESS) {
      PerfTableEntry = (TP_Perf_STRUCT *) (LocateHeapStructPtr.BufferPtr);
    } else {
      AllocHeapParams.RequestedBufferSize = sizeof (TP_Perf_STRUCT);
      AllocHeapParams.BufferHandle = IDS_CHECK_POINT_PERF_HANDLE;
      AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
      status = HeapAllocateBuffer (&AllocHeapParams, StdHeader);
      if (status != AGESA_SUCCESS) {
        return status;
      }
      PerfTableEntry = (TP_Perf_STRUCT *) (AllocHeapParams.BufferPtr);
      LibAmdMemFill (PerfTableEntry, 0, sizeof (TP_Perf_STRUCT), StdHeader);
      PerfTableEntry->Signature = 'EMIT';
    }

    Index = PerfTableEntry ->Index;

    PerfTableEntry ->TP[Index].LineInFile = LineInFile;
    PerfTableEntry ->TP[Index].StartTsc = CurrentTsc;
    PerfTableEntry ->Index = ++Index;
  }
  return AGESA_SUCCESS;
}

typedef struct _PERFREGBACKUP {
  UINT64 SMsr;
  UINT32 Dr0Reg;
  UINT32 Dr7Reg;
  UINT32 Cr4Reg;
} PERFREGBACKUP;

VOID
IdsPerfSaveReg (
  IN OUT   PERFREGBACKUP * perfreg,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{

  LibAmdReadCpuReg (DR0_REG, &perfreg->Dr0Reg);

  LibAmdReadCpuReg (DR7_REG, &perfreg->Dr7Reg);

  LibAmdReadCpuReg (CR4_REG, &perfreg->Cr4Reg);
}

VOID
IdsPerfRestoreReg (
  IN       PERFREGBACKUP * perfreg,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{

  LibAmdWriteCpuReg (DR0_REG, perfreg->Dr0Reg);

  LibAmdWriteCpuReg (DR7_REG, perfreg->Dr7Reg);

  LibAmdWriteCpuReg (CR4_REG, perfreg->Cr4Reg);
}
/**
 * Output Test Point function .
 *
 *  @param[in,out] StdHeader    The Pointer of Standard Header.
 *
 *  @retval AGESA_SUCCESS       Success to get the pointer of IDS_CHECK_POINT_PERF_HANDLE.
 *  @retval AGESA_ERROR         Fail to get the pointer of IDS_CHECK_POINT_PERF_HANDLE.
 *
 **/
AGESA_STATUS
IdsPerfAnalyseTimestamp (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AGESA_STATUS status;
  LOCATE_HEAP_PTR LocateHeapStructPtr;
  UINT32 TscRateInMhz;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  PERFREGBACKUP PerfReg;
  UINT32 CR4reg;
  UINT64 SMsr;

  LocateHeapStructPtr.BufferHandle = IDS_CHECK_POINT_PERF_HANDLE;
  LocateHeapStructPtr.BufferPtr = NULL;
  status = HeapLocateBuffer (&LocateHeapStructPtr, StdHeader);
  if (status != AGESA_SUCCESS) {
    return status;
  }
  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetTscRate (FamilySpecificServices, &TscRateInMhz, StdHeader);
  ((TP_Perf_STRUCT *) (LocateHeapStructPtr.BufferPtr)) ->TscInMhz = TscRateInMhz;
  if (AmdIdsHdtOutSupport () == FALSE) {
  //Init break point
    IdsPerfSaveReg (&PerfReg, StdHeader);

    SMsr |= 1;

    LibAmdWriteCpuReg (DR2_REG, 0x99cc);
    LibAmdWriteCpuReg (DR7_REG, 0x02000420);

    LibAmdReadCpuReg (CR4_REG, &CR4reg);
    LibAmdWriteCpuReg (CR4_REG, CR4reg | ((UINT32)1 << 3));

    IdsPerfHdtOut (1, (UINT32) (UINT64) LocateHeapStructPtr.BufferPtr, StdHeader);
    IdsPerfRestoreReg (&PerfReg, StdHeader);
  }
  return status;
}


