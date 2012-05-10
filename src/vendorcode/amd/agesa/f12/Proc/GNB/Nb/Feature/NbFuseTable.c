/* $NoKeywords:$ */
/**
 * @file
 *
 * Fuse table initialization
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 48452 $   @e \$Date: 2011-03-09 12:50:44 +0800 (Wed, 09 Mar 2011) $
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include  "AGESA.h"
#include  "amdlib.h"
#include  "Ids.h"
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbFuseTable.h"
#include  "GnbCommonLib.h"
#include  "GnbNbInitLibV1.h"
#include  "OptionGnb.h"
#include  "GnbRegistersLN.h"
#include  "NbSmuLib.h"
#include  "NbConfigData.h"
#include  "NbFuseTable.h"
#include  "NbFamilyServices.h"
#include  "GfxLib.h"
#include  "Filecode.h"

#define FILECODE PROC_GNB_NB_FEATURE_NBFUSETABLE_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
extern GNB_BUILD_OPTIONS  GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
NbFuseLoadDefaultFuseTable (
  OUT     PP_FUSE_ARRAY       *PpFuseArray,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbFuseLoadFuseTableFromFcr (
  OUT     PP_FUSE_ARRAY       *PpFuseArray,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbFuseDebugDump (
  IN       PP_FUSE_ARRAY       *PpFuseArray,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbFuseAdjustFuseTableToCurrentMainPllVco (
  IN  OUT   PP_FUSE_ARRAY      *PpFuseArray,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

PP_FUSE_ARRAY  DefaultPpFuseArray = {
  0,                              ///< PP table revision
  {1,    0,  0,  0,  0,  0},      ///< Valid DPM states
  {0x40, 0,  0,  0,  0,  0},      ///< Sclk DPM DID
  {0,    0,  0,  0,  0,  0},      ///< Sclk DPM VID
  {0,    0,  0,  0,  0},          ///< Sclk DPM Cac
  {1,    0,  0,  0,  0,  0},      ///< State policy flags
  {2,    0,  0,  0,  0,  0},      ///< State policy label
  {0x40, 0,  0,  0},              ///< VCLK DID
  {0x40, 0,  0,  0},              ///< DCLK DID
  0x40,                           ///< Thermal SCLK
  {0,    0,  0,  0,  0,  0},      ///< Vclk/Dclk selector
  {0,    0,  0,  0},              ///< Valid Lclk DPM states
  {0,    0,  0,  0},              ///< Lclk DPM DID
  {0,    0,  0,  0},              ///< Lclk DPM VID
  {0,    0,  0,  0},              ///< Displclk DID
  3,                              ///< Pcie Gen 2 VID
  0x10,                           ///< Main PLL id for 3200 VCO
  0,                              ///< WRCK SMU clock Divisor
  {0x24, 0x24, 0x24, 0x24}        ///< Sclk VID
};


/*----------------------------------------------------------------------------------------*/
/**
 * Fuse Table Init
 *
 *
 *
 * @param[in] StdHeader        Pointer to Standard configuration
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
NbFuseTableFeature (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  PP_FUSE_ARRAY          *PpFuseArray;
  D18F3xA0_STRUCT        D18F3xA0;
  BOOLEAN                LoadDefaultFuses;
  IDS_HDT_CONSOLE (GNB_TRACE, "NbFuseTableFeature Enter\n");

  PpFuseArray = (PP_FUSE_ARRAY *) GnbAllocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, sizeof (PP_FUSE_ARRAY), StdHeader);
  ASSERT (PpFuseArray != NULL);
  if (PpFuseArray == NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  ERROR!!! Heap Allocation\n");
    return AGESA_ERROR;
  }
  LibAmdMemFill (PpFuseArray, 0x00, sizeof (PP_FUSE_ARRAY), StdHeader);
  GnbLibPciRead (
       MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3xA0_ADDRESS),
       AccessWidth32,
       &D18F3xA0.Value,
       StdHeader
     );

  LoadDefaultFuses = TRUE;
  if (GnbBuildOptions.GnbLoadRealFuseTable == 1) {
    if (D18F3xA0.Field.CofVidProg == 1) {
      IDS_HDT_CONSOLE (NB_MISC, "  Processor Fused\n");
      NbFuseLoadFuseTableFromFcr (PpFuseArray, StdHeader);
      if (PpFuseArray->PPlayTableRev != 0) {
        LoadDefaultFuses = FALSE;
      } else {
        IDS_HDT_CONSOLE (NB_MISC, "  PowerPlay Table Unfused\n");
      }
    } else {
      IDS_HDT_CONSOLE (NB_MISC, "  Processor Unfuse\n");
    }
  } else {
    IDS_HDT_CONSOLE (NB_MISC, "  Force default fuse table Unfuse\n");
  }

  if (LoadDefaultFuses) {
    IDS_HDT_CONSOLE (NB_MISC, "  Load default fuses\n");
    NbFuseLoadDefaultFuseTable (PpFuseArray, StdHeader);
  }
  NbFmFuseAdjustFuseTablePatch (PpFuseArray, StdHeader);
  NbFuseAdjustFuseTableToCurrentMainPllVco (PpFuseArray, StdHeader);
  IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_PPFUSE_OVERRIDE, PpFuseArray, StdHeader);
  GNB_DEBUG_CODE (
    NbFuseDebugDump (PpFuseArray, StdHeader)
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "NbFuseTableFeature Exit\n");
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Load Fuse Table From FCRs
 *
 *
 * @param[out] PpFuseArray      Pointer to save fuse table
 * @param[in]  StdHeader        Pointer to Standard configuration
 * @retval     AGESA_STATUS
 */

