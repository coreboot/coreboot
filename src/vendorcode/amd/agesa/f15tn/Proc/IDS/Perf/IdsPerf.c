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


