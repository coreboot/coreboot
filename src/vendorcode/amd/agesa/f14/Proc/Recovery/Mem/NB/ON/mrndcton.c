/* $NoKeywords:$ */
/**
 * @file
 *
 * mrndcton.c
 *
 * Northbridge DCT support for Ontario Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 48803 $ @e \$Date: 2011-03-10 20:18:28 -0700 (Thu, 10 Mar 2011) $
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
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Ids.h"
#include "amdlib.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mru.h"
#include "mrnon.h"
#include "cpuFamilyTranslation.h"
#include "cpuCommonF14Utilities.h"
#include "Filecode.h"
#define FILECODE PROC_RECOVERY_MEM_NB_ON_MRNDCTON_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define RECDEF_CSMASK_REG         0x00003FE0
#define RECDEF_DRAM_BASE_REG      0x00000003

#define MAX_RD_DQS_DLY  0x1F
#define DEFAULT_WR_ODT_ON_ON 6
#define DEFAULT_RD_ODT_ON_ON 6
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
 *   This function gets platform specific config/timing values from the interface layer and
 *   programs them into DCT.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - An Error value lower than AGESA_ERROR may have occurred
 *     @return          FALSE - An Error value greater than or equal to AGESA_ERROR may have occurred
 */

BOOLEAN
MemRecNPlatformSpecON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 AddrTmgValue;
  UINT32 DrvStrValue;
  UINT32 RODTCSLow;
  UINT32 WODTCSLow;
  CH_DEF_STRUCT *ChannelPtr;

  ChannelPtr = NBPtr->ChannelPtr;
  if (ChannelPtr->SODimmPresent != 0) {
    // SODIMM
    if (ChannelPtr->Dimms == 2) {
      AddrTmgValue = 0x00000039;
      DrvStrValue = 0x20222323;
    } else {
      AddrTmgValue = 0;
      DrvStrValue = 0x00002223;
    }
  } else {
    // UDIMM
    if (ChannelPtr->Dimms == 2) {
      AddrTmgValue = 0x00390039;
      DrvStrValue = 0x30222322;
    } else {
      AddrTmgValue = 0;
      DrvStrValue = 0x00112222;
      if (ChannelPtr->DimmDrPresent != 0) {
        AddrTmgValue = 0x003B0000;
      }
    }
  }
  MemRecNSetBitFieldNb (NBPtr, BFODCControl, DrvStrValue);
  MemRecNSetBitFieldNb (NBPtr, BFAddrTmgControl, AddrTmgValue);
  RODTCSLow = 0;
  if (ChannelPtr->Dimms == 2) {
    RODTCSLow = 0x01010404;
    WODTCSLow = 0x09050605;
  } else if (NBPtr->ChannelPtr->DimmDrPresent != 0) {
    WODTCSLow = 0x00000201;
    if (NBPtr->DimmToBeUsed == 1) {
      WODTCSLow = 0x08040000;
    }
  } else {
    WODTCSLow = 0x00000001;
    if (NBPtr->DimmToBeUsed == 1) {
      WODTCSLow = 0x00040000;
    }
  }
  MemRecNSetBitFieldNb (NBPtr, BFPhyRODTCSLow, RODTCSLow);
  MemRecNSetBitFieldNb (NBPtr, BFPhyWODTCSLow, WODTCSLow);

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function  sets the maximum round-trip latency in the system from the processor to the DRAM
 *   devices and back.

 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]     MaxRcvEnDly - Maximum receiver enable delay value
 *
 */

