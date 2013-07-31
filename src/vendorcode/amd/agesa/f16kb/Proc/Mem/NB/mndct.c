/* $NoKeywords:$ */
/**
 * @file
 *
 * mndct.c
 *
 * Common Northbridge DCT support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
 * @e \$Revision: 85818 $ @e \$Date: 2013-01-11 17:04:21 -0600 (Fri, 11 Jan 2013) $
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
#include "mftds.h"
#include "merrhdl.h"
#include "cpuFamilyTranslation.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_MNDCT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define UNUSED_CLK 4

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

VOID
STATIC
MemNAfterStitchMemNb (
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
MemNStitchMemoryNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  BOOLEAN DSpareEn;
  UINT32 NxtCSBase;
  UINT32 CurCSBase;
  UINT32 CsSize;
  UINT32 BiggestBank;
  UINT8 p;
  UINT8 q;
  UINT8 BiggestDimm;
  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;
  DCTPtr = NBPtr->DCTPtr;
  DSpareEn = FALSE;
  if (NBPtr->IsSupported[SetSpareEn]) {
    DSpareEn = FALSE;
    if (RefPtr->GStatus[GsbEnDIMMSpareNW]) {
      DSpareEn = TRUE;
    }
  }

  DCTPtr->Timings.CsEnabled = 0;
  NxtCSBase = 0;
  for (p = 0; p < MAX_CS_PER_CHANNEL; p++) {
    BiggestBank = 0;
    BiggestDimm = 0;
    for (q = 0; q < MAX_CS_PER_CHANNEL; q++) {
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
      if (NBPtr->IsSupported[CheckSpareEn]) {
        if (DSpareEn) {
          CurCSBase = ((UINT32)1 << BFSpare);
          DSpareEn = FALSE;
        } else {
          CurCSBase |= ((UINT32)1 << BFCSEnable);
          NxtCSBase += BiggestBank;
        }
      } else {
        CurCSBase |= ((UINT32)1 << BFCSEnable);
        NxtCSBase += BiggestBank;
      }
      if ((BiggestDimm & 1) != 0) {
        if (!(MCTPtr->Status[SbLrdimms])) {
          // For LRDIMMS, On Dimm Mirroring is enabled after SDI
          if ((DCTPtr->Timings.DimmMirrorPresent & (1 << (BiggestDimm >> 1))) != 0) {
            CurCSBase |= ((UINT32)1 << BFOnDimmMirror);
          }
        }
      }
      MemNSetBitFieldNb (NBPtr, BFCSBaseAddr0Reg + BiggestDimm, CurCSBase);
      DCTPtr->Timings.CsEnabled |= (1 << BiggestDimm);
    }
    if ((DCTPtr->Timings.CsTestFail & ((UINT16)1 << p)) != 0) {
      IDS_HDT_CONSOLE (MEM_FLOW, "Node %d Dct %d exclude CS %d\n", NBPtr->Node, NBPtr->Dct, p);
      MemNSetBitFieldNb (NBPtr, (BFCSBaseAddr0Reg + p), (UINT32)1 << BFTestFail);
    }
  }

  if (NxtCSBase != 0) {
    DCTPtr->Timings.DctMemSize = NxtCSBase >> 8;    // Scale base address from [39:8] to [47:16]
    MemNAfterStitchMemNb (NBPtr);
  }

  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets platform specific config/timing values from the interface layer and
 *   programs them into DCT.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - An Error value lower than AGESA_FATAL may have occurred
 *     @return          FALSE - An Error value greater than or equal to AGESA_FATAL may have occurred
 */

