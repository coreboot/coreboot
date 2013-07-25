/**
 * @file
 *
 * AMD Integrated Debug Debug_library Routines
 *
 * Contains all functions related to HDTOUT
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
#include "IdsLib.h"
#include "amdlib.h"
#include "AMD.h"
#include "heapManager.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "IdsDebugPrint.h"
#include "IdsDpHdtout.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_IDS_DEBUG_IDSDPHDTOUT_FILECODE

/**
 *  Check if String contain the substring
 *
 *  @param[in] String  Pointer of string.
 *  @param[in] Substr  Pointer of sub string.
 *
 *  @retval TRUE S2 is substring of S1
 *  @retval FALSE S2 isn't substring of S1
 *
**/
STATIC BOOLEAN
AmdIdsSubStr (
  IN      CHAR8 *String,
  IN      CHAR8 *Substr
  )
{
  UINT16 i;
  UINT16 j;

  for (i = 0; String[i] != 0 ; i++) {
    for (j = 0; (Substr[j] != 0) && (Substr[j] == String[i + j]); j++) {
    }
    if (Substr[j] == 0) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 *  Determine whether IDS console is enabled.
 *
 *  @param[in,out] pHdtoutHeader    Address of hdtout header pointer
 *  @param[in,out] StdHeader        The Pointer of AGESA Header
 *
 *  @retval       TRUE       pHdtoutHeader Non zero
 *  @retval       FALSE   pHdtoutHeader is NULL
 *
 **/
STATIC BOOLEAN
AmdIdsHdtoutGetHeader (
  IN OUT   HDTOUT_HEADER **pHdtoutHeaderPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 Dr3Reg;
  HDTOUT_HEADER *HdtoutHeaderPtr;
  LibAmdReadCpuReg (DR3_REG, &Dr3Reg);
  HdtoutHeaderPtr = (HDTOUT_HEADER *) (UINTN) Dr3Reg;
  if ((HdtoutHeaderPtr != NULL) && (HdtoutHeaderPtr->Signature == HDTOUT_HEADER_SIGNATURE)) {
    *pHdtoutHeaderPtr = HdtoutHeaderPtr;
    return TRUE;
  } else {
    return FALSE;
  }
}
/**
 *  Determine whether IDS console is enabled.
 *
 *  @param[in,out] IdsConsole    The Pointer of Ids console data
 *
 *  @retval       TRUE    Ids console is enabled.
 *  @retval       FALSE   Ids console is disabled.
 *
 **/
BOOLEAN
AmdIdsHdtOutSupport (
  VOID
  )
{
  BOOLEAN Result;
  UINT32  DR2reg;

  Result = FALSE;

  LibAmdReadCpuReg (DR2_REG, &DR2reg);
  if (DR2reg == 0x99CC) {
    Result = TRUE;
  }

  return Result;
}

/**
 *  Get HDTOUT customize Filter
 *
 *  @param[in,out] Filter    Filter do be filled
 *
 *  @retval       TRUE    Alway return true, for HDTOUT has its own filter mechanism
 *
 **/
STATIC BOOLEAN
AmdIdsHdtOutGetFilter (
  IN OUT   UINT64 *Filter
  )
{
  HDTOUT_HEADER *HdtoutHeaderPtr;

  if (AmdIdsHdtoutGetHeader (&HdtoutHeaderPtr, NULL) == TRUE) {
    *Filter = HdtoutHeaderPtr->ConsoleFilter;
  }
  return TRUE;
}

/**
 *
 *  Initial register setting used for HDT out Function.
 *
 *
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
STATIC VOID
AmdIdsHdtOutRegisterRestore (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 CR4reg;
  UINT64 SMsr;

  LibAmdMsrRead (0xC001100A, (UINT64 *)&SMsr, StdHeader);
  SMsr &= ~BIT0;
  LibAmdMsrWrite (0xC001100A, (UINT64 *)&SMsr, StdHeader);

  LibAmdWriteCpuReg (DR2_REG, 0);
  LibAmdWriteCpuReg (DR3_REG, 0);
  LibAmdWriteCpuReg (DR7_REG, 0);

  LibAmdReadCpuReg (CR4_REG, &CR4reg);
  LibAmdWriteCpuReg (CR4_REG, CR4reg & (~BIT3));

}

/**
 *
 *  Restore register setting used for HDT out Function.
 *
 *
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
STATIC VOID
AmdIdsHdtOutRegisterInit (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 CR4reg;
  UINT64 SMsr;

  LibAmdMsrRead (0xC001100A, (UINT64 *)&SMsr, StdHeader);
  SMsr |= 1;
  LibAmdMsrWrite (0xC001100A, (UINT64 *)&SMsr, StdHeader);

  LibAmdWriteCpuReg (DR2_REG, 0x99CC);

  LibAmdWriteCpuReg (DR7_REG, 0x02000420);

  LibAmdReadCpuReg (CR4_REG, &CR4reg);
  LibAmdWriteCpuReg (CR4_REG, CR4reg | ((UINT32)1 << 3));
}

/**
 *
 *  Initial function for HDT out Function.
 *
 *  Init required Debug register & heap, and will also fire a HDTOUT
 *  Command to let hdtout script do corresponding things.
 *
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
VOID
AmdIdsHdtOutInit (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  HDTOUT_HEADER HdtoutHeader;
  UINT8  Persist;
  AGESA_STATUS IgnoreSts;
  HDTOUT_HEADER *pHdtoutHeader;

  IDS_FUNCLIST_EXTERN ();
  if (AmdIdsHdtOutSupport ()) {
    AmdIdsHdtOutRegisterInit (StdHeader);
    // Initialize HDTOUT Header
    HdtoutHeader.Signature = HDTOUT_HEADER_SIGNATURE;
    HdtoutHeader.Version = HDTOUT_VERSION;
    HdtoutHeader.BufferSize = HDTOUT_DEFAULT_BUFFER_SIZE;
    HdtoutHeader.DataIndex = 0;
    HdtoutHeader.PrintCtrl = HDTOUT_PRINTCTRL_ON;
    HdtoutHeader.NumBreakpointUnit = 0;
    HdtoutHeader.FuncListAddr = (UINT32) IDS_FUNCLIST_ADDR;
    HdtoutHeader.StatusStr[0] = 0;
    HdtoutHeader.OutBufferMode = HDTOUT_BUFFER_MODE_ON;
    HdtoutHeader.EnableMask = 0;
    HdtoutHeader.ConsoleFilter = IDS_DEBUG_PRINT_MASK;

    // Trigger HDTOUT breakpoint to get inputs from script
    IdsOutPort (HDTOUT_INIT, (UINT32) &HdtoutHeader, 0);
    // Disable AP HDTOUT if set BspOnlyFlag
    if (HdtoutHeader.BspOnlyFlag == HDTOUT_BSP_ONLY) {
      if (!IsBsp (StdHeader, &IgnoreSts)) {
        AmdIdsHdtOutRegisterRestore (StdHeader);
        return;
      }
    }
    // Convert legacy EnableMask to new ConsoleFilter
    HdtoutHeader.ConsoleFilter |= HdtoutHeader.EnableMask;

    // Disable the buffer if the size is not large enough
    if (HdtoutHeader.BufferSize < 128) {
      HdtoutHeader.BufferSize = 0;
      HdtoutHeader.OutBufferMode = HDTOUT_BUFFER_MODE_OFF;
    } else {
      HdtoutHeader.OutBufferMode = HDTOUT_BUFFER_MODE_ON;
    }

    // Check if Hdtout header have been initialed, if so it must 2nd time come here
    if (AmdIdsHdtoutGetHeader (&pHdtoutHeader, StdHeader)) {
      Persist = HEAP_SYSTEM_MEM;
    } else {
      Persist = HEAP_LOCAL_CACHE;
    }

    // Allocate heap
    do {
      AllocHeapParams.RequestedBufferSize = HdtoutHeader.BufferSize + sizeof (HdtoutHeader) - 2;
      AllocHeapParams.BufferHandle = IDS_HDT_OUT_BUFFER_HANDLE;
      AllocHeapParams.Persist = Persist;
      if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
        break;
      } else {
        IdsOutPort (HDTOUT_ERROR, HDTOUT_ERROR_HEAP_ALLOCATION, AllocHeapParams.RequestedBufferSize);
        HdtoutHeader.BufferSize -= 256;
      }
    } while ((HdtoutHeader.BufferSize & 0x8000) == 0);
    // If the buffer have been successfully allocated?
    if ((HdtoutHeader.BufferSize & 0x8000) == 0) {
      LibAmdWriteCpuReg (DR3_REG, (UINT32) AllocHeapParams.BufferPtr);
      LibAmdMemCopy (AllocHeapParams.BufferPtr, &HdtoutHeader, sizeof (HdtoutHeader) - 2, StdHeader);
    } else {
      /// Clear DR3_REG
      IdsOutPort ((UINT32)HDTOUT_ERROR, (UINT32)HDTOUT_ERROR_HEAP_AllOCATE_FAIL, (UINT32)IDS_DEBUG_PRINT_MASK);
      LibAmdWriteCpuReg (DR3_REG, 0);
    }
  }
}

/**
 *
 *  Flush all HDTOUT buffer data before HOB transfer
 *
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
VOID
AmdIdsHdtOutBufferFlush (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  HDTOUT_HEADER *HdtoutHeaderPtr ;

  if (AmdIdsHdtOutSupport ()) {
    if (AmdIdsHdtoutGetHeader (&HdtoutHeaderPtr, StdHeader)) {
      if ((HdtoutHeaderPtr->PrintCtrl == HDTOUT_PRINTCTRL_ON) &&
          (HdtoutHeaderPtr->OutBufferMode == HDTOUT_BUFFER_MODE_ON)) {
        IdsOutPort (HDTOUT_PRINT, (UINT32) HdtoutHeaderPtr->Data, HdtoutHeaderPtr->DataIndex);
        HdtoutHeaderPtr->DataIndex = 0;
      }
    }
  }
}

/**
 *  Exit function for HDT out Function for each cores
 *
 *  @param[in] Ignored  no used
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *
 *  @retval AGESA_SUCCESS       Success
 *  @retval AGESA_ERROR         meet some error
 *
 **/
STATIC AGESA_STATUS
AmdIdsHdtOutExitCoreTask (
  IN       VOID  *Ignored,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  HDTOUT_HEADER *HdtoutHeaderPtr;

  if (AmdIdsHdtoutGetHeader (&HdtoutHeaderPtr, StdHeader)) {
    if ((HdtoutHeaderPtr->PrintCtrl == HDTOUT_PRINTCTRL_ON) &&
        (HdtoutHeaderPtr->OutBufferMode == HDTOUT_BUFFER_MODE_ON)) {
      IdsOutPort (HDTOUT_PRINT, (UINT32) HdtoutHeaderPtr->Data, HdtoutHeaderPtr->DataIndex);
    }
  }
  IdsOutPort (HDTOUT_EXIT, (UINT32) HdtoutHeaderPtr, 0);

  AmdIdsHdtOutRegisterRestore (StdHeader);

  return AGESA_SUCCESS;
}
/**
 *
 *  Exit function for HDT out Function.
 *
 *  Restore debug register and Deallocate heap, and will also fire a HDTOUT
 *  Command to let hdtout script do corresponding things.
 *
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
VOID
AmdIdsHdtOutExit (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  IDSAPLATETASK IdsApLateTask;
  if (AmdIdsHdtOutSupport ()) {
    IdsApLateTask.ApTask = (PF_IDS_AP_TASK) AmdIdsHdtOutExitCoreTask;
    IdsApLateTask.ApTaskPara = NULL;
    IdsAgesaRunFcnOnAllCoresLate (&IdsApLateTask, StdHeader);
    HeapDeallocateBuffer (IDS_HDT_OUT_BUFFER_HANDLE, StdHeader);
  }
}

/**
 *
 *  Exit function for HDT out Function of S3 Resume
 *
 *  Restore debug register and Deallocate heap, and will also fire a HDTOUT
 *  Command to let hdtout script do corresponding things.
 *
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
VOID
AmdIdsHdtOutS3Exit (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AGESA_STATUS AgesaStatus;

  if (AmdIdsHdtOutSupport ()) {
    //Ap debug print exit have been done at the end of AmdInitResume, so we only BSP at here
    AmdIdsHdtOutExitCoreTask (NULL, StdHeader);
    if (IsBsp (StdHeader, &AgesaStatus)) {
      HeapDeallocateBuffer (IDS_HDT_OUT_BUFFER_HANDLE, StdHeader);
    }
  }
}
/**
 *
 *  Exit function for HDT out Function of S3 Resume
 *
 *  Restore debug register and Deallocate heap, and will also fire a HDTOUT
 *  Command to let hdtout script do corresponding things.
 *
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
VOID
AmdIdsHdtOutS3ApExit (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AP_TASK TaskPtr;
  UINT32  Ignored;
  UINT32  BscSocket;
  UINT32  BscCoreNum;
  UINT32  Core;
  UINT32  Socket;
  UINT32  NumberOfSockets;
  UINT32  NumberOfCores;
  AGESA_STATUS IgnoredSts;

  if (AmdIdsHdtOutSupport ()) {
    // run code on all APs except BSP
    TaskPtr.FuncAddress.PfApTaskI = (PF_AP_TASK_I)AmdIdsHdtOutExitCoreTask;
    TaskPtr.DataTransfer.DataSizeInDwords = 0;
    TaskPtr.DataTransfer.DataPtr = NULL;
    TaskPtr.DataTransfer.DataTransferFlags = 0;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;

    NumberOfSockets = GetPlatformNumberOfSockets ();
    IdentifyCore (StdHeader, &BscSocket, &Ignored, &BscCoreNum, &IgnoredSts);

    for (Socket = 0; Socket < NumberOfSockets; Socket++) {
      if (IsProcessorPresent (Socket, StdHeader)) {
        if (GetActiveCoresInGivenSocket (Socket, &NumberOfCores, StdHeader)) {
          for (Core = 0; Core < NumberOfCores; Core++) {
            if ((Socket != BscSocket) || (Core != BscCoreNum)) {
              ApUtilRunCodeOnSocketCore ((UINT8) Socket, (UINT8) Core, &TaskPtr, StdHeader);
            }
          }
        }
      }
    }
  }
}

/**
 *  Print formated string with accerate buffer
 *  Flow out only when buffer will full
 *
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *  @param[in] HdtoutHeaderPtr  - Point to Hdtout Header
 *
**/
STATIC VOID
AmdIdsHdtOutPrintWithBuffer (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize,
  IN      HDTOUT_HEADER *HdtoutHeaderPtr
  )
{
  if ((HdtoutHeaderPtr == NULL) || (Buffer == NULL)) {
    ASSERT (FALSE);
    return;
  }

  while (BufferSize--) {
    if (HdtoutHeaderPtr->DataIndex >= HdtoutHeaderPtr->BufferSize) {
      //Flow out current buffer, and clear the index
      IdsOutPort (HDTOUT_PRINT, (UINT32) (UINTN) &HdtoutHeaderPtr->Data[0], HdtoutHeaderPtr->BufferSize);
      HdtoutHeaderPtr->DataIndex = 0;
    }
    HdtoutHeaderPtr->Data[HdtoutHeaderPtr->DataIndex++] = *(Buffer++);
  }
}
/**
 *  Save HDTOUT context, use for break point function
 *
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *  @param[in] HdtoutHeaderPtr  - Point to Hdtout Header
 *
**/
STATIC VOID
AmdIdsHdtOutSaveContext (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize,
  IN      HDTOUT_HEADER *HdtoutHeaderPtr
  )
{
  UINTN i;
  UINTN j;
  UINTN ArrayIndex;
  UINTN unusedPrefix;
  UINTN ArrayLength;
  BOOLEAN SaveStatus;

  ArrayLength = 0;

  // Look for the start of the first ASCII
  for (i = 0; i < BufferSize - 1; i++) {
    if ((Buffer[i] > 32) && (Buffer[i] < 127)) {
      break;
    }
  }

  unusedPrefix = i;
  //ASSERT if no "\n" in status string
  ASSERT (AmdIdsSubStr (&Buffer[i], (CHAR8 *)"\n"));

  if (i < (BufferSize - 1)) {
    // Match the first word in StatusStr
    SaveStatus = FALSE;
    for (j = 0; !SaveStatus && (HdtoutHeaderPtr->StatusStr[j] != 0); j++) {
      for (; (Buffer[i] == HdtoutHeaderPtr->StatusStr[j]) && (i < BufferSize); i++, j++) {
        ArrayLength++;
        if (Buffer[i] == ' ') {
          ArrayIndex = j;
          for (; HdtoutHeaderPtr->StatusStr[j] != '\n'; j++) {
            ArrayLength++;
          }
          // Remove old entry if it's size does not fit
          if (ArrayLength != ((UINT32) BufferSize - unusedPrefix)) {
            for (++j; HdtoutHeaderPtr->StatusStr[j] != 0; j++) {
              HdtoutHeaderPtr->StatusStr[j - ArrayLength] = HdtoutHeaderPtr->StatusStr[j];
            }
            j = j - ArrayLength - 1;
            i = unusedPrefix;
            // Mark the end of string
            HdtoutHeaderPtr->StatusStr[j + BufferSize - unusedPrefix + 1] = 0;
          } else {
            j = ArrayIndex - 1;
          }

          // Word match, exit for saving
          SaveStatus = TRUE;
          break;
        }
      }
    }

    // Copy string to StatusStr
    if ((HdtoutHeaderPtr->StatusStr[j] == 0) || SaveStatus) {
      for (; i < BufferSize; j++, i++) {
        HdtoutHeaderPtr->StatusStr[j] = Buffer[i];
      }
    }

    if (!SaveStatus) {
      // Mark the end of string if not done so
      HdtoutHeaderPtr->StatusStr[j] = 0;
    }
  }
}

STATIC BOOLEAN
AmdIdsHdtOutBreakPointUnit (
  IN OUT   BREAKPOINT_UNIT **pBpunitptr,
  IN OUT   UINT32 *numBp,
  IN       HDTOUT_HEADER *HdtoutHeaderPtr,
  IN       CHAR8   *Buffer
  )
{
  BOOLEAN isMatched;
  CHAR8 *PCmpStr;
  CHAR8 *Pbpstr;
  BREAKPOINT_UNIT *pBpunit;

  pBpunit = *pBpunitptr;
  if ((pBpunit == NULL) ||
      (numBp == NULL) ||
      (HdtoutHeaderPtr == NULL) ||
      (*numBp == 0)) {
    ASSERT (FALSE);
    return FALSE;
  }
  //Get to be compared string
  if (pBpunit->BpFlag == IDS_HDTOUT_BPFLAG_FORMAT_STR) {
    PCmpStr = Buffer;
  } else {
    PCmpStr = HdtoutHeaderPtr->StatusStr;
  }
  //Get BreakPoint string
  Pbpstr = HdtoutHeaderPtr->BreakpointList + pBpunit->BpStrOffset;
  isMatched = AmdIdsSubStr (PCmpStr, Pbpstr);
  //Point to next one, and decrease the numbp
  *pBpunitptr = ++pBpunit;
  (*numBp)--;
  return isMatched;
}
/**
 *  Process HDTOUT breakpoint
 *
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *  @param[in] HdtoutHeaderPtr  - Point to Hdtout Header
 *
**/
STATIC VOID
AmdIdsHdtOutBreakPoint (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize,
  IN      HDTOUT_HEADER *HdtoutHeaderPtr
  )
{
  UINT32 numBp;
  BREAKPOINT_UNIT *Pbpunit;
  BOOLEAN isMatched;
  UINT32 i;
  Pbpunit = (BREAKPOINT_UNIT *) &HdtoutHeaderPtr->BreakpointList[0];
  numBp = HdtoutHeaderPtr->NumBreakpointUnit;

  for (;;) {
    if (Pbpunit->AndFlag == IDS_HDTOUT_BP_AND_ON) {
      isMatched = TRUE;
      do {
        isMatched &= AmdIdsHdtOutBreakPointUnit (&Pbpunit, &numBp, HdtoutHeaderPtr, Buffer);
      } while ((Pbpunit->AndFlag == IDS_HDTOUT_BP_AND_ON) &&
               (isMatched == TRUE) &&
               (numBp > 0));
      //Next one is IDS_HDTOUT_BP_AND_OFF
      if (numBp > 0) {
        if (isMatched == TRUE) {
          isMatched &= AmdIdsHdtOutBreakPointUnit (&Pbpunit, &numBp, HdtoutHeaderPtr, Buffer);
        } else {
          Pbpunit++;
          numBp--;
        }
      }
    } else {
      isMatched = AmdIdsHdtOutBreakPointUnit (&Pbpunit, &numBp, HdtoutHeaderPtr, Buffer);
    }
    if ((isMatched == TRUE) || (numBp == 0)) {
      break;
    }
  }
  //Do action
  if (isMatched) {
//    AmdIdsSerialPrint (Buffer, BufferSize, NULL);
    Pbpunit--;
    switch (Pbpunit->Action) {
    case HDTOUT_BP_ACTION_HALT:
      i = (UINT32) (Pbpunit - ((BREAKPOINT_UNIT *) &HdtoutHeaderPtr->BreakpointList[0]));
      IdsOutPort (HDTOUT_BREAKPOINT, (UINT32) (UINTN) Buffer, ( i << 16) | (UINT32) BufferSize);
      break;
    case HDTOUT_BP_ACTION_PRINTON:
      if (HdtoutHeaderPtr->PrintCtrl != 1) {
        HdtoutHeaderPtr->PrintCtrl = 1;
        if (HdtoutHeaderPtr->OutBufferMode == HDTOUT_BUFFER_MODE_ON) {
          AmdIdsHdtOutPrintWithBuffer (Buffer, BufferSize, HdtoutHeaderPtr);
        } else {
          IdsOutPort (HDTOUT_PRINT, (UINT32) (UINTN) Buffer, (UINT32) BufferSize);
        }
      }
      break;
    case HDTOUT_BP_ACTION_PRINTOFF:
      if (HdtoutHeaderPtr->PrintCtrl != 0) {
        HdtoutHeaderPtr->PrintCtrl = 0;
        IdsOutPort (HDTOUT_PRINT, (UINT32) (UINTN) Buffer, (UINT32)BufferSize);
      }
      break;
    default:
      ASSERT (FALSE);
    }
  }
}


/**
 *  Print formated string to HDTOUT
 *
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *  @param[in] debugPrintPrivate  - Option
 *
**/
STATIC VOID
AmdIdsHdtOutPrint (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize,
  IN      IDS_DEBUG_PRINT_PRIVATE_DATA *debugPrintPrivate
  )
{
  HDTOUT_HEADER *HdtoutHeaderPtr;
  if (AmdIdsHdtoutGetHeader (&HdtoutHeaderPtr, NULL)) {
    //Print Function
    if (HdtoutHeaderPtr->PrintCtrl == HDTOUT_PRINTCTRL_ON) {
      if (HdtoutHeaderPtr->OutBufferMode == HDTOUT_BUFFER_MODE_ON) {
        AmdIdsHdtOutPrintWithBuffer (Buffer, BufferSize, HdtoutHeaderPtr);
      } else {
        IdsOutPort (HDTOUT_PRINT, (UINT32) (UINTN) Buffer, (UINT32) BufferSize);
      }
    }
    //Check BreakPoint
    if (HdtoutHeaderPtr->NumBreakpointUnit) {
      AmdIdsHdtOutBreakPoint (Buffer, BufferSize, HdtoutHeaderPtr);
      if (debugPrintPrivate->saveContext) {
        AmdIdsHdtOutSaveContext (Buffer, BufferSize, HdtoutHeaderPtr);
        debugPrintPrivate->saveContext = FALSE;
      }
    }
  } else {
    //No HDTOUT header found print directly without buffer
    IdsOutPort (HDTOUT_PRINT, (UINT32) (UINTN) Buffer, (UINT32) BufferSize);
  }
}

/**
 *  Init local private data for HDTOUT
 *
 *  @param[in] Flag    - filter flag
 *  @param[in] debugPrintPrivate  - Point to debugPrintPrivate
 *
**/
STATIC VOID
AmdIdsHdtOutInitPrivateData (
  IN      UINT64 Flag,
  IN      IDS_DEBUG_PRINT_PRIVATE_DATA *debugPrintPrivate
  )
{
  if (Flag == MEM_STATUS) {
    debugPrintPrivate->saveContext = TRUE;
  }
}

CONST IDS_DEBUG_PRINT ROMDATA  IdsDebugPrintHdtoutInstance =
{
  AmdIdsHdtOutSupport,
  AmdIdsHdtOutGetFilter,
  AmdIdsHdtOutInitPrivateData,
  AmdIdsHdtOutPrint
};



