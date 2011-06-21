/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe PIF initialization routine
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
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  GNB_MODULE_DEFINITIONS (GnbPcieInitLibV1)
#include  "GnbRegistersON.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV1_PCIEPHYSERVICES_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define MAX_NUM_PHYs          2
#define MAX_NUM_LANE_PER_PHY  8

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/
/**
 * PHY lane ganging
 *
 *
 *
 * @param[out] Wrapper             Pointer to internal configuration data area
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PciePhyApplyGanging (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_ENGINE_CONFIG  *EngineList;
  UINT8               GangMatrix [MAX_NUM_PHYs][MAX_NUM_LANE_PER_PHY];
  UINT8               MasterMatrix [MAX_NUM_PHYs][MAX_NUM_LANE_PER_PHY];
  UINT16              LoPhylane;
  UINT16              HiPhylane;
  UINT8               Phy;
  UINT16              Lane;
  UINT16              PhyLinkWidth;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePhyApplyGanging Enter\n");
  LibAmdMemFill (GangMatrix, 0, sizeof (GangMatrix), GnbLibGetHeader (Pcie));
  LibAmdMemFill (MasterMatrix, 0, sizeof (MasterMatrix), GnbLibGetHeader (Pcie));
  EngineList = PcieWrapperGetEngineList (Wrapper);
  while (EngineList != NULL) {
    if (PcieLibIsEngineAllocated (EngineList)) {
      HiPhylane = PcieUtilGetHiPhyLane (EngineList) - Wrapper->StartPhyLane;
      LoPhylane = PcieUtilGetLoPhyLane (EngineList) - Wrapper->StartPhyLane;
      PhyLinkWidth = HiPhylane - LoPhylane + 1;

      if (PhyLinkWidth >= 8) {
        for (Lane = LoPhylane; Lane <= HiPhylane; Lane++) {
          ((UINT8 *) GangMatrix)[Lane] = 1;
        }
      } else {
        if (PhyLinkWidth > 0 && PhyLinkWidth < 4) {
          for (Lane = (LoPhylane / 4) * 4; Lane < (((LoPhylane / 4) * 4) + 4) ; Lane++) {
            ((UINT8 *) MasterMatrix)[Lane] = 1;
          }
        }
      }
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
  for (Phy = 0; Phy < Wrapper->NumberOfPIFs; Phy++) {
    for (Lane = 0; Lane < MAX_NUM_LANE_PER_PHY; Lane++) {
      D0F0xE4_PHY_6005_STRUCT  D0F0xE4_PHY_6005;
      D0F0xE4_PHY_6005.Value = PcieRegisterRead (
                                 Wrapper,
                                 PHY_SPACE (Wrapper->WrapId, Phy, D0F0xE4_PHY_6005_ADDRESS + Lane * 0x80),
                                 Pcie
                                 );
      D0F0xE4_PHY_6005.Field.GangedModeEn = GangMatrix [Phy][Lane];
      D0F0xE4_PHY_6005.Field.IsOwnMstr = MasterMatrix [Phy][Lane];
      PcieRegisterWrite (
        Wrapper,
        PHY_SPACE (Wrapper->WrapId, Phy, D0F0xE4_PHY_6005_ADDRESS + Lane * 0x80),
        D0F0xE4_PHY_6005.Value,
        FALSE,
        Pcie
        );
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePhyApplyGanging Exit\n");
}


/*----------------------------------------------------------------------------------------*/
/**
 * Point "virtual" PLL clock picker away from PCIe
 *
 *
 *
 * @param[in]  Wrapper             Pointer to internal configuration data area
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PciePhyAvertClockPickers (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32  DdiLanes;
  UINT8   Nibble;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePhyAvertClockPickers Enter\n");
  DdiLanes = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_DDI_ALLOCATED, 0, Wrapper, Pcie);
  for (Nibble = 0; Nibble < 4; Nibble++) {
    if (DdiLanes & (0xf << (Nibble * 4))) {
      PcieRegisterRMW (
        Wrapper,
        PHY_SPACE (Wrapper->WrapId, (Nibble >> 1), D0F0xE4_PHY_0009_ADDRESS + (Nibble & 0x1)),
        D0F0xE4_PHY_0009_PCIePllSel_MASK,
        0x0 << D0F0xE4_PHY_0009_PCIePllSel_OFFSET,
        FALSE,
        Pcie
        );
      PcieRegisterRMW (
        Wrapper,
        PHY_SPACE (Wrapper->WrapId, (Nibble >> 1), D0F0xE4_PHY_000B_ADDRESS + (Nibble & 0x1)),
        D0F0xE4_PHY_000B_MargPktSbiEn_MASK | D0F0xE4_PHY_000B_PcieModeSbiEn_MASK,
        (0x0 << D0F0xE4_PHY_000B_MargPktSbiEn_OFFSET) | (0x0 << D0F0xE4_PHY_000B_PcieModeSbiEn_OFFSET),
        FALSE,
        Pcie
        );
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePhyAvertClockPickers Exit\n");
}
