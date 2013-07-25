/* $NoKeywords:$ */
/**
 * @file
 *
 * mfRdWr2DKb.c
 *
 * KB - Specific funtion for 2D Read and write training feature
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/RdWr2DTraining/Kb)
 * @e \$Revision: 84150 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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
#include "AdvancedApi.h"
#include "GeneralServices.h"
#include "Ids.h"
#include "heapManager.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mport.h"
#include "merrhdl.h"
#include "OptionMemory.h"
#include "mfRdWr2DTraining.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_FEAT_RDWR2DTRAINING_KB_MFRDWR2DKB_FILECODE
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

BOOLEAN
MemFRdWr2DTrainingInitKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
STATIC
MemFRdWr2DProgramVrefKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *VrefPtr
  );

BOOLEAN
STATIC
MemFRdWr2DScaleVrefKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Vref
  );

BOOLEAN
STATIC
MemFRdWr2DProgramIntExtVrefSelectKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
STATIC
MemFRdWr2DProgramDataPatternKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID* PatternIndexPtr
  );

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initializes the 2D Read/Write Training Feature Hooks for KB
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *
 *     @return         BOOLEAN
 *                             TRUE - Function was implemented
 *
 */

BOOLEAN
MemFRdWr2DTrainingInitKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  ASSERT (NBPtr != NULL);
  NBPtr->FamilySpecificHook[RdWr2DTraining] = MemFAmdRdWr2DTraining;
  NBPtr->FamilySpecificHook[CheckRdWr2DTrainingPerConfig] = MemFCheckRdWr2DTrainingPerConfig;
  NBPtr->FamilySpecificHook[RdWr2DSelectIntExtVref] = MemFRdWr2DProgramIntExtVrefSelectKB;
  NBPtr->FamilySpecificHook[RdWr2DProgramVref] = MemFRdWr2DProgramVrefKB;
  NBPtr->FamilySpecificHook[RdWr2DScaleVref] = MemFRdWr2DScaleVrefKB;
  NBPtr->FamilySpecificHook[RdWr2DProgramDelays] = MemFRdWr2DProgramDelays;
  NBPtr->FamilySpecificHook[RdWr2DDataCollection] = MemFRdWr2DEyeRimSearch;
  NBPtr->FamilySpecificHook[RdWr2DInitVictim] = MemFRdWr2DInitVictim;
  NBPtr->FamilySpecificHook[RdWr2DInitVictimChipSel] = MemFRdWr2DInitVictimChipSel;
  NBPtr->FamilySpecificHook[RdWr2DStartVictim] = MemFRdWr2DStartVictim;
  NBPtr->FamilySpecificHook[RdWr2DFinalizeVictim] = MemFRdWr2DFinalizeVictim;
  NBPtr->FamilySpecificHook[RdWr2DCompareInPhase] = MemFRdWr2DCompareInPhase;
  NBPtr->FamilySpecificHook[RdWr2DCompare180Phase] = MemFRdWr2DCompare180Phase;
  NBPtr->FamilySpecificHook[RdWr2DProgramDataPattern] = MemFRdWr2DProgramDataPatternKB;
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs Vref for 2D Read/Write Training
 *
 *     @param[in,out] *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *VrefPtr - Pointer to Vref value
 *
 *     @return         BOOLEAN
 *                               TRUE - Success
 *                               FAIL (External Callout only)
 *
 */
