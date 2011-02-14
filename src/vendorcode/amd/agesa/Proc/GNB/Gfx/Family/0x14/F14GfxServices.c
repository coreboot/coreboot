/* $NoKeywords:$ */
/**
 * @file
 *
 * Graphics Controller family specific service procedure
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 39275 $   @e \$Date: 2010-10-09 08:22:05 +0800 (Sat, 09 Oct 2010) $
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
#include  "amdlib.h"
#include  "Ids.h"
#include  "GeneralServices.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbGfx.h"
#include  "GnbRegistersON.h"
#include  "GfxIntegratedInfoTableInit.h"
#include  "GfxRegisterAcc.h"
#include  "GfxLib.h"
#include  GNB_MODULE_DEFINITIONS (GnbGfxInitLibV1)
#include  "GnbRegistersON.h"
#include  "F14NbPowerGate.h"
#include  "cpuFamilyTranslation.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_GFX_FAMILY_0X14_F14GFXSERVICES_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */




/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

UINT8 NumberOfChannels = 1;

UINT8  DdiLaneConfigArray [][4] = {
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
GfxFmMapEngineToDisplayPath (
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
  for (DisplayPathIndex = 0; DisplayPathIndex <  (sizeof (DdiLaneConfigArray) / 4); DisplayPathIndex++) {
    if (DdiLaneConfigArray[DisplayPathIndex][0] == Engine->EngineData.StartLane &&
        DdiLaneConfigArray[DisplayPathIndex][1] == Engine->EngineData.EndLane) {
      PrimaryDisplayPathId = DdiLaneConfigArray[DisplayPathIndex][2];
      SecondaryDisplayPathId = DdiLaneConfigArray[DisplayPathIndex][3];
      break;
    }
  }
  if (Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeDualLinkDVI ||
     (Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeLvds && PrimaryDisplayPathId != 0)) {
    // Display config invalid for ON
    PrimaryDisplayPathId = 0xff;
  }
  if (PrimaryDisplayPathId != 0xff) {
    ASSERT (Engine->Type.Ddi.DdiData.AuxIndex <= Aux3);
    IDS_HDT_CONSOLE (GFX_MISC, "  Allocate Display Connector at Primary sPath[%d]\n", PrimaryDisplayPathId);
    Engine->InitStatus |= INIT_STATUS_DDI_ACTIVE;
    if (Engine->Type.Ddi.DdiData.AuxIndex == Aux3) {
      Engine->Type.Ddi.DdiData.AuxIndex = 7;
    }
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
    IDS_HDT_CONSOLE (GFX_MISC, "  ERROR!!! Map DDI lanes %d - %d to display path failed\n",
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
 * Family specific integrated info table init
 *
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

VOID
GfxFmIntegratedInfoTableInit (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  IntegratedInfoTable->ulDDR_DLL_PowerUpTime = 2380;
  IntegratedInfoTable->ulDDR_PLL_PowerUpTime = 30000;
  IntegratedInfoTable->ulGMCRestoreResetTime = F14NbPowerGateGmcRestoreLatency (GnbLibGetHeader (Gfx));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Family specific address swizzle settings.
 *
 *
 * @param[in] Gfx                 Gfx configuration info
 */

VOID
GfxFmGmcAddressSwizzel (
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  GMMx2864_STRUCT GMMx2864;
  GMMx2864.Value = GmmRegisterRead (GMMx2864_ADDRESS, Gfx);
  if (GMMx2864.Value == 0) {
    GMMx2864.Value = 0x32100876;

    GmmRegisterWrite (
      GMMx2864_ADDRESS,
      GMMx2864.Value,
      TRUE,
      Gfx
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Calculate COF for DFS out of Main PLL
 *
 *
 *
 * @param[in] Did         Did
 * @param[in] StdHeader   Standard Configuration Header
 * @retval                COF in 10khz
 */

AGESA_STATUS
GfxFmCalculateClock (
  IN       UINT8                       Did,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  UINT32  MainPllFreq10kHz;
  MainPllFreq10kHz = GfxLibGetMainPllFreq (StdHeader) * 100;
  return GfxLibCalculateClk (Did, MainPllFreq10kHz);
}
/*----------------------------------------------------------------------------------------
 *           GMC Disable Clock Gating
 *----------------------------------------------------------------------------------------
 */

GMM_REG_ENTRY  GmcDisableClockGating[] = {
  { 0x20C0, 0x00000C80 },
  { 0x20B8, 0x00000400 },
  { 0x20BC, 0x00000400 },
  { 0x2640, 0x00000400 },
  { 0x263C, 0x00000400 },
  { 0x2638, 0x00000400 },
  { 0x15C0, 0x00081401 }
};

TABLE_INDIRECT_PTR GmcDisableClockGatingPtr = {
  sizeof (GmcDisableClockGating) / sizeof (GMM_REG_ENTRY),
  GmcDisableClockGating
};

/*----------------------------------------------------------------------------------------
 *           GMC Enable Clock Gating
 *----------------------------------------------------------------------------------------
 */
GMM_REG_ENTRY  GmcEnableClockGating[] = {
  { 0x20C0, 0x00040C80 },
  { 0x20B8, 0x00040400 },
  { 0x20BC, 0x00040400 },
  { 0x2640, 0x00040400 },
  { 0x263C, 0x00040400 },
  { 0x2638, 0x00040400 },
  { 0x15C0, 0x000C1401 }
};


TABLE_INDIRECT_PTR GmcEnableClockGatingPtr = {
  sizeof (GmcEnableClockGating) / sizeof (GMM_REG_ENTRY),
  GmcEnableClockGating
};

/*----------------------------------------------------------------------------------------
 *           GMC Performance Tuning
 *----------------------------------------------------------------------------------------
 */
GMM_REG_ENTRY  GmcPerformanceTuningTable [] = {
  { GMMx27CC_ADDRESS, 0x00032005 },
  { GMMx27DC_ADDRESS, 0x00734847 },
  { GMMx27D0_ADDRESS, 0x00012008 },
  { GMMx27E0_ADDRESS, 0x00003D3C },
  { GMMx2784_ADDRESS, 0x00000007 },
  { GMMx21C8_ADDRESS, 0x0000A1F1 },
  { GMMx217C_ADDRESS, 0x0000A1F1 },
  { GMMx2188_ADDRESS, 0x000221b1 },
  { GMMx2814_ADDRESS, 0x00000200 },
  { GMMx201C_ADDRESS, 0x03330003 },
  { GMMx2020_ADDRESS, 0x70760007 },
  { GMMx2018_ADDRESS, 0x00000050 },
  { GMMx2014_ADDRESS, 0x00005500 },
  { GMMx2610_ADDRESS, 0x44111222 },
  { GMMx2618_ADDRESS, 0x00006664 },
  { GMMx2614_ADDRESS, 0x11333111 },
  { GMMx261C_ADDRESS, 0x00000003 },
  { GMMx279C_ADDRESS, 0xfcfcfdfc },
  { GMMx27A0_ADDRESS, 0xfcfcfdfc }
};

TABLE_INDIRECT_PTR GmcPerformanceTuningTablePtr = {
  sizeof (GmcPerformanceTuningTable) / sizeof (GMM_REG_ENTRY),
  GmcPerformanceTuningTable
};

/*----------------------------------------------------------------------------------------
 *           GMC Misc init table
 *----------------------------------------------------------------------------------------
 */
GMM_REG_ENTRY  GmcMiscInitTable [] = {
  { GMMx25C8_ADDRESS, 0x007F605F },
  { GMMx25CC_ADDRESS, 0x00007F7E },
  { 0x20B4, 0x00000000 },
  { GMMx28C8_ADDRESS, 0x00000003 },
  { GMMx202C_ADDRESS, 0x0003FFFF }
};

TABLE_INDIRECT_PTR GmcMiscInitTablePtr = {
  sizeof (GmcMiscInitTable) / sizeof (GMM_REG_ENTRY),
  GmcMiscInitTable
};

/*----------------------------------------------------------------------------------------
 *           GMC Remove blackout
 *----------------------------------------------------------------------------------------
 */
GMM_REG_ENTRY  GmcRemoveBlackoutTable [] = {
  { GMMx25C0_ADDRESS, 0x00000000 },
  { 0x20EC, 0x000001FC },
  { 0x20D4, 0x00000016 }
};

TABLE_INDIRECT_PTR GmcRemoveBlackoutTablePtr = {
  sizeof (GmcRemoveBlackoutTable) / sizeof (GMM_REG_ENTRY),
  GmcRemoveBlackoutTable
};

/*----------------------------------------------------------------------------------------
 *           GMC Register Engine Init Table
 *----------------------------------------------------------------------------------------
 */
GMM_REG_ENTRY GmcRegisterEngineInitTable [] = {
  { GMMx2B8C_ADDRESS, 0x00000000 },
  { GMMx2B90_ADDRESS, 0x001e0a07 },
  { GMMx2B8C_ADDRESS, 0x00000020 },
  { GMMx2B90_ADDRESS, 0x00050500 },
  { GMMx2B8C_ADDRESS, 0x00000027 },
  { GMMx2B90_ADDRESS, 0x0001050c },
  { GMMx2B8C_ADDRESS, 0x0000002a },
  { GMMx2B90_ADDRESS, 0x0001051c },
  { GMMx2B8C_ADDRESS, 0x0000002d },
  { GMMx2B90_ADDRESS, 0x00030534 },
  { GMMx2B8C_ADDRESS, 0x00000032 },
  { GMMx2B90_ADDRESS, 0x0001053e },
  { GMMx2B8C_ADDRESS, 0x00000035 },
  { GMMx2B90_ADDRESS, 0x00010546 },
  { GMMx2B8C_ADDRESS, 0x00000038 },
  { GMMx2B90_ADDRESS, 0x0002054e },
  { GMMx2B8C_ADDRESS, 0x0000003c },
  { GMMx2B90_ADDRESS, 0x00010557 },
  { GMMx2B8C_ADDRESS, 0x0000003f },
  { GMMx2B90_ADDRESS, 0x0001055f },
  { GMMx2B8C_ADDRESS, 0x00000042 },
  { GMMx2B90_ADDRESS, 0x00010567 },
  { GMMx2B8C_ADDRESS, 0x00000045 },
  { GMMx2B90_ADDRESS, 0x0001056f },
  { GMMx2B8C_ADDRESS, 0x00000048 },
  { GMMx2B90_ADDRESS, 0x00050572 },
  { GMMx2B8C_ADDRESS, 0x0000004f },
  { GMMx2B90_ADDRESS, 0x00000800 },
  { GMMx2B8C_ADDRESS, 0x00000051 },
  { GMMx2B90_ADDRESS, 0x00260801 },
  { GMMx2B8C_ADDRESS, 0x00000079 },
  { GMMx2B90_ADDRESS, 0x004b082d },
  { GMMx2B8C_ADDRESS, 0x000000c6 },
  { GMMx2B90_ADDRESS, 0x0013088d },
  { GMMx2B8C_ADDRESS, 0x000000db },
  { GMMx2B90_ADDRESS, 0x100008a1 },
  { GMMx2B90_ADDRESS, 0x00000040 },
  { GMMx2B90_ADDRESS, 0x00000040 },
  { GMMx2B8C_ADDRESS, 0x000000df },
  { GMMx2B90_ADDRESS, 0x000008a2 },
  { GMMx2B8C_ADDRESS, 0x000000e1 },
  { GMMx2B90_ADDRESS, 0x0001094d },
  { GMMx2B8C_ADDRESS, 0x000000e4 },
  { GMMx2B90_ADDRESS, 0x00000952 },
  { GMMx2B8C_ADDRESS, 0x000000e6 },
  { GMMx2B90_ADDRESS, 0x00010954 },
  { GMMx2B8C_ADDRESS, 0x000000e9 },
  { GMMx2B90_ADDRESS, 0x0009095a },
  { GMMx2B8C_ADDRESS, 0x000000f4 },
  { GMMx2B90_ADDRESS, 0x0022096e },
  { GMMx2B8C_ADDRESS, 0x00000118 },
  { GMMx2B90_ADDRESS, 0x000e0997 },
  { GMMx2B8C_ADDRESS, 0x00000128 },
  { GMMx2B90_ADDRESS, 0x100009a6 },
  { GMMx2B90_ADDRESS, 0x00000040 },
  { GMMx2B90_ADDRESS, 0x00000040 },
  { GMMx2B8C_ADDRESS, 0x0000012c },
  { GMMx2B90_ADDRESS, 0x000009a7 },
  { GMMx2B8C_ADDRESS, 0x0000012e },
  { GMMx2B90_ADDRESS, 0x002e09d7 },
  { GMMx2B8C_ADDRESS, 0x0000015e },
  { GMMx2B90_ADDRESS, 0x00170a26 },
  { 0x2B94, 0x5d976000 },
  { 0x2B98, 0x410af020 }
};

TABLE_INDIRECT_PTR GmcRegisterEngineInitTablePtr = {
  sizeof (GmcRegisterEngineInitTable) / sizeof (GMM_REG_ENTRY),
  GmcRegisterEngineInitTable
};

/*----------------------------------------------------------------------------------------
 *           GMC Address Translation Table
 *----------------------------------------------------------------------------------------
 */
// Entries for Bank 1 will be fused out

REGISTER_COPY_ENTRY CnbToGncRegisterCopyTable [] = {
  {
    MAKE_SBDFO (0, 0, 0x18, 2, D18F2x40_ADDRESS),
    GMMx281C_ADDRESS,
    0,
    31,
    0,
    31
  },
  {
    MAKE_SBDFO (0, 0, 0x18, 2, D18F2x44_ADDRESS),
    GMMx2824_ADDRESS,
    0,
    31,
    0,
    31
  },
  {
    MAKE_SBDFO (0, 0, 0x18, 2, D18F2x48_ADDRESS),
    GMMx282C_ADDRESS,
    0,
    31,
    0,
    31
  },
  {
    MAKE_SBDFO (0, 0, 0x18, 2, D18F2x4C_ADDRESS),
    GMMx2834_ADDRESS,
    0,
    31,
    0,
    31
  },
  {
    MAKE_SBDFO (0, 0, 0x18, 2, D18F2x60_ADDRESS),
    GMMx283C_ADDRESS,
    0,
    31,
    0,
    31
  },
  {
    MAKE_SBDFO (0, 0, 0x18, 2, D18F2x64_ADDRESS),
    GMMx2840_ADDRESS,
    0,
    31,
    0,
    31
  },
  {
    MAKE_SBDFO (0, 0, 0x18, 2, D18F2x80_ADDRESS),
    GMMx284C_ADDRESS,
    D18F2x80_Dimm0AddrMap_OFFSET,
    D18F2x80_Dimm0AddrMap_WIDTH + D18F2x80_Dimm1AddrMap_WIDTH,
    GMMx284C_Dimm0AddrMap_OFFSET,
    GMMx284C_Dimm0AddrMap_WIDTH + GMMx284C_Dimm1AddrMap_WIDTH
  },
  {
    MAKE_SBDFO (0, 0, 0x18, 2, D18F2x94_ADDRESS),
    GMMx284C_ADDRESS,
    D18F2x94_BankSwizzleMode_OFFSET,
    D18F2x94_BankSwizzleMode_WIDTH,
    GMMx284C_BankSwizzleMode_OFFSET,
    GMMx284C_BankSwizzleMode_WIDTH
  },
  {
    MAKE_SBDFO (0, 0, 0x18, 2, D18F2xA8_ADDRESS),
    GMMx284C_ADDRESS,
    D18F2xA8_BankSwap_OFFSET,
    D18F2xA8_BankSwap_WIDTH,
    GMMx284C_BankSwap_OFFSET,
    GMMx284C_BankSwap_WIDTH
  },
  {
    MAKE_SBDFO (0, 0, 0x18, 2, D18F2x114_ADDRESS),
    GMMx2858_ADDRESS,
    0,
    31,
    0,
    31
  },
  {
    MAKE_SBDFO (0, 0, 0x18, 1, D18F1xF0_ADDRESS),
    GMMx285C_ADDRESS,
    0,
    31,
    0,
    31
  }
};


TABLE_INDIRECT_PTR CnbToGncRegisterCopyTablePtr = {
  sizeof (CnbToGncRegisterCopyTable) / sizeof (REGISTER_COPY_ENTRY),
  CnbToGncRegisterCopyTable
};

