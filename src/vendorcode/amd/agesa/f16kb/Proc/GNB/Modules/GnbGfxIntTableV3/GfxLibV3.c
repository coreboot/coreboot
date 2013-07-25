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
#include  "GfxLibV3.h"
#include  "GnbSmuInitLibV7.h"
#include  "GnbCommonLib.h"
#include  "GnbRegistersCommonV2.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBGFXINTTABLEV3_GFXLIBV3_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
CONST UINT16 GfxMemClockFrequencyDefinitionTableV3 [][8] = {
{0,   0,  0,  0,  333,  0,  400,  0},
{0,  0,  533,  0,  0,  0,  667,  0},
{0,  0,  800,  0,  0,  0,  933,  0},
{0, 1050, 1066, 0, 0,    0, 0,    0}
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Extract DRAM frequency
 *
 *
 *
 * @param[in] Encoding   Memory Clock Frequency Value Definition
 * @param[in] StdHeader  Standard configuration header
 * @retval    Dram frequency Mhz
 */
UINT32
GfxLibExtractDramFrequencyV3 (
  IN       UINT8                  Encoding,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  if (Encoding >= (sizeof (GfxMemClockFrequencyDefinitionTableV3) / sizeof (UINT16))) {
    ASSERT (FALSE);
    return 0;
  }
  return GfxMemClockFrequencyDefinitionTableV3[Encoding / 8][Encoding % 8];
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable GMM Access for gBIF BAR Arrangement
 *
 *
 *
 * @param[in,out]   Gfx            Pointer to GFX configuration
 * @retval          AGESA_STATUS
 */

AGESA_STATUS
GfxEnableGmmAccessV3 (
  IN OUT   GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINT32      Value;

  if (!GnbLibPciIsDevicePresent (Gfx->GfxPciAddress.AddressValue, GnbLibGetHeader (Gfx))) {
    IDS_ERROR_TRAP;
    return AGESA_ERROR;
  }

  // Check if base address for GMM allocated by reading D1F0x24 Graphics Memory Mapped Base Address
  Gfx->GmmBase = 0;
  GnbLibPciRead (Gfx->GfxPciAddress.AddressValue | 0x24, AccessWidth32, &Value, GnbLibGetHeader (Gfx));
  Gfx->GmmBase |= (Value & 0xfffffff0);
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
  // Turn on memory decoding on GFX to enable access to GMM register space
  GnbLibPciRMW (Gfx->GfxPciAddress.AddressValue | 0x4, AccessWidth32, 0xffffffff, BIT1 | BIT2, GnbLibGetHeader (Gfx));
  //Push iGPU pci config to S3 script
  GnbLibS3SaveConfigSpace (Gfx->GfxPciAddress.AddressValue, 0x24, 0x10, AccessS3SaveWidth32, GnbLibGetHeader (Gfx));
  GnbLibS3SaveConfigSpace (Gfx->GfxPciAddress.AddressValue, 0x04, 0x04, AccessS3SaveWidth16, GnbLibGetHeader (Gfx));
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Power Up/Down iGPU
 *
 *
 *
 * @param[in,out]   Gfx            Pointer to GFX configuration
 * @param[in,out]   PowerControl    Control power Up/Down iGPU, 0, power down iGPU, 1, power on iGPU
 * @retval          AGESA_STATUS
 */
AGESA_STATUS
GfxRequestGPUPowerV3 (
  IN OUT   GFX_PLATFORM_CONFIG   *Gfx,
  IN       UINT8                 PowerControl
  )
{
  GNB_HANDLE              *GnbHandle;
  DEV_OBJECT              DevObject;

  GnbHandle = GnbGetHandle (GnbLibGetHeader (Gfx));
  DevObject.DevPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
  DevObject.GnbHandle = GnbHandle;
  DevObject.StdHeader = GnbLibGetHeader (Gfx);


  return AGESA_SUCCESS;
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
GfxIntAudioEpEnumCallback (
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
 * Enumerate audio endpoint in all display connectors.
 *
 *
 *
 * @param[in]       Gfx             Gfx configuration info
 * @param[in, out]  AudioEPCount    Total Audio endpoint number
 * @retval          AGESA_STATUS
 */
AGESA_STATUS
GfxIntAudioEPEnumV3 (
  IN       GFX_PLATFORM_CONFIG    *Gfx,
  IN OUT   UINT8                  *AudioEPCount
  )
{
  UINT8                   NumAudioEp;
  AGESA_STATUS            Status;
  PCIe_PLATFORM_CONFIG    *Pcie;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntAudioEPEnumV3 Enter\n");

  NumAudioEp = 0;
  Status = PcieLocateConfigurationData (GnbLibGetHeader (Gfx), &Pcie);
  if ((Status == AGESA_SUCCESS) && (Gfx->GnbHdAudio != 0)) {
    PcieConfigRunProcForAllEngines (
      DESCRIPTOR_ALLOCATED | DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_VIRTUAL,
      GfxIntAudioEpEnumCallback,
      &NumAudioEp,
      Pcie
      );

    if (Gfx->GnbRemoteDisplaySupport) {
      NumAudioEp++;
    }
  }

  *AudioEPCount = NumAudioEp;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntAudioEPEnumV3 Exit\n");
  return Status;
}
