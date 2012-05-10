/* $NoKeywords:$ */
/**
 * @file
 *
 * mnphyln.c
 *
 * Northbridge Phy support for LN
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/LN)
 * @e \$Revision: 48400 $ @e \$Date: 2011-03-08 16:28:12 +0800 (Tue, 08 Mar 2011) $
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
#include "mnln.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_LN_MNPHYLN_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define MAX_CS_PER_CHANNEL_LN 4

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
MemNBeforeDQSTrainingLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;

  MemTBeginTraining (NBPtr->TechPtr);

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_LN; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 1);
      MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 0);

      MemNSetBitFieldNb (NBPtr, BFRxMaxDurDllNoLock, 0);
      MemNSetBitFieldNb (NBPtr, BFTxMaxDurDllNoLock, 0);
      MemNSetBitFieldNb (NBPtr, BFEnRxPadStandby, 0);

      // Enable cut through mode for NB P0
      MemNSetBitFieldNb (NBPtr, BFDisCutThroughMode, 0);

      MemNSetBitFieldNb (NBPtr, BFMaxLatency, 0x12);
    }
    MemNSetBitFieldNb (NBPtr, BFTraceModeEn, 0);
  }

  MemNSetBitFieldNb (NBPtr, BFPrefCpuDis, 1);
  MemNSetBitFieldNb (NBPtr, BFDctWrLimit, 0x1F);

  MemNSetBitFieldNb (NBPtr, BFEnCpuSerRdBehindNpIoWr, 1);   // #158498

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
MemNAfterDQSTrainingLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_LN; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 0);
      MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 2);

      MemNSetBitFieldNb (NBPtr, BFAddrCmdTriEn, 1);
    }
  }

  MemNSetBitFieldNb (NBPtr, BFPrefCpuDis, 0);
  MemNSetBitFieldNb (NBPtr, BFDctWrLimit, 0x1C);
  MemNSetBitFieldNb (NBPtr, BFDramTrainPdbDis, 1);

  MemNSetBitFieldNb (NBPtr, BFEnCpuSerRdBehindNpIoWr, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function returns the number of chipselects per channel of Llano.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return
 */

UINT8
MemNCSPerChannelLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return MAX_CS_PER_CHANNEL_LN;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function overrides the seed for Pass N hardware based RcvEn training of UNB.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *PrevPassRcvEnDly - Pointer to the PrevPassRcvEnDly
 *
 *     @return    TRUE
 */

BOOLEAN
MemNOverridePrevPassRcvEnDlyLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *PrevPassRcvEnDly
  )
{
  if ((*(UINT16*)PrevPassRcvEnDly) < 0x20) {
    *(UINT16*)PrevPassRcvEnDly += 0x40;
  }
  return TRUE;
}

