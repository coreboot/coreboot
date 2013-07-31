/* $NoKeywords:$ */
/**
 * @file
 *
 * Integrated table info init
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 87902 $   @e \$Date: 2013-02-12 15:59:48 -0600 (Tue, 12 Feb 2013) $
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
#include  "amdlib.h"
#include  "Ids.h"
#include  "heapManager.h"
#include  "GeneralServices.h"
#include  "Gnb.h"
#include  "GnbF1Table.h"
#include  "GnbPcie.h"
#include  "GnbGfx.h"
#include  "GnbSbLib.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbGfxConfig.h"
#include  "GnbGfxInitLibV1.h"
#include  "GnbGfxFamServices.h"
#include  "GnbRegistersKB.h"
#include  "GnbRegisterAccKB.h"
#include  "GnbNbInitLibV1.h"
#include  "GfxConfigLib.h"
#include  "GfxIntegratedInfoTable.h"
#include  "GfxPwrPlayTable.h"
#include  "GfxLibKB.h"
#include  "GfxLibV3.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_GFXINTEGRATEDINFOTABLEKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define GFX_REFCLK                        100     // (in MHz) Reference clock is 100 MHz

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GfxMapEngineToDisplayPathKB (
  IN       PCIe_ENGINE_CONFIG          *Engine,
     OUT   EXT_DISPLAY_PATH            *DisplayPathList,
  IN       GFX_PLATFORM_CONFIG         *Gfx
  );

AGESA_STATUS
GfxIntInfoTableInterfaceKB (
  IN      AMD_CONFIG_PARAMS       *StdHeader
  );

CONST UINT8  DdiLaneConfigArrayKB [][4] = {
  {8,  11, 0, 0},
  {12, 15, 1, 1},
  {11, 8,  0, 0},
  {15, 12, 1, 1},
  {16, 19, 6, 6},
  {19, 16, 6, 6}
};

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize display path for given engine
 *
 *
 *
 * @param[in]   Engine            Engine configuration info
 * @param[out]  DisplayPathList   Display path list
 * @param[in]   Gfx               Pointer to global GFX configuration
 */

