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
 * @e \$Revision: 11190 $ @e \$Date: 2009-03-02 10:39:45 -0600 (Mon, 02 Mar 2009) $
 *
 **/
/*****************************************************************************
*
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
#include "Filecode.h"
#include "GeneralServices.h"
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
  ASSERT (NBPtr->DctCount > 1);
  NBPtr->Dct = Dct ? 1 : 0;
  NBPtr->MCTPtr->Dct = NBPtr->Dct;
  NBPtr->DCTPtr = &(NBPtr->MCTPtr->DctData[NBPtr->Dct]);
  NBPtr->PsPtr = &(NBPtr->PSBlock[NBPtr->Dct]);
  NBPtr->DctCachePtr = &(NBPtr->DctCache[NBPtr->Dct]);

  MemNSwitchChannelNb (NBPtr, NBPtr->Channel);
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

  ASSERT (FieldName < BFEndOfList);
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
  ASSERT (FieldName < BFEndOfList);
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
  return ((NBPtr->MCTPtr->DieId *NBPtr->DctCount) + Dct);
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
  TimeOut = InitTSC + ((UINT64) MicroSecond * 1600);

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
    // Default event class
    // If different event class is needed in one entry, override it.
    EventClass = AGESA_ERROR;
    switch (FieldName) {
    case BFDramEnabled:
      EventInfo = MEM_ERROR_DRAM_ENABLED_TIME_OUT;
      break;
    case BFDctAccessDone:
      EventInfo = MEM_ERROR_DCT_ACCESS_DONE_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      break;
    case BFSendCtrlWord:
      EventInfo = MEM_ERROR_SEND_CTRL_WORD_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      break;
    case BFPrefDramTrainMode:
      EventInfo = MEM_ERROR_PREF_DRAM_TRAIN_MODE_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      break;
    case BFEnterSelfRef:
      EventInfo = MEM_ERROR_ENTER_SELF_REF_TIME_OUT;
      break;
    case BFFreqChgInProg:
      EventInfo = MEM_ERROR_FREQ_CHG_IN_PROG_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      break;
    case BFExitSelfRef:
      EventInfo = MEM_ERROR_EXIT_SELF_REF_TIME_OUT;
      break;
    case BFSendMrsCmd:
      EventInfo = MEM_ERROR_SEND_MRS_CMD_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      break;
    case BFSendZQCmd:
      EventInfo = MEM_ERROR_SEND_ZQ_CMD_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      break;
    case BFDctExtraAccessDone:
      EventInfo = MEM_ERROR_DCT_EXTRA_ACCESS_DONE_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      break;
    case BFMemClrBusy:
      EventInfo = MEM_ERROR_MEM_CLR_BUSY_TIME_OUT;
      break;
    case BFMemCleared:
      EventInfo = MEM_ERROR_MEM_CLEARED_TIME_OUT;
      break;
    case BFFlushWr:
      EventInfo = MEM_ERROR_FLUSH_WR_TIME_OUT;
      ExcludeDCT = NBPtr->Dct;
      break;
    default:
      EventClass = 0;
      EventInfo = 0;
      IDS_ERROR_TRAP;
    }

    PutEventLog (EventClass, EventInfo, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &MemPtr->StdHeader);
    SetMemError (EventClass, MCTPtr);
    MemPtr->ErrorHandling (MCTPtr, ExcludeDCT, ExcludeChipSelMask, &MemPtr->StdHeader);
  }
}


