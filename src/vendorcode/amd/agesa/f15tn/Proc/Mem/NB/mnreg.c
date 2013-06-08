/* $NoKeywords:$ */
/**
 * @file
 *
 * mnreg.c
 *
 * Common Northbridge register access functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
*
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
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "merrhdl.h"
#include "heapManager.h"
#include "Filecode.h"
#include "GeneralServices.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_MNREG_FILECODE


/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets the current DCT to work on.
 *   Should be called before accessing a certain DCT
 *   All data structures will be updated to point to the current DCT
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   Dct - ID of the target DCT
 *
 */

VOID
MemNSwitchDCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct
  )
{
  ASSERT (NBPtr->DctCount > Dct);
  //
  // Set the DctCfgSel to new DCT
  //
  NBPtr->FamilySpecificHook[DCTSelectSwitch] (NBPtr, &Dct);
  NBPtr->Dct = Dct;
  NBPtr->MCTPtr->Dct = NBPtr->Dct;
  NBPtr->DCTPtr = &(NBPtr->MCTPtr->DctData[NBPtr->Dct]);
  NBPtr->PsPtr = &(NBPtr->PSBlock[NBPtr->Dct]);
  NBPtr->DctCachePtr = &(NBPtr->DctCache[NBPtr->Dct]);

  MemNSwitchChannelNb (NBPtr, NBPtr->Channel);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function is used by families that use a separate DctCfgSel bit to
 *     select the current DCT which will be accessed by function 2.
 *     NOTE:  This function must be called BEFORE the NBPtr->Dct variable is
 *     updated.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *Dct     - Pointer to ID of the target DCT
 *
 */

BOOLEAN
MemNDctCfgSelectUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *Dct
  )
{
  //
  // Sanity check the current DctCfgSel setting
  //
  ASSERT (NBPtr->Dct == NBPtr->GetBitField (NBPtr, BFDctCfgSel));
  //
  // Set the DctCfgSel to new DCT
  //
  NBPtr->SetBitField (NBPtr, BFDctCfgSel, *(UINT8*)Dct);

  return TRUE;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets the current channel to work on.
 *   Should be called before accessing a certain channel
 *   All data structures will be updated to point to the current channel
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   Channel - ID of the target channel
 *
 */

VOID
MemNSwitchChannelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Channel
  )
{
  NBPtr->Channel = Channel ? 1 : 0;
  NBPtr->ChannelPtr = &(NBPtr->DCTPtr->ChData[NBPtr->Channel]);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets a bit field from PCI register
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   FieldName - Field name
 *
 *     @return      Bit field value
 */

UINT32
MemNGetBitFieldNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName
  )
{
  UINT32 Value;

  Value = NBPtr->MemNCmnGetSetFieldNb (NBPtr, 0, FieldName, 0);
  return Value;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets a bit field from PCI register
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   FieldName - Field name
 *     @param[in]   Field - Value to be stored in PCT register
 *
 */

VOID
MemNSetBitFieldNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  )
{
  NBPtr->MemNCmnGetSetFieldNb (NBPtr, 1, FieldName, Field);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Check if bitfields of all enabled DCTs on a die have the expected value. Ignore
 *  DCTs that are disabled.
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   FieldName - Bit Field name
 *     @param[in]   Field - Value to be checked
 *
 *     @return          TRUE -  All enabled DCTs have the expected value on the bitfield.
 *     @return          FALSE - Not all enabled DCTs have the expected value on the bitfield.
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNBrdcstCheckNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  )
{
  UINT8 Dct;
  UINT8 CurrentDCT;
  Dct = NBPtr->Dct;
  for (CurrentDCT = 0; CurrentDCT < NBPtr->DctCount; CurrentDCT++) {
    MemNSwitchDCTNb (NBPtr, CurrentDCT);
    if ((NBPtr->DCTPtr->Timings.DctMemSize != 0) && !((CurrentDCT == 1) && NBPtr->Ganged)) {
      if (MemNGetBitFieldNb (NBPtr, FieldName) != Field) {
        MemNSwitchDCTNb (NBPtr, Dct);
        return FALSE;
      }
    }
  }
  MemNSwitchDCTNb (NBPtr, Dct);
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Set bitfields of all enabled DCTs on a die to a value. Ignore
 *  DCTs that are disabled.
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   FieldName - Bit Field name
 *     @param[in]   Field - Value to be set
 *
 * ----------------------------------------------------------------------------
 */
VOID
MemNBrdcstSetNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  )
{
  UINT8 Dct;
  UINT8 CurrentDCT;
  Dct = NBPtr->Dct;
  for (CurrentDCT = 0; CurrentDCT < NBPtr->DctCount; CurrentDCT++) {
    MemNSwitchDCTNb (NBPtr, CurrentDCT);
    if ((NBPtr->DCTPtr->Timings.DctMemSize != 0) && !((CurrentDCT == 1) && NBPtr->Ganged)) {
      MemNSetBitFieldNb (NBPtr, FieldName, Field);
    }
  }
  MemNSwitchDCTNb (NBPtr, Dct);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Set bitfields of all DCTs regardless of if they are being enabled or not on a
 *  die to a value.
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   FieldName - Bit Field name
 *     @param[in]   Field - Value to be set
 *
 * ----------------------------------------------------------------------------
 */
VOID
MemNBrdcstSetUnConditionalNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  )
{
  UINT8 Dct;
  UINT8 CurrentDCT;
  Dct = NBPtr->Dct;
  for (CurrentDCT = 0; CurrentDCT < NBPtr->DctCount; CurrentDCT++) {
    MemNSwitchDCTNb (NBPtr, CurrentDCT);
    MemNSetBitFieldNb (NBPtr, FieldName, Field);
  }
  MemNSwitchDCTNb (NBPtr, Dct);
}

