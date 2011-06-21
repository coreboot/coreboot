/* $NoKeywords:$ */
/**
 * @file
 *
 * mndctdr.c
 *
 * Northbridge DR DCT supporting functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/DR)
 * @e \$Revision: 36462 $ @e \$Date: 2010-08-20 00:49:49 +0800 (Fri, 20 Aug 2010) $
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
#include "cpuFamRegisters.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mu.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mndr.h"
#include "merrhdl.h"
#include "OptionMemory.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_DR_MNDCTDR_FILECODE
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
MemTCtlOnDimmMirrorDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BOOLEAN SetFlag
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
memNAutoConfigDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  MEM_PARAMETER_STRUCT *RefPtr;

  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;
  DCTPtr = NBPtr->DCTPtr;
  //======================================================================
  // Build Dram Control Register Value (F2x78)
  //======================================================================
  //

  MemNSetBitFieldNb (NBPtr, BFRdPtrInit, 6);
  MemNSetBitFieldNb (NBPtr, BFDataTxFifoWrDly, 0);

  MemNSetBitFieldNb (NBPtr, BFEarlyArbEn, 1);

  //======================================================================
  // Build Dram Config Lo Register Value
  //======================================================================
  //


  if (MCTPtr->Status[SbParDimms]) {
    //
    // SbParDimms should be set for all DDR3 RDIMMS or DDR2 that support parity
    // Cannot turn off ParEn for DDR3
    //
    //@attention - add debug option for parity control
    if ((MemNGetBitFieldNb (NBPtr, BFDdr3Mode)!= 0) || (RefPtr->EnableParity)) {
      MemNSetBitFieldNb (NBPtr, BFParEn, 1);
    }
  }

  if (MCTPtr->GangedMode) {
    MemNSetBitFieldNb (NBPtr, BFWidth128, 1);
  }

  MemNSetBitFieldNb (NBPtr, BFX4Dimm, DCTPtr->Timings.Dimmx4Present & 0xF);

  if (!MCTPtr->Status[SbRegistered]) {
    MemNSetBitFieldNb (NBPtr, BFUnBuffDimm, 1);
  }

  if (MCTPtr->Status[SbEccDimms]) {
    MemNSetBitFieldNb (NBPtr, BFDimmEccEn, 1);
  }

  //======================================================================
  // Build Dram Config Hi Register Value
  //======================================================================
  //

  MemNSetBitFieldNb (NBPtr, BFMemClkFreq, MemNGetMemClkFreqIdNb (NBPtr, DCTPtr->Timings.Speed));

  if (MCTPtr->Status[SbRegistered]) {
    if (DCTPtr->Timings.Dimmx4Present && DCTPtr->Timings.Dimmx8Present) {
      MemNSetBitFieldNb (NBPtr, BFRDqsEn, 1);
    }
  }

  if (RefPtr->EnableBankSwizzle) {
    MemNSetBitFieldNb (NBPtr, BFBankSwizzleMode, 1);
  }

  if (DCTPtr->Timings.DimmQrPresent) {
    if (UserOptions.CfgMemoryQuadrankType == QUADRANK_UNBUFFERED) {
      MemNSetBitFieldNb (NBPtr, BFFourRankSoDimm, 1);
    } else if (UserOptions.CfgMemoryQuadrankType == QUADRANK_REGISTERED) {
      MemNSetBitFieldNb (NBPtr, BFFourRankRDimm, 1);
    }
  }

  MemNSetBitFieldNb (NBPtr, BFDcqBypassMax, 0xF);

  MemNSetBitFieldNb (NBPtr, BFDcqArbBypassEn, 1);

  //======================================================================
  // Build Dram Config Misc Register Value
  //======================================================================
  //
  MemNSetBitFieldNb (NBPtr, BFOdtSwizzle, 1);
  // For DDR3 Registered Dimms
  if (MCTPtr->Status[SbRegistered]) {
    if (MemNGetBitFieldNb (NBPtr, BFDdr3Mode)!= 0) {
      MemNSetBitFieldNb (NBPtr, BFSubMemclkRegDly, 1);
    }
  }
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
 *
 *   This is a general purpose function that executes before DRAM init
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNBeforeDramInitDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sends an MRS command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSendMrsCmdDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  BOOLEAN ClearODM;

  ClearODM = FALSE;
  if ((NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F10_C0) != 0) {
    if (MemNGetBitFieldNb (NBPtr, BFEnDramInit) == 0) {
      // For C0, if EnDramInit bit is cleared, ODM needs to be cleared before sending MRS
      MemTCtlOnDimmMirrorDr (NBPtr, FALSE);
      ClearODM = TRUE;
    }
  }

  MemNSwapBitsNb (NBPtr);

  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tCS%d MR%d %04x\n",
              (MemNGetBitFieldNb (NBPtr, BFDramInitRegReg) >> 20) & 0xF,
              (MemNGetBitFieldNb (NBPtr, BFDramInitRegReg) >> 16) & 0xF,
              (MemNGetBitFieldNb (NBPtr, BFDramInitRegReg) & 0xFFFF));

  // 1.Set SendMrsCmd=1
  MemNSetBitFieldNb (NBPtr, BFSendMrsCmd, 1);

  // 2.Wait for SendMrsCmd=0
  MemNPollBitFieldNb (NBPtr, BFSendMrsCmd, 0, PCI_ACCESS_TIMEOUT, FALSE);

  if (ClearODM) {
    // Restore ODM if necessary
    MemTCtlOnDimmMirrorDr (NBPtr, TRUE);
  }
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   Workaround for erratum 322 and 263
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNBeforePlatformSpecDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  // Errata 263
  if ((NBPtr->DCTPtr->Timings.Speed == DDR533_FREQUENCY) || (NBPtr->DCTPtr->Timings.Speed == DDR667_FREQUENCY)) {
    MemNSetBitFieldNb (NBPtr, BFErr263, 0x0800);
  } else {
    MemNSetBitFieldNb (NBPtr, BFErr263, 0);
  }

  // Errata 322
  // 1.Write 00000000h to F2x[1,0]9C_xD08E000
  MemNSetBitFieldNb (NBPtr, BFErr322I, 0);
  // 2.If DRAM Configuration Register[MemClkFreq] (F2x[1,0]94[2:0]) is
  //    greater than or equal to 011b (DDR-800 and higher),
  //    then write 00000080h to F2x[1,0]9C_xD02E001,
  //    else write 00000090h to F2x[1,0]9C_xD02E001.
  MemNSetBitFieldNb (NBPtr, BFErr322II, (NBPtr->DCTPtr->Timings.Speed >= DDR800_FREQUENCY) ? 0x80 : 0x90);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function disables/enables F2x[1, 0][5C:40][OnDimmMirror]
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   SetFlag   - Enable or disable flag - TRUE - Enable, FALSE - DISABLE
 *
 */

