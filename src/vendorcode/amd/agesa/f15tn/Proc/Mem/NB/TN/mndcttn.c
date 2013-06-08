/* $NoKeywords:$ */
/**
 * @file
 *
 * mndcttn.c
 *
 * Northbridge DCT support for TN
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/TN)
 * @e \$Revision: 63661 $ @e \$Date: 2012-01-03 01:02:47 -0600 (Tue, 03 Jan 2012) $
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
#include "amdlib.h"
#include "Ids.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mu.h"
#include "OptionMemory.h"
#include "mntn.h"
#include "mftds.h"
#include "merrhdl.h"
#include "cpuFamRegisters.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuCommonF15Utilities.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE PROC_MEM_NB_TN_MNDCTTN_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define UNUSED_CLK 4
#define MAX_RD_DQS_DLY 0x1F

CONST BIT_FIELD_NAME MemPstateBF[4] = {BFMemPstate0, BFMemPstate1, BFMemPstate2, BFMemPstate3};
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
UINT32
STATIC
MemNTotalSyncComponentsTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

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
MemNAutoConfigTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 i;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  UINT32 PowerDownMode;

  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;
  DCTPtr = NBPtr->DCTPtr;

  //
  //======================================================================
  // Build Dram Config Lo Register Value
  //======================================================================
  MemNSetBitFieldNb (NBPtr, BFUnBuffDimm, 1);
  MemNSetBitFieldNb (NBPtr, BFPendRefPaybackS3En, 1);
  MemNSetBitFieldNb (NBPtr, BFStagRefEn, 1);
  //
  //======================================================================
  // Build Dram Config Hi Register Value
  //======================================================================
  //
  //
  // MemClkFreq
  //
  MemNSetBitFieldNb (NBPtr, BFMemClkFreq, MemNGetMemClkFreqIdUnb (NBPtr, DCTPtr->Timings.Speed));

  PowerDownMode = 1;
  IDS_OPTION_HOOK (IDS_POWERDOWN_MODE, &PowerDownMode, &(NBPtr->MemPtr->StdHeader));
  MemNSetBitFieldNb (NBPtr, BFPowerDownMode, PowerDownMode);

  if (NBPtr->MemPstateStage == MEMORY_PSTATE_1ST_STAGE) {
    MemNBrdcstSetNb (NBPtr, BFM1MemClkFreq, MemNGetMemClkFreqIdUnb (NBPtr, DDR667_FREQUENCY));
    MemNBrdcstSetNb (NBPtr, BFRate, MemNGetMemClkFreqIdUnb (NBPtr, DDR667_FREQUENCY) | 0x8);
    MemNBrdcstSetNb (NBPtr, BFMxMrsEn, 7);
  }

  MemNSetBitFieldNb (NBPtr, BFDphyMemPsSelEn, 1);
  //
  //======================================================================
  // Build Dram MRS Register Value
  //======================================================================
  //
  MemNSetBitFieldNb (NBPtr, BFPchgPDModeSel, 1);
  MemNSetBitFieldNb (NBPtr, BFBurstCtrl, 1);

  //======================================================================
  // DRAM Controller Miscellaneous 2
  //======================================================================
  MemNSetBitFieldNb (NBPtr, BFPerRankTimingEn, 1);
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\nEnable Per Rank Training....\n\n");
  MemNSetBitFieldNb (NBPtr, BFPrtlChPDEnhEn, 0);
  MemNSetBitFieldNb (NBPtr, BFAggrPDEn, 1);

  //======================================================================
  // GMC to DCT control
  //======================================================================
  MemNSetBitFieldNb (NBPtr, BFGmcTokenLimit, 4);
  MemNSetBitFieldNb (NBPtr, BFMctTokenLimit, 4);
  MemNSetBitFieldNb (NBPtr, BFGmcToDctControl1, 0x4444);
  if ((MCTPtr->LogicalCpuid.Revision & 0x0000000000000100ull ) != 0) {
    MemNSetBitFieldNb (NBPtr, BFCpuElevPrioDis, 1);
  }

  //======================================================================
  // Other Registers
  //======================================================================
  //
  //
  // Non-SPD Timings
  //
  MemNSetBitFieldNb (NBPtr, BFTrwtWB, 0x17);
  MemNSetBitFieldNb (NBPtr, BFTrwtTO, 0x16);
  MemNSetBitFieldNb (NBPtr, BFTwrrd, 0xB );

  MemNSetBitFieldNb (NBPtr, BFTrdrdSdSc, 0xB);
  MemNSetBitFieldNb (NBPtr, BFTrdrdSdDc, 0xB);
  MemNSetBitFieldNb (NBPtr, BFTrdrdDd, 0xB);

  MemNSetBitFieldNb (NBPtr, BFTwrwrSdSc, 0xB);
  MemNSetBitFieldNb (NBPtr, BFTwrwrSdDc, 0xB);
  MemNSetBitFieldNb (NBPtr, BFTwrwrDd, 0xB);

  MemNSetBitFieldNb (NBPtr, BFWrOdtOnDuration, DEFAULT_WR_ODT_TN);
  MemNSetBitFieldNb (NBPtr, BFRdOdtOnDuration, DEFAULT_RD_ODT_TN);
  MemNSetBitFieldNb (NBPtr, BFWrOdtTrnOnDly, DEFAULT_RD_ODT_TRNONDLY_TN);

  for (i = 0; i < 4; i++) {
    MemNSetBitFieldNb (NBPtr, BFTstag0 + i, 0x14);
  }

  MemNSetBitFieldNb (NBPtr, BFTmrd, 4);
  MemNSetBitFieldNb (NBPtr, BFFlushWrOnS3StpGnt, 1);
  MemNSetBitFieldNb (NBPtr, BFFastSelfRefEntryDis, 0);

  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function caps speed based on battery life check.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 */
