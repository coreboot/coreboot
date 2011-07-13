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
 * @e \$Revision: 11758 $ @e \$Date: 2009-04-03 10:58:22 -0500 (Fri, 03 Apr 2009) $
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
#include "mftds.h"
#include "merrhdl.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
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

UINT8
MemNGet1KTFawTkNb (
  IN       UINT8 k
  );

UINT8
MemNGet2KTFawTkNb (
  IN       UINT8 k
  );

VOID
STATIC
MemNQuarterMemClk2NClkNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   UINT16 *SubTotalPtr
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
MemNPlatformSpecNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST BIT_FIELD_NAME ChipletPDRegs[] = {
  BFPhyClkConfig0,
  BFPhyClkConfig3,
  BFPhyClkConfig1,
  BFPhyClkConfig2
  };
  CONST UINT8 ChipletPDClkDisMap[][2] = {
  //F2[1, 0]x9C_x0D0F2030 -> F2x[1, 0]88[MemClkDis[1:0]]
  {0, 1},
  //F2[1, 0]x9C_x0D0F2330 -> F2x[1, 0]88[MemClkDis[7:6]]
  {6, 7},
  //F2x09C_x0D0F2130 -> F2x88[MemClkDis[5:4]]
  {4, 5},
  //F2x09C_x0D0F2230 -> F2x88[MemClkDis[3:2]]
  {2, 3},
  //F2x19C_x0D0F2130 -> F2x188[MemClkDis[5:2]]
  {2, 5},
  //F2x19C_x0D0F2230 -> F2x188[MemClkDis[4:3]]
  {3, 4}
  };

  UINT8 MemClkDis;
  UINT8 i;
  UINT8 MemoryAllClocks;
  UINT8 *MemClkDisMap;
  UINT16 CsPresent;
  UINT8 RegIndex;
  UINT8 Cs1;
  UINT8 Cs2;

  if (!MemNGetPlatformCfgNb (NBPtr)) {
    IDS_ERROR_TRAP;
  }

  MemNProgramPlatformSpecNb (NBPtr);
  if (!NBPtr->PsPtr->MemPDoPs (NBPtr)) {
    IDS_ERROR_TRAP;
  }

  MemProcessConditionalOverrides (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr, PSO_ACTION_ODT, ALL_DIMMS);

  if (NBPtr->MCTPtr->GangedMode) {
    MemNSwitchDCTNb (NBPtr, 1);
    if (!MemNGetPlatformCfgNb (NBPtr)) {
      IDS_ERROR_TRAP;
    }
    MemNProgramPlatformSpecNb (NBPtr);
    MemNSwitchDCTNb (NBPtr, 0);
  }

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
      MemClkDisMap = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MEMCLK_DIS, NBPtr->MCTPtr->SocketId, MemNGetSocketRelativeChannelNb (NBPtr, NBPtr->Dct, 0));
      if (MemClkDisMap == NULL) {
        MemClkDisMap = NBPtr->ChannelPtr->MemClkDisMap;
      }

      // Turn off the unused CS clocks
      CsPresent = NBPtr->DCTPtr->Timings.CsPresent;

      if (NBPtr->IsSupported[CheckMemClkCSPresent]) {
        if (NBPtr->ChannelPtr->RegDimmPresent != 0) {
          // All DDR3 RDIMM use only one MEMCLOCK from edge finger to the register
          // regardless of how many Ranks are on the DIMM (Single, Dual or Quad)
          CsPresent = (CsPresent | (CsPresent >> 1)) & 0x5555;
        }
      }
      for (i = 0; i < 8; i++) {
        if ((CsPresent & MemClkDisMap[i]) == 0) {
          MemClkDis |= (UINT8) (1 << i);
        }
      }
      //Chiplet power down
      for (RegIndex = 0; RegIndex < GET_SIZE_OF (ChipletPDRegs); RegIndex++) {
        if ((NBPtr->Dct == 1) && (RegIndex >= 2)) {
          Cs1 =  MemClkDisMap[ChipletPDClkDisMap[RegIndex + 2][0]];
          Cs2 =  MemClkDisMap[ChipletPDClkDisMap[RegIndex + 2][1]];
        } else {
          Cs1 =  MemClkDisMap[ChipletPDClkDisMap[RegIndex][0]];
          Cs2 =  MemClkDisMap[ChipletPDClkDisMap[RegIndex][1]];
        }
        if ((CsPresent & (UINT16) (Cs1 | Cs2)) == 0) {
          MemNSetBitFieldNb (NBPtr, ChipletPDRegs[RegIndex], (MemNGetBitFieldNb (NBPtr, ChipletPDRegs[RegIndex]) | 0x10));
        }
      }
    }
  }
  MemNSetBitFieldNb (NBPtr, BFMemClkDis, MemClkDis);

  AGESA_TESTPOINT (TPProcMemPhyCompensation, &(NBPtr->MemPtr->StdHeader));
  NBPtr->MemNInitPhyComp (NBPtr);

  MemProcessConditionalOverrides (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr, PSO_ACTION_SLEWRATE, ALL_DIMMS);

  if (NBPtr->IsSupported[CheckDynamicDramTerm]) {
    // Program DramTerm for DDR2
    if ((MemNGetBitFieldNb (NBPtr, BFDdr3Mode)) == 0) {
      MemNSetBitFieldNb (NBPtr, BFDramTerm, NBPtr->PsPtr->DramTerm);
    } else {
      // Dynamic Dynamic DramTerm for DDR3
      // Dram Term for DDR3 may vary based on chip selects
      MemNSetBitFieldNb (NBPtr, BFDramTermDyn, NBPtr->PsPtr->DynamicDramTerm);
    }
  }
  // Program DramTerm for DDR2
  if ((MemNGetBitFieldNb (NBPtr, BFDdr3Mode)) == 0) {
    MemNSetBitFieldNb (NBPtr, BFDramTerm, NBPtr->PsPtr->DramTerm);
  } else {
    // Dynamic Dynamic DramTerm for DDR3
    // Dram Term for DDR3 may vary based on chip selects
    MemNSetBitFieldNb (NBPtr, BFDramTermDyn, NBPtr->PsPtr->DynamicDramTerm);
  }

  MemFInitTableDrive (NBPtr, MTAfterPlatformSpec);

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
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
MemNPlatformSpecClientNb (
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

  MemNProgramPlatformSpecNb (NBPtr);
  if (!NBPtr->PsPtr->MemPDoPs (NBPtr)) {
    IDS_ERROR_TRAP;
  }

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
      MemClkDisMap = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MEMCLK_DIS, NBPtr->MCTPtr->SocketId, NBPtr->Dct);
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
      if ((MemClkDis & 0x3) == 0x3) {
        MemNSetBitFieldNb (NBPtr, BFPhyClkConfig0, 0x0010);
      }
      if ((MemClkDis & 0xC) == 0xC) {
        MemNSetBitFieldNb (NBPtr, BFPhyClkConfig1, 0x0010);
      }
    }
  }
  MemNSetBitFieldNb (NBPtr, BFMemClkDis, MemClkDis);
  MemFInitTableDrive (NBPtr, MTAfterPlatformSpec);

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function disables the DCT and mem clock
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNDisableDCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemNSetBitFieldNb (NBPtr, BFCKETri, 0x03);
  MemNSetBitFieldNb (NBPtr, BFODTTri, 0x0F);
  MemNSetBitFieldNb (NBPtr, BFChipSelTri, 0xFF);

  // To maximize power savings when DisDramInterface=1b,
  // all of the MemClkDis bits should also be set.
  //
  MemNSetBitFieldNb (NBPtr, BFMemClkDis, 0xFF);

  MemNSetBitFieldNb (NBPtr, BFDisDramInterface, 1);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function disables the DCT and mem clock for client NB
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNDisableDCTClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemNSetBitFieldNb (NBPtr, BFCKETri, 0x03);
  MemNSetBitFieldNb (NBPtr, BFODTTri, 0x0F);
  MemNSetBitFieldNb (NBPtr, BFChipSelTri, 0xFF);

  //Wait for 24 MEMCLKs
  MemUWait10ns (6, NBPtr->MemPtr);

  // To maximize power savings when DisDramInterface=1b,
  // all of the MemClkDis bits should also be set.
  //
  MemNSetBitFieldNb (NBPtr, BFMemClkDis, 0xFF);

  MemNSetBitFieldNb (NBPtr, BFDramPhyStatusReg, 0x80800000);

  MemNSetBitFieldNb (NBPtr, BFDisDramInterface, 1);
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
MemNStartupDCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  // 1. Ensure F2x[1, 0]9C_x08[DisAutoComp] = 1.
  // 2. BIOS waits 5 us for the disabling of the compensation engine to complete.
  // DisAutoComp is still being set since InitPhyComp

  if (NBPtr->MCTPtr->NodeMemSize != 0) {
    // Init MemClk frequency
    MemNBrdcstSetNb (NBPtr, BFMemClkFreqVal, 1);


    AGESA_TESTPOINT (TpProcMemBeforeDramInit, &(NBPtr->MemPtr->StdHeader));
    NBPtr->MemNBeforeDramInitNb (NBPtr);

    AGESA_TESTPOINT (TpProcMemDramInit, &(NBPtr->MemPtr->StdHeader));
    NBPtr->FeatPtr->DramInit (NBPtr->TechPtr);
  }

  // 7. Program F2x[1, 0]9C_x08[DisAutoComp] = 0.
  // 8. BIOS must wait 750 us for the phy compensation engine
  //    to reinitialize.
  // DisAutoComp will be cleared after DramEnabled turns to 1

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function initializes the DRAM devices on all DCTs at the same time
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNStartupDCTClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;

  // Update NB frequency for startup DDR speed
  NBPtr->ChangeNbFrequency (NBPtr);

  // Program D18F2x[1,0]9C_x0000_000B = 80000000h. #109999.
  MemNBrdcstSetNb (NBPtr, BFDramPhyStatusReg, 0x80000000);

  // Program D18F2x[1,0]9C_x0D0F_E013[PllRegWaitTime] = 0118h. #193770.
  MemNBrdcstSetNb (NBPtr, BFPllRegWaitTime, 0x118);

  // Phy Voltage Level Programming
  MemNPhyVoltageLevelClientNb (NBPtr);

  // Run frequency change sequence
  MemNBrdcstSetNb (NBPtr, BFPllLockTime, NBPtr->FreqChangeParam->PllLockTimeDefault);
  MemNBrdcstSetNb (NBPtr, BFMemClkFreq, NBPtr->GetMemClkFreqId (NBPtr, NBPtr->DCTPtr->Timings.Speed));
  MemNProgramNbPstateDependentRegistersClientNb (NBPtr);
  MemNBrdcstSetNb (NBPtr, BFMemClkFreqVal, 1);
  MemNBrdcstSetNb (NBPtr, BFPllLockTime, 0x000F);

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      IDS_HDT_CONSOLE ("!\tDct %d\n", Dct);

      // Phy fence programming
      AGESA_TESTPOINT (TpProcMemPhyFenceTraining, &(NBPtr->MemPtr->StdHeader));
      NBPtr->PhyFenceTraining (NBPtr);

      // Phy compensation initialization
      AGESA_TESTPOINT (TPProcMemPhyCompensation, &(NBPtr->MemPtr->StdHeader));
      NBPtr->MemNInitPhyComp (NBPtr);
    }
  }

  // Run DramInit sequence
  AGESA_TESTPOINT (TpProcMemDramInit, &(NBPtr->MemPtr->StdHeader));
  NBPtr->FeatPtr->DramInit (NBPtr->TechPtr);

}

