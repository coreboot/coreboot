/* $NoKeywords:$ */
/**
 * @file
 *
 * mnphyon.c
 *
 * Northbridge Phy support for ON
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/ON)
 * @e \$Revision: 37401 $ @e \$Date: 2010-09-03 05:32:06 +0800 (Fri, 03 Sep 2010) $
 *
 **/
/*
 *****************************************************************************
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
#include "amdlib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mu.h"
#include "merrhdl.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnon.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_NB_ON_MNPHYON_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define MAX_CS_PER_CHANNEL_ON 4
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
 *   This is a general purpose function that executes before DRAM training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNBeforeDQSTrainingON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemTBeginTraining (NBPtr->TechPtr);

  MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 1);
  MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 0);
  MemNSetBitFieldNb (NBPtr, BFRxMaxDurDllNoLock, 0);
  MemNSetBitFieldNb (NBPtr, BFTxMaxDurDllNoLock, 0);
  MemNSetBitFieldNb (NBPtr, BFEnRxPadStandby, 0);
  MemNSetBitFieldNb (NBPtr, BFPrefCpuDis, 1);
  MemNSetBitFieldNb (NBPtr, BFDctWrLimit, 0x1F);
  MemNSetBitFieldNb (NBPtr, BFEnCpuSerRdBehindNpIoWr, 1);
  MemNSetBitFieldNb (NBPtr, BFDbeGskMemClkAlignMode, 0);
  MemNSetBitFieldNb (NBPtr, BFMaxLatency, 0x12);
  MemNSetBitFieldNb (NBPtr, BFTraceModeEn, 0);

  // Enable cut through mode for NB P0
  MemNSetBitFieldNb (NBPtr, BFDisCutThroughMode, 0);

  MemTEndTraining (NBPtr->TechPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This is a general purpose function that executes after DRAM training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNAfterDQSTrainingON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if (NBPtr->DCTPtr->Timings.Speed <= DDR1066_FREQUENCY) {
    MemNSetBitFieldNb (NBPtr, BFAddrCmdTriEn, 1);
  }
  MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 0);
  MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 2);
  MemNSetBitFieldNb (NBPtr, BFEnRxPadStandby, (NBPtr->DCTPtr->Timings.Speed <= DDR1600_FREQUENCY) ? 0x1000 : 0);
  MemNSetBitFieldNb (NBPtr, BFPrefCpuDis, 0);
  MemNSetBitFieldNb (NBPtr, BFDctWrLimit, 0x1C);
  MemNSetBitFieldNb (NBPtr, BFDramTrainPdbDis, 1);
  MemNSetBitFieldNb (NBPtr, BFEnCpuSerRdBehindNpIoWr, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function returns the number of chipselects per channel of Ontario.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return
 */

UINT8
MemNCSPerChannelON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return MAX_CS_PER_CHANNEL_ON;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function overrides the seed for hardware based RcvEn training of Ontario.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *SeedPtr - Pointer to the seed value.
 *
 *     @return    TRUE
 */

BOOLEAN
MemNOverrideRcvEnSeedON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  )
{
  *(UINT16*) SeedPtr = 0x5B;
  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function force the Rd Dqs Delay to phase B (0x20)
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *RdDqsDlyPtr - Pointer to Rd DQS delay.
 *
 *     @return    TRUE
 */

BOOLEAN
MemNForceRdDqsPhaseBON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *RdDqsDlyPtr
  )
{
  if ((NBPtr->DCTPtr->Timings.Speed == DDR1333_FREQUENCY) && (*(UINT8 *) RdDqsDlyPtr < 0x20)) {
    *(UINT8 *) RdDqsDlyPtr = 0x20;
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function resets RxFifo pointer during Read DQS training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *OptParam - Optional parameter
 *
 *     @return    TRUE
 */

BOOLEAN
MemNResetRxFifoPtrON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  if (NBPtr->TechPtr->Direction == DQS_READ_DIR) {
    MemNSetBitFieldNb (NBPtr, BFRxPtrInitReq, 1);
    MemNPollBitFieldNb (NBPtr, BFRxPtrInitReq, 0, PCI_ACCESS_TIMEOUT, FALSE);
  }
  return TRUE;
}