VOID
MemNCapSpeedBatteryLifeTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST UINT16 SupportedFreq[] = {
    DDR2133_FREQUENCY,
    DDR1866_FREQUENCY,
    DDR1600_FREQUENCY,
    DDR1333_FREQUENCY,
    DDR1066_FREQUENCY,
    DDR800_FREQUENCY,
    DDR667_FREQUENCY
  };

  UINT32 FreqNumeratorInMHz;
  UINT32 FreqDivisor;
  UINT32 VoltageInuV;
  UINT32 NBFreq;
  UINT16 DdrFreq;
  UINT16 j;
  INT8   NbPs;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  BOOLEAN SkipAdjustNbPs;

  FamilySpecificServices = NULL;
  GetCpuServicesOfSocket (NBPtr->MCTPtr->SocketId, (CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, &(NBPtr->MemPtr->StdHeader));


  // Find the lowest supported NB Pstate
  NBFreq = 0;
  for (NbPs = 3; NbPs >= 0; NbPs--) {
    if (FamilySpecificServices->GetNbPstateInfo (FamilySpecificServices,
                                                  NBPtr->MemPtr->PlatFormConfig,
                                                  &NBPtr->PciAddr,
                                                  (UINT32) NbPs,
                                                  &FreqNumeratorInMHz,
                                                  &FreqDivisor,
                                                  &VoltageInuV,
                                                  &(NBPtr->MemPtr->StdHeader))) {
      if (MemNGetBitFieldNb (NBPtr, MemPstateBF[NbPs]) == 0) {
        NBFreq = FreqNumeratorInMHz / FreqDivisor;
        break;
      }
    }
  }

  ASSERT (NBFreq > 0);

  // Pick Max MEMCLK that is less than or equal to NCLK
  DdrFreq = DDR800_FREQUENCY;
  for (j = 0; j < GET_SIZE_OF (SupportedFreq); j++) {
    if (NBFreq >= ((UINT32) SupportedFreq[j])) {
      DdrFreq = SupportedFreq[j];
      break;
    }
  }

  // Cap MemClk frequency to lowest NCLK frequency
  if (NBPtr->DCTPtr->Timings.TargetSpeed > DdrFreq) {
    NBPtr->DCTPtr->Timings.TargetSpeed = DdrFreq;
  }

  // Adjust the NB P-state northbridge voltage
  SkipAdjustNbPs = FALSE;
  IDS_OPTION_HOOK (IDS_NBPSDIS_OVERRIDE, &SkipAdjustNbPs, &(NBPtr->MemPtr->StdHeader));
  if (SkipAdjustNbPs == FALSE) {
    MemNAdjustNBPstateVolTN (NBPtr);
  }

  // Initialize NbPsCtlReg
  NBPtr->NbPsCtlReg = 0;
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
 *     @param[in]  MaxRcvEnDly - Maximum receiver enable delay value
 */

VOID
MemNGetMaxLatParamsTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly,
  IN OUT   UINT16 *MinDlyPtr,
  IN OUT   UINT16 *MaxDlyPtr,
  IN OUT   UINT16 *DlyBiasPtr
  )
{
  UINT32 N;
  UINT32 T;
  UINT32 P;
  UINT32 MemClkPeriod;

  // 1. P = N = T = 0.
  P = N = T = 0;

  // Get all sync components BKDG steps 3,4,6
  P = MemNTotalSyncComponentsTN (NBPtr);

  // 7. P = P + CEIL(MAX(D18F2x9C_x0000_00[2A:10]_dct[1:0][DqsRcvEnGrossDelay, DqsRcvEnFineDelay] +
  //    D18F2x9C_x0000_0[3:0]0[7:5]_dct[1:0][RdDqsTime] PCLKs)) + 1
  P = P + (MaxRcvEnDly + 31) / 32 + 1;

  // 10. N = (P/(MemClkFreq * 2) + T) * NclkFreq; Convert from PCLKs plus time to NCLKs.
  MemClkPeriod = 1000000 / ((NBPtr->MemPstate == MEMORY_PSTATE0) ? NBPtr->DCTPtr->Timings.Speed : DDR667_FREQUENCY);
  N = ((((P * MemClkPeriod + 1) / 2) + T) * NBPtr->NBClkFreq + 999999) / 1000000;

  // Calculate a starting MaxRdLatency delay value with steps 5, 9, and 12 excluded
  *MinDlyPtr = (UINT16) N;

  *MaxDlyPtr = 0x3FF;

  // Left edge of MaxRdLat will be added with 1 NCLK and 3 PCLK (1.5 MEMCLK)
  N = 1;
  P = 3;
  N += (((P * MemClkPeriod + 1) / 2) * NBPtr->NBClkFreq + 999999) / 1000000;
  *DlyBiasPtr = (UINT16) N;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function  sets the maximum round-trip latency in the system from the processor to the DRAM
 *   devices and back.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]     MaxRcvEnDly - Maximum receiver enable delay value
 *
 */

