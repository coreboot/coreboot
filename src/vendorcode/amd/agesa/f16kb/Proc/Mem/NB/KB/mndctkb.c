/* $NoKeywords:$ */
/**
 * @file
 *
 * mndctkb.c
 *
 * Northbridge DCT support for KB
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/KB)
 * @e \$Revision: 86880 $ @e \$Date: 2013-01-28 11:15:07 -0600 (Mon, 28 Jan 2013) $
 *
 **/
/*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include "PlatformMemoryConfiguration.h"
#include "mnkb.h"
#include "mftds.h"
#include "merrhdl.h"
#include "cpuFamRegisters.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuF16Utilities.h"
#include "IdsF16KbAllService.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE PROC_MEM_NB_KB_MNDCTKB_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define UNUSED_CLK 4

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
MemNTotalSyncComponentsKB (
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
MemNAutoConfigKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 i;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  UINT32 PowerDownMode;
  UINT32 Tstag;

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
  // DimmEccEn
  //
  if (MCTPtr->Status[SbEccDimms]) {
    MemNSetBitFieldNb (NBPtr, BFDimmEccEn, 1);
  }
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
    MemNSetBitFieldNb (NBPtr, BFDphyMemPsSelEn, 0);
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
  MemNSetBitFieldNb (NBPtr, BFDctSelBankSwap, 1);

  //======================================================================
  // Trace Buffer Extended Address Initialization
  //======================================================================
  MemNSetBitFieldNb (NBPtr, BFTrcBufAdrPtrHi, 0);
  MemNSetBitFieldNb (NBPtr, BFTrcBufDramLimitHi, 0);
  MemNSetBitFieldNb (NBPtr, BFTrcBufDramBaseHi, 0);

  //======================================================================
  // GMC to DCT control
  //======================================================================
  MemNSetBitFieldNb (NBPtr, BFGmcTokenLimit, 4);
  MemNSetBitFieldNb (NBPtr, BFMctTokenLimit, 4);
  MemNSetBitFieldNb (NBPtr, BFGmcToDctControl1, 0x04040404);
  MemNSetBitFieldNb (NBPtr, BFCpuElevPrioPeriod, 0xC);
  MemNSetBitFieldNb (NBPtr, BFCpuElevPrioDis, 0);

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

  MemNSetBitFieldNb (NBPtr, BFWrOdtOnDuration, DEFAULT_WR_ODT_KB);
  MemNSetBitFieldNb (NBPtr, BFRdOdtOnDuration, DEFAULT_RD_ODT_KB);
  MemNSetBitFieldNb (NBPtr, BFWrOdtTrnOnDly, DEFAULT_RD_ODT_TRNONDLY_KB);

  // Tstag = BIOS: MAX(D18F2x204_dct[0]_mp[1:0][Trrd], CEIL(D18F2x204_dct[0]_mp[1:0][FourActWindow]/4))
  Tstag = MAX (MemNGetBitFieldNb (NBPtr, BFTrrd), (MemNGetBitFieldNb (NBPtr, BFFourActWindow) + 3) / 4);
  for (i = 0; i < 4; i++) {
    MemNSetBitFieldNb (NBPtr, BFTstag0 + i, Tstag);
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
MemNCapSpeedBatteryLifeKB (
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
  UINT8 Dct;
  INT8   NbPs;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

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
    if (NBFreq >= ((UINT32) SupportedFreq[j]) && NBFreq <= ((UINT32) SupportedFreq[j] * 2)) {
      DdrFreq = SupportedFreq[j];
      break;
    }
  }
  ASSERT (j < GET_SIZE_OF (SupportedFreq));

  // Cap MemClk frequency to lowest NCLK frequency
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_KB; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.TargetSpeed > DdrFreq) {
      NBPtr->DCTPtr->Timings.TargetSpeed = DdrFreq;
    }
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
MemNGetMaxLatParamsKB (
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
  P = MemNTotalSyncComponentsKB (NBPtr);

  // 8. P = P + CEIL(MAX(D18F2x9C_x0000_00[2A:10]_dct[1:0][DqsRcvEnGrossDelay, DqsRcvEnFineDelay] +
  //    D18F2x9C_x0000_0[3:0]0[7:5]_dct[1:0][RdDqsTime] PCLKs)) + 1
  P = P + (MaxRcvEnDly + 31) / 32 + 1;

  // 11. N = (P/(MemClkFreq * 2) + T) * NclkFreq; Convert from PCLKs plus time to NCLKs.
  MemClkPeriod = 1000000 / ((NBPtr->MemPstate == MEMORY_PSTATE0) ? NBPtr->DCTPtr->Timings.Speed : DDR667_FREQUENCY);
  N = ((((P * MemClkPeriod + 1) / 2) + T) * NBPtr->NBClkFreq + 999999) / 1000000;

  // Calculate a starting MaxRdLatency delay value with steps 5, 9, and 12 excluded
  *MinDlyPtr = (UINT16) N;

  *MaxDlyPtr = 0x3FF;

  // Program D18F2x210_dct[0]_nbp[3:0][MaxRdLatency] = CEIL(current value + 1 NCLK + 1.5 MEMCLK +
  // IF (NclkFreq/MemClkFreq < 2) THEN 1 MEMCLK ELSE 0 ENDIF)
  N = 1;
  P = 3 + (((NBPtr->NBClkFreq / ((NBPtr->MemPstate == MEMORY_PSTATE0) ? NBPtr->DCTPtr->Timings.Speed : DDR667_FREQUENCY)) < 2) ? 2 : 0);
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
MemNSetMaxLatencyKB (
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
    P = MemNTotalSyncComponentsKB (NBPtr);

    // 5. P = P + 6
    P += 6;

    // 8. P = P + CEIL(MAX(D18F2x9C_x0000_00[2A:10]_dct[1:0][DqsRcvEnGrossDelay, DqsRcvEnFineDelay] +
    //    D18F2x9C_x0000_0[3:0]0[6:5]_dct[1:0][RdDqsTime] PCLKs)) + 1
    P = P + (MaxRcvEnDly + 31) / 32 + 1;

    // 9. If (NclkFreq/MemClkFreq < 2) then P = P + 4.5 Else P = P + 2.5
    if ((NBPtr->NBClkFreq / NBPtr->DCTPtr->Timings.Speed) < 2) {
      Px2 = P * 2 + 9;
    } else {
      Px2 = P * 2 + 5;
    }

    // 10. T = T + 1050 ps
    T += 1050;

    // 11. N = (P/(MemClkFreq * 2) + T) * NclkFreq; Convert from PCLKs plus time to NCLKs.
    MemClkPeriod = 1000000 / NBPtr->DCTPtr->Timings.Speed;
    N = ((((Px2 * MemClkPeriod + 3) / 4) + T) * NBPtr->NBClkFreq + 999999) / 1000000;

    // 12. D18F2x210_dct[1:0]_nbp[3:0][MaxRdLatency] = CEIL(N) + 1
    N = N + 1;
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
MemNExitPhyAssistedTrainingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 Dct;
  UINT8 ChipSel;
  MEM_TECH_BLOCK  *TechPtr;

  TechPtr = NBPtr->TechPtr;

  // Calculate Max Latency for both channels to prepare for position training
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_KB ; Dct++) {
    IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
    NBPtr->SwitchDCT (NBPtr, Dct);

    // Reset DisAutoRefresh and ZqcsInterval for position training.
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 1);
      MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 0);
      MemNSetBitFieldNb (NBPtr, BFRxDqInsDly, 0);
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
MemNTotalSyncComponentsKB (
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

  // 7. P = P + (2 * (D18F2x200_dct[1:0][Tcl] - 1 clocks))
  P = P + (2 * (MemNGetBitFieldNb (NBPtr, BFTcl) - 1));

  return P;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function obtains the memory frequency in the current context
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
UINT16
MemNGetMemClkFreqInCurrentContextKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  UINT16 MemClkSpeed;

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_KB; Dct++) {
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

  return MemClkSpeed;
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
MemNProgramNbPstateDependentRegistersKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 RdPtrInit;
  UINT8 Dct;
  UINT8 *DimmsPerChPtr;
  UINT8 MaxSolderedDownDimmPerCh;
  UINT32 MemClkSpeed;

  MemClkSpeed = NBPtr->GetMemClkFreqInCurrentContext (NBPtr);

  // IF (((NBCOF >= DdrRate) == FALSE) THEN
  //   RdPtrInit = 0010b
  // ELSE IF (((NBCOF >= DdrRate) == TRUE) && (DdrRate == 667 || DdrRate == 800 || DdrRate == 1600))THEN
  //   RdPtrInit = 0110b
  // ELSE IF (((NBCOF >= DdrRate) == TRUE) && (DdrRate == 1866 || DdrRate ==2133))THEN
  //   RdPtrInit = 0101b
  // ELSE IF (((NBCOF >= DdrRate) == TRUE) && (DdrRate == 2400))THEN
  //   RdPtrInit = 0100b
  // ENDIF
  if (NBPtr->NBClkFreq < (UINT32) (MemClkSpeed * 2)) {
    RdPtrInit = 2;
  } else {
    RdPtrInit = (MemClkSpeed < DDR1600_FREQUENCY) ? 6 : ((MemClkSpeed < DDR2400_FREQUENCY) ? 5 : 4);
  }
  MemNBrdcstSetNb (NBPtr, BFRdPtrInit, RdPtrInit);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tRdPtr: %d\n", RdPtrInit);

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_KB; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      // Set ProcOdtAdv
      // BIOS:IF(Solder-down DRAM || SODIMM) && DdrRate <= 1333) THEN 0 ELSE 1 ENDIF.
      MaxSolderedDownDimmPerCh = GetMaxSolderedDownDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration,
                                                                    NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr->ChannelID);
      DimmsPerChPtr = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration,
                                           PSO_SOLDERED_DOWN_SODIMM_TYPE,
                                           NBPtr->MCTPtr->SocketId,
                                           NBPtr->ChannelPtr->ChannelID,
                                           0, NULL, NULL);
      if (((DimmsPerChPtr != NULL) || (MaxSolderedDownDimmPerCh != 0) || (NBPtr->ChannelPtr->SODimmPresent != 0)) &&
         (NBPtr->DCTPtr->Timings.Speed <= DDR1333_FREQUENCY)) {
        MemNSetBitFieldNb (NBPtr, BFProcOdtAdv, 0);
      } else {
        MemNSetBitFieldNb (NBPtr, BFProcOdtAdv, 0x4000);
      }
      MemNSetBitFieldNb (NBPtr, BFDataTxFifoWrDly, 0);
      if (NBPtr->DCTPtr->Timings.Speed >= DDR1600_FREQUENCY) {
        MemNSetBitFieldNb (NBPtr, BFReducedLoop, (2 << 13));
      }
    }
  }

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
MemNBeforeDramInitKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  // c. Program DCT training specific configuration.
  MemNConfigureDctTrainingKB (NBPtr);
  // d. Program the remaining DCT registers not covered by an explicit sequence dependency.
  MemNProgramNonSeqDependentRegistersKB (NBPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs the memory controller for training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNConfigureDctTrainingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_KB; Dct++) {
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
      MemNSetBitFieldNb (NBPtr, BFCmdThrottleMode, 0);
      MemNSetBitFieldNb (NBPtr, BFBwCapEn, 0);
      MemNSetBitFieldNb (NBPtr, BFDramScrub, 0);
      MemNSetBitFieldNb (NBPtr, BFScrubReDirEn, 0);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs the remaining DCT registers not covered by
 *     an explicit sequence dependency.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNProgramNonSeqDependentRegistersKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_KB; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      MemNSetBitFieldNb (NBPtr, BFDllCSRBiasTrim, 0x1000);

      IEM_INSERT_CODE (IEM_BEFORE_DRAM_INIT, IemBeforeDramInitOverrideKB, (NBPtr));
    }
  }
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs the memory controller for normal operation
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNConfigureDctNormalKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  BOOLEAN DllShutDownEn;

  DllShutDownEn = TRUE;
  IDS_OPTION_HOOK (IDS_DLL_SHUT_DOWN, &DllShutDownEn, &(NBPtr->MemPtr->StdHeader));

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_KB; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      //
      // 2.10.6.7 DCT Training Specific Configuration
      //
      MemNSetBitFieldNb (NBPtr, BFAddrCmdTriEn, 1);
      MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 0);
      if (DllShutDownEn && NBPtr->IsSupported[SetDllShutDown]) {
        MemNSetBitFieldNb (NBPtr, BFDisDllShutdownSR, 0);
      }
      MemNSetBitFieldNb (NBPtr , BFForceAutoPchg, 0);
      MemNSetBitFieldNb (NBPtr , BFDynPageCloseEn, 0);
      if (NBPtr->RefPtr->EnableBankSwizzle) {
        MemNSetBitFieldNb (NBPtr, BFBankSwizzleMode, 1);
      }
      MemNSetBitFieldNb (NBPtr, BFDcqBypassMax, 0x01F);
      MemNPowerDownCtlKB (NBPtr);
      MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 2);
      MemNSetBitFieldNb (NBPtr, BFBankSwap, 1);
      //
      // Post Training values for BFRxMaxDurDllNoLock, BFTxMaxDurDllNoLock,
      //  and BFEnRxPadStandby are handled by Power savings code
      //
      // BFBwCapEn and BFODTSEn are handled by OnDimmThermal Code
      //
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function modifies CS interleaving low address according to several conditions for KB.
 *
 *     @param[in,out]  *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  *LowBit   - Pointer to low bit
 *
 */

