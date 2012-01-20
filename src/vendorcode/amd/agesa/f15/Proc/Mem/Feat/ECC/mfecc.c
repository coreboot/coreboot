/* $NoKeywords:$ */
/**
 * @file
 *
 * mfecc.c
 *
 * Feature ECC initialization functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/ECC)
 * @e \$Revision: 54543 $ @e \$Date: 2011-06-08 23:19:25 -0600 (Wed, 08 Jun 2011) $
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

#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mfecc.h"
#include "Filecode.h"
#include "mfmemclr.h"
#include "GeneralServices.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_ECC_MFECC_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

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
InitECCOverriedeStruct (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   ECC_OVERRIDE_STRUCT *pecc_override_struct
  );

BOOLEAN
MemFCheckECC (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */

extern BUILD_OPT_CFG UserOptions;

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function checks to see if ECC can be enabled on all nodes
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFCheckECC (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  DIE_STRUCT *MCTPtr;
  MEM_SHARED_DATA *SharedPtr;
  BOOLEAN ErrorRecovery;

  ASSERT (NBPtr != NULL);

  MCTPtr = NBPtr->MCTPtr;
  SharedPtr = NBPtr->SharedPtr;

  ErrorRecovery = TRUE;
  IDS_OPTION_HOOK (IDS_MEM_ERROR_RECOVERY, &ErrorRecovery, &NBPtr->MemPtr->StdHeader);

  if (MCTPtr->NodeMemSize != 0) {
    if (SharedPtr->AllECC && MCTPtr->Status[SbEccDimms] && (ErrorRecovery || (MCTPtr->ErrCode < AGESA_ERROR))) {
      // Clear all MCA reports before using scrubber
      // to initialize ECC check bits
      //
      NBPtr->McaNbCtlReg = NBPtr->GetBitField (NBPtr, BFMcaNbCtlReg);
      NBPtr->SetBitField (NBPtr, BFMcaNbCtlReg, 0);
      NBPtr->SetBitField (NBPtr, BFSyncOnUcEccEn, 0);
      // In unganged mode, set DctDctIntlv
      if (!NBPtr->Ganged) {
        NBPtr->SetBitField (NBPtr, BFDctDatIntLv, 1);
      }
      //
      // Set Ecc Symbol Size
      //
      NBPtr->SetEccSymbolSize (NBPtr);
      // If ECC can be enabled on this node,
      // set the master ECCen bit (according to setup)
      //
      NBPtr->SetBitField (NBPtr, BFDramEccEn, 1);
      // Do mem clear on current node
      MemFMctMemClr_Init (NBPtr);
      return TRUE;
    } else {
      if (SharedPtr->AllECC) {
        SharedPtr->AllECC = FALSE;
      }
      // ECC requested but cannot be enabled
      MCTPtr->Status[SbEccDimms] = FALSE;
      MCTPtr->ErrStatus[EsbDramECCDis] = TRUE;
      PutEventLog (AGESA_WARNING, MEM_WARNING_ECC_DIS, NBPtr->Node, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_WARNING, MCTPtr);
    }
  }
  return FALSE;
}

  /* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function initializes the ECC on all nodes
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFInitECC (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Node;
  UINT32 ScrubAddrRJ16;
  DIE_STRUCT *MCTPtr;
  MEM_SHARED_DATA *SharedPtr;
  ECC_OVERRIDE_STRUCT  ecc_override_struct;
  BOOLEAN Flag;

  InitECCOverriedeStruct (NBPtr, &ecc_override_struct);
  IDS_OPTION_HOOK (IDS_ECC, &ecc_override_struct, &(NBPtr->MemPtr->StdHeader));

  ASSERT (NBPtr != NULL);

  MCTPtr = NBPtr->MCTPtr;
  Node = MCTPtr->NodeId;
  SharedPtr = NBPtr->SharedPtr;
  Flag = TRUE;

  NBPtr->FamilySpecificHook[ScrubberErratum] (NBPtr, (VOID *) &Flag);

  if ((MCTPtr->Status[SbEccDimms]) && (SharedPtr->AllECC)) {
    // Check if the input dram scrub rate is supported or not
    ASSERT (ecc_override_struct.CfgScrubDramRate <= 0x16);
    if (ecc_override_struct.CfgScrubDramRate != 0) {
    // Program scrub address,
    // let the scrub Addr be the Base of this Node
    // Only enable Dram scrubber when there is memory on current node
    //
      NBPtr->SetBitField (NBPtr, BFScrubReDirEn, 0);
      ScrubAddrRJ16 = (NBPtr->GetBitField (NBPtr, BFDramBaseReg0 + Node) & 0xFFFF0000) >> 8;
      ScrubAddrRJ16 |= NBPtr->GetBitField (NBPtr, BFDramBaseHiReg0 + Node) << 24;
      NBPtr->SetBitField (NBPtr, BFScrubAddrLoReg, ScrubAddrRJ16 << 16);
      NBPtr->SetBitField (NBPtr, BFScrubAddrHiReg, ScrubAddrRJ16 >> 16);
      NBPtr->SetBitField (NBPtr, BFDramScrub, ecc_override_struct.CfgScrubDramRate);
    }
  }
  // Scrub CTL for Dcache, L2, L3
  // Check if the input L2 scrub rate is supported or not
  ASSERT (ecc_override_struct.CfgScrubL2Rate <= 0x16);
  NBPtr->SetBitField (NBPtr, BFL2Scrub, ecc_override_struct.CfgScrubL2Rate);
  // Check if the input Dcache scrub rate is supported or not
  ASSERT (ecc_override_struct.CfgScrubDcRate <= 0x16);
  NBPtr->SetBitField (NBPtr, BFDcacheScrub, ecc_override_struct.CfgScrubDcRate);
  // Do not enable L3 Scrub if F3xE8[L3Capable] is 0 or F3x188[BFReserved00B] is 1
  if ((NBPtr->GetBitField (NBPtr, BFL3Capable) == 1) && (NBPtr->GetBitField (NBPtr, BFReserved00B) == 0)) {
    // Check if input L3 scrub rate is supported or not
    ASSERT (ecc_override_struct.CfgScrubL3Rate <= 0x16);
    NBPtr->SetBitField (NBPtr, BFL3Scrub, ecc_override_struct.CfgScrubL3Rate);
  }

  // Check if Dcache scrubber or L2 scrubber is enabled
  if ((ecc_override_struct.CfgScrubL2Rate != 0) || (ecc_override_struct.CfgScrubDcRate!= 0)) {
    // If ClkDivisor is deeper than divide-by-16
    if (NBPtr->GetBitField (NBPtr, BFC1ClkDivisor) > 4) {
      // Set it to divide-by-16
      NBPtr->SetBitField (NBPtr, BFC1ClkDivisor, 4);
    }
  }

  NBPtr->SetBitField (NBPtr, BFScrubReDirEn, ecc_override_struct.CfgEccRedirection);
  NBPtr->SetBitField (NBPtr, BFSyncOnUcEccEn, ecc_override_struct.CfgEccSyncFlood);
  // Restore MCA reports after scrubber is done
  // with initializing ECC check bits
  NBPtr->SetBitField (NBPtr, BFMcaNbCtlReg, NBPtr->McaNbCtlReg);

  Flag = FALSE;
  NBPtr->FamilySpecificHook[ScrubberErratum] (NBPtr, (VOID *) &Flag);

  return TRUE;
}

VOID
STATIC
InitECCOverriedeStruct (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   ECC_OVERRIDE_STRUCT *pecc_override_struct
  )
{
  //
  // If (D18F3x44[DramEccEn]==1) THEN 1 ELSE 0 ENDIF
  //
  if (NBPtr->GetBitField (NBPtr, BFDramEccEn) == 1) {
    pecc_override_struct->CfgEccRedirection = 1;
  } else {
    pecc_override_struct->CfgEccRedirection = 0;
  }

  pecc_override_struct->CfgEccSyncFlood = UserOptions.CfgEccSyncFlood;
  pecc_override_struct->CfgScrubDcRate = UserOptions.CfgScrubDcRate;

  if (UserOptions.CfgScrubDramRate != 0xFF) {
    pecc_override_struct->CfgScrubDramRate = UserOptions.CfgScrubDramRate;
  } else {
    if (NBPtr->MCTPtr->NodeMemSize <= 0x4000) {
      pecc_override_struct->CfgScrubDramRate = 0x12; // 1 ~ 1 GB
    } else if (NBPtr->MCTPtr->NodeMemSize <= 0x8000) {
      pecc_override_struct->CfgScrubDramRate = 0x11; // 1 GB + 1 ~ 2 GB
    } else if (NBPtr->MCTPtr->NodeMemSize <= 0x10000) {
      pecc_override_struct->CfgScrubDramRate = 0x10; // 2 GB + 1 ~ 4 GB
    } else if (NBPtr->MCTPtr->NodeMemSize <= 0x20000) {
      pecc_override_struct->CfgScrubDramRate = 0x0F; // 4 GB + 1 ~ 8 GB
    } else if (NBPtr->MCTPtr->NodeMemSize <= 0x40000) {
      pecc_override_struct->CfgScrubDramRate = 0x0E; // 8 GB + 1 ~ 16 GB
    } else {
      pecc_override_struct->CfgScrubDramRate = 0x0D; //16 GB + 1 above
    }
  }

  pecc_override_struct->CfgScrubL2Rate = UserOptions.CfgScrubL2Rate;
  pecc_override_struct->CfgScrubL3Rate = UserOptions.CfgScrubL3Rate;
}