VOID
MemNSetMaxLatencyTN (
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

  //
  // Initial value for MaxRdLat used in training
  //
  N = 0x55;

  if (MaxRcvEnDly != 0xFFFF) {
    // 1. P = N = T = 0.
    P = N = T = 0;

    // Get all sync components BKDG steps 3,4,6
    P = MemNTotalSyncComponentsTN (NBPtr);

    // 5. P = P + 5
    P += 5;

    // 7. P = P + CEIL(MAX(D18F2x9C_x0000_00[2A:10]_dct[1:0][DqsRcvEnGrossDelay, DqsRcvEnFineDelay] +
    //    D18F2x9C_x0000_0[3:0]0[6:5]_dct[1:0][RdDqsTime] PCLKs)) + 1
    P = P + ((MaxRcvEnDly + MAX_RD_DQS_DLY) + 31) / 32 + 1;

    // 8. If (NclkFreq/MemClkFreq < 2) then P = P + 4.5 Else P = P + 2.5
    if ((NBPtr->NBClkFreq / NBPtr->DCTPtr->Timings.Speed) < 2) {
      Px2 = P * 2 + 9;
    } else {
      Px2 = P * 2 + 5;
    }

    // 9. T = T + 1050 ps
    T += 1050;

    // 10. N = (P/(MemClkFreq * 2) + T) * NclkFreq; Convert from PCLKs plus time to NCLKs.
    MemClkPeriod = 1000000 / NBPtr->DCTPtr->Timings.Speed;
    N = ((((Px2 * MemClkPeriod + 3) / 4) + T) * NBPtr->NBClkFreq + 999999) / 1000000;

    // 11. D18F2x210_dct[1:0]_nbp[3:0][MaxRdLatency] = CEIL(N) - 1
    N = N - 1;
  }

  NBPtr->DCTPtr->Timings.MaxRdLat = (UINT16) N;
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tMaxRdLat: %03x\n", N);
  MemNSetBitFieldNb (NBPtr, BFMaxLatency, N);
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function set MaxRdLat after HW receiver enable training is completed
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNExitPhyAssistedTrainingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 Dct;
  UINT8 ChipSel;
  MEM_TECH_BLOCK  *TechPtr;

  TechPtr = NBPtr->TechPtr;

  // Calculate Max Latency for both channels to prepare for position training
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_TN ; Dct++) {
    IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
    NBPtr->SwitchDCT (NBPtr, Dct);

    // Reset DisAutoRefresh and ZqcsInterval for position training.
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 1);
      MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 0);
      MemNSetBitFieldNb (NBPtr, BFRx4thStgEn, 0);
      MemNSetBitFieldNb (NBPtr, BFRxBypass3rd4thStg, 4);
    }

    if (TechPtr->FindMaxDlyForMaxRdLat (TechPtr, &ChipSel)) {
      NBPtr->SetMaxLatency (NBPtr, TechPtr->MaxDlyForMaxRdLat);
    }
  }

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets the total of sync components for Max Read Latency calculation
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      Total in PCLKs
 */
