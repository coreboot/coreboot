/* $NoKeywords:$ */
/**
 * @file
 *
 * mnmctor.c
 *
 * Northbridge Orochi MCT supporting functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/OR)
 * @e \$Revision: 51634 $ @e \$Date: 2011-04-26 03:12:52 -0600 (Tue, 26 Apr 2011) $
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
#include "cpuFeatures.h"
#include "mnor.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE PROC_MEM_NB_OR_MNMCTOR_FILECODE
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
 *   This function sets final values for specific registers.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNFinalizeMctOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_DATA_STRUCT *MemPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  DRAM_PREFETCH_MODE DramPrefetchMode;
  UINT16 Speed;
  UINT32 Value32;
  UINT8 DcqBwThrotWm;
  UINT8 DcqBwThrotWm1;
  UINT8 DcqBwThrotWm2;
  UINT8 Dct;

  MemPtr = NBPtr->MemPtr;
  RefPtr = MemPtr->ParameterListPtr;
  DramPrefetchMode = MemPtr->PlatFormConfig->PlatformProfile.AdvancedPerformanceProfile.DramPrefetchMode;
  Speed = NBPtr->DCTPtr->Timings.Speed;
  //
  // F2x11C
  //
  // FlushWrOnStpGnt = 0
  // PrefThreeConf = 110b
  // PrefTwoConf = 011b
  // PrefOneConf = 10b
  // PrefConfSat = 00b
  // PrefIoDis = 0
  // PrefCpuDis = 0
  // MctWrLimit = 10h
  // DctWrLimit = 01b
  Value32 = MemNGetBitFieldNb (NBPtr, BFMctCfgHiReg);
  Value32 &= 0xD003CF80;
  Value32 |= 0x0CE00041;
  MemNSetBitFieldNb (NBPtr, BFMctCfgHiReg, Value32);

  if (DramPrefetchMode == DISABLE_DRAM_PREFETCH_FOR_IO || DramPrefetchMode == DISABLE_DRAM_PREFETCHER) {
    MemNSetBitFieldNb (NBPtr, BFPrefIoDis, 1);
  }

  if (DramPrefetchMode == DISABLE_DRAM_PREFETCH_FOR_CPU || DramPrefetchMode == DISABLE_DRAM_PREFETCHER) {
    MemNSetBitFieldNb (NBPtr, BFPrefCpuDis, 1);
  }

  if (Speed == DDR667_FREQUENCY) {
    DcqBwThrotWm = 0;
    DcqBwThrotWm1 = 3;
    DcqBwThrotWm2 = 4;
  } else if (Speed == DDR800_FREQUENCY) {
    DcqBwThrotWm = 0;
    DcqBwThrotWm1 = 3;
    DcqBwThrotWm2 = 5;
  } else if (Speed == DDR1066_FREQUENCY) {
    DcqBwThrotWm = 0;
    DcqBwThrotWm1 = 4;
    DcqBwThrotWm2 = 6;
  } else if (Speed == DDR1333_FREQUENCY) {
    DcqBwThrotWm = 0;
    DcqBwThrotWm1 = 5;
    DcqBwThrotWm2 = 8;
  } else if  (Speed == DDR1600_FREQUENCY) {
    DcqBwThrotWm = 0;
    DcqBwThrotWm1 = 6;
    DcqBwThrotWm2 = 9;
  } else if  (Speed == DDR1866_FREQUENCY) {
    DcqBwThrotWm = 0;
    DcqBwThrotWm1 = 7;
    DcqBwThrotWm2 = 10;
  } else {
    DcqBwThrotWm = 0;
    DcqBwThrotWm1 = 8;
    DcqBwThrotWm2 = 12;
  }

  //
  // F2x1B0
  //
  // DcqBwThrotWm
  // PrefFiveConf = 111b
  // PrefFourConf = 111b
  // EnSplitDctLimits = 1
  // CohPrefPrbLmt = IF (PrbFltrEn) THEN 000b ELSE 001b ENDIF
  // AdapPrefNegativeStep = 00b
  // AdapPrefPositiveStep = 00b
  // AdapPrefMissRatio = 01b
  Value32 = MemNGetBitFieldNb (NBPtr, BFExtMctCfgLoReg);
  Value32 &= 0x003FE8C0;
  Value32 |= 0x0FC01001;
  Value32 |= (UINT32) DcqBwThrotWm << 28;
  MemNSetBitFieldNb (NBPtr, BFExtMctCfgLoReg, Value32);

  //
  // F2x1B4
  //
  // DcqBwThrotWm2
  // DcqBwThrotWm1
  Value32 = MemNGetBitFieldNb (NBPtr, BFExtMctCfgHiReg);
  Value32 &= 0xF7FFFC00;
  Value32 |= (((UINT32) DcqBwThrotWm2 << 5) | (UINT32) DcqBwThrotWm1);
  // FlushWrOnS3StpGnt to 1
  Value32 |= (UINT32) 1 << 27;
  MemNSetBitFieldNb (NBPtr, BFExtMctCfgHiReg, Value32);

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_OR; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);

    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      //
      // Phy Power Saving
      //
      MemNPhyPowerSavingUnb (NBPtr);
      //
      // Power Down Enable
      //
      if (NBPtr->RefPtr->EnablePowerDown) {
        MemNSetBitFieldNb (NBPtr, BFPowerDownEn, 1);
      }
    }
  }

  // Set LockDramCfg
  if (IsFeatureEnabled (C6Cstate, NBPtr->MemPtr->PlatFormConfig, &(NBPtr->MemPtr->StdHeader))) {
    IDS_SKIP_HOOK (IDS_TRACE_MODE, NBPtr, &NBPtr->MemPtr->StdHeader) {
        MemNSetBitFieldNb (NBPtr, BFLockDramCfg, 1);
    }
  }

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}


/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
 /*-----------------------------------------------------------------------------
 *
 *
 *     This function handles scrubber register settings for orochi.
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - If the function is called before scrub rate is set or after
 *                                  TRUE    function is called before scrub rate is set
 *                                  FALSE   function is called after scrub rate is set
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNScrubberErratumOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 L3Scrub;
  UINT8 DramScrub;

  if (*(BOOLEAN *) OptParam == TRUE) {
    MemNSwitchDCTNb (NBPtr, 0);
  } else {
    ASSERT (NBPtr->Dct == 0);
    L3Scrub = (UINT8) MemNGetBitFieldNb (NBPtr, BFL3Scrub);
    DramScrub = (UINT8) MemNGetBitFieldNb (NBPtr, BFDramScrub);

    // Set scrubber for DCT1
    MemNSwitchDCTNb (NBPtr, 1);
    MemNSetBitFieldNb (NBPtr, BFL3Scrub, L3Scrub);
    MemNSetBitFieldNb (NBPtr, BFDramScrub, DramScrub);
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function changes DataTxFifoWrDly based on training result of WrDatDly
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 */

