/* $NoKeywords:$ */
/**
 * @file
 *
 * mrndctln.c
 *
 * Northbridge DCT support for Llano Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Ids.h"
#include "amdlib.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mru.h"
#include "mrnln.h"
#include "cpuFamilyTranslation.h"
#include "cpuCommonF12Utilities.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_RECOVERY_MEM_NB_LN_MRNDCTLN_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define RECDEF_DRAM_CONTROL_REG   0x14042A08
#define RECDEF_DRAM_MRSREG        0x000400A5
#define RECDEF_DRAM_TIMING_LO     0x00000002
#define RECDEF_DRAM_TIMING_HI     0x02D218FF
#define RECDEF_CSMASK_REG         0x00003FE0
#define RECDEF_DRAM_CONFIG_HI_REG 0x1E000000
#define RECDEF_DRAM_BASE_REG      0x00000003
#define RECDEF_DRAM_TIMING_0      0x0A000101
#define RECDEF_DRAM_TIMING_1      0

#define MAX_RD_DQS_DLY  0x1F
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
MemRecNPlatformSpecLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 AddrTmgValue;
  UINT32 DrvStrValue;
  CH_DEF_STRUCT *ChannelPtr;

  ChannelPtr = NBPtr->ChannelPtr;

  if (ChannelPtr->SODimmPresent != 0) {
    // SODIMM
    if (ChannelPtr->Dimms == 2) {
      AddrTmgValue = 0x00000039;
      DrvStrValue = 0x20222323;
    } else {
      AddrTmgValue = 0;
      DrvStrValue = 0x00002222;
    }
  } else {
    // UDIMM
    if (ChannelPtr->Dimms == 2) {
      AddrTmgValue = 0x00390039;
      DrvStrValue = 0x20222322;
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
MemRecNSetMaxLatencyLN (
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

  // P = P + 8.5
  // T = T + 2586 ps
  Px2 = (P * 2) + 17;
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
MemRecNSetDramOdtLN (
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
      DramTerm = DramTermDyn;

      MaxDimmsPerChannel = RecGetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, 0, NBPtr->ChannelPtr->ChannelID);

      if (MaxDimmsPerChannel == 2) {
        if (Dimms == 2) {
          WrLvOdt = 5;
        } else {
          // Dimms = 1
          if (TargetCS == 0) {
            WrLvOdt = 0xF;
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
MemRecNAutoConfigLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dimm;
  UINT8 Dct;
  UINT8 ChipSel;
  UINT32 CSBase;
  UINT32 NBClkFreq;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;

  Dct = NBPtr->Dct;
  DCTPtr = NBPtr->DCTPtr;
  ChannelPtr = NBPtr->ChannelPtr;

  // Force NB P-state to NBP0
  F12NbPstateInit (DDR800_FREQUENCY,
                          6,
                          0,
                          &NBClkFreq,
                          &(NBPtr->MemPtr->StdHeader));
  NBPtr->NBClkFreq = NBClkFreq;
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

  // Disable the other DCT
  NBPtr->MemRecNSwitchDctNb (NBPtr, Dct ^ 0x01);
  MemRecNSetBitFieldNb (NBPtr, BFDisDramInterface, 1);
  NBPtr->MemRecNSwitchDctNb (NBPtr, Dct);
  if (Dct != 0) {
    // If DCT 1, set DctSelBase registers
    MemRecNSetBitFieldNb (NBPtr, BFDctSelBaseAddrReg, 0x00000003);
    MemRecNSetBitFieldNb (NBPtr, BFDctSelBaseOffsetReg, 0x00000000);
  }

  MemRecNSetBitFieldNb (NBPtr, BFDramBankAddrReg, 0x00000011);

  // Set timing registers
  MemRecNSetBitFieldNb (NBPtr, BFDramTiming0, RECDEF_DRAM_TIMING_0);
  MemRecNSetBitFieldNb (NBPtr, BFDramTiming1, RECDEF_DRAM_TIMING_1);
  MemRecNSetBitFieldNb (NBPtr, BFDramTimingLoReg, RECDEF_DRAM_TIMING_LO);
  MemRecNSetBitFieldNb (NBPtr, BFDramTimingHiReg, RECDEF_DRAM_TIMING_HI);
  MemRecNSetBitFieldNb (NBPtr, BFDramMRSReg, RECDEF_DRAM_MRSREG);
  MemRecNSetBitFieldNb (NBPtr, BFDramControlReg, RECDEF_DRAM_CONTROL_REG);

  // Set DRAM Config High Register
  MemRecNSetBitFieldNb (NBPtr, BFDramConfigHiReg, RECDEF_DRAM_CONFIG_HI_REG);

  // DctWrLimit = 0x1F
  MemRecNSetBitFieldNb (NBPtr, BFDctWrLimit, 0x1F);
  // EnCpuSerRdBehindNpIoWr = 1
  MemRecNSetBitFieldNb (NBPtr, BFEnCpuSerRdBehindNpIoWr, 1);


  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


