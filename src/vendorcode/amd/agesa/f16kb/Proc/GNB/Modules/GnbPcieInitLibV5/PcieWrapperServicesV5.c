/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe topology initialization service procedures.
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
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbPcieInitLibV5.h"
#include  "GnbRegistersCommonV2.h"
#include  "GnbSmuInitLibV7.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV5_PCIEWRAPPERSERVICESV5_FILECODE
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


UINT8 LaneMuxSelectorArrayV5[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };




/*----------------------------------------------------------------------------------------*/
/**
 * Check if lane topology supports Gen3
 *
 *  Check if more that one link mapped to single PIF slice
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 */

BOOLEAN
PcieTopologyIsGen3SupportedV5 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper
  )
{
  UINT8                 LaneNibbleArray [4];
  UINT32                LaneBitmap;
  UINT8                 Nibble;
  UINT8                 NibbleBitmap;
  PCIe_ENGINE_CONFIG    *Engine;


  LibAmdMemFill (&LaneNibbleArray[0], 0x00, sizeof (LaneNibbleArray), PcieConfigGetStdHeader (Wrapper));
  Engine = PcieConfigGetChildEngine (Wrapper);
  while (Engine != NULL) {
    LaneBitmap = PcieUtilGetEngineLaneBitMap (LANE_TYPE_PCIE_PHY_NATIVE, 0, Engine);
    for (Nibble = 0; Nibble < 4; Nibble++) {
      NibbleBitmap = (0xF << (Nibble * 4));
      if ((LaneBitmap & NibbleBitmap) != 0) {
        if (++LaneNibbleArray [Nibble] > 1) {
          return FALSE;
        }
      }
    }
    Engine = PcieLibGetNextDescriptor (Engine);
  }
  return TRUE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Execute/clean up reconfiguration
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PcieTopologyExecuteReconfigV5 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  D0F0xE4_WRAP_8062_STRUCT  D0F0xE4_WRAP_8062;
  PCIe_SILICON_CONFIG       *Silicon;
  DEV_OBJECT                DevObject;

  if (PcieLibIsPcieWrapper (Wrapper)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "PcieTopologyExecuteReconfigV5 Enter\n");

    D0F0xE4_WRAP_8062.Value = PcieRegisterRead (
                                Wrapper,
                                WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8062_ADDRESS),
                                Pcie
                                );
    D0F0xE4_WRAP_8062.Field.ReconfigureEn = 0x1;
    D0F0xE4_WRAP_8062.Field.ResetPeriod = 0x2;
    PcieRegisterWrite (
      Wrapper,
      WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8062_ADDRESS),
      D0F0xE4_WRAP_8062.Value,
      FALSE,
      Pcie
      );

    Silicon = PcieConfigGetParentSilicon (Wrapper);
    DevObject.StdHeader = GnbLibGetHeader (Pcie);
    DevObject.GnbHandle = GnbGetHandle (GnbLibGetHeader (Pcie));
    DevObject.DevPciAddress.AddressValue = Silicon->Address.AddressValue;
    GnbSmuServiceRequestV7 (
      &DevObject,
      25,
      0,
      0
      );

    D0F0xE4_WRAP_8062.Field.ConfigXferMode = 0x1;
    D0F0xE4_WRAP_8062.Field.ReconfigureEn = 0x0;
    PcieRegisterWrite (
      Wrapper,
      WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8062_ADDRESS),
      D0F0xE4_WRAP_8062.Value,
      FALSE,
      Pcie
      );

    IDS_HDT_CONSOLE (GNB_TRACE, "PcieTopologyExecuteReconfigV5 Exit\n");
  }
}
