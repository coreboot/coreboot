/* $NoKeywords:$ */
/**
 * @file
 *
 * mndctln.c
 *
 * Northbridge LN DCT supporting functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/LN)
 * @e \$Revision: 45647 $ @e \$Date: 2011-01-20 04:53:23 +0800 (Thu, 20 Jan 2011) $
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
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnln.h"
#include "mftds.h"
#include "merrhdl.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuCommonF12Utilities.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_LN_MNDCTLN_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
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

extern BUILD_OPT_CFG UserOptions;

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function programs the memory controller with configuration parameters
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - An Error value lower than AGESA_FATAL may have occurred
 *     @return          FALSE - An Error value greater than or equal to AGESA_FATAL may have occurred
 *     @return          NBPtr->MCTPtr->ErrCode - Contains detailed AGESA_STATUS value
 */

BOOLEAN
MemNAutoConfigLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  UINT8 PowerDownMode;

  MCTPtr = NBPtr->MCTPtr;
  DCTPtr = NBPtr->DCTPtr;

  //======================================================================
  // Build Dram Control Register Value (F2x78)
  //======================================================================
  //

  //======================================================================
  // Build Dram Config Lo Register Value
  //======================================================================
  //
  MemNSetBitFieldNb (NBPtr, BFEnDispAutoPrecharge, 1);

  MemNSetBitFieldNb (NBPtr, BFIdleCycInit, 3);

  //======================================================================
  // Build Dram Config Hi Register Value
  //======================================================================
  //

  PowerDownMode = (UINT8) ((UserOptions.CfgPowerDownMode == POWER_DOWN_MODE_AUTO) ? POWER_DOWN_BY_CHIP_SELECT : UserOptions.CfgPowerDownMode);
  PowerDownMode = (!NBPtr->IsSupported[ChannelPDMode]) ? PowerDownMode : 0;
  IDS_OPTION_HOOK (IDS_POWERDOWN_MODE, &PowerDownMode, &(NBPtr->MemPtr->StdHeader));
  if (PowerDownMode == 1) {
    MemNSetBitFieldNb (NBPtr, BFPowerDownMode, 1);
  }

  MemNSetBitFieldNb (NBPtr, BFPchgPDModeSel, 1);

  MemNSetBitFieldNb (NBPtr, BFDcqBypassMax, 0xE);

  //======================================================================
  // Build Dram Config Misc Register Value
  //======================================================================
  //
  // Max out Non-SPD timings
  MemNSetBitFieldNb (NBPtr, BFNonSPD, 0x18FF);
  MemNSetBitFieldNb (NBPtr, BFNonSPDHi, 0x2A);
  MemNSetBitFieldNb (NBPtr, BFTwrrdSD, 0xA);
  MemNSetBitFieldNb (NBPtr, BFTrdrdSD, 0x8);
  MemNSetBitFieldNb (NBPtr, BFTwrwrSD, 0x9);

  MemNSetBitFieldNb (NBPtr, BFWrOdtOnDuration, DEFAULT_WR_ODT_ON_LN);
  MemNSetBitFieldNb (NBPtr, BFRdOdtOnDuration, DEFAULT_RD_ODT_ON_LN);
  MemNSetBitFieldNb (NBPtr, BFWrOdtTrnOnDly, 0);

  //======================================================================
  // DRAM MRS Register, set ODT
  //======================================================================
  MemNSetBitFieldNb (NBPtr, BFBurstCtrl, 1);

  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends an MRS command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSendMrsCmdLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemNSetASRSRTNb (NBPtr);
  MemNSwapBitsNb (NBPtr);

  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tCS%d MR%d %04x\n",
              (MemNGetBitFieldNb (NBPtr, BFDramInitRegReg) >> 20) & 0xF,
              (MemNGetBitFieldNb (NBPtr, BFDramInitRegReg) >> 16) & 0xF,
              (MemNGetBitFieldNb (NBPtr, BFDramInitRegReg) & 0xFFFF));

  // 1.Set SendMrsCmd=1
  MemNSetBitFieldNb (NBPtr, BFSendMrsCmd, 1);

  // 2.Wait for SendMrsCmd=0
  MemNPollBitFieldNb (NBPtr, BFSendMrsCmd, 0, PCI_ACCESS_TIMEOUT, FALSE);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function  sets the maximum round-trip latency in the system from the processor to the DRAM
 *   devices and back for Llano.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]     MaxRcvEnDly - Maximum receiver enable delay value
 *
 */

