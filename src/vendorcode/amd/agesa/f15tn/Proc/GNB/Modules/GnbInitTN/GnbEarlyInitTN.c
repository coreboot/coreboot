/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe early post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 64732 $   @e \$Date: 2012-01-30 02:16:26 -0600 (Mon, 30 Jan 2012) $
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
#include  "Gnb.h"
#include  "OptionGnb.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbTable.h"
#include  "GnbNbInitLibV4.h"
#include  "GnbSmuFirmwareTN.h"
#include  "GnbRegisterAccTN.h"
#include  "GnbRegistersTN.h"
#include  "GfxLibTN.h"
#include  "GnbCacWeightsTN.h"
#include  "cpuFamilyTranslation.h"
#include  "GnbHandleLib.h"
#include  "GnbBapmCoeffCalcTN.h"
#include  "GnbInitTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_GNBEARLYINITTN_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_TABLE ROMDATA      GnbEarlyInitTableTN [];
extern GNB_TABLE ROMDATA      GnbEarlierInitTableBeforeSmuTN [];
extern GNB_TABLE ROMDATA      GnbEarlierInitTableAfterSmuTN [];
extern GNB_BUILD_OPTIONS      GnbBuildOptions;
extern BUILD_OPT_CFG          UserOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GnbEarlyInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

AGESA_STATUS
GnbEarlierInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
);
/*----------------------------------------------------------------------------------------*/
/**
 * Gnb TN Decrease all of the SMU VIDs by 4 (+25mV)

 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 */
STATIC VOID
GnbAdjustSmuVidBeforeSmuTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  D0F0xBC_xE0001008_STRUCT  D0F0xBC_xE0001008;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbAdjustSmuVidBeforeSmuTN Enter\n");

  GnbRegisterReadTN (D0F0xBC_xE0001008_TYPE, D0F0xBC_xE0001008_ADDRESS, &D0F0xBC_xE0001008, 0, StdHeader);
  D0F0xBC_xE0001008.Field.SClkVid3 -= 4;
  D0F0xBC_xE0001008.Field.SClkVid2 -= 4;
  D0F0xBC_xE0001008.Field.SClkVid1 -= 4;
  D0F0xBC_xE0001008.Field.SClkVid0 -= 4;
  GnbRegisterWriteTN (D0F0xBC_xE0001008_TYPE, D0F0xBC_xE0001008_ADDRESS, &D0F0xBC_xE0001008, 0, StdHeader);

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbAdjustSmuVidBeforeSmuTN Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Gnb TN Decrease all of the SMU VIDs by 4 (+25mV)

 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 */
