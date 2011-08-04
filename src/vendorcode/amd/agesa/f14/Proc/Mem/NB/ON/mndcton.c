/* $NoKeywords:$ */
/**
 * @file
 *
 * mndcton.c
 *
 * Northbridge ON DCT supporting functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/ON)
 * @e \$Revision: 48511 $ @e \$Date: 2011-03-09 13:53:13 -0700 (Wed, 09 Mar 2011) $
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
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnon.h"
#include "merrhdl.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuCommonF14Utilities.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_NB_ON_MNDCTON_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define UNUSED_CLK 4
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
MemNAutoConfigON (
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

  MemNSetBitFieldNb (NBPtr, BFMemClkFreq, MemNGetMemClkFreqIdClientNb (NBPtr, DCTPtr->Timings.Speed));

  PowerDownMode = (UINT8) ((UserOptions.CfgPowerDownMode == POWER_DOWN_MODE_AUTO) ? POWER_DOWN_BY_CHIP_SELECT : UserOptions.CfgPowerDownMode);
  PowerDownMode = (!NBPtr->IsSupported[ChannelPDMode]) ? PowerDownMode : 0;
  IDS_OPTION_HOOK (IDS_POWERDOWN_MODE, &PowerDownMode, &(NBPtr->MemPtr->StdHeader));
  if (PowerDownMode == 1) {
    MemNSetBitFieldNb (NBPtr, BFPowerDownMode, 1);
  }

  MemNSetBitFieldNb (NBPtr, BFPchgPDModeSel, (((MemNGetBitFieldNb (NBPtr, BFLowPowerDefault)) == 1) && (NBPtr->MemPtr->PlatFormConfig->PlatformProfile.PlatformPowerPolicy == BatteryLife)) ? 0 : 1);

  MemNSetBitFieldNb (NBPtr, BFDcqBypassMax, 0xE);

  MemNSetBitFieldNb (NBPtr, BFDctSelBankSwap, 1);

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

  MemNSetBitFieldNb (NBPtr, BFWrOdtOnDuration, DEFAULT_WR_ODT_ON_ON);
  MemNSetBitFieldNb (NBPtr, BFRdOdtOnDuration, DEFAULT_RD_ODT_ON_ON);
  MemNSetBitFieldNb (NBPtr, BFWrOdtTrnOnDly, 0);

  //======================================================================
  // DRAM MRS Register, set ODT
  //======================================================================
  MemNSetBitFieldNb (NBPtr, BFBurstCtrl, 1);

  // DrvImpCtrl: drive impedance control.01b(34 ohm driver; Ron34 = Rzq/7)
  MemNSetBitFieldNb (NBPtr, BFDrvImpCtrl, 1);

  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends an MRS command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSendMrsCmdON (
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
 *      This function combines all the memory into a contiguous map.
 *      Requires that Mask values for each bank be programmed first and that
 *      the chip-select population indicator is correctly set.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - An Error value lower than AGESA_FATAL may have occurred
 *     @return          FALSE - An Error value greater than or equal to AGESA_FATAL may have occurred
 */

BOOLEAN
MemNStitchMemoryON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 NxtCSBase;
  UINT32 CurCSBase;
  UINT32 CsSize;
  UINT32 BiggestBank;
  UINT8 p;
  UINT8 q;
  UINT8 BiggestDimm;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  MCTPtr = NBPtr->MCTPtr;
  DCTPtr = NBPtr->DCTPtr;

  DCTPtr->Timings.CsEnabled = 0;
  NxtCSBase = 0;
  for (p = 0; p < MAX_CS_PER_CHANNEL_ON; p++) {
    BiggestBank = 0;
    BiggestDimm = 0;
    for (q = 0; q < MAX_CS_PER_CHANNEL_ON; q++) {
      if (((DCTPtr->Timings.CsPresent & ~DCTPtr->Timings.CsTestFail) & ((UINT16)1 << q)) != 0) {
        if ((MemNGetBitFieldNb (NBPtr, BFCSBaseAddr0Reg + q) & 7) == 0) {
          // (CSEnable|Spare==1)bank is not enabled yet
          CsSize = MemNGetBitFieldNb (NBPtr, BFCSMask0Reg + (q >> 1));
          if (CsSize != 0) {
            CsSize += ((UINT32)1 << 19);
            CsSize &= 0xFFF80000;
          }
          if (CsSize > BiggestBank) {
            BiggestBank = CsSize;
            BiggestDimm = q;
          }
        }
      }
    }

    if (BiggestBank != 0) {
      CurCSBase = NxtCSBase;
      CurCSBase |= ((UINT32)1 << BFCSEnable);
      NxtCSBase += BiggestBank;
      if ((BiggestDimm & 1) != 0) {
        if ((DCTPtr->Timings.DimmMirrorPresent & (1 << (BiggestDimm >> 1))) != 0) {
          CurCSBase |= ((UINT32)1 << BFOnDimmMirror);
        }
      }
      MemNSetBitFieldNb (NBPtr, BFCSBaseAddr0Reg + BiggestDimm, CurCSBase);
      DCTPtr->Timings.CsEnabled |= (1 << BiggestDimm);
    }
    if ((DCTPtr->Timings.CsTestFail & ((UINT16)1 << p)) != 0) {
      MemNSetBitFieldNb (NBPtr, (BFCSBaseAddr0Reg + p), (UINT32)1 << BFTestFail);
    }
  }

  if (NxtCSBase != 0) {
    DCTPtr->Timings.DctMemSize = NxtCSBase >> 8;    // Scale base address from [39:8] to [47:16]
    NBPtr->MCTPtr->NodeMemSize += NBPtr->DCTPtr->Timings.DctMemSize;
    NBPtr->MCTPtr->NodeSysLimit = NBPtr->MCTPtr->NodeMemSize - 1;
  } else {
    PutEventLog (AGESA_FATAL, MEM_ERROR_NO_DIMM_FOUND_ON_SYSTEM, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_FATAL, MCTPtr);
  }

  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function  sets the maximum round-trip latency in the system from the processor to the DRAM
 *   devices and back for Ontario.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]     MaxRcvEnDly - Maximum receiver enable delay value
 *
 */

