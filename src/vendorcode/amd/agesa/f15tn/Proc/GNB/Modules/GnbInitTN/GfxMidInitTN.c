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
 * @e \$Revision: 65199 $   @e \$Date: 2012-02-09 21:36:06 -0600 (Thu, 09 Feb 2012) $
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
#include  "GnbGfx.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbGfxConfig.h"
#include  "GnbGfxInitLibV1.h"
#include  "GnbNbInitLibV1.h"
#include  "GnbGfxFamServices.h"
#include  "GfxLibTN.h"
#include  "GfxGmcInitTN.h"
#include  "GnbRegisterAccTN.h"
#include  "GnbRegistersTN.h"
#include  "PcieConfigData.h"
#include  "PcieConfigLib.h"
#include  "cpuFamilyTranslation.h"
#include  "GnbHandleLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_GFXMIDINITTN_FILECODE
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
AGESA_STATUS
GfxIntegratedEnumerateAudioConnectors (
  IN      GFX_PLATFORM_CONFIG         *Gfx
  );

AGESA_STATUS
GfxMidInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

VOID
exec803 /* GfxAzWorkaroundTN */ (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );
/*----------------------------------------------------------------------------------------*/
/**
 * Set boot up voltage
 *
 *
 * @param[in] Gfx             Pointer to global GFX configuration
 * @retval    AGESA_STATUS
 */

STATIC AGESA_STATUS
GfxSetBootUpVoltageTN (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxSetBootUpVoltageTN Enter\n");
  GfxRequestVoltageTN (GnbLocateHighestVidCode (GnbLibGetHeader (Gfx)), GnbLibGetHeader (Gfx));
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set boot up voltage
 *
 *
 * @param[in] Gfx             Pointer to global GFX configuration
 */

VOID
exec803 /* GfxAzWorkaroundTN */ (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINT32                  i;
  UINT32                  Address;
  UINT32                  Data;


  Data = 0x156;
  for (i = 0; i < 6; i++) {
    Address = 0x5E00 + (i * 0x18);
    GnbLibMemWrite (Gfx->GmmBase + Address, AccessS3SaveWidth32, &Data, GnbLibGetHeader (Gfx));
    GnbLibMemRMW (Gfx->GmmBase + Address + 4, AccessS3SaveWidth32, 0xFFFFFF00, 0xF0, GnbLibGetHeader (Gfx));
  }

  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init GFX at Mid Post.
 *
 *
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
GfxMidInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          Status;
  AGESA_STATUS          AgesaStatus;
  GFX_PLATFORM_CONFIG   *Gfx;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxMidInterfaceTN Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status =  GfxLocateConfigData (StdHeader, &Gfx);
  ASSERT (Status == AGESA_SUCCESS);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_FATAL) {
    GfxFmDisableController (StdHeader);
  } else {
    if (Gfx->UmaInfo.UmaMode != UMA_NONE) {
      Status = GfxEnableGmmAccess (Gfx);
      ASSERT (Status == AGESA_SUCCESS);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
      if (Status != AGESA_SUCCESS) {
        // Can not initialize GMM registers going to disable GFX controller
        IDS_HDT_CONSOLE (GNB_TRACE, "  Fail to establish GMM access\n");
        Gfx->UmaInfo.UmaMode = UMA_NONE;
        GfxFmDisableController (StdHeader);
      } else {
        Status = GfxGmcInitTN (Gfx);
        AGESA_STATUS_UPDATE (Status, AgesaStatus);

        Status = GfxSetBootUpVoltageTN (Gfx);
        AGESA_STATUS_UPDATE (Status, AgesaStatus);

        Status = GfxInitSsid (Gfx);
        AGESA_STATUS_UPDATE (Status, AgesaStatus);

        Status = GfxIntegratedEnumerateAudioConnectors (Gfx);
        AGESA_STATUS_UPDATE (Status, AgesaStatus);

        exec803 /* GfxAzWorkaroundTN */ (Gfx);
      }
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxMidInterfaceTN Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Determine number of audio ports for each connector
 *
 *
 *
 * @param[in]     Engine  Engine configuration info
 * @param[in,out] Buffer  Buffer pointer
 * @param[in]     Pcie    PCIe configuration info
 */
VOID
STATIC
GfxIntegratedAudioEnumCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8    *AudioCount;
  AudioCount = (UINT8*) Buffer;
  if (Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeHDMI) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Found HDMI Connector\n");
    (*AudioCount)++;
  } else if (Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeDP) {
    if ((Engine->Type.Ddi.DdiData.Flags & DDI_DATA_FLAGS_DP1_1_ONLY) == 0) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Found DP1.2 Connector\n");
      *AudioCount += 4;
    } else {
      IDS_HDT_CONSOLE (GNB_TRACE, "Found DP1.1 Connector\n");
      (*AudioCount)++;
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "New AudioCount = %d\n", *AudioCount);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enumerate all display connectors with audio capability and configure number of ports
 *
 *
 *
 * @param[in]     Gfx             Gfx configuration info
 */
AGESA_STATUS
GfxIntegratedEnumerateAudioConnectors (
  IN      GFX_PLATFORM_CONFIG         *Gfx
  )
{
  UINT8                   AudioCount;
  AGESA_STATUS            Status;
  GMMx5F50_STRUCT         GMMx5F50;
  PCIe_PLATFORM_CONFIG    *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntegratedEnumerateAudioConnectors Enter\n");

  Status = PcieLocateConfigurationData (GnbLibGetHeader (Gfx), &Pcie);
  if ((Status == AGESA_SUCCESS) && (Gfx->GnbHdAudio != 0)) {
    AudioCount = 0;
    PcieConfigRunProcForAllEngines (
      DESCRIPTOR_ALLOCATED | DESCRIPTOR_VIRTUAL | DESCRIPTOR_DDI_ENGINE,
      GfxIntegratedAudioEnumCallback,
      &AudioCount,
      Pcie
      );
    if (AudioCount > 4) {
      AudioCount = 4;
    }
    GMMx5F50.Value = 0x00;
    GMMx5F50.Field.PortConnectivity = (7 - AudioCount);
    GMMx5F50.Field.PortConnectivityOverrideEnable = 1;
    GnbRegisterWriteTN (GMMx5F50_TYPE, GMMx5F50_ADDRESS, &GMMx5F50.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntegratedEnumerateAudioConnectors Exit\n");
  return Status;
}