VOID
STATIC
MemTCtlOnDimmMirrorDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BOOLEAN SetFlag
  )
{
  UINT8 Chipsel;
  UINT32 CSBaseAddrReg;

  for (Chipsel = 0; Chipsel < MAX_CS_PER_CHANNEL; Chipsel += 2) {
    CSBaseAddrReg = MemNGetBitFieldNb (NBPtr, BFCSBaseAddr1Reg + Chipsel);
    if ((CSBaseAddrReg & 1) == 1) {
      if (SetFlag && ((NBPtr->DCTPtr->Timings.DimmMirrorPresent & ((UINT8) 1 << (Chipsel >> 1))) != 0)) {
        CSBaseAddrReg |= ((UINT32) 1 << BFOnDimmMirror);
      } else {
        CSBaseAddrReg &= ~((UINT32) 1 << BFOnDimmMirror);
      }
      MemNSetBitFieldNb (NBPtr, BFCSBaseAddr1Reg + Chipsel, CSBaseAddrReg);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function adjusts Avg PRE value of Phy fence training according to specific CPU family.
 *
 *     @param[in,out]   *NBPtr  - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *Value16 - Pointer to the value that we want to adjust
 *
 */

VOID
MemNPFenceAdjustDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   UINT16 *Value16
  )
{
  if ((NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F10_C0) != 0) {
    *Value16 += 5; //for RB C0, the Avg PRE value is subtracted by 3 only.
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function uses calculated values from DCT.Timings structure to
 *      program its RB registers.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNProgramCycTimingsDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CTENTRY DDR3TmgAdjTab[] = {
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

  CTENTRY DDR2TmgAdjTab[] = {
    // BitField, Min, Max, Bias, Ratio_x2
    {BFTcl, 3, 7, 1, 2},
    {BFTrcd, 3, 6, 3, 2},
    {BFTrp, 3, 6, 3, 4},
    {BFTrtp, 2, 3, 2, 4},
    {BFTras, 5, 18, 3, 2},
    {BFTrc, 11, 26, 11, 2},
    {BFTwr, 3, 6, 3, 2},
    {BFTrrd, 2, 5, 2, 2},
    {BFTwtr, 5, 8, 4, 2},
    {BFFourActWindow, 8, 20, 7, 2}
  };

  CTENTRY *TmgAdjTab;
  DCT_STRUCT *DCTPtr;
  UINT8  *MiniMaxTmg;
  UINT8  *MiniMaxTrfc;
  UINT8  Value8;
  UINT8  j;
  UINT8  BurstMode;
  BIT_FIELD_NAME BitField;

  DCTPtr = NBPtr->DCTPtr;

  //======================================================================
  // Program DRAM Timing values
  //======================================================================
  //
  if (MemNGetBitFieldNb (NBPtr, BFDdr3Mode) == 1) {
    TmgAdjTab = DDR3TmgAdjTab;
  } else if (DCTPtr->Timings.Speed == DDR1066_FREQUENCY) {
    // DDR2-1066 uses DDR3 table but differs in tCL
    TmgAdjTab = DDR3TmgAdjTab;
    TmgAdjTab[0].Min = 3;
    TmgAdjTab[0].Max = 7;
    TmgAdjTab[0].Bias = 1;
  } else {
    TmgAdjTab = DDR2TmgAdjTab;
  }

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

  MemNSetBitFieldNb (NBPtr, BFTref, 2);      // 7.8 us

  if (MemNGetBitFieldNb (NBPtr, BFDdr3Mode) == 1) {
    //======================================================================
    // DDR3 additional settings
    //======================================================================

    MemNSetBitFieldNb (NBPtr, BFTcwl, ((DCTPtr->Timings.Speed >= DDR800_FREQUENCY) ?
                                       (NBPtr->GetMemClkFreqId (NBPtr, DCTPtr->Timings.Speed) - 3) : 0));

    MemNSetBitFieldNb (NBPtr, BFNonSPD, 0x28FF);

    MemNSetBitFieldNb (NBPtr, BFNonSPDHi, 0x2A);

    // DrvImpCtrl: drive impedance control.01b(34 ohm driver; Ron34 = Rzq/7)
    MemNSetBitFieldNb (NBPtr, BFDrvImpCtrl, 1);

    // burst length control
    if (NBPtr->MCTPtr->Status[Sb128bitmode]) {
      MemNSetBitFieldNb (NBPtr, BFBurstCtrl, 2);
    }

    // ASR=1, auto self refresh; SRT=0
    MemNSetBitFieldNb (NBPtr, BFASR, 1);
  } else {
    //======================================================================
    // DDR2 additional settings
    //======================================================================

    MemNSetBitFieldNb (NBPtr, BFNonSPD, 0xFF77);

    BurstMode = UserOptions.CfgUseBurstMode;
    IDS_OPTION_HOOK (IDS_BURST_LENGTH32, &BurstMode, &(NBPtr->MemPtr->StdHeader));
    if (BurstMode) {
      if (!NBPtr->MCTPtr->GangedMode) {
        NBPtr->SetBitField (NBPtr, BFBurstLength32, 1);
      }
    }
  }
}