/* -----------------------------------------------------------------------------*/
/**
 *
 * MemNChangeFrequencyHy:
 *
 *  This function change MemClk frequency to the value that is specified by DCTPtr->Timings.Speed
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNChangeFrequencyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;
  UINT8 Dct;
  UINT8 ChipSel;
  UINT32 Dummy;

  TechPtr = NBPtr->TechPtr;
  if (NBPtr->IsSupported[CheckDisDllShutdownSR] && !(NBPtr->IsSupported[SetDllShutDown])) {
    // #107421
    MemNBrdcstSetNb (NBPtr, BFDisDllShutdownSR, 1);
  }

  //Program F2x[1,0]90[EnterSelfRefresh]=1.
  //Wait until the hardware resets F2x[1,0]90[EnterSelfRefresh]=0.
  MemNBrdcstSetNb (NBPtr, BFEnterSelfRef, 1);
  MemNPollBitFieldNb (NBPtr, BFEnterSelfRef, 0, PCI_ACCESS_TIMEOUT, TRUE);

  //Program F2x9C_x08[DisAutoComp]=1
  MemNSwitchDCTNb (NBPtr, 0);
  MemNSetBitFieldNb (NBPtr, BFDisAutoComp, 1);

  //Program F2x[1, 0]94[MemClkFreqVal] = 0.
  MemNBrdcstSetNb (NBPtr, BFMemClkFreqVal, 0);

  //Program F2x[1, 0]94[MemClkFreq] to specify the target MEMCLK frequency.
  MemNBrdcstSetNb (NBPtr, BFMemClkFreq, NBPtr->GetMemClkFreqId (NBPtr, NBPtr->DCTPtr->Timings.Speed));

  //Program F2x[1, 0]94[MemClkFreqVal] = 1.
  MemNBrdcstSetNb (NBPtr, BFMemClkFreqVal, 1);

  //Wait until F2x[1, 0]94[FreqChgInProg]=0.
  MemNPollBitFieldNb (NBPtr, BFFreqChgInProg, 0, PCI_ACCESS_TIMEOUT, TRUE);

  if (NBPtr->IsSupported[CheckPhyFenceTraining]) {
    //Perform Phy Fence retraining after frequency changed
    AGESA_TESTPOINT (TpProcMemPhyFenceTraining, &(NBPtr->MemPtr->StdHeader));
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      MemNSwitchDCTNb (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        IDS_HDT_CONSOLE ("!\tDct %d\n", Dct);
        AGESA_TESTPOINT (TpProcMemPhyFenceTraining, &(NBPtr->MemPtr->StdHeader));
        MemNPhyFenceTrainingNb (NBPtr);
      }
    }
  }

  //Program F2x9C_x08[DisAutoComp]=0
  MemNSwitchDCTNb (NBPtr, 0);
  MemNSetBitFieldNb (NBPtr, BFDisAutoComp, 0);

  //Program F2x[1,0]90[ExitSelfRef]=1 for both DCTs.
  //Wait until the hardware resets F2x[1, 0]90[ExitSelfRef]=0.
  MemNBrdcstSetNb (NBPtr, BFExitSelfRef, 1);
  MemNPollBitFieldNb (NBPtr, BFExitSelfRef, 0, PCI_ACCESS_TIMEOUT, TRUE);

  if (NBPtr->MCTPtr->Status[SbRegistered]) {
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      MemNSwitchDCTNb (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        TechPtr->FreqChgCtrlWrd (TechPtr);
      }
    }
  }

  //wait for 500 MCLKs after ExitSelfRef, 500*2.5ns=1250ns
  MemUWait10ns (125, NBPtr->MemPtr);

  if (NBPtr->IsSupported[CheckDisDllShutdownSR] && !(NBPtr->IsSupported[SetDllShutDown])) {
    // #107421
    MemNBrdcstSetNb (NBPtr, BFDisDllShutdownSR, 0);
  }

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    IDS_HDT_CONSOLE ("!\tDct %d\n", Dct);
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {

      //9.Configure the DCT to send initialization MR commands:
      //  BIOS must reprogram Twr, Tcwl, and Tcl based on the new MEMCLK frequency.
      //  Program F2x[1, 0]7C similar to step #2 in Pass 1 above for the new Dimm values.
      TechPtr->AutoCycTiming (TechPtr);
      if (!MemNPlatformSpecNb (NBPtr)) {
        IDS_ERROR_TRAP;
      }

      for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel++) {
        if (NBPtr->IsSupported[CheckGetMCTSysAddr]) {
          if (MemNGetMCTSysAddrNb (NBPtr, ChipSel, &Dummy)) {
            // if chip select present
            TechPtr->SendAllMRCmds (TechPtr, ChipSel);
            // NOTE: wait 512 clocks for DLL-relock
            MemUWait10ns (50000, NBPtr->MemPtr);  // wait 500us
          }
        }
        if (NBPtr->IsSupported[CheckSendAllMRCmds]) {
          if (MemNGetMCTSysAddrNb (NBPtr, ChipSel, &Dummy)) {
            // if chip select present
            TechPtr->SendAllMRCmds (TechPtr, ChipSel);
          }
        }
      }
      if ((NBPtr->DCTPtr->Timings.Speed == DDR1600_FREQUENCY) && (NBPtr->IsSupported[CheckDllSpeedUp])) {
        MemNSetBitFieldNb (NBPtr, BFPhy0x0D080F11, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D080F11) | 0x2000));
        MemNSetBitFieldNb (NBPtr, BFPhy0x0D080F10, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D080F10) | 0x2000));
        MemNSetBitFieldNb (NBPtr, BFPhy0x0D088F30, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D088F30) | 0x2000));
        MemNSetBitFieldNb (NBPtr, BFPhy0x0D08C030, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D08C030) | 0x2000));
        if (Dct == 0) {
          MemNSetBitFieldNb (NBPtr, BFPhy0x0D082F30, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D082F30) | 0x2000));
        }
        // NOTE: wait 512 clocks for DLL-relock
        MemUWait10ns (50000, NBPtr->MemPtr);  // wait 500us
      }
    }
  }
  // Re-enable phy compensation since it had been disabled during InitPhyComp
  MemNSwitchDCTNb (NBPtr, 0);
  MemNSetBitFieldNb (NBPtr, BFDisAutoComp, 0);
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function ramp up frequency the next level if it have not reached
 *      its TargetSpeed yet.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNRampUpFrequencyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST UINT16 FreqList[] = {
    DDR400_FREQUENCY,
    DDR533_FREQUENCY,
    DDR667_FREQUENCY,
    DDR800_FREQUENCY,
    DDR1066_FREQUENCY,
    DDR1333_FREQUENCY,
    DDR1600_FREQUENCY,
    DDR1866_FREQUENCY
  };
  UINT8 Dct;
  UINT8 i;
  UINT16 NewSpeed;
  DIE_STRUCT *MCTPtr;

  MCTPtr = NBPtr->MCTPtr;

  // Do not change frequency when it is already at TargetSpeed
  if (NBPtr->DCTPtr->Timings.Speed == NBPtr->DCTPtr->Timings.TargetSpeed) {
    return TRUE;
  }

  // Find the next supported frequency level
  NewSpeed = NBPtr->DCTPtr->Timings.TargetSpeed;
  for (i = 0; i < (GET_SIZE_OF (FreqList) - 1); i++) {
    if (NBPtr->DCTPtr->Timings.Speed == FreqList[i]) {
      NewSpeed = FreqList[i + 1];
      break;
    }
  }
  ASSERT (i < (GET_SIZE_OF (FreqList) - 1));
  ASSERT (NewSpeed <= NBPtr->DCTPtr->Timings.TargetSpeed);

  // BIOS must program both DCTs to the same frequency.
  IDS_HDT_CONSOLE ("\nMemClkFreq changed: %d MHz", NBPtr->DCTPtr->Timings.Speed);
  for (Dct = 0; Dct < MCTPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    NBPtr->DCTPtr->Timings.Speed = NewSpeed;
  }
  IDS_HDT_CONSOLE (" -> %d MHz", NewSpeed);

  IDS_OPTION_HOOK (IDS_BEFORE_MEM_FREQ_CHG, NBPtr, &(NBPtr->MemPtr->StdHeader));
  NBPtr->ChangeFrequency (NBPtr);

  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function uses calculated values from DCT.Timings structure to
 *      program its registers.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNProgramCycTimingsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST CTENTRY TmgAdjTab[] = {
    // BitField, Min, Max, Bias, Ratio_x2
    {BFTcl, 4, 12, 4, 2},
    {BFTrcd, 5, 12, 5, 2},
    {BFTrp, 5, 12, 5, 2},
    {BFTrtp, 4, 7, 4, 2},
    {BFTras, 15, 30, 15, 2},
    {BFTrc, 11, 42, 11, 2},
    {BFTwrDDR3, 5, 12, 4, 2},
    {BFTrrd, 4, 7, 4, 2},
    {BFTwtr, 4, 7, 4, 2},
    {BFFourActWindow, 16, 32, 14, 1}
  };

  DCT_STRUCT *DCTPtr;
  UINT8  *MiniMaxTmg;
  UINT8  *MiniMaxTrfc;
  UINT8  Value8;
  UINT8  j;
  BIT_FIELD_NAME BitField;

  DCTPtr = NBPtr->DCTPtr;

  //======================================================================
  // Program turnaround timings to their max during DRAM init and training
  //======================================================================
  //
  MemNSetBitFieldNb (NBPtr, BFNonSPD, 0x28FF);

  MemNSetBitFieldNb (NBPtr, BFNonSPDHi, 0x2A);

  //======================================================================
  // Program DRAM Timing values
  //======================================================================
  //
  MiniMaxTmg = &DCTPtr->Timings.CasL;
  for (j = 0; j < GET_SIZE_OF (TmgAdjTab); j++) {
    BitField = TmgAdjTab[j].BitField;

    if (MiniMaxTmg[j] < TmgAdjTab[j].Min) {
      MiniMaxTmg[j] = TmgAdjTab[j].Min;
    } else if (MiniMaxTmg[j] > TmgAdjTab[j].Max) {
      MiniMaxTmg[j] = TmgAdjTab[j].Max;
    }

    Value8 = (UINT8) MiniMaxTmg[j];

    if (BitField == BFTwrDDR3) {
      Value8 = (Value8 == 10) ? 9 : (Value8 == 12) ? 10 : Value8;
    } else if (BitField == BFTrtp) {
      Value8 = (DCTPtr->Timings.Speed <= DDR1066_FREQUENCY) ? 4 : (DCTPtr->Timings.Speed == DDR1333_FREQUENCY) ? 5 : 6;
    }

    Value8 = Value8 - TmgAdjTab[j].Bias;
    Value8 = (Value8 * TmgAdjTab[j].Ratio_x2) >> 1;

    MemNSetBitFieldNb (NBPtr, BitField, Value8);
  }

  MiniMaxTrfc = &DCTPtr->Timings.Trfc0;
  for (j = 0; j < 4; j++) {
    MemNSetBitFieldNb (NBPtr, BFTrfc0 + j, MiniMaxTrfc[j]);
  }

  MemNSetBitFieldNb (NBPtr, BFTcwl, ((DCTPtr->Timings.Speed >= DDR800_FREQUENCY) ?
                                     (NBPtr->GetMemClkFreqId (NBPtr, DCTPtr->Timings.Speed) - 3) : 0));

  MemNSetBitFieldNb (NBPtr, BFTref, 2);      // 7.8 us

  //======================================================================
  // DRAM MRS Register, set ODT
  //======================================================================
  //
  // DrvImpCtrl: drive impedance control.01b(34 ohm driver; Ron34 = Rzq/7)
  MemNSetBitFieldNb (NBPtr, BFDrvImpCtrl, 1);

  // burst length control
  if (NBPtr->MCTPtr->Status[Sb128bitmode]) {
    MemNSetBitFieldNb (NBPtr, BFBurstCtrl, 2);
  }

  // ASR=1, auto self refresh; SRT=0
  MemNSetBitFieldNb (NBPtr, BFASR, 1);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function uses calculated values from DCT.Timings structure to
 *      program its registers.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNProgramCycTimingsClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST CTENTRY TmgAdjTab[] = {
    // BitField, Min, Max, Bias, Ratio_x2
    {BFTcl, 5, 14, 4, 2},
    {BFTrcd, 5, 14, 5, 2},
    {BFTrp, 5, 14, 5, 2},
    {BFTrtp, 4, 8, 4, 2},
    {BFTras, 15, 36, 15, 2},
    {BFTrc, 20, 49, 11, 2},
    {BFTwrDDR3, 5, 16, 4, 2},
    {BFTrrd, 4, 8, 4, 2},
    {BFTwtr, 4, 8, 4, 2},
    {BFFourActWindow, 16, 40, 14, 1}
  };

  DCT_STRUCT *DCTPtr;
  UINT8  *MiniMaxTmg;
  UINT8  *MiniMaxTrfc;
  UINT8  Value8;
  UINT8  j;
  UINT8  Tcwl;
  BIT_FIELD_NAME BitField;

  DCTPtr = NBPtr->DCTPtr;

  //======================================================================
  // Program DRAM Timing values
  //======================================================================
  //
  MiniMaxTmg = &DCTPtr->Timings.CasL;
  for (j = 0; j < GET_SIZE_OF (TmgAdjTab); j++) {
    BitField = TmgAdjTab[j].BitField;

    if (MiniMaxTmg[j] < TmgAdjTab[j].Min) {
      MiniMaxTmg[j] = TmgAdjTab[j].Min;
    } else if (MiniMaxTmg[j] > TmgAdjTab[j].Max) {
      MiniMaxTmg[j] = TmgAdjTab[j].Max;
    }

    Value8 = (UINT8) MiniMaxTmg[j];

    if (BitField == BFTwrDDR3) {
      Value8 = (Value8 >= 10) ? (((Value8 + 1) / 2) + 4) : Value8;
    }

    Value8 = Value8 - TmgAdjTab[j].Bias;
    Value8 = (Value8 * TmgAdjTab[j].Ratio_x2) >> 1;

    MemNSetBitFieldNb (NBPtr, BitField, Value8);
  }

  MiniMaxTrfc = &DCTPtr->Timings.Trfc0;
  for (j = 0; j < 4; j++) {
    MemNSetBitFieldNb (NBPtr, BFTrfc0 + j, MiniMaxTrfc[j]);
  }

  Tcwl = (UINT8) (DCTPtr->Timings.Speed / 133) + 2;
  MemNSetBitFieldNb (NBPtr, BFTcwl, ((Tcwl > 5) ? (Tcwl - 5) : 0));

  MemNSetBitFieldNb (NBPtr, BFTref, 2);      // Tref = 7.8 us

  MemNSetBitFieldNb (NBPtr, BFDbeSkidBufDis, (DCTPtr->Timings.Trcd > 10) ? 0 : 1);

  MemNSetBitFieldNb (NBPtr, BFRdOdtTrnOnDly, (DCTPtr->Timings.CasL > Tcwl) ? (DCTPtr->Timings.CasL - Tcwl) : 0);

  // Set ProcOdtAdv
  if (DCTPtr->Timings.Speed <= DDR1333_FREQUENCY) {
    MemNSetBitFieldNb (NBPtr, BFPhy0x0D0F0F13, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D0F0F13) & 0xBFFF));
  } else {
    MemNSetBitFieldNb (NBPtr, BFPhy0x0D0F0F13, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D0F0F13) | 0x4000));
  }
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
MemNGetMaxLatParamsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly,
  IN OUT   UINT16 *MinDlyPtr,
  IN OUT   UINT16 *MaxDlyPtr,
  IN OUT   UINT16 *DlyBiasPtr
  )
{
  *MinDlyPtr = (MemNTotalSyncComponentsNb (NBPtr) + (MaxRcvEnDly >> 5)) * 2;
  MemNQuarterMemClk2NClkNb (NBPtr, MinDlyPtr);

  *MaxDlyPtr = 0x3FF;

  *DlyBiasPtr = 4;
  MemNQuarterMemClk2NClkNb (NBPtr, DlyBiasPtr);   // 1 MEMCLK Margin

  *DlyBiasPtr += 1;  // add 1 NCLK
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
MemNGetMaxLatParamsClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxDlyForMaxRdLat,
  IN OUT   UINT16 *MinDlyPtr,
  IN OUT   UINT16 *MaxDlyPtr,
  IN OUT   UINT16 *DlyBiasPtr
  )
{
  UINT32 P;
  UINT32 T;
  UINT32 MemClkPeriod;

  T = MemNTotalSyncComponentsClientNb (NBPtr);

  // P = P + CEIL(MAX (total delay in DqsRcvEn + RdDqsTime))
  P = (MaxDlyForMaxRdLat + 31) / 32;

  MemClkPeriod = 1000000 / NBPtr->DCTPtr->Timings.Speed;

  *MinDlyPtr = (UINT16) (((((P * MemClkPeriod + 1) / 2) + T) * NBPtr->NBClkFreq + 999999) / 1000000);

  *MinDlyPtr += 4;

  *MaxDlyPtr = 0x50;

  *DlyBiasPtr = 3;

  // Need to set ForceCasToSlot0=1 before MaxRdLatency training
  MemNSetBitFieldNb (NBPtr, BFForceCasToSlot0, 1);
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
MemNSetMaxLatencyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  )
{
  UINT16 SubTotal;

  AGESA_TESTPOINT (TpProcMemRcvrCalcLatency, &(NBPtr->MemPtr->StdHeader));

  SubTotal = 0xC8;    // init value for MaxRdLat used in training


  if (MaxRcvEnDly != 0xFFFF) {
    // Get all sync components BKDG steps 1-5
    SubTotal = MemNTotalSyncComponentsNb (NBPtr);

    // Add the maximum (worst case) delay value of DqsRcvEnGrossDelay
    // that exists across all DIMMs and byte lanes.
    //
    SubTotal += MaxRcvEnDly >> 5;


    // Add 14.5 to the sub-total. 14.5 represents part of the processor
    // specific constant delay value in the DRAM clock domain.
    //
    SubTotal <<= 1;             // scale 1/2 MemClk to 1/4 MemClk
    SubTotal += 29;             // add 14.5 1/2 MemClk

    // Convert the sub-total (in 1/2 MEMCLKs) to northbridge clocks (NCLKs)
    // as follows (assuming DDR400 and assuming that no P-state or link speed
    // changes have occurred).
    //
    MemNQuarterMemClk2NClkNb (NBPtr, &SubTotal);

    // Add 2 NCLKs to the sub-total. 2 represents part of the processor
    // specific constant value in the northbridge clock domain.
    //
    SubTotal += 2;
  }

  NBPtr->DCTPtr->Timings.MaxRdLat = SubTotal;
  // Program the F2x[1, 0]78[MaxRdLatency] register with the total delay value
  IDS_HDT_CONSOLE ("\t\tMaxRdLat: %03x\n", SubTotal);
  MemNSetBitFieldNb (NBPtr, BFMaxLatency, SubTotal);
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
  MemUWait10ns (128, NBPtr->MemPtr);   // 512*2.5ns=1280, wait 1280ns
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
 *       This function Return the binary value of tfaw associated with
 *       the index k
 *
 *     @param[in]     k value
 *
 *     @return       F[k], in Binary MHz.
 */

