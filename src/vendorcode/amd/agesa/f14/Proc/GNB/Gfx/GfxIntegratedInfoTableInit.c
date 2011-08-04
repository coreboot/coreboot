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
 * @e \$Revision: 48924 $   @e \$Date: 2011-03-14 12:45:15 -0600 (Mon, 14 Mar 2011) $
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
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbFuseTable.h"
#include  "GnbPcie.h"
#include  "GnbGfx.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  GNB_MODULE_DEFINITIONS (GnbPcieConfig)
#include  GNB_MODULE_DEFINITIONS (GnbGfxInitLibV1)
#include  GNB_MODULE_DEFINITIONS (GnbGfxConfig)
#include  "GfxLib.h"
#include  "GfxConfigData.h"
#include  "GfxRegisterAcc.h"
#include  "GfxFamilyServices.h"
#include  "GnbGfxFamServices.h"
#include  "GfxIntegratedInfoTableInit.h"
#include  "GnbRegistersON.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_GFX_GFXINTEGRATEDINFOTABLEINIT_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */



/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

ULONG ulCSR_M3_ARB_CNTL_DEFAULT[] = {
  0x80040810,
  0x00040810,
  0x00040810,
  0x00040810,
  0x00040810,
  0x00040810,
  0x00204080,
  0x00204080,
  0x0000001E,
  0x00000000
};


ULONG ulCSR_M3_ARB_CNTL_UVD[] = {
  0x80040810,
  0x00040810,
  0x00040810,
  0x00040810,
  0x00040810,
  0x00040810,
  0x00204080,
  0x00204080,
  0x0000001E,
  0x00000000
};


ULONG ulCSR_M3_ARB_CNTL_FS3D[] = {
  0x80040810,
  0x00040810,
  0x00040810,
  0x00040810,
  0x00040810,
  0x00040810,
  0x00204080,
  0x00204080,
  0x0000001E,
  0x00000000
};



/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

