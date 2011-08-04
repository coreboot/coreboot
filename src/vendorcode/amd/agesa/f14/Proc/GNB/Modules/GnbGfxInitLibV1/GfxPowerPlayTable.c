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
 * @e \$Revision: 38882 $   @e \$Date: 2010-09-30 18:42:57 -0700 (Thu, 30 Sep 2010) $
 *
 */
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
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
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
  BOOLEAN   Valid;                                  ///< State valid
  UINT16    Classification;                         ///< State classification
  UINT32    CapsAndSettings;                        ///< State capability and settings
  UINT32    Vclk;                                   ///< UVD VCLK
  UINT32    Dclk;                                   ///< UVD DCLK
  UINT8     NumberOfDpmStates;                      ///< Number of DPM states
  UINT8     DpmSatesArray[MAX_NUM_OF_DPM_STATES];   ///< DPM state index array
} SW_STATE;

/// DPM state
typedef struct {
  BOOLEAN   Valid;                                  ///< State valid
  UINT32    Sclk;                                   ///< Sclk in kHz
  UINT8     Vid;                                    ///< VID index
  UINT16    Tdp;                                    ///< Tdp limit
} DPM_STATE;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
UINT16
GfxPowerPlayLocateTdp (
  IN      PP_FUSE_ARRAY                 *PpFuses,
  IN      UINT32                        Sclk,
  IN      AMD_CONFIG_PARAMS             *StdHeader
  );

SW_STATE*
GfxPowerPlayCreateSwState (
  IN OUT   SW_STATE                     *SwStateArray
  );

UINT8
GfxPowerPlayCreateDpmState (
  IN      DPM_STATE                     *DpmStateArray,
  IN      UINT32                        Sclk,
  IN      UINT8                         Vid,
  IN      UINT16                        Tdp
  );

UINT8
GfxPowerPlayAddDpmState (
  IN      DPM_STATE                     *DpmStateArray,
  IN      UINT32                        Sclk,
  IN      UINT8                         Vid,
  IN      UINT16                        Tdp
  );

VOID
GfxPowerPlayAddDpmStateToSwState (
  IN OUT   SW_STATE                     *SwStateArray,
  IN       UINT8                        DpmStateIndex
  );