VOID
MemNSetMaxLatencyON (
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

  N = 0x50;    // init value for MaxRdLat used in training

  if (MaxRcvEnDly != 0xFFFF) {
    T = MemNTotalSyncComponentsClientNb (NBPtr);

    // P = P + CEIL(MAX (total delay in DqsRcvEn + RdDqsTime))
    P = ((MaxRcvEnDly + MAX_RD_DQS_DLY) + 31) / 32;

    // P = P + 6.5
    // T = T + 2586 ps
    Px2 = (P * 2) + 13;
    T += 2586;

    // N = (P/(MemClkFreq * 2) + T) * NclkFreq
    MemClkPeriod = 1000000 / NBPtr->DCTPtr->Timings.Speed;
    N = ((((Px2 * MemClkPeriod + 3) / 4) + T) * NBPtr->NBClkFreq + 999999) / 1000000;
    N += 2;
  }

  NBPtr->DCTPtr->Timings.MaxRdLat = (UINT16) N;
  ASSERT (N <= 0x3FF);
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
MemNGetMaxLatParamsClientON (
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

  // P = P + 6.5
  // T = T + 2586 ps
  Px2 = (P * 2) + 13;
  T += 2586;

  // N = (P/(MemClkFreq * 2) + T) * NclkFreq
  MemClkPeriod = 1000000 / NBPtr->DCTPtr->Timings.Speed;

  *MinDlyPtr = (UINT16) (((((Px2 * MemClkPeriod + 3) / 4) + T) * NBPtr->NBClkFreq + 999999) / 1000000);

  if (NBPtr->NbFreqChgState == 1) {
    *MinDlyPtr += 2;
  } else {
    *MinDlyPtr += 1;
  }

  *MaxDlyPtr = 100 + *MinDlyPtr;  // 100 fixed iterations

  // IF ((NCLK!=MEMCLK) && (NCLK!=MEMCLK/2))
  // THEN TrainingOffset = 3
  // ELSE TrainingOffset = 2
  if ((NBPtr->NBClkFreq == NBPtr->DCTPtr->Timings.Speed) ||
      (NBPtr->NBClkFreq == (UINT32) (NBPtr->DCTPtr->Timings.Speed / 2)) ||
      (NBPtr->NBClkFreq == (UINT32) (NBPtr->DCTPtr->Timings.Speed / 2 + 1))) {
    *DlyBiasPtr = 2;
  } else {
    *DlyBiasPtr = 3;
  }

  // Register settings required before MaxRdLat training
  MemNSetBitFieldNb (NBPtr, BFForceCasToSlot0, 1);
}

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
MemNChangeNbFrequencyWrapON (
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

  Status = F14NbPstateInit (Memclk,
                            MemNGetMemClkFreqIdClientNb (NBPtr, NBPtr->DCTPtr->Timings.Speed),
                            NBPstate,
                            &NBFreq,
                            &(NBPtr->MemPtr->StdHeader));
  if (Status) {
    // When NB frequency change succeeds, TSC rate may have changed.
    // We need to update TSC rate
    GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, &NBPtr->MemPtr->StdHeader);
    FamilySpecificServices->GetTscRate (FamilySpecificServices, &NBPtr->MemPtr->TscRate, &NBPtr->MemPtr->StdHeader);
  }
  return Status;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function sets Dqs Odt for ON
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *OptParam - Optional parameter
 *
 *     @return    TRUE
 */

BOOLEAN
MemNSetDqsODTON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  if ((NBPtr->DCTPtr->Timings.Speed == DDR1333_FREQUENCY) && (NBPtr->ChannelPtr->Dimms == 1)) {
    MemNSetBitFieldNb (NBPtr, BFDQOdt03, 0x20);
    MemNSetBitFieldNb (NBPtr, BFDQOdt47, 0x20);
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function sets reduceloop and trim value for DDR-1333 for C0
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 *
 */

BOOLEAN
MemNBeforeMemClkFreqValON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  if ((NBPtr->DCTPtr->Timings.Speed == DDR1333_FREQUENCY) && ((NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F14_ON_Cx) != 0)) {
    MemNBrdcstSetNb (NBPtr, BFDllCSRBisaTrimDByte, 0x7000);
    MemNBrdcstSetNb (NBPtr, BFDllCSRBisaTrimClk, 0x7000);
    MemNBrdcstSetNb (NBPtr, BFDllCSRBisaTrimCsOdt, 0x7000);
    MemNBrdcstSetNb (NBPtr, BFDllCSRBisaTrimAByte2, 0x7000);
    MemNBrdcstSetNb (NBPtr, BFReduceLoop, 0x6000);
  }
  return TRUE;
}