UINT32
STATIC
MemNTotalSyncComponentsTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 P;

  P = 0;

  // 3. If (D18F2x9C_x0000_0004_dct[1:0][AddrCmdSetup] = 0 & D18F2x9C_x0000_0004_dct[1:0][CsOdt-
  // Setup] = 0 & D18F2x9C_x0000_0004_dct[1:0][CkeSetup] = 0)
  // then P = P + 1
  // else P = P + 2
  if ((MemNGetBitFieldNb (NBPtr, BFAddrTmgControl) & 0x0202020) == 0) {
    P += 1;
  } else {
    P += 2;
  }

  // 4. P = P + (8 - D18F2x210_dct[1:0]_nbp[3:0][RdPtrInit])
  P = P + (8 - (UINT16) MemNGetBitFieldNb (NBPtr, BFRdPtrInit));

  // 6. P = P + (2 * (D18F2x200_dct[1:0][Tcl] - 1 clocks))
  P = P + (2 * (MemNGetBitFieldNb (NBPtr, BFTcl) - 1));

  return P;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function calculates and programs NB P-state dependent registers
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNProgramNbPstateDependentRegistersTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 RdPtrInit;
  UINT8 Dct;
  MEMORY_BUS_SPEED MemClkSpeed;

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_TN; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      break;
    }
  }

  if (MemNGetBitFieldNb (NBPtr, MemPstateBF[MemNGetBitFieldNb (NBPtr, BFNbPsSel)]) == 0) {
    MemClkSpeed = NBPtr->DCTPtr->Timings.Speed;
  } else {
    MemClkSpeed = MemNGetMemClkFreqUnb (NBPtr, (UINT8) MemNGetBitFieldNb (NBPtr, BFM1MemClkFreq));
  }
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tMemclk Freq: %d\n", MemClkSpeed);

  // NCLK:MCLK ratio    DDR rate (MT/s)       RdPtrInit
  // < 2:1              < 2133                0011b (2.5T)
  // < 2:1              2133 <= rate <= 2400  0011b (2.5T) or 0010b (3T)
  // For each NB P-state, IF any D18F2x9C_x0000_0[3:0]0[2:1]_dct[1:0]_mp[MemPstate][WrDat-
  // GrossDly] ==0 THEN RdPtrInit=0010b ELSE RdPtrInit=0011b
  // >=2:1              < 1866                0110b
  // >=2:1              1866 <= rate < 2400   0101b
  // >=2:1              2400                  0100b
  if (NBPtr->NBClkFreq < (UINT32) (MemClkSpeed * 2)) {
    RdPtrInit = ((MemClkSpeed >= DDR2133_FREQUENCY) && (NBPtr->TechPtr->GetMinMaxGrossDly (NBPtr->TechPtr, AccessWrDatDly, FALSE) == 0)) ? 2 : 3;
  } else {
    RdPtrInit = (MemClkSpeed < DDR1866_FREQUENCY) ? 6 : ((MemClkSpeed < DDR2400_FREQUENCY) ? 5 : 4);
  }
  MemNBrdcstSetNb (NBPtr, BFRdPtrInit, RdPtrInit);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tRdPtr: %d\n", RdPtrInit);

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_TN; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      // Set ProcOdtAdv
      if ((NBPtr->ChannelPtr->SODimmPresent != 0) && (NBPtr->DCTPtr->Timings.Speed <= DDR1333_FREQUENCY)) {
        MemNSetBitFieldNb (NBPtr, BFProcOdtAdv, 0);
      } else {
        MemNSetBitFieldNb (NBPtr, BFProcOdtAdv, 0x4000);
      }
    }
  }

  MemNBrdcstSetNb (NBPtr, BFDataTxFifoWrDly, 0);

  IDS_OPTION_HOOK (IDS_NBPS_REG_OVERRIDE, NBPtr, &NBPtr->MemPtr->StdHeader);
  MemFInitTableDrive (NBPtr, MTAfterNbPstateChange);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This is a general purpose function that executes before DRAM init
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNBeforeDramInitTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_TN; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      //
      // 2.10.6.7 DCT Training Specific Configuration
      //
      MemNSetBitFieldNb (NBPtr, BFAddrCmdTriEn, 0);
      MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 1);
      MemNSetBitFieldNb (NBPtr, BFForceAutoPchg, 0);
      MemNSetBitFieldNb (NBPtr, BFDynPageCloseEn, 0);
      MemNSetBitFieldNb (NBPtr, BFBankSwizzleMode, 0);
      MemNSetBitFieldNb (NBPtr, BFDcqBypassMax, 0);
      MemNSetBitFieldNb (NBPtr, BFPowerDownEn, 0);
      MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 0);
      MemNSetBitFieldNb (NBPtr, BFRxMaxDurDllNoLock, 0);
      MemNSetBitFieldNb (NBPtr, BFTxMaxDurDllNoLock, 0);
      MemNSetBitFieldNb (NBPtr, BFEnRxPadStandby, 0);
      MemNSetBitFieldNb (NBPtr, BFBankSwap, 0);
      MemNSetBitFieldNb (NBPtr, BFODTSEn, 0);
      MemNSetBitFieldNb (NBPtr, BFDctSelIntLvEn, 0);
      MemNSetBitFieldNb (NBPtr, BFCmdThrottleMode, 0);
      MemNSetBitFieldNb (NBPtr, BFBwCapEn, 0);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function modifies CS interleaving low address according to several conditions for TN.
 *
 *     @param[in,out]  *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  *LowBit   - Pointer to low bit
 *
 */

