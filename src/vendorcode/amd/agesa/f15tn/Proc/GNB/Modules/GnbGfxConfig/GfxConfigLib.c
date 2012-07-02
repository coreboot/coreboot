/* $NoKeywords:$ */
/**
 * @file
 *
 * Initialize GFX configuration data structure.
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
#include  "amdlib.h"
#include  "Ids.h"
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbGfx.h"
#include  "GfxConfigLib.h"
#include  "GnbCommonLib.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBGFXCONFIG_GFXCONFIGLIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern BUILD_OPT_CFG      UserOptions;
extern GNB_BUILD_OPTIONS  GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Enable GMM Access
 *
 *
 *
 * @param[in,out]   Gfx            Pointer to GFX configuration
 * @retval          AGESA_STATUS
 */

AGESA_STATUS
GfxEnableGmmAccess (
  IN OUT   GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINT32      Value;

  if (!GnbLibPciIsDevicePresent (Gfx->GfxPciAddress.AddressValue, GnbLibGetHeader (Gfx))) {
    IDS_ERROR_TRAP;
    return AGESA_ERROR;
  }

  // Check if base address for GMM allocated
  GnbLibPciRead (Gfx->GfxPciAddress.AddressValue | 0x18, AccessWidth32, &Gfx->GmmBase, GnbLibGetHeader (Gfx));
  if (Gfx->GmmBase == 0) {
    IDS_ERROR_TRAP;
    return AGESA_ERROR;
  }
  // Check if base address for FB allocated
  GnbLibPciRead (Gfx->GfxPciAddress.AddressValue | 0x10, AccessWidth32, &Value, GnbLibGetHeader (Gfx));
  if ((Value & 0xfffffff0) == 0) {
    IDS_ERROR_TRAP;
    return AGESA_ERROR;
  }
  //Push CPU MMIO pci config to S3 script
  GnbLibS3SaveConfigSpace (MAKE_SBDFO (0, 0, 0x18, 1, 0), 0xBC, 0x80, AccessS3SaveWidth32, GnbLibGetHeader (Gfx));
  // Turn on memory decoding on APC to enable access to GMM register space
  if (Gfx->GfxControllerMode == GfxControllerLegacyBridgeMode) {
    GnbLibPciRMW (MAKE_SBDFO (0, 0, 1, 0, 0x4), AccessWidth32, 0xffffffff, BIT1 | BIT2, GnbLibGetHeader (Gfx));
    //Push APC pci config to S3 script
    GnbLibS3SaveConfigSpace (MAKE_SBDFO (0, 0, 1, 0, 0), 0x2C, 0x18, AccessS3SaveWidth32, GnbLibGetHeader (Gfx));
    GnbLibS3SaveConfigSpace (MAKE_SBDFO (0, 0, 1, 0, 0), 0x4, 0x4, AccessS3SaveWidth16, GnbLibGetHeader (Gfx));
  }
  // Turn on memory decoding on GFX to enable access to GMM register space
  GnbLibPciRMW (Gfx->GfxPciAddress.AddressValue | 0x4, AccessWidth32, 0xffffffff, BIT1 | BIT2, GnbLibGetHeader (Gfx));
  //Push iGPU pci config to S3 script
  GnbLibS3SaveConfigSpace (Gfx->GfxPciAddress.AddressValue, 0x24, 0x10, AccessS3SaveWidth32, GnbLibGetHeader (Gfx));
  GnbLibS3SaveConfigSpace (Gfx->GfxPciAddress.AddressValue, 0x04, 0x04, AccessS3SaveWidth16, GnbLibGetHeader (Gfx));
  return AGESA_SUCCESS;
}



/*----------------------------------------------------------------------------------------*/
/**
 * Get UMA info
 *
 * UMA info stored on heap by memory module
 *
 * @param[out] UmaInfo      Pointer to UMA info structure
 * @param[in]  StdHeader    Standard configuration header
 */

