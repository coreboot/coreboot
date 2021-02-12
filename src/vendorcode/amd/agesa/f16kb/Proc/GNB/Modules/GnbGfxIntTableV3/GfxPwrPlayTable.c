/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to initialize Integrated Info Table
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 67269 $   @e \$Date: 2012-03-26 02:53:08 -0500 (Mon, 26 Mar 2012) $
 *
 */
/*
*****************************************************************************
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


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbF1Table.h"
#include  "GnbPcie.h"
#include  "GnbGfx.h"
#include  "GnbGfxFamServices.h"
#include  "GnbCommonLib.h"
#include  "GfxPwrPlayTable.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBGFXINTTABLEV3_GFXPWRPLAYTABLE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/// Software state
typedef struct {
  BOOLEAN     Valid;                                  ///< State valid
  UINT16      Classification;                         ///< State classification
  UINT32      CapsAndSettings;                        ///< State capability and settings
  UINT16      Classification2;                        ///< State classification2
  UINT32      SW_STATE_fld4;
  UINT32      SW_STATE_fld5;
  UINT8       SW_STATE_fld6;
  UINT8       SW_STATE_fld7[10];
} SW_STATE;

typedef struct {
  BOOLEAN     Valid;
  UINT32      GfxPwrPlayTable120_STRUCT_fld1;
  UINT8       Vid;
  UINT16      Tdp;
} GfxPwrPlayTable120_STRUCT;

typedef struct {
  GFX_PLATFORM_CONFIG                       *Gfx;
  ATOM_PPLIB_POWERPLAYTABLE4                *PpTable;
  PP_F1_ARRAY_V2                          *PpF1s;
  SW_STATE                                  SwStateArray [MAX_NUM_OF_SW_STATES];          ///< SW state array
  GfxPwrPlayTable120_STRUCT                                 PP_WORKSPACE_V2_fld4[10];
  UINT8                                     NumOfClockVoltageLimitEnties;                 ///
  ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_RECORD VceClockVoltageLimitArray[MAX_NUM_OF_VCE_CLK_STATES];
  UINT8                                     NumOfVceClockEnties;
  GfxPwrPlayTable204_STRUCT                              VceClockInfoArray[MAX_NUM_OF_VCE_CLK_STATES];
  UINT8                                     NumOfVceStateEntries;
  ATOM_PPLIB_VCE_STATE_RECORD               VceStateArray[MAX_NUM_OF_VCE_STATES];         ///< VCE state array
  UINT8                                     NumOfUvdClkVoltLimitEntries;                  ///
  ATOM_PPLIB_UVD_CLK_VOLT_LIMIT_RECORD      UvdClkVoltLimitArray[MAX_NUM_OF_UVD_CLK_STATES];
  UINT8                                     NumOfUvdClockEntries;
  GfxPwrPlayTable261_STRUCT                 UvdClockInfoArray[MAX_NUM_OF_UVD_CLK_STATES];
  UINT8                                     PP_WORKSPACE_V2_fld15;                  ///
  ATOM_PPLIB_SAMCLK_VOLT_LIMIT_RECORD       PP_WORKSPACE_V2_fld16[MAX_NUM_OF_SAMCLK_STATES];
  UINT8                                     PP_WORKSPACE_V2_fld17;                      ///
  GfxPwrPlayTable310_STRUCT     PP_WORKSPACE_V2_fld18[5];
} PP_WORKSPACE_V2;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
GfxIntDebugDumpPpTable (
  IN       ATOM_PPLIB_POWERPLAYTABLE4   *PpTable,
  IN       GFX_PLATFORM_CONFIG          *Gfx
  );


/*----------------------------------------------------------------------------------------*/
/**
 * Create new software state
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 * @retval                           Pointer to state entry in SW state array
 */

STATIC SW_STATE *
GfxPwrPlayCreateSwState (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace
  )
{
  UINTN Index;
  for (Index = 0; Index < MAX_NUM_OF_SW_STATES; Index++) {
    if (PpWorkspace->SwStateArray[Index].Valid == FALSE) {
      PpWorkspace->SwStateArray[Index].Valid = TRUE;
      return  &(PpWorkspace->SwStateArray[Index]);
    }
  }
  return NULL;
}

/*----------------------------------------------------------------------------------------*/

STATIC UINT8
GfxPwrPlayTable192_fun (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace,
  IN       UINT32                        fv1,
  IN       UINT8                         Vid
  )
{
  UINT8 Index;

  for (Index = 0; Index < 10; Index++) {
    if (PpWorkspace->PP_WORKSPACE_V2_fld4[Index].Valid == FALSE) {
      PpWorkspace->PP_WORKSPACE_V2_fld4[Index].GfxPwrPlayTable120_STRUCT_fld1 = fv1;
      PpWorkspace->PP_WORKSPACE_V2_fld4[Index].Vid = Vid;
      PpWorkspace->PP_WORKSPACE_V2_fld4[Index].Valid = TRUE;
      PpWorkspace->PP_WORKSPACE_V2_fld4[Index].Tdp = 0;
      return Index;
    }
  }
  return 0;
}

/*----------------------------------------------------------------------------------------*/

STATIC UINT8
GfxPwrPlayTable224_fun (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace,
  IN       UINT32                        fv1,
  IN       UINT8                         Vid
  )
{
  UINT8 Index;

  for (Index = 0; Index < 10; Index++) {
    if (PpWorkspace->PP_WORKSPACE_V2_fld4[Index].Valid &&
        fv1 == PpWorkspace->PP_WORKSPACE_V2_fld4[Index].GfxPwrPlayTable120_STRUCT_fld1 &&
        Vid == PpWorkspace->PP_WORKSPACE_V2_fld4[Index].Vid) {

      return Index;
    }
  }

  Index = GfxPwrPlayTable192_fun (PpWorkspace, fv1, Vid);

  return Index;
}