/*-----------------------------------------------------------------------------*/
/**
 *      This function calculates the memory channel index relative to the
 *      socket, taking the Die number, the Dct, and the channel.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in] Dct
 *     @param[in] Channel
 *
 */
UINT8
MemNGetSocketRelativeChannelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct,
  IN       UINT8 Channel
  )
{
  return ((NBPtr->MCTPtr->DieId * NBPtr->DctCount) + Dct);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Poll a bitfield. If the bitfield does not get set to the target value within
 *  specified microseconds, it times out.
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   FieldName - Bit Field name
 *     @param[in]   Field - Value to be set
 *     @param[in]   MicroSecond - Number of microsecond to wait
 *     @param[in]   IfBroadCast - Need to broadcast to both DCT or not
 *
 * ----------------------------------------------------------------------------
 */
VOID
MemNPollBitFieldNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field,
  IN       UINT32 MicroSecond,
  IN       BOOLEAN IfBroadCast
  )
{
  BOOLEAN ErrorRecovery;
  BOOLEAN IgnoreErr;
  UINT8 ExcludeDCT;
  UINT16 ExcludeChipSelMask;
  UINT32 EventInfo;
  UINT64 InitTSC;
  UINT64 CurrentTSC;
  UINT64 TimeOut;
  AGESA_STATUS EventClass;
  MEM_DATA_STRUCT *MemPtr;
  DIE_STRUCT *MCTPtr;
  BOOLEAN TimeoutEn;

  MemPtr = NBPtr->MemPtr;
  MCTPtr = NBPtr->MCTPtr;
  ExcludeDCT = EXCLUDE_ALL_DCT;
  ExcludeChipSelMask = EXCLUDE_ALL_CHIPSEL;
  TimeoutEn = TRUE;
  IDS_TIMEOUT_CTL (&TimeoutEn);

  CurrentTSC = 0;
  LibAmdMsrRead (TSC, &InitTSC, &MemPtr->StdHeader);
  TimeOut = InitTSC + ((UINT64) MicroSecond * MemPtr->TscRate);

  while ((CurrentTSC < TimeOut) || !TimeoutEn) {
    if (IfBroadCast) {
      if (NBPtr->BrdcstCheck (NBPtr, FieldName, Field)) {
        break;
      }
    } else {
      if (MemNGetBitFieldNb (NBPtr, FieldName) == Field) {
        break;
      }
    }
    LibAmdMsrRead (TSC, &CurrentTSC, &MemPtr->StdHeader);
  }

  if ((CurrentTSC >= TimeOut) && TimeoutEn) {
    ErrorRecovery = TRUE;
    IgnoreErr = FALSE;
    IDS_OPTION_HOOK (IDS_MEM_ERROR_RECOVERY, &ErrorRecovery, &MemPtr->StdHeader);
    IDS_OPTION_HOOK (IDS_MEM_IGNORE_ERROR, &IgnoreErr, &MemPtr->StdHeader);

    // Default event class
    // If different event class is needed in one entry, override it.
    EventClass = AGESA_ERROR;
    switch (FieldName) {
    case BFDramEnabled:
      EventInfo = MEM_ERROR_DRAM_ENABLED_TIME_OUT;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tDramEnabled bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    case BFDctAccessDone:
      EventInfo = MEM_ERROR_DCT_ACCESS_DONE_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tDctAccessDone bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    case BFSendCtrlWord:
      EventInfo = MEM_ERROR_SEND_CTRL_WORD_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tSendCtrlWord bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    case BFPrefDramTrainMode:
      EventInfo = MEM_ERROR_PREF_DRAM_TRAIN_MODE_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tPrefDramTrainMode bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    case BFEnterSelfRef:
      EventInfo = MEM_ERROR_ENTER_SELF_REF_TIME_OUT;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tEnterSelfRef bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    case BFFreqChgInProg:
      EventInfo = MEM_ERROR_FREQ_CHG_IN_PROG_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tFreqChgInProg bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    case BFExitSelfRef:
      EventInfo = MEM_ERROR_EXIT_SELF_REF_TIME_OUT;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tExitSelfRef bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    case BFSendMrsCmd:
      EventInfo = MEM_ERROR_SEND_MRS_CMD_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tSendMrsCmd bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    case BFSendZQCmd:
      EventInfo = MEM_ERROR_SEND_ZQ_CMD_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tSendZQCmd bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    case BFDctExtraAccessDone:
      EventInfo = MEM_ERROR_DCT_EXTRA_ACCESS_DONE_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tDctExtraAccessDone bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    case BFMemClrBusy:
      EventInfo = MEM_ERROR_MEM_CLR_BUSY_TIME_OUT;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tMemClrBusy bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    case BFMemCleared:
      EventInfo = MEM_ERROR_MEM_CLEARED_TIME_OUT;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tMemCleared bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    case BFFlushWr:
      EventInfo = MEM_ERROR_FLUSH_WR_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tFlushWr bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    case BFCurNbPstate:
      EventInfo = MEM_ERROR_NBPSTATE_TRANSITION_TIME_OUT;
      IDS_HDT_CONSOLE (MEM_FLOW, "\tCurNBPstate bitfield times out.\n");
      ASSERT (ErrorRecovery || IgnoreErr);
      break;
    default:
      EventClass = 0;
      EventInfo = 0;
      IDS_ERROR_TRAP;
    }

    PutEventLog (EventClass, EventInfo, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &MemPtr->StdHeader);
    SetMemError (EventClass, MCTPtr);
    if (!MemPtr->ErrorHandling (MCTPtr, ExcludeDCT, ExcludeChipSelMask, &MemPtr->StdHeader)) {
      ASSERT (FALSE);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function changes memory Pstate context
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in]      MemPstate  - Target Memory Pstate
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
VOID
MemNChangeMemPStateContextNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSTATE MemPstate
  )
{
  UINT8 PSMasterChannel;
  UINT8 Dct;

  ASSERT ((MemPstate == 0) || (MemPstate == 1));
  ASSERT (NBPtr->MemPstate == ((MemNGetBitFieldNb (NBPtr, BFMemPsSel) == 0) ? MEMORY_PSTATE0 : MEMORY_PSTATE1));

  IDS_HDT_CONSOLE (MEM_SETREG, "\nGo to Memory Pstate Conext %d\n", MemPstate);
  Dct = NBPtr->Dct;
  MemNSwitchDCTNb (NBPtr, 0);
  // Figure out what is the master channel
  PSMasterChannel = (UINT8) (MemNGetBitFieldNb (NBPtr, BFPhyPSMasterChannel) >> 8);

  // Switch to the master channel to change PStateToAccess
  // PStateToAccess is only effective on the master channel
  MemNSwitchDCTNb (NBPtr, PSMasterChannel);
  MemNSetBitFieldNb (NBPtr, BFMemPsSel, MemPstate);
  MemNSetBitFieldNb (NBPtr, BFPStateToAccess, MemPstate << 8);

  NBPtr->MemPstate = (MemPstate == 0) ? MEMORY_PSTATE0 : MEMORY_PSTATE1;
  MemNSwitchDCTNb (NBPtr, Dct);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function allocates buffer for NB register table
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Handle   - Handle for heap allocation for NBRegTable
 *
 *     @return      TRUE  - Successfully allocates buffer the first time
 *     @return      FALSE - Buffer already allocated or fails to allocate
 */

BOOLEAN
MemNAllocateNBRegTableNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       NB_REG_TAB_HANDLE  Handle
  )
{
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  LOCATE_HEAP_PTR LocHeap;

  // If NBRegTable for this family exists, use it
  LocHeap.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_NB_REG_TABLE, Handle, 0, 0);
  if (HeapLocateBuffer (&LocHeap, &(NBPtr->MemPtr->StdHeader)) == AGESA_SUCCESS) {
    NBPtr->NBRegTable = (TSEFO *) LocHeap.BufferPtr;
    return FALSE;
  }

  // Allocate new buffer for NBRegTable if it has not been allocated
  AllocHeapParams.RequestedBufferSize = sizeof (TSEFO) * BFEndOfList;
  AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_NB_REG_TABLE, Handle, 0, 0);
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (AGESA_SUCCESS != HeapAllocateBuffer (&AllocHeapParams, &(NBPtr->MemPtr->StdHeader))) {
    ASSERT(FALSE); // NB and Tech Block Heap allocate error
    return FALSE;
  }
  NBPtr->NBRegTable = (TSEFO *)AllocHeapParams.BufferPtr;
  return TRUE;
}