BOOLEAN
MemNCSIntLvLowAddrAdjKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *LowBit
  )
{
  UINT8 DctSelBankSwap;

  DctSelBankSwap = (UINT8) MemNGetBitFieldNb (NBPtr, BFDctSelBankSwap);
  //
  //D18F2x[5C:40]_dct[1:0][15:5] = BaseAddr[21:11] &&
  //D18F2x[6C:60]_dct[1:0][15:5] = AddrMask[21:11], so *LowBit needs to be added with 2.
  //
  *(UINT8 *) LowBit += 2;

  if (MemNGetBitFieldNb (NBPtr, BFBankSwap) == 1) {
    if (DctSelBankSwap == 1) {
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
MemNReleaseNbPstateKB (
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
MemNMemPstateStageChangeKB (
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
  UINT8 Tcl;

  RetVal = FALSE;

  if (NBPtr->MemPstateStage == MEMORY_PSTATE_1ST_STAGE) {
    MemNChangeMemPStateContextNb (NBPtr, 1);
    // Load memory registers in M1 context from data saved in the heap
    IDS_HDT_CONSOLE (MEM_FLOW, "\nLoad Training registers for M1 with DDR667 training result\n");
    for (Dct = 0; Dct < MAX_DCTS_PER_NODE_KB; Dct++) {
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
          for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL_KB; ChipSel = ChipSel + NBPtr->CsPerDelay) {
            if ((CsEnabled & ((UINT16) ((NBPtr->CsPerDelay == 2)? 3 : 1) << ChipSel)) != 0) {
              for (ByteLane = 0; ByteLane < (NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
                TrnDly = (UINT16) GetTrainDlyFromHeapNb (NBPtr, AccessType, DIMM_BYTE_ACCESS (ChipSel / NBPtr->CsPerDelay, ByteLane));
                NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_BYTE_ACCESS (ChipSel / NBPtr->CsPerDelay, ByteLane), TrnDly);
              }
            }
          }
        }

        if (NBPtr->RefPtr->EnablePowerDown) {
          MemNSetTxpNb (NBPtr);
          //
          // BFPchgPDEnDelay =
          // IF (D18F2xA8_dct[0][AggrPDEn]) THEN
          // (D18F2x200_dct[0]_mp[1:0][Tcl] + 5  +
          // CEIL((MAX(D18F2x9C_x0000_00[2A:10]_dct[0]_mp[1:0][DqsRcvEnGrossDelay])
          // + 0.5) / 2)) ELSE 00h ENDIF.
          //
          if (MemNGetBitFieldNb (NBPtr, BFAggrPDEn) == 1) {
            Tcl = NBPtr->DCTPtr->Timings.CasL;
            MemNSetBitFieldNb (NBPtr, BFPchgPDEnDelay, Tcl + 5 + NBPtr->TechPtr->GetMinMaxGrossDly (NBPtr->TechPtr, AccessRcvEnDly, TRUE) / 2 + 1);
          } else {
            MemNSetBitFieldNb (NBPtr, BFPchgPDEnDelay, 0);
          }
          MemNSetBitFieldNb (NBPtr, BFAggrPDDelay, 0x20);
        }
        MemNSetOtherTimingKB (NBPtr);
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
MemNPowerDownCtlKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 PowerDownMode;
  CONST UINT32 PwrMngm1[] = {0, 0, 0x05050403, 0x05050403, 0x06060403, 0x07070504, 0x08080504, 0x0A0A0605, 0x0B0B0706};
  UINT8 i;
  UINT16 Speed;
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
    // Partial Channel Power Down
    //
    MemNSetBitFieldNb (NBPtr, BFPrtlChPDDynDly, 4);
    MemNSetBitFieldNb (NBPtr, BFPrtlChPDEnhEn, 0);
    //
    // Aggressive PowerDown
    //
    MemNSetBitFieldNb (NBPtr, BFAggrPDDelay, 0x20);
    MemNSetBitFieldNb (NBPtr, BFAggrPDEn, 1);
    //
    // BFPchgPDEnDelay =
    // IF (D18F2xA8_dct[0][AggrPDEn]) THEN
    // (D18F2x200_dct[0]_mp[1:0][Tcl] + 5  +
    // CEIL((MAX(D18F2x9C_x0000_00[2A:10]_dct[0]_mp[1:0][DqsRcvEnGrossDelay])
    // + 0.5) / 2)) ELSE 00h ENDIF.
    //
    if (MemNGetBitFieldNb (NBPtr, BFAggrPDEn) == 1) {
      Tcl = NBPtr->DCTPtr->Timings.CasL;
      MemNSetBitFieldNb (NBPtr, BFPchgPDEnDelay, Tcl + 5 + NBPtr->TechPtr->GetMinMaxGrossDly (NBPtr->TechPtr, AccessRcvEnDly, TRUE) / 2 + 1);
    } else {
      MemNSetBitFieldNb (NBPtr, BFPchgPDEnDelay, 0);
    }

    // Program DRAM Power Management 1 register
    Speed = NBPtr->DCTPtr->Timings.Speed;
    i = (UINT8) ((Speed < DDR800_FREQUENCY) ? ((Speed / 66) - 3) : (Speed / 133));
    ASSERT ((i > 1) && (i < sizeof (PwrMngm1)));
    MemNSetBitFieldNb (NBPtr, BFDramPwrMngm1Reg, PwrMngm1[i]);
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
MemNBeforePlatformSpecKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemNSetBitFieldNb (NBPtr, BFCSMapCKE, 0x08040201);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function programs MaxRdLatency based on the seeded value of RxEnDly
 *      prior to DQS Receiver Enable Training
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 *
 */

BOOLEAN
MemNSetMaxRdLatBasedOnSeededRxEnDlyKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  MEM_TECH_BLOCK  *TechPtr;
  UINT8 ChipSel;

  TechPtr = NBPtr->TechPtr;

  if (TechPtr->FindMaxDlyForMaxRdLat (TechPtr, &ChipSel)) {
    NBPtr->SetMaxLatency (NBPtr, TechPtr->MaxDlyForMaxRdLat);
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function uses calculated values from DCT.Timings structure to
 *      program its registers for KB
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNProgramCycTimingsKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST CTENTRY TmgAdjTab[] = {
    // BitField, Min, Max, Bias, Ratio_x2
    {BFTcl, 5, 14, 0, 2},
    {BFTrcd, 2, 19, 0, 2},
    {BFTrp, 2, 19, 0, 2},
    {BFTrtp, 4, 10, 0, 2},
    {BFTras, 8, 40, 0, 2},
    {BFTrc, 10, 56, 0, 2},
    {BFTwrDDR3, 5, 16, 0, 2},
    {BFTrrd, 4, 9, 0, 2},
    {BFTwtr, 4, 9, 0, 2},
    {BFFourActWindow, 6, 42, 0, 2}
  };

  DCT_STRUCT *DCTPtr;
  UINT8  *MiniMaxTmg;
  UINT8  *MiniMaxTrfc;
  UINT8  Value8;
  UINT8  ValFAW;
  UINT8  ValTrrd;
  UINT8  j;
  UINT8  Tcwl;
  UINT8 RdOdtTrnOnDly;
  BIT_FIELD_NAME BitField;
  MEM_PARAMETER_STRUCT *RefPtr;

  DCTPtr = NBPtr->DCTPtr;
  RefPtr = NBPtr->RefPtr;

  ValFAW = 0;
  ValTrrd = 0;

  //======================================================================
  // Program DRAM Timing values
  //======================================================================
  //
  MiniMaxTmg = &DCTPtr->Timings.CasL;
  for (j = 0; j < GET_SIZE_OF (TmgAdjTab); j++) {
    BitField = TmgAdjTab[j].BitField;

    if (BitField == BFTrp) {
      if (NBPtr->IsSupported[AdjustTrp]) {
        MiniMaxTmg[j] ++;
        if (MiniMaxTmg[j] < 5) {
          MiniMaxTmg[j] = 5;
        }
      }
    }

    if (MiniMaxTmg[j] < TmgAdjTab[j].Min) {
      MiniMaxTmg[j] = TmgAdjTab[j].Min;
    } else if (MiniMaxTmg[j] > TmgAdjTab[j].Max) {
      MiniMaxTmg[j] = TmgAdjTab[j].Max;
    }

    Value8 = (UINT8) MiniMaxTmg[j];

    if (BitField == BFTwrDDR3) {
      if ((Value8 > 8) && ((Value8 & 1) != 0)) {
        Value8++;
      }
    } else if (BitField == BFTrrd) {
      ValTrrd = Value8;
    } else if (BitField == BFFourActWindow) {
      ValFAW = Value8;
    }

    MemNSetBitFieldNb (NBPtr, BitField, Value8);
  }

  MiniMaxTrfc = &DCTPtr->Timings.Trfc0;
  for (j = 0; j < 4; j++) {
    if ((NBPtr->DCTPtr->Timings.DctDimmValid & (1 << j)) != 0) {
      ASSERT (MiniMaxTrfc[j] <= 4);
      MemNSetBitFieldNb (NBPtr, BFTrfc0 + j, MiniMaxTrfc[j]);
    }
  }

  Tcwl = (UINT8) (DCTPtr->Timings.Speed / 133) + 2;
  Tcwl = (Tcwl > 5) ? Tcwl : 5;
  MemNSetBitFieldNb (NBPtr, BFTcwl, Tcwl);

  if (RefPtr->DramDoubleRefreshRate) {
    MemNSetBitFieldNb (NBPtr, BFTref, 3);      // 3.9 us
  } else {
    MemNSetBitFieldNb (NBPtr, BFTref, 2);      // 7.8 us
  }

  RdOdtTrnOnDly = (DCTPtr->Timings.CasL > Tcwl) ? (DCTPtr->Timings.CasL - Tcwl) : 0;
  MemNSetBitFieldNb (NBPtr, BFRdOdtTrnOnDly, RdOdtTrnOnDly);
  NBPtr->FamilySpecificHook[ProgOdtControl] (NBPtr, NULL);

  //
  // Program Tstag
  //
  if (NBPtr->MemPstate == 0) {
    for (j = 0; j < 4; j++) {
      MemNSetBitFieldNb (NBPtr, BFTstag0 + j, MAX (ValTrrd,  (ValFAW + 3) / 4));
    }
  }

  //
  // Program Tmod
  //
  MemNSetBitFieldNb (NBPtr, BFTmod, (DCTPtr->Timings.Speed < DDR1866_FREQUENCY) ? 0x0C :
                                    (DCTPtr->Timings.Speed > DDR1866_FREQUENCY) ? 0x10 : 0x0E);
  //
  // Program Tzqcs and Tzqoper
  //
  // Tzqcs max(64nCK, 80ns)
  MemNSetBitFieldNb (NBPtr, BFTzqcs, MIN (6, (MAX (64, MemUnsToMemClk (NBPtr->DCTPtr->Timings.Speed, 80)) + 15) / 16));
  // Tzqoper max(256nCK, 320ns)
  MemNSetBitFieldNb (NBPtr, BFTzqoper, MIN (0xC, (MAX (256, MemUnsToMemClk (NBPtr->DCTPtr->Timings.Speed, 320)) + 31) / 32));

  // Program power management timing
  MemNDramPowerMngTimingNb (NBPtr);
}