STATIC VOID
GfxPwrPlayTable256_fun (
  IN OUT   SW_STATE                      *SwStateArray,
  IN       UINT8                         DpmStateIndex
  )
{
  SwStateArray->SW_STATE_fld7[SwStateArray->SW_STATE_fld6++] = DpmStateIndex;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Copy SW state info to PPTable
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */
STATIC VOID *
GfxPwrPlayAttachStateInfoBlock (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace
  )
{
  UINT8                 Index;
  UINT8                 SwStateIndex;
  STATE_ARRAY           *StateArray;
  ATOM_PPLIB_STATE_V2   *States;
  StateArray = (STATE_ARRAY *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  States = &StateArray->States[0];
  SwStateIndex = 0;
  for (Index = 0; Index < MAX_NUM_OF_SW_STATES; Index++) {
    if (PpWorkspace->SwStateArray[Index].Valid && PpWorkspace->SwStateArray[Index].SW_STATE_fld6 != 0) {
      States->nonClockInfoIndex  = SwStateIndex;
      States->ATOM_PPLIB_STATE_V2_fld0 = PpWorkspace->SwStateArray[Index].SW_STATE_fld6;
      LibAmdMemCopy (
        &States->ClockInfoIndex[0],
        PpWorkspace->SwStateArray[Index].SW_STATE_fld7,
        PpWorkspace->SwStateArray[Index].SW_STATE_fld6,
        GnbLibGetHeader (PpWorkspace->Gfx)
        );
      States = (ATOM_PPLIB_STATE_V2*) ((UINT8*) States + sizeof (ATOM_PPLIB_STATE_V2) + sizeof (UINT8) * (States->ATOM_PPLIB_STATE_V2_fld0 - 1));
      SwStateIndex++;
    }
  }
  StateArray->ucNumEntries = SwStateIndex;
  PpWorkspace->PpTable->sHeader.usStructureSize = PpWorkspace->PpTable->sHeader.usStructureSize + (USHORT) ((UINT8 *) States - (UINT8 *) StateArray);
  return StateArray;
}
/*----------------------------------------------------------------------------------------*/
/**
 * Copy clock info to PPTable
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPwrPlayAttachClockInfoBlock (
  IN OUT   PP_WORKSPACE_V2              *PpWorkspace
  )
{
  CLOCK_INFO_ARRAY            *ClockInfoArray;
  UINT8                       Index;
  UINT8                       ClkStateIndex;
  ClkStateIndex = 0;
  ClockInfoArray = (CLOCK_INFO_ARRAY *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  for (Index = 0; Index < 10; Index++) {
    if (PpWorkspace->PP_WORKSPACE_V2_fld4[Index].Valid == TRUE) {
      ClockInfoArray->ClockInfo[ClkStateIndex].ucEngineClockHigh = (UINT8) (PpWorkspace->PP_WORKSPACE_V2_fld4[Index].GfxPwrPlayTable120_STRUCT_fld1  >> 16);
      ClockInfoArray->ClockInfo[ClkStateIndex].usEngineClockLow = (UINT16) (PpWorkspace->PP_WORKSPACE_V2_fld4[Index].GfxPwrPlayTable120_STRUCT_fld1);
      ClockInfoArray->ClockInfo[ClkStateIndex].vddcIndex = PpWorkspace->PP_WORKSPACE_V2_fld4[Index].Vid;
      ClockInfoArray->ClockInfo[ClkStateIndex].ATOM_PPLIB_SUMO_CLOCK_INFO_fld3 = PpWorkspace->PP_WORKSPACE_V2_fld4[Index].Tdp;
      ClkStateIndex++;
    }
  }
  ClockInfoArray->ucNumEntries = ClkStateIndex;
  ClockInfoArray->ucEntrySize = sizeof (GfxPwrPlayTable143_STRUCT);
  PpWorkspace->PpTable->sHeader.usStructureSize += sizeof (CLOCK_INFO_ARRAY) + sizeof (GfxPwrPlayTable143_STRUCT) * ClkStateIndex - sizeof (GfxPwrPlayTable143_STRUCT);
  return ClockInfoArray;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Copy non clock info to PPTable
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPwrPlayAttachNonClockInfoBlock (
  IN OUT   PP_WORKSPACE_V2              *PpWorkspace
  )
{
  NON_CLOCK_INFO_ARRAY  *NonClockInfoArray;
  UINT8                 Index;
  UINT8                 NonClkStateIndex;

  NonClockInfoArray = (NON_CLOCK_INFO_ARRAY *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  NonClkStateIndex = 0;
  for (Index = 0; Index < MAX_NUM_OF_SW_STATES; Index++) {
    if (PpWorkspace->SwStateArray[Index].Valid && PpWorkspace->SwStateArray[Index].SW_STATE_fld6 != 0) {
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].usClassification = PpWorkspace->SwStateArray[Index].Classification;
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].ulCapsAndSettings = PpWorkspace->SwStateArray[Index].CapsAndSettings;
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].usClassification2 = PpWorkspace->SwStateArray[Index].Classification2;
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].ATOM_PPLIB_NONCLOCK_INFO_fld7 = PpWorkspace->SwStateArray[Index].SW_STATE_fld5;
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].ATOM_PPLIB_NONCLOCK_INFO_fld6 = PpWorkspace->SwStateArray[Index].SW_STATE_fld4;
      NonClkStateIndex++;
    }
  }
  NonClockInfoArray->ucNumEntries = NonClkStateIndex;
  NonClockInfoArray->ucEntrySize = sizeof (ATOM_PPLIB_NONCLOCK_INFO);
  PpWorkspace->PpTable->sHeader.usStructureSize += sizeof (NON_CLOCK_INFO_ARRAY) + sizeof (ATOM_PPLIB_NONCLOCK_INFO) * NonClkStateIndex - sizeof (ATOM_PPLIB_NONCLOCK_INFO);
  return NonClockInfoArray;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check if  state valid
 *
 *
 * @param[out]      Index             State index
 * @param[in]       PpF1s           Pointer
 * @param[in]       Gfx               Gfx configuration info
 * @retval          TRUE              State is valid
 */
STATIC BOOLEAN
GfxPwrPlayIsF1dStateValid (
  IN      UINT8                         Index,
  IN      PP_F1_ARRAY_V2              *PpF1s,
  IN      GFX_PLATFORM_CONFIG           *Gfx
  )
{
  BOOLEAN Result;
  Result = FALSE;
  if ((PpF1s->PP_FUSE_ARRAY_V2_fld37 & (1 << Index)) || (PpF1s->PP_FUSE_ARRAY_V2_fld38 & (1 << Index))) {
    Result = TRUE;
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/

STATIC VOID
GfxPwrPlayTable437_fun (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace
  )
{
  UINT8                       ClkStateIndex;
  UINT8                       DpmF1Index;
  UINT32                      fv2;
  SW_STATE                    *State;
  PP_F1_ARRAY_V2            *PpF1s;

  PpF1s = PpWorkspace->PpF1s;

  // Create Battery state
  State = GfxPwrPlayCreateSwState (PpWorkspace);

  State->Classification = ATOM_PPLIB_CLASSIFICATION_UI_BATTERY;
  State->Classification2 = 0;
  State->SW_STATE_fld4 = 0;
  State->SW_STATE_fld5 = 0;
  if (PpWorkspace->Gfx->AbmSupport != 0) {
    State->CapsAndSettings |= ATOM_PPLIB_ENABLE_VARIBRIGHT;
  }
  if (PpWorkspace->Gfx->DynamicRefreshRate != 0) {
    State->CapsAndSettings |= ATOM_PPLIB_ENABLE_DRR;
  }

  for (DpmF1Index = 0; DpmF1Index < 5; DpmF1Index++) {

    if (PpF1s->PP_FUSE_ARRAY_V2_fld38 & (1 << DpmF1Index)) {

      fv2 = (PpF1s->PP_FUSE_ARRAY_V2_fld33[DpmF1Index] != 0) ?
             GfxFmCalculateClock (PpF1s->PP_FUSE_ARRAY_V2_fld33[DpmF1Index],
                                  GnbLibGetHeader (PpWorkspace->Gfx)) : 0;

      if (fv2 != 0) {
        ClkStateIndex = GfxPwrPlayTable224_fun (PpWorkspace, fv2, PpF1s->PP_FUSE_ARRAY_V2_fld32[DpmF1Index]);
        GfxPwrPlayTable256_fun (State, ClkStateIndex);
      }
    }
  }

  // Create Performance state
  State = GfxPwrPlayCreateSwState (PpWorkspace);

  State->Classification = ATOM_PPLIB_CLASSIFICATION_UI_PERFORMANCE;
  State->Classification2 = 0;
  State->SW_STATE_fld4 = 0;
  State->SW_STATE_fld5 = 0;

  // Loop through fused DPM states and find those that go with Performance
  for (DpmF1Index = 0; DpmF1Index < 5; DpmF1Index++) {

    if (PpF1s->PP_FUSE_ARRAY_V2_fld37 & (1 << DpmF1Index)) {

      fv2 = (PpF1s->PP_FUSE_ARRAY_V2_fld33[DpmF1Index] != 0) ?
             GfxFmCalculateClock (PpF1s->PP_FUSE_ARRAY_V2_fld33[DpmF1Index],
                                  GnbLibGetHeader (PpWorkspace->Gfx)) : 0;

      if (fv2 != 0) {
        ClkStateIndex = GfxPwrPlayTable224_fun (PpWorkspace, fv2, PpF1s->PP_FUSE_ARRAY_V2_fld32[DpmF1Index]);
        GfxPwrPlayTable256_fun (State, ClkStateIndex);
      }
    }
  }

  // Create Boot State
  State = GfxPwrPlayCreateSwState (PpWorkspace);
  State->Classification = ATOM_PPLIB_CLASSIFICATION_BOOT;
  fv2 = 200 * 100;
  ClkStateIndex = GfxPwrPlayTable224_fun (PpWorkspace, fv2, 0);
  GfxPwrPlayTable256_fun (State, ClkStateIndex);

}


/*----------------------------------------------------------------------------------------*/

STATIC UINT8
GfxPwrPlayAddEclkState (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace,
  IN       UINT32                        Eclk
  )
{
  UINT8   Index;
  USHORT  EclkLow;
  UCHAR   EclkHigh;
  EclkLow = (USHORT) (Eclk & 0xffff);
  EclkHigh = (UCHAR) (Eclk >> 16);
  for (Index = 0; Index < PpWorkspace->NumOfVceClockEnties; Index++) {
    if (PpWorkspace->VceClockInfoArray[Index].ucECClkHigh == EclkHigh && PpWorkspace->VceClockInfoArray[Index].usECClkLow == EclkLow) {
      return Index;
    }
  }
  PpWorkspace->VceClockInfoArray[PpWorkspace->NumOfVceClockEnties].ucECClkHigh = EclkHigh;
  PpWorkspace->VceClockInfoArray[PpWorkspace->NumOfVceClockEnties].usECClkLow = EclkLow;
  PpWorkspace->VceClockInfoArray[PpWorkspace->NumOfVceClockEnties].GfxPwrPlayTable204_STRUCT_fld1 = EclkHigh;
  PpWorkspace->VceClockInfoArray[PpWorkspace->NumOfVceClockEnties].GfxPwrPlayTable204_STRUCT_fld0 = EclkLow;
  return PpWorkspace->NumOfVceClockEnties++;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Add ECLK state
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 * @param[in]       EclkIndex        ECLK index
 * @param[in]      Vid              Vid index
 * @retval                           Index of state entry in Eclk Voltage record array
 */

STATIC UINT8
GfxPwrPlayAddEclkVoltageRecord (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace,
  IN       UINT8                         EclkIndex,
  IN       UINT8                         Vid
  )
{
  UINT8   Index;
  for (Index = 0; Index < PpWorkspace->NumOfClockVoltageLimitEnties; Index++) {
    if (PpWorkspace->VceClockVoltageLimitArray[Index].ucVCEClockInfoIndex == EclkIndex) {
      return Index;
    }
  }
  PpWorkspace->VceClockVoltageLimitArray[PpWorkspace->NumOfClockVoltageLimitEnties].ucVCEClockInfoIndex = EclkIndex;
  PpWorkspace->VceClockVoltageLimitArray[PpWorkspace->NumOfClockVoltageLimitEnties].usVoltage = Vid;
  return PpWorkspace->NumOfClockVoltageLimitEnties++;
}


/*----------------------------------------------------------------------------------------*/

STATIC UINT8
GfxPwrPlayTable588_fun (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace,
  IN       UINT32                        fv1,
  IN       UINT32                        fv2
  )
{
  UINT8   Index;
  USHORT  v1;
  UCHAR   v2;
  USHORT  v3;
  UCHAR   v4;
  v1 = (USHORT) (fv1 & 0xffff);
  v2 = (UCHAR) (fv1 >> 16);
  v3 = (USHORT) (fv2 & 0xffff);
  v4 = (UCHAR) (fv2 >> 16);
  for (Index = 0; Index < PpWorkspace->NumOfUvdClockEntries; Index++) {
    if (PpWorkspace->UvdClockInfoArray[Index].GfxPwrPlayTable261_STRUCT_fld1 == v2 &&
        PpWorkspace->UvdClockInfoArray[Index].GfxPwrPlayTable261_STRUCT_fld0 == v1) {
      return Index;
    }
  }
  PpWorkspace->UvdClockInfoArray[PpWorkspace->NumOfUvdClockEntries].GfxPwrPlayTable261_STRUCT_fld1 = v2;
  PpWorkspace->UvdClockInfoArray[PpWorkspace->NumOfUvdClockEntries].GfxPwrPlayTable261_STRUCT_fld0 = v1;
  PpWorkspace->UvdClockInfoArray[PpWorkspace->NumOfUvdClockEntries].GfxPwrPlayTable261_STRUCT_fld3 = v4;
  PpWorkspace->UvdClockInfoArray[PpWorkspace->NumOfUvdClockEntries].GfxPwrPlayTable261_STRUCT_fld2 = v3;
  return PpWorkspace->NumOfUvdClockEntries++;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Add Uvd voltage record
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 * @param[in]       ClkIndex         CLK index
 * @param[in]       Vid              Vid index
 * @retval                           Index of state entry in Eclk Voltage record array
 */

STATIC UINT8
GfxPwrPlayAddUvdVoltageRecord (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace,
  IN       UINT8                         ClkIndex,
  IN       UINT8                         Vid
  )
{
  UINT8   Index;
  for (Index = 0; Index < PpWorkspace->NumOfUvdClkVoltLimitEntries; Index++) {
    if (PpWorkspace->UvdClkVoltLimitArray[Index].ucUVDClockInfoIndex == ClkIndex) {
      return Index;
    }
  }
  PpWorkspace->UvdClkVoltLimitArray[PpWorkspace->NumOfUvdClkVoltLimitEntries].ucUVDClockInfoIndex =
      ClkIndex;
  PpWorkspace->UvdClkVoltLimitArray[PpWorkspace->NumOfUvdClkVoltLimitEntries].usVoltage = Vid;
  return PpWorkspace->NumOfUvdClkVoltLimitEntries++;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Add Samu voltage record
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 * @param[in]       Vid              Vid
 * @param[in]       Samclk           CLK associated with the Vid
 * @retval                           Index of state entry in Voltage record array
 */

STATIC UINT8
GfxPwrPlayAddSamuVoltageRecord (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace,
  IN       UINT8                         Vid,
  IN       UINT32                        Samclk
  )
{
  UINT8   Index;
  USHORT  SamclkLow;
  UCHAR   SamclkHigh;
  SamclkLow = (USHORT) (Samclk & 0xffff);
  SamclkHigh = (UCHAR) (Samclk >> 16);
  for (Index = 0; Index < PpWorkspace->PP_WORKSPACE_V2_fld15; Index++) {
    if ((PpWorkspace->PP_WORKSPACE_V2_fld16[Index].usSAMClockHigh == SamclkHigh) &&
        (PpWorkspace->PP_WORKSPACE_V2_fld16[Index].usSAMClockLow == SamclkLow) &&
        (PpWorkspace->PP_WORKSPACE_V2_fld16[Index].usVoltage == Vid)
        ) {
      return Index;
    }
  }
  PpWorkspace->PP_WORKSPACE_V2_fld16[PpWorkspace->PP_WORKSPACE_V2_fld15].usSAMClockHigh =
      SamclkHigh;
  PpWorkspace->PP_WORKSPACE_V2_fld16[PpWorkspace->PP_WORKSPACE_V2_fld15].usSAMClockLow =
      SamclkLow;
  PpWorkspace->PP_WORKSPACE_V2_fld16[PpWorkspace->PP_WORKSPACE_V2_fld15].usVoltage = Vid;
  return PpWorkspace->PP_WORKSPACE_V2_fld15++;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Attach extended header
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPwrPlayAttachExtendedHeaderBlock (
  IN OUT   PP_WORKSPACE_V2              *PpWorkspace
  )
{
  ATOM_PPLIB_EXTENDEDHEADER *ExtendedHeader;
  ExtendedHeader = (ATOM_PPLIB_EXTENDEDHEADER *)
      ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  ExtendedHeader->usSize = sizeof (ATOM_PPLIB_EXTENDEDHEADER);
  PpWorkspace->PpTable->sHeader.usStructureSize += sizeof (ATOM_PPLIB_EXTENDEDHEADER);
  return ExtendedHeader;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Attach Vce Rev Block
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPwrPlayAttachVceTableRevBlock (
  IN OUT   PP_WORKSPACE_V2              *PpWorkspace
  )
{
  ATOM_PPLIB_VCE_TABLE  *VceTable;
  VceTable = (ATOM_PPLIB_VCE_TABLE *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  VceTable->revid = 0;
  PpWorkspace->PpTable->sHeader.usStructureSize += sizeof (ATOM_PPLIB_VCE_TABLE);
  return VceTable;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Attach VCE clock info block
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPwrPlayAttachVceClockInfoBlock (
  IN OUT   PP_WORKSPACE_V2              *PpWorkspace
  )
{
  VCECLOCKINFOARRAY   *VceClockInfoArray;
  VceClockInfoArray = (VCECLOCKINFOARRAY *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  VceClockInfoArray->ucNumEntries = PpWorkspace->NumOfVceClockEnties;
  LibAmdMemCopy (
    &VceClockInfoArray->entries[0],
    &PpWorkspace->VceClockInfoArray[0],
    VceClockInfoArray->ucNumEntries * sizeof (GfxPwrPlayTable204_STRUCT),
    GnbLibGetHeader (PpWorkspace->Gfx)
    );
  PpWorkspace->PpTable->sHeader.usStructureSize = PpWorkspace->PpTable->sHeader.usStructureSize +
                                                  sizeof (VCECLOCKINFOARRAY) +
                                                  VceClockInfoArray->ucNumEntries * sizeof (GfxPwrPlayTable204_STRUCT) -
                                                  sizeof (GfxPwrPlayTable204_STRUCT);
  return VceClockInfoArray;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Attach VCE voltage limit block
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPwrPlayAttachVceVoltageLimitBlock (
  IN OUT   PP_WORKSPACE_V2              *PpWorkspace
  )
{
  ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_TABLE  *VceClockVoltageLimitTable;
  VceClockVoltageLimitTable = (ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_TABLE *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  VceClockVoltageLimitTable->numEntries = PpWorkspace->NumOfClockVoltageLimitEnties;
  LibAmdMemCopy (
    &VceClockVoltageLimitTable->entries[0],
    &PpWorkspace->VceClockVoltageLimitArray[0],
    VceClockVoltageLimitTable->numEntries * sizeof (ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_RECORD),
    GnbLibGetHeader (PpWorkspace->Gfx)
    );
  PpWorkspace->PpTable->sHeader.usStructureSize =
      PpWorkspace->PpTable->sHeader.usStructureSize +
      sizeof (ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_TABLE) +
      VceClockVoltageLimitTable->numEntries * sizeof (ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_RECORD) -
      sizeof (ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_RECORD);
  return VceClockVoltageLimitTable;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Attach VCE state block
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPwrPlayAttachVceStateTableBlock (
  IN OUT   PP_WORKSPACE_V2              *PpWorkspace
  )
{
  ATOM_PPLIB_VCE_STATE_TABLE  *VceStateTable;
  VceStateTable = (ATOM_PPLIB_VCE_STATE_TABLE *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  VceStateTable->numEntries = PpWorkspace->NumOfVceStateEntries;
  LibAmdMemCopy (
    &VceStateTable->entries[0],
    &PpWorkspace->VceStateArray[0],
    VceStateTable->numEntries * sizeof (ATOM_PPLIB_VCE_STATE_RECORD),
    GnbLibGetHeader (PpWorkspace->Gfx)
    );
  PpWorkspace->PpTable->sHeader.usStructureSize = PpWorkspace->PpTable->sHeader.usStructureSize +
                                                  sizeof (ATOM_PPLIB_VCE_STATE_TABLE) +
                                                  VceStateTable->numEntries * sizeof (ATOM_PPLIB_VCE_STATE_RECORD) -
                                                  sizeof (ATOM_PPLIB_VCE_STATE_RECORD);
  return VceStateTable;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Attach Uvd Rev Block
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPwrPlayAttachUvdTableRevBlock (
  IN OUT   PP_WORKSPACE_V2              *PpWorkspace
  )
{
  ATOM_PPLIB_UVD_TABLE  *UvdTable;
  UvdTable = (ATOM_PPLIB_UVD_TABLE *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  UvdTable->revid = 0;
  PpWorkspace->PpTable->sHeader.usStructureSize += sizeof (ATOM_PPLIB_UVD_TABLE);
  return UvdTable;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Attach UVD clock info block
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPwrPlayAttachUvdClockInfoBlock (
  IN OUT   PP_WORKSPACE_V2              *PpWorkspace
  )
{
  GfxPwrPlayTable267_STRUCT   *UvdClockInfoArray;
  UvdClockInfoArray = (GfxPwrPlayTable267_STRUCT *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  UvdClockInfoArray->ucNumEntries = PpWorkspace->NumOfUvdClockEntries;
  LibAmdMemCopy (
    &UvdClockInfoArray->entries[0],
    &PpWorkspace->UvdClockInfoArray[0],
    UvdClockInfoArray->ucNumEntries * sizeof (GfxPwrPlayTable261_STRUCT),
    GnbLibGetHeader (PpWorkspace->Gfx)
    );
  PpWorkspace->PpTable->sHeader.usStructureSize = PpWorkspace->PpTable->sHeader.usStructureSize +
                                                  sizeof (GfxPwrPlayTable267_STRUCT) +
                                                  UvdClockInfoArray->ucNumEntries * sizeof (GfxPwrPlayTable261_STRUCT) -
                                                  sizeof (GfxPwrPlayTable261_STRUCT);
  return UvdClockInfoArray;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Attach UVD voltage limit block
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPwrPlayAttachUvdVoltageLimitBlock (
  IN OUT   PP_WORKSPACE_V2              *PpWorkspace
  )
{
  UVD_CLK_VOLT_LIMIT_TABLE  *UvdClockVoltageLimitTable;
  UvdClockVoltageLimitTable = (UVD_CLK_VOLT_LIMIT_TABLE *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  UvdClockVoltageLimitTable->numEntries = PpWorkspace->NumOfUvdClkVoltLimitEntries;
  LibAmdMemCopy (
    &UvdClockVoltageLimitTable->entries[0],
    &PpWorkspace->UvdClkVoltLimitArray[0],
    UvdClockVoltageLimitTable->numEntries * sizeof (ATOM_PPLIB_UVD_CLK_VOLT_LIMIT_RECORD),
    GnbLibGetHeader (PpWorkspace->Gfx)
    );
  PpWorkspace->PpTable->sHeader.usStructureSize =
      PpWorkspace->PpTable->sHeader.usStructureSize +
      sizeof (UVD_CLK_VOLT_LIMIT_TABLE) +
      UvdClockVoltageLimitTable->numEntries * sizeof (ATOM_PPLIB_UVD_CLK_VOLT_LIMIT_RECORD) -
      sizeof (ATOM_PPLIB_UVD_CLK_VOLT_LIMIT_RECORD);
  return UvdClockVoltageLimitTable;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Attach SAMU Rev Block
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPwrPlayAttachSamuTableRevBlock (
  IN OUT   PP_WORKSPACE_V2              *PpWorkspace
  )
{
  ATOM_PPLIB_SAMU_TABLE  *VceTable;
  VceTable = (ATOM_PPLIB_SAMU_TABLE *) ((UINT8 *) PpWorkspace->PpTable +
      PpWorkspace->PpTable->sHeader.usStructureSize);
  VceTable->revid = 0;
  PpWorkspace->PpTable->sHeader.usStructureSize += sizeof (ATOM_PPLIB_SAMU_TABLE);
  return VceTable;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Attach SAMU voltage limit block
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPwrPlayAttachSamuVoltageLimitBlock (
  IN OUT   PP_WORKSPACE_V2              *PpWorkspace
  )
{
  ATOM_PPLIB_SAMCLK_VOLT_LIMIT_TABLE  *SamuClockVoltageLimitTable;
  SamuClockVoltageLimitTable = (ATOM_PPLIB_SAMCLK_VOLT_LIMIT_TABLE *)
    ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  SamuClockVoltageLimitTable->numEntries = PpWorkspace->PP_WORKSPACE_V2_fld15;
  LibAmdMemCopy (
    &SamuClockVoltageLimitTable->entries[0],
    &PpWorkspace->PP_WORKSPACE_V2_fld16[0],
    SamuClockVoltageLimitTable->numEntries * sizeof (ATOM_PPLIB_SAMCLK_VOLT_LIMIT_RECORD),
    GnbLibGetHeader (PpWorkspace->Gfx)
    );
  PpWorkspace->PpTable->sHeader.usStructureSize =
      PpWorkspace->PpTable->sHeader.usStructureSize +
      sizeof (ATOM_PPLIB_SAMCLK_VOLT_LIMIT_TABLE) +
      SamuClockVoltageLimitTable->numEntries * sizeof (ATOM_PPLIB_SAMCLK_VOLT_LIMIT_RECORD) -
      sizeof (ATOM_PPLIB_SAMCLK_VOLT_LIMIT_RECORD);
  return SamuClockVoltageLimitTable;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Attach Sclk Volt Dep Block
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPwrPlayTable956_fun (
  IN OUT   PP_WORKSPACE_V2              *PpWorkspace
  )
{
  GfxPwrPlayTable316_STRUCT  *v0;

  v0 = (GfxPwrPlayTable316_STRUCT *)
      ((UINT8 *) PpWorkspace->PpTable +
      PpWorkspace->PpTable->sHeader.usStructureSize);
  v0->numEntries = PpWorkspace->PP_WORKSPACE_V2_fld17;
  LibAmdMemCopy (
    &v0->entries[0],
    &PpWorkspace->PP_WORKSPACE_V2_fld18[0],
    5 * sizeof (GfxPwrPlayTable310_STRUCT),
    GnbLibGetHeader (PpWorkspace->Gfx)
    );
  PpWorkspace->PpTable->sHeader.usStructureSize =
      PpWorkspace->PpTable->sHeader.usStructureSize +
      sizeof (GfxPwrPlayTable316_STRUCT) +
      v0->numEntries * sizeof (GfxPwrPlayTable310_STRUCT) -
      sizeof (GfxPwrPlayTable310_STRUCT);


  return v0;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Build VCE state info
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID
GfxPwrPlayBuildVceStateTable (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace
  )
{
  UINT8                       Index;
  UINT8                       VceStateIndex;
  UINT8                       Vid;
  UINT32                      Eclk;
  UINT32                      v4;
  UINT8                       UsedStateBitmap;
  UsedStateBitmap = 0;
  // build used state
  for (Index = 0;
       Index < ARRAY_SIZE(PpWorkspace->PpF1s->VceFlags);
       Index++) {
    UsedStateBitmap |= PpWorkspace->PpF1s->VceFlags[Index];
    for (VceStateIndex = 0;
         VceStateIndex < ARRAY_SIZE(PpWorkspace->VceStateArray);
         VceStateIndex++) {
      if ((PpWorkspace->PpF1s->VceFlags[Index] & (1 << VceStateIndex)) != 0) {
        v4 = GfxFmCalculateClock (PpWorkspace->PpF1s->PP_FUSE_ARRAY_V2_fld33[PpWorkspace->PpF1s->PP_FUSE_ARRAY_V2_fld16[Index]],
                                    GnbLibGetHeader (PpWorkspace->Gfx));
        Vid = PpWorkspace->PpF1s->PP_FUSE_ARRAY_V2_fld32[PpWorkspace->PpF1s->PP_FUSE_ARRAY_V2_fld16[Index]];
        PpWorkspace->VceStateArray[VceStateIndex].ucClockInfoIndex =
            GfxPwrPlayTable224_fun (PpWorkspace, v4, Vid);
        if (PpWorkspace->PpF1s->VceMclk) {
          PpWorkspace->VceStateArray[VceStateIndex].ucClockInfoIndex |=
              (PpWorkspace->PpF1s->VceMclk << 6);
        }
        Eclk = GfxFmCalculateClock (PpWorkspace->PpF1s->EclkDid[Index],
                                    GnbLibGetHeader (PpWorkspace->Gfx));
        PpWorkspace->VceStateArray[VceStateIndex].ucVCEClockInfoIndex =
            GfxPwrPlayAddEclkState (PpWorkspace, Eclk);
        GfxPwrPlayAddEclkVoltageRecord (PpWorkspace,
            PpWorkspace->VceStateArray[VceStateIndex].ucVCEClockInfoIndex, Vid);
        PpWorkspace->NumOfVceStateEntries++;
      }
    }
  }
  //build unused states
  for (VceStateIndex = 0;
       VceStateIndex < ARRAY_SIZE(PpWorkspace->VceStateArray);
       VceStateIndex++) {
    if ((UsedStateBitmap & (1 << VceStateIndex)) == 0) {
      PpWorkspace->VceStateArray[VceStateIndex].ucClockInfoIndex = 0;
      PpWorkspace->VceStateArray[VceStateIndex].ucVCEClockInfoIndex = GfxPwrPlayAddEclkState (PpWorkspace, 0);
      PpWorkspace->NumOfVceStateEntries++;
    }
  }
}

/*----------------------------------------------------------------------------------------*/

STATIC VOID
GfxPwrPlayBuildUvdClockTable (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace
  )
{
  UINT8                       Index;
  UINT8                       Vid;
  UINT32                      v2;
  UINT32                      v3;
  UINT8                       UsedStateBitmap;
  UINT8                       UvdIndex;

  UsedStateBitmap = 0;
  // build used state
  for (Index = 0; Index < MAX_NUM_OF_UVD_CLK_STATES ; Index++) {
    if (GfxPwrPlayIsF1dStateValid (Index, PpWorkspace->PpF1s, PpWorkspace->Gfx)) {
      Vid = PpWorkspace->PpF1s->PP_FUSE_ARRAY_V2_fld32[Index];
      v2 = GfxFmCalculateClock (PpWorkspace->PpF1s->PP_FUSE_ARRAY_V2_fld1[Index],
                                  GnbLibGetHeader (PpWorkspace->Gfx));
      v3 = GfxFmCalculateClock (PpWorkspace->PpF1s->PP_FUSE_ARRAY_V2_fld2[Index],
                                  GnbLibGetHeader (PpWorkspace->Gfx));
      UvdIndex = GfxPwrPlayTable588_fun (PpWorkspace, v2, v3);
      GfxPwrPlayAddUvdVoltageRecord (PpWorkspace,
          UvdIndex, Vid);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Build SAMU info
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID
GfxPwrPlayBuildSamuTable (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace
  )
{
  UINT8                       SamuIndex;
  UINT8                       Vid;
  UINT32                      Samuclk;
  UINT8                       UsedStateBitmap;
  UsedStateBitmap = 0;
  // build used state
  for (SamuIndex = 0; SamuIndex < MAX_NUM_OF_SAMCLK_STATES; SamuIndex++) {
    if (GfxPwrPlayIsF1dStateValid (SamuIndex, PpWorkspace->PpF1s, PpWorkspace->Gfx)) {
      Vid = PpWorkspace->PpF1s->PP_FUSE_ARRAY_V2_fld32[SamuIndex];
      Samuclk = GfxFmCalculateClock (PpWorkspace->PpF1s->PP_FUSE_ARRAY_V2_fld34[SamuIndex],
          GnbLibGetHeader (PpWorkspace->Gfx));
      GfxPwrPlayAddSamuVoltageRecord (PpWorkspace, Vid, Samuclk);
    }
  }
}

/*----------------------------------------------------------------------------------------*/

STATIC VOID
GfxPwrPlayTable1122_fun (
  IN OUT   PP_WORKSPACE_V2               *PpWorkspace
  )
{
  UINT8                       v0;
  UINT8                       Vid;
  UINT32                      v2;
  USHORT  v3;
  UCHAR   v4;

  // build the table
  for (v0 = 0; v0 < 5; v0++) {
    Vid = PpWorkspace->PpF1s->PP_FUSE_ARRAY_V2_fld32[v0];
    v2 = GfxFmCalculateClock (PpWorkspace->PpF1s->PP_FUSE_ARRAY_V2_fld33[v0],
        GnbLibGetHeader (PpWorkspace->Gfx));
    ASSERT (Vid != 0)
    ASSERT (v2 != 0)
    v3 = (USHORT) (v2 & 0xffff);
    v4 = (UCHAR) (v2 >> 16);
    PpWorkspace->PP_WORKSPACE_V2_fld18[v0].GfxPwrPlayTable310_fld2 = Vid;
    PpWorkspace->PP_WORKSPACE_V2_fld18[v0].GfxPwrPlayTable310_fld1 = v4;
    PpWorkspace->PP_WORKSPACE_V2_fld18[v0].GfxPwrPlayTable310_fld0 = v3;
    PpWorkspace->PP_WORKSPACE_V2_fld17++;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Build PP table
 *
 *
 * @param[out] Buffer           Buffer to create PP table
 * @param[in]  Gfx              Gfx configuration info
 * @retval     AGESA_SUCCESS
 * @retval     AGESA_ERROR
 */

AGESA_STATUS
GfxPwrPlayBuildTable (
     OUT   VOID                          *Buffer,
  IN       GFX_PLATFORM_CONFIG           *Gfx
  )
{
  PP_WORKSPACE_V2             PpWorkspace;
  VOID                        *BlockPtr;

  LibAmdMemFill (&PpWorkspace, 0x00, sizeof (PP_WORKSPACE_V2), GnbLibGetHeader (Gfx));
  PpWorkspace.PpF1s = GnbLocateHeapBuffer (AMD_PP_F1_TABLE_HANDLE, GnbLibGetHeader (Gfx));
  ASSERT (PpWorkspace.PpF1s != NULL);
  if (PpWorkspace.PpF1s == NULL) {
    return AGESA_ERROR;
  }

  PpWorkspace.PpTable = (ATOM_PPLIB_POWERPLAYTABLE4 *) Buffer;
  PpWorkspace.Gfx = Gfx;
  //Fill static info
  PpWorkspace.PpTable->sHeader.ucTableFormatRevision = 6;
  PpWorkspace.PpTable->sHeader.ucTableContentRevision = 1;
  PpWorkspace.PpTable->ucDataRevision = PpWorkspace.PpF1s->PPlayTableRev;
  PpWorkspace.PpTable->sThermalController.ucType = ATOM_PP_THERMALCONTROLLER_KV;
  PpWorkspace.PpTable->sThermalController.ucFanParameters = ATOM_PP_FANPARAMETERS_NOFAN;
  PpWorkspace.PpTable->sHeader.usStructureSize = sizeof (ATOM_PPLIB_POWERPLAYTABLE4);
  PpWorkspace.PpTable->usTableSize = sizeof (ATOM_PPLIB_POWERPLAYTABLE4);
  PpWorkspace.PpTable->usFormatID = 0x13;
  if ((Gfx->AmdPlatformType & AMD_PLATFORM_MOBILE) != 0) {
    PpWorkspace.PpTable->ulPlatformCaps |= ATOM_PP_PLATFORM_CAP_POWERPLAY;
  }


  GfxPwrPlayTable437_fun (&PpWorkspace);

  // Fill Eclk state info
  if (PpWorkspace.PpF1s->PP_FUSE_ARRAY_V2_fld13) {
    GfxPwrPlayBuildVceStateTable (&PpWorkspace);
    GfxPwrPlayBuildUvdClockTable (&PpWorkspace);
    GfxPwrPlayBuildSamuTable (&PpWorkspace);
    GfxPwrPlayTable1122_fun (&PpWorkspace);
  }

  //Copy state info to actual PP table
  BlockPtr = GfxPwrPlayAttachStateInfoBlock (&PpWorkspace);
  PpWorkspace.PpTable->usStateArrayOffset = (USHORT) ((UINT8 *) BlockPtr - (UINT8 *) (PpWorkspace.PpTable));
  BlockPtr = GfxPwrPlayAttachClockInfoBlock (&PpWorkspace);
  PpWorkspace.PpTable->usClockInfoArrayOffset = (USHORT) ((UINT8 *) BlockPtr - (UINT8 *) (PpWorkspace.PpTable));
  BlockPtr = GfxPwrPlayAttachNonClockInfoBlock (&PpWorkspace);
  PpWorkspace.PpTable->usNonClockInfoArrayOffset = (USHORT) ((UINT8 *) BlockPtr - (UINT8 *) (PpWorkspace.PpTable));

  if (PpWorkspace.PpF1s->PP_FUSE_ARRAY_V2_fld13) {
    ATOM_PPLIB_EXTENDEDHEADER   *ExtendedHeader;
    ExtendedHeader =
        (ATOM_PPLIB_EXTENDEDHEADER *) GfxPwrPlayAttachExtendedHeaderBlock (&PpWorkspace);
    PpWorkspace.PpTable->usExtendendedHeaderOffset =
        (USHORT) ((UINT8 *) ExtendedHeader - (UINT8 *) (PpWorkspace.PpTable));
    BlockPtr = GfxPwrPlayAttachVceTableRevBlock (&PpWorkspace);
    ExtendedHeader->usVCETableOffset =
        (USHORT) ((UINT8 *) BlockPtr - (UINT8 *) (PpWorkspace.PpTable));
    GfxPwrPlayAttachVceClockInfoBlock (&PpWorkspace);
    GfxPwrPlayAttachVceVoltageLimitBlock (&PpWorkspace);
    GfxPwrPlayAttachVceStateTableBlock (&PpWorkspace);

    BlockPtr = GfxPwrPlayAttachUvdTableRevBlock (&PpWorkspace);
    ExtendedHeader->usUVDTableOffset =
        (USHORT) ((UINT8 *) BlockPtr - (UINT8 *) (PpWorkspace.PpTable));
    GfxPwrPlayAttachUvdClockInfoBlock (&PpWorkspace);
    GfxPwrPlayAttachUvdVoltageLimitBlock (&PpWorkspace);

    BlockPtr = GfxPwrPlayAttachSamuTableRevBlock (&PpWorkspace);
    ExtendedHeader->usSAMUTableOffset =
        (USHORT) ((UINT8 *) BlockPtr - (UINT8 *) (PpWorkspace.PpTable));
    GfxPwrPlayAttachSamuVoltageLimitBlock (&PpWorkspace);

    BlockPtr = GfxPwrPlayTable956_fun (&PpWorkspace);
    PpWorkspace.PpTable->ATOM_PPLIB_POWERPLAYTABLE4_fld17 =
        (USHORT) ((UINT8 *) BlockPtr - (UINT8 *) (PpWorkspace.PpTable));

    IDS_HDT_CONSOLE (GNB_TRACE, "ExtendedHeader \n");
    IDS_HDT_CONSOLE (GNB_TRACE, "   VceTableOffset = %04x\n", ExtendedHeader->usVCETableOffset);
    IDS_HDT_CONSOLE (GNB_TRACE, "   UvdTableOffset = %04x\n", ExtendedHeader->usUVDTableOffset);
    IDS_HDT_CONSOLE (GNB_TRACE, "   SamTableOffset = %04x\n", ExtendedHeader->usSAMUTableOffset);
    IDS_HDT_CONSOLE (GNB_TRACE, "\n");

  }
  GNB_DEBUG_CODE (
    GfxIntDebugDumpPpTable (PpWorkspace.PpTable, Gfx);
  );

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Dump PP table
 *
 *
 *
 * @param[in] PpTable           Power Play table
 * @param[in] Gfx               Gfx configuration info
 */

VOID
GfxIntDebugDumpPpTable (
  IN       ATOM_PPLIB_POWERPLAYTABLE4   *PpTable,
  IN       GFX_PLATFORM_CONFIG          *Gfx
  )
{
  UINTN                                     Index;
  STATE_ARRAY                               *StateArray;
  ATOM_PPLIB_STATE_V2                       *StatesPtr;
  NON_CLOCK_INFO_ARRAY                      *NonClockInfoArrayPtr;
  CLOCK_INFO_ARRAY                          *ClockInfoArrayPtr;
  ATOM_PPLIB_EXTENDEDHEADER                 *ExtendedHeader;
  ATOM_PPLIB_VCE_STATE_TABLE                *VceStateTable;
  ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_TABLE  *VceClockVoltageLimitTable;
  VCECLOCKINFOARRAY                         *VceClockInfoArray;
  GfxPwrPlayTable267_STRUCT                      *UvdClockInfoArray;
  UVD_CLK_VOLT_LIMIT_TABLE                  *UvdClockVoltLimitTable;
  ATOM_PPLIB_SAMCLK_VOLT_LIMIT_TABLE        *SamuClockVoltLimitTable;
  UINT8                                     EclkIndex;

  IDS_HDT_CONSOLE (GFX_MISC, "  < --- Power Play Table ------ > \n");
  IDS_HDT_CONSOLE (GFX_MISC, "  Table Revision = %d\n", PpTable->ucDataRevision);
  StateArray = (STATE_ARRAY *) ((UINT8 *) PpTable + PpTable->usStateArrayOffset);
  StatesPtr = StateArray->States;
  NonClockInfoArrayPtr = (NON_CLOCK_INFO_ARRAY *) ((UINT8 *) PpTable + PpTable->usNonClockInfoArrayOffset);
  ClockInfoArrayPtr = (CLOCK_INFO_ARRAY *) ((UINT8 *) PpTable + PpTable->usClockInfoArrayOffset);
  IDS_HDT_CONSOLE (GFX_MISC, "  < --- SW State Table ---------> \n");
  for (Index = 0; Index < StateArray->ucNumEntries; Index++) {
    IDS_HDT_CONSOLE (GFX_MISC, "  State #%d\n", Index + 1
      );
    IDS_HDT_CONSOLE (GFX_MISC, "    Classification 0x%x\n",
       NonClockInfoArrayPtr->NonClockInfo[StatesPtr->nonClockInfoIndex].usClassification
      );
    IDS_HDT_CONSOLE (GFX_MISC, "    Classification2 0x%x\n",
       NonClockInfoArrayPtr->NonClockInfo[StatesPtr->nonClockInfoIndex].usClassification2
      );
    IDS_HDT_CONSOLE (GFX_MISC, "\n");
    StatesPtr = (ATOM_PPLIB_STATE_V2 *) ((UINT8 *) StatesPtr + sizeof (ATOM_PPLIB_STATE_V2) + StatesPtr->ATOM_PPLIB_STATE_V2_fld0 - 1);
  }
  if (PpTable->usExtendendedHeaderOffset != 0) {
    ExtendedHeader = (ATOM_PPLIB_EXTENDEDHEADER *) ((UINT8 *) PpTable +
      PpTable->usExtendendedHeaderOffset);

    IDS_HDT_CONSOLE (GNB_TRACE, "ExtendedHeader = %08x\n", ExtendedHeader);

    VceClockInfoArray = (VCECLOCKINFOARRAY *) ((UINT8 *) PpTable +
        ExtendedHeader->usVCETableOffset + sizeof (ATOM_PPLIB_VCE_TABLE));
    VceClockVoltageLimitTable =
        (ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_TABLE *) ((UINT8 *) VceClockInfoArray +
        sizeof (VCECLOCKINFOARRAY) +
        VceClockInfoArray->ucNumEntries * sizeof (GfxPwrPlayTable204_STRUCT) -
        sizeof (GfxPwrPlayTable204_STRUCT));
    VceStateTable =
        (ATOM_PPLIB_VCE_STATE_TABLE *) ((UINT8 *) VceClockVoltageLimitTable +
        sizeof (ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_TABLE) +
        VceClockVoltageLimitTable->numEntries * sizeof (ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_RECORD) -
        sizeof (ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_RECORD));
    UvdClockInfoArray =
        (GfxPwrPlayTable267_STRUCT *) ((UINT8 *) PpTable + ExtendedHeader->usUVDTableOffset +
         sizeof (ATOM_PPLIB_UVD_TABLE));
    UvdClockVoltLimitTable =
        (UVD_CLK_VOLT_LIMIT_TABLE *) ((UINT8 *) UvdClockInfoArray + sizeof (GfxPwrPlayTable267_STRUCT) +
        UvdClockInfoArray->ucNumEntries * sizeof (GfxPwrPlayTable261_STRUCT) -
        sizeof (GfxPwrPlayTable261_STRUCT));
    SamuClockVoltLimitTable =
        (ATOM_PPLIB_SAMCLK_VOLT_LIMIT_TABLE *)
        ((UINT8 *) PpTable + ExtendedHeader->usSAMUTableOffset + sizeof (ATOM_PPLIB_SAMU_TABLE));

    IDS_HDT_CONSOLE (GFX_MISC, "  < --- VCE State Table [%d]--> \n", VceStateTable->numEntries);

    IDS_HDT_CONSOLE (GFX_MISC, "  < --- VCE Voltage Record Table ---> \n");
    for (Index = 0; Index < VceClockVoltageLimitTable->numEntries; Index++) {
      EclkIndex = VceClockVoltageLimitTable->entries[Index].ucVCEClockInfoIndex;
      IDS_HDT_CONSOLE (GFX_MISC, "  VCE Voltage Record #%d\n", Index
        );
      IDS_HDT_CONSOLE (GFX_MISC, "    ECLK = %d\n",
        VceClockInfoArray->entries[EclkIndex].usECClkLow | (VceClockInfoArray->entries[EclkIndex].ucECClkHigh << 16)
        );
      IDS_HDT_CONSOLE (GFX_MISC, "    VID index = %d\n",
        VceClockVoltageLimitTable->entries[Index].usVoltage
        );
    }


    IDS_HDT_CONSOLE (GFX_MISC, "  < --- SAMU Voltage Record Table ---> \n");
    for (Index = 0; Index < SamuClockVoltLimitTable->numEntries; Index++) {
      IDS_HDT_CONSOLE (GFX_MISC, "  SAMU Voltage Record #%d\n", Index
        );
      IDS_HDT_CONSOLE (GFX_MISC, "    SAMCLK = %d\n",
          SamuClockVoltLimitTable->entries[Index].usSAMClockLow |
          (SamuClockVoltLimitTable->entries[Index].usSAMClockHigh << 16)
        );
      IDS_HDT_CONSOLE (GFX_MISC, "    VID index = %d\n",
          SamuClockVoltLimitTable->entries[Index].usVoltage
        );
    }
  }

  IDS_HDT_CONSOLE (GFX_MISC, " PplayDumpExit\n");

}