AGESA_STATUS
GfxMapEngineToDisplayPathKB (
  IN       PCIe_ENGINE_CONFIG          *Engine,
     OUT   EXT_DISPLAY_PATH            *DisplayPathList,
  IN       GFX_PLATFORM_CONFIG         *Gfx
  )
{
  AGESA_STATUS      Status;
  UINT8             PrimaryDisplayPathId;
  UINT8             SecondaryDisplayPathId;
  UINTN             DisplayPathIndex;
  PrimaryDisplayPathId = 0xff;
  SecondaryDisplayPathId = 0xff;
  for (DisplayPathIndex = 0; DisplayPathIndex <  (sizeof (DdiLaneConfigArrayKB) / 4); DisplayPathIndex++) {
    if (DdiLaneConfigArrayKB[DisplayPathIndex][0] == Engine->EngineData.StartLane &&
        DdiLaneConfigArrayKB[DisplayPathIndex][1] == Engine->EngineData.EndLane) {
      PrimaryDisplayPathId = DdiLaneConfigArrayKB[DisplayPathIndex][2];
      SecondaryDisplayPathId = DdiLaneConfigArrayKB[DisplayPathIndex][3];
      break;
    }
  }

  if (Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeDualLinkDVI ||
     (Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeLvds && PrimaryDisplayPathId != 0)) {
    // Display config invalid for KB
    PrimaryDisplayPathId = 0xff;
  }

  if (PrimaryDisplayPathId != 0xff) {
    ASSERT (Engine->Type.Ddi.DdiData.AuxIndex <= Aux3);
    IDS_HDT_CONSOLE (GFX_MISC, "  Allocate Display Connector at Primary sPath[%d]\n", PrimaryDisplayPathId);
    Engine->InitStatus |= INIT_STATUS_DDI_ACTIVE;
    GfxIntegratedCopyDisplayInfo (
      Engine,
      &DisplayPathList[PrimaryDisplayPathId],
      (PrimaryDisplayPathId != SecondaryDisplayPathId) ? &DisplayPathList[SecondaryDisplayPathId] : NULL,
      Gfx
      );

    if (Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeSingleLinkDviI) {
      LibAmdMemCopy (&DisplayPathList[6], &DisplayPathList[PrimaryDisplayPathId], sizeof (EXT_DISPLAY_PATH), GnbLibGetHeader (Gfx));
      DisplayPathList[6].usDeviceACPIEnum = 0x100;
      DisplayPathList[6].usDeviceTag = ATOM_DEVICE_CRT1_SUPPORT;
    }

    Status = AGESA_SUCCESS;
  } else {
    IDS_HDT_CONSOLE (GFX_MISC, "  Error!!! Map DDI lanes %d - %d to display path failed\n",
      Engine->EngineData.StartLane,
      Engine->EngineData.EndLane
      );
    PutEventLog (
      AGESA_ERROR,
      GNB_EVENT_INVALID_DDI_LINK_CONFIGURATION,
      Engine->EngineData.StartLane,
      Engine->EngineData.EndLane,
      0,
      0,
      GnbLibGetHeader (Gfx)
      );
    Status = AGESA_ERROR;
  }
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 *Init KB Nb p-State MemclkFreq
 *
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

STATIC VOID
GfxFillNbPstateMemclkFreqKB (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V1_8 *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  D18F2x94_dct0_STRUCT            D18F2x94;
  D18F2x2E0_dct0_STRUCT           D18F2x2E0;
  D18F5x160_STRUCT                NbPstate;
  UINT8                           i;
  ULONG                           memps0_freq;
  ULONG                           memps1_freq;
  UINT8                           last_valid_pstate;
  GNB_HANDLE        *GnbHandle;

  GnbHandle = GnbGetHandle (GnbLibGetHeader (Gfx));
  ASSERT (GnbHandle != NULL);

  GnbRegisterReadKB (
    GnbHandle,
    D18F2x94_dct0_TYPE,
    D18F2x94_dct0_ADDRESS,
    &D18F2x94.Value,
    0,
    GnbLibGetHeader (Gfx)
    );

  GnbRegisterReadKB (
    GnbHandle,
    D18F2x2E0_dct0_TYPE,
    D18F2x2E0_dct0_ADDRESS,
    &D18F2x2E0.Value,
    0,
    GnbLibGetHeader (Gfx)
    );

  memps0_freq = 100 * GfxLibExtractDramFrequencyV3 ((UINT8) D18F2x94.Field.MemClkFreq, GnbLibGetHeader (Gfx));
  memps1_freq = 100 * GfxLibExtractDramFrequencyV3 ((UINT8) D18F2x2E0.Field.M1MemClkFreq, GnbLibGetHeader (Gfx));

  last_valid_pstate = 0;
  for (i = 0; i < 4; i++) {
    NbPstate.Value = 0;
    GnbRegisterReadKB (
      GnbHandle,
      TYPE_D18F5,
      (D18F5x160_ADDRESS + (i * 4)),
      &NbPstate.Value,
      0,
      GnbLibGetHeader (Gfx)
      );
    if (NbPstate.Field.NbPstateEn == 1) {
      last_valid_pstate = i;
      IntegratedInfoTable->ulNbpStateMemclkFreq[i] = (NbPstate.Field.MemPstate == 0) ? memps0_freq : memps1_freq;
    } else {
      IntegratedInfoTable->ulNbpStateMemclkFreq[i] =
          IntegratedInfoTable->ulNbpStateMemclkFreq[last_valid_pstate];
    }
  }

  for (i = 0; i < 4; i++) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  Nclk[%d] = %08x\n", i, IntegratedInfoTable->ulNbpStateNClkFreq[i]);
    IDS_HDT_CONSOLE (GNB_TRACE, "  Mclk[%d] = %08x\n", i, IntegratedInfoTable->ulNbpStateMemclkFreq[i]);
  }

}

