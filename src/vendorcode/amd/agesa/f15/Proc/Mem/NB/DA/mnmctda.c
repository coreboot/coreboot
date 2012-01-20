/* $NoKeywords:$ */
/**
 * @file
 *
 * mnmctda.c
 *
 * Northbridge DA MCT supporting functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/DA)
 * @e \$Revision: 56279 $ @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2012 Advanced Micro Devices, Inc.
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
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnda.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_NB_DA_MNMCTDA_FILECODE
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

extern BUILD_OPT_CFG UserOptions;


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets final values in BUCFG and BUCFG2
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNFinalizeMctDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  MEM_DATA_STRUCT *MemPtr;
  DRAM_PREFETCH_MODE DramPrefetchMode;
  S_UINT64 SMsr;

  MemPtr = NBPtr->MemPtr;
  DramPrefetchMode = MemPtr->PlatFormConfig->PlatformProfile.AdvancedPerformanceProfile.DramPrefetchMode;
  MemNSetBitFieldNb (NBPtr, BFAdapPrefMissRatio, 1);
  MemNSetBitFieldNb (NBPtr, BFAdapPrefPosStep, 0);
  MemNSetBitFieldNb (NBPtr, BFAdapPrefNegStep, 0);
  MemNSetBitFieldNb (NBPtr, BFCohPrefPrbLmt, 1);
  // Recommended settings for F2x11C
  MemNSetBitFieldNb (NBPtr, BFMctWrLimit, 16);
  MemNSetBitFieldNb (NBPtr, BFPrefCpuDis, 0);
  MemNSetBitFieldNb (NBPtr, BFPrefIoDis, 0);
  MemNSetBitFieldNb (NBPtr, BFFlushWrOnStpGnt, 1);

  if (DramPrefetchMode == DISABLE_DRAM_PREFETCH_FOR_IO || DramPrefetchMode == DISABLE_DRAM_PREFETCHER) {
    MemNSetBitFieldNb (NBPtr, BFPrefIoDis, 1);
  }

  if (DramPrefetchMode == DISABLE_DRAM_PREFETCH_FOR_CPU || DramPrefetchMode == DISABLE_DRAM_PREFETCHER) {
    MemNSetBitFieldNb (NBPtr, BFPrefCpuDis, 1);
  }
  // For power saving
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      if (NBPtr->ChannelPtr->Dimmx4Present == 0) {
        MemNSetBitFieldNb (NBPtr, BFPhy0x0D0F0F13, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D0F0F13) | 0x80));
      }
      if (!NBPtr->MCTPtr->Status[SbEccDimms]) {
        MemNSetBitFieldNb (NBPtr, BFPhy0x0D0F0830, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D0F0830) | 0x10));
      }
      MemNSetBitFieldNb (NBPtr, BFPhy0x0D07812F, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D07812F) | 0x01));
    }
  }

  if (NBPtr->Node == BSP_DIE) {
    if (!NBPtr->ClToNbFlag) {
      LibAmdMsrRead (BU_CFG2, (UINT64 *)&SMsr, &MemPtr->StdHeader);
      SMsr.lo &= ~((UINT32)1 << 15);                // ClLinesToNbDis
      LibAmdMsrWrite (BU_CFG2, (UINT64 *)&SMsr, &MemPtr->StdHeader);
    }

    LibAmdMsrRead (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
    SMsr.hi &= ~((UINT32)1 << (48 - 32));               // WbEnhWsbDis
    LibAmdMsrWrite (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  }

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets initial values in BUCFG and BUCFG2
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNInitializeMctDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_DATA_STRUCT *MemPtr;
  S_UINT64 SMsr;

  MemPtr = NBPtr->MemPtr;

  if (NBPtr->Node == BSP_DIE) {
    LibAmdMsrRead (BU_CFG2, (UINT64 *)&SMsr, &MemPtr->StdHeader);
    if (SMsr.lo & ((UINT32)1 << 15)) {
      NBPtr->ClToNbFlag = 1;
    }
    SMsr.lo |= (UINT32)1 << 15;                   // ClLinesToNbDis
    LibAmdMsrWrite (BU_CFG2, (UINT64 *)&SMsr, &MemPtr->StdHeader);

    LibAmdMsrRead (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
    SMsr.hi |= (UINT32)1 << (48 - 32);              // WbEnhWsbDis
    LibAmdMsrWrite (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
    LibAmdMsrWrite (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  }
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