VOID
MemNSetMaxLatencyLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  )
{
  UINT32 N;
  UINT32 T;
  UINT32 P;
  UINT32 Px2;
  UINT32 MemClkPeriod;

  AGESA_TESTPOINT (TpProcMemRcvrCalcLatency, &(NBPtr->MemPtr->StdHeader));

  N = 0x50;    // init value for MaxRdLat used in SW RcvEn training, when MaxRcvEnDly==FFFF

  if (MaxRcvEnDly != 0xFFFF) {
    T = MemNTotalSyncComponentsClientNb (NBPtr);

    // P = P + CEIL(MAX (total delay in DqsRcvEn + RdDqsTime))
    P = ((MaxRcvEnDly + MAX_RD_DQS_DLY) + 31) / 32;

    // P = P + 7.5
    // T = T + 2586 ps
    Px2 = (P * 2) + 15;
    T += 2586;

    if (NBPtr->IsSupported[ExtraPclkInMaxRdLat]) {
      Px2 += 2;
    }

    // N = (P/(MemClkFreq * 2) + T) * NclkFreq
    MemClkPeriod = 1000000 / NBPtr->DCTPtr->Timings.Speed;
    N = ((((Px2 * MemClkPeriod + 3) / 4) + T) * NBPtr->NBClkFreq + 999999) / 1000000;
  }

  NBPtr->DCTPtr->Timings.MaxRdLat = (UINT16) N;
  ASSERT (N <= 0x50);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tMaxRdLat: %03x\n", N);
  MemNSetBitFieldNb (NBPtr, BFMaxLatency, N);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function retrieves the Max latency parameters
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @param[in]  *MinDlyPtr - Pointer to variable to store the Minimum Delay value
 *     @param[in]  *MaxDlyPtr - Pointer to variable to store the Maximum Delay value
 *     @param[in]  *DlyBiasPtr - Pointer to variable to store Delay Bias value
 *     @param[in]  MaxDlyForMaxRdLat - Maximum receiver enable delay value
 *
 */

VOID
MemNGetMaxLatParamsClientLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxDlyForMaxRdLat,
  IN OUT   UINT16 *MinDlyPtr,
  IN OUT   UINT16 *MaxDlyPtr,
  IN OUT   UINT16 *DlyBiasPtr
  )
{
  UINT32 P;
  UINT32 Px2;
  UINT32 T;
  UINT32 MemClkPeriod;

  T = MemNTotalSyncComponentsClientNb (NBPtr);

  // P = P + CEIL(MAX (total delay in DqsRcvEn + RdDqsTime))
  P = (MaxDlyForMaxRdLat + 31) / 32;

  // P = P + 8.5
  // T = T + 2586 ps
  Px2 = (P * 2) + 17;
  T += 2586;

  // N = (P/(MemClkFreq * 2) + T) * NclkFreq
  MemClkPeriod = 1000000 / NBPtr->DCTPtr->Timings.Speed;

  *MinDlyPtr = (UINT16) (((((Px2 * MemClkPeriod + 3) / 4) + T) * NBPtr->NBClkFreq + 999999) / 1000000);

  *MaxDlyPtr = 100 + *MinDlyPtr;  // 100 fixed iterations

  // IF (REVB) THEN
  //   IF (D18F2x[1,0]78[MaxSkipErrTrain]==0 && NCLK!=MEMCLK && NCLK!=MEMCLK/2)
  //   THEN TrainingOffset = 3
  //   ELSE TrainingOffset = 2
  // ELSE
  //   IF (NCLK!=MEMCLK && NCLK!=MEMCLK/2)
  //   THEN TrainingOffset = 3
  //   ELSE TrainingOffset = 2
  *DlyBiasPtr = 3;
  if (((NBPtr->DCTPtr->Timings.CasL > 5) && NBPtr->IsSupported[SkipErrTrain]) ||
      (NBPtr->NBClkFreq == NBPtr->DCTPtr->Timings.Speed) ||
      (NBPtr->NBClkFreq == (UINT32) (NBPtr->DCTPtr->Timings.Speed / 2)) ||
      (NBPtr->NBClkFreq == (UINT32) (NBPtr->DCTPtr->Timings.Speed / 2 + 1))) {
    *DlyBiasPtr = 2;
  }

  // Register settings required before MaxRdLat training
  if (NBPtr->DCTPtr->Timings.CasL == 5) {
    MemNSetBitFieldNb (NBPtr, BFMaxSkipErrTrain, 0);
  } else {
    MemNSetBitFieldNb (NBPtr, BFMaxSkipErrTrain, 1);
  }
  MemNSetBitFieldNb (NBPtr, BFSlotSel, 0);
  MemNSetBitFieldNb (NBPtr, BFSlot1ExtraClkEn, 0);
  MemNSetBitFieldNb (NBPtr, BFForceCasToSlot0, 1);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function is a wrapper to call a CPU routine to change NB P-state and
 *   update NB frequency.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]  *NBPstate - NB Pstate
 *
 *     @return          TRUE - Succeed
 *     @return          FALSE - Fail
 */