UINT32
GfxPowerPlayCopyStateInfo (
  IN OUT   STATE_ARRAY                   *StateArray,
  IN       SW_STATE                      *SwStateArray,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

UINT32
GfxPowerPlayCopyClockInfo (
  IN      CLOCK_INFO_ARRAY              *ClockInfoArray,
  IN      DPM_STATE                     *DpmStateArray,
  IN      AMD_CONFIG_PARAMS             *StdHeader
  );

UINT32
GfxPowerPlayCopyNonClockInfo (
  IN      NON_CLOCK_INFO_ARRAY          *NonClockInfoArray,
  IN      SW_STATE                      *SwStateArray,
  IN      AMD_CONFIG_PARAMS             *StdHeader
  );

BOOLEAN
GfxPowerPlayIsFusedStateValid (
  IN      UINT8                         Index,
  IN      PP_FUSE_ARRAY                 *PpFuses,
  IN      GFX_PLATFORM_CONFIG           *Gfx
  );

UINT16
GfxPowerPlayGetClassificationFromFuses (
  IN      UINT8                         Index,
  IN      PP_FUSE_ARRAY                 *PpFuses,
  IN      GFX_PLATFORM_CONFIG           *Gfx
  );

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

UINT16
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
        MinDeltaSclk = MinDeltaSclk;
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
 * @param[in, out]  SwStateArray     Pointer to SW state array
 * @retval                           Pointer to state entry in SW state array
 */

SW_STATE*
GfxPowerPlayCreateSwState (
  IN OUT   SW_STATE                     *SwStateArray
  )
{
  UINTN Index;
  for (Index = 0; Index < MAX_NUM_OF_SW_STATES; Index++) {
    if (SwStateArray[Index].Valid == FALSE) {
      SwStateArray[Index].Valid = TRUE;
      return  &SwStateArray[Index];
    }
  }
  return NULL;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create new DPM state
 *
 *
 * @param[in, out]  DpmStateArray    Pointer to DPM state array
 * @param[in]       Sclk             SCLK in kHz
 * @param[in]       Vid              Vid index
 * @param[in]       Tdp              Tdp limit
 * @retval                           Index of state entry in DPM state array
 */

UINT8
GfxPowerPlayCreateDpmState (
  IN      DPM_STATE                     *DpmStateArray,
  IN      UINT32                        Sclk,
  IN      UINT8                         Vid,
  IN      UINT16                        Tdp
  )
{
  UINT8 Index;
  for (Index = 0; Index < MAX_NUM_OF_DPM_STATES; Index++) {
    if (DpmStateArray[Index].Valid == FALSE) {
      DpmStateArray[Index].Sclk = Sclk;
      DpmStateArray[Index].Vid = Vid;
      DpmStateArray[Index].Valid = TRUE;
      DpmStateArray[Index].Tdp = Tdp;
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
 * @param[in, out]  DpmStateArray    Pointer to DPM state array
 * @param[in]       Sclk             SCLK in kHz
 * @param[in]       Vid              Vid index
 * @param[in]       Tdp              Tdp limit
 * @retval                           Index of state entry in DPM state array
 */

UINT8
GfxPowerPlayAddDpmState (
  IN      DPM_STATE                     *DpmStateArray,
  IN      UINT32                        Sclk,
  IN      UINT8                         Vid,
  IN      UINT16                        Tdp
  )
{
  UINT8 Index;
  for (Index = 0; Index < MAX_NUM_OF_DPM_STATES; Index++) {
    if (DpmStateArray[Index].Valid && Sclk == DpmStateArray[Index].Sclk && Vid == DpmStateArray[Index].Vid) {
      return Index;
    }
  }
  return GfxPowerPlayCreateDpmState (DpmStateArray, Sclk, Vid, Tdp);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Add reference to DPM state for SW state
 *
 *
 * @param[in, out]  SwStateArray     Pointer to SW state array
 * @param[in]       DpmStateIndex    DPM state index
 */

VOID
GfxPowerPlayAddDpmStateToSwState (
  IN OUT   SW_STATE                     *SwStateArray,
  IN       UINT8                        DpmStateIndex
  )
{
  SwStateArray->DpmSatesArray[SwStateArray->NumberOfDpmStates++] = DpmStateIndex;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Copy SW state info to PPTable
 *
 *
 * @param[out]      StateArray        Pointer to PPtable SW state array
 * @param[in]       SwStateArray      Pointer to SW state array
 * @param[in]       StdHeader         Standard configuration header
 */
UINT32
GfxPowerPlayCopyStateInfo (
  IN OUT   STATE_ARRAY                   *StateArray,
  IN       SW_STATE                      *SwStateArray,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  )
{
  UINT8                 Index;
  UINT8                 SwStateIndex;
  ATOM_PPLIB_STATE_V2   *States;
  States = &StateArray->States[0];
  SwStateIndex = 0;
  for (Index = 0; Index < MAX_NUM_OF_SW_STATES; Index++) {
    if (SwStateArray[Index].Valid && SwStateArray[Index].NumberOfDpmStates != 0) {
      States->nonClockInfoIndex  = SwStateIndex;
      States->ucNumDPMLevels = SwStateArray[Index].NumberOfDpmStates;
      LibAmdMemCopy (
        &States->ClockInfoIndex[0],
        SwStateArray[Index].DpmSatesArray,
        SwStateArray[Index].NumberOfDpmStates,
        StdHeader
        );
      States = (ATOM_PPLIB_STATE_V2*) ((UINT8*) States + sizeof (ATOM_PPLIB_STATE_V2) + sizeof (UINT8) * (States->ucNumDPMLevels - 1));
      SwStateIndex++;
    }
  }
  StateArray->ucNumEntries = SwStateIndex;
  return (UINT32) ((UINT8*) States - (UINT8*) StateArray);
}
/*----------------------------------------------------------------------------------------*/
/**
 * Copy clock info to PPTable
 *
 *
 * @param[out]      ClockInfoArray    Pointer to clock info array
 * @param[in]       DpmStateArray     Pointer to DPM state array
 * @param[in]       StdHeader         Standard configuration header
 */
UINT32
GfxPowerPlayCopyClockInfo (
  IN      CLOCK_INFO_ARRAY              *ClockInfoArray,
  IN      DPM_STATE                     *DpmStateArray,
  IN      AMD_CONFIG_PARAMS             *StdHeader
  )
{
  UINT8                       Index;
  UINT8                       ClkStateIndex;
  ClkStateIndex = 0;
  for (Index = 0; Index < MAX_NUM_OF_DPM_STATES; Index++) {
    if (DpmStateArray[Index].Valid == TRUE) {
      ClockInfoArray->ClockInfo[ClkStateIndex].ucEngineClockHigh = (UINT8) (DpmStateArray[Index].Sclk  >> 16);
      ClockInfoArray->ClockInfo[ClkStateIndex].usEngineClockLow = (UINT16) (DpmStateArray[Index].Sclk);
      ClockInfoArray->ClockInfo[ClkStateIndex].vddcIndex = DpmStateArray[Index].Vid;
      ClockInfoArray->ClockInfo[ClkStateIndex].tdpLimit = DpmStateArray[Index].Tdp;
      ClkStateIndex++;
    }
  }
  ClockInfoArray->ucNumEntries = ClkStateIndex;
  ClockInfoArray->ucEntrySize = sizeof (ATOM_PPLIB_SUMO_CLOCK_INFO);
  return sizeof (CLOCK_INFO_ARRAY) + sizeof (ATOM_PPLIB_SUMO_CLOCK_INFO) * (ClkStateIndex) - sizeof (ATOM_PPLIB_SUMO_CLOCK_INFO);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Copy non clock info to PPTable
 *
 *
 * @param[out]      NonClockInfoArray Pointer to PPtable Non clock array
 * @param[in]       SwStateArray      Pointer to SW state array
 * @param[in]       StdHeader         Standard configuration header
 */

UINT32
GfxPowerPlayCopyNonClockInfo (
  IN      NON_CLOCK_INFO_ARRAY          *NonClockInfoArray,
  IN      SW_STATE                      *SwStateArray,
  IN      AMD_CONFIG_PARAMS             *StdHeader
  )
{
  UINT8                 Index;
  UINT8                 NonClkStateIndex;
  NonClkStateIndex = 0;
  for (Index = 0; Index < MAX_NUM_OF_SW_STATES; Index++) {
    if (SwStateArray[Index].Valid && SwStateArray[Index].NumberOfDpmStates != 0) {
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].usClassification = SwStateArray[Index].Classification;
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].ulCapsAndSettings = SwStateArray[Index].CapsAndSettings;
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].ulDCLK = SwStateArray[Index].Dclk;
      NonClockInfoArray->NonClockInfo[NonClkStateIndex].ulVCLK = SwStateArray[Index].Vclk;
      NonClkStateIndex++;
    }
  }
  NonClockInfoArray->ucNumEntries = NonClkStateIndex;
  NonClockInfoArray->ucEntrySize = sizeof (ATOM_PPLIB_NONCLOCK_INFO);
  return sizeof (NON_CLOCK_INFO_ARRAY) + sizeof (ATOM_PPLIB_NONCLOCK_INFO) * NonClkStateIndex - sizeof (ATOM_PPLIB_NONCLOCK_INFO);
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
BOOLEAN
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
UINT16
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
  ATOM_PPLIB_POWERPLAYTABLE3  *PpTable;
  SW_STATE                    SwStateArray [MAX_NUM_OF_SW_STATES];
  DPM_STATE                   DpmStateArray[MAX_NUM_OF_DPM_STATES];
  UINT8                       ClkStateIndex;
  UINT8                       DpmFuseIndex;
  UINT8                       Index;
  UINT32                      StateArrayLength;
  UINT32                      ClockArrayLength;
  UINT32                      NonClockArrayLength;
  SW_STATE                    *State;
  PP_FUSE_ARRAY               *PpFuses;
  UINT32                      Sclk;

  PpFuses = GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, GnbLibGetHeader (Gfx));
  ASSERT (PpFuses != NULL);
  if (PpFuses == NULL) {
    return AGESA_ERROR;
  }

  PpTable = (ATOM_PPLIB_POWERPLAYTABLE3 *) Buffer;
  LibAmdMemFill (SwStateArray, 0x00, sizeof (SwStateArray), GnbLibGetHeader (Gfx));
  LibAmdMemFill (DpmStateArray, 0x00, sizeof (DpmStateArray), GnbLibGetHeader (Gfx));
  // Create States from Fuses
  for (Index = 0; Index < MAX_NUM_OF_FUSED_SW_STATES; Index++) {
    if (GfxPowerPlayIsFusedStateValid (Index, PpFuses, Gfx)) {
      //Create new SW State;
      State = GfxPowerPlayCreateSwState (SwStateArray);
      State->Classification = GfxPowerPlayGetClassificationFromFuses (Index, PpFuses, Gfx);
      if ((State->Classification & (ATOM_PPLIB_CLASSIFICATION_HDSTATE | ATOM_PPLIB_CLASSIFICATION_UVDSTATE)) != 0) {
        State->Vclk = (PpFuses->VclkDid[PpFuses->VclkDclkSel[Index]] != 0) ? GfxFmCalculateClock (PpFuses->VclkDid[PpFuses->VclkDclkSel[Index]],  GnbLibGetHeader (Gfx)) : 0;
        State->Dclk = (PpFuses->DclkDid[PpFuses->VclkDclkSel[Index]] != 0) ? GfxFmCalculateClock (PpFuses->DclkDid[PpFuses->VclkDclkSel[Index]],  GnbLibGetHeader (Gfx)) : 0;
      }
      if ((State->Classification & 0x7) == ATOM_PPLIB_CLASSIFICATION_UI_BATTERY) {
        if (Gfx->AbmSupport != 0) {
          State->CapsAndSettings |= ATOM_PPLIB_ENABLE_VARIBRIGHT;
        }
        if (Gfx->DynamicRefreshRate != 0) {
          State->CapsAndSettings |= ATOM_PPLIB_ENABLE_DRR;
        }
      }
      for (DpmFuseIndex = 0; DpmFuseIndex < MAX_NUM_OF_FUSED_DPM_STATES; DpmFuseIndex++) {
        if ((PpFuses->SclkDpmValid[Index] & (1 << DpmFuseIndex)) != 0 ) {
          Sclk = (PpFuses->SclkDpmDid[DpmFuseIndex] != 0) ? GfxFmCalculateClock (PpFuses->SclkDpmDid[DpmFuseIndex], GnbLibGetHeader (Gfx)) : 0;
          if (Sclk != 0) {
            ClkStateIndex = GfxPowerPlayAddDpmState (DpmStateArray, Sclk, PpFuses->SclkDpmVid[DpmFuseIndex], PpFuses->SclkDpmTdpLimit[DpmFuseIndex]);
            GfxPowerPlayAddDpmStateToSwState (State, ClkStateIndex);
          }
        }
      }
    }
  }
  // Create Boot State
  State = GfxPowerPlayCreateSwState (SwStateArray);
  State->Classification = ATOM_PPLIB_CLASSIFICATION_BOOT;
  Sclk = 200 * 100;
  ClkStateIndex = GfxPowerPlayAddDpmState (DpmStateArray, Sclk, 0, GfxPowerPlayLocateTdp (PpFuses, Sclk, GnbLibGetHeader (Gfx)));
  GfxPowerPlayAddDpmStateToSwState (State, ClkStateIndex);

  // Create Thermal State
  State = GfxPowerPlayCreateSwState (SwStateArray);
  State->Classification = ATOM_PPLIB_CLASSIFICATION_THERMAL;
  Sclk = GfxFmCalculateClock (PpFuses->SclkThermDid, GnbLibGetHeader (Gfx));
  ClkStateIndex = GfxPowerPlayAddDpmState (DpmStateArray, Sclk, 0, GfxPowerPlayLocateTdp (PpFuses, Sclk, GnbLibGetHeader (Gfx)));
  GfxPowerPlayAddDpmStateToSwState (State, ClkStateIndex);

  //Copy state info to actual PP table
  StateArrayLength = GfxPowerPlayCopyStateInfo (
                       &PpTable->StateArray,
                       SwStateArray,
                       GnbLibGetHeader (Gfx)
                       );
  ClockArrayLength = GfxPowerPlayCopyClockInfo (
                       (CLOCK_INFO_ARRAY*) ((UINT8 *)&PpTable->StateArray + StateArrayLength),
                       DpmStateArray,
                       GnbLibGetHeader (Gfx)
                       );
  NonClockArrayLength = GfxPowerPlayCopyNonClockInfo (
                          (NON_CLOCK_INFO_ARRAY*) ((UINT8 *)&PpTable->StateArray + StateArrayLength + ClockArrayLength),
                          SwStateArray,
                          GnbLibGetHeader (Gfx)
                          );
  //Fill static info
  PpTable->sHeader.ucTableFormatRevision = 6;
  PpTable->sHeader.ucTableContentRevision = 1;
  PpTable->ucDataRevision = PpFuses->PPlayTableRev;
  PpTable->sThermalController.ucType = ATOM_PP_THERMALCONTROLLER_SUMO;
  PpTable->sThermalController.ucFanParameters = ATOM_PP_FANPARAMETERS_NOFAN;
  if ((Gfx->AmdPlatformType & AMD_PLATFORM_MOBILE) != 0) {
    PpTable->ulPlatformCaps |= ATOM_PP_PLATFORM_CAP_POWERPLAY;
  }
  PpTable->usStateArrayOffset = offsetof (ATOM_PPLIB_POWERPLAYTABLE3, StateArray);
  PpTable->usClockInfoArrayOffset = (USHORT) (offsetof (ATOM_PPLIB_POWERPLAYTABLE3, StateArray) + StateArrayLength);
  PpTable->usNonClockInfoArrayOffset = (USHORT) (offsetof (ATOM_PPLIB_POWERPLAYTABLE3, StateArray) + StateArrayLength + ClockArrayLength);
  PpTable->sHeader.usStructureSize = (USHORT) (offsetof (ATOM_PPLIB_POWERPLAYTABLE3, StateArray) + StateArrayLength + ClockArrayLength + NonClockArrayLength);
  PpTable->usFormatID = 7;
  GNB_DEBUG_CODE (
    GfxIntegratedDebugDumpPpTable (PpTable, Gfx);
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
  UINTN                 Index;
  UINTN                 DpmIndex;
  ATOM_PPLIB_STATE_V2   *StatesPtr;
  NON_CLOCK_INFO_ARRAY  *NonClockInfoArrayPtr;
  CLOCK_INFO_ARRAY      *ClockInfoArrayPtr;
  IDS_HDT_CONSOLE (GFX_MISC, "  < --- Power Play Table ------ > \n");

  IDS_HDT_CONSOLE (GFX_MISC, "  Table Revision = %d\n", PpTable->ucDataRevision
    );
  StatesPtr = PpTable->StateArray.States;
  NonClockInfoArrayPtr = (NON_CLOCK_INFO_ARRAY *) ((UINT8 *) PpTable + PpTable->usNonClockInfoArrayOffset);
  ClockInfoArrayPtr = (CLOCK_INFO_ARRAY *) ((UINT8 *) PpTable + PpTable->usClockInfoArrayOffset);
  for (Index = 0; Index < PpTable->StateArray.ucNumEntries; Index++) {
    IDS_HDT_CONSOLE (GFX_MISC, "  State #%d\n", Index + 1
      );
    IDS_HDT_CONSOLE (GFX_MISC, "    Classification 0x%x\n",
       NonClockInfoArrayPtr->NonClockInfo[StatesPtr->nonClockInfoIndex].usClassification
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
}