STATIC VOID
GnbAdjustSmuVidAfterSmuTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  D0F0xBC_x1F88C_STRUCT                   D0F0xBC_x1F88C;
  D0F0xBC_x1F8DC_STRUCT                   D0F0xBC_x1F8DC;
  D0F0xBC_x1F8E0_STRUCT                   D0F0xBC_x1F8E0;
  D0F0xBC_x1F8E4_STRUCT                   D0F0xBC_x1F8E4;
  D0F0xBC_x1F8E8_STRUCT                   D0F0xBC_x1F8E8;
  D0F0xBC_x1F400_STRUCT                   D0F0xBC_x1F400;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbAdjustSmuVidAfterSmuTN Enter\n");

  //Adjust SMU VIDs
  GnbRegisterReadTN (D0F0xBC_x1F88C_TYPE, D0F0xBC_x1F88C_ADDRESS, &D0F0xBC_x1F88C, 0, StdHeader);
  GnbRegisterReadTN (D0F0xBC_x1F8DC_TYPE, D0F0xBC_x1F8DC_ADDRESS, &D0F0xBC_x1F8DC, 0, StdHeader);
  GnbRegisterReadTN (D0F0xBC_x1F8E0_TYPE, D0F0xBC_x1F8E0_ADDRESS, &D0F0xBC_x1F8E0, 0, StdHeader);
  GnbRegisterReadTN (D0F0xBC_x1F8E4_TYPE, D0F0xBC_x1F8E4_ADDRESS, &D0F0xBC_x1F8E4, 0, StdHeader);
  GnbRegisterReadTN (D0F0xBC_x1F8E8_TYPE, D0F0xBC_x1F8E8_ADDRESS, &D0F0xBC_x1F8E8, 0, StdHeader);

  D0F0xBC_x1F88C.Field.NbVid_3 -= 4;
  D0F0xBC_x1F88C.Field.NbVid_2 -= 4;
  D0F0xBC_x1F88C.Field.NbVid_1 -= 4;
  D0F0xBC_x1F88C.Field.NbVid_0 -= 4;

  D0F0xBC_x1F8DC.Field.SClkVid3 -= 4;
  D0F0xBC_x1F8DC.Field.SClkVid2 -= 4;
  D0F0xBC_x1F8DC.Field.SClkVid1 -= 4;
  D0F0xBC_x1F8DC.Field.SClkVid0 -= 4;
  D0F0xBC_x1F8E0.Field.BapmSclkVid_2 -= 4;
  D0F0xBC_x1F8E0.Field.BapmSclkVid_1 -= 4;
  D0F0xBC_x1F8E0.Field.BapmSclkVid_0 -= 4;
  D0F0xBC_x1F8E4.Field.BapmNbVid_1 -= 4;
  D0F0xBC_x1F8E4.Field.BapmNbVid_0 -= 4;
  D0F0xBC_x1F8E4.Field.BapmSclkVid_3 -= 4;
  D0F0xBC_x1F8E8.Field.BapmNbVid_3 -= 4;
  D0F0xBC_x1F8E8.Field.BapmNbVid_2 -= 4;

  GnbRegisterWriteTN (D0F0xBC_x1F88C_TYPE, D0F0xBC_x1F88C_ADDRESS, &D0F0xBC_x1F88C, 0, StdHeader);
  GnbRegisterWriteTN (D0F0xBC_x1F8DC_TYPE, D0F0xBC_x1F8DC_ADDRESS, &D0F0xBC_x1F8DC, 0, StdHeader);
  GnbRegisterWriteTN (D0F0xBC_x1F8E0_TYPE, D0F0xBC_x1F8E0_ADDRESS, &D0F0xBC_x1F8E0, 0, StdHeader);
  GnbRegisterWriteTN (D0F0xBC_x1F8E4_TYPE, D0F0xBC_x1F8E4_ADDRESS, &D0F0xBC_x1F8E4, 0, StdHeader);
  GnbRegisterWriteTN (D0F0xBC_x1F8E8_TYPE, D0F0xBC_x1F8E8_ADDRESS, &D0F0xBC_x1F8E8, 0, StdHeader);

  //D0F0xBC_x1F400[SviLoadLineOffsetVddNB]=01b (-25mV)
  GnbRegisterReadTN (D0F0xBC_x1F400_TYPE, D0F0xBC_x1F400_ADDRESS, &D0F0xBC_x1F400, 0, StdHeader);
  D0F0xBC_x1F400.Field.SviLoadLineOffsetVddNB = 1;
  GnbRegisterWriteTN (D0F0xBC_x1F400_TYPE, D0F0xBC_x1F400_ADDRESS, &D0F0xBC_x1F400, 0, StdHeader);


  IDS_HDT_CONSOLE (GNB_TRACE, "GnbAdjustSmuVidAfterSmuTN Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Gnb SMU LHTC support
 *
 * Part of BAPM enablement.
 * When BAPM is disabled in battery mode firmware will enable LHTC.
 *
 * @param[in]  StdHeader  Standard configuration header
 */
STATIC VOID
GnbBapmLhtcInitTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  D0F0xBC_x1F638_STRUCT            D0F0xBC_x1F638;
  D0F0xBC_x1F428_STRUCT            D0F0xBC_x1F428;
  D0F0xBC_x1F86C_STRUCT            D0F0xBC_x1F86C;
  D0F0xBC_x1F628_STRUCT            D0F0xBC_x1F628;
  D0F0xBC_xE0104188_STRUCT         D0F0xBC_xE0104188;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbBapmLhtcInitTN Enter\n");

  GnbRegisterReadTN (D0F0xBC_x1F638_TYPE, D0F0xBC_x1F638_ADDRESS, &D0F0xBC_x1F638, 0, StdHeader);
  GnbRegisterReadTN (D0F0xBC_x1F428_TYPE, D0F0xBC_x1F428_ADDRESS, &D0F0xBC_x1F428, 0, StdHeader);
  GnbRegisterReadTN (D0F0xBC_xE0104188_TYPE, D0F0xBC_xE0104188_ADDRESS, &D0F0xBC_xE0104188, 0, StdHeader);

  //1. Set HTC period to 10 in PM_TIMERS_2 register
  //Still need to keep PM_CONFIG.Enable_HTC_Limit to 0
  D0F0xBC_x1F428.Field.field_4  = 0;
  GnbRegisterWriteTN (D0F0xBC_x1F428_TYPE, D0F0xBC_x1F428_ADDRESS, &D0F0xBC_x1F428, 0, StdHeader);
  D0F0xBC_x1F638.Field.HtcPeriod = 10;
  GnbRegisterWriteTN (D0F0xBC_x1F638_TYPE, D0F0xBC_x1F638_ADDRESS, &D0F0xBC_x1F638, 0, StdHeader);

  //2. Read BapmLhtcCap fuse
  GnbRegisterReadTN (D0F0xBC_x1F86C_TYPE, D0F0xBC_x1F86C_ADDRESS, &D0F0xBC_x1F86C, 0, StdHeader);
  GnbRegisterReadTN (D0F0xBC_x1F628_TYPE, D0F0xBC_x1F628_ADDRESS, &D0F0xBC_x1F628, 0, StdHeader);
  if (D0F0xBC_x1F86C.Field.BapmLhtcCap == 0) {
    D0F0xBC_x1F628.Field.HtcActivePstateLimit = 0;
  } else {
    D0F0xBC_x1F628.Field.HtcActivePstateLimit = D0F0xBC_xE0104188.Field.LhtcPstateLimit;
  }
  GnbRegisterWriteTN (D0F0xBC_x1F628_TYPE, D0F0xBC_x1F628_ADDRESS, &D0F0xBC_x1F628, 0, StdHeader);

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbBapmLhtcInitTN Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Measured temperature with BAPM
 *
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 */
STATIC VOID
GnbBapmMeasuredTempTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  D0F0xBC_x1F428_STRUCT            D0F0xBC_x1F428;
  D0F0xBC_xE0104188_STRUCT         D0F0xBC_xE0104188;
  D0F0xBC_x1F844_STRUCT            D0F0xBC_x1F844;
  D0F0xBC_x1F848_STRUCT            D0F0xBC_x1F848;
  D0F0xBC_x1F84C_STRUCT            D0F0xBC_x1F84C;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbBapmMeasuredTempTN Enter\n");

  GnbRegisterReadTN (D0F0xBC_xE0104188_TYPE, D0F0xBC_xE0104188_ADDRESS, &D0F0xBC_xE0104188, 0, StdHeader);

  //Measured temperature with BAPM
  GnbRegisterReadTN (D0F0xBC_x1F428_TYPE, D0F0xBC_x1F428_ADDRESS, &D0F0xBC_x1F428, 0, StdHeader);
  D0F0xBC_x1F428.Field.line180  = 0;
  if (D0F0xBC_xE0104188.Field.BapmMeasuredTemp == 1) {
    D0F0xBC_x1F844.Value = 0x38B;
    GnbRegisterWriteTN (D0F0xBC_x1F844_TYPE, D0F0xBC_x1F844_ADDRESS, &D0F0xBC_x1F844, 0, StdHeader);
    D0F0xBC_x1F848.Value = 0x38D;
    GnbRegisterWriteTN (D0F0xBC_x1F848_TYPE, D0F0xBC_x1F848_ADDRESS, &D0F0xBC_x1F848, 0, StdHeader);
    D0F0xBC_x1F84C.Value = 0x389;
    GnbRegisterWriteTN (D0F0xBC_x1F84C_TYPE, D0F0xBC_x1F84C_ADDRESS, &D0F0xBC_x1F84C, 0, StdHeader);

    D0F0xBC_x1F428.Field.line180  = 1;
  }
  GnbRegisterWriteTN (D0F0xBC_x1F428_TYPE, D0F0xBC_x1F428_ADDRESS, &D0F0xBC_x1F428, 0, StdHeader);

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbBapmMeasuredTempTN Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Gnb SMU LHTC Enable
 *
 * Part of BAPM enablement.
 * When BAPM is disabled in battery mode firmware will enable LHTC.
 *
 * @param[in]  StdHeader  Standard configuration header
 */
STATIC VOID
GnbLhtcEnableTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  D0F0xBC_x1F428_STRUCT            D0F0xBC_x1F428;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbLhtcEnableTN Enter\n");

  GnbRegisterReadTN (D0F0xBC_x1F428_TYPE, D0F0xBC_x1F428_ADDRESS, &D0F0xBC_x1F428, 0, StdHeader);
  D0F0xBC_x1F428.Field.field_4  = 1;
  GnbRegisterWriteTN (D0F0xBC_x1F428_TYPE, D0F0xBC_x1F428_ADDRESS, &D0F0xBC_x1F428, 0, StdHeader);

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbLhtcEnableTN Exit\n");
}
/*----------------------------------------------------------------------------------------*/
/**
 * Gnb TN Update BAPMTI_TjOffset
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 */
STATIC VOID
GnbTjOffsetUpdateTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  D0F0xBC_x1F870_STRUCT             D0F0xBC_x1F870;

  CPU_LOGICAL_ID                    LogicalId;
  GNB_HANDLE                        *GnbHandle;
  D0F0xBC_xE0104040_STRUCT          D0F0xBC_xE0104040;
  D0F0xBC_x1F85C_STRUCT             D0F0xBC_x1F85C;
  ex1075_STRUCT           ex1075 ;
  UINT32                            TimerPeriod;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbTjOffsetUpdateTN Enter\n");


  TimerPeriod = D0F0xBC_x1F468_TimerPeriod_Value;
  GnbRegisterReadTN (D0F0xBC_x1F85C_TYPE, D0F0xBC_x1F85C_ADDRESS, &D0F0xBC_x1F85C, 0, StdHeader);
  GnbRegisterReadTN (TYPE_D0F0xBC , 0xe010413c , &ex1075, 0, StdHeader);
  // Determine desired AgingRate:
  // PM_FUSES4.TdpAgeRate * Fuse[BAPMTI_Ts] (encoded in us)
  // Re-encode TdpAgeRate with 1ms BAPM interval
  D0F0xBC_x1F85C.Field.TdpAgeRate = (D0F0xBC_x1F85C.Field.TdpAgeRate * ex1075.Field.ex1075_0 ) / (TimerPeriod / 100);
  GnbRegisterWriteTN (D0F0xBC_x1F85C_TYPE, D0F0xBC_x1F85C_ADDRESS, &D0F0xBC_x1F85C, 0, StdHeader);

  GnbHandle = GnbGetHandle (StdHeader);
  ASSERT (GnbHandle != NULL);
  GetLogicalIdOfSocket (GnbGetSocketId (GnbHandle), &LogicalId, StdHeader);
  if ((LogicalId.Revision & 0x0000000000000100ull ) != 0x0000000000000100ull ) {
    IDS_HDT_CONSOLE (GNB_TRACE, "CPU Rev = %x, Skip GnbTjOffsetUpdateTN\n", LogicalId.Revision);
    return;
  }
  GnbRegisterReadTN (D0F0xBC_xE0104040_TYPE, D0F0xBC_xE0104040_ADDRESS, &D0F0xBC_xE0104040, 0, StdHeader);
  GnbRegisterReadTN (D0F0xBC_x1F870_TYPE, D0F0xBC_x1F870_ADDRESS, &D0F0xBC_x1F870, 0, StdHeader);
  //9900h=FS1r2/FP2 Devastator
  //9903h=FS1r2/FP2 Devastator Lite
  //9990h=FS1r2/FP2 Scrapper
  //9901h=FM2 Devastator
  //9904h=FM2 Devastator Lite
  //9991h=FM2 Scrapper
  if ((D0F0xBC_xE0104040.Field.DeviceID == 0x9900) || (D0F0xBC_xE0104040.Field.DeviceID == 0x9903)) {
    D0F0xBC_x1F870.Field.BAPMTI_TjOffset_0 = 0x26;
    D0F0xBC_x1F870.Field.BAPMTI_TjOffset_1 = 0x26;
    D0F0xBC_x1F870.Field.BAPMTI_TjOffset_2 = 0x26;
  } else if (D0F0xBC_xE0104040.Field.DeviceID == 0x9990) {
    D0F0xBC_x1F870.Field.BAPMTI_TjOffset_0 = 0x2E;
    D0F0xBC_x1F870.Field.BAPMTI_TjOffset_1 = 0x2E;
    D0F0xBC_x1F870.Field.BAPMTI_TjOffset_2 = 0x2E;
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "GnbTjOffsetUpdateTN Skip DID- %x\n", D0F0xBC_xE0104040.Field.DeviceID);
  }
  GnbRegisterWriteTN (D0F0xBC_x1F870_TYPE, D0F0xBC_x1F870_ADDRESS, &D0F0xBC_x1F870, 0, StdHeader);

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbTjOffsetUpdateTN Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * GPU CAC enablement and weights programming
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 */