BOOLEAN
MemNChangeNbFrequencyWrapLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 NBPstate
  )
{
  BOOLEAN Status;
  UINT32  NBFreq;
  UINT32  Memclk;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  if (NBPtr->NbFreqChgState == 0) {
    // While in state 0, report the new memclk to the
    // CPU module to adjust the NB P-state settings.
    Memclk = NBPtr->DCTPtr->Timings.Speed;
  } else {
    // We have already adjusted for target memclk.
    // Indicate NB P-state change only.
    Memclk = 0;
  }

  Status = F12NbPstateInit (Memclk,
                            MemNGetMemClkFreqIdClientNb (NBPtr, NBPtr->DCTPtr->Timings.Speed),
                            NBPstate,
                            &NBFreq,
                            &(NBPtr->MemPtr->StdHeader));

  if (Status) {
    // When NB frequency change succeeds, TSC rate may have changed.
    // We need to update TSC rate
    GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **) &FamilySpecificServices, &NBPtr->MemPtr->StdHeader);
    FamilySpecificServices->GetTscRate (FamilySpecificServices, &NBPtr->MemPtr->TscRate, &NBPtr->MemPtr->StdHeader);

    if (NBPtr->DCTPtr->Timings.Speed == NBPtr->DCTPtr->Timings.TargetSpeed) {
      // Turn on adjust negative WL only at target speed
      NBPtr->IsSupported[WLNegativeDelay] = TRUE;
    }
  }
  return Status;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function enables swapping interleaved region feature.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   Base   - Swap interleaved region base [47:27]
 *     @param[in]   Limit   - Swap interleaved region limit [47:27]
 *
 */
VOID
MemNEnableSwapIntlvRgnLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Base,
  IN       UINT32 Limit
  )
{
  // Swapped interleaving region must be below 4G
  if (Limit < (1 << (32 - 27))) {
    MemNSetBitFieldNb (NBPtr, BFIntlvRegionBase, Base);
    MemNSetBitFieldNb (NBPtr, BFIntlvRegionLimit, (Limit - 1));
    MemNSetBitFieldNb (NBPtr, BFIntlvRegionEn, 1);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function skips setting LowPowerDrvStrengthEn on two DIMMs per channel config
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    FALSE   - Set LowPowerDrvStrengthEn
 *     @return    TRUE    - Clear LowPowerDrvStrengthEn
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNDisLowPwrDrvStrLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  if (NBPtr->ChannelPtr->Dimms > 1) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function returns MR0[WR] value
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return       MR0[WR] value
 */
UINT32
MemNGetMR0WRLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 Value32;

  Value32 = NBPtr->DCTPtr->Timings.Twr;
  Value32 = ((Value32 >= 10) ? ((Value32 + 1) / 2) : (Value32 - 4)) & 7;
  Value32 = Value32 << 9;

  return Value32;
}