BOOLEAN
MemNCSIntLvLowAddrAdjTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *LowBit
  )
{
  UINT8 DctSelIntLvAddr;

  DctSelIntLvAddr = (UINT8) MemNGetBitFieldNb (NBPtr, BFDctSelIntLvAddr);
  //
  //D18F2x[5C:40]_dct[1:0][15:5] = BaseAddr[21:11] &&
  //D18F2x[6C:60]_dct[1:0][15:5] = AddrMask[21:11], so *LowBit needs to be added with 2.
  //
  *(UINT8 *) LowBit += 2;

  if (MemNGetBitFieldNb (NBPtr, BFBankSwap) == 1) {
    if (DctSelIntLvAddr == 4) {
      *(UINT8 *) LowBit = 5;
    } else {
      *(UINT8 *) LowBit = 6;
    }
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function releases the NB P-state force.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   OptParam - Optional parameter
 *
 *     @return    TRUE
 */
BOOLEAN
MemNReleaseNbPstateTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, &NBPtr->MemPtr->StdHeader);

  // 6. Restore the initial D18F5x170[SwNbPstateLoDis, NbPstateDisOnP0] values.
  MemNSetBitFieldNb (NBPtr, BFNbPstateCtlReg, (MemNGetBitFieldNb (NBPtr, BFNbPstateCtlReg) & 0xFFFF9FFF) | (NBPtr->NbPsCtlReg & 0x6000));
  // 7. Restore the initial D18F5x170[NbPstateThreshold, NbPstateHi] values.
  MemNSetBitFieldNb (NBPtr, BFNbPstateCtlReg, (MemNGetBitFieldNb (NBPtr, BFNbPstateCtlReg) & 0xFFFFF13F) | (NBPtr->NbPsCtlReg & 0x0EC0));
  // 8. Restore the initial D18F5x170[NbPstateLo] values.
  MemNSetBitFieldNb (NBPtr, BFNbPstateLo, (NBPtr->NbPsCtlReg >> 3) & 3);

  // Clear NbPsSel to 0
  MemNSetBitFieldNb (NBPtr, BFNbPsSel, 0);
  // Update TSC rate
  FamilySpecificServices->GetTscRate (FamilySpecificServices, &NBPtr->MemPtr->TscRate, &NBPtr->MemPtr->StdHeader);

  if (MemNGetBitFieldNb (NBPtr, BFMemPsSel) != 0) {
    MemNChangeMemPStateContextNb (NBPtr, 0);
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function handles multiple stage of training when multiple Mem Pstate is enabled
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 *
 */

BOOLEAN
MemNMemPstateStageChangeTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  BOOLEAN RetVal;
  TRN_DLY_TYPE AccessType;
  UINT8 Dct;
  UINT8 ChipSel;
  UINT8 ByteLane;
  UINT16 CsEnabled;
  UINT16 TrnDly;

  RetVal = FALSE;

  if (NBPtr->MemPstateStage == MEMORY_PSTATE_1ST_STAGE) {
    MemNChangeMemPStateContextNb (NBPtr, 1);
    // Load memory registers in M1 context from data saved in the heap
    IDS_HDT_CONSOLE (MEM_FLOW, "\nLoad Training registers for M1 with DDR667 training result\n");
    for (Dct = 0; Dct < MAX_DCTS_PER_NODE_TN; Dct++) {
      MemNSwitchDCTNb (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        // Save MemPstate 1 data in output data structures
        LibAmdMemCopy  (NBPtr->ChannelPtr->RcvEnDlysMemPs1, NBPtr->ChannelPtr->RcvEnDlys, (MAX_DIMMS * MAX_DELAYS) * 2, &(NBPtr->MemPtr->StdHeader));
        LibAmdMemCopy  (NBPtr->ChannelPtr->RdDqsDlysMemPs1, NBPtr->ChannelPtr->RdDqsDlys, MAX_DIMMS * MAX_DELAYS, &(NBPtr->MemPtr->StdHeader));
        LibAmdMemCopy  (NBPtr->ChannelPtr->WrDqsDlysMemPs1, NBPtr->ChannelPtr->WrDqsDlys, MAX_DIMMS * MAX_DELAYS, &(NBPtr->MemPtr->StdHeader));
        LibAmdMemCopy  (NBPtr->ChannelPtr->WrDatDlysMemPs1, NBPtr->ChannelPtr->WrDatDlys, MAX_DIMMS * MAX_DELAYS, &(NBPtr->MemPtr->StdHeader));
        LibAmdMemCopy  (NBPtr->ChannelPtr->RdDqs2dDlysMemPs1, NBPtr->ChannelPtr->RdDqs2dDlys, MAX_DIMMS * MAX_NUMBER_LANES, &(NBPtr->MemPtr->StdHeader));
        LibAmdMemCopy  (NBPtr->ChannelPtr->RdDqsMinDlysMemPs1, NBPtr->ChannelPtr->RdDqsMinDlys, MAX_DIMMS * MAX_DELAYS, &(NBPtr->MemPtr->StdHeader));
        LibAmdMemCopy  (NBPtr->ChannelPtr->RdDqsMaxDlysMemPs1, NBPtr->ChannelPtr->RdDqsMaxDlys, MAX_DIMMS * MAX_DELAYS, &(NBPtr->MemPtr->StdHeader));
        LibAmdMemCopy  (NBPtr->ChannelPtr->WrDatMinDlysMemPs1, NBPtr->ChannelPtr->WrDatMinDlys, MAX_DIMMS * MAX_DELAYS, &(NBPtr->MemPtr->StdHeader));
        LibAmdMemCopy  (NBPtr->ChannelPtr->WrDatMaxDlysMemPs1, NBPtr->ChannelPtr->WrDatMaxDlys, MAX_DIMMS * MAX_DELAYS, &(NBPtr->MemPtr->StdHeader));
        LibAmdMemCopy  (NBPtr->ChannelPtr->FailingBitMaskMemPs1, NBPtr->ChannelPtr->FailingBitMask, MAX_CS_PER_CHANNEL * MAX_DELAYS, &(NBPtr->MemPtr->StdHeader));

        CsEnabled = NBPtr->DCTPtr->Timings.CsEnabled;
        // Set Memory Pstate 1 training value into registers
        for (AccessType = AccessRcvEnDly; AccessType <= AccessWrDqsDly; AccessType ++) {
          for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL_TN; ChipSel = ChipSel + NBPtr->CsPerDelay) {
            if ((CsEnabled & ((UINT16) ((NBPtr->CsPerDelay == 2)? 3 : 1) << ChipSel)) != 0) {
              for (ByteLane = 0; ByteLane < 8; ByteLane++) {
                TrnDly = (UINT16) GetTrainDlyFromHeapNb (NBPtr, AccessType, DIMM_BYTE_ACCESS (ChipSel / NBPtr->CsPerDelay, ByteLane));
                NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_BYTE_ACCESS (ChipSel / NBPtr->CsPerDelay, ByteLane), TrnDly);
              }
            }
          }
        }

        if (NBPtr->RefPtr->EnablePowerDown) {
          MemNSetTxpNb (NBPtr);
          MemNSetBitFieldNb (NBPtr, BFPchgPDEnDelay, (MAX (MAX ((NBPtr->DCTPtr->Timings.CasL + 5),
                                                                (UINT8) (MemNGetBitFieldNb (NBPtr, BFTcwl) + NBPtr->DCTPtr->Timings.Twr + 5)),
                                                           (UINT8) MemNGetBitFieldNb (NBPtr, BFTmod))));
          MemNSetBitFieldNb (NBPtr, BFAggrPDDelay, 0x20);
        }
        MemNSetOtherTimingTN (NBPtr);
        // Save timing data structure for memory Pstate 1
        LibAmdMemCopy  (NBPtr->DCTPtr->TimingsMemPs1, &(NBPtr->DCTPtr->Timings), sizeof (CH_TIMING_STRUCT), &(NBPtr->MemPtr->StdHeader));

        MemFInitTableDrive (NBPtr, MTAfterMemPstate1PartialTrn);
      }
    }

    // Switch back to M0 context
    MemNChangeMemPStateContextNb (NBPtr, 0);

    // Load memory registers in M1 context from data saved in the heap
    IDS_HDT_CONSOLE (MEM_FLOW, "\nGoing into training stage 2. Complete training at DDR667 is done.\n");
    NBPtr->MemPstateStage = MEMORY_PSTATE_2ND_STAGE;
  } else if ((NBPtr->MemPstateStage == MEMORY_PSTATE_2ND_STAGE) && (NBPtr->DCTPtr->Timings.TargetSpeed == NBPtr->DCTPtr->Timings.Speed)) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nGoing into training stage 3. Partial training at all frequencies is done.\n");
    NBPtr->MemPstateStage = MEMORY_PSTATE_3RD_STAGE;
    RetVal = TRUE;
  } else {
    // MemPstate is disabled. Do not go through the MemPstate handling flow.
    RetVal = TRUE;
  }

  return RetVal;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function Sets Power Down options and enables Power Down
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     The following registers are set:
 *      BFPowerDownMode         BFPrtlChPDEnhEn
 *      BFTxp                   BFAggrPDDelay
 *      BFTxpDll                BFAggrPDEn
 *      BFPchgPDEnDelay         BFPowerDownEn
 *
 * NOTE:  Delay values must be set before turning on the associated Enable bit
 */
