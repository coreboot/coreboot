/* $NoKeywords:$ */
/**
 * @file
 *
 * Graphics controller BIF straps control services.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 51087 $   @e \$Date: 2011-04-19 07:38:57 +0800 (Tue, 19 Apr 2011) $
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
//#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbGfx.h"
#include  "GnbCommonLib.h"
#include  "GnbNbInitLibV1.h"
#include  "GfxStrapsInit.h"
#include  "GfxLib.h"
#include  "GfxRegisterAcc.h"
#include  "NbSmuLib.h"
#include  "OptionGnb.h"
#include  "GnbRegistersLN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_GFX_GFXSTRAPSINIT_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern BUILD_OPT_CFG UserOptions;
extern GNB_BUILD_OPTIONS GnbBuildOptions;

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
 * Initialize GFX straps.
 *
 *
 * @param[in] Gfx             Pointer to global GFX configuration
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
GfxStrapsInit (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  D0F0x64_x1C_STRUCT  D0F0x64_x1C;
  D0F0x64_x1D_STRUCT  D0F0x64_x1D;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxStrapsInit Enter\n");

  GnbLibPciIndirectRead (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1C_ADDRESS | IOC_WRITE_ENABLE,
    AccessWidth32,
    &D0F0x64_x1C.Value,
    GnbLibGetHeader (Gfx)
    );

  GnbLibPciIndirectRead (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1D_ADDRESS | IOC_WRITE_ENABLE,
    AccessWidth32,
    &D0F0x64_x1D.Value,
    GnbLibGetHeader (Gfx)
    );

  D0F0x64_x1C.Field.AudioNonlegacyDeviceTypeEn = 0x0;
  D0F0x64_x1C.Field.F0NonlegacyDeviceTypeEn = 0x0;

  if (Gfx->GfxControllerMode == GfxControllerLegacyBridgeMode) {
    D0F0x64_x1D.Field.IntGfxAsPcieEn = 0x0;
    D0F0x64_x1C.Field.RcieEn = 0x0;
    D0F0x64_x1C.Field.PcieDis = 0x1;
  } else {
    D0F0x64_x1D.Field.IntGfxAsPcieEn = 0x1;
    D0F0x64_x1C.Field.RcieEn = 0x1;
    D0F0x64_x1C.Field.PcieDis = 0x0;
    //LN/ON A0 (MSI)
    GnbLibPciRMW (MAKE_SBDFO (0, 0, 1, 0, 0x4), AccessS3SaveWidth32, 0xffffffff, BIT2, GnbLibGetHeader (Gfx));
  }
  if (Gfx->ForceGfxMode == GfxEnableForceSecondary) {
    D0F0x64_x1D.Field.VgaEn = 0x0;
  } else {
    D0F0x64_x1D.Field.VgaEn = 0x1;
  }
  D0F0x64_x1C.Field.AudioEn = Gfx->GnbHdAudio;
  D0F0x64_x1C.Field.F0En = 0x1;
  D0F0x64_x1C.Field.RegApSize = 0x1;

  if (Gfx->UmaInfo.UmaSize > 128 * 0x100000) {
    D0F0x64_x1C.Field.MemApSize = 0x1;
  } else if (Gfx->UmaInfo.UmaSize > 64 * 0x100000) {
    D0F0x64_x1C.Field.MemApSize = 0x0;
  } else if (Gfx->UmaInfo.UmaSize > 32 * 0x100000) {
    D0F0x64_x1C.Field.MemApSize = 0x2;
  } else {
    D0F0x64_x1C.Field.MemApSize = 0x3;
  }
  GnbLibPciIndirectWrite (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1D_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &D0F0x64_x1D.Value,
    GnbLibGetHeader (Gfx)
    );

  GnbLibPciIndirectWrite (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1C_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &D0F0x64_x1C.Value,
    GnbLibGetHeader (Gfx)
    );

  D0F0x64_x1C.Field.WriteDis = 0x1;

  GnbLibPciIndirectWrite (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1C_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &D0F0x64_x1C.Value,
    GnbLibGetHeader (Gfx)
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxStrapsInit Exit\n");
  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Disable integrated GFX controller
 *
 *
 * @param[in] StdHeader       Standard configuration header
 */

VOID
GfxDisableController (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  FCRxFF30_0AE6_STRUCT       FCRxFF30_0AE6;
  D18F6x90_STRUCT            D18F6x90;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxDisableController Enter\n");
  GnbLibPciRMW (
    GNB_SBDFO | D0F0x7C_ADDRESS,
    AccessS3SaveWidth32,
    0xffffffff,
    1 << D0F0x7C_ForceIntGFXDisable_OFFSET,
    StdHeader
    );

  // With iGPU is disabled, Program D18F6x90[NbPs1GnbSlowIgn]=1
  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 6, D18F6x90_ADDRESS),
    AccessWidth32,
    &D18F6x90.Value,
    StdHeader
    );
  D18F6x90.Field.NbPs1GnbSlowIgn = 0x1;
  GnbLibPciWrite (
    MAKE_SBDFO ( 0, 0, 0x18, 6, D18F6x90_ADDRESS),
    AccessWidth32,
    &D18F6x90.Value,
    StdHeader
    );

  // With iGPU is disabled, Enable stutter without gmc power gating.
  NbSmuSrbmRegisterRead (FCRxFF30_0AE6_ADDRESS, &FCRxFF30_0AE6.Value, StdHeader);
  FCRxFF30_0AE6.Field.StctrlStutterEn = 0x1;
  NbSmuSrbmRegisterWrite (FCRxFF30_0AE6_ADDRESS, &FCRxFF30_0AE6.Value, TRUE, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxDisableController Exit\n");
}



/*----------------------------------------------------------------------------------------*/
/**
 * Request GFX boot up voltage
 *
 *
 * @param[in] Gfx             Pointer to global GFX configuration
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
GfxSetBootUpVoltage (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  GMMx770_STRUCT  GMMx770;
  GMMx774_STRUCT  GMMx774;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxSetBootUpVoltage Enter\n");

  GMMx770.Value = GmmRegisterRead (GMMx770_ADDRESS, Gfx);
  GMMx770.Field.VoltageChangeEn = 1;
  GmmRegisterWrite (GMMx770_ADDRESS, GMMx770.Value, TRUE, Gfx);
  GMMx770.Field.VoltageLevel = GnbLocateHighestVidIndex (GnbLibGetHeader (Gfx));
  GMMx770.Field.VoltageChangeReq = !GMMx770.Field.VoltageChangeReq;
  GmmRegisterWrite (GMMx770_ADDRESS, GMMx770.Value, TRUE, Gfx);
  do {
    GMMx774.Value = GmmRegisterRead (GMMx774_ADDRESS, Gfx);
  } while (GMMx774.Field.VoltageChangeAck != GMMx770.Field.VoltageChangeReq);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxSetBootUpVoltage Exit\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set idle voltage mode for GFX
 *
 *
 * @param[in] Gfx             Pointer to global GFX configuration
 */

VOID
GfxSetIdleVoltageMode (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
}
