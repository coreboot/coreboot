/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe port initialization service procedure
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
#include  "GnbPcieConfig.h"
#include  "GnbSbLib.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbPcieInitLibV5.h"
#include  "GnbRegistersCommonV2.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV5_PCIEPORTSERVICESV5_FILECODE
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


/*----------------------------------------------------------------------------------------*/
/**
 * Init hotplug port
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

VOID
PcieLinkInitHotplugV5 (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32  Value;
  PcieLinkInitHotplug (Engine, Pcie);
  if (Engine->Type.Port.PortData.LinkHotplug == HotplugEnhanced) {
    Value = 1;
  } else {
    Value = 0;
  }
  PciePortRegisterWriteField (
    Engine,
    0x10,
    3,
    1,
    Value,
    TRUE,
    Pcie
    );
}



/*----------------------------------------------------------------------------------------*/
/**
 * Set slot power limit
 *
 *
 *
 * @param[in]  Engine              Pointer to engine configuration
 * @param[in]  Pcie                Pointer to PCIe configuration
 */


VOID
PcieEnableSlotPowerLimitV5 (
  IN      PCIe_ENGINE_CONFIG     *Engine,
  IN      PCIe_PLATFORM_CONFIG   *Pcie
  )
{
  PCIe_SILICON_CONFIG   *Silicon;
  if (PcieLibIsEngineAllocated (Engine) && Engine->Type.Port.PortData.PortPresent != PortDisabled && !PcieConfigIsSbPcieEngine (Engine)) {
    IDS_HDT_CONSOLE (PCIE_MISC, "   Enable Slot Power Limit for Port % d\n", Engine->Type.Port.Address.Address.Device);
    Silicon = PcieConfigGetParentSilicon (Engine);
    GnbLibPciIndirectRMW (
      Silicon->Address.AddressValue | D0F0xC8_ADDRESS,
      D0F0xCC_x01_ADDRESS | ((Engine->Type.Port.PortData.DeviceNumber << 3 | Engine->Type.Port.PortData.FunctionNumber)  << D0F0xC8_NB_DEV_IND_SEL_OFFSET),
      AccessS3SaveWidth32,
      0xffffffff,
      1 << D0F0xCC_x01_SetPowEn_OFFSET,
      GnbLibGetHeader (Pcie)
    );
  }
}