VOID
MemNPowerDownCtlTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 PowerDownMode;
  UINT8 Tmod;
  UINT8 Twr;
  UINT8 Tcwl;
  UINT8 Tcl;

  if (NBPtr->RefPtr->EnablePowerDown) {
    //
    // PowerDownMode
    //
    PowerDownMode = (UINT8) UserOptions.CfgPowerDownMode;
    PowerDownMode = (!NBPtr->IsSupported[ChannelPDMode]) ? PowerDownMode : 0;
    IDS_OPTION_HOOK (IDS_POWERDOWN_MODE, &PowerDownMode, &(NBPtr->MemPtr->StdHeader));
    if (PowerDownMode == 1) {
      MemNSetBitFieldNb (NBPtr, BFPowerDownMode, 1);
    }
    //
    // Txp
    //
    MemNSetTxpNb (NBPtr);
    //
    // PchgPDModeSel is set elswhere.
    //
    // PchgPDEnDelay = MAX(Tcl + 5, Tcwl + Twr + 5, Tmod)
    //
    Tmod = (UINT8) MemNGetBitFieldNb (NBPtr, BFTmod);
    Twr = NBPtr->DCTPtr->Timings.Twr;
    Tcwl = (UINT8) MemNGetBitFieldNb (NBPtr, BFTcwl);
    Tcl = NBPtr->DCTPtr->Timings.CasL;
    MemNSetBitFieldNb (NBPtr, BFPchgPDEnDelay, (MAX (MAX ((Tcl + 5), (Tcwl + Twr + 5)), Tmod)));
    //
    // Partial Channel Power Down
    //
    MemNSetBitFieldNb (NBPtr, BFPrtlChPDDynDly, 0);
    MemNSetBitFieldNb (NBPtr, BFPrtlChPDEnhEn, 0);
    //
    // Aggressive PowerDown
    //
    MemNSetBitFieldNb (NBPtr, BFAggrPDDelay, 0x20);
    MemNSetBitFieldNb (NBPtr, BFAggrPDEn, 1);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   Always set upper 2 bits of CKETri bitfield
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNBeforePlatformSpecTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemNSetBitFieldNb (NBPtr, BFCKETri, 0xC | MemNGetBitFieldNb (NBPtr, BFCKETri));
}