STATIC VOID
GnbCacEnablement (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  D0F0xBC_x1F464_STRUCT   D0F0xBC_x1F464;
  ex1071_STRUCT    ex1071 ;
  ex1072_STRUCT    ex1072 ;
  PCI_ADDR                PciAddress;
  UINT8                   Index;
  ex1073_STRUCT    ex1073 ;
  D18F5x160_STRUCT        D18F5x160;
  DOUBLE                  UnbCac;
  GMMx898_STRUCT          GMMx898;

  GnbRegisterReadTN (TYPE_D0F0xBC , 0x1f920 , &ex1072, 0, StdHeader);
  ex1072.Field.ex1072_2  = 0x29;
  GnbRegisterWriteTN (TYPE_D0F0xBC , 0x1f920 , &ex1072, 0, StdHeader);

  //UNB_CAC_VALUE.UNB_CAC = 2.3734E-04 * FNBPS0 (in MHz) * 2^GPU_CAC_AVRG_CNTL.WEIGHT_PREC
  GnbRegisterReadTN (D18F5x160_TYPE, D18F5x160_ADDRESS, &D18F5x160.Value, 0, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "NBP0 10khz %x (%d)\n", GfxLibGetNclkTN ((UINT8) D18F5x160.Field.NbFid, (UINT8) D18F5x160.Field.NbDid), GfxLibGetNclkTN ((UINT8) D18F5x160.Field.NbFid, (UINT8) D18F5x160.Field.NbDid));
  UnbCac = 0.00000204831536 * (1 << ex1072.Field.ex1072_0 ) * GfxLibGetNclkTN ((UINT8) D18F5x160.Field.NbFid, (UINT8) D18F5x160.Field.NbDid);
  ex1073.Field.ex1073_0  = (UINT32) GnbFpLibDoubleToInt32 (UnbCac);
  IDS_HDT_CONSOLE (GNB_TRACE, "UnbCac %x (%d)\n", ex1073.Field.ex1073_0 , ex1073.Field.ex1073_0 );
  GnbRegisterWriteTN (TYPE_D0F0xBC , 0x1f91c , &ex1073.Value, 0, StdHeader);

  GnbRegisterReadTN (TYPE_D0F0xBC , 0x1f160 , &ex1071, 0, StdHeader);
  ex1071.Field.ex1071_0  = 0x1;
  ex1071.Field.ex1071_3  = 0x4;
  ex1071.Field.ex1071_4  = 0x25;
  GnbRegisterWriteTN (TYPE_D0F0xBC , 0x1f160 , &ex1071, 0, StdHeader);

  GnbRegisterReadTN (GMMx898_TYPE, GMMx898_ADDRESS, &GMMx898, 0, StdHeader);
  GMMx898.Field.Threshold = 0x31;
  GnbRegisterWriteTN (GMMx898_TYPE, GMMx898_ADDRESS, &GMMx898, 0, StdHeader);

  // Set CAC/TDP interval
  GnbRegisterReadTN (D0F0xBC_x1F464_TYPE, D0F0xBC_x1F464_ADDRESS, &D0F0xBC_x1F464, 0, StdHeader);
  D0F0xBC_x1F464.Field.TdpCntl = 1;
  GnbRegisterWriteTN (D0F0xBC_x1F464_TYPE, D0F0xBC_x1F464_ADDRESS, &D0F0xBC_x1F464, 0, StdHeader);

  // Program GPU CAC weights

  for (Index = 0; Index < ARRAY_SIZE(CacWeightsTN); Index++) {
    GnbRegisterWriteTN (TYPE_D0F0xBC , (0x1f9a0  + (Index * 4)), &CacWeightsTN[Index], 0, StdHeader);
  }

  // Call BIOS service SMC_MSG_CONFIG_TDP_CNTL
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
  GnbSmuServiceRequestV4 (
    PciAddress,
    SMC_MSG_CONFIG_TDP_CNTL,
    0,
    StdHeader
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Decode power of CPU out of Watt
 *
 *
 *
 * @param[in] Encode      PwrCpu encode
 * @param[in] StdHeader   Standard Configuration Header
 * @retval                mWatt
 */
STATIC INT32
CpuPowerDecode (
  IN      UINT8                           Encode,
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  INT32                   Power;
  ex1002_STRUCT    ex1002 ;

  GnbRegisterReadTN (TYPE_D0F0xBC , 0x1f850 , &ex1002, 0, StdHeader);

  //TdpWatt = TdpWattEncode / 1024
  //PwrCpu / TdpWatt = Encode
  //PwrCpu = Encode * TdpWattEncode / 1024

  Power = (INT32) ((Encode * ex1002.Field.ex1002_0  *1000) / 1024);

  return Power;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Encode the offset of power of CPU
 *
 *
 *
 * @param[in] NewPower    New power of mWatt
 * @param[in] OrgPower    Original power of mWatt
 * @param[in] StdHeader   Standard Configuration Header
 * @retval                Encode
 */
STATIC UINT8
CpuPowerOffsetEncode (
  IN      INT32                           NewPower,
  IN      INT32                           OrgPower,
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  INT8                    PowerOffsetEncode;
  INT32                   PowerOffset;
  ex1002_STRUCT    ex1002 ;
  BOOLEAN                 Postive;

  GnbRegisterReadTN (TYPE_D0F0xBC , 0x1f850 , &ex1002, 0, StdHeader);
  if (NewPower > OrgPower) {
    PowerOffset = NewPower - OrgPower;
    Postive = TRUE;
  } else {
    PowerOffset = OrgPower - NewPower;
    Postive = FALSE;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "Cpu New Pwr %x (%d)\n", NewPower, NewPower);
  IDS_HDT_CONSOLE (GNB_TRACE, "Cpu org Pwr %x (%d)\n", OrgPower, OrgPower);
  IDS_HDT_CONSOLE (GNB_TRACE, "Tdp2Watt %x, (%d)\n", ex1002.Field.ex1002_0 , ex1002.Field.ex1002_0 );
  //Ceil of (mWatt *1024 / TdpWattEncode) / 1000 = Encode in watt
  PowerOffset = (((PowerOffset * 1024) / ex1002.Field.ex1002_0 ) + 500) / 1000;

  if (Postive) {
    PowerOffsetEncode = (INT8) PowerOffset;
  } else {
    PowerOffsetEncode = 0 - (INT8) PowerOffset;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PowerOffsetEncode %x\n", PowerOffsetEncode);
  return (UINT8) PowerOffsetEncode;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Decode power of GPU out of Watt
 *
 *
 *
 * @param[in] Encode      PwrGpu encode
 * @param[in] StdHeader   Standard Configuration Header
 * @retval                mWatt
 */
STATIC INT16
GpuPowerDecode (
  IN      UINT16                          Encode,
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  INT16                   Power;

  Power = (INT16) Encode;


  return Power;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Decode Max Tj
 *
 *
 *
 * @param[in] Encode      Tj encode
 * @retval                100 x Tj
 */
STATIC INT16
TjMaxDecode (
  IN      UINT8               Encode
  )
{
  INT16    TjMax;

  TjMax = (INT16) Encode;

  return (TjMax * 100);
}

/*----------------------------------------------------------------------------------------*/
/**
 * for BAPMTI_TjOffset decoding
 *
 *
 *
 * @param[in] Encode      Tj encode
 * @retval                100 x Tjoffset
 */
STATIC INT16
TjOffsetDecode (
  IN      UINT8               Encode
  )
{
  UINT16      Number;
  UINT8       Floating;
  BOOLEAN     Postive;
  UINT8       TjOffsetEncode;

  TjOffsetEncode = Encode;
  Postive = TRUE;

  if (Encode == 0) {
    return 0;
  }

  if ((TjOffsetEncode & 0x80) != 0) {
    Postive = FALSE;
    TjOffsetEncode = (UINT8) (~(Encode - 1));
  }

  Number = ((TjOffsetEncode >> 2) & 0x1F) * 100;

  Floating = (TjOffsetEncode & 0x3);
  if (Floating == 1) {
    Number += 25;
  } else if (Floating == 2) {
    Number += 50;
  } else if (Floating == 3) {
    Number += 75;
  } else {
  }

  if (Postive) {
    return  (INT16) Number;
  } else {
    return (INT16) (0 - Number);
  }

}

/*----------------------------------------------------------------------------------------*/
/**
 * Trinity SMU supports a software-writeable TjOffset (called swTjOffset) that can be programmed to
 * account for underspec thermal solutions.
 * There is a mechanism for customers to adjust TjOffset (via BAPM_PARAMETERS3.TjOffset)
 * for under-performing thermal solutions.
 * BIOS will adjust NomPow/MidPow/MaxPow based on this software-programmable TjOffset (called swTjOffset).
 * SMU firmware will add this value to Fuse[TjOffset] for all TE's during BAPM calculations.
 *
 * Tj stands for junction temperature of the processor. However, here is a general description of
 * our software-programmable TjOffset for BAPM (Birdirectional Application Power Management):
 * "swTjOffset is an adjustable offset for BAPM thermal calculations to account for changes in
 * junction temperature, TjOffset. For further details, see Thermal Guide."
 *
 * @param[in]  StdHeader  Standard configuration header
 */
STATIC VOID
GnbSoftwareTjOffsetTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  D0F0xBC_x1F860_STRUCT   D0F0xBC_x1F860;
  D0F0xBC_x1F864_STRUCT   D0F0xBC_x1F864;
  ex999_STRUCT    ex999 ;
  D0F0xBC_x1F870_STRUCT   D0F0xBC_x1F870;
  ex1000_STRUCT    ex1000 ;
  ex1001_STRUCT    ex1001 ;

  ex996_STRUCT   ex996;
  ex997_STRUCT    ex997 ;
  D0F0xBC_x1F6B4_STRUCT   D0F0xBC_x1F6B4;
  ex998_STRUCT    ex998 ;
  INT8                    SwTjOffset;
  INT16                   Delta_T_org;
  INT16                   Delta_T_new;
  INT32                   Cpu_New_Pwr;
  INT32                   Gpu_New_Pwr;

  SwTjOffset = (INT8) UserOptions.CfgGnbSwTjOffset;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbSoftwareTjOffsetTN Enter\n");

  IDS_OPTION_HOOK (IDS_GNB_PMM_SWTJOFFSET, &SwTjOffset, StdHeader);
  if (SwTjOffset == 0) {
    return;
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "User Input Tj Offset %x\n", SwTjOffset);
  GnbRegisterReadTN (D0F0xBC_x1F860_TYPE, D0F0xBC_x1F860_ADDRESS, &D0F0xBC_x1F860, 0, StdHeader);
  GnbRegisterReadTN (D0F0xBC_x1F864_TYPE, D0F0xBC_x1F864_ADDRESS, &D0F0xBC_x1F864, 0, StdHeader);
  GnbRegisterReadTN (TYPE_D0F0xBC , 0x1f868 , &ex999, 0, StdHeader);
  GnbRegisterReadTN (D0F0xBC_x1F870_TYPE, D0F0xBC_x1F870_ADDRESS, &D0F0xBC_x1F870, 0, StdHeader);
  GnbRegisterReadTN (TYPE_D0F0xBC , 0x1f898 , &ex1000, 0, StdHeader);
  GnbRegisterReadTN (TYPE_D0F0xBC , 0x1f8c0 , &ex1001, 0, StdHeader);

  //Tjoffset_new = Tjoffset_org + SwTjOffset
  //Delta_T_org = T_die - Tjoffset_org - 45

  //Delta_T_new = T_die - Tjoffset_new - 45
  //            = T_die - (Tjoffset_org + SwTjOffset) - 45
  //            = T_die - Tjoffset_org - SwTjOffset - 45

  //Pwr_new = Pwr_org * (Delta_T_new/Delta_T_org)
  //        = Pwr_org * (T_org - TjOffset) / T_org

  //Cpu0
  Delta_T_org = TjMaxDecode ((UINT8) D0F0xBC_x1F860.Field.BAPMTI_TjMax_0) - TjOffsetDecode ((UINT8) D0F0xBC_x1F870.Field.BAPMTI_TjOffset_0) - 4500;
  Delta_T_new = TjMaxDecode ((UINT8) D0F0xBC_x1F860.Field.BAPMTI_TjMax_0) - TjOffsetDecode ((UINT8) D0F0xBC_x1F870.Field.BAPMTI_TjOffset_0) - (SwTjOffset * 100) - 4500;
  IDS_HDT_CONSOLE (GNB_TRACE, "Cpu0 Delta T org %x (%d)\n", Delta_T_org, Delta_T_org);
  IDS_HDT_CONSOLE (GNB_TRACE, "Cpu0 Delta T New %x (%d)\n", Delta_T_new, Delta_T_new);
  Cpu_New_Pwr = (CpuPowerDecode ((UINT8) ex999.Field.ex999_3 , StdHeader) * Delta_T_new) / Delta_T_org;
  ex996.Field.ex996_2 = CpuPowerOffsetEncode (Cpu_New_Pwr, CpuPowerDecode ((UINT8) ex999.Field.ex999_3 , StdHeader), StdHeader);
  Cpu_New_Pwr = (CpuPowerDecode ((UINT8) ex999.Field.ex999_2 , StdHeader) * Delta_T_new) / Delta_T_org;
  ex996.Field.ex996_3 = CpuPowerOffsetEncode (Cpu_New_Pwr, CpuPowerDecode ((UINT8) ex999.Field.ex999_2 , StdHeader), StdHeader);
  Cpu_New_Pwr = (CpuPowerDecode ((UINT8) ex1001.Field.ex1001_2 , StdHeader) * Delta_T_new) / Delta_T_org;
  ex998.Field.ex998_2  = CpuPowerOffsetEncode (Cpu_New_Pwr, CpuPowerDecode ((UINT8) ex1001.Field.ex1001_2 , StdHeader), StdHeader);

  //Cpu1
  Delta_T_org = TjMaxDecode ((UINT8) D0F0xBC_x1F860.Field.BAPMTI_TjMax_1) - TjOffsetDecode ((UINT8) D0F0xBC_x1F870.Field.BAPMTI_TjOffset_1) - 4500;
  Delta_T_new = TjMaxDecode ((UINT8) D0F0xBC_x1F860.Field.BAPMTI_TjMax_1) - TjOffsetDecode ((UINT8) D0F0xBC_x1F870.Field.BAPMTI_TjOffset_1) - (SwTjOffset * 100) - 4500;
  IDS_HDT_CONSOLE (GNB_TRACE, "Cpu1 Delta T org %x (%d)\n", Delta_T_org, Delta_T_org);
  IDS_HDT_CONSOLE (GNB_TRACE, "Cpu1 Delta T New %x (%d)\n", Delta_T_new, Delta_T_new);
  Cpu_New_Pwr = (CpuPowerDecode ((UINT8) ex999.Field.ex999_1 , StdHeader) * Delta_T_new) / Delta_T_org;
  ex996.Field.ex996_0 = CpuPowerOffsetEncode (Cpu_New_Pwr, CpuPowerDecode ((UINT8) ex999.Field.ex999_1, StdHeader), StdHeader);
  Cpu_New_Pwr = (CpuPowerDecode ((UINT8) ex999.Field.ex999_0 , StdHeader) * Delta_T_new) / Delta_T_org;
  ex996.Field.ex996_1 = CpuPowerOffsetEncode (Cpu_New_Pwr, CpuPowerDecode ((UINT8) ex999.Field.ex999_0 , StdHeader), StdHeader);
  Cpu_New_Pwr = (CpuPowerDecode ((UINT8) ex1001.Field.ex1001_1 , StdHeader) * Delta_T_new) / Delta_T_org;
  ex998.Field.ex998_1  = CpuPowerOffsetEncode (Cpu_New_Pwr, CpuPowerDecode ((UINT8) ex1001.Field.ex1001_1 , StdHeader), StdHeader);

  //GPU
  Delta_T_org = TjMaxDecode ((UINT8) D0F0xBC_x1F864.Field.BAPMTI_GpuTjMax) - TjOffsetDecode ((UINT8) D0F0xBC_x1F870.Field.BAPMTI_TjOffset_2) - 4500;
  Delta_T_new = TjMaxDecode ((UINT8) D0F0xBC_x1F864.Field.BAPMTI_GpuTjMax) - TjOffsetDecode ((UINT8) D0F0xBC_x1F870.Field.BAPMTI_TjOffset_2) - (SwTjOffset * 100) - 4500;
  IDS_HDT_CONSOLE (GNB_TRACE, "Gpu Delta T org %x (%d)\n", Delta_T_org, Delta_T_org);
  IDS_HDT_CONSOLE (GNB_TRACE, "Gpu Delta T New %x (%d)\n", Delta_T_new, Delta_T_new);
  Gpu_New_Pwr = (GpuPowerDecode ((UINT16) ex1000.Field.ex1000_1 , StdHeader) * Delta_T_new) / Delta_T_org;
  ex997.Field.ex997_0  = (UINT16) (Gpu_New_Pwr - GpuPowerDecode ((UINT16) ex1000.Field.ex1000_1 , StdHeader));
  Gpu_New_Pwr = (GpuPowerDecode ((UINT16) ex1000.Field.ex1000_0 , StdHeader) * Delta_T_new) / Delta_T_org;
  ex997.Field.ex997_1  = (UINT16) (Gpu_New_Pwr - GpuPowerDecode ((UINT16) ex1000.Field.ex1000_0 , StdHeader));
  Gpu_New_Pwr = (GpuPowerDecode ((UINT16) ex1001.Field.ex1001_0 , StdHeader) * Delta_T_new) / Delta_T_org;
  ex998.Field.ex998_0  = (UINT16) (Gpu_New_Pwr - GpuPowerDecode ((UINT16) ex1001.Field.ex1001_0 , StdHeader));

  //SwTjOffset
  D0F0xBC_x1F6B4.Field.TjOffset = SwTjOffset;

  GnbRegisterWriteTN (TYPE_D0F0xBC , 0x1f6ac , &ex996, 0, StdHeader);
  GnbRegisterWriteTN (TYPE_D0F0xBC , 0x1f6b0 , &ex997, 0, StdHeader);
  GnbRegisterWriteTN (D0F0xBC_x1F6B4_TYPE, D0F0xBC_x1F6B4_ADDRESS, &D0F0xBC_x1F6B4, 0, StdHeader);
  GnbRegisterWriteTN (TYPE_D0F0xBC , 0x1F6B8 , &ex998, 0, StdHeader);

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbSoftwareTjOffsetTN Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init TDC
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

STATIC VOID
GnbInitTdc (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AMD_EARLY_PARAMS        *EarlyParams;
  D0F0xBC_x1F62C_STRUCT   D0F0xBC_x1F62C;
  D0F0xBC_x1F840_STRUCT   D0F0xBC_x1F840;

  EarlyParams = (AMD_EARLY_PARAMS *) StdHeader;
  D0F0xBC_x1F62C.Field.Idd = EarlyParams->PlatformConfig.VrmProperties[CoreVrm].CurrentLimit / 10;
  D0F0xBC_x1F62C.Field.Iddnb = EarlyParams->PlatformConfig.VrmProperties[NbVrm].CurrentLimit / 10;
  GnbRegisterWriteTN (D0F0xBC_x1F62C_TYPE, D0F0xBC_x1F62C_ADDRESS, &D0F0xBC_x1F62C, 0, StdHeader);

  D0F0xBC_x1F840.Field.IddspikeOCP = EarlyParams->PlatformConfig.VrmProperties[CoreVrm].SviOcpLevel / 10;
  D0F0xBC_x1F840.Field.IddNbspikeOCP = EarlyParams->PlatformConfig.VrmProperties[NbVrm].SviOcpLevel / 10;
  GnbRegisterWriteTN (D0F0xBC_x1F840_TYPE, D0F0xBC_x1F840_ADDRESS, &D0F0xBC_x1F840, 0, StdHeader);
}


/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Early Post Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
GnbEarlyInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS                     Status;
  GNB_HANDLE                       *GnbHandle;
  UINT32                           Property;
  D0F0xBC_xE0104188_STRUCT         D0F0xBC_xE0104188;

  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEarlyInterfaceTN Enter\n");
  Property = TABLE_PROPERTY_DEAFULT;

  //Check fuse to support BAPM disabled.
  GnbRegisterReadTN (D0F0xBC_xE0104188_TYPE, D0F0xBC_xE0104188_ADDRESS, &D0F0xBC_xE0104188, 0, StdHeader);
  if (D0F0xBC_xE0104188.Field.BapmDisable == 0) {
    Property |= GnbBuildOptions.CfgBapmSupport ? TABLE_PROPERTY_BAPM : 0;
  }

  IDS_OPTION_HOOK (IDS_GNB_PROPERTY, &Property, StdHeader);

  // SMU LHTC support init
  GnbBapmLhtcInitTN (StdHeader);

  if ((Property & TABLE_PROPERTY_BAPM) == TABLE_PROPERTY_BAPM) {
    GnbTjOffsetUpdateTN (StdHeader);
    GnbSoftwareTjOffsetTN (StdHeader);
    GnbBapmCalculateCoeffsTN (StdHeader);
    GnbCacEnablement (StdHeader);
    GnbBapmMeasuredTempTN (StdHeader);
  } else {
    //  If BAPM is disabled (either through fusing or CBS option), AGESA should enable LHTC algorithm.
    //  Right now, LHTC is only enabled in the "DisableBAPM()" firmware routine, so unless Driver specifically calls this message,
    //  LHTC will never be enabled if BAPM is disabled from the start.
    GnbLhtcEnableTN (StdHeader);
  }

  GnbInitTdc (StdHeader);
  GnbHandle = GnbGetHandle (StdHeader);
  ASSERT (GnbHandle != NULL);
  Status = GnbProcessTable (
             GnbHandle,
             GnbEarlyInitTableTN,
             Property,
             0,
             StdHeader
             );
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEarlyInterfaceTN Exit [0x%x]\n", Status);
  return  Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Early Post Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
GnbEarlierInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS              Status;
  GNB_HANDLE                *GnbHandle;
  D0F0xBC_xE0107060_STRUCT  D0F0xBC_xE0107060;
  D0F0xBC_xE0001008_STRUCT  D0F0xBC_xE0001008;
  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEarlierInterfaceTN Enter\n");

  GnbAdjustSmuVidBeforeSmuTN (StdHeader);

  GnbHandle = GnbGetHandle (StdHeader);
  ASSERT (GnbHandle != NULL);
  GnbRegisterReadTN (D0F0xBC_xE0107060_TYPE, D0F0xBC_xE0107060_ADDRESS, &D0F0xBC_xE0107060, 0, StdHeader);
  GnbRegisterReadTN (D0F0xBC_xE0001008_TYPE, D0F0xBC_xE0001008_ADDRESS, &D0F0xBC_xE0001008, 0, StdHeader);
  GfxRequestVoltageTN ((UINT8) D0F0xBC_xE0001008.Field.SClkVid1, StdHeader);
  GfxRequestSclkTN ((UINT8) D0F0xBC_xE0107060.Field.SClkDpmDid1, StdHeader);
  Status = GnbProcessTable (
             GnbHandle,
             GnbEarlierInitTableBeforeSmuTN,
             0,
             0,
             StdHeader
             );
  GnbSmuFirmwareLoadV4 (GnbHandle->Address, (FIRMWARE_HEADER_V4*) &FirmwareTN[0], StdHeader);
  Status = GnbProcessTable (
             GnbHandle,
             GnbEarlierInitTableAfterSmuTN,
             0,
             0,
             StdHeader
             );

  GnbAdjustSmuVidAfterSmuTN (StdHeader);

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEarlierInterfaceTN Exit [0x%x]\n", Status);
  return  Status;
}