VOID
NbFuseLoadFuseTableFromFcr (
     OUT   PP_FUSE_ARRAY       *PpFuseArray,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  FUSE_TABLE             *FuseTable;
  UINTN                  RegisterIndex;
  FuseTable = NbFmGetFuseTranslationTable ();
  for (RegisterIndex = 0; RegisterIndex < FuseTable->FuseTableLength; RegisterIndex++ ) {
    UINTN   FieldIndex;
    UINTN   FuseRegisterTableLength;
    UINT32  FuseValue;
    FuseRegisterTableLength = FuseTable->FuseTable[RegisterIndex].FuseRegisterTableLength;
    FuseValue = NbSmuReadEfuse (
                  FuseTable->FuseTable[RegisterIndex].Register,
                  StdHeader
                  );
    for (FieldIndex = 0; FieldIndex < FuseRegisterTableLength;  FieldIndex++) {
      FUSE_REGISTER_ENTRY   RegisterEntry;
      UINT8                 *FuseArrayPtr;
      UINT32                FuseArrauValue;
      RegisterEntry = FuseTable->FuseTable[RegisterIndex].FuseRegisterTable[FieldIndex];
      FuseArrayPtr = (UINT8*) PpFuseArray + RegisterEntry.FuseOffset;
      FuseArrauValue = (FuseValue >> RegisterEntry.FieldOffset) & ((1 << RegisterEntry.FieldWidth) - 1);
      if (RegisterEntry.FieldWidth > 16) {
        *((UINT32 *) FuseArrayPtr) = FuseArrauValue;
      } else if (RegisterEntry.FieldWidth > 8) {
        *((UINT16 *) FuseArrayPtr) = (UINT16) FuseArrauValue;
      } else {
        *((UINT8 *) FuseArrayPtr) = (UINT8) FuseArrauValue;
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Load Default Fuse Table
 *
 *
 * @param[out] PpFuseArray      Pointer to save fuse table
 * @param[in]  StdHeader        Pointer to Standard configuration
 * @retval     AGESA_STATUS
 */

VOID
NbFuseLoadDefaultFuseTable (
     OUT   PP_FUSE_ARRAY       *PpFuseArray,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  D18F3x15C_STRUCT  D18F3x15C;
  UINT8             MaxVidIndex;
  LibAmdMemCopy (PpFuseArray, &DefaultPpFuseArray, sizeof (PP_FUSE_ARRAY), StdHeader);
  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3x15C_ADDRESS),
    AccessWidth32,
    &D18F3x15C.Value,
    StdHeader
    );
  if (D18F3x15C.Value == 0) {
    D18F3x15C.Value = 0x24242424;
    GnbLibPciWrite (
      MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3x15C_ADDRESS),
      AccessWidth32,
      &D18F3x15C.Value,
      StdHeader
      );
  }
  MaxVidIndex = GnbLocateHighestVidIndex (StdHeader);
  PpFuseArray->SclkDpmVid[0] = MaxVidIndex;
  PpFuseArray->PcieGen2Vid = MaxVidIndex;

}

/*----------------------------------------------------------------------------------------*/
/**
 * Adjust DIDs to current main PLL VCO
 *
 *  Main PLL VCO can be changed for debug perpouses
 *
 * @param[in,out] PpFuseArray      Pointer to save fuse table
 * @param[in]     StdHeader        Pointer to Standard configuration
 */

VOID
NbFuseAdjustFuseTableToCurrentMainPllVco (
  IN OUT   PP_FUSE_ARRAY       *PpFuseArray,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32  EffectiveMainPllFreq10KHz;
  UINT32  FusedMainPllFreq10KHz;
  UINT32  TempVco;
  UINTN   Index;
  EffectiveMainPllFreq10KHz = GfxLibGetMainPllFreq (StdHeader) * 100;
  FusedMainPllFreq10KHz = (PpFuseArray->MainPllId + 0x10) * 100 * 100;
  if (FusedMainPllFreq10KHz != EffectiveMainPllFreq10KHz) {
    IDS_HDT_CONSOLE (NB_MISC, "  WARNING! Adjusting fuse table for reprogrammed VCO \n");
    IDS_HDT_CONSOLE (NB_MISC, "  Actual main Freq %d \n", EffectiveMainPllFreq10KHz);
    IDS_HDT_CONSOLE (NB_MISC, "  Fused  main Freq %d \n", FusedMainPllFreq10KHz);
    for (Index = 0; Index < 5; Index++) {
      if (PpFuseArray->SclkDpmDid[Index] != 0) {
        TempVco = GfxLibCalculateClk (PpFuseArray->SclkDpmDid[Index], FusedMainPllFreq10KHz);
        PpFuseArray->SclkDpmDid[Index] = GfxLibCalculateDid (TempVco, EffectiveMainPllFreq10KHz);
      }
    }
    for (Index = 0; Index < 4; Index++) {
      if (PpFuseArray->VclkDid[Index] != 0) {
        TempVco = GfxLibCalculateClk (PpFuseArray->VclkDid[Index], FusedMainPllFreq10KHz);
        PpFuseArray->VclkDid[Index] = GfxLibCalculateDid (TempVco, EffectiveMainPllFreq10KHz);
      }
      if (PpFuseArray->DclkDid[Index] != 0) {
        TempVco = GfxLibCalculateClk (PpFuseArray->DclkDid[Index], FusedMainPllFreq10KHz);
        PpFuseArray->DclkDid[Index] = GfxLibCalculateDid (TempVco, EffectiveMainPllFreq10KHz);
      }
      if (PpFuseArray->LclkDpmDid[Index] != 0) {
        TempVco = GfxLibCalculateClk (PpFuseArray->LclkDpmDid[Index], FusedMainPllFreq10KHz);
        PpFuseArray->LclkDpmDid[Index] = GfxLibCalculateDid (TempVco, EffectiveMainPllFreq10KHz);
      }
      if (PpFuseArray->DisplclkDid[Index] != 0) {
        TempVco = GfxLibCalculateClk (PpFuseArray->DisplclkDid[Index], FusedMainPllFreq10KHz);
        PpFuseArray->DisplclkDid[Index] = GfxLibCalculateDid (TempVco, EffectiveMainPllFreq10KHz);
      }
    }
    if (PpFuseArray->SclkThermDid != 0) {
      TempVco = GfxLibCalculateClk (PpFuseArray->SclkThermDid , FusedMainPllFreq10KHz);
      PpFuseArray->SclkThermDid = GfxLibCalculateDid (TempVco, EffectiveMainPllFreq10KHz);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Debug dump fuse table
 *
 *
 * @param[out] PpFuseArray      Pointer to save fuse table
 * @param[in]  StdHeader        Pointer to Standard configuration
 */

VOID
NbFuseDebugDump (
  IN       PP_FUSE_ARRAY       *PpFuseArray,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINTN   Index;
  UINT32  EffectiveMainPllFreq10KHz;

  EffectiveMainPllFreq10KHz = GfxLibGetMainPllFreq (StdHeader) * 100;
  IDS_HDT_CONSOLE (NB_MISC, "<------------ GNB FUSE TABLE------------>\n");
  for (Index = 0; Index < 4; Index++) {
    if (PpFuseArray->LclkDpmValid[Index]  != 0) {
      IDS_HDT_CONSOLE (
        NB_MISC,
        "  LCLK DID[%d] - 0x%02x (%dMHz)\n",
        Index,
        PpFuseArray->LclkDpmDid[Index],
        GfxLibCalculateClk (PpFuseArray->LclkDpmDid[Index], EffectiveMainPllFreq10KHz) / 100);
      IDS_HDT_CONSOLE (NB_MISC, "  LCLK VID[%d] - 0x02%x\n", Index, PpFuseArray->LclkDpmVid[Index]);
    }
  }
  for (Index = 0; Index < 4; Index++) {
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  VCLK DID[%d] - 0x%02x (%dMHz)\n",
      Index,
      PpFuseArray->VclkDid[Index],
      (PpFuseArray->VclkDid[Index] != 0) ? (GfxLibCalculateClk (PpFuseArray->VclkDid[Index], EffectiveMainPllFreq10KHz) / 100) : 0
      );
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  DCLK DID[%d] - 0x%02x (%dMHz)\n",
      Index,
      PpFuseArray->DclkDid[Index],
      (PpFuseArray->DclkDid[Index] != 0) ? (GfxLibCalculateClk (PpFuseArray->DclkDid[Index], EffectiveMainPllFreq10KHz) / 100) : 0
    );
  }
  for (Index = 0; Index < 4; Index++) {
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  DISPCLK DID[%d] - 0x%02x (%dMHz)\n",
      Index,
      PpFuseArray->DisplclkDid[Index],
      (PpFuseArray->DisplclkDid[Index] != 0) ? (GfxLibCalculateClk (PpFuseArray->DisplclkDid[Index], EffectiveMainPllFreq10KHz) / 100) : 0
      );
  }
  for (Index = 0; Index < 6; Index++) {
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  SCLK DID[%d] - 0x%02x (%dMHz)\n",
      Index,
      PpFuseArray->SclkDpmDid[Index],
      (PpFuseArray->SclkDpmDid[Index] != 0) ? (GfxLibCalculateClk (PpFuseArray->SclkDpmDid[Index], EffectiveMainPllFreq10KHz) / 100) : 0
    );
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  SCLK TDP[%d] - 0x%x \n",
      Index,
      PpFuseArray->SclkDpmTdpLimit[Index]
    );
    IDS_HDT_CONSOLE (NB_MISC, "  SCLK VID[%d] - 0x%02x\n", Index, PpFuseArray->SclkDpmVid[Index]);
  }
  for (Index = 0; Index < 6; Index++) {
    IDS_HDT_CONSOLE (NB_MISC, "  State #%d\n", Index);
    IDS_HDT_CONSOLE (NB_MISC, "    Policy Label    - 0x%x\n", PpFuseArray->PolicyLabel[Index]);
    IDS_HDT_CONSOLE (NB_MISC, "    Policy Flag     - 0x%x\n", PpFuseArray->PolicyFlags[Index]);
    IDS_HDT_CONSOLE (NB_MISC, "    Valid SCLK      - 0x%x\n", PpFuseArray->SclkDpmValid[Index]);
    IDS_HDT_CONSOLE (NB_MISC, "    Vclk/Dclk Index - 0x%x\n", PpFuseArray->VclkDclkSel[Index]);
  }
  IDS_HDT_CONSOLE (NB_MISC, "  GEN2 VID - 0x%x\n", PpFuseArray->PcieGen2Vid);
  IDS_HDT_CONSOLE (NB_MISC, "  Main PLL Id - 0x%x\n", PpFuseArray->MainPllId);
  IDS_HDT_CONSOLE (NB_MISC, "  GpuBoostCap  - %x\n", PpFuseArray->GpuBoostCap);
  IDS_HDT_CONSOLE (NB_MISC, "  SclkDpmBoostMargin    - %x\n", PpFuseArray->SclkDpmBoostMargin);
  IDS_HDT_CONSOLE (NB_MISC, "  SclkDpmThrottleMargin - %x\n", PpFuseArray->SclkDpmThrottleMargin);
  IDS_HDT_CONSOLE (NB_MISC, "  SclkDpmTdpLimitPG     - %x\n", PpFuseArray->SclkDpmTdpLimitPG);
  IDS_HDT_CONSOLE (
    NB_MISC, "  SclkThermDid          - %x(%dMHz)\n",
    PpFuseArray->SclkThermDid,
    (PpFuseArray->SclkThermDid != 0) ? (GfxLibCalculateClk (PpFuseArray->SclkThermDid, EffectiveMainPllFreq10KHz) / 100) : 0
    );
  IDS_HDT_CONSOLE (NB_MISC, "<------------ GNB FUSE END-------------->\n");
}