UINT8
MemNGet1KTFawTkNb (
  IN       UINT8 k
  )
{
  CONST UINT8 Tab1KTfawTK[] = {0, 8, 10, 13, 14, 19};
  ASSERT (k <= 5);
  return Tab1KTfawTK[k];
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *       This function Return the binary value of the 2KTFaw associated with
 *       the index k
 *
 *     @param[in]     k value
 *
 *     @return       2KTFaw converted based on k.
 */

UINT8
MemNGet2KTFawTkNb (
  IN       UINT8 k
  )
{
  CONST UINT8 Tab2KTfawTK[] = {0, 10, 14, 17, 18, 24};
  ASSERT (k <= 5);
  return Tab2KTfawTK[k];
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function converts the sub-total (in 1/4 MEMCLKs) to northbridge clocks (NCLKs)
 *      (assuming DDR400 and assuming that no P-state or link speed
 *      changes have occurred).
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *SubTotalPtr - pointer to Sub-Total
 */

VOID
STATIC
MemNQuarterMemClk2NClkNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   UINT16 *SubTotalPtr
  )
{
  UINT32 NBFreq;
  UINT32 MemFreq;

  // Multiply SubTotal by NB COF
  NBFreq = (MemNGetBitFieldNb (NBPtr, BFNbFid) + 4) * 200;
  // Divide SubTotal by 4 times current MemClk frequency
  MemFreq = NBPtr->DCTPtr->Timings.Speed * 4;
  *SubTotalPtr = (UINT16) (((NBFreq * (*SubTotalPtr)) + MemFreq - 1) / MemFreq);  // round up
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets the total of sync components for Max Read Latency calculation
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      Total in 1/2 MEMCLKs
 */

UINT16
MemNTotalSyncComponentsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT16 SubTotal;

  // Multiply the CAS Latency by two to get a number of 1/2 MEMCLKs UINTs.
  SubTotal = (UINT16) MemNGetBitFieldNb (NBPtr, BFTcl) + 1;
  if ((MemNGetBitFieldNb (NBPtr, BFDdr3Mode)) != 0) {
    SubTotal += 3;
  }
  SubTotal *= 2;

  // If registered DIMMs are being used then add 1 MEMCLK to the sub-total.
  if ((MemNGetBitFieldNb (NBPtr, BFUnBuffDimm)) == 0) {
    SubTotal += 2;
  }

  // If (F2x[1, 0]9C_x04[AddrCmdSetup] and F2x[1, 0]9C_x04[CsOdtSetup] and F2x[1, 0]9C_x04[Cke-Setup] = 0) then K = K + 1
  // If (F2x[1, 0]9C_x04[AddrCmdSetup] or F2x[1, 0]9C_x04[CsOdtSetup] or F2x[1, 0]9C_x04[CkeSetup] = 1) then K = K + 2
  if ((MemNGetBitFieldNb (NBPtr, BFAddrTmgControl) & 0x0202020) == 0) {
    SubTotal += 1;
  } else {
    SubTotal += 2;
  }

  // If the F2x[1, 0]78[RdPtrInit] field is 4, 5, 6 or 7 MEMCLKs,
  // then add 4, 3, 2, or 1 MEMCLKs, respectively to the sub-total.
  //
  SubTotal = SubTotal + (8 - (UINT16) MemNGetBitFieldNb (NBPtr, BFRdPtrInit));

  return SubTotal;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function swaps bits for OnDimmMirror support
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSwapBitsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 ChipSel;
  UINT32 MRSReg;

  ChipSel = (UINT8) MemNGetBitFieldNb (NBPtr, BFMrsChipSel);
  if ((ChipSel & 1) != 0) {
    MRSReg = MemNGetBitFieldNb (NBPtr, BFDramInitRegReg);
    if ((NBPtr->DCTPtr->Timings.DimmMirrorPresent & (1 << (ChipSel >> 1))) != 0) {
      MRSReg = (MRSReg & 0xFFFCFE07) | ((MRSReg&0x100A8) << 1) | ((MRSReg&0x20150) >> 1);
      MemNSetBitFieldNb (NBPtr, BFDramInitRegReg, MRSReg);
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
  CONST UINT8           TabSize[3]  = {          2,           4,            8};
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
      TabPtr = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PinType[k], NBPtr->MCTPtr->SocketId, MemNGetSocketRelativeChannelNb (NBPtr, NBPtr->Dct, 0));
    }
    if (NBPtr->IsSupported[CheckFindPSDct]) {
      TabPtr = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PinType[k], NBPtr->MCTPtr->SocketId, NBPtr->Dct);
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
  MemNSetBitFieldNb (NBPtr, BFODCControl, NBPtr->ChannelPtr->DctOdcCtl);
  MemNSetBitFieldNb (NBPtr, BFAddrTmgControl, NBPtr->ChannelPtr->DctAddrTmg);

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
 *   This function gets the Trdrd value
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      Trdrd value
 */

UINT8
MemNGetTrdrdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  DCT_STRUCT *DCTPtr;
  INT8 Cgdd;

  DCTPtr = NBPtr->DCTPtr;

  // BIOS calculates Trdrd (in MEMCLKs) = CGDD / 2 + 3 clocks and programs F2x[1, 0]8C[Trdrd] with the
  // converted field value. BIOS rounds fractional values down.
  // The Critical Gross Delay Difference (CGDD) for Trdrd on any given byte lane is the largest F2x[1,
  // 0]9C_x[3:0][2B:10][DqsRcvEnGrossDelay] delay of any DIMM minus the F2x[1,
  // 0]9C_x[3:0][2B:10][DqsRcvEnGrossDelay] delay of any other DIMM.

  Cgdd = MemNGetOptimalCGDDNb (NBPtr, AccessRcvEnDly, AccessRcvEnDly);
  DCTPtr->Timings.Trdrd = (Cgdd / 2) + 3;

  // Transfer clk to reg definition, 2T is 00b, etc.
  DCTPtr->Timings.Trdrd -= 2;
  if (DCTPtr->Timings.Trdrd > 8) {
    DCTPtr->Timings.Trdrd = 8;
  }

  return DCTPtr->Timings.Trdrd;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets the Twrwr value
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      Twrwr value
 */

UINT8
MemNGetTwrwrNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  DCT_STRUCT *DCTPtr;
  INT8 Cgdd;

  DCTPtr = NBPtr->DCTPtr;

  // Twrwr (in MEMCLKs) = CGDD / 2 + 3 clocks and programs F2x[1, 0]8C[Twrwr] with the
  // converted field value. BIOS rounds fractional values down.
  // On any given byte lane, the largest F2x[1, 0]9C_x[3:0][A, 7, 6, 0][2:1]:F2x[1, 0]9C_x[3:0][A, 7, 6,
  // 0]3[WrDatGrossDlyByte] delay of any DIMM minus the F2x[1, 0]9C_x[3:0][A, 7, 6, 0][2:1]:F2x[1,
  // 0]9C_x[3:0][A, 7, 6, 0]3[WrDatGrossDlyByte] delay of any other DIMM is equal to the Critical Gross
  // Delay Difference (CGDD) for Twrwr.

  Cgdd = MemNGetOptimalCGDDNb (NBPtr, AccessWrDatDly, AccessWrDatDly);
  DCTPtr->Timings.Twrwr = (Cgdd / 2) + 3;
  NBPtr->TechPtr->AdjustTwrwr (NBPtr->TechPtr);

  return DCTPtr->Timings.Twrwr;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets the Twrrd value. BIOS calculates Twrrd (in MEMCLKs) = CGDD / 2 - LD + 3 clocks and programs
 * F2x[1, 0]8C[Twrrd] with the converted field value. BIOS rounds fractional
 * values down.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return  Value to be programmed to Twrrd field
 *              pDCT->Timings.Twrrd updated
 */

UINT8
MemNGetTwrrdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  INT8 Cgdd;
  INT8 Ld;
  INT8 Twrrd;
  DCT_STRUCT *DCTPtr;

  DCTPtr = NBPtr->DCTPtr;

  //
  // For DDR3, BIOS calculates the latency difference (Ld) as equal to read CAS latency minus write CAS
  // latency, in MEMCLKs (see F2x[1, 0]88[Tcl] and F2x[1, 0]84[Tcwl]) which can be a negative or positive
  // value.
  // For DDR2, LD is always one clock (For DDR2, Tcwl is always Tcl minus 1).
  //
  Ld = NBPtr->TechPtr->GetLD (NBPtr->TechPtr);

  // On any given byte lane, the largest WrDatGrossDlyByte delay of any DIMM
  // minus the DqsRcvEnGrossDelay delay of any other DIMM is
  // equal to the Critical Gross Delay Difference (CGDD) for Twrrd.
  Cgdd = MemNGetOptimalCGDDNb (NBPtr, AccessWrDatDly, AccessRcvEnDly);
  Twrrd = (Cgdd / 2) - Ld + 3;
  DCTPtr->Timings.Twrrd = (UINT8) ((Twrrd >= 0) ? Twrrd : 0);
  NBPtr->TechPtr->AdjustTwrrd (NBPtr->TechPtr);

  return DCTPtr->Timings.Twrrd;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets the TrwtTO value
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return  pDCT->Timings.TrwtTO updated
 */

UINT8
MemNGetTrwtTONb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  INT8 Cgdd;
  INT8 Ld;
  INT8 TrwtTO;
  DCT_STRUCT *DCTPtr;

  DCTPtr = NBPtr->DCTPtr;
  //
  // For DDR3, BIOS calculates the latency difference (Ld) as equal to read CAS latency minus write CAS
  // latency, in MEMCLKs (see F2x[1, 0]88[Tcl] and F2x[1, 0]84[Tcwl]) which can be a negative or positive
  // value.
  // For DDR2, LD is always one clock (For DDR2, Tcwl is always Tcl minus 1).
  //
  Ld = NBPtr->TechPtr->GetLD (NBPtr->TechPtr);

  // On any byte lane, the largest DqsRcvEnGrossDelay delay of any DIMM minus
  // the WrDatGrossDlyByte delay of any other DIMM is equal to the Critical Gross
  // Delay Difference (CGDD) for TrwtTO.
  Cgdd = MemNGetOptimalCGDDNb (NBPtr, AccessRcvEnDly, AccessWrDatDly);
  TrwtTO = (Cgdd / 2) + Ld + 3;
  TrwtTO -= 2;
  DCTPtr->Timings.TrwtTO = (UINT8) ((TrwtTO > 1) ? TrwtTO : 1);

  return DCTPtr->Timings.TrwtTO;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets the TrwtWB value
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      TrwtWB value
 */
UINT8
MemNGetTrwtWBNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  DCT_STRUCT *DCTPtr;

  DCTPtr = NBPtr->DCTPtr;

  // TrwtWB ensures read-to-write data-bus turnaround.
  // This value should be one more than the programmed TrwtTO.
  return DCTPtr->Timings.TrwtWB = DCTPtr->Timings.TrwtTO;
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
MemNGetMemClkFreqIdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 Speed
  )
{
  return (UINT8) ((Speed < DDR800_FREQUENCY) ? ((Speed / 66) - 3) : (Speed / 133));
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
MemNGetMemClkFreqIdClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 Speed
  )
{
  return (UINT8) ((Speed > DDR400_FREQUENCY) ? ((Speed / 33) - 6) : ((Speed == DDR400_FREQUENCY) ? 2 : (Speed / 55)));
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function change MemClk frequency to the value that is specified by DCTPtr->Timings.Speed
 *  for client NB.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNChangeFrequencyClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;
  UINT8 Dct;
  UINT8 ChipSel;
  UINT32 Dummy;
  BOOLEAN FrequencyChangeSuccess;

  TechPtr = NBPtr->TechPtr;

  // 1. Program D18F2x[1,0]9C_x0D0F_E006[PllLockTime] = 0190h for Llano
  // 1. Program D18F2x9C_x0D0F_E006[PllLockTime] = 1838h for Ontario
  MemNBrdcstSetNb (NBPtr, BFPllLockTime, NBPtr->FreqChangeParam->PllLockTimeDefault);
  MemNBrdcstSetNb (NBPtr, BFDisDllShutdownSR, 1);

  //Program F2x[1,0]90[EnterSelfRefresh]=1.
  //Wait until the hardware resets F2x[1,0]90[EnterSelfRefresh]=0.
  MemNBrdcstSetNb (NBPtr, BFEnterSelfRef, 1);
  MemNPollBitFieldNb (NBPtr, BFEnterSelfRef, 0, PCI_ACCESS_TIMEOUT, TRUE);

  if (NBPtr->ChangeNbFrequency (NBPtr)) {
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
    MemNProgramNbPstateDependentRegistersClientNb (NBPtr);

    // 7. Program D18F2x[1,0]94[MemClkFreqVal] = 1.
    MemNBrdcstSetNb (NBPtr, BFMemClkFreqVal, 1);

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

  //Program F2x[1,0]90[ExitSelfRef]=1 for both DCTs.
  //Wait until the hardware resets F2x[1, 0]90[ExitSelfRef]=0.
  MemNBrdcstSetNb (NBPtr, BFExitSelfRef, 1);
  MemNPollBitFieldNb (NBPtr, BFExitSelfRef, 0, PCI_ACCESS_TIMEOUT, TRUE);
  MemNBrdcstSetNb (NBPtr, BFDisDllShutdownSR, 0);

  // 8. IF (D18F2x[1,0]9C_x0D0F_E00A[CsrPhySrPllPdMode]==0) THEN program
  // D18F2x[1,0]9C_x0D0F_E006[PllLockTime] = 0Fh.
  // (CsrPhySrPllPdMode is kept 0 before training)
  MemNBrdcstSetNb (NBPtr, BFPllLockTime, 0x000F);

  if (FrequencyChangeSuccess) {
    // Perform Phy Fence training and Phy comp init after frequency change
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      MemNSwitchDCTNb (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        IDS_HDT_CONSOLE ("!\tDct %d\n", Dct);

        // Phy fence programming
        AGESA_TESTPOINT (TpProcMemPhyFenceTraining, &(NBPtr->MemPtr->StdHeader));
        NBPtr->PhyFenceTraining (NBPtr);

        // Phy compensation initialization
        AGESA_TESTPOINT (TPProcMemPhyCompensation, &(NBPtr->MemPtr->StdHeader));
        NBPtr->MemNInitPhyComp (NBPtr);
      }
    }

    //======================================================================
    // Calculate and program DRAM Timings at new frequency
    //======================================================================
    //
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      IDS_HDT_CONSOLE ("!\tDct %d\n", Dct);
      MemNSwitchDCTNb (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {

        //9.Configure the DCT to send initialization MR commands:
        //  BIOS must reprogram Twr, Tcwl, and Tcl based on the new MEMCLK frequency.
        TechPtr->AutoCycTiming (TechPtr);
        if (!MemNPlatformSpecNb (NBPtr)) {
          IDS_ERROR_TRAP;
        }

        for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel++) {
          if (MemNGetMCTSysAddrNb (NBPtr, ChipSel, &Dummy)) {
            // if chip select present
            TechPtr->SendAllMRCmds (TechPtr, ChipSel);
          }
        }
        // Wait 512 clocks for DLL-relock
        MemUWait10ns (50000, NBPtr->MemPtr);  // wait 500us
      }
    }
  }
}


