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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
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


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbFuseTable.h"
#include  "GnbPcie.h"
#include  "GnbGfx.h"
#include  "GnbFuseTable.h"
#include  "GnbGfxFamServices.h"
#include  "GnbCommonLib.h"
#include  "GfxPowerPlayTable.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBGFXINITLIBV1_GFXPOWERPLAYTABLE_FILECODE

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
  UINT32      Vclk;                                   ///< UVD VCLK
  UINT32      Dclk;                                   ///< UVD DCLK
  UINT8       NumberOfDpmStates;                      ///< Number of DPM states
  UINT8       DpmSatesArray[MAX_NUM_OF_DPM_STATES];   ///< DPM state index array
} SW_STATE;

/// DPM state
typedef struct {
  BOOLEAN     Valid;                                  ///< State valid
  UINT32      Sclk;                                   ///< Sclk in kHz
  UINT8       Vid;                                    ///< VID index
  UINT16      Tdp;                                    ///< Tdp limit
} DPM_STATE;

typedef struct {
  GFX_PLATFORM_CONFIG                       *Gfx;
  ATOM_PPLIB_POWERPLAYTABLE3                *PpTable;
  PP_FUSE_ARRAY                             *PpFuses;
  SW_STATE                                  SwStateArray [MAX_NUM_OF_SW_STATES];          ///< SW state array
  DPM_STATE                                 DpmStateArray[MAX_NUM_OF_DPM_STATES];         ///< Sclk DPM state array
  UINT8                                     NumOfClockVoltageLimitEnties;                 ///
  ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_RECORD VceClockVoltageLimitArray[MAX_NUM_OF_VCE_CLK_STATES];
  UINT8                                     NumOfVceClockEnties;
  VCECLOCKINFO                              VceClockInfoArray[MAX_NUM_OF_VCE_CLK_STATES];
  UINT8                                     NumOfVceStateEntries;
  ATOM_PPLIB_VCE_STATE_RECORD               VceStateArray[MAX_NUM_OF_VCE_STATES];         ///< VCE state array
} PP_WORKSPACE;
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
GfxIntegratedDebugDumpPpTable (
  IN       ATOM_PPLIB_POWERPLAYTABLE3   *PpTable,
  IN       GFX_PLATFORM_CONFIG          *Gfx
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Locate existing tdp
 *
 *
 * @param[in     ]  PpFuses          Pointer to  PP_FUSE_ARRAY
 * @param[in]       Sclk             Sclk in 10kHz
 * @param[in]       StdHeader        Standard configuration header
 * @retval                           Tdp limit in DPM state array
 */

STATIC UINT16
GfxPowerPlayLocateTdp (
  IN      PP_FUSE_ARRAY                 *PpFuses,
  IN      UINT32                        Sclk,
  IN      AMD_CONFIG_PARAMS             *StdHeader
  )
{
  UINT8   Index;
  UINT32  DpmIndex;
  UINT32  DpmSclk;
  UINT32  DeltaSclk;
  UINT32  MinDeltaSclk;

  DpmIndex = 0;
  MinDeltaSclk = 0xFFFFFFFF;
  for (Index = 0; Index < MAX_NUM_OF_FUSED_DPM_STATES; Index++) {
    if (PpFuses->SclkDpmDid[Index] != 0) {
      DpmSclk = GfxFmCalculateClock (PpFuses->SclkDpmDid[Index], StdHeader);
      DeltaSclk = (DpmSclk > Sclk) ? (DpmSclk - Sclk) : (Sclk - DpmSclk);
      if (DeltaSclk < MinDeltaSclk) {
        MinDeltaSclk = DeltaSclk;
        DpmIndex = Index;
      }
    }
  }
  return PpFuses->SclkDpmTdpLimit[DpmIndex];
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create new software state
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 * @retval                           Pointer to state entry in SW state array
 */

STATIC SW_STATE*
GfxPowerPlayCreateSwState (
  IN OUT   PP_WORKSPACE                  *PpWorkspace
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
/**
 * Create new DPM state
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 * @param[in]       Sclk             SCLK in kHz
 * @param[in]       Vid              Vid index
 * @param[in]       Tdp              Tdp limit
 * @retval                           Index of state entry in DPM state array
 */

STATIC UINT8
GfxPowerPlayCreateDpmState (
  IN OUT   PP_WORKSPACE                  *PpWorkspace,
  IN       UINT32                        Sclk,
  IN       UINT8                         Vid,
  IN       UINT16                        Tdp
  )
{
  UINT8 Index;
  for (Index = 0; Index < MAX_NUM_OF_DPM_STATES; Index++) {
    if (PpWorkspace->DpmStateArray[Index].Valid == FALSE) {
      PpWorkspace->DpmStateArray[Index].Sclk = Sclk;
      PpWorkspace->DpmStateArray[Index].Vid = Vid;
      PpWorkspace->DpmStateArray[Index].Valid = TRUE;
      PpWorkspace->DpmStateArray[Index].Tdp = Tdp;
      return Index;
    }
  }
  return 0;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Locate existing or Create new DPM state
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 * @param[in]       Sclk             SCLK in kHz
 * @param[in]       Vid              Vid index
 * @param[in]       Tdp              Tdp limit
 * @retval                           Index of state entry in DPM state array
 */

STATIC UINT8
GfxPowerPlayAddDpmState (
  IN OUT   PP_WORKSPACE                  *PpWorkspace,
  IN       UINT32                        Sclk,
  IN       UINT8                         Vid,
  IN       UINT16                        Tdp
  )
{
  UINT8 Index;
  for (Index = 0; Index < MAX_NUM_OF_DPM_STATES; Index++) {
    if (PpWorkspace->DpmStateArray[Index].Valid && Sclk == PpWorkspace->DpmStateArray[Index].Sclk && Vid == PpWorkspace->DpmStateArray[Index].Vid) {
      return Index;
    }
  }
  return GfxPowerPlayCreateDpmState (PpWorkspace, Sclk, Vid, Tdp);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Add reference to DPM state for SW state
 *
 *
 * @param[in, out]  SwStateArray     Pointer to SW state array
 * @param[in]       DpmStateIndex    DPM state index
 */

STATIC VOID
GfxPowerPlayAddDpmStateToSwState (
  IN OUT   SW_STATE                      *SwStateArray,
  IN       UINT8                         DpmStateIndex
  )
{
  SwStateArray->DpmSatesArray[SwStateArray->NumberOfDpmStates++] = DpmStateIndex;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Copy SW state info to PPTable
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */
STATIC VOID *
GfxPowerPlayAttachStateInfoBlock (
  IN OUT   PP_WORKSPACE                  *PpWorkspace
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
    if (PpWorkspace->SwStateArray[Index].Valid && PpWorkspace->SwStateArray[Index].NumberOfDpmStates != 0) {
      States->nonClockInfoIndex  = SwStateIndex;
      States->ucNumDPMLevels = PpWorkspace->SwStateArray[Index].NumberOfDpmStates;
      LibAmdMemCopy (
        &States->ClockInfoIndex[0],
        PpWorkspace->SwStateArray[Index].DpmSatesArray,
        PpWorkspace->SwStateArray[Index].NumberOfDpmStates,
        GnbLibGetHeader (PpWorkspace->Gfx)
        );
      States = (ATOM_PPLIB_STATE_V2*) ((UINT8*) States + sizeof (ATOM_PPLIB_STATE_V2) + sizeof (UINT8) * (States->ucNumDPMLevels - 1));
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
GfxPowerPlayAttachClockInfoBlock (
  IN OUT   PP_WORKSPACE                 *PpWorkspace
  )
{
  CLOCK_INFO_ARRAY            *ClockInfoArray;
  UINT8                       Index;
  UINT8                       ClkStateIndex;
  ClkStateIndex = 0;
  ClockInfoArray = (CLOCK_INFO_ARRAY *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  for (Index = 0; Index < MAX_NUM_OF_DPM_STATES; Index++) {
    if (PpWorkspace->DpmStateArray[Index].Valid == TRUE) {
      ClockInfoArray->ClockInfo[ClkStateIndex].ucEngineClockHigh = (UINT8) (PpWorkspace->DpmStateArray[Index].Sclk  >> 16);
      ClockInfoArray->ClockInfo[ClkStateIndex].usEngineClockLow = (UINT16) (PpWorkspace->DpmStateArray[Index].Sclk);
      ClockInfoArray->ClockInfo[ClkStateIndex].vddcIndex = PpWorkspace->DpmStateArray[Index].Vid;
      ClockInfoArray->ClockInfo[ClkStateIndex].tdpLimit = PpWorkspace->DpmStateArray[Index].Tdp;
      ClkStateIndex++;
    }
  }
  ClockInfoArray->ucNumEntries = ClkStateIndex;
  ClockInfoArray->ucEntrySize = sizeof (ATOM_PPLIB_SUMO_CLOCK_INFO);
  PpWorkspace->PpTable->sHeader.usStructureSize += sizeof (CLOCK_INFO_ARRAY) + sizeof (ATOM_PPLIB_SUMO_CLOCK_INFO) * ClkStateIndex - sizeof (ATOM_PPLIB_SUMO_CLOCK_INFO);
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
GfxPowerPlayAttachNonClockInfoBlock (
  IN OUT   PP_WORKSPACE                 *PpWorkspace
  )
{
  NON_CLOCK_INFO_ARRAY  *NonClockInfoArray;
  UINT8                 Index;
  UINT8                 NonClkStateIndex;

  NonClockInfoArray = (NON_CLOCK_INFO_ARRAY *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  NonClkStateIndex = 0;
  for (Index = 0; Index < MAX_NUM_OF_SW_STATES; Index++) {
    if (PpWorkspace->SwStateArray[Index].Valid && PpWorkspace->SwStateArray[Index].NumberOfDpmStates != 0) {
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].usClassification = PpWorkspace->SwStateArray[Index].Classification;
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].ulCapsAndSettings = PpWorkspace->SwStateArray[Index].CapsAndSettings;
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].usClassification2 = PpWorkspace->SwStateArray[Index].Classification2;
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].ulDCLK = PpWorkspace->SwStateArray[Index].Dclk;
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].ulVCLK = PpWorkspace->SwStateArray[Index].Vclk;
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
 * Check if fused state valid
 *
 *
 * @param[out]      Index             State index
 * @param[in]       PpFuses           Pointer to fuse table
 * @param[in]       Gfx               Gfx configuration info
 * @retval          TRUE              State is valid
 */
STATIC BOOLEAN
GfxPowerPlayIsFusedStateValid (
  IN      UINT8                         Index,
  IN      PP_FUSE_ARRAY                 *PpFuses,
  IN      GFX_PLATFORM_CONFIG           *Gfx
  )
{
  BOOLEAN Result;
  Result = FALSE;
  if (PpFuses->SclkDpmValid[Index] != 0) {
    Result = TRUE;
    if (PpFuses->PolicyLabel[Index] == POLICY_LABEL_BATTERY && (Gfx->AmdPlatformType & AMD_PLATFORM_MOBILE) == 0) {
      Result = FALSE;
    }
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get SW state calssification from fuses
 *
 *
 * @param[out]      Index             State index
 * @param[in]       PpFuses           Pointer to fuse table
 * @param[in]       Gfx               Gfx configuration info
 * @retval                            State classification
 */

STATIC UINT16
GfxPowerPlayGetClassificationFromFuses (
  IN      UINT8                         Index,
  IN      PP_FUSE_ARRAY                 *PpFuses,
  IN      GFX_PLATFORM_CONFIG           *Gfx
  )
{
  UINT16  Classification;
  Classification = 0;
  switch (PpFuses->PolicyFlags[Index]) {
  case 0x1:
    Classification |= ATOM_PPLIB_CLASSIFICATION_NONUVDSTATE;
    break;
  case 0x2:
    Classification |= ATOM_PPLIB_CLASSIFICATION_UVDSTATE;
    break;
  case 0x4:
    //Possible SD + HD state
    break;
  case 0x8:
    Classification |= ATOM_PPLIB_CLASSIFICATION_HDSTATE;
    break;
  case 0x10:
    Classification |= ATOM_PPLIB_CLASSIFICATION_SDSTATE;
    break;
  default:
    break;
  }
  switch (PpFuses->PolicyLabel[Index]) {
  case POLICY_LABEL_BATTERY:
    Classification |= ATOM_PPLIB_CLASSIFICATION_UI_BATTERY;
    break;
  case POLICY_LABEL_PERFORMANCE:
    Classification |= ATOM_PPLIB_CLASSIFICATION_UI_PERFORMANCE;
    break;
  default:
    break;
  }
  return Classification;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get SW state calssification2 from fuses
 *
 *
 * @param[out]      Index             State index
 * @param[in]       PpFuses           Pointer to fuse table
 * @param[in]       Gfx               Gfx configuration info
 * @retval                            State classification2
 */

STATIC UINT16
GfxPowerPlayGetClassification2FromFuses (
  IN      UINT8                         Index,
  IN      PP_FUSE_ARRAY                 *PpFuses,
  IN      GFX_PLATFORM_CONFIG           *Gfx
  )
{
  UINT16  Classification2;
  Classification2 = 0;

  switch (PpFuses->PolicyFlags[Index]) {

  case 0x4:
    Classification2 |= ATOM_PPLIB_CLASSIFICATION2_MVC;
    break;

  default:
    break;
  }

  return Classification2;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Build SCLK state info
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID
GfxPowerPlayBuildSclkStateTable (
  IN OUT   PP_WORKSPACE                  *PpWorkspace
  )
{
  UINT8                       ClkStateIndex;
  UINT8                       DpmFuseIndex;
  UINT8                       Index;
  UINT32                      Sclk;
  SW_STATE                    *State;
  PP_FUSE_ARRAY               *PpFuses;

  PpFuses = PpWorkspace->PpFuses;
  // Create States from Fuses
  for (Index = 0; Index < MAX_NUM_OF_FUSED_SW_STATES; Index++) {
    if (GfxPowerPlayIsFusedStateValid (Index, PpFuses, PpWorkspace->Gfx)) {
      //Create new SW State;
      State = GfxPowerPlayCreateSwState (PpWorkspace);
      State->Classification = GfxPowerPlayGetClassificationFromFuses (Index, PpFuses, PpWorkspace->Gfx);
      State->Classification2 = GfxPowerPlayGetClassification2FromFuses (Index, PpFuses, PpWorkspace->Gfx);
      if ((State->Classification & (ATOM_PPLIB_CLASSIFICATION_HDSTATE | ATOM_PPLIB_CLASSIFICATION_UVDSTATE | ATOM_PPLIB_CLASSIFICATION_SDSTATE)) != 0 ||
          (State->Classification2 & ATOM_PPLIB_CLASSIFICATION2_MVC) != 0) {
        State->Vclk = (PpFuses->VclkDid[PpFuses->VclkDclkSel[Index]] != 0) ? GfxFmCalculateClock (PpFuses->VclkDid[PpFuses->VclkDclkSel[Index]],  GnbLibGetHeader (PpWorkspace->Gfx)) : 0;
        State->Dclk = (PpFuses->DclkDid[PpFuses->VclkDclkSel[Index]] != 0) ? GfxFmCalculateClock (PpFuses->DclkDid[PpFuses->VclkDclkSel[Index]],  GnbLibGetHeader (PpWorkspace->Gfx)) : 0;
      }
      if (((State->Classification & 0x7) == ATOM_PPLIB_CLASSIFICATION_UI_BATTERY) ||
          ((State->Classification & (ATOM_PPLIB_CLASSIFICATION_HDSTATE | ATOM_PPLIB_CLASSIFICATION_SDSTATE)) != 0)) {
        if (PpWorkspace->Gfx->AbmSupport != 0) {
          State->CapsAndSettings |= ATOM_PPLIB_ENABLE_VARIBRIGHT;
        }
        if (PpWorkspace->Gfx->DynamicRefreshRate != 0) {
          State->CapsAndSettings |= ATOM_PPLIB_ENABLE_DRR;
        }
      }
      for (DpmFuseIndex = 0; DpmFuseIndex < MAX_NUM_OF_FUSED_DPM_STATES; DpmFuseIndex++) {
        if ((PpFuses->SclkDpmValid[Index] & (1 << DpmFuseIndex)) != 0 ) {
          Sclk = (PpFuses->SclkDpmDid[DpmFuseIndex] != 0) ? GfxFmCalculateClock (PpFuses->SclkDpmDid[DpmFuseIndex], GnbLibGetHeader (PpWorkspace->Gfx)) : 0;
          if (Sclk != 0) {
            ClkStateIndex = GfxPowerPlayAddDpmState (PpWorkspace, Sclk, PpFuses->SclkDpmVid[DpmFuseIndex], PpFuses->SclkDpmTdpLimit[DpmFuseIndex]);
            GfxPowerPlayAddDpmStateToSwState (State, ClkStateIndex);
          }
        }
      }
    }
  }
  // Create Boot State
  State = GfxPowerPlayCreateSwState (PpWorkspace);
  State->Classification = ATOM_PPLIB_CLASSIFICATION_BOOT;
  Sclk = 200 * 100;
  ClkStateIndex = GfxPowerPlayAddDpmState (PpWorkspace, Sclk, 0, GfxPowerPlayLocateTdp (PpFuses, Sclk, GnbLibGetHeader (PpWorkspace->Gfx)));
  GfxPowerPlayAddDpmStateToSwState (State, ClkStateIndex);

  // Create Thermal State
  State = GfxPowerPlayCreateSwState (PpWorkspace);
  State->Classification = ATOM_PPLIB_CLASSIFICATION_THERMAL;
  Sclk = GfxFmCalculateClock (PpFuses->SclkThermDid, GnbLibGetHeader (PpWorkspace->Gfx));
  ClkStateIndex = GfxPowerPlayAddDpmState (PpWorkspace, Sclk, 0, GfxPowerPlayLocateTdp (PpFuses, Sclk, GnbLibGetHeader (PpWorkspace->Gfx)));
  GfxPowerPlayAddDpmStateToSwState (State, ClkStateIndex);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Add ECLK state
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 * @param[in]       Eclk             SCLK in kHz
 * @retval                           Index of state entry in ECLK clock array
 */

STATIC UINT8
GfxPowerPlayAddEclkState (
  IN OUT   PP_WORKSPACE                  *PpWorkspace,
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
  PpWorkspace->VceClockInfoArray[PpWorkspace->NumOfVceClockEnties].ucEVClkHigh = EclkHigh;
  PpWorkspace->VceClockInfoArray[PpWorkspace->NumOfVceClockEnties].usEVClkLow = EclkLow;
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
GfxPowerPlayAddEclkVoltageRecord (
  IN OUT   PP_WORKSPACE                  *PpWorkspace,
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
/**
 * Attach extended header
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPowerPlayAttachVceTableRevBlock (
  IN OUT   PP_WORKSPACE                 *PpWorkspace
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
 * Attach extended header
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPowerPlayAttachExtendedHeaderBlock (
  IN OUT   PP_WORKSPACE                 *PpWorkspace
  )
{
  ATOM_PPLIB_EXTENDEDHEADER *ExtendedHeader;
  ExtendedHeader = (ATOM_PPLIB_EXTENDEDHEADER *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  ExtendedHeader->usSize = sizeof (ATOM_PPLIB_EXTENDEDHEADER);
  PpWorkspace->PpTable->sHeader.usStructureSize += sizeof (ATOM_PPLIB_EXTENDEDHEADER);
  return ExtendedHeader;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Attach VCE clock info block
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID *
GfxPowerPlayAttachVceClockInfoBlock (
  IN OUT   PP_WORKSPACE                 *PpWorkspace
  )
{
  VCECLOCKINFOARRAY   *VceClockInfoArray;
  VceClockInfoArray = (VCECLOCKINFOARRAY *) ((UINT8 *) PpWorkspace->PpTable + PpWorkspace->PpTable->sHeader.usStructureSize);
  VceClockInfoArray->ucNumEntries = PpWorkspace->NumOfVceClockEnties;
  LibAmdMemCopy (
    &VceClockInfoArray->entries[0],
    &PpWorkspace->VceClockInfoArray[0],
    VceClockInfoArray->ucNumEntries * sizeof (VCECLOCKINFO),
    GnbLibGetHeader (PpWorkspace->Gfx)
    );
  PpWorkspace->PpTable->sHeader.usStructureSize = PpWorkspace->PpTable->sHeader.usStructureSize +
                                                  sizeof (VCECLOCKINFOARRAY) +
                                                  VceClockInfoArray->ucNumEntries * sizeof (VCECLOCKINFO) -
                                                  sizeof (VCECLOCKINFO);
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
GfxPowerPlayAttachVceVoltageLimitBlock (
  IN OUT   PP_WORKSPACE                 *PpWorkspace
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
  PpWorkspace->PpTable->sHeader.usStructureSize = PpWorkspace->PpTable->sHeader.usStructureSize +
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
GfxPowerPlayAttachVceStateTaleBlock (
  IN OUT   PP_WORKSPACE                 *PpWorkspace
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
 * Build VCE state info
 *
 *
 * @param[in, out]  PpWorkspace      PP workspace
 */

STATIC VOID
GfxPowerPlayBuildVceStateTable (
  IN OUT   PP_WORKSPACE                  *PpWorkspace
  )
{
  UINT8                       Index;
  UINT8                       VceStateIndex;
  UINT8                       Vid;
  UINT32                      Eclk;
  UINT32                      Sclk;
  UINT8                       UsedStateBitmap;
  UsedStateBitmap = 0;
  // build used state
  for (Index = 0; Index < ARRAY_SIZE(PpWorkspace->PpFuses->VceFlags); Index++) {
    UsedStateBitmap |= PpWorkspace->PpFuses->VceFlags[Index];
    for (VceStateIndex = 0; VceStateIndex < ARRAY_SIZE(PpWorkspace->VceStateArray); VceStateIndex++) {
      if ((PpWorkspace->PpFuses->VceFlags[Index] & (1 << VceStateIndex)) != 0) {
        Sclk = GfxFmCalculateClock (PpWorkspace->PpFuses->SclkDpmDid[PpWorkspace->PpFuses->VceReqSclkSel[Index]], GnbLibGetHeader (PpWorkspace->Gfx));
        Vid = PpWorkspace->PpFuses->SclkDpmVid[PpWorkspace->PpFuses->VceReqSclkSel[Index]];
        PpWorkspace->VceStateArray[VceStateIndex].ucClockInfoIndex = GfxPowerPlayAddDpmState (PpWorkspace, Sclk, Vid, GfxPowerPlayLocateTdp (PpWorkspace->PpFuses, Sclk, GnbLibGetHeader (PpWorkspace->Gfx)));
        if (PpWorkspace->PpFuses->VceMclk[Index] == 1) {
          PpWorkspace->VceStateArray[VceStateIndex].ucClockInfoIndex |= (PpWorkspace->PpFuses->VceMclk[Index] << 6);
        }
        Eclk = GfxFmCalculateClock (PpWorkspace->PpFuses->EclkDid[Index], GnbLibGetHeader (PpWorkspace->Gfx));
        PpWorkspace->VceStateArray[VceStateIndex].ucVCEClockInfoIndex = GfxPowerPlayAddEclkState (PpWorkspace, Eclk);
        GfxPowerPlayAddEclkVoltageRecord (PpWorkspace, PpWorkspace->VceStateArray[VceStateIndex].ucVCEClockInfoIndex, Vid);
        PpWorkspace->NumOfVceStateEntries++;
      }
    }
  }
  //build unused states
  for (VceStateIndex = 0; VceStateIndex < ARRAY_SIZE(PpWorkspace->VceStateArray); VceStateIndex++) {
    if ((UsedStateBitmap & (1 << VceStateIndex)) == 0) {
      PpWorkspace->VceStateArray[VceStateIndex].ucClockInfoIndex = 0;
      PpWorkspace->VceStateArray[VceStateIndex].ucVCEClockInfoIndex = GfxPowerPlayAddEclkState (PpWorkspace, 0);
      PpWorkspace->NumOfVceStateEntries++;
    }
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
GfxPowerPlayBuildTable (
     OUT   VOID                          *Buffer,
  IN       GFX_PLATFORM_CONFIG           *Gfx
  )
{
  PP_WORKSPACE                PpWorkspace;
  VOID                        *BlockPtr;

  LibAmdMemFill (&PpWorkspace, 0x00, sizeof (PP_WORKSPACE), GnbLibGetHeader (Gfx));
  PpWorkspace.PpFuses = GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, GnbLibGetHeader (Gfx));
  ASSERT (PpWorkspace.PpFuses != NULL);
  if (PpWorkspace.PpFuses == NULL) {
    return AGESA_ERROR;
  }
  PpWorkspace.PpTable = (ATOM_PPLIB_POWERPLAYTABLE3 *) Buffer;
  PpWorkspace.Gfx = Gfx;
  //Fill static info
  PpWorkspace.PpTable->sHeader.ucTableFormatRevision = 6;
  PpWorkspace.PpTable->sHeader.ucTableContentRevision = 1;
  PpWorkspace.PpTable->ucDataRevision = PpWorkspace.PpFuses->PPlayTableRev;
  PpWorkspace.PpTable->sThermalController.ucType = ATOM_PP_THERMALCONTROLLER_SUMO;
  PpWorkspace.PpTable->sThermalController.ucFanParameters = ATOM_PP_FANPARAMETERS_NOFAN;
  PpWorkspace.PpTable->sHeader.usStructureSize = sizeof (ATOM_PPLIB_POWERPLAYTABLE3);
  PpWorkspace.PpTable->usTableSize = sizeof (ATOM_PPLIB_POWERPLAYTABLE3);
  PpWorkspace.PpTable->usFormatID = 7;
  if ((Gfx->AmdPlatformType & AMD_PLATFORM_MOBILE) != 0) {
    PpWorkspace.PpTable->ulPlatformCaps |= ATOM_PP_PLATFORM_CAP_POWERPLAY;
  }

  // Fill Slck SW/DPM state info
  GfxPowerPlayBuildSclkStateTable (&PpWorkspace);
  // Fill Eclk state info
  if (PpWorkspace.PpFuses->VceSateTableSupport) {
    GfxPowerPlayBuildVceStateTable (&PpWorkspace);
  }

  //Copy state info to actual PP table
  BlockPtr = GfxPowerPlayAttachStateInfoBlock (&PpWorkspace);
  PpWorkspace.PpTable->usStateArrayOffset = (USHORT) ((UINT8 *) BlockPtr - (UINT8 *) (PpWorkspace.PpTable));
  BlockPtr = GfxPowerPlayAttachClockInfoBlock (&PpWorkspace);
  PpWorkspace.PpTable->usClockInfoArrayOffset = (USHORT) ((UINT8 *) BlockPtr - (UINT8 *) (PpWorkspace.PpTable));
  BlockPtr = GfxPowerPlayAttachNonClockInfoBlock (&PpWorkspace);
  PpWorkspace.PpTable->usNonClockInfoArrayOffset = (USHORT) ((UINT8 *) BlockPtr - (UINT8 *) (PpWorkspace.PpTable));
  if (PpWorkspace.PpFuses->VceSateTableSupport) {
    ATOM_PPLIB_EXTENDEDHEADER   *ExtendedHeader;
    ExtendedHeader = (ATOM_PPLIB_EXTENDEDHEADER *) GfxPowerPlayAttachExtendedHeaderBlock (&PpWorkspace);
    PpWorkspace.PpTable->usExtendendedHeaderOffset = (USHORT) ((UINT8 *) ExtendedHeader - (UINT8 *) (PpWorkspace.PpTable));
    BlockPtr = GfxPowerPlayAttachVceTableRevBlock (&PpWorkspace);
    ExtendedHeader->usVCETableOffset = (USHORT) ((UINT8 *) BlockPtr - (UINT8 *) (PpWorkspace.PpTable));
    GfxPowerPlayAttachVceClockInfoBlock (&PpWorkspace);
    GfxPowerPlayAttachVceVoltageLimitBlock (&PpWorkspace);
    GfxPowerPlayAttachVceStateTaleBlock (&PpWorkspace);

  }
  GNB_DEBUG_CODE (
    GfxIntegratedDebugDumpPpTable (PpWorkspace.PpTable, Gfx);
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
GfxIntegratedDebugDumpPpTable (
  IN       ATOM_PPLIB_POWERPLAYTABLE3   *PpTable,
  IN       GFX_PLATFORM_CONFIG          *Gfx
  )
{
  UINTN                                     Index;
  UINTN                                     DpmIndex;
  STATE_ARRAY                               *StateArray;
  ATOM_PPLIB_STATE_V2                       *StatesPtr;
  NON_CLOCK_INFO_ARRAY                      *NonClockInfoArrayPtr;
  CLOCK_INFO_ARRAY                          *ClockInfoArrayPtr;
  ATOM_PPLIB_EXTENDEDHEADER                 *ExtendedHeader;
  ATOM_PPLIB_VCE_STATE_TABLE                *VceStateTable;
  ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_TABLE  *VceClockVoltageLimitTable;
  VCECLOCKINFOARRAY                         *VceClockInfoArray;
  UINT8                                     SclkIndex;
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
    IDS_HDT_CONSOLE (GFX_MISC, "    VCLK = %dkHz\n",
       NonClockInfoArrayPtr->NonClockInfo[StatesPtr->nonClockInfoIndex].ulVCLK
      );
    IDS_HDT_CONSOLE (GFX_MISC, "    DCLK = %dkHz\n",
       NonClockInfoArrayPtr->NonClockInfo[StatesPtr->nonClockInfoIndex].ulDCLK
      );
    IDS_HDT_CONSOLE (GFX_MISC, "    DPM State Index: ");
    for (DpmIndex = 0; DpmIndex < StatesPtr->ucNumDPMLevels; DpmIndex++) {
      IDS_HDT_CONSOLE (GFX_MISC, "%d ",
        StatesPtr->ClockInfoIndex [DpmIndex]
        );
    }
    IDS_HDT_CONSOLE (GFX_MISC, "\n");
    StatesPtr = (ATOM_PPLIB_STATE_V2 *) ((UINT8 *) StatesPtr + sizeof (ATOM_PPLIB_STATE_V2) + StatesPtr->ucNumDPMLevels - 1);
  }
  IDS_HDT_CONSOLE (GFX_MISC, "  < --- SCLK DPM State Table ---> \n");
  for (Index = 0; Index < ClockInfoArrayPtr->ucNumEntries; Index++) {
    UINT32  Sclk;
    Sclk = ClockInfoArrayPtr->ClockInfo[Index].usEngineClockLow | (ClockInfoArrayPtr->ClockInfo[Index].ucEngineClockHigh << 16);
    IDS_HDT_CONSOLE (GFX_MISC, "  DPM State #%d\n",
      Index
      );
    IDS_HDT_CONSOLE (GFX_MISC, "    SCLK = %d\n",
      ClockInfoArrayPtr->ClockInfo[Index].usEngineClockLow | (ClockInfoArrayPtr->ClockInfo[Index].ucEngineClockHigh << 16)
      );
    IDS_HDT_CONSOLE (GFX_MISC, "    VID index = %d\n",
      ClockInfoArrayPtr->ClockInfo[Index].vddcIndex
      );
    IDS_HDT_CONSOLE (GFX_MISC, "    tdpLimit  = %d\n",
      ClockInfoArrayPtr->ClockInfo[Index].tdpLimit
      );
  }
  if (PpTable->usExtendendedHeaderOffset != 0) {
    ExtendedHeader = (ATOM_PPLIB_EXTENDEDHEADER *) ((UINT8 *) PpTable + PpTable->usExtendendedHeaderOffset);
    VceClockInfoArray = (VCECLOCKINFOARRAY *) ((UINT8 *) ExtendedHeader + sizeof (ATOM_PPLIB_EXTENDEDHEADER) + sizeof (ATOM_PPLIB_VCE_TABLE));
    VceClockVoltageLimitTable = (ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_TABLE *) ((UINT8 *) VceClockInfoArray +
                                sizeof (VCECLOCKINFOARRAY) +
                                VceClockInfoArray->ucNumEntries * sizeof (VCECLOCKINFO) -
                                sizeof (VCECLOCKINFO));
    VceStateTable = (ATOM_PPLIB_VCE_STATE_TABLE *) ((UINT8 *) VceClockVoltageLimitTable +
                    sizeof (ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_TABLE) +
                    VceClockVoltageLimitTable->numEntries * sizeof (ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_RECORD) -
                    sizeof (ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_RECORD));

    IDS_HDT_CONSOLE (GFX_MISC, "  < --- VCE State Table [%d]--> \n", VceStateTable->numEntries);
    for (Index = 0; Index < VceStateTable->numEntries; Index++) {
      SclkIndex = VceStateTable->entries[Index].ucClockInfoIndex & 0x3F;
      EclkIndex = VceStateTable->entries[Index].ucVCEClockInfoIndex;
      IDS_HDT_CONSOLE (GFX_MISC, "  VCE State #%d\n", Index
        );
      if ((VceClockInfoArray->entries[EclkIndex].usECClkLow | (VceClockInfoArray->entries[EclkIndex].ucECClkHigh << 16)) == 0) {
        IDS_HDT_CONSOLE (GFX_MISC, "    Disable\n");
      } else {
        IDS_HDT_CONSOLE (GFX_MISC, "    SCLK = %d\n",
          ClockInfoArrayPtr->ClockInfo[SclkIndex].usEngineClockLow | (ClockInfoArrayPtr->ClockInfo[SclkIndex].ucEngineClockHigh << 16)
          );
        IDS_HDT_CONSOLE (GFX_MISC, "    ECCLK = %d\n",
          VceClockInfoArray->entries[EclkIndex].usECClkLow | (VceClockInfoArray->entries[EclkIndex].ucECClkHigh << 16)
          );
        IDS_HDT_CONSOLE (GFX_MISC, "    EVCLK = %d\n",
          VceClockInfoArray->entries[EclkIndex].usEVClkLow | (VceClockInfoArray->entries[EclkIndex].ucEVClkHigh << 16)
          );
        IDS_HDT_CONSOLE (GFX_MISC, "    MCLK = %d\n",
          (VceStateTable->entries[Index].ucClockInfoIndex >> 6 ) & 0x3
          );
      }
    }
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
  }
}
