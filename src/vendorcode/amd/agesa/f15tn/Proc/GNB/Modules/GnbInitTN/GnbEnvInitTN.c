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
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
