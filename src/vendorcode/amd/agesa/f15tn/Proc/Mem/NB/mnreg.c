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