/*----------------------------------------------------------------------------------------*/
/**
 *Calculate ulGMCRestoreResetTime
 *
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 * @param[in] PpF1Array
 * @retval    AGESA_STATUS
 */
///@todo - this is currently based on TN
STATIC AGESA_STATUS
GfxCalculateRestoreResetTimeKB (
  IN       ATOM_INTEGRATED_SYSTEM_INFO_V1_8 *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx,
  IN       PP_F1_ARRAY_V2                  *PpF1Array
  )
{
  UINT8      MaxDid;
  ULONG      FreqSclk;
  UINTN      Index;
  UINT32     TSclk;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxCalculateRestoreResetTimeKB Enter\n");
  MaxDid = PpF1Array->PP_FUSE_ARRAY_V2_fld3;
  for (Index = 0; Index < 4; Index++) {
    MaxDid = MAX (MaxDid, PpF1Array->PP_FUSE_ARRAY_V2_fld33[Index]);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "MaxDid = %d\n", MaxDid);
  FreqSclk = GfxFmCalculateClock (MaxDid, GnbLibGetHeader (Gfx));
  // FreqSclk is in 10KHz units - need calculations in nS
  // For accuracy, do calculations in .01nS, then convert at the end
  TSclk = (100 * (1000000000 / 10000)) / FreqSclk;

  IntegratedInfoTable->ulGMCRestoreResetTime = ((TSclk * 662) + 99) / 100;
  IDS_HDT_CONSOLE (GNB_TRACE, "ulGMCRestoreResetTime = %d\n", IntegratedInfoTable->ulGMCRestoreResetTime);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxCalculateRestoreResetTimeKB Exit\n");

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 *Init KB HTC Data
 *
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

STATIC VOID
GfxFillHtcDataKB (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V1_8 *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  D18F3x64_STRUCT  D18F3x64;
  GNB_HANDLE        *GnbHandle;

  GnbHandle = GnbGetHandle (GnbLibGetHeader (Gfx));
  ASSERT (GnbHandle != NULL);

  GnbRegisterReadKB (
    GnbHandle,
    D18F3x64_TYPE,
    D18F3x64_ADDRESS,
    &D18F3x64.Value,
    0,
    GnbLibGetHeader (Gfx)
    );

  if (D18F3x64.Field.HtcEn == 1) {
    IntegratedInfoTable->ucHtcTmpLmt = (UCHAR) (D18F3x64.Field.HtcTmpLmt / 2 + 52);
    IntegratedInfoTable->ATOM_INTEGRATED_SYSTEM_INFO_V1_8_fld11 = (UCHAR) (D18F3x64.Field.HtcHystLmt / 2);
  } else {
    IntegratedInfoTable->ucHtcTmpLmt = 0;
    IntegratedInfoTable->ATOM_INTEGRATED_SYSTEM_INFO_V1_8_fld11 = 0;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 *Init Sclk <-> VID table
 *
 *
 * @param[in] PpF1Array
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

STATIC VOID
GfxIntInfoTableInitSclkTableKB (
  IN       PP_F1_ARRAY_V2                  *PpF1Array,
  IN       ATOM_INTEGRATED_SYSTEM_INFO_V1_8 *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  UINT8                       Index;
  UINTN                       AvailSclkIndex;
  GnbGfx275_STRUCT    *AvailSclkList;
  BOOLEAN                     Sorting;
  AvailSclkList = &IntegratedInfoTable->ATOM_INTEGRATED_SYSTEM_INFO_V1_8[0];

  AvailSclkIndex = 0;
  for (Index = 0; Index < 5; Index++) {
    if (PpF1Array->PP_FUSE_ARRAY_V2_fld33[Index] != 0) {
      AvailSclkList[AvailSclkIndex].GnbGfx275_STRUCT_fld0 = GfxFmCalculateClock (PpF1Array->PP_FUSE_ARRAY_V2_fld33[Index], GnbLibGetHeader (Gfx));
      AvailSclkList[AvailSclkIndex].GnbGfx275_STRUCT_fld1 = Index;
      AvailSclkList[AvailSclkIndex].GnbGfx275_STRUCT_fld2 = PpF1Array->PP_FUSE_ARRAY_V2_fld32[Index];
      AvailSclkIndex++;
    }
  }
  //Sort by VoltageIndex & GnbGfx275_STRUCT_fld0
  if (AvailSclkIndex > 1) {
    do {
      Sorting = FALSE;
      for (Index = 0; Index < (AvailSclkIndex - 1); Index++) {
        GnbGfx275_STRUCT  Temp;
        BOOLEAN                   Exchange;
        Exchange = FALSE;
        if (AvailSclkList[Index].GnbGfx275_STRUCT_fld1 > AvailSclkList[Index + 1].GnbGfx275_STRUCT_fld1) {
          Exchange = TRUE;
        }
        if ((AvailSclkList[Index].GnbGfx275_STRUCT_fld1 == AvailSclkList[Index + 1].GnbGfx275_STRUCT_fld1) &&
            (AvailSclkList[Index].GnbGfx275_STRUCT_fld0 > AvailSclkList[Index + 1].GnbGfx275_STRUCT_fld0)) {
          Exchange = TRUE;
        }
        if (Exchange) {
          Sorting = TRUE;
          LibAmdMemCopy (&Temp, &AvailSclkList[Index], sizeof (GnbGfx275_STRUCT), GnbLibGetHeader (Gfx));
          LibAmdMemCopy (&AvailSclkList[Index], &AvailSclkList[Index + 1], sizeof (GnbGfx275_STRUCT), GnbLibGetHeader (Gfx));
          LibAmdMemCopy (&AvailSclkList[Index + 1], &Temp, sizeof (GnbGfx275_STRUCT), GnbLibGetHeader (Gfx));
        }
      }
    } while (Sorting);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Build integrated info table
 *
 *
 *
 * @param[in] Gfx             Gfx configuration info
 * @retval    AGESA_STATUS
 */
AGESA_STATUS
STATIC
GfxIntInfoTableInitKB (
  IN      GFX_PLATFORM_CONFIG     *Gfx
  )
{
  AGESA_STATUS                    Status;
  AGESA_STATUS                    AgesaStatus;
  ATOM_FUSION_SYSTEM_INFO_V3      SystemInfoTableV3;
  PP_F1_ARRAY_V2                *PpF1Array;
  ATOM_PPLIB_POWERPLAYTABLE4      *PpTable;
  D18F5x170_STRUCT                D18F5x170;
  GNB_HANDLE        *GnbHandle;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntInfoTableInitKB Enter\n");

  AgesaStatus = AGESA_SUCCESS;
  GnbHandle = GnbGetHandle (GnbLibGetHeader (Gfx));
  ASSERT (GnbHandle != NULL);
  PpF1Array = GnbLocateHeapBuffer (AMD_PP_F1_TABLE_HANDLE, GnbLibGetHeader (Gfx));
  if (PpF1Array != NULL) {
    LibAmdMemFill (&SystemInfoTableV3, 0x00, sizeof (ATOM_FUSION_SYSTEM_INFO_V3), GnbLibGetHeader (Gfx));

    // Use common initialization first
    Status = GfxIntInfoTableInitV3 (Gfx, &SystemInfoTableV3, PpF1Array);
    // Complete table with KB-specific fields

    // Build PP table
    PpTable = (ATOM_PPLIB_POWERPLAYTABLE4*) &SystemInfoTableV3.ulPowerplayTable;
    Status = GfxPwrPlayBuildTable (PpTable,  Gfx);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);

    IDS_HDT_CONSOLE (GNB_TRACE, "KB Pplay done\n");

    // Assign usFormatID to 0x0013 to represent Kabini
    PpTable->usFormatID = 0x13;

    // Build Sclk info table
    GfxIntInfoTableInitSclkTableKB (PpF1Array, &SystemInfoTableV3.sIntegratedSysInfo, Gfx);

    // Fill in Nb P-state MemclkFreq Data
    GfxFillNbPstateMemclkFreqKB (&SystemInfoTableV3.sIntegratedSysInfo, Gfx);
    // Fill in HTC Data
    GfxFillHtcDataKB (&SystemInfoTableV3.sIntegratedSysInfo, Gfx);

    // Family specific data update
    // Determine ulGMCRestoreResetTime
    Status = GfxCalculateRestoreResetTimeKB (&SystemInfoTableV3.sIntegratedSysInfo, Gfx, PpF1Array);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);

    //GfxFmIntegratedInfoTableInit (&SystemInfoV1Table.sIntegratedSysInfo, Gfx);
    SystemInfoTableV3.sIntegratedSysInfo.ulDDR_DLL_PowerUpTime = 4940;
    SystemInfoTableV3.sIntegratedSysInfo.ulDDR_PLL_PowerUpTime = 2000;

    SystemInfoTableV3.sIntegratedSysInfo.ulGPUCapInfo = GPUCAPINFO_DFS_BYPASS_DISABLE;

    // GPUCAPINFO_DFS_BYPASS_ENABLE should be enabled by default for MOBILE systems
    if ((Gfx->AmdPlatformType & AMD_PLATFORM_MOBILE) != 0) {
      SystemInfoTableV3.sIntegratedSysInfo.ulGPUCapInfo |= GPUCAPINFO_DFS_BYPASS_ENABLE;
    }

    // Check if NbPstate enable
    GnbRegisterReadKB (GnbHandle, TYPE_D18F5, D18F5x170_ADDRESS, &D18F5x170.Value, 0, GnbLibGetHeader (Gfx));
    if ((D18F5x170.Field.SwNbPstateLoDis != 1) && (D18F5x170.Field.NbPstateMaxVal != 0)) {
      // If NbPstate enable, then enable NBDPM for driver
      SystemInfoTableV3.sIntegratedSysInfo.ulSystemConfig |= BIT3;
    }

    IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_INTEGRATED_TABLE_CONFIG, &SystemInfoTableV3.sIntegratedSysInfo, GnbLibGetHeader (Gfx));
    //Copy integrated info table to Frame Buffer. (Do not use LibAmdMemCopy, routine not guaranteed access to above 4G memory in 32 bit mode.)
    GfxIntInfoTablePostToFbV3 (&SystemInfoTableV3, Gfx);

    GNB_DEBUG_CODE (
      GfxIntInfoTableDebugDumpV3 (&SystemInfoTableV3, Gfx);
    );
  } else {
    Status = AGESA_ERROR;
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntInfoTableInitKB Exit [0x%x]\n", Status);
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Build integrated info table
 *  GMC FB access requred
 *
 *
 * @param[in]   StdHeader     Standard configuration header
 * @retval      AGESA_STATUS
 */
AGESA_STATUS
GfxIntInfoTableInterfaceKB (
  IN      AMD_CONFIG_PARAMS       *StdHeader
  )
{
  AGESA_STATUS                    AgesaStatus;
  AGESA_STATUS                    Status;
  GFX_PLATFORM_CONFIG             *Gfx;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntInfoTableInterfaceKB Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  if (GfxLibIsControllerPresent (StdHeader)) {
    Status = GfxLocateConfigData (StdHeader, &Gfx);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    if (Status != AGESA_FATAL) {
      Status = GfxIntInfoTableInitKB (Gfx);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntInfoTableInterfaceKB Exit[0x%x]\n", AgesaStatus);
  return AgesaStatus;
}
