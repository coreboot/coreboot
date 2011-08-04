/* $NoKeywords:$ */
/**
 * @file
 *
 * NB power management features
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 47490 $   @e \$Date: 2011-02-22 08:34:28 -0700 (Tue, 22 Feb 2011) $
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
#include  "Gnb.h"
#include  "GnbFuseTable.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  "NbConfigData.h"
#include  "NbSmuLib.h"
#include  "NbFamilyServices.h"
#include  "NbPowerMgmt.h"
#include  "OptionGnb.h"
#include  "GfxLib.h"
#include  "GnbRegistersON.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_NB_NBPOWERMGMT_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern GNB_BUILD_OPTIONS  GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
*/

VOID
NbInitLclkDeepSleep (
  IN      GNB_PLATFORM_CONFIG *Gnb
  );

VOID
NbInitClockGating (
  IN      GNB_PLATFORM_CONFIG *Gnb
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Init various power management features
 *
 *
 *
 * @param[in] Gnb             Pointer to global Gnb configuration
 * @retval    AGESA_SUCCESS   LCLK DPM initialization  success
 * @retval    AGESA_ERROR     LCLK DPM initialization  error
 */

AGESA_STATUS
NbInitPowerManagement (
  IN      GNB_PLATFORM_CONFIG *Gnb
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;
  NbInitLclkDeepSleep (Gnb);
  NbInitClockGating (Gnb);
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Init NB LCLK Deep Sleep
 *
 *
 *
 * @param[in] Gnb             Pointer to global Gnb configuration
 */

VOID
NbInitLclkDeepSleep (
  IN      GNB_PLATFORM_CONFIG *Gnb
  )
{
  SMUx1B_STRUCT   SMUx1B;
  SMUx1D_STRUCT   SMUx1D;
  UINT32          LclkDpSlpEn;
  IDS_HDT_CONSOLE (GNB_TRACE, "NbInitLclkDeepSleep Enter\n");
  LclkDpSlpEn = GnbBuildOptions.LclkDeepSleepEn ? 1 : 0;
  NbSmuIndirectRead (SMUx1B_ADDRESS, AccessWidth16, &SMUx1B.Value, Gnb->StdHeader);
  NbSmuIndirectRead (SMUx1D_ADDRESS, AccessWidth16, &SMUx1D.Value, Gnb->StdHeader);
  SMUx1B.Field.LclkDpSlpDiv = 5;
  SMUx1B.Field.LclkDpSlpMask = (GfxLibIsControllerPresent (Gnb->StdHeader) ? (0xFF) : 0xEF);
  SMUx1B.Field.RampDis = 0;
  SMUx1D.Field.LclkDpSlpHyst = 0xf;
  IDS_OPTION_HOOK (IDS_GNB_LCLK_DEEP_SLEEP, &LclkDpSlpEn, Gnb->StdHeader);
  SMUx1D.Field.LclkDpSlpEn = LclkDpSlpEn;
  IDS_HDT_CONSOLE (GNB_TRACE, "  LCLK Deep Sleep [%s]\n", (LclkDpSlpEn != 0) ? "Enabled" : "Disabled");
  NbSmuIndirectWrite (SMUx1B_ADDRESS, AccessS3SaveWidth16, &SMUx1B.Value, Gnb->StdHeader);
  NbSmuIndirectWrite (SMUx1D_ADDRESS, AccessS3SaveWidth16, &SMUx1D.Value, Gnb->StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "NbInitLclkDeepSleep Exit\n");
}

/**
 * Init NB SMU clock gating
 *
 *
 *
 * @param[in] NbClkGatingCtrl             Pointer to Clock gating control structure
 * @param[in] Gnb             Pointer to global Gnb configuration
 */

VOID
NbInitSmuClockGating (
  IN      NB_CLK_GATING_CTRL  *NbClkGatingCtrl,
  IN      GNB_PLATFORM_CONFIG *Gnb
  )
{
  BOOLEAN       Smu_Lclk_Gating;
  BOOLEAN       Smu_Sclk_Gating;
  SMUx73_STRUCT SMUx73;
  UINT32        Value;

  Smu_Lclk_Gating = NbClkGatingCtrl->Smu_Lclk_Gating;
  Smu_Sclk_Gating = NbClkGatingCtrl->Smu_Sclk_Gating;
//SMUx6F
  Value = 0x006001F0;
  NbSmuIndirectWrite (SMUx6F_ADDRESS, AccessS3SaveWidth32, &Value, Gnb->StdHeader);
//SMUx71
  Value = 0x007001F0;
  NbSmuIndirectWrite (SMUx71_ADDRESS, AccessS3SaveWidth32, &Value, Gnb->StdHeader);
//SMUx73
  NbSmuIndirectRead (SMUx73_ADDRESS, AccessWidth16, &SMUx73.Value, Gnb->StdHeader);
  SMUx73.Field.DisLclkGating = Smu_Lclk_Gating ? 0 : 1;
  SMUx73.Field.DisSclkGating = Smu_Sclk_Gating ? 0 : 1;
  NbSmuIndirectWrite (SMUx73_ADDRESS, AccessS3SaveWidth16, &SMUx73.Value, Gnb->StdHeader);

}

/**
 * Init NB ORB clock gating
 *
 *
 *
 * @param[in] NbClkGatingCtrl             Pointer to Clock gating control structure
 * @param[in] Gnb             Pointer to global Gnb configuration
 */

VOID
NbInitOrbClockGating (
  IN      NB_CLK_GATING_CTRL *NbClkGatingCtrl,
  IN      GNB_PLATFORM_CONFIG *Gnb
  )
{
  BOOLEAN                Orb_Sclk_Gating;
  BOOLEAN                Orb_Lclk_Gating;
  D0F0x98_x49_STRUCT     D0F0x98_x49;
  D0F0x98_x4A_STRUCT     D0F0x98_x4A;
  D0F0x98_x4B_STRUCT     D0F0x98_x4B;
  FCRxFF30_01F5_STRUCT   FCRxFF30_01F5;

  Orb_Sclk_Gating = NbClkGatingCtrl->Orb_Sclk_Gating;
  Orb_Lclk_Gating = NbClkGatingCtrl->Orb_Lclk_Gating;

  // ORB clock gating (Lclk)
//D0F0x98_x4[A:9]
  GnbLibPciIndirectRead (
    Gnb->GnbPciAddress.AddressValue | D0F0x94_ADDRESS,
    D0F0x98_x49_ADDRESS,
    AccessWidth32,
    &D0F0x98_x49.Value,
    Gnb->StdHeader
  );

  D0F0x98_x49.Field.SoftOverrideClk6 =  Orb_Lclk_Gating ? 0 : 1;
  D0F0x98_x49.Field.SoftOverrideClk5 =  Orb_Lclk_Gating ? 0 : 1;
  D0F0x98_x49.Field.SoftOverrideClk4 =  Orb_Lclk_Gating ? 0 : 1;
  D0F0x98_x49.Field.SoftOverrideClk3 =  Orb_Lclk_Gating ? 0 : 1;
  D0F0x98_x49.Field.SoftOverrideClk2 =  Orb_Lclk_Gating ? 0 : 1;
  D0F0x98_x49.Field.SoftOverrideClk1 =  Orb_Lclk_Gating ? 0 : 1;
  D0F0x98_x49.Field.SoftOverrideClk0 =  Orb_Lclk_Gating ? 0 : 1;

  GnbLibPciIndirectWrite (
    Gnb->GnbPciAddress.AddressValue | D0F0x94_ADDRESS,
    D0F0x98_x49_ADDRESS | (1 << D0F0x94_OrbIndWrEn_OFFSET),
    AccessS3SaveWidth32,
    &D0F0x98_x49.Value,
    Gnb->StdHeader
  );

  GnbLibPciIndirectRead (
    Gnb->GnbPciAddress.AddressValue | D0F0x94_ADDRESS,
    D0F0x98_x4A_ADDRESS | (1 << D0F0x94_OrbIndWrEn_OFFSET),
    AccessWidth32,
    &D0F0x98_x4A.Value,
    Gnb->StdHeader
  );

  D0F0x98_x4A.Field.SoftOverrideClk6 =  Orb_Lclk_Gating ? 0 : 1;
  D0F0x98_x4A.Field.SoftOverrideClk5 =  Orb_Lclk_Gating ? 0 : 1;
  D0F0x98_x4A.Field.SoftOverrideClk4 =  Orb_Lclk_Gating ? 0 : 1;
  D0F0x98_x4A.Field.SoftOverrideClk3 =  Orb_Lclk_Gating ? 0 : 1;
  D0F0x98_x4A.Field.SoftOverrideClk2 =  Orb_Lclk_Gating ? 0 : 1;
  D0F0x98_x4A.Field.SoftOverrideClk1 =  Orb_Lclk_Gating ? 0 : 1;
  D0F0x98_x4A.Field.SoftOverrideClk0 =  Orb_Lclk_Gating ? 0 : 1;


  GnbLibPciIndirectWrite (
    Gnb->GnbPciAddress.AddressValue | D0F0x94_ADDRESS,
    D0F0x98_x4A_ADDRESS | (1 << D0F0x94_OrbIndWrEn_OFFSET),
    AccessS3SaveWidth32,
    &D0F0x98_x4A.Value,
    Gnb->StdHeader
  );

//D0F0x98_x4B
  GnbLibPciIndirectRead (
    Gnb->GnbPciAddress.AddressValue | D0F0x94_ADDRESS,
    D0F0x98_x4B_ADDRESS | (1 << D0F0x94_OrbIndWrEn_OFFSET),
    AccessWidth32,
    &D0F0x98_x4B.Value,
    Gnb->StdHeader
  );

  D0F0x98_x4B.Field.SoftOverrideClk = Orb_Sclk_Gating ? 0 : 1;

  GnbLibPciIndirectWrite (
    Gnb->GnbPciAddress.AddressValue | D0F0x94_ADDRESS,
    D0F0x98_x4B_ADDRESS | (1 << D0F0x94_OrbIndWrEn_OFFSET),
    AccessS3SaveWidth32,
    &D0F0x98_x4B.Value,
    Gnb->StdHeader
  );

//FCRxFF30_01F5[CgOrbCgttLclkOverride, CgOrbCgttSclkOverride]
  NbSmuSrbmRegisterRead (FCRxFF30_01F5_ADDRESS, &FCRxFF30_01F5.Value, Gnb->StdHeader);
  FCRxFF30_01F5.Field.CgOrbCgttLclkOverride = 0;
  FCRxFF30_01F5.Field.CgOrbCgttSclkOverride = 0;
  NbSmuSrbmRegisterWrite (FCRxFF30_01F5_ADDRESS, &FCRxFF30_01F5.Value, TRUE, Gnb->StdHeader);

}

/**
 * Init NB IOC clock gating
 *
 *
 *
 * @param[in] NbClkGatingCtrl             Pointer to Clock gating control structure
 * @param[in] Gnb             Pointer to global Gnb configuration
 */

VOID
NbInitIocClockGating (
  IN      NB_CLK_GATING_CTRL *NbClkGatingCtrl,
  IN      GNB_PLATFORM_CONFIG *Gnb
  )
{
  BOOLEAN                Ioc_Lclk_Gating;
  BOOLEAN                Ioc_Sclk_Gating;
  D0F0x64_x22_STRUCT     D0F0x64_x22;
  D0F0x64_x23_STRUCT     D0F0x64_x23;
  D0F0x64_x24_STRUCT     D0F0x64_x24;
  FCRxFF30_01F5_STRUCT   FCRxFF30_01F5;

  Ioc_Lclk_Gating = NbClkGatingCtrl->Ioc_Lclk_Gating;
  Ioc_Sclk_Gating = NbClkGatingCtrl->Ioc_Sclk_Gating;

//D0F0x64_x22
  GnbLibPciIndirectRead (
    Gnb->GnbPciAddress.AddressValue | D0F0x60_ADDRESS,
    D0F0x64_x22_ADDRESS | IOC_WRITE_ENABLE,
    AccessWidth32,
    &D0F0x64_x22.Value,
    Gnb->StdHeader
  );

  D0F0x64_x22.Field.SoftOverrideClk4 = Ioc_Lclk_Gating ? 0 : 1;
  D0F0x64_x22.Field.SoftOverrideClk3 = Ioc_Lclk_Gating ? 0 : 1;
  D0F0x64_x22.Field.SoftOverrideClk2 = Ioc_Lclk_Gating ? 0 : 1;
  D0F0x64_x22.Field.SoftOverrideClk1 = Ioc_Lclk_Gating ? 0 : 1;
  D0F0x64_x22.Field.SoftOverrideClk0 = Ioc_Lclk_Gating ? 0 : 1;

  GnbLibPciIndirectWrite (
    Gnb->GnbPciAddress.AddressValue | D0F0x60_ADDRESS,
    D0F0x64_x22_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &D0F0x64_x22.Value,
    Gnb->StdHeader
  );
//D0F0x64_x23
  GnbLibPciIndirectRead (
    Gnb->GnbPciAddress.AddressValue | D0F0x60_ADDRESS,
    D0F0x64_x23_ADDRESS | IOC_WRITE_ENABLE,
    AccessWidth32,
    &D0F0x64_x23.Value,
    Gnb->StdHeader
  );

  //D0F0x64_x23.Field.SoftOverrideClk4 = Ioc_Lclk_Gating ? 0 : 1;
  D0F0x64_x23.Field.SoftOverrideClk3 = Ioc_Lclk_Gating ? 0 : 1;
  D0F0x64_x23.Field.SoftOverrideClk2 = Ioc_Lclk_Gating ? 0 : 1;
  D0F0x64_x23.Field.SoftOverrideClk1 = Ioc_Lclk_Gating ? 0 : 1;
  D0F0x64_x23.Field.SoftOverrideClk0 = Ioc_Lclk_Gating ? 0 : 1;

  GnbLibPciIndirectWrite (
    Gnb->GnbPciAddress.AddressValue | D0F0x60_ADDRESS,
    D0F0x64_x23_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &D0F0x64_x23.Value,
    Gnb->StdHeader
  );
  //D0F0x64_x24
  GnbLibPciIndirectRead (
    Gnb->GnbPciAddress.AddressValue | D0F0x60_ADDRESS,
    D0F0x64_x24_ADDRESS | IOC_WRITE_ENABLE,
    AccessWidth32,
    &D0F0x64_x24.Value,
    Gnb->StdHeader
  );

  D0F0x64_x24.Field.SoftOverrideClk1 = Ioc_Sclk_Gating ? 0 : 1;
  D0F0x64_x24.Field.SoftOverrideClk0 = Ioc_Sclk_Gating ? 0 : 1;

  GnbLibPciIndirectWrite (
    Gnb->GnbPciAddress.AddressValue | D0F0x60_ADDRESS,
    D0F0x64_x24_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &D0F0x64_x24.Value,
    Gnb->StdHeader
  );
//FCRxFF30_01F5[CgIocCgttLclkOverride, CgIocCgttSclkOverride]
  NbSmuSrbmRegisterRead (FCRxFF30_01F5_ADDRESS, &FCRxFF30_01F5.Value, Gnb->StdHeader);
  FCRxFF30_01F5.Field.CgIocCgttLclkOverride = 0;
  FCRxFF30_01F5.Field.CgIocCgttSclkOverride = 0;
  NbSmuSrbmRegisterWrite (FCRxFF30_01F5_ADDRESS, &FCRxFF30_01F5.Value, TRUE, Gnb->StdHeader);
}
/**
 * Init NB BIF clock gating
 *
 *
 *
 * @param[in] NbClkGatingCtrl             Pointer to Clock gating control structure
 * @param[in] Gnb             Pointer to global Gnb configuration
 */

VOID
NbInitBifClockGating (
  IN      NB_CLK_GATING_CTRL *NbClkGatingCtrl,
  IN      GNB_PLATFORM_CONFIG *Gnb
  )
{
  BOOLEAN               Bif_Sclk_Gating;
  FCRxFF30_01F4_STRUCT  FCRxFF30_01F4;
  FCRxFF30_1512_STRUCT  FCRxFF30_1512;


  Bif_Sclk_Gating = NbClkGatingCtrl->Bif_Sclk_Gating;

//FCRxFF30_01F4[CgBifCgttSclkOverride].
  NbSmuSrbmRegisterRead (FCRxFF30_01F4_ADDRESS, &FCRxFF30_01F4.Value, Gnb->StdHeader);
  FCRxFF30_01F4.Field.CgBifCgttSclkOverride = 0;
  NbSmuSrbmRegisterWrite (FCRxFF30_01F4_ADDRESS, &FCRxFF30_01F4.Value, TRUE, Gnb->StdHeader);
//FCRxFF30_1512
  NbSmuSrbmRegisterRead (FCRxFF30_1512_ADDRESS, &FCRxFF30_1512.Value, Gnb->StdHeader);
  FCRxFF30_1512.Field.SoftOverride0 = Bif_Sclk_Gating ? 0 : 1;
  NbSmuSrbmRegisterWrite (FCRxFF30_1512_ADDRESS, &FCRxFF30_1512.Value, TRUE, Gnb->StdHeader);

}

/**
 * Init NB Gmc clock gating
 *
 *
 *
 * @param[in] NbClkGatingCtrl             Pointer to Clock gating control structure
 * @param[in] Gnb             Pointer to global Gnb configuration
 */

VOID
NbInitGmcClockGating (
  IN      NB_CLK_GATING_CTRL *NbClkGatingCtrl,
  IN      GNB_PLATFORM_CONFIG *Gnb
  )
{
  BOOLEAN               Gmc_Sclk_Gating;
  FCRxFF30_01F4_STRUCT  FCRxFF30_01F4;
  FCRxFF30_01F5_STRUCT  FCRxFF30_01F5;

  Gmc_Sclk_Gating = NbClkGatingCtrl->Gmc_Sclk_Gating;

//FCRxFF30_01F4[CgMcdwCgttSclkOverride, CgMcbCgttSclkOverride]
  NbSmuSrbmRegisterRead (FCRxFF30_01F4_ADDRESS, &FCRxFF30_01F4.Value, Gnb->StdHeader);
  FCRxFF30_01F4.Field.CgMcbCgttSclkOverride = Gmc_Sclk_Gating ? 0 : 1;
  FCRxFF30_01F4.Field.CgMcdwCgttSclkOverride = Gmc_Sclk_Gating ? 0 : 1;
  NbSmuSrbmRegisterWrite (FCRxFF30_01F4_ADDRESS, &FCRxFF30_01F4.Value, TRUE, Gnb->StdHeader);

//FCRxFF30_01F5[CgVmcCgttSclkOverride]
  NbSmuSrbmRegisterRead (FCRxFF30_01F5_ADDRESS, &FCRxFF30_01F5.Value, Gnb->StdHeader);
  FCRxFF30_01F5.Field.CgVmcCgttSclkOverride = Gmc_Sclk_Gating ? 0 : 1;
  NbSmuSrbmRegisterWrite (FCRxFF30_01F5_ADDRESS, &FCRxFF30_01F5.Value, TRUE, Gnb->StdHeader);

}

/**
 * Init NB Dce Sclk clock gating
 *
 *
 *
 * @param[in] NbClkGatingCtrl             Pointer to Clock gating control structure
 * @param[in] Gnb             Pointer to global Gnb configuration
 */

VOID
NbInitDceSclkClockGating (
  IN      NB_CLK_GATING_CTRL *NbClkGatingCtrl,
  IN      GNB_PLATFORM_CONFIG *Gnb
  )
{
  BOOLEAN               Dce_Sclk_Gating;
  FCRxFF30_0134_STRUCT   FCRxFF30_0134;
  FCRxFF30_01F4_STRUCT  FCRxFF30_01F4;

  Dce_Sclk_Gating = NbClkGatingCtrl->Dce_Sclk_Gating;

//GMMx4D0[SymclkbGateDisable, SymclkaGateDisable, SclkGateDisable]
  NbSmuSrbmRegisterRead (FCRxFF30_0134_ADDRESS, &FCRxFF30_0134.Value, Gnb->StdHeader);
  FCRxFF30_0134.Field.SclkGateDisable = Dce_Sclk_Gating ? 0 : 1;
  FCRxFF30_0134.Field.SymclkaGateDisable = Dce_Sclk_Gating ? 0 : 1;
  FCRxFF30_0134.Field.SymclkbGateDisable = Dce_Sclk_Gating ? 0 : 1;
  NbSmuSrbmRegisterWrite (FCRxFF30_0134_ADDRESS, &FCRxFF30_0134.Value, TRUE, Gnb->StdHeader);

//FCRxFF30_01F4[CgDcCgttSclkOverride]
  NbSmuSrbmRegisterRead (FCRxFF30_01F4_ADDRESS, &FCRxFF30_01F4.Value, Gnb->StdHeader);
  FCRxFF30_01F4.Field.CgDcCgttSclkOverride = 0;
  NbSmuSrbmRegisterWrite (FCRxFF30_01F4_ADDRESS, &FCRxFF30_01F4.Value, TRUE, Gnb->StdHeader);

}

/**
 * Init NB Dce Display clock gating
 *
 *
 *
 * @param[in] NbClkGatingCtrl             Pointer to Clock gating control structure
 * @param[in] Gnb             Pointer to global Gnb configuration
 */

VOID
NbInitDceDisplayClockGating (
  IN      NB_CLK_GATING_CTRL *NbClkGatingCtrl,
  IN      GNB_PLATFORM_CONFIG *Gnb
  )
{
  BOOLEAN               Dce_Dispclk_Gating;
  FCRxFF30_0134_STRUCT   FCRxFF30_0134;
  FCRxFF30_1B7C_STRUCT  FCRxFF30_1B7C;
  FCRxFF30_1E7C_STRUCT  FCRxFF30_1E7C;
  FCRxFF30_01F5_STRUCT  FCRxFF30_01F5;

  Dce_Dispclk_Gating = NbClkGatingCtrl->Dce_Dispclk_Gating;

//GMMx4D0[DispclkRDccgGateDisable,DispclkDccgGateDisable]
  NbSmuSrbmRegisterRead (FCRxFF30_0134_ADDRESS, &FCRxFF30_0134.Value, Gnb->StdHeader);
  FCRxFF30_0134.Field.DispclkDccgGateDisable = Dce_Dispclk_Gating ? 0 : 1;
  FCRxFF30_0134.Field.DispclkRDccgGateDisable = Dce_Dispclk_Gating ? 0 : 1;
  NbSmuSrbmRegisterWrite (FCRxFF30_0134_ADDRESS, &FCRxFF30_0134.Value, TRUE, Gnb->StdHeader);

//GMMx[79,6D]F0[CrtcDispclkGSclGateDisable, CrtcDispclkGDcpGateDisable, CrtcDispclkRDcfeGateDisable]
  NbSmuSrbmRegisterRead (FCRxFF30_1B7C_ADDRESS, &FCRxFF30_1B7C.Value, Gnb->StdHeader);
  FCRxFF30_1B7C.Field.CrtcDispclkRDcfeGateDisable = Dce_Dispclk_Gating ? 0 : 1;
  FCRxFF30_1B7C.Field.CrtcDispclkGDcpGateDisable = Dce_Dispclk_Gating ? 0 : 1;
  FCRxFF30_1B7C.Field.CrtcDispclkGSclGateDisable = Dce_Dispclk_Gating ? 0 : 1;
  NbSmuSrbmRegisterWrite (FCRxFF30_1B7C_ADDRESS, &FCRxFF30_1B7C.Value, TRUE, Gnb->StdHeader);

  NbSmuSrbmRegisterRead (FCRxFF30_1E7C_ADDRESS, &FCRxFF30_1E7C.Value, Gnb->StdHeader);
  FCRxFF30_1E7C.Field.CrtcDispclkRDcfeGateDisable = Dce_Dispclk_Gating ? 0 : 1;
  FCRxFF30_1E7C.Field.CrtcDispclkGDcpGateDisable = Dce_Dispclk_Gating ? 0 : 1;
  FCRxFF30_1E7C.Field.CrtcDispclkGSclGateDisable = Dce_Dispclk_Gating ? 0 : 1;
  NbSmuSrbmRegisterWrite (FCRxFF30_1E7C_ADDRESS, &FCRxFF30_1E7C.Value, TRUE, Gnb->StdHeader);

//FCRxFF30_01F5[CgDcCgttDispclkOverride]
  NbSmuSrbmRegisterRead (FCRxFF30_01F5_ADDRESS, &FCRxFF30_01F5.Value, Gnb->StdHeader);
  FCRxFF30_01F5.Field.CgDcCgttDispclkOverride = 0;
  NbSmuSrbmRegisterWrite (FCRxFF30_01F5_ADDRESS, &FCRxFF30_01F5.Value, TRUE, Gnb->StdHeader);

}

/*----------------------------------------------------------------------------------------*/
/**
 * Init NB clock gating
 *
 *
 *
 * @param[in] Gnb             Pointer to global Gnb configuration
 */

VOID
NbInitClockGating (
  IN      GNB_PLATFORM_CONFIG *Gnb
  )
{
  NB_CLK_GATING_CTRL NbClkGatingCtrl;

  //Init the default value of control structure.
  NbClkGatingCtrl.Smu_Sclk_Gating = GnbBuildOptions.SmuSclkClockGatingEnable;
  NbClkGatingCtrl.Smu_Lclk_Gating = TRUE;
  NbClkGatingCtrl.Orb_Sclk_Gating = TRUE;
  NbClkGatingCtrl.Orb_Lclk_Gating = TRUE;
  NbClkGatingCtrl.Ioc_Sclk_Gating = TRUE;
  NbClkGatingCtrl.Ioc_Lclk_Gating = TRUE;
  NbClkGatingCtrl.Bif_Sclk_Gating = TRUE;
  NbClkGatingCtrl.Gmc_Sclk_Gating = TRUE;
  NbClkGatingCtrl.Dce_Sclk_Gating = TRUE;
  NbClkGatingCtrl.Dce_Dispclk_Gating = TRUE;

  NbFmNbClockGating (&NbClkGatingCtrl, Gnb->StdHeader);

  IDS_OPTION_HOOK (IDS_GNB_CLOCK_GATING, &NbClkGatingCtrl, Gnb->StdHeader);


  IDS_HDT_CONSOLE (GNB_TRACE, "NbInitClockGating Enter\n");

//SMU SCLK/LCLK clock gating
  NbInitSmuClockGating (&NbClkGatingCtrl, Gnb);

// ORB clock gating
  NbInitOrbClockGating (&NbClkGatingCtrl, Gnb);

//IOC clock gating
  NbInitIocClockGating (&NbClkGatingCtrl, Gnb);

//BIF Clock Gating
  NbInitBifClockGating (&NbClkGatingCtrl, Gnb);

//GMC Clock Gating
  NbInitGmcClockGating (&NbClkGatingCtrl, Gnb);

//DCE Sclk clock gating
  NbInitDceSclkClockGating (&NbClkGatingCtrl, Gnb);

//DCE Display clock gating
  NbInitDceDisplayClockGating (&NbClkGatingCtrl, Gnb);

  GNB_DEBUG_CODE (
    {
      FCRxFF30_01F4_STRUCT  FCRxFF30_01F4;
      FCRxFF30_01F5_STRUCT  FCRxFF30_01F5;
      FCRxFF30_1512_STRUCT  FCRxFF30_1512;
      NbSmuSrbmRegisterRead (FCRxFF30_01F4_ADDRESS, &FCRxFF30_01F4.Value, Gnb->StdHeader);
      NbSmuSrbmRegisterRead (FCRxFF30_01F5_ADDRESS, &FCRxFF30_01F5.Value, Gnb->StdHeader);
      NbSmuSrbmRegisterRead (FCRxFF30_1512_ADDRESS, &FCRxFF30_1512.Value, Gnb->StdHeader);
      IDS_HDT_CONSOLE (NB_MISC, "   Clock Gating FCRxFF30_01F4 - 0x%x\n", FCRxFF30_01F4.Value);
      IDS_HDT_CONSOLE (NB_MISC, "   Clock Gating FCRxFF30_01F5 - 0x%x\n", FCRxFF30_01F5.Value);
      IDS_HDT_CONSOLE (NB_MISC, "   Clock Gating FCRxFF30_1512 - 0x%x\n", FCRxFF30_1512.Value);
    }
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "NbInitClockGating End\n");
}