BOOLEAN
MemNPlatformSpecUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 MemClkDis;
  UINT8 i;
  UINT8 MemoryAllClocks;
  UINT8 *MemClkDisMap;
  UINT16 CsPresent;

  if (!MemNGetPlatformCfgNb (NBPtr)) {
    IDS_ERROR_TRAP;
  }

  if (!NBPtr->PsPtr->MemPDoPs (NBPtr)) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\tDisable DCT%d due to unsupported DIMM configuration\n", NBPtr->Dct);
    if (!NBPtr->MemPtr->ErrorHandling (NBPtr->MCTPtr, NBPtr->Dct, EXCLUDE_ALL_CHIPSEL, &NBPtr->MemPtr->StdHeader)) {
      ASSERT (FALSE);
    }
    NBPtr->DisableDCT (NBPtr);
  } else {

    MemNProgramPlatformSpecNb (NBPtr);
    MemProcessConditionalOverrides (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr, PSO_ACTION_ODT, ALL_DIMMS);

    //======================================================================
    // Disable unused MemClk to save power
    //======================================================================
    //
    MemClkDis = 0;
    MemoryAllClocks = UserOptions.CfgMemoryAllClocksOn;
    IDS_OPTION_HOOK (IDS_ALL_MEMORY_CLOCK, &MemoryAllClocks, &(NBPtr->MemPtr->StdHeader));
    if (!MemoryAllClocks) {
      // Special Jedec SPD diagnostic bit - "enable all clocks"
      if (!NBPtr->MCTPtr->Status[SbDiagClks]) {
        MemClkDisMap = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MEMCLK_DIS, NBPtr->MCTPtr->SocketId, NBPtr->Dct, 0,
                                            &(NBPtr->MCTPtr->LogicalCpuid), &(NBPtr->MemPtr->StdHeader));
        if (MemClkDisMap == NULL) {
          MemClkDisMap = NBPtr->ChannelPtr->MemClkDisMap;
        }

        // Turn off unused clocks
        CsPresent = NBPtr->DCTPtr->Timings.CsPresent;

        for (i = 0; i < 8; i++) {
          if ((CsPresent & MemClkDisMap[i]) == 0) {
            MemClkDis |= (UINT8) (1 << i);
          }
        }

        // Turn off unused chiplets
        for (i = 0; i < 3; i++) {
          if (((MemClkDis >> (i * 2)) & 0x3) == 0x3) {
            MemNSetBitFieldNb (NBPtr, BFPhyClkConfig0 + i, 0x0010);
          }
        }
      }
    }
    MemNSetBitFieldNb (NBPtr, BFMemClkDis, MemClkDis);
    MemFInitTableDrive (NBPtr, MTAfterPlatformSpec);
  }

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function disables the DCT and mem clock for UNB
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNDisableDCTUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemNSetBitFieldNb (NBPtr, BFExtendedParityEn, 0);
  MemNSetBitFieldNb (NBPtr, BFParEn, 0);
  MemNSetBitFieldNb (NBPtr, BFCKETri, 0x0F);

  //Wait for 24 MEMCLKs
  MemNWaitXMemClksNb (NBPtr, 24);

  // To maximize power savings when DisDramInterface=1b,
  // all of the MemClkDis bits should also be set.
  //
  MemNSetBitFieldNb (NBPtr, BFMemClkDis, 0xFF);

  MemNSetBitFieldNb (NBPtr, BFDisDramInterface, 1);

  if (NBPtr->Dct == 0) {
    MemNSetBitFieldNb (NBPtr, BFPhyPSMasterChannel, 0x100);
  }

  // If channel is disabled after dram init, set DisDllShutdownSR
  if (MemNGetBitFieldNb (NBPtr, BFDramEnabled) == 1) {
    MemNSetBitFieldNb (NBPtr, BFDisDllShutdownSR, 1);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *  This function initializes the DRAM devices on all DCTs at the same time
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNStartupDCTUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  UINT16 FinalPllLockTime;

  if (NBPtr->MCTPtr->NodeMemSize != 0) {
    // Update NB frequency for startup DDR speed
    NBPtr->ChangeNbFrequency (NBPtr);

    if (!NBPtr->IsSupported[ForcePhyToM0]) {
      // Program D18F2x[1,0]9C_x0000_000B = 80000000h. #109999.
      MemNBrdcstSetNb (NBPtr, BFDramPhyStatusReg, 0x80000000);

      // Program D18F2x[1,0]9C_x0D0F_E013[PllRegWaitTime] = 0118h. #194060.
      MemNBrdcstSetNb (NBPtr, BFPllRegWaitTime, 0x118);
    }

    // Phy Voltage Level Programming
    MemNPhyVoltageLevelNb (NBPtr);

    // Run frequency change sequence
    MemNBrdcstSetNb (NBPtr, BFPllLockTime, NBPtr->FreqChangeParam->PllLockTimeDefault);
    MemNBrdcstSetNb (NBPtr, BFMemClkFreq, NBPtr->GetMemClkFreqId (NBPtr, NBPtr->DCTPtr->Timings.Speed));
    NBPtr->FamilySpecificHook[SetSkewMemClk] (NBPtr, NULL);
    NBPtr->ProgramNbPsDependentRegs (NBPtr);
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      MemNSwitchDCTNb (NBPtr, Dct);
      if ((NBPtr->DCTPtr->Timings.DctMemSize != 0)) {
        NBPtr->FamilySpecificHook[IntermediateMemclkFreqVal] (NBPtr, NULL);
        MemNSetBitFieldNb (NBPtr, BFMemClkFreqVal, 1);
        MemNPollBitFieldNb (NBPtr, BFFreqChgInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
      }
    }
    FinalPllLockTime = 0xF;
    NBPtr->FamilySpecificHook[AfterMemClkFreqVal] (NBPtr, &FinalPllLockTime);
    if (!NBPtr->IsSupported[CsrPhyPllPdEn]) {
      // IF (D18F2x[1,0]9C_x0D0F_E00A[CsrPhySrPllPdMode]==0) THEN program
      // D18F2x[1,0]9C_x0D0F_E006[PllLockTime] = 0Fh
      MemNBrdcstSetNb (NBPtr, BFPllLockTime, FinalPllLockTime);
    }

    NBPtr->FamilySpecificHook[BeforePhyFenceTraining] (NBPtr, NBPtr);

    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      MemNSwitchDCTNb (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);

        // Phy fence programming
        AGESA_TESTPOINT (TpProcMemPhyFenceTraining, &(NBPtr->MemPtr->StdHeader));
        NBPtr->PhyFenceTraining (NBPtr);

        // Phy compensation initialization
        AGESA_TESTPOINT (TPProcMemPhyCompensation, &(NBPtr->MemPtr->StdHeader));
        NBPtr->MemNInitPhyComp (NBPtr);
        MemProcessConditionalOverrides (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr, PSO_ACTION_SLEWRATE, ALL_DIMMS);
      }
    }

    AGESA_TESTPOINT (TpProcMemBeforeDramInit, &(NBPtr->MemPtr->StdHeader));
    NBPtr->MemNBeforeDramInitNb (NBPtr);

    AGESA_TESTPOINT (TpProcMemDramInit, &(NBPtr->MemPtr->StdHeader));
    IDS_HDT_CONSOLE (MEM_FLOW, "\nMemClkFreq: %d MHz\n", NBPtr->DCTPtr->Timings.Speed);
    NBPtr->FeatPtr->DramInit (NBPtr->TechPtr);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function ramp up frequency to target frequency
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNRampUpFrequencyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  DIE_STRUCT *MCTPtr;

  MCTPtr = NBPtr->MCTPtr;

  // Do not change frequency when it is already at TargetSpeed
  if (NBPtr->DCTPtr->Timings.Speed == NBPtr->DCTPtr->Timings.TargetSpeed) {
    return TRUE;
  }

  // BIOS must program both DCTs to the same frequency.
  IDS_HDT_CONSOLE (MEM_FLOW, "\nMemClkFreq changed: %d MHz", NBPtr->DCTPtr->Timings.Speed);
  for (Dct = 0; Dct < MCTPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    NBPtr->DCTPtr->Timings.Speed = NBPtr->DCTPtr->Timings.TargetSpeed;
  }
  IDS_HDT_CONSOLE (MEM_FLOW, " -> %d MHz", NBPtr->DCTPtr->Timings.TargetSpeed);

  NBPtr->ChangeFrequency (NBPtr);

  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function uses calculated values from DCT.Timings structure to
 *      program its registers for UNB
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNProgramCycTimingsUnb (
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
  UINT8  j;
  UINT8  Tcwl;
  UINT8 RdOdtTrnOnDly;
  BIT_FIELD_NAME BitField;
  MEM_PARAMETER_STRUCT *RefPtr;

  DCTPtr = NBPtr->DCTPtr;
  RefPtr = NBPtr->RefPtr;
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

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets platform specific settings for the current channel
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - All platform types defined have initialized successfully
 *     @return          FALSE - At least one of the platform types gave not been initialized successfully
 */

BOOLEAN
MemNGetPlatformCfgNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 p;

  for (p = 0; p < MAX_PLATFORM_TYPES; p++) {
    ASSERT (NBPtr->MemPtr->GetPlatformCfg[p] != NULL);
    if (NBPtr->MemPtr->GetPlatformCfg[p] (NBPtr->MemPtr, NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr) == AGESA_SUCCESS) {
      break;
    }
  }
  return (p < MAX_PLATFORM_TYPES);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sends the ZQCL command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSendZQCmdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  // 1.Program MrsAddress[10]=1
  MemNSetBitFieldNb (NBPtr, BFMrsAddress, (UINT32)1 << 10);

  // 2.Set SendZQCmd=1
  MemNSetBitFieldNb (NBPtr, BFSendZQCmd, 1);

  // 3.Wait for SendZQCmd=0
  MemNPollBitFieldNb (NBPtr, BFSendZQCmd, 0, PCI_ACCESS_TIMEOUT, FALSE);

  // 4.Wait 512 MEMCLKs
  MemNWaitXMemClksNb (NBPtr, 512);
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
 *   This function is used to create the DRAM map
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 */

VOID
STATIC
MemNAfterStitchMemNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if (NBPtr->MCTPtr->GangedMode) {
    NBPtr->MCTPtr->NodeMemSize = NBPtr->DCTPtr->Timings.DctMemSize;
    NBPtr->MCTPtr->NodeSysLimit = NBPtr->MCTPtr->NodeMemSize - 1;
    NBPtr->MCTPtr->DctData[1].Timings.CsPresent = NBPtr->DCTPtr->Timings.CsPresent;
    NBPtr->MCTPtr->DctData[1].Timings.CsEnabled = NBPtr->DCTPtr->Timings.CsEnabled;
    NBPtr->MCTPtr->DctData[1].Timings.DctMemSize = NBPtr->DCTPtr->Timings.DctMemSize;
  } else {
    // In unganged mode, add DCT0 and DCT1 to NodeMemSize
    NBPtr->MCTPtr->NodeMemSize += NBPtr->DCTPtr->Timings.DctMemSize;
    NBPtr->MCTPtr->NodeSysLimit = NBPtr->MCTPtr->NodeMemSize - 1;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function swaps bits for OnDimmMirror support for Unb
 *
 *     Dimm Mirroring Requires that, during MRS command cycles, the following
 *     bits are swapped by software
 *
 *             A3 -> A4         A7 -> A8
 *             A4 -> A3         BA0 -> BA1
 *             A5 -> A6         BA1 -> BA0
 *             A6 -> A5
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSwapBitsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 ChipSel;
  UINT32 MRSBank;
  UINT32 MRSAddr;

  ChipSel = (UINT8) MemNGetBitFieldNb (NBPtr, BFMrsChipSel);
  if ((ChipSel & 1) != 0) {
    if ((NBPtr->DCTPtr->Timings.DimmMirrorPresent & (1 << (ChipSel >> 1))) != 0) {
      MRSBank = MemNGetBitFieldNb (NBPtr, BFMrsBank);
      MRSAddr = MemNGetBitFieldNb (NBPtr, BFMrsAddress);

      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tCS%d MR%d %05x swapped to ->",
                                                                    (ChipSel & 0x7),
                                                                    (MRSBank & 0x7),
                                                                    (MRSAddr & 0x3FFFF));
      //
      // Swap Mrs Bank bits 0 with 1
      MRSBank = (MRSBank & 0x0100) | ((MRSBank & 0x01) << 1) | ((MRSBank & 0x02) >> 1);
      //
      // Swap Mrs Address bits 3 with 4, 5 with 6, and 7 with 8
      MRSAddr = (MRSAddr & 0x03FE07) | ((MRSAddr&0x000A8) << 1) | ((MRSAddr&0x00150) >> 1);
      MemNSetBitFieldNb (NBPtr, BFMrsBank, MRSBank);
      MemNSetBitFieldNb (NBPtr, BFMrsAddress, MRSAddr);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   Programs Address/command timings, driver strengths, and tri-state fields.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNProgramPlatformSpecNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST UINT8           PinType[3]  = {PSO_CKE_TRI, PSO_ODT_TRI,   PSO_CS_TRI};
  CONST UINT8           TabSize[3]  = {          4,           4,            8};
  CONST BIT_FIELD_NAME  BitField[3] = {   BFCKETri,    BFODTTri, BFChipSelTri};
  UINT8  *TabPtr;
  UINT8  i;
  UINT8  k;
  UINT8  Value;
  //===================================================================
  // Tristate unused CKE, ODT and chip select to save power
  //===================================================================
  //
  TabPtr = NULL;
  for (k = 0; k < sizeof (PinType); k++) {
    if (NBPtr->IsSupported[CheckFindPSOverideWithSocket]) {
      TabPtr = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PinType[k], NBPtr->MCTPtr->SocketId, MemNGetSocketRelativeChannelNb (NBPtr, NBPtr->Dct, 0), 0,
                                    &(NBPtr->MCTPtr->LogicalCpuid), &(NBPtr->MemPtr->StdHeader));
    }
    if (NBPtr->IsSupported[CheckFindPSDct]) {
      TabPtr = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PinType[k], NBPtr->MCTPtr->SocketId, NBPtr->Dct, 0,
                                    &(NBPtr->MCTPtr->LogicalCpuid), &(NBPtr->MemPtr->StdHeader));
    }
    if (TabPtr == NULL) {
      switch (k) {
      case 0:
        TabPtr = NBPtr->ChannelPtr->CKETriMap;
        break;
      case 1:
        TabPtr = NBPtr->ChannelPtr->ODTTriMap;
        break;
      case 2:
        TabPtr = NBPtr->ChannelPtr->ChipSelTriMap;
        break;
      default:
        IDS_ERROR_TRAP;
      }
    }
    ASSERT (TabPtr != NULL);

    Value = 0;
    for (i = 0; i < TabSize[k]; i++) {
      if ((NBPtr->DCTPtr->Timings.CsPresent & TabPtr[i]) == 0) {
        Value |= (UINT8) (1 << i);
      }
    }

    if (k == PSO_CS_TRI) {
      NBPtr->FamilySpecificHook[BeforeSetCsTri] (NBPtr, &Value);
    }

    ASSERT (k < GET_SIZE_OF (BitField));
    MemNSetBitFieldNb (NBPtr, BitField[k], Value);
  }
  NBPtr->MemNBeforePlatformSpecNb (NBPtr);

  //===================================================================
  // Program Address/Command timings and driver strength
  //===================================================================
  //
  MemProcessConditionalOverrides (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr, PSO_ACTION_ADDRTMG, ALL_DIMMS);
  MemProcessConditionalOverrides (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr, PSO_ACTION_ODCCONTROL, ALL_DIMMS);

  MemNSetBitFieldNb (NBPtr, BFSlowAccessMode, (NBPtr->ChannelPtr->SlowMode) ? 1 : 0);
  NBPtr->FamilySpecificHook[ProgramPOdtOff] (NBPtr, &NBPtr->PsPtr->ProcessorOnDieTerminationOff);
  MemNSetBitFieldNb (NBPtr, BFODCControl, NBPtr->ChannelPtr->DctOdcCtl);
  MemNSetBitFieldNb (NBPtr, BFAddrTmgControl, NBPtr->ChannelPtr->DctAddrTmg);
  NBPtr->FamilySpecificHook[SetDqsODT] (NBPtr, NBPtr);

  if (NBPtr->IsSupported[CheckODTControls]) {
    MemNSetBitFieldNb (NBPtr, BFPhyRODTCSLow, NBPtr->ChannelPtr->PhyRODTCSLow);
    MemNSetBitFieldNb (NBPtr, BFPhyRODTCSHigh, NBPtr->ChannelPtr->PhyRODTCSHigh);
    MemNSetBitFieldNb (NBPtr, BFPhyWODTCSLow, NBPtr->ChannelPtr->PhyWODTCSLow);
    MemNSetBitFieldNb (NBPtr, BFPhyWODTCSHigh, NBPtr->ChannelPtr->PhyWODTCSHigh);
  }
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
MemNEnableSwapIntlvRgnNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Base,
  IN       UINT32 Limit
  )
{
  UINT32 Size;
  UINT32 SizeOfAlign;

  // Swapped interleaving region must be below 16G
  if (Limit < (1 << (34 - 27))) {
    // Adjust Base and Size to meet :
    // 1. The size of the swapped region must be less than or equal to the alignment of F2x10C[IntLvRegionBase].
    // 2. Entire UMA region is swapped with interleaving region.
    Size = Limit - Base;
    SizeOfAlign = (UINT32) 1 << LibAmdBitScanForward (Base);
    while (SizeOfAlign <= Size) {
      // In case of SizeOfAlign <= Size, UmaBase -= 128MB, SizeOfIntlvrgn += 128MB.
      Base -= 1;
      Size += 1;
      SizeOfAlign = (UINT32) 1 << LibAmdBitScanForward (Base);
    }
    MemNSetBitFieldNb (NBPtr, BFIntLvRgnBaseAddr, Base);
    MemNSetBitFieldNb (NBPtr, BFIntLvRgnLmtAddr, (Limit - 1));
    MemNSetBitFieldNb (NBPtr, BFIntLvRgnSize, Size);
    MemNSetBitFieldNb (NBPtr, BFIntLvRgnSwapEn, 1);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function converts MemClk frequency in MHz to MemClkFreq value
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Speed   - MemClk frequency in MHz
 *
 *     @return      MemClkFreq value
 */
UINT8
MemNGetMemClkFreqIdUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 Speed
  )
{
  return (UINT8) ((Speed > DDR400_FREQUENCY) ? ((Speed / 33) - 6) : ((Speed == DDR400_FREQUENCY) ? 2 : (Speed / 55)));
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function converts MemClkFreq Id value to MemClk frequency in MHz
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       FreqId   - FreqId from Register
 *
 *     @return          MemClk frequency in MHz
 */
UINT16
MemNGetMemClkFreqUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 FreqId
  )
{
  UINT16 MemClkFreq;
  if (FreqId > 2) {
    MemClkFreq = (FreqId == 14) ? 667 : (300 + ((FreqId - 3) * 33) + (FreqId - 3) / 3);
  } else if (FreqId == 2) {
    MemClkFreq = 200;
  } else {
    MemClkFreq = 50 + (50 * FreqId);
  }
  return MemClkFreq;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function change MemClk frequency to the value that is specified by DCTPtr->Timings.Speed
 *  for UNB.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNChangeFrequencyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;
  UINT8 Dct;
  UINT8 ChipSel;
  UINT16 FinalPllLockTime;
  BOOLEAN FrequencyChangeSuccess;
  UINT64  OrgMMIOCfgBase;
  UINT64  NewMMIOCfgBase;

  TechPtr = NBPtr->TechPtr;

  // Disable MMIO to prevent speculative DRAM reads during self refresh
  LibAmdMsrRead (MSR_MMIO_Cfg_Base, &OrgMMIOCfgBase, &(NBPtr->MemPtr->StdHeader));
  NewMMIOCfgBase = OrgMMIOCfgBase & (~(BIT0));
  LibAmdMsrWrite (MSR_MMIO_Cfg_Base, &NewMMIOCfgBase, &(NBPtr->MemPtr->StdHeader));

  MemNBrdcstSetNb (NBPtr, BFDisDllShutdownSR, 1);

  //Program F2x[1,0]90[EnterSelfRefresh]=1.
  //Wait until the hardware resets F2x[1,0]90[EnterSelfRefresh]=0.
  MemNBrdcstSetNb (NBPtr, BFEnterSelfRef, 1);
  MemNPollBitFieldNb (NBPtr, BFEnterSelfRef, 0, PCI_ACCESS_TIMEOUT, TRUE);

  if (NBPtr->ChangeNbFrequency (NBPtr)) {
    // Reprogram Twr, Tcwl, and Tcl based on the new MEMCLK frequency.
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      MemNSwitchDCTNb (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        TechPtr->AutoCycTiming (TechPtr);
        if (!MemNPlatformSpecUnb (NBPtr)) {
          IDS_ERROR_TRAP;
        }
      }
    }

    // 1. Program PllLockTime to Family-specific value
    MemNBrdcstSetNb (NBPtr, BFPllLockTime, NBPtr->FreqChangeParam->PllLockTimeDefault);

    // 2. Program D18F2x[1,0]94[MemClkFreqVal] = 0.
    MemNBrdcstSetNb (NBPtr, BFMemClkFreqVal, 0);

    // 3. Program D18F2x[1,0]94[MemClkFreq] to the desired DRAM frequency.
    MemNBrdcstSetNb (NBPtr, BFMemClkFreq, NBPtr->GetMemClkFreqId (NBPtr, NBPtr->DCTPtr->Timings.Speed));

    // 4. Program D18F2x[1,0]F4_x30[DbeGskFifoNumerator] and D18F2x[1,0]F4_x31[DbeGskFifoDenominator].
    // 5. Program D18F2x[1,0]F4_x32[DataTxFifoSchedDlyNegSlot1, DataTxFifoSchedDlySlot1,
    // DataTxFifoSchedDlyNegSlot0, DataTxFifoSchedDlySlot0]. See 2.10.3.2.2.1 [DCT Transmit Fifo Schedule
    // Delay Programming].
    // 6. D18F2x[1,0]78[RdPtrInit] = IF (D18F2x[1,0]94[MemClkFreq] >= 667 MHz) THEN 7 ELSE 8 ENDIF (Llano)
    //                                                                         THEN 2 ELSE 3 ENDIF (Ontario)
    NBPtr->ProgramNbPsDependentRegs (NBPtr);

    IDS_OPTION_HOOK (IDS_BEFORE_MEM_FREQ_CHG, NBPtr, &(NBPtr->MemPtr->StdHeader));
    // 7. Program D18F2x[1,0]94[MemClkFreqVal] = 1.
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      MemNSwitchDCTNb (NBPtr, Dct);
      if ((NBPtr->DCTPtr->Timings.DctMemSize != 0)) {
        MemNSetBitFieldNb (NBPtr, BFMemClkFreqVal, 1);
        MemNPollBitFieldNb (NBPtr, BFFreqChgInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
      }
    }
    FinalPllLockTime = 0xF;
    NBPtr->FamilySpecificHook[AfterMemClkFreqVal] (NBPtr, &FinalPllLockTime);

    // 8. IF (D18F2x[1,0]9C_x0D0F_E00A[CsrPhySrPllPdMode]==0) THEN program
    // D18F2x[1,0]9C_x0D0F_E006[PllLockTime] = 0Fh.
    if (!NBPtr->IsSupported[CsrPhyPllPdEn]) {
      MemNBrdcstSetNb (NBPtr, BFPllLockTime, FinalPllLockTime);
    }

    FrequencyChangeSuccess = TRUE;
  } else {
    // If NB frequency cannot be updated, use the current speed as the target speed
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      MemNSwitchDCTNb (NBPtr, Dct);
      NBPtr->DCTPtr->Timings.Speed = NBPtr->TechPtr->PrevSpeed;
      NBPtr->DCTPtr->Timings.TargetSpeed = NBPtr->TechPtr->PrevSpeed;
    }
    FrequencyChangeSuccess = FALSE;
  }

  if (FrequencyChangeSuccess) {
    // Perform Phy Fence training and Phy comp init after frequency change
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      MemNSwitchDCTNb (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);

        // Phy fence programming
        AGESA_TESTPOINT (TpProcMemPhyFenceTraining, &(NBPtr->MemPtr->StdHeader));
        NBPtr->PhyFenceTraining (NBPtr);

        // Phy compensation initialization
        AGESA_TESTPOINT (TPProcMemPhyCompensation, &(NBPtr->MemPtr->StdHeader));
        NBPtr->MemNInitPhyComp (NBPtr);
        MemProcessConditionalOverrides (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr, PSO_ACTION_SLEWRATE, ALL_DIMMS);
        // Hardware programs POdtOff for the current M-state (D18F2x9C_x0D0F_E006_dct[0][PhyPS]) with the value in D18F2x94_dct[0]
        // [ProcOdtDis] (via D18F2x9C_x0000_000B_dct[0][ProcOdtDis]) when the memory frequency is updated. BIOS must reprogram this
        // BIOS must reprogram this field after each frequency change if the target value differs from D18F2x94_dct[0][ProcOdtDis].
        NBPtr->FamilySpecificHook[ProgramPOdtOff] (NBPtr, &NBPtr->PsPtr->ProcessorOnDieTerminationOff);
      }
    }
  }

  //Program F2x[1,0]90[ExitSelfRef]=1 for both DCTs.
  //Wait until the hardware resets F2x[1, 0]90[ExitSelfRef]=0.
  MemNBrdcstSetNb (NBPtr, BFExitSelfRef, 1);
  MemNPollBitFieldNb (NBPtr, BFExitSelfRef, 0, PCI_ACCESS_TIMEOUT, TRUE);
  if (NBPtr->IsSupported[SetDllShutDown]) {
    MemNBrdcstSetNb (NBPtr, BFDisDllShutdownSR, 0);
  }

  if (FrequencyChangeSuccess) {
    NBPtr->FamilySpecificHook[AfterMemClkFreqChg] (NBPtr, NULL);

    //======================================================================
    // Calculate and program DRAM Timings at new frequency
    //======================================================================
    //
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
      MemNSwitchDCTNb (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel++) {
          if ((NBPtr->DCTPtr->Timings.CsPresent & ((UINT16)1 << ChipSel)) != 0) {
            // if chip select present
            if (!(TechPtr->TechnologySpecificHook[LrdimmSendAllMRCmds] (TechPtr, &ChipSel))) {
              TechPtr->SendAllMRCmds (TechPtr, ChipSel);
            }
          }
        }
        // Wait 512 clocks for DLL-relock
        MemNWaitXMemClksNb (NBPtr, 512);
      }
    }
  }

  // Restore MMIO setting
  LibAmdMsrWrite (MSR_MMIO_Cfg_Base, &OrgMMIOCfgBase, &(NBPtr->MemPtr->StdHeader));

  MemFInitTableDrive (NBPtr, MTAfterFreqChg);
}

/**
 *
 *
 *   This function overrides the ASR and SRT value in MRS command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNSetASRSRTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 MrsAddress;
  UINT8 Dimm;
  UINT8 *SpdBufferPtr;
  BOOLEAN ASREn;
  BOOLEAN SRTEn;

  // Look for MR2
  if (NBPtr->GetBitField (NBPtr, BFMrsBank) == 2) {
    MrsAddress = NBPtr->GetBitField (NBPtr, BFMrsAddress);
    // Clear A6(ASR) and A7(SRT)
    MrsAddress &= (UINT32) ~0xC0;
    if ((NBPtr->ChannelPtr->RegDimmPresent) || (NBPtr->ChannelPtr->LrDimmPresent)) {
      // For registered dimm and LR dimm, MRS command is sent to all chipselects.
      // So different ASR/SRT setting can be sent to each chip select.
      Dimm = (UINT8) (NBPtr->GetBitField (NBPtr, BFMrsChipSel) >> 1);
      // Make sure we access SPD of the second logical dimm of QR dimm correctly
      if ((Dimm >= 2) && ((NBPtr->ChannelPtr->DimmQrPresent & (UINT8) (1 << Dimm)) != 0)) {
        Dimm -= 2;
      }
      if (NBPtr->TechPtr->GetDimmSpdBuffer (NBPtr->TechPtr, &SpdBufferPtr, Dimm)) {
        // Bit 2 is ASR
        if (SpdBufferPtr[THERMAL_OPT] & 0x4) {
          // when ASR is 1, set SRT to 0
          MrsAddress |= 0x40;
        } else {
          // Set SRT based on bit on of thermal byte
          MrsAddress |= ((SpdBufferPtr[THERMAL_OPT] & 1) << 7);
        }
      }
    } else {
      // Udimm and unbuffered dimm, MSR command will be broadcasted during Dram Init.
      // ASR/SRT value needs to be leveled across the DCT. Only if all dimms on the DCT
      // support ASR or SRT can ASR or SRT be enabled.
      ASREn = TRUE;
      SRTEn = TRUE;
      for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm ++) {
        if (NBPtr->TechPtr->GetDimmSpdBuffer (NBPtr->TechPtr, &SpdBufferPtr, Dimm)) {
          // Bit 2 is ASR
          if ((SpdBufferPtr[THERMAL_OPT] & 0x4) == 0) {
            // when any dimm in the DCT does not support ASR, disable ASR for the DCT
            ASREn = FALSE;
            // When any dimm does not have SRT with a value of 1, set SRT to 0 for the DCT
            if ((SpdBufferPtr[THERMAL_OPT] & 1) == 0) {
              SRTEn = FALSE;
            }
          }
        }
      }
      if (ASREn) {
        MrsAddress |= 0x40;
      } else {
        MrsAddress |= (UINT8) SRTEn << 7;
      }
    }

    NBPtr->SetBitField (NBPtr, BFMrsAddress, MrsAddress);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function changes NB frequency foras below:
 *     NBP0-DDR800 -> NBP0-DDR1066 -> ... -> NBP0-DDRTarget -> NBP1-DDRTarget -> NBP2-DDRTarget -> NBP3-DDRTarget -> NBP0-DDRTarget
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
MemNChangeNbFrequencyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  BOOLEAN Status;

  Status = FALSE;

  // State machine to change NB frequency and NB Pstate
  switch (NBPtr->NbFreqChgState) {
  case 0:
    // Do not change NB Pstate, just to save initial NB Pstate value
    Status = NBPtr->ChangeNbFrequencyWrap (NBPtr, 0);
    if (NBPtr->DCTPtr->Timings.Speed == NBPtr->DCTPtr->Timings.TargetSpeed) {
      // When MemClk has been ramped up to its max, transition to next state, which changes NBPstate to P1
      NBPtr->NbFreqChgState = 1;
      IDS_OPTION_HOOK (IDS_NB_PSTATE_DIDVID, NBPtr, &(NBPtr->MemPtr->StdHeader));
    }
    break;

  case 1:
  case 2:
  case 3:
    // Change NB P-State to NBP1 for MaxRdLat training
    if (NBPtr->ChangeNbFrequencyWrap (NBPtr, NBPtr->NbFreqChgState)) {
      // Next state is to try all NBPstates
      NBPtr->NbFreqChgState++;

      // Return TRUE to repeat MaxRdLat training
      Status = TRUE;
    } else {
      // If transition to any NBPs fails, transition to exit state machine
      NBPtr->NbFreqChgState = 4;
    }
    break;

  case 4:
    // Change NB P-State back to NBP0
    Status = NBPtr->ChangeNbFrequencyWrap (NBPtr, 0);
    ASSERT (Status);

    // Return FALSE to get out of MaxRdLat training loop
    Status = FALSE;

    // Exit state machine
    NBPtr->NbFreqChgState = 5;
    break;

  default:
    break;
  }

  return Status;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets "Dram Term" value from data structure for Unb
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       ChipSel  - Targeted chipsel
 *
 *     @return       Dram Term value
 */
UINT8
MemNGetDramTermTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSel
  )
{
  UINT8 RttNom;
  RttNom = NBPtr->PsPtr->RttNom[ChipSel];
  IDS_OPTION_HOOK (IDS_MEM_DRAM_TERM, &RttNom, &NBPtr->MemPtr->StdHeader);
  return RttNom;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets "Dynamic Dram Term" value from data structure
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       ChipSel  - Targeted chipsel
 *
 *     @return       Dynamic Dram Term value
 */
UINT8
MemNGetDynDramTermTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSel
  )
{
  UINT8 RttWr;
  RttWr = NBPtr->PsPtr->RttWr[ChipSel];
  IDS_OPTION_HOOK (IDS_MEM_DYN_DRAM_TERM, &RttWr, &NBPtr->MemPtr->StdHeader);
  return RttWr;
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
MemNGetMR0WRTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return (UINT32) (((NBPtr->PsPtr->MR0WR & 0x7) << 9) | ((NBPtr->PsPtr->MR0WR & 0x8) << (13 - 3)));
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function returns MR2[CWL] value for UNB
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return       MR0[CWL] value
 */
UINT32
MemNGetMR2CWLUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 Value32;

  Value32 = (MemNGetBitFieldNb (NBPtr, BFTcwl) - 5) << 3;

  return Value32;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sets Txp and Txpdll
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return       none
 */
VOID
MemNSetTxpNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST UINT8 Txp[] = {0xFF, 0xFF, 3, 3, 4, 4, 5, 6, 7};
  CONST UINT8 Txpdll[] = {0xFF, 0xFF, 0xA, 0xA, 0xD, 0x10, 0x14, 0x17, 0x1A};
  UINT8 i;
  UINT8 TxpVal;
  UINT8 TxpdllVal;
  UINT16 Speed;

  Speed = NBPtr->DCTPtr->Timings.Speed;
  i = (UINT8) ((Speed < DDR800_FREQUENCY) ? ((Speed / 66) - 3) : (Speed / 133));
  ASSERT (i < sizeof (Txp));
  ASSERT (i < sizeof (Txpdll));

  TxpdllVal = Txpdll[i];

  if ((NBPtr->MCTPtr->Status[SbLrdimms] || NBPtr->MCTPtr->Status[SbRegistered]) &&
      ((NBPtr->DCTPtr->Timings.Speed == DDR667_FREQUENCY) || (NBPtr->DCTPtr->Timings.Speed == DDR800_FREQUENCY)) &&
      (NBPtr->RefPtr->DDR3Voltage == VOLT1_25)) {
    TxpVal = 4;
  } else {
    TxpVal = Txp[i];
  }

  if (TxpVal != 0xFF) {
    MemNSetBitFieldNb (NBPtr, BFTxp, TxpVal);
  }
  if (TxpdllVal != 0xFF) {
    NBPtr->FamilySpecificHook[AdjustTxpdll] (NBPtr, &TxpdllVal);
    MemNSetBitFieldNb (NBPtr, BFTxpdll, TxpdllVal);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function is a wrapper to handle or switch NB Pstate for UNB
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]  *NBPstate - NB Pstate
 *
 *     @return          TRUE - Succeed
 *     @return          FALSE - Fail
 */

BOOLEAN
MemNChangeNbFrequencyWrapUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 NBPstate
  )
{
  BOOLEAN SkipTransToLo;
  UINT8   TargetNbPs;
  UINT32  FreqNumeratorInMHz;
  UINT32  FreqDivisor;
  UINT32  VoltageInuV;
  UINT8   NbPstateMaxVal;
  UINT64  MsrValue;
  UINT64  PerfCtrlSave;
  UINT64  PerfStsSave;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  SkipTransToLo = FALSE;
  if (NBPtr->NbFreqChgState == 0) {
    // While in state 0, keep NB Pstate at the highest supported
    TargetNbPs = 0;
    if (NBPtr->NbPsCtlReg == 0) {
      // Save NbPsCtl register on the first run
      NBPtr->NbPsCtlReg = MemNGetBitFieldNb (NBPtr, BFNbPstateCtlReg);
    } else {
      // Do not need to switch NB Pstate again if it is already at highest
      return TRUE;
    }

    // Find out if Current NBPstate is NBPstateLo or not
    // If yes, skip the steps that transition the Pstate to Lo
    LibAmdMsrRead (MSR_NB_PERF_CTL3, &PerfCtrlSave, &(NBPtr->MemPtr->StdHeader));
    MsrValue = 0x00000006004004E9;
    LibAmdMsrRead (MSR_NB_PERF_CTR3, &PerfStsSave, &(NBPtr->MemPtr->StdHeader));
    LibAmdMsrWrite (MSR_NB_PERF_CTL3, &MsrValue, &(NBPtr->MemPtr->StdHeader));
    MsrValue = 0;
    LibAmdMsrWrite (MSR_NB_PERF_CTR3, &MsrValue, &(NBPtr->MemPtr->StdHeader));
    LibAmdMsrRead (MSR_NB_PERF_CTR3, &MsrValue, &(NBPtr->MemPtr->StdHeader));
    if (MsrValue != 0) {
      SkipTransToLo = TRUE;
    }
    LibAmdMsrWrite (MSR_NB_PERF_CTL3, &PerfCtrlSave, &(NBPtr->MemPtr->StdHeader));
    LibAmdMsrWrite (MSR_NB_PERF_CTR3, &PerfStsSave, &(NBPtr->MemPtr->StdHeader));
  } else if (NBPtr->NbFreqChgState < 4) {
    // While in other states, go to the next lower NB Pstate
    TargetNbPs = (UINT8) MemNGetBitFieldNb (NBPtr, BFCurNbPstate) + 1;
    if (TargetNbPs == 1) {
      // Set up intermediate NBPstate
      NbPstateMaxVal = (UINT8) MemNGetBitFieldNb (NBPtr, BFNbPstateMaxVal);
      MemNSetBitFieldNb (NBPtr, BFNbPsSel, NbPstateMaxVal);
      GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, &NBPtr->MemPtr->StdHeader);
      if (FamilySpecificServices->GetNbPstateInfo (FamilySpecificServices,
                                                  NBPtr->MemPtr->PlatFormConfig,
                                                  &NBPtr->PciAddr,
                                                  (UINT32) NbPstateMaxVal,
                                                  &FreqNumeratorInMHz,
                                                  &FreqDivisor,
                                                  &VoltageInuV,
                                                  &(NBPtr->MemPtr->StdHeader))) {
        // Get NCLK speed for intermediate NBPstate
        NBPtr->NBClkFreq = FreqNumeratorInMHz / FreqDivisor;
        NBPtr->ProgramNbPsDependentRegs (NBPtr);
      } else {
        ASSERT (FALSE);
      }
    }
  } else {
    // When done with training, release NB Pstate force by restoring NbPsCtl register
    NBPtr->FamilySpecificHook[ReleaseNbPstate] (NBPtr, NBPtr);
    IDS_HDT_CONSOLE (MEM_FLOW, "\tRelease NB Pstate force\n");
    return TRUE;
  }

  // Make sure target NB Pstate is enabled, else find next enabled NB Pstate
  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, &NBPtr->MemPtr->StdHeader);
  for (; TargetNbPs < 4; TargetNbPs++) {
    if (FamilySpecificServices->GetNbPstateInfo (FamilySpecificServices,
                                                  NBPtr->MemPtr->PlatFormConfig,
                                                  &NBPtr->PciAddr,
                                                  (UINT32) TargetNbPs,
                                                  &FreqNumeratorInMHz,
                                                  &FreqDivisor,
                                                  &VoltageInuV,
                                                  &(NBPtr->MemPtr->StdHeader))) {
      // Record NCLK speed
      NBPtr->NBClkFreq = FreqNumeratorInMHz / FreqDivisor;
      break;
    }
  }

  if (TargetNbPs < 4) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\tNB P%d: %dMHz\n", TargetNbPs, NBPtr->NBClkFreq);

    // 1.Program the configuration registers which contain multiple internal copies for each NB P-state. See
    //   D18F1x10C[NbPsSel].
    MemNSetBitFieldNb (NBPtr, BFNbPsSel, TargetNbPs);

    /// Check to see if NB P-states have been disabled.  @todo This should only be needed for
    /// bring up, but must be included in any releases that occur before NB P-state operation
    /// has been debugged/fixed.
    if ((NBPtr->NbPsCtlReg & 0x00000003) != 0) {
      NbPstateMaxVal = (UINT8) MemNGetBitFieldNb (NBPtr, BFNbPstateMaxVal);
      // Set up RdPtrInit before transit to target NBPstate
      if ((TargetNbPs > 0) && (TargetNbPs != NbPstateMaxVal))  {
        NBPtr->ProgramNbPsDependentRegs (NBPtr);
      } else {
        NBPtr->GetMemClkFreqInCurrentContext (NBPtr);
      }

      // If current NBPstate is already in NBPstateLo, do not do transition to NBPstateLo.
      if ((TargetNbPs != 0) || !SkipTransToLo) {
        // 2.Program D18F5x170 to transition the NB P-state:
        //   NbPstateLo = NbPstateMaxVal. (HW requires an intermediate transition to low)
        //   SwNbPstateLoDis = NbPstateDisOnP0 = NbPstateThreshold = 0.
        MemNSetBitFieldNb (NBPtr, BFNbPstateLo, NbPstateMaxVal);
        MemNSetBitFieldNb (NBPtr, BFNbPstateCtlReg, MemNGetBitFieldNb (NBPtr, BFNbPstateCtlReg) & 0xFFFF91FF);

        // 3.Wait for D18F5x174[CurNbPstate] to equal NbPstateLo.
        MemNPollBitFieldNb (NBPtr, BFCurNbPstate, NbPstateMaxVal, PCI_ACCESS_TIMEOUT, FALSE);
      }
      // 4.Program D18F5x170 to force the NB P-state:
      //   NbPstateHi = target NB P-state.
      //   SwNbPstateLoDis = 1 (triggers the transition)
      MemNSetBitFieldNb (NBPtr, BFNbPstateHi, TargetNbPs);
      MemNSetBitFieldNb (NBPtr, BFSwNbPstateLoDis, 1);

      // 5.Wait for D18F5x174[CurNbPstate] to equal the target NB P-state.
      MemNPollBitFieldNb (NBPtr, BFCurNbPstate, TargetNbPs, PCI_ACCESS_TIMEOUT, FALSE);
    }

    // When NB frequency change succeeds, TSC rate may have changed.
    // We need to update TSC rate
    FamilySpecificServices->GetTscRate (FamilySpecificServices, &NBPtr->MemPtr->TscRate, &NBPtr->MemPtr->StdHeader);
    // Switch MemPstate context if the current MemPstate does not sync with MemPstate context
    if (MemNGetBitFieldNb (NBPtr, BFCurMemPstate) != MemNGetBitFieldNb (NBPtr, BFMemPsSel)) {
      MemNChangeMemPStateContextNb (NBPtr, MemNGetBitFieldNb (NBPtr, BFCurMemPstate));
    }
  } else {
    // Cannot find a supported NB Pstate to switch to
    // Release NB Pstate force by restoring NbPsCtl register
    NBPtr->FamilySpecificHook[ReleaseNbPstate] (NBPtr, NBPtr);
    IDS_HDT_CONSOLE (MEM_FLOW, "\tRelease NB Pstate force\n");
    return FALSE;
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sends an MRS command for Unb
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSendMrsCmdUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 MrsBank;
  UINT16 MrsBuffer;
  UINT8 MrsChipSel;

  MemNSetASRSRTNb (NBPtr);
  MemNSwapBitsUnb (NBPtr);

  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tCS%d MR%d %05x\n",
              (MemNGetBitFieldNb (NBPtr, BFMrsChipSel) & 0x7),
              (MemNGetBitFieldNb (NBPtr, BFMrsBank) & 0x7),
              (MemNGetBitFieldNb (NBPtr, BFMrsAddress) & 0x3FFFF));

  // 1.Set SendMrsCmd=1
  MemNSetBitFieldNb (NBPtr, BFSendMrsCmd, 1);

  // 2.Wait for SendMrsCmd=0
  MemNPollBitFieldNb (NBPtr, BFSendMrsCmd, 0, PCI_ACCESS_TIMEOUT, FALSE);

  // Send MRS buffer if memory pstate is supported and enabled
  if (NBPtr->MemPstateStage != 0) {
    MrsChipSel = (UINT8) (MemNGetBitFieldNb (NBPtr, BFMrsChipSel) & 0x7);
    // Only user even rank MRS to set MRS buffer
    if ((MrsChipSel & 1) == 0) {
      MrsBank = (UINT8) (MemNGetBitFieldNb (NBPtr, BFMrsBank) & 0x7);
      MrsBuffer = (UINT16) (MemNGetBitFieldNb (NBPtr, BFMrsAddress) & 0xFFFF);
      if (MrsBank == 0) {
        MrsBuffer &= 0xFEFF;
        MemNSetBitFieldNb (NBPtr, BFMxMr0, MrsBuffer);
      } else if (MrsBank == 1) {
        MemNSetBitFieldNb (NBPtr, BFMxMr1, MrsBuffer);
      } else if (MrsBank == 2) {
        MemNSetBitFieldNb (NBPtr, BFMxMr2, MrsBuffer);
      }
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function returns MR0[CL] value with table driven support
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return       MR0[CL] value
 */
UINT32
MemNGetMR0CLTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return (UINT32) ((NBPtr->PsPtr->MR0CL31 << 4) | (NBPtr->PsPtr->MR0CL0 << 2));
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function programs dram power management timing related registers
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *
 *     @return    none
 * ----------------------------------------------------------------------------
 */
VOID
MemNDramPowerMngTimingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  STATIC CONST UINT8 Tckesr[] = {4, 4, 4, 5, 5, 6, 7, 2, 2};
  UINT8 Tck;

  // These timings are based on DDR3 spec
  // Tcksrx = max(5 nCK, 10 ns)
  Tck = (UINT8) MAX (5, (MemUnsToMemClk (NBPtr->DCTPtr->Timings.Speed, 10)));
  MemNSetBitFieldNb (NBPtr, BFTcksrx, MIN (0xE, MAX (Tck, 2)));

  // Tcksre = max(5 nCK, 10 ns)
  MemNSetBitFieldNb (NBPtr, BFTcksre, MIN (0x27, MAX (Tck, 5)));

  // Tckesr = tCKE(min) + 1 nCK
  //                               tCKE(min)
  // DDR-667   7.5ns = 3nCk     max(3nCK, 7.5ns)   + 1 = 3nCK + 1nCK = 4nCK
  // DDR-800   7.5ns = 3nCk     max(3nCK, 7.5ns)   + 1 = 3nCK + 1nCK = 4nCK
  // DDR-1066  5.625ns = 3nCK   max(3nCK, 5.625ns) + 1 = 3nCL + 1nCK = 4nCK
  // DDR-1333  5.625ns = 4nCK   max(3nCK, 4nCK)    + 1 = 4nCK + 1nCK = 5nCK
  // DDR-1600  5ns = 4nCK       max(3nCK, 4nCK)    + 1 = 4nCK + 1nCK = 5nCK
  // DDR-1866  5ns = 5nCK       max(3nCK, 5nCK)    + 1 = 5nCK + 1nCK = 6nCK
  // DDR-2133  5ns = 6nCK       max(3nCK, 6nCK)    + 1 = 6nCK + 1nCK = 7nCK
  ASSERT (((NBPtr->DCTPtr->Timings.Speed / 133) >= 2) && ((NBPtr->DCTPtr->Timings.Speed / 133) <= 10));
  MemNSetBitFieldNb (NBPtr, BFTckesr, Tckesr[(NBPtr->DCTPtr->Timings.Speed / 133) - 2]);

  // Tpd = tCKE(min)
  MemNSetBitFieldNb (NBPtr, BFTpd, Tckesr[(NBPtr->DCTPtr->Timings.Speed / 133) - 2] - 1);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       The function resets Rcv Fifo
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Dummy - Dummy parameter
 *
 */

VOID
MemTResetRcvFifoUnb (
  IN OUT   struct _MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dummy
  )
{
  // Program D18F2x9C_x0000_0050_dct[1:0]=00000000h
  MemNSetBitFieldNb (TechPtr->NBPtr, BFRstRcvFifo, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets the memory width
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      Memory width
 */

UINT32
MemNGetMemoryWidthUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  DIE_STRUCT      *MCTPtr;
  MEM_SHARED_DATA *SharedPtr;

  MCTPtr    = NBPtr->MCTPtr;
  SharedPtr = NBPtr->SharedPtr;

  return 64 + ((SharedPtr->AllECC && MCTPtr->Status[SbEccDimms]) ? 8 : 0);
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function displays AMP voltage values if applicable
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    FALSE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNAmpVoltageDispUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  IDS_HDT_CONSOLE (MEM_FLOW, "\nAMP VDDIO = 1.%dV\n", (NBPtr->RefPtr->AmpVoltage >= AMP_VOLT1_45) ? 50 - 5 * (NBPtr->RefPtr->AmpVoltage >> 4) :
                                                      45 + 5 * NBPtr->RefPtr->AmpVoltage);

  // FALSE return to skip normal voltage display if wanted
  return FALSE;
}

