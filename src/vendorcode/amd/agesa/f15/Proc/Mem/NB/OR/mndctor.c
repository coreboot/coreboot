/* $NoKeywords:$ */
/**
 * @file
 *
 * mndctOr.c
 *
 * Northbridge DCT support for Orochi
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/OR)
 * @e \$Revision: 60556 $ @e \$Date: 2011-10-17 20:19:58 -0600 (Mon, 17 Oct 2011) $
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
#include "mt.h"
#include "mu.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnor.h"
#include "merrhdl.h"
#include "cpuFamRegisters.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuCommonF15Utilities.h"
#include "F15PackageType.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE PROC_MEM_NB_OR_MNDCTOR_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define UNUSED_CLK 4
#define MAX_RD_DQS_DLY 0x1F

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
MemNTotalSyncComponentsOr (
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
MemNAutoConfigOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 i;
  UINT8 NumDimmslots;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  MEM_PS_BLOCK * PsPtr;
  BOOLEAN ExtraAddrBits;
  BOOLEAN RankMultEn;
  UINT8 ROOD;

  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;
  DCTPtr = NBPtr->DCTPtr;
  PsPtr = NBPtr->PsPtr;

  ExtraAddrBits = FALSE;
  RankMultEn = FALSE;

  ROOD = DEFAULT_RD_ODT_OR;
  //
  // Check for Extra Address bit requirement for LRDIMMs
  //
  if (MCTPtr->Status[SbLrdimms]) {
    for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
      if (PsPtr->LrdimmRowAddrBits[i] > 16) {
        ExtraAddrBits = TRUE;
      }
      if (NBPtr->ChannelPtr->LrDimmRankMult[i] > 1) {
        RankMultEn = TRUE;
      }
    }
  }

  NumDimmslots = GetMaxDimmsPerChannel (RefPtr->PlatformMemoryConfiguration,
                                        MCTPtr->SocketId,
                                        NBPtr->ChannelPtr->ChannelID);
  //
  //======================================================================
  // Build Dram Config Lo Register Value
  //======================================================================
  //
  // Disable Parity Prior to Dram init
  //
  MemNSetBitFieldNb (NBPtr, BFParEn, 0);
  //
  // LRDIMMS Extended Parity
  //
  MemNSetBitFieldNb (NBPtr, BFExtendedParityEn, 0);
  //
  // X4Dimm
  //
  MemNSetBitFieldNb (NBPtr, BFX4Dimm, NBPtr->ChannelPtr->DimmNibbleAccess & 0xF);
  //
  // UnBuffDimm
  //
  if (!(MCTPtr->Status[SbRegistered] || MCTPtr->Status[SbLrdimms])) {
    MemNSetBitFieldNb (NBPtr, BFUnBuffDimm, 1);
  }
  //
  // DimmEccEn
  //
  if (MCTPtr->Status[SbEccDimms]) {
    MemNSetBitFieldNb (NBPtr, BFDimmEccEn, 1);
  }
  //
  // PendRefPayback, StatgRefEn, TStag[0:4]
  //
  MemNSetBitFieldNb (NBPtr, BFPendRefPaybackS3En, 1);
  MemNSetBitFieldNb (NBPtr, BFStagRefEn, 1);
  for (i = 0; i < 4; i++) {
    MemNSetBitFieldNb (NBPtr, BFTstag0 + i, 0x14);
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
  //
  // FourRankRDimm0 , FourRankRDimm1 ( Reset Values are 0)
  //
  //======================================================================
  // Build Dram MRS Register Value (Not used for MRS command)
  //======================================================================
  //
  // PchgPDModeSel - This is done here so that the value can be used by
  //                 MR0 function
  //
  if (NBPtr->IsSupported[PchgPDMode]) {
    MemNSetBitFieldNb (NBPtr, BFPchgPDModeSel, 1);
  }
  MemNSetBitFieldNb (NBPtr, BFBurstCtrl, 0);

  //======================================================================
  // Build Dram Config Misc Register Value
  //======================================================================
  //
  //
  // LRDIMMs CSMux45 and CSMux67
  //
  if (MCTPtr->Status[SbLrdimms]) {
    if (NumDimmslots == 3) {
      MemNSetBitFieldNb (NBPtr, BFCSMux45, 0);
      MemNSetBitFieldNb (NBPtr, BFCSMux67, ExtraAddrBits ? 1 : 0);
    } else if (NumDimmslots <= 2) {
      MemNSetBitFieldNb (NBPtr, BFCSMux45, (PsPtr->LrdimmRowAddrBits[0] > 16) ? 1 : 0);
      MemNSetBitFieldNb (NBPtr, BFCSMux67, (PsPtr->LrdimmRowAddrBits[1] > 16) ? 1 : 0);
    }
  }
  //
  // LrDimmMrsCtrl
  //
  MemNSetBitFieldNb (NBPtr, BFLrDimmMrsCtrl, RankMultEn ? 1 : 0);
  //
  // BFLrDimmEnhRefEn
  //
  MemNSetBitFieldNb (NBPtr, BFLrDimmEnhRefEn, RankMultEn ? 1 : 0);
  //
  // SubMemclkRegDly
  //
  MemNSetBitFieldNb (NBPtr, BFSubMemclkRegDly, (MCTPtr->Status[SbRegistered] || MCTPtr->Status[SbLrdimms])? 1 : 0);
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

  if (NBPtr->MCTPtr->Status[SbLrdimms]) {
    ROOD = ROOD + (UINT8) (MemNCalBufDatDelaySkewOr (NBPtr, GetBufDatDlySkew));
  }
  MemNSetBitFieldNb (NBPtr, BFWrOdtOnDuration, DEFAULT_WR_ODT_OR);
  MemNSetBitFieldNb (NBPtr, BFRdOdtOnDuration, ROOD);
  MemNSetBitFieldNb (NBPtr, BFWrOdtTrnOnDly, 0);

  MemNSetBitFieldNb (NBPtr, BFTmrd, (NBPtr->MCTPtr->Status[SbLrdimms] ? 6 : 4));

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
MemNCapSpeedBatteryLifeOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST UINT16 SupportedFreq[] = {
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

  FamilySpecificServices = NULL;
  GetCpuServicesOfSocket (NBPtr->MCTPtr->SocketId, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, &(NBPtr->MemPtr->StdHeader));

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
      NBFreq = FreqNumeratorInMHz / FreqDivisor;
      break;
    }
  }

  ASSERT (NBFreq > 0);

  // Pick Max MEMCLK that is less than or equal to (NCLK / 2)
  DdrFreq = DDR800_FREQUENCY;
  for (j = 0; j < GET_SIZE_OF (SupportedFreq); j++) {
    if (NBFreq >= ((UINT32) 2 * SupportedFreq[j])) {
      DdrFreq = SupportedFreq[j];
      break;
    }
  }

  // Cap MemClk frequency to lowest NCLK frequency
  if (NBPtr->DCTPtr->Timings.TargetSpeed > DdrFreq) {
    NBPtr->DCTPtr->Timings.TargetSpeed = DdrFreq;
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
MemNGetMaxLatParamsOr (
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

  // Get all sync components BKDG steps 3,4,6,7
  P = MemNTotalSyncComponentsOr (NBPtr);

  // 8. P = P + CEIL(MAX(D18F2x9C_x0000_00[2A:10]_dct[1:0][DqsRcvEnGrossDelay, DqsRcvEnFineDelay] +
  //    D18F2x9C_x0000_0[3:0]0[7:5]_dct[1:0][RdDqsTime] PCLKs))
  P = P + (MaxRcvEnDly + 31) / 32;

  // 10. T = T + 800 ps
  T += 800;

  // 11. N = (P/(MemClkFreq * 2) + T) * NclkFreq; Convert from PCLKs plus time to NCLKs.
  MemClkPeriod = 1000000 / NBPtr->DCTPtr->Timings.Speed;
  N = ((((P * MemClkPeriod + 1) / 2) + T) * NBPtr->NBClkFreq + 999999) / 1000000;

  // 13. D18F2x210_dct[1:0]_nbp[3:0][MaxRdLatency] = CEIL(N) - 1
  N = N - 1;

  // Calculate a starting MaxRdLatency delay value with steps 5, 9, and 12 excluded
  *MinDlyPtr = (UINT16) N;

  *MaxDlyPtr = 0x3FF;

  // Left edge of MaxRdLat will be added with 1 NCLK and 3 PCLK
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
MemNSetMaxLatencyOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  )
{
  UINT32 N;
  UINT32 T;
  UINT32 P;
  UINT32 MemClkPeriod;

  AGESA_TESTPOINT (TpProcMemRcvrCalcLatency, &(NBPtr->MemPtr->StdHeader));

  //
  // Initial value for MaxRdLat used in training
  //
  N = 0x55;

  if (MaxRcvEnDly != 0xFFFF) {
    // 1. P = N = T = 0.
    P = N = T = 0;

    // Get all sync components BKDG steps 3,4,6,7
    P = MemNTotalSyncComponentsOr (NBPtr);

    // 5. P = P + 5
    P += 5;

    // 8. P = P + CEIL(MAX(D18F2x9C_x0000_00[2A:10]_dct[1:0][DqsRcvEnGrossDelay, DqsRcvEnFineDelay] +
    //    D18F2x9C_x0000_0[3:0]0[7:5]_dct[1:0][RdDqsTime] PCLKs))
    P = P + ((MaxRcvEnDly + MAX_RD_DQS_DLY) + 31) / 32;

    // 9. P = P + 5
    P += 5;

    // 10. T = T + 800 ps
    T += 800;

    // 11. N = (P/(MemClkFreq * 2) + T) * NclkFreq; Convert from PCLKs plus time to NCLKs.
    MemClkPeriod = 1000000 / NBPtr->DCTPtr->Timings.Speed;
    N = ((((P * MemClkPeriod + 1) / 2) + T) * NBPtr->NBClkFreq + 999999) / 1000000;

    // 12. N = N - 1. See step 9.
    N = N - 1;

    // 13. D18F2x210_dct[1:0]_nbp[3:0][MaxRdLatency] = CEIL(N) - 1
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
MemNExitPhyAssistedTrainingOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 Dct;
  UINT8 ChipSel;
  MEM_TECH_BLOCK  *TechPtr;

  TechPtr = NBPtr->TechPtr;

  MemNReEnablePhyCompNb (NBPtr, NULL);

  // Calculate Max Latency for both channels to prepare for position training
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
    NBPtr->SwitchDCT (NBPtr, Dct);
    //
    // For Orochi, we need to reset DisAutoRefresh and ZqcsInterval for
    // Position training.
    //
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 1);
      MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 0);
    }

    if (TechPtr->FindMaxDlyForMaxRdLat (TechPtr, &ChipSel)) {
      NBPtr->SetMaxLatency (NBPtr, TechPtr->MaxDlyForMaxRdLat);
    }
  }

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/*-----------------------------------------------------------------------------
 *
 *     This function send control words after MEMCLK frequency change
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNAfterMemClkFreqChgOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 Dct;
  MEM_TECH_BLOCK *TechPtr;

  TechPtr = NBPtr->TechPtr;

  // Reprogram the DIMMs' buffers right after MEMCLK frequency change
  if (!(TechPtr->TechnologySpecificHook[LrdimmFreqChgCtrlWrd] (TechPtr, NULL))) {
    if (NBPtr->MCTPtr->Status[SbRegistered]) {
      for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
        MemNSwitchDCTNb (NBPtr, Dct);
        if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
          TechPtr->FreqChgCtrlWrd (TechPtr);
        }
      }
    }
  }

  return TRUE;
}

/*-----------------------------------------------------------------------------
 *
 *     This function modifies CS tri-state bit map
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  CsTriBitmap - Bitmap of chipselects to be tristated
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNBeforeSetCsTriOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *CsTriBitmap
  )
{
  //
  // The tri-state of CS[7:4] for LrDIMM should be determined by
  // D18F2xA8_dct[1:0][CsMux45]/[CsMux67]
  if (NBPtr->MCTPtr->Status[SbLrdimms]) {
    if (MemNGetBitFieldNb (NBPtr, BFCSMux45) == 1) {
      *(UINT8*) CsTriBitmap &= 0xCF;
    }
    if (MemNGetBitFieldNb (NBPtr, BFCSMux67) == 1) {
      *(UINT8*) CsTriBitmap &= 0x3F;
    }
  }

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
 *
 *   This function gets the total of sync components for Max Read Latency calculation
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      Total in PCLKs
 */