BOOLEAN
MemNDataTxFifoWrDlyOverrideOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 i;
  UINT8 Dct;
  UINT8 ByteLane;
  UINT16 CsEnabled;
  BOOLEAN DataTxFifoWrDly;

  // This should be done only after DQS Position training
  if (NBPtr->NbFreqChgState <= 1) {
    return TRUE;
  }

  // Set DataTxFifoWrDly based on WrDatDly
  // if all WrDatDly of populated dimms on a DCT are equal to 7h
  // Set DataTxFifoWrDly to >= 2h
  for (Dct = 0; Dct < 2; Dct ++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    DataTxFifoWrDly = TRUE;
    CsEnabled = NBPtr->DCTPtr->Timings.CsEnabled;
    for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i ++) {
      if ((CsEnabled & (UINT16) (3 << (i << 1))) != 0) {
        for (ByteLane = 0; ByteLane < (NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
          if ((GetTrainDlyFromHeapNb (NBPtr, AccessWrDatDly, DIMM_BYTE_ACCESS (i, ByteLane)) >> 5) != 7) {
            DataTxFifoWrDly = FALSE;
            break;
          }
        }
        if (!DataTxFifoWrDly) {
          break;
        }
      }
    }

    if (DataTxFifoWrDly) {
      if (MemNGetBitFieldNb (NBPtr, BFDataTxFifoWrDly) < 2) {
        MemNSetBitFieldNb (NBPtr, BFDataTxFifoWrDly, 2);
      }
    }
  }

  return TRUE;
}
