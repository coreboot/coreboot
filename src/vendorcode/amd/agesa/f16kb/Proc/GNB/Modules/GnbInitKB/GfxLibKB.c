/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific GFX library
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 85947 $   @e \$Date: 2013-01-14 17:25:21 -0600 (Mon, 14 Jan 2013) $
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
#include  "amdlib.h"
#include  "Ids.h"
#include  "S3SaveState.h"
#include  "Gnb.h"
#include  "GnbPcieConfig.h"
#include  "GnbGfx.h"
#include  "GfxLibKB.h"
#include  "GnbCommonLib.h"
#include  "GnbRegisterAccKB.h"
#include  "GnbRegistersKB.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_GFXLIBKB_FILECODE
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
VOID
GfxDisableControllerKB (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

UINT32
GfxCalculateClockKB (
  IN       UINT8                       Did,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

BOOLEAN
GfxIsVbiosPostedKB (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Disable GFX controller
 *
 *
 *
 * @param[in] StdHeader  Standard configuration header
 */

VOID
GfxDisableControllerKB (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  GnbLibPciRMW (
    MAKE_SBDFO (0, 0, 0, 0,D0F0x7C_ADDRESS),
    AccessS3SaveWidth32,
    0xffffffff,
    1 << D0F0x7C_ForceIntGfxDisable_OFFSET,
    StdHeader
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get system PLL COF
 *
 *
 *
 * @param[in] StdHeader  Standard configuration header
 * @retval    System PLL COF
 */
UINT32
GfxLibGetSystemPllCofKB (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32  v0;
  GnbRegisterReadKB (GnbGetHandle (StdHeader), 0x4,  0xC0500000, &v0, 0, StdHeader);
  return 100 * (((v0 >> 1) & 0x3F) + 0x10);
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

UINT32
GfxCalculateClockKB (
  IN       UINT8                       Did,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  UINT32  Divider;
  UINT32  SystemPllCof;
  SystemPllCof = GfxLibGetSystemPllCofKB (StdHeader) * 100;
  if (Did >= 8 && Did <= 0x3F) {
    Divider = Did * 25;
  } else if (Did > 0x3F && Did <= 0x5F) {
    Divider = (Did - 64) * 50 + 1600;
  } else if (Did > 0x5F && Did <= 0x7E) {
    Divider = (Did - 96) * 100 + 3200;
  } else if (Did == 0x7f) {
    Divider = 128 * 100;
  } else {
    ASSERT (FALSE);
    return 200 * 100;
  }
  ASSERT (Divider != 0);
  return (((SystemPllCof * 100) + (Divider - 1)) / Divider);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check if Video BIOS has posted or not
 *
 *
 * @param[in] Gfx             Pointer to global GFX configuration
 */

BOOLEAN
GfxIsVbiosPostedKB (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINT32  D0F0xBC_xC0200000;
  GnbRegisterReadKB (GnbGetHandle (GnbLibGetHeader (Gfx)), 0x4,  0xC0200000, &D0F0xBC_xC0200000, 0, GnbLibGetHeader (Gfx));

  return (((D0F0xBC_xC0200000 >> 16) & 1) == 0) ? TRUE : FALSE;
}


