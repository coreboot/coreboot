/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe late post initialization.
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
#include  "Gnb.h"
#include  "GnbCommonLib.h"
#include  "GnbTable.h"
#include  "GnbPcieConfig.h"
#include  "GnbNbInitLibV1.h"
#include  "GnbNbInitLibV4.h"
#include  "GnbFuseTableTN.h"
#include  "GnbRegisterAccTN.h"
#include  "GnbRegistersTN.h"
#include  "heapManager.h"
#include  "GnbFuseTable.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_GNBENVINITTN_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern GNB_BUILD_OPTIONS      GnbBuildOptions;
extern GNB_TABLE ROMDATA      GnbEnvInitTableTN [];

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GnbEnvInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );


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
GnbEnvInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS                   Status;
  AMD_ENV_PARAMS                 *EnvParamsPtr;
  UINT32                         Property;
  GNB_HANDLE                     *GnbHandle;
  D18F5x170_STRUCT               D18F5x170;
  D0F0xBC_x1F8DC_STRUCT          D0F0xBC_x1F8DC;
  PP_FUSE_ARRAY                  *PpFuseArray;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnvInterfaceTN Enter\n");
  Property = TABLE_PROPERTY_DEAFULT;
  EnvParamsPtr = (AMD_ENV_PARAMS *) StdHeader;
  GnbHandle = GnbGetHandle (StdHeader);
  ASSERT (GnbHandle != NULL);
  GnbLoadFuseTableTN (StdHeader);
  Status = GnbSetTom (GnbGetHostPciAddress (GnbHandle), StdHeader);
  GnbOrbDynamicWake (GnbGetHostPciAddress (GnbHandle), StdHeader);
  GnbClumpUnitIdV4 (GnbHandle, StdHeader);
  GnbLpcDmaDeadlockPreventionV4 (GnbHandle, StdHeader);
  Property |= GnbBuildOptions.CfgLoadlineEnable ? TABLE_PROPERTY_LOADLINE_ENABLE : 0;
  Property |= GnbBuildOptions.CfgIommuL1ClockGatingEnable ? TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING : 0;
  Property |= GnbBuildOptions.CfgIommuL2ClockGatingEnable ? TABLE_PROPERTY_IOMMU_L2_CLOCK_GATING : 0;
  if (!EnvParamsPtr->GnbEnvConfiguration.IommuSupport) {
    Property |= TABLE_PROPERTY_IOMMU_DISABLED;
  }

  if (GnbBuildOptions.CfgNbdpmEnable) {
    GnbRegisterReadTN (
      TYPE_D18F5,
      D18F5x170_ADDRESS,
      &D18F5x170.Value,
      0,
      StdHeader
    );
    // Check if NbPstate enbale
    if ((D18F5x170.Field.SwNbPstateLoDis != 1) && (D18F5x170.Field.NbPstateMaxVal != 0)) {
      Property |= TABLE_PROPERTY_NBDPM;
      PpFuseArray = GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, StdHeader);
      if (PpFuseArray != NULL) {
        // NBDPM is requesting SclkVid0 from the register.
        // Write them back if SclkVid has been changed in PpFuseArray.
        GnbRegisterReadTN (D0F0xBC_x1F8DC_TYPE, D0F0xBC_x1F8DC_ADDRESS, &D0F0xBC_x1F8DC.Value, 0, StdHeader);
        if ((D0F0xBC_x1F8DC.Field.SClkVid0 != PpFuseArray->SclkVid[0]) ||
            (D0F0xBC_x1F8DC.Field.SClkVid1 != PpFuseArray->SclkVid[1]) ||
            (D0F0xBC_x1F8DC.Field.SClkVid2 != PpFuseArray->SclkVid[2]) ||
            (D0F0xBC_x1F8DC.Field.SClkVid3 != PpFuseArray->SclkVid[3])) {
          D0F0xBC_x1F8DC.Field.SClkVid0 = PpFuseArray->SclkVid[0];
          D0F0xBC_x1F8DC.Field.SClkVid1 = PpFuseArray->SclkVid[1];
          D0F0xBC_x1F8DC.Field.SClkVid2 = PpFuseArray->SclkVid[2];
          D0F0xBC_x1F8DC.Field.SClkVid3 = PpFuseArray->SclkVid[3];
          GnbRegisterWriteTN (D0F0xBC_x1F8DC_TYPE, D0F0xBC_x1F8DC_ADDRESS, &D0F0xBC_x1F8DC.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
        }
      }
    }
  }

  IDS_OPTION_HOOK (IDS_GNB_PROPERTY, &Property, StdHeader);

  Status = GnbProcessTable (
             GnbHandle,
             GnbEnvInitTableTN,
             Property,
             GNB_TABLE_FLAGS_FORCE_S3_SAVE,
             StdHeader
             );
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnvInterfaceTN Exit [0x%x]\n", Status);
  return  Status;
}