VOID
MemRecNSetMaxLatencyON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  )
{
  UINT32 N;
  UINT32 T;
  UINT32 P;
  UINT32 Px2;
  UINT32 MemClkPeriod;

  T = MemRecNTotalSyncComponentsClientNb (NBPtr);

  // P = P + CEIL(MAX (total delay in DqsRcvEn + RdDqsTime))
  P = (MaxRcvEnDly + MAX_RD_DQS_DLY + 31) / 32;

  MemClkPeriod = 1000000 / DDR800_FREQUENCY;

  // P = P + 6.5
  // T = T + 2586 ps
  Px2 = (P * 2) + 13;
  T += 2586;

  // N = (P/(MemClkFreq * 2) + T) * NclkFreq
  N = ((((Px2 * MemClkPeriod + 3) / 4) + T) * NBPtr->NBClkFreq + 999999) / 1000000;

  MemRecNSetBitFieldNb (NBPtr, BFMaxLatency, N);
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *   Set Dram ODT for mission mode and write leveling mode.
 *
 *     @param[in,out]   *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in]       OdtMode    - Mission mode or write leveling mode
 *     @param[in]       ChipSelect - Chip select number
 *     @param[in]       TargetCS   - Chip select number that is being trained
 *
 */

VOID
MemRecNSetDramOdtON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       ODT_MODE OdtMode,
  IN       UINT8 ChipSelect,
  IN       UINT8 TargetCS
  )
{
  UINT8 Dimms;
  UINT8 DramTerm;
  UINT8 DramTermDyn;
  UINT8 WrLvOdt;
  UINT8 MaxDimmsPerChannel;

  Dimms = NBPtr->ChannelPtr->Dimms;

  // Dram nominal termination
  if (Dimms == 1) {
    DramTerm = 2;   // 120 Ohms
    DramTermDyn = 0;    // Disabled
  } else {
    DramTerm = 3;   // 40 Ohms
    DramTermDyn = 2;    // 120 Ohms
  }

  if (OdtMode == WRITE_LEVELING_MODE) {
    if (ChipSelect == TargetCS) {
      if (Dimms >= 2) {
      DramTerm = DramTermDyn;
      }

      MaxDimmsPerChannel = RecGetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, 0, NBPtr->ChannelPtr->ChannelID);

      if (MaxDimmsPerChannel == 2) {
        if (Dimms == 2) {
          WrLvOdt = 5;
        } else {
          // Dimms = 1
          if (TargetCS == 0) {
            WrLvOdt = 1;
          } else {
            // TargetCS = 2
            WrLvOdt = 4;
          }
        }
      } else {
        WrLvOdt = 1;
      }
      MemRecNSetBitFieldNb (NBPtr, BFWrLvOdt, WrLvOdt);
    }
  }
  MemRecNSetBitFieldNb (NBPtr, BFDramTerm, DramTerm);
  MemRecNSetBitFieldNb (NBPtr, BFDramTermDyn, DramTermDyn);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function programs the memory controller with configuration parameters
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - An Error value lower than AGESA_ERROR may have occurred
 *     @return          FALSE - An Error value greater than or equal to AGESA_ERROR may have occurred
 */