UINT32
STATIC
MemNTotalSyncComponentsOr (
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

  // 4. P = P + (8 - D18F2x210_dct[1:0]_nbp[3:0][RdPtrInit]) + 1
  P = P + (8 - (UINT16) MemNGetBitFieldNb (NBPtr, BFRdPtrInit)) + 1;

  // 6. If (D18F2xA8_dct[1:0][SubMemclkRegDly] = 0 & D18F2x90_dct[1:0][UnbuffDimm] = 0)
  // then P = P + 2
  if ((MemNGetBitFieldNb (NBPtr, BFSubMemclkRegDly) == 0) && (MemNGetBitFieldNb (NBPtr, BFUnBuffDimm) == 0)) {
    P += 2;
  }

  // 7. P = P + (2 * (D18F2x200_dct[1:0][Tcl] - 1 clocks))
  P = P + (2 * (MemNGetBitFieldNb (NBPtr, BFTcl) - 1));

  return P;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function is used to calculate BufDatDelay and BufDatDelaySkew value.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   GetDelay   - Get either BufDatDly or BufDatDlySkew value
 *
 *     @return      BufDatDly or BufDatDlySkew value
 */
UINT32
MemNCalBufDatDelaySkewOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8   GetDelay
  )
{
  UINT8 SpdOffset;
  UINT8 Dimm;
  UINT8 i;
  UINT8 j;
  UINT32 MinModuleDly;
  UINT32 MaxModuleDly;
  UINT8 SyncDelay;
  UINT32 SmallestModuleDly;
  UINT32 LargestModuleDly;
  UINT32 BufDatDly;
  UINT32 BufDatDlySkew;
  MEM_TECH_BLOCK  *TechPtr;
  UINT8 *SpdBufferPtr;

  TechPtr = NBPtr->TechPtr;

  i = 0;
  j = 0;
  SmallestModuleDly = 0;
  LargestModuleDly = 0;
  SyncDelay = 0;
  BufDatDly = 0;
  BufDatDlySkew = 0;
  MinModuleDly = 0xFF;
  MaxModuleDly = 0;
  SpdOffset = (UINT8) (2 * CONVERT_VDDIO_TO_ENCODED (NBPtr->RefPtr->DDR3Voltage));

  for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
    if (TechPtr->GetDimmSpdBuffer (TechPtr, &SpdBufferPtr, Dimm)) {
      if ((SpdBufferPtr[90 + SpdOffset] & 0x7F) < (UINT8) MinModuleDly) {
        MinModuleDly = SpdBufferPtr[90 + SpdOffset] & 0x7F;
        i = Dimm;
      }
      if ((SpdBufferPtr[91 + SpdOffset] & 0x7F) > (UINT8) MaxModuleDly) {
        MaxModuleDly = SpdBufferPtr[91 + SpdOffset] & 0x7F;
        j = Dimm;
      }
    }
  }
  if (!(MinModuleDly >= 0x3C && MinModuleDly <= 0x58)) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\tMinModuleDly out of range (0x3C - 0x58): %02x\n", MinModuleDly);
  }
  if (!(MaxModuleDly >= 0x3C && MaxModuleDly <= 0x58)) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\tMaxModuleDly out of range (0x3C - 0x58): %02x\n", MaxModuleDly);
  }
  //
  //Calculate BufDatDly
  //
  //SmallestModuleDelay = MinimumModuleDelay * .000125 us * 400 MHz * 0x40.
  //MinimumModuleDelay is the minimum SPD module delay across all DIMMs on a channel.
  //
  SmallestModuleDly =  (UINT32) (MinModuleDly * 125 * 400 * 0x40 / 1000000);
  //
  //SyncDelay = (F0RC2[AddrCmdPrelaunch] ? 0x30 - (2*F1RC12[QCAPrelaunchDelay]) : 0x20) + 0x10.
  //SyncDelay is calculated from the SPD values of the MinimumModuleDelay DIMM.
  //
  TechPtr->GetDimmSpdBuffer (TechPtr, &SpdBufferPtr, i);
  SyncDelay = (UINT8) ((((SpdBufferPtr[67] & 0x01) == 1) ? (0x30 - 2 * (SpdBufferPtr[70] & 0x07)) : 0x20) + 0x10);
  //
  //BufDatDelay = FLOOR((((SmallestModuleDelay - SynchDelay) * (MemClkFreq/400 MHz)) + SynchDelay)/0x40).
  //
  BufDatDly = (((SmallestModuleDly - SyncDelay) * NBPtr->DCTPtr->Timings.Speed / 400) + SyncDelay) / 0x40;

  if (GetDelay == GetBufDatDly) {
    return BufDatDly;
  }
  //
  //Calculate BufDatDlySkew
  //
  //LargestModuleDelay = MaximumModuleDelay * .000125 us * 400 MHz * 0x40.
  //MaximumModuleDelay is the maximum SPD module delay across all DIMMs on a channel.
  //
  LargestModuleDly = (UINT32) (MaxModuleDly * 125 * 400 * 0x40 / 1000000);
  //
  //SyncDelay = (F0RC2[AddrCmdPrelaunch] ? 0x30 - (2*F1RC12[QCAPrelaunchDelay]) : 0x20) + 0x10.
  //SyncDelay is calculated from the SPD values of the MaximumModuleDelay DIMM.
  //
  TechPtr->GetDimmSpdBuffer (TechPtr, &SpdBufferPtr, j);
  SyncDelay = (UINT8) ((((SpdBufferPtr[67] & 0x01) == 1) ? (0x30 - 2 * (SpdBufferPtr[70] & 0x07)) : 0x20) + 0x10);
  //
  //BufDatDelaySkew = FLOOR((((LargestModuleDelay - SynchDelay) * (MemClkFreq/400 MHz)) + SynchDelay)/0x40) - BufDatDelay.
  //
  BufDatDlySkew = ((((((LargestModuleDly - SyncDelay) * NBPtr->DCTPtr->Timings.Speed + 200) / 400) + SyncDelay) - (BufDatDly * 0x40) + 0x20) / 0x40);

  return BufDatDlySkew ;
}
/*-----------------------------------------------------------------------------
 *
 *     This function Enables parity on both DCTs if Parity is supported.
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  *OptParam  - Unused
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNEnableParityAfterMemRstOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  DIE_STRUCT *MCTPtr;
  MEM_PS_BLOCK * PsPtr;
  UINT8 i;

  MCTPtr = NBPtr->MCTPtr;
  PsPtr = NBPtr->PsPtr;

  if (NBPtr->MCTPtr->Status[SbParDimms]) {
    //
    // SbParDimms should be set for all DDR3 RDIMMS
    // Cannot turn off ParEn for DDR3
    //
    MemNSetBitFieldNb (NBPtr, BFParEn, 1);
    //
    // LRDIMMS Extended Parity
    //
    if (MCTPtr->Status[SbLrdimms]) {
      for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
        if (PsPtr->LrdimmRowAddrBits[i] > 16) {
          MemNSetBitFieldNb (NBPtr, BFExtendedParityEn, 1);
          break;
        }
      }
    }
  }
  return TRUE;
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function calculates RdOdtTrnOnDly and RdOdtOnDuration when LrDimms
 *     are present
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  *OptParam  - Not Used
 *
 *     @return  TRUE - always
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNProgOdtControlOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  DCT_STRUCT *DCTPtr;
  UINT8 Tcwl;
  UINT8 RdOdtTrnOnDly;
  UINT8 RdOdtOnDuration;

  DCTPtr = NBPtr->DCTPtr;
  Tcwl = (UINT8) (DCTPtr->Timings.Speed / 133) + 2;
  RdOdtTrnOnDly = (DCTPtr->Timings.CasL > Tcwl) ? (DCTPtr->Timings.CasL - Tcwl) : 0;
  RdOdtOnDuration = 6;
  if (NBPtr->MCTPtr->Status[SbLrdimms]) {
    RdOdtTrnOnDly = RdOdtTrnOnDly + (UINT8) MemNCalBufDatDelaySkewOr (NBPtr, GetBufDatDly);
    RdOdtOnDuration = RdOdtOnDuration + (UINT8) MemNCalBufDatDelaySkewOr (NBPtr, GetBufDatDlySkew);
  }
  MemNSetBitFieldNb (NBPtr, BFRdOdtTrnOnDly, RdOdtTrnOnDly);
  MemNSetBitFieldNb (NBPtr, BFRdOdtOnDuration, RdOdtOnDuration);
  IDS_HDT_CONSOLE (MEM_FLOW,"\n\t\t\tRdOdtTrnOnDly = %x",RdOdtTrnOnDly);
  IDS_HDT_CONSOLE (MEM_FLOW,"\n\t\t\tRdOdtOnDuration = %x\n",RdOdtOnDuration);
  return TRUE;
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
MemNBeforeDramInitOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_OR; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      //
      // 2.10.6.6 DCT Training Specific Configuration
      //
      MemNSetBitFieldNb (NBPtr, BFAddrCmdTriEn, 0);
      MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 1);
      MemNSetBitFieldNb (NBPtr, BFDisDllShutdownSR, 1);
      MemNSetBitFieldNb (NBPtr, BFForceAutoPchg, 0);
      MemNSetBitFieldNb (NBPtr, BFDynPageCloseEn, 0);
      MemNSetBitFieldNb (NBPtr, BFBankSwizzleMode, 0);
      MemNSetBitFieldNb (NBPtr, BFDcqBypassMax, 0);
      MemNSetBitFieldNb (NBPtr, BFPowerDownEn, 0);
      MemNSetBitFieldNb (NBPtr, BFDisSimulRdWr, 0);
      MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 0);
      MemNSetBitFieldNb (NBPtr, BFRxMaxDurDllNoLock, 0);
      MemNSetBitFieldNb (NBPtr, BFTxMaxDurDllNoLock, 0);
      MemNSetBitFieldNb (NBPtr, BFEnRxPadStandby, 0);
      MemNSetBitFieldNb (NBPtr, BFBwCapEn, 0);
      MemNSetBitFieldNb (NBPtr, BFODTSEn, 0);
      MemNSetBitFieldNb (NBPtr, BFDctSelIntLvEn, 0);
      MemNSetBitFieldNb (NBPtr, BFL3Scrub, 0);
      MemNSetBitFieldNb (NBPtr, BFDramScrub, 0);
      MemNSetBitFieldNb (NBPtr, BFScrubReDirEn, 0);
      MemNSetBitFieldNb (NBPtr, BFL3ScrbRedirDis, 1);
    }
  }
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
MemNReleaseNbPstateOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  // 6. Restore the initial D18F5x170[SwNbPstateLoDis, NbPstateDisOnP0] values.
  MemNSetBitFieldNb (NBPtr, BFNbPstateCtlReg, (MemNGetBitFieldNb (NBPtr, BFNbPstateCtlReg) & 0xFFFF9FFF) | (NBPtr->NbPsCtlReg & 0x6000));

  // 7. Restore the initial D18F5x170[NbPstateThreshold, NbPstateHi] values.
  MemNSetBitFieldNb (NBPtr, BFNbPstateCtlReg, (MemNGetBitFieldNb (NBPtr, BFNbPstateCtlReg) & 0xFFFFF13F) | (NBPtr->NbPsCtlReg & 0x0EC0));

  return TRUE;
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
MemNPowerDownCtlOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 PowerDownMode;
  UINT8 Tmod;
  CONST UINT32 PwrMngm1[] = {0, 0, 0x05050403, 0x05050403, 0x06060403, 0x07070504, 0x08080504, 0x0A0A0605, 0x0B0B0706};
  UINT8 i;
  UINT16 Speed;
  UINT32 PackageType;

  if (NBPtr->RefPtr->EnablePowerDown) {
    PackageType = LibAmdGetPackageType (&(NBPtr->MemPtr->StdHeader));
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
    Tmod = (UINT8) MemNGetBitFieldNb (NBPtr, BFTmod);
    //
    // Partial Channel Power Down
    //
    MemNSetBitFieldNb (NBPtr, BFPrtlChPDDynDly, 2);
    MemNSetBitFieldNb (NBPtr, BFPrtlChPDEnhEn, 1);
    //
    // Aggressive PowerDown
    // PchgPDEnDelay: IF (D18F2xA8_dct[1:0][AggrPDEn]) THEN 1 ELSE 0 ENDIF.
    //
    MemNSetBitFieldNb (NBPtr, BFAggrPDDelay, 0);
    if (PackageType != PACKAGE_TYPE_AM3r2) {
      MemNSetBitFieldNb (NBPtr, BFAggrPDEn, 1);
      MemNSetBitFieldNb (NBPtr, BFPchgPDEnDelay, 1);
    }

    // Program DRAM Power Management 1 register
    Speed = NBPtr->DCTPtr->Timings.Speed;
    i = (UINT8) ((Speed < DDR800_FREQUENCY) ? ((Speed / 66) - 3) : (Speed / 133));
    ASSERT ((i > 1) && (i < sizeof (PwrMngm1)));
    MemNSetBitFieldNb (NBPtr, BFDramPwrMngm1Reg, PwrMngm1[i]);
  }
}
