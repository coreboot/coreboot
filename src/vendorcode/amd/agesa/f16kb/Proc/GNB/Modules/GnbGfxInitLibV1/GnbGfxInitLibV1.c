/* $NoKeywords:$ */
/**
 * @file
 *
 *  Supporting services to collect discrete GFX card info
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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
#include  "Ids.h"
#include  "amdlib.h"
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbGfx.h"
#include  "GnbCommonLib.h"
#include  "GfxCardInfo.h"
#include  "GnbRegistersCommon.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBGFXINITLIBV1_GNBGFXINITLIBV1_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern BUILD_OPT_CFG UserOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */



/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN
GfxLibIsControllerPresent (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

VOID
GfxLibCopyMemToFb (
  IN     VOID                   *Source,
  IN     UINT32                  FbOffset,
  IN     UINT32                  Length,
  IN     GFX_PLATFORM_CONFIG    *Gfx
  );

VOID
GfxLibSetiGpuVgaMode (
  IN     GFX_PLATFORM_CONFIG    *Gfx
  );

AGESA_STATUS
GfxInitSsid (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

/*----------------------------------------------------------------------------------------*/
/**
 *
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval    TRUE            Gfx controller present and available
 */
BOOLEAN
GfxLibIsControllerPresent (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  return GnbLibPciIsDevicePresent (MAKE_SBDFO (0, 0, 1, 0, 0), StdHeader);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Init Gfx SSID Registers
 *
 *
 *
 * @param[in] Gfx           Pointer to global GFX configuration
 * @retval    AGESA_STATUS  Always succeeds
 */

AGESA_STATUS
GfxInitSsid (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  AGESA_STATUS    Status;
  UINT32          TempData;
  PCI_ADDR        IgpuAddress;
  PCI_ADDR        HdaudioAddress;

  Status = AGESA_SUCCESS;
  TempData = 0;

  IgpuAddress = Gfx->GfxPciAddress;
  HdaudioAddress = Gfx->GfxPciAddress;
  HdaudioAddress.Address.Function = 1;

  // Set SSID for internal GPU
  if (UserOptions.CfgGnbIGPUSSID != 0) {
    GnbLibPciRMW ((IgpuAddress.AddressValue | 0x4C), AccessS3SaveWidth32, 0, UserOptions.CfgGnbIGPUSSID, GnbLibGetHeader (Gfx));
  } else {
    GnbLibPciRead (IgpuAddress.AddressValue, AccessS3SaveWidth32, &TempData, GnbLibGetHeader (Gfx));
    GnbLibPciRMW ((IgpuAddress.AddressValue | 0x4C), AccessS3SaveWidth32, 0, TempData, GnbLibGetHeader (Gfx));
  }

  // Set SSID for internal HD Audio
  if (UserOptions.CfgGnbHDAudioSSID != 0) {
    GnbLibPciRMW ((HdaudioAddress.AddressValue | 0x4C), AccessS3SaveWidth32, 0, UserOptions.CfgGnbHDAudioSSID, GnbLibGetHeader (Gfx));
  } else {
    GnbLibPciRead (HdaudioAddress.AddressValue, AccessS3SaveWidth32, &TempData, GnbLibGetHeader (Gfx));
    GnbLibPciRMW ((HdaudioAddress.AddressValue | 0x4C), AccessS3SaveWidth32, 0, TempData, GnbLibGetHeader (Gfx));
  }

  return  Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Copy memory content to FB
 *
 *
 * @param[in] Source       Pointer to source
 * @param[in] FbOffset     FB offset
 * @param[in] Length       The length to copy
 * @param[in] Gfx          Pointer to global GFX configuration
 *
 */
VOID
GfxLibCopyMemToFb (
  IN     VOID                   *Source,
  IN     UINT32                  FbOffset,
  IN     UINT32                  Length,
  IN     GFX_PLATFORM_CONFIG    *Gfx
  )
{
  GMMx00_STRUCT GMMx00;
  GMMx04_STRUCT GMMx04;
  UINT32        Index;
  for (Index = 0; Index < Length; Index = Index + 4 ) {
    GMMx00.Value = 0x80000000 | (FbOffset + Index);
    GMMx04.Value = *(UINT32*) ((UINT8*)Source + Index);
    GnbLibMemWrite (Gfx->GmmBase + GMMx00_ADDRESS, AccessWidth32, &GMMx00.Value, GnbLibGetHeader (Gfx));
    GnbLibMemWrite (Gfx->GmmBase + GMMx04_ADDRESS, AccessWidth32, &GMMx04.Value, GnbLibGetHeader (Gfx));
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Set iGpu VGA mode
 *
 *
 * @param[in] Gfx          Pointer to global GFX configuration
 *
 */
VOID
GfxLibSetiGpuVgaMode (
  IN     GFX_PLATFORM_CONFIG    *Gfx
  )
{
  GnbLibPciIndirectRMW (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1D_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    (UINT32) ~D0F0x64_x1D_VgaEn_MASK,
    ((Gfx->iGpuVgaMode == iGpuVgaAdapter) ? 1 : 0) << D0F0x64_x1D_VgaEn_OFFSET,
    GnbLibGetHeader (Gfx)
    );
}