UINT32
GfxLibGetCsrPhySrPllPdMode (
  IN       UINT8       Channel,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

AGESA_STATUS
GfxIntegratedInfoTableEntry (
  IN      AMD_CONFIG_PARAMS       *StdHeader
  );

UINT32
GfxLibGetDisDllShutdownSR (
  IN       UINT8       Channel,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

VOID
GfxIntegratedInfoInitDispclkTable (
  IN       PP_FUSE_ARRAY                    *PpFuseArray,
  IN       ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  );

VOID
GfxIntegratedInfoInitSclkTable (
  IN       PP_FUSE_ARRAY                    *PpFuseArray,
  IN       ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  );

VOID
GfxFillHtcData (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  );

VOID
GfxFillNbPStateVid (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  );

VOID
GfxFillM3ArbritrationControl (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  );


VOID
GfxFillSbMmioBaseAddress (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  );

VOID
GfxFillNclkInfo (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  );

AGESA_STATUS
GfxIntegratedInfoTableInit (
  IN      GFX_PLATFORM_CONFIG               *Gfx
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Get CSR phy self refresh power down mode.
 *
 *
 * @param[in] Channel    DCT controller index
 * @param[in] StdHeader  Standard configuration header
 * @retval    CsrPhySrPllPdMode
 */
UINT32
GfxLibGetCsrPhySrPllPdMode (
  IN       UINT8       Channel,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  D18F2x09C_x0D0FE00A_STRUCT     D18F2x09C_x0D0FE00A;

  GnbLibCpuPciIndirectRead (
    MAKE_SBDFO ( 0, 0, 0x18, 2, (Channel == 0) ? D18F2x98_ADDRESS : D18F2x198_ADDRESS),
    D18F2x09C_x0D0FE00A_ADDRESS,
    &D18F2x09C_x0D0FE00A.Value,
    StdHeader
    );

  return  D18F2x09C_x0D0FE00A.Field.CsrPhySrPllPdMode;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get disable DLL shutdown in self-refresh mode.
 *
 *
 * @param[in] Channel    DCT controller index
 * @param[in] StdHeader  Standard configuration header
 * @retval    DisDllShutdownSR
 */
UINT32
GfxLibGetDisDllShutdownSR (
  IN       UINT8       Channel,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  D18F2x90_STRUCT D18F2x090;

  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 2, (Channel == 0) ? D18F2x90_ADDRESS : D18F2x190_ADDRESS),
    AccessWidth32,
    &D18F2x090.Value,
    StdHeader
    );

  return  D18F2x090.Field.DisDllShutdownSR;
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
GfxIntegratedInfoTableEntry (
  IN      AMD_CONFIG_PARAMS       *StdHeader
  )
{
  AGESA_STATUS                    AgesaStatus;
  AGESA_STATUS                    Status;
  GFX_PLATFORM_CONFIG             *Gfx;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntegratedInfoTableEntry Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  if (GfxLibIsControllerPresent (StdHeader)) {
    Status = GfxLocateConfigData (StdHeader, &Gfx);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    if (Status != AGESA_FATAL) {
      Status = GfxIntegratedInfoTableInit (Gfx);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntegratedInfoTableEntry Exit[0x%x]\n", AgesaStatus);
  return AgesaStatus;
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
GfxIntegratedInfoTableInit (
  IN      GFX_PLATFORM_CONFIG     *Gfx
  )
{
  AGESA_STATUS                    Status;
  AGESA_STATUS                    AgesaStatus;
  ATOM_FUSION_SYSTEM_INFO_V1      SystemInfoV1Table;
  PP_FUSE_ARRAY                   *PpFuseArray;
  PCIe_PLATFORM_CONFIG            *Pcie;
  UINT32                          IntegratedInfoAddress;
  ATOM_PPLIB_POWERPLAYTABLE3      *PpTable;
  UINT8                           Channel;

  AgesaStatus = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntegratedInfoTableInit Enter\n");
  LibAmdMemFill (&SystemInfoV1Table, 0x00, sizeof (ATOM_FUSION_SYSTEM_INFO_V1), GnbLibGetHeader (Gfx));
  SystemInfoV1Table.sIntegratedSysInfo.sHeader.usStructureSize = sizeof (ATOM_INTEGRATED_SYSTEM_INFO_V6);
  ASSERT (SystemInfoV1Table.sIntegratedSysInfo.sHeader.usStructureSize == 512);
  SystemInfoV1Table.sIntegratedSysInfo.sHeader.ucTableFormatRevision = 1;
  SystemInfoV1Table.sIntegratedSysInfo.sHeader.ucTableContentRevision = 6;
  SystemInfoV1Table.sIntegratedSysInfo.ulDentistVCOFreq = GfxLibGetMainPllFreq (GnbLibGetHeader (Gfx)) * 100;
  SystemInfoV1Table.sIntegratedSysInfo.ulBootUpUMAClock = Gfx->UmaInfo.MemClock * 100;
  SystemInfoV1Table.sIntegratedSysInfo.usRequestedPWMFreqInHz = Gfx->LcdBackLightControl;
  SystemInfoV1Table.sIntegratedSysInfo.ucUMAChannelNumber = ((Gfx->UmaInfo.UmaAttributes & UMA_ATTRIBUTE_INTERLEAVE) == 0) ? 1 : 2;
  SystemInfoV1Table.sIntegratedSysInfo.ucMemoryType = 3;     //DDR3
  SystemInfoV1Table.sIntegratedSysInfo.ulBootUpEngineClock = 200 * 100;    //Set default engine clock to 200MhZ
  SystemInfoV1Table.sIntegratedSysInfo.usBootUpNBVoltage = GfxLibMaxVidIndex (GnbLibGetHeader (Gfx));
  SystemInfoV1Table.sIntegratedSysInfo.ulMinEngineClock = GfxLibGetMinSclk (GnbLibGetHeader (Gfx));
  SystemInfoV1Table.sIntegratedSysInfo.usPanelRefreshRateRange = Gfx->DynamicRefreshRate;

  SystemInfoV1Table.sIntegratedSysInfo.usLvdsSSPercentage = Gfx->LvdsSpreadSpectrum;
  SystemInfoV1Table.sIntegratedSysInfo.usLvdsSSpreadRateIn10Hz = Gfx->LvdsSpreadSpectrumRate;
  SystemInfoV1Table.sIntegratedSysInfo.usPCIEClkSSPercentage = Gfx->PcieRefClkSpreadSpectrum;
//  SystemInfoV1Table.sIntegratedSysInfo.ucLvdsMisc = Gfx->LvdsMiscControl.Value;

  //Locate PCIe configuration data to get definitions of display connectors
  SystemInfoV1Table.sIntegratedSysInfo.sExtDispConnInfo.sHeader.usStructureSize = sizeof (ATOM_EXTERNAL_DISPLAY_CONNECTION_INFO);
  SystemInfoV1Table.sIntegratedSysInfo.sExtDispConnInfo.sHeader.ucTableFormatRevision = 1;
  SystemInfoV1Table.sIntegratedSysInfo.sExtDispConnInfo.sHeader.ucTableContentRevision = 1;
  SystemInfoV1Table.sIntegratedSysInfo.sExtDispConnInfo.uc3DStereoPinId = Gfx->Gnb3dStereoPinIndex;

  ASSERT ((Gfx->UmaInfo.UmaAttributes & (UMA_ATTRIBUTE_ON_DCT0 | UMA_ATTRIBUTE_ON_DCT1)) != 0);

  if ((Gfx->UmaInfo.UmaAttributes & UMA_ATTRIBUTE_ON_DCT0) != 0) {
    Channel = 0;
  } else {
    Channel = 1;
  }
  if (GfxLibGetCsrPhySrPllPdMode (Channel, GnbLibGetHeader (Gfx)) != 0) {
    SystemInfoV1Table.sIntegratedSysInfo.ulSystemConfig |= BIT2;
  }
  if (GfxLibGetDisDllShutdownSR (Channel, GnbLibGetHeader (Gfx)) == 0) {
    SystemInfoV1Table.sIntegratedSysInfo.ulSystemConfig |= BIT1;
  }
  Status = PcieLocateConfigurationData (GnbLibGetHeader (Gfx), &Pcie);
  ASSERT (Status == AGESA_SUCCESS);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    Status = GfxIntegratedEnumerateAllConnectors (
               &SystemInfoV1Table.sIntegratedSysInfo.sExtDispConnInfo.sPath[0],
               Pcie,
               Gfx
               );
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
  }
  SystemInfoV1Table.sIntegratedSysInfo.usExtDispConnInfoOffset = offsetof (ATOM_INTEGRATED_SYSTEM_INFO_V6, sExtDispConnInfo);
  // Build PP table
  PpTable = (ATOM_PPLIB_POWERPLAYTABLE3*) &SystemInfoV1Table.ulPowerplayTable;
  // Build PP table
  Status = GfxPowerPlayBuildTable (PpTable,  Gfx);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  // Build info from fuses
  PpFuseArray = GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, GnbLibGetHeader (Gfx));
  ASSERT (PpFuseArray != NULL);
  if (PpFuseArray != NULL) {
    // Build Display clock info
    GfxIntegratedInfoInitDispclkTable (PpFuseArray, &SystemInfoV1Table.sIntegratedSysInfo, Gfx);
    // Build Sclk info table
    GfxIntegratedInfoInitSclkTable (PpFuseArray, &SystemInfoV1Table.sIntegratedSysInfo, Gfx);
  } else {
    Status = AGESA_ERROR;
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
  }
  // Fill in HTC Data
  GfxFillHtcData (&SystemInfoV1Table.sIntegratedSysInfo, Gfx);
  // Fill in NB P states VID
  GfxFillNbPStateVid (&SystemInfoV1Table.sIntegratedSysInfo, Gfx);
  // Fill in NCLK info
  GfxFillNclkInfo (&SystemInfoV1Table.sIntegratedSysInfo, Gfx);
  // Fill in the M3 arbitration control tables
  GfxFillM3ArbritrationControl (&SystemInfoV1Table.sIntegratedSysInfo, Gfx);
  // Fill South bridge MMIO Base address
  GfxFillSbMmioBaseAddress (&SystemInfoV1Table.sIntegratedSysInfo, Gfx);
  // Family specific data update
  GfxFmIntegratedInfoTableInit (&SystemInfoV1Table.sIntegratedSysInfo, Gfx);
  IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_INTEGRATED_TABLE_CONFIG, &SystemInfoV1Table.sIntegratedSysInfo, GnbLibGetHeader (Gfx));
  //Copy integrated info table to Frame Buffer. (Do not use LibAmdMemCopy, routine not guaranteed access to above 4G memory in 32 bit mode.)
  IntegratedInfoAddress = (UINT32) (Gfx->UmaInfo.UmaSize - sizeof (ATOM_FUSION_SYSTEM_INFO_V1));
  GfxLibCopyMemToFb ((VOID *) (&SystemInfoV1Table), IntegratedInfoAddress, sizeof (ATOM_FUSION_SYSTEM_INFO_V1), Gfx);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntegratedInfoTableInit Exit [0x%x]\n", Status);
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 *Init Dispclk <-> VID table
 *
 *
 * @param[in] PpFuseArray         Fuse array pointer
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

VOID
GfxIntegratedInfoInitDispclkTable (
  IN       PP_FUSE_ARRAY                    *PpFuseArray,
  IN       ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  UINTN   Index;
  for (Index = 0; Index < 4; Index++) {
    if (PpFuseArray->DisplclkDid[Index] != 0) {
      IntegratedInfoTable->sDISPCLK_Voltage[Index].ulMaximumSupportedCLK = GfxLibCalculateClk (
                                                                             PpFuseArray->DisplclkDid[Index],
                                                                             IntegratedInfoTable->ulDentistVCOFreq
                                                                             );
      IntegratedInfoTable->sDISPCLK_Voltage[Index].ulVoltageIndex = (ULONG) Index;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 *Init Sclk <-> VID table
 *
 *
 * @param[in] PpFuseArray         Fuse array pointer
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

VOID
GfxIntegratedInfoInitSclkTable (
  IN       PP_FUSE_ARRAY                    *PpFuseArray,
  IN       ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  UINTN                       Index;
  UINTN                       TargetIndex;
  UINTN                       ValidSclkStateMask;
  UINT8                       TempDID;
  UINT8                       SclkVidArray[4];
  UINTN                       AvailSclkIndex;
  ATOM_AVAILABLE_SCLK_LIST    *AvailSclkList;
  BOOLEAN                     Sorting;
  AvailSclkList = &IntegratedInfoTable->sAvail_SCLK[0];
  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3x15C_ADDRESS),
    AccessWidth32,
    &SclkVidArray[0],
    GnbLibGetHeader (Gfx)
    );
  AvailSclkIndex = 0;
  for (Index = 0; Index < MAX_NUM_OF_FUSED_DPM_STATES; Index++) {
    if (PpFuseArray->SclkDpmDid[Index] != 0) {
      AvailSclkList[AvailSclkIndex].ulSupportedSCLK = GfxLibCalculateClk (PpFuseArray->SclkDpmDid[Index], IntegratedInfoTable->ulDentistVCOFreq);
      AvailSclkList[AvailSclkIndex].usVoltageIndex = PpFuseArray->SclkDpmVid[Index];
      AvailSclkList[AvailSclkIndex].usVoltageID = SclkVidArray [PpFuseArray->SclkDpmVid[Index]];
      AvailSclkIndex++;
    }
  }
  //Sort by VoltageIndex & ulSupportedSCLK
  do {
    Sorting = FALSE;
    for (Index = 0; Index < (AvailSclkIndex - 1); Index++) {
      ATOM_AVAILABLE_SCLK_LIST  Temp;
      BOOLEAN                   Exchange;
      Exchange = FALSE;
      if (AvailSclkList[Index].usVoltageIndex > AvailSclkList[Index + 1].usVoltageIndex) {
        Exchange = TRUE;
      }
      if ((AvailSclkList[Index].usVoltageIndex == AvailSclkList[Index + 1].usVoltageIndex) &&
          (AvailSclkList[Index].ulSupportedSCLK > AvailSclkList[Index + 1].ulSupportedSCLK)) {
        Exchange = TRUE;
      }
      if (Exchange) {
        Sorting = TRUE;
        LibAmdMemCopy (&Temp, &AvailSclkList[Index], sizeof (ATOM_AVAILABLE_SCLK_LIST), GnbLibGetHeader (Gfx));
        LibAmdMemCopy (&AvailSclkList[Index], &AvailSclkList[Index + 1], sizeof (ATOM_AVAILABLE_SCLK_LIST), GnbLibGetHeader (Gfx));
        LibAmdMemCopy (&AvailSclkList[Index + 1], &Temp, sizeof (ATOM_AVAILABLE_SCLK_LIST), GnbLibGetHeader (Gfx));
      }
    }
  } while (Sorting);

  if (PpFuseArray->GpuBoostCap == 1) {
    IntegratedInfoTable->SclkDpmThrottleMargin = PpFuseArray->SclkDpmThrottleMargin;
    IntegratedInfoTable->SclkDpmTdpLimitPG = PpFuseArray->SclkDpmTdpLimitPG;
    IntegratedInfoTable->EnableBoost = PpFuseArray->GpuBoostCap;
    IntegratedInfoTable->SclkDpmBoostMargin = PpFuseArray->SclkDpmBoostMargin;
    IntegratedInfoTable->SclkDpmTdpLimitBoost = (PpFuseArray->SclkDpmTdpLimit)[5];
    IntegratedInfoTable->ulBoostEngineCLock = GfxFmCalculateClock ((PpFuseArray->SclkDpmDid)[5], GnbLibGetHeader (Gfx));
    IntegratedInfoTable->ulBoostVid_2bit = (PpFuseArray->SclkDpmVid)[5];

    ValidSclkStateMask = 0;
    TargetIndex = 0;
    for (Index = 0; Index < 6; Index++) {
      ValidSclkStateMask |= (PpFuseArray->SclkDpmValid)[Index];
    }
    TempDID = 0x7F;
    for (Index = 0; Index < 6; Index++) {
      if ((ValidSclkStateMask & ((UINTN)1 << Index)) != 0) {
        if ((PpFuseArray->SclkDpmDid)[Index] <= TempDID) {
          TempDID = (PpFuseArray->SclkDpmDid)[Index];
          TargetIndex = Index;
        }
      }
    }
    IntegratedInfoTable->GnbTdpLimit = (PpFuseArray->SclkDpmTdpLimit)[TargetIndex];
  }

}

/*----------------------------------------------------------------------------------------*/
/**
 *Init HTC Data
 *
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

VOID
GfxFillHtcData (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  D18F3x64_STRUCT  D18F3x64;

  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3x64_ADDRESS),
    AccessWidth32,
    &D18F3x64.Value,
    GnbLibGetHeader (Gfx)
    );
  IntegratedInfoTable->ucHtcTmpLmt = (UCHAR) (D18F3x64.Field.HtcTmpLmt / 2 + 52);
  IntegratedInfoTable->ucHtcHystLmt = (UCHAR) (D18F3x64.Field.HtcHystLmt / 2);
}

/*----------------------------------------------------------------------------------------*/
/**
 *Init NbPstateVid
 *
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

VOID
GfxFillNbPStateVid (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  D18F3xDC_STRUCT  D18F3xDC;
  D18F6x90_STRUCT  D18F6x90;

  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3xDC_ADDRESS),
    AccessWidth32,
    &D18F3xDC.Value,
    GnbLibGetHeader (Gfx)
    );
  IntegratedInfoTable->usNBP0Voltage = (USHORT) D18F3xDC.Field.NbPs0Vid;

  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 6, D18F6x90_ADDRESS),
    AccessWidth32,
    &D18F6x90.Value,
    GnbLibGetHeader (Gfx)
    );
  IntegratedInfoTable->usNBP1Voltage = (USHORT) D18F6x90.Field.NbPs1Vid;
  IntegratedInfoTable->ulMinimumNClk = GfxLibCalculateClk (
                                         (UINT8) (((D18F6x90.Field.NbPs1NclkDiv != 0) && (D18F6x90.Field.NbPs1NclkDiv < D18F3xDC.Field.NbPs0NclkDiv)) ? D18F6x90.Field.NbPs1NclkDiv : D18F3xDC.Field.NbPs0NclkDiv),
                                         IntegratedInfoTable->ulDentistVCOFreq
                                         );
}

/*----------------------------------------------------------------------------------------*/
/**
 *Init M3 Arbitration Control values.
 *
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

VOID
GfxFillM3ArbritrationControl (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  LibAmdMemCopy (IntegratedInfoTable->ulCSR_M3_ARB_CNTL_DEFAULT, ulCSR_M3_ARB_CNTL_DEFAULT, sizeof (ulCSR_M3_ARB_CNTL_DEFAULT), GnbLibGetHeader (Gfx));
  LibAmdMemCopy (IntegratedInfoTable->ulCSR_M3_ARB_CNTL_UVD, ulCSR_M3_ARB_CNTL_UVD, sizeof (ulCSR_M3_ARB_CNTL_UVD), GnbLibGetHeader (Gfx));
  LibAmdMemCopy (IntegratedInfoTable->ulCSR_M3_ARB_CNTL_FS3D, ulCSR_M3_ARB_CNTL_FS3D, sizeof (ulCSR_M3_ARB_CNTL_FS3D), GnbLibGetHeader (Gfx));
}

/*----------------------------------------------------------------------------------------*/
/**
 *Init M3 Arbitration Control values.
 *
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

VOID
GfxFillSbMmioBaseAddress (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  UINT8   Index;
  UINT32  SbMmioBaseAddress;
  SbMmioBaseAddress = 0;
  //Read Dword from PMIO 24h.  SB PMIO region supports only byte read.
  for (Index = 0x24; Index < 0x28; Index++) {
    GnbLibIoWrite (SB_IOMAP_REGCD6, AccessWidth8, &Index, GnbLibGetHeader (Gfx));
    GnbLibIoRead (SB_IOMAP_REGCD7, AccessWidth8, &(((UINT8*) &SbMmioBaseAddress)[Index - 0x24]), GnbLibGetHeader (Gfx));
  }
  // If MMIO is enabled and set for memory(not IO) then set MMIO_Base_Addr parameter.
  if ((SbMmioBaseAddress & (SB_MMIO_IO_MAPPED_ENABLE | SB_MMIO_DECODE_ENABLE)) == SB_MMIO_DECODE_ENABLE) {
    IntegratedInfoTable->ulSB_MMIO_Base_Addr = (ULONG) (SbMmioBaseAddress & (~SB_MMIO_DECODE_ENABLE)) ;
  } else {
    IntegratedInfoTable->ulSB_MMIO_Base_Addr = 0;
  }
  IDS_HDT_CONSOLE (GFX_MISC, "  ulSB_MMIO_Base_Addr = 0x%x\n", IntegratedInfoTable->ulSB_MMIO_Base_Addr);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Fill in NCLK info
 *
 * set ulMinimumNClk and ulIdleNClk
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

VOID
GfxFillNclkInfo (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V6   *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{

  D18F3xA0_STRUCT  D18F3xA0;
  D18F6x9C_STRUCT  D18F6x9C;
  D18F3xDC_STRUCT  D18F3xDC;
  D18F6x90_STRUCT  D18F6x90;

  //
  // ulIdleNClk = GfxLibGetMainPllFreq (...) / F6x9C[NclkRedDiv] divisor (main PLL frequency / NCLK divisor)
  //
  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 6, D18F6x9C_ADDRESS),
    AccessWidth32,
    &D18F6x9C.Value,
    GnbLibGetHeader (Gfx)
    );

  IntegratedInfoTable->ulIdleNClk = GfxLibCalculateIdleNclk (
                                      (UINT8) D18F6x9C.Field.NclkRedDiv,
                                      IntegratedInfoTable->ulDentistVCOFreq
                                      );

  //
  // Set ulMinimumNClk depends on CPU fused and NB Pstate.
  //
  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3xA0_ADDRESS),
    AccessWidth32,
    &D18F3xA0.Value,
    GnbLibGetHeader (Gfx)
    );

  if (D18F3xA0.Field.CofVidProg) {

    GnbLibPciRead (
      MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3xDC_ADDRESS),
      AccessWidth32,
      &D18F3xDC.Value,
      GnbLibGetHeader (Gfx)
      );

    GnbLibPciRead (
      MAKE_SBDFO ( 0, 0, 0x18, 6, D18F6x90_ADDRESS),
      AccessWidth32,
      &D18F6x90.Value,
      GnbLibGetHeader (Gfx)
      );

    //
    // Set ulMinimumNClk if (F6x90[NbPsCap]==1 && F6x90[NbPsCtrlDis]==0) then  (
    // GfxLibGetMainPllFreq (...) / F6x90[NbPs1NclkDiv] divisor
    // ) else ( GfxLibGetMainPllFreq (...) / F3xDC[NbPs0NclkDiv] divisor
    // )
    //
    IntegratedInfoTable->ulMinimumNClk = GfxLibCalculateNclk (
                                           (UINT8) (((D18F6x90.Field.NbPsCap == 1) && (D18F6x90.Field.NbPsCtrlDis == 0)) ? D18F6x90.Field.NbPs1NclkDiv : D18F3xDC.Field.NbPs0NclkDiv),
                                           IntegratedInfoTable->ulDentistVCOFreq
                                           );
  } else {
    IntegratedInfoTable->ulMinimumNClk = 200 * 100;
  }

}