BOOLEAN
MemRecNAutoConfigON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dimm;
  UINT8 ChipSel;
  UINT32 CSBase;
  UINT32 NBClkFreq;
  UINT8 i;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;

  DCTPtr = NBPtr->DCTPtr;
  ChannelPtr = NBPtr->ChannelPtr;

  // Force NB P-state to NBP0
  F14NbPstateInit (DDR800_FREQUENCY,
                          6,
                          0,
                          &NBClkFreq,
                          &(NBPtr->MemPtr->StdHeader));
  MemRecNSetBitFieldNb (NBPtr, BFNbPsCtrlDis, 1);

  //Prepare variables for future usage.
  for (Dimm = 0; Dimm < 2; Dimm++) {
    if ((ChannelPtr->ChDimmValid & (UINT8) 1 << Dimm) != 0) {
      DCTPtr->Timings.CsPresent |= (UINT16) 1 << (Dimm * 2);
      if (((ChannelPtr->DimmDrPresent & (UINT8) 1 << Dimm) == 0) && ((ChannelPtr->DimmQrPresent & (UINT8) 1 << Dimm) == 0)) {
        continue;
      } else {
        DCTPtr->Timings.CsPresent |= (UINT16) 1 << (Dimm * 2 + 1);
      }
    }
  }

  //Temporarily set all CS Base/Limit registers (corresponding to Dimms exist on a channel) with 256MB size for WL training.
  CSBase = 0;
  for (ChipSel = 0; ChipSel < 4; ChipSel++) {
    if (DCTPtr->Timings.CsPresent & (UINT8) 1 << ChipSel) {

      CSBase &= (UINT32) ~0x08; //Clear OnDimmMirror bit.
      if (((ChipSel & 1) != 0) && ((ChannelPtr->DimmMirrorPresent & (UINT8) 1 << (ChipSel >> 1)) != 0)) {
        CSBase |= (UINT32) 0x08; //Set OnDimmMirror bit.
      }
      MemRecNSetBitFieldNb (NBPtr, (BFCSBaseAddr0Reg + ChipSel), (CSBase | 0x01));
      CSBase += 0x100000;
      if ((ChipSel & 1) == 0) {
        MemRecNSetBitFieldNb (NBPtr, (BFCSMask0Reg + (ChipSel >> 1)), RECDEF_CSMASK_REG);
      }
    }
  }
  MemRecNSetBitFieldNb (NBPtr, BFDramBaseReg0, RECDEF_DRAM_BASE_REG);
  MemRecNSetBitFieldNb (NBPtr, BFDramLimitReg0, 0x70000);

  // Use default values for common registers
  i = 0;
  while (NBPtr->RecModeDefRegArray[i] != NULL) {
    MemRecNSetBitFieldNb (NBPtr, NBPtr->RecModeDefRegArray[i], NBPtr->RecModeDefRegArray[i + 1]);
    i += 2;
  }

  //======================================================================
  // Build Dram Config Misc Register Value
  //======================================================================
  //
  // Max out Non-SPD timings
  MemRecNSetBitFieldNb (NBPtr, BFTwrrdSD, 0xA);
  MemRecNSetBitFieldNb (NBPtr, BFTrdrdSD, 0x8);
  MemRecNSetBitFieldNb (NBPtr, BFTwrwrSD, 0x9);

  MemRecNSetBitFieldNb (NBPtr, BFWrOdtOnDuration, DEFAULT_WR_ODT_ON_ON);
  MemRecNSetBitFieldNb (NBPtr, BFRdOdtOnDuration, DEFAULT_RD_ODT_ON_ON);
  MemRecNSetBitFieldNb (NBPtr, BFWrOdtTrnOnDly, 0);

  MemRecNSetBitFieldNb (NBPtr, BFRdOdtTrnOnDly, 6 - 5);
  //======================================================================
  // DRAM MRS Register, set ODT
  //======================================================================
  MemRecNSetBitFieldNb (NBPtr, BFBurstCtrl, 1);

  //
  // Recommended registers setting BEFORE DRAM device initialization and training
  //
  MemRecNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 1);
  MemRecNSetBitFieldNb (NBPtr, BFZqcsInterval, 0);
  MemRecNSetBitFieldNb (NBPtr, BFRxMaxDurDllNoLock, 0);
  MemRecNSetBitFieldNb (NBPtr, BFTxMaxDurDllNoLock, 0);
  MemRecNSetBitFieldNb (NBPtr, BFEnRxPadStandby, 0);
  MemRecNSetBitFieldNb (NBPtr, BFPrefCpuDis, 1);
  MemRecNSetBitFieldNb (NBPtr, BFDctWrLimit, 0x1F);
  MemRecNSetBitFieldNb (NBPtr, BFEnCpuSerRdBehindNpIoWr, 1);
  MemRecNSetBitFieldNb (NBPtr, BFDbeGskMemClkAlignMode, 0);
  MemRecNSetBitFieldNb (NBPtr, BFMaxLatency, 0x12);
  MemRecNSetBitFieldNb (NBPtr, BFTraceModeEn, 0);

  // Enable cut through mode for NB P0
  MemRecNSetBitFieldNb (NBPtr, BFDisCutThroughMode, 0);

  return TRUE;
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
MemRecNOverrideRcvEnSeedON (
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