VOID
GfxGetUmaInfo (
     OUT   UMA_INFO          *UmaInfo,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UMA_INFO *MemUmaInfo;

  MemUmaInfo = GnbLocateHeapBuffer (AMD_UMA_INFO_HANDLE, StdHeader);
  if (MemUmaInfo == NULL) {
    LibAmdMemFill (UmaInfo, 0x00, sizeof (UMA_INFO), StdHeader);
    UmaInfo->UmaMode = UMA_NONE;
  } else {
    LibAmdMemCopy (UmaInfo, MemUmaInfo, sizeof (UMA_INFO), StdHeader);
    if ((UmaInfo->UmaBase == 0) || (UmaInfo->UmaSize == 0)) {
      UmaInfo->UmaMode = UMA_NONE;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Locate UMA configuration data
 *
 *
 *
 * @param[in]     StdHeader     Standard configuration header
 * @param[in,out] Gfx           Pointer to GFX configuration
 * @retval        AGESA_STATUS  Data located
 * @retval        AGESA_FATA    Data not found
 */

AGESA_STATUS
GfxLocateConfigData (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
     OUT   GFX_PLATFORM_CONFIG   **Gfx
  )
{
  *Gfx = GnbLocateHeapBuffer (AMD_GFX_PLATFORM_CONFIG_HANDLE, StdHeader);
  if (*Gfx == NULL) {
    IDS_ERROR_TRAP;
    return  AGESA_FATAL;
  }
  (*Gfx)->StdHeader = /* (PVOID) */(UINT32) StdHeader;
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Debug dump
 *
 *
 *
 * @param[in]     Gfx           Pointer to GFX configuration
 */

VOID
GfxConfigDebugDump (
  IN       GFX_PLATFORM_CONFIG      *Gfx
  )
{
  IDS_HDT_CONSOLE (GFX_MISC, "<-------------- GFX Config Start ------------->\n");
  IDS_HDT_CONSOLE (GFX_MISC, "  HD Audio - %s\n", (Gfx->GnbHdAudio == 0) ? "Disabled" : "Enabled");
  IDS_HDT_CONSOLE (GFX_MISC, "  DynamicRefreshRate - 0x%x\n", Gfx->DynamicRefreshRate);
  IDS_HDT_CONSOLE (GFX_MISC, "  LcdBackLightControl - 0x%x\n", Gfx->LcdBackLightControl);
  IDS_HDT_CONSOLE (GFX_MISC, "  AbmSupport - %s\n", (Gfx->AbmSupport == 0) ? "Disabled" : "Enabled");
  IDS_HDT_CONSOLE (GFX_MISC, "  GmcClockGating - %s\n", (Gfx->GmcClockGating == 0) ? "Disabled" : "Enabled");
  IDS_HDT_CONSOLE (GFX_MISC, "  GmcPowerGating - %s\n",
    (Gfx->GmcPowerGating == GmcPowerGatingDisabled) ? "Disabled" : (
    (Gfx->GmcPowerGating == GmcPowerGatingStutterOnly) ? "GmcPowerGatingStutterOnly" : (
    (Gfx->GmcPowerGating == GmcPowerGatingWidthStutter) ? "GmcPowerGatingWidthStutter" : "Unknown"))
  );
  IDS_HDT_CONSOLE (GFX_MISC, "  UmaSteering - %s\n",
    (Gfx->UmaSteering == excel993 ) ? "excel993" : (
    (Gfx->UmaSteering == excel992 ) ? "excel992" : "Unknown")
    );
  IDS_HDT_CONSOLE (GFX_MISC, "  iGpuVgaMode - %s\n",
    (Gfx->iGpuVgaMode == iGpuVgaAdapter) ? "VGA" : (
    (Gfx->iGpuVgaMode == iGpuVgaNonAdapter) ? "Non VGA" : "Unknown")
    );
  IDS_HDT_CONSOLE (GFX_MISC, "  UmaMode - %s\n", (Gfx->UmaInfo.UmaMode == UMA_NONE) ? "No UMA" : "UMA");
  if (Gfx->UmaInfo.UmaMode != UMA_NONE) {
    IDS_HDT_CONSOLE (GFX_MISC, "  UmaBase - 0x%x\n", Gfx->UmaInfo.UmaBase);
    IDS_HDT_CONSOLE (GFX_MISC, "  UmaSize - 0x%x\n", Gfx->UmaInfo.UmaSize);
    IDS_HDT_CONSOLE (GFX_MISC, "  UmaAttributes - 0x%x\n", Gfx->UmaInfo.UmaAttributes);
  }
  IDS_HDT_CONSOLE (GFX_MISC, "<-------------- GFX Config End --------------->\n");

}
