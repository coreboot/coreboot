/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe utility. Various supporting functions.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 38931 $   @e \$Date: 2010-10-01 15:50:05 -0700 (Fri, 01 Oct 2010) $
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
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  GNB_MODULE_DEFINITIONS (GnbPcieInitLibV1)
#include  GNB_MODULE_DEFINITIONS (GnbPcieConfig)
#include  "PcieMiscLib.h"
#include  "GnbRegistersON.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_PCIE_PCIEMISCLIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
typedef struct {
  UINT32               Flags;
  PCIE_LINK_SPEED_CAP  LinkSpeedCapability;
} PCIE_GLOBAL_GEN_CAP_WORKSPACE;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/
/**
 * Training state handling
 *
 *
 *
 * @param[in]       Engine              Pointer to engine config descriptor
 * @param[in, out]  Buffer              Indicate if engine in non final state
 * @param[in]       Pcie                Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PcieUtilGlobalGenCapabilityCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIE_GLOBAL_GEN_CAP_WORKSPACE *GlobalGenCapability;
  PCIE_LINK_SPEED_CAP           LinkSpeedCapability;
  PCIE_HOTPLUG_TYPE             HotPlugType;
  UINT32                        Flags;

  Flags = PCIE_GLOBAL_GEN_CAP_ALL_PORTS;
  GlobalGenCapability = (PCIE_GLOBAL_GEN_CAP_WORKSPACE*) Buffer;
  LinkSpeedCapability = PcieGen1;
  if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
    Flags |= PCIE_GLOBAL_GEN_CAP_TRAINED_PORTS;
  }
  HotPlugType = Engine->Type.Port.PortData.LinkHotplug;
  if ((HotPlugType == HotplugBasic) || (HotPlugType == HotplugServer) || (HotPlugType == HotplugEnhanced)) {
    Flags |= PCIE_GLOBAL_GEN_CAP_HOTPLUG_PORTS;
  }
  if ((GlobalGenCapability->Flags & Flags) != 0) {
    ASSERT ((GlobalGenCapability->Flags & (PCIE_PORT_GEN_CAP_MAX | PCIE_PORT_GEN_CAP_BOOT)) != 0);
    LinkSpeedCapability = PcieFmGetLinkSpeedCap (GlobalGenCapability->Flags, Engine, Pcie);
    if (GlobalGenCapability->LinkSpeedCapability < LinkSpeedCapability) {
      GlobalGenCapability->LinkSpeedCapability = LinkSpeedCapability;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Determine global GEN capability
 *
 *
 * @param[in]  Flags           global GEN capability flags
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */
PCIE_LINK_SPEED_CAP
PcieUtilGlobalGenCapability (
  IN      UINT32                         Flags,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  )
{
  PCIE_LINK_SPEED_CAP             GlobalCapability;
  PCIE_GLOBAL_GEN_CAP_WORKSPACE   GlobalGenCap;

  GlobalGenCap.LinkSpeedCapability = PcieGen1;
  GlobalGenCap.Flags = Flags;
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PcieUtilGlobalGenCapabilityCallback,
    &GlobalGenCap,
    Pcie
    );

  GlobalCapability = GlobalGenCap.LinkSpeedCapability;

  return GlobalCapability;
}