BOOLEAN
STATIC
MemFRdWr2DProgramVrefKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *VrefPtr
  )
{
  AGESA_STATUS Status;
  MEM_DATA_STRUCT *MemPtr;
  ID_INFO CallOutIdInfo;
  VOLTAGE_ADJUST Va;
  UINT8 Vref;

  ASSERT (NBPtr != NULL);
  ASSERT (VrefPtr != NULL);
  MemPtr = NBPtr->MemPtr;
  Vref = *(UINT8*)VrefPtr;
  CallOutIdInfo.IdField.SocketId = NBPtr->MCTPtr->SocketId;
  CallOutIdInfo.IdField.ModuleId = NBPtr->MCTPtr->DieId;
  LibAmdMemCopy ((VOID *)&Va, (VOID *)MemPtr, (UINTN)sizeof (Va.StdHeader), &MemPtr->StdHeader);
  Va.MemData = MemPtr;
  Status = AGESA_SUCCESS;
  if (NBPtr->TechPtr->Direction == DQS_READ_DIR) {
    if (NBPtr->RefPtr->ExternalVrefCtl == FALSE) {
      //
      // Internal vref control
      //
      ASSERT (Vref < 61);
      if (Vref < 30) {
        Vref = (62 - Vref);
      } else {
        Vref = (Vref - 30);
      }
      NBPtr->SetBitField (NBPtr, BFVrefDAC, Vref << 3);
    } else {
      //
      // External vref control
      //
      AGESA_TESTPOINT (TpProcMemBefore2dTrainExtVrefChange, &(NBPtr->MemPtr->StdHeader));
      NBPtr->MemPtr->ParameterListPtr->ExternalVrefValue = Vref;
      IDS_HDT_CONSOLE (MEM_FLOW, "\n2D Read Training External CPU Vref Callout \n");
      Va.VoltageType = VTYPE_CPU_VREF;
      Va.AdjustValue = Vref = (Vref - 15) << 1;
      Status = AgesaExternalVoltageAdjust ((UINTN)CallOutIdInfo.IdInformation, &Va);
      AGESA_TESTPOINT (TpProcMemAfter2dTrainExtVrefChange, &(NBPtr->MemPtr->StdHeader));
    }
  } else {
    //
    // DIMM Vref Control
    //
    Va.VoltageType = VTYPE_DIMM_VREF;
    //
    // Offset by 15 and multiply by 2.
    //
    Va.AdjustValue = Vref = (Vref - 15) << 1;
    Status = AgesaExternalVoltageAdjust ((UINTN)CallOutIdInfo.IdInformation, &Va);
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tDimm Vref = %c%d%    ", (Va.AdjustValue < 0) ? '-':'+', (Va.AdjustValue < 0) ? (0 - Va.AdjustValue) : Va.AdjustValue );
    if (Status != AGESA_SUCCESS) {
      IDS_HDT_CONSOLE (MEM_FLOW, "* Dimm Vref Callout Failed *");
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
  }
  return (Status == AGESA_SUCCESS) ? TRUE : FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function scales Vref from the range used in Data Collection to
 *     the range that is programmed into the register.
 *
 *     @param[in,out] *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] *Vref  - Pointer to UINT8 Vref Value to scale.
 *
 *     @return       BOOLEAN
 *                             TRUE Function was implemented
 *
 */
BOOLEAN
STATIC
MemFRdWr2DScaleVrefKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Vref
  )
{
  *(UINT8*)Vref = *(UINT8*)Vref * 2;
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs Vref to internal or external control for 2D Read
 *     or Write Training
 *
 *     @param[in,out] *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] *OptParam - Unused
 *
 *     @return          BOOLEAN
 *                                TRUE -  External Vref was selected
 *                                FALSE - Internal Vref was selected
 *
 */
BOOLEAN
STATIC
MemFRdWr2DProgramIntExtVrefSelectKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  if (NBPtr->TechPtr->Direction == DQS_READ_DIR) {
    NBPtr->SetBitField (NBPtr, BFVrefSel, (NBPtr->RefPtr->ExternalVrefCtl ? 0x0002 : 0x0001));
  }
  return NBPtr->RefPtr->ExternalVrefCtl;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs the Data Pattern that will be sent and compared
 *     against.
 *
 *     @param[in,out] *NBPtr           - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *PatternIndexPtr - Pointer to a generic index used to
 *                                         determine which pattern to program.
 *
 *     @return                 BOOLEAN
 *                                       TRUE
 *
 */
BOOLEAN
STATIC
MemFRdWr2DProgramDataPatternKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID* PatternIndexPtr
  )
{
  UINT8 SeedCount;
  UINT32 PrbsSeed;
  CONST STATIC UINT32 CmdStreamLenTbl[4] = {13, 61, 127, 251};

  ASSERT (NBPtr != 0);
  ASSERT (PatternIndexPtr != NULL);
  SeedCount = *(UINT8*)PatternIndexPtr;
  ASSERT (SeedCount <= (NBPtr->MaxSeedCount - 1));
  MemNSetBitFieldNb (NBPtr, BFCmdStreamLen, CmdStreamLenTbl[SeedCount]);
  PrbsSeed = 0x7EA05;
  switch (SeedCount) {
  case 0:
    MemNSetBitFieldNb (NBPtr, BFDataPatGenSel, 0x01);
    PrbsSeed = 0x7FFFF;
    break;
  case 1:
    MemNSetBitFieldNb (NBPtr, BFDataPatGenSel, 0x04);
    MemNSetBitFieldNb (NBPtr, BFXorPatOvr, 0xFF);
    PrbsSeed = 0x7EA05;
    break;
  case 2:
    MemNSetBitFieldNb (NBPtr, BFDataPatGenSel, 0x03);
    MemNSetBitFieldNb (NBPtr, BFDramUserDataPattern0, 0x55555549);
    MemNSetBitFieldNb (NBPtr, BFDramUserDataPattern1, 0x55555555);
    MemNSetBitFieldNb (NBPtr, BFDramUserDataPattern2, 0x55555555);
    MemNSetBitFieldNb (NBPtr, BFDramUserDataPattern3, 0x55555555);
    break;
  case 3:
    MemNSetBitFieldNb (NBPtr, BFDataPatGenSel, 0x03);
    MemNSetBitFieldNb (NBPtr, BFDramUserDataPattern0, 0xA5A5A55A);
    MemNSetBitFieldNb (NBPtr, BFDramUserDataPattern1, 0xA5A5A5A5);
    MemNSetBitFieldNb (NBPtr, BFDramUserDataPattern2, 0xA5A5A5A5);
    MemNSetBitFieldNb (NBPtr, BFDramUserDataPattern3, 0xA5A5A5A5);
    break;
  default:
    ASSERT (FALSE);
  }
  ASSERT (PrbsSeed != 0);
  //
  // Program the PRBS Seed
  //
  NBPtr->SetBitField (NBPtr, BFDataPrbsSeed, PrbsSeed);
  return TRUE;
}
