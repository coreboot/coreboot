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
 * @e \$Revision: 40151 $   @e \$Date: 2010-10-20 06:38:17 +0800 (Wed, 20 Oct 2010) $
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
#include  "GnbGfx.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  "GfxStrapsInit.h"
#include  "GfxConfigData.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_GFX_GFXCONFIGDATA_FILECODE
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