/* -----------------------------------------------------------------------------*/
CONST UINT16 PllDivTab[10] = {1, 2, 4, 8, 16, 128, 256, 1, 3, 6};

/**
 *
 *  This function calculates and programs NB P-state dependent registers
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNProgramNbPstateDependentRegistersClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 i;
  UINT8 Dct;
  UINT8 NclkFid;
  UINT16 MemClkDid;
  UINT8 PllMult;
  UINT8 NclkDiv;
  UINT8 RdPtrInitMin;
  UINT8 RdPtrInit;
  UINT32 NclkPeriod;
  UINT32 MemClkPeriod;
  INT32 PartialSum2x;
  INT32 PartialSumSlotI2x;

  NclkFid = (UINT8) (MemNGetBitFieldNb (NBPtr, BFMainPllOpFreqId) + 10);
  ASSERT (MemNGetBitFieldNb (NBPtr, BFPllDiv) < 10);
  MemClkDid = PllDivTab[MemNGetBitFieldNb (NBPtr, BFPllDiv)];
  PllMult = (UINT8) MemNGetBitFieldNb (NBPtr, BFPllMult);
  if (MemNGetBitFieldNb (NBPtr, BFNbPs1Act) == 1) {
    NclkDiv = (UINT8) MemNGetBitFieldNb (NBPtr, BFNbPs1NclkDiv);
    IDS_HDT_CONSOLE ("\n\tNB P1");
  } else {
    NclkDiv = (UINT8) MemNGetBitFieldNb (NBPtr, BFNbPs0NclkDiv);
    IDS_HDT_CONSOLE ("\n\tNB P0");
  }
  NclkPeriod = (2500 * NclkDiv) / NclkFid;
  MemClkPeriod = 1000000 / NBPtr->DCTPtr->Timings.Speed;
  RdPtrInitMin = RdPtrInit = (NBPtr->DCTPtr->Timings.Speed >= DDR1333_FREQUENCY) ? NBPtr->FreqChangeParam->RdPtrInit667orHigher : NBPtr->FreqChangeParam->RdPtrInitLower667;
  ASSERT (NBPtr->NBClkFreq == (((UINT32) NclkFid * 400) / NclkDiv));

  IDS_HDT_CONSOLE ("  Freq: %lxMHz\n", NBPtr->NBClkFreq);
  IDS_HDT_CONSOLE ("\tMemClk Freq: %dMHz\n", NBPtr->DCTPtr->Timings.Speed);
  // D18F2x[1,0]78[RdPtrInit] = IF (D18F2x[1,0]94[MemClkFreq] >= 667 MHz) THEN 7 ELSE 8 ENDIF (Llano)
  //                                                                      THEN 2 ELSE 3 ENDIF (Ontario)
  MemNBrdcstSetNb (NBPtr, BFRdPtrInit, RdPtrInit);
  IDS_HDT_CONSOLE ("\t\tRdPtr: %d\n", RdPtrInit);

  // Program D18F2x[1,0]F4_x30[DbeGskFifoNumerator] and D18F2x[1,0]F4_x31[DbeGskFifoDenominator].
  MemNBrdcstSetNb (NBPtr, BFDbeGskFifoNumerator, NclkFid * MemClkDid * 16);
  MemNBrdcstSetNb (NBPtr, BFDbeGskFifoDenominator, PllMult * NclkDiv);
  IDS_HDT_CONSOLE ("\t\tDbeGskFifoNumerator: %d\n", NclkFid * MemClkDid * 16);
  IDS_HDT_CONSOLE ("\t\tDbeGskFifoDenominator: %d\n", PllMult * NclkDiv);

  // Program D18F2x[1,0]F4_x32[DataTxFifoSchedDlyNegSlot1, DataTxFifoSchedDlySlot1,
  // DataTxFifoSchedDlyNegSlot0, DataTxFifoSchedDlySlot0].
  //   PartialSum = ((7 * NclkPeriod) + (1.5 * MemClkPeriod) + 520ps)*MemClkFrequency - tCWL -
  //   CmdSetup - PtrSeparation - 1. (Llano)
  //   PartialSum = ((5 * NclkPeriod) + MemClkPeriod) + 520ps)*MemClkFrequency - tCWL -
  //   CmdSetup - PtrSeparation - 1. (Ontario)
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      PartialSum2x = NBPtr->FreqChangeParam->NclkPeriodMul2x * NclkPeriod;
      PartialSum2x += NBPtr->FreqChangeParam->MemClkPeriodMul2x * MemClkPeriod;
      PartialSum2x += 520 * 2;
      PartialSum2x = (PartialSum2x + MemClkPeriod - 1) / MemClkPeriod;  // round-up here
      PartialSum2x -= 2 * (MemNGetBitFieldNb (NBPtr, BFTcwl) + 5);
      if ((MemNGetBitFieldNb (NBPtr, BFAddrTmgControl) & 0x0202020) == 0) {
        PartialSum2x -= 1;
      } else {
        PartialSum2x -= 2;
      }
      PartialSum2x -= ((16 + RdPtrInitMin - RdPtrInit) % 16);
      PartialSum2x -= 2;

      // If PartialSumSlotN is positive:
      //   DataTxFifoSchedDlySlotN=CEIL(PartialSumSlotN).
      //   DataTxFifoSchedDlyNegSlotN=0.
      // Else if PartialSumSlotN is negative:
      //   DataTxFifoSchedDlySlotN=ABS(CEIL(PartialSumSlotN*MemClkPeriod/NclkPeriod)).
      //   DataTxFifoSchedDlyNegSlotN=1.
      for (i = 0; i < 2; i++) {
        PartialSumSlotI2x = PartialSum2x;
        if ((i == 0) && (MemNGetBitFieldNb (NBPtr, BFSlowAccessMode) == 0)) {
          PartialSumSlotI2x += 2;
        }
        if (PartialSumSlotI2x > 0) {
          ASSERT ((i != 0) && (PartialSumSlotI2x <= 2));  // Real system constrain
          MemNSetBitFieldNb (NBPtr, BFDataTxFifoSchedDlyNegSlot0 + i, 0);
          MemNSetBitFieldNb (NBPtr, BFDataTxFifoSchedDlySlot0 + i, (PartialSumSlotI2x + 1) / 2);
          IDS_HDT_CONSOLE ("\t\tDataTxFifoSchedDlySlot%d: %lx\n", i, (PartialSumSlotI2x + 1) / 2);
        } else {
          MemNSetBitFieldNb (NBPtr, BFDataTxFifoSchedDlyNegSlot0 + i, 1);
          PartialSumSlotI2x = (((-PartialSumSlotI2x) * MemClkPeriod) + (2 * NclkPeriod - 1)) / (2 * NclkPeriod);
          MemNSetBitFieldNb (NBPtr, BFDataTxFifoSchedDlySlot0 + i, PartialSumSlotI2x);
          IDS_HDT_CONSOLE ("\t\tDataTxFifoSchedDlySlot%d: -%lx\n", i, PartialSumSlotI2x);
        }
      }
    }
  }

  MemFInitTableDrive (NBPtr, MTAfterNbPstateChange);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets the total of sync components for Max Read Latency calculation
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      Total in 1/2 MEMCLKs
 */

UINT32
MemNTotalSyncComponentsClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 P;
  UINT32 T;
  UINT8  RdPtrInitMin;
  UINT8  RdPtrInit;
  UINT32 AddrTmgCtl;
  UINT8  DbeGskMemClkAlignMode;
  UINT32 MemClkPeriod;

  // P = P + ((16 + RdPtrInitMin - D18F2x[1,0]78[RdPtrInit]) MOD 16)
  RdPtrInitMin = (NBPtr->DCTPtr->Timings.Speed >= DDR1333_FREQUENCY) ? NBPtr->FreqChangeParam->RdPtrInit667orHigher : NBPtr->FreqChangeParam->RdPtrInitLower667;
  RdPtrInit = (UINT8) MemNGetBitFieldNb (NBPtr, BFRdPtrInit);
  P = (16 + RdPtrInitMin - RdPtrInit) % 16;

  // IF (AddrCmdSetup != CkeSetup) THEN P = P + 1
  AddrTmgCtl = MemNGetBitFieldNb (NBPtr, BFAddrTmgControl);
  if (((AddrTmgCtl >> 16) & 0x20) != (AddrTmgCtl & 0x20)) {
    P += 1;
  }

  // IF (DbeGskMemClkAlignMode==01b || (DbeGskMemClkAlignMode==00b && !(AddrCmdSetup==CsOdtSetup==CkeSetup)))
  // THEN P = P + 1
  DbeGskMemClkAlignMode = 2;  // from BKDG recommendation.
  if ((DbeGskMemClkAlignMode == 1) || ((DbeGskMemClkAlignMode == 0) &&
      !((((AddrTmgCtl >> 16) & 0x20) == (AddrTmgCtl & 0x20)) && (((AddrTmgCtl >> 8) & 0x20) == (AddrTmgCtl & 0x20))))) {
    P += 1;
  }

  // IF (SlowAccessMode==1) THEN P = P + 2
  if (MemNGetBitFieldNb (NBPtr, BFSlowAccessMode) == 1) {
    P += 2;
  }

  // IF (D18F2x[1,0]94[MemClkFreq] >= 667 MHz)
  // THEN T = T + MemClkPeriod - 786 ps
  // ELSE T = T + (0.5 * MemClkPeriod) - 786 ps
  MemClkPeriod = 1000000 / NBPtr->DCTPtr->Timings.Speed;
  if (NBPtr->DCTPtr->Timings.Speed >= DDR1333_FREQUENCY) {
    T = MemClkPeriod - 786;
  } else {
    T = (MemClkPeriod / 2) - 786;
  }

  // If (AddrCmdSetup==0 && CsOdtSetup==0 && CkeSetup==0)
  // then P = P + 1
  // else P = P + 2
  if ((AddrTmgCtl & 0x0202020) == 0) {
    P += 1;
  } else {
    P += 2;
  }

  // P = P + (2 * (D18F2x[1,0]88[Tcl] clocks - 1))
  P += 2 * (NBPtr->DCTPtr->Timings.CasL - 1);

  // If (DisCutThroughMode==0)
  // then P = P + 3
  // else P = P + 7
  if (MemNGetBitFieldNb (NBPtr, BFDisCutThroughMode) == 0) {
    P += 3;
  } else {
    P += 7;
  }

  return (((P * MemClkPeriod + 1) / 2) + T);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets up phy power saving for client NB
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNPhyPowerSavingClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  // 4. Program D18F2x[1,0]9C_x0D0F_0[F,7:0]13[DllDisEarlyU] = 1b.
  // 5. Program D18F2x[1,0]9C_x0D0F_0[F,7:0]13[DllDisEarlyL] = 1b.
  // 6. Program D18F2x[1,0]9C_x0D0F_0[F,7:0]13[7:4] = 1010b.
  MemNSetBitFieldNb (NBPtr, BFPhy0x0D0F0F13Bit0to7, 0xA3);
  // 7. Program D18F2x[1,0]9C_x0D0F_812F[7, 5, 0] = {1b, 1b, 1b} to disable unused PAR and A[17:16] pins.
  MemNSetBitFieldNb (NBPtr, BFAddrCmdTri, 0xA1);
  // 8. Program D18F2x[1,0]9C_x0D0F_C000[LowPowerDrvStrengthEn] = 1.
  MemNSetBitFieldNb (NBPtr, BFLowPowerDrvStrengthEn, 0x100);
  // 9. Program D18F2x[1,0]9C_x0D0F_0[F,7:0]10[EnRxPadStandby]= IF (D18F2x[1,0]94[MemClkFreq] <=
  //    800 MHz) THEN 1 ELSE 0 ENDIF.
  MemNSetBitFieldNb (NBPtr, BFEnRxPadStandby, (NBPtr->DCTPtr->Timings.Speed <= DDR1600_FREQUENCY) ? 0x1000 : 0);
  // 10. Program D18F2x[1,0]9C_x0000_000D as follows:
  //    TxMaxDurDllNoLock/RxMaxDurDllNoLock = 9h.
  MemNSetBitFieldNb (NBPtr, BFRxMaxDurDllNoLock, 9);
  MemNSetBitFieldNb (NBPtr, BFTxMaxDurDllNoLock, 9);
  //    TxCPUpdPeriod/RxCPUpdPeriod = 000b.
  MemNSetBitFieldNb (NBPtr, BFTxCPUpdPeriod, 0);
  MemNSetBitFieldNb (NBPtr, BFRxCPUpdPeriod, 0);
  //    TxDLLWakeupTime/RxDLLWakeupTime = 11b.
  MemNSetBitFieldNb (NBPtr, BFTxDLLWakeupTime, 3);
  MemNSetBitFieldNb (NBPtr, BFRxDLLWakeupTime, 3);
}