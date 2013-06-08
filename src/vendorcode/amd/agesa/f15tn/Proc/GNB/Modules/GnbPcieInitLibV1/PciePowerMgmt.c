/* $NoKeywords:$ */
/**
 * @file
 *
 * Power saving features/services
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
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbRegistersLN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV1_PCIEPOWERMGMT_FILECODE

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
 * Power down unused lanes and plls
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */

VOID
PciePwrPowerDownUnusedLanes (
  IN       PCIe_WRAPPER_CONFIG    *Wrapper,
  IN       PCIe_PLATFORM_CONFIG   *Pcie
  )
{
  UINT32              UnusedLanes;
  UINT32              AllLanes;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrPowerDownUnusedLanes Enter\n");
  if (Wrapper->Features.PowerOffUnusedPlls != 0) {
    AllLanes = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_PHY_NATIVE_ALL, 0, Wrapper);
    UnusedLanes = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_PHY_NATIVE_ALL, LANE_TYPE_PCIE_PHY_NATIVE_ALLOC_ACTIVE, Wrapper);
    if (AllLanes != UnusedLanes) {
      //Some lanes end up beeing used. We should keep master PLL powered up
      UnusedLanes = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_PHY_NATIVE_ALL, LANE_TYPE_PCIE_PHY_NATIVE_ALLOC_ACTIVE | LANE_TYPE_PCIE_PHY_NATIVE_MASTER_PLL, Wrapper);
    }
    PciePifPllPowerDown (
      UnusedLanes,
      Wrapper,
      Pcie
      );
  }
  if (Wrapper->Features.PowerOffUnusedLanes != 0) {
    UnusedLanes = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_CORE_ALL, LANE_TYPE_PCIE_CORE_ALLOC_ACTIVE, Wrapper);
    PcieTopologyLaneControl (
      DisableLanes,
      UnusedLanes,
      Wrapper,
      Pcie
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrPowerDownUnusedLanes Exit\n");
}


/*----------------------------------------------------------------------------------------*/
/**
 * Lane bitmam to enable PLL power down in L1
 *
 *
 * @param[in]  PllPowerUpLatency  Pointer to wrapper config descriptor
 * @param[in]  Wrapper            Pointer to wrapper config descriptor
 * @param[in]  Pcie               Pointer to global PCIe configuration
 * @retval     Lane bitmap for which PLL can be powered down in L1
 */

UINT32
PcieLanesToPowerDownPllInL1 (
  IN       UINT8                  PllPowerUpLatency,
  IN       PCIe_WRAPPER_CONFIG    *Wrapper,
  IN       PCIe_PLATFORM_CONFIG   *Pcie
  )
{
  UINT8               LaneGroupExitLatency [4];
  UINT32              LaneBitmapForPllOffInL1;
  PCIe_ENGINE_CONFIG  *EngineList;
  UINTN               Index;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieLanesToPowerDownPllInL1 Enter\n");
  LaneBitmapForPllOffInL1 = 0;
  if (PcieUtilGetWrapperLaneBitMap (LANE_TYPE_PCIE_PHY_NATIVE_ALLOC_ACTIVE | LANE_TYPE_PCIE_PHY_NATIVE_HOTPLUG, 0, Wrapper) != 0) {
    if (Wrapper->Features.PllOffInL1 != 0) {
      LibAmdMemFill (&LaneGroupExitLatency[0], 0xFF, sizeof (LaneGroupExitLatency), GnbLibGetHeader (Pcie));
      EngineList = PcieConfigGetChildEngine (Wrapper);
      while (EngineList != NULL) {
        PCIe_ASPM_LATENCY_INFO  LinkLatencyInfo;
        UINT32                  ActiveLanesBitmap;
        UINT32                  HotplugLanesBitmap;
        if (EngineList->EngineData.EngineType == PciePortEngine) {
          LinkLatencyInfo.MaxL1ExitLatency = 0;
          LinkLatencyInfo.MaxL0sExitLatency = 0;
          ActiveLanesBitmap = PcieUtilGetEngineLaneBitMap (LANE_TYPE_PCIE_PHY_NATIVE_ALLOC_ACTIVE, 0, EngineList);
          HotplugLanesBitmap = PcieUtilGetEngineLaneBitMap (LANE_TYPE_PCIE_PHY_NATIVE_HOTPLUG, 0, EngineList);
          if (ActiveLanesBitmap != 0 && HotplugLanesBitmap == 0 && !PcieConfigIsSbPcieEngine (EngineList)) {
            PcieAspmGetMaxExitLatency (EngineList->Type.Port.Address, &LinkLatencyInfo, GnbLibGetHeader (Pcie));
          }
          if (HotplugLanesBitmap != 0 || PcieConfigIsSbPcieEngine (EngineList)) {
            LinkLatencyInfo.MaxL1ExitLatency = 0xff;
          }
          IDS_HDT_CONSOLE (GNB_TRACE, "  Engine %d Active Lanes 0x%x, Hotplug Lanes 0x%x\n", EngineList->Type.Port.NativeDevNumber, ActiveLanesBitmap, HotplugLanesBitmap);
          for (Index = 0; Index < 4; Index++) {
            if ((ActiveLanesBitmap  & (0xF << (Index * 4))) != 0) {
              if (LaneGroupExitLatency [Index] > LinkLatencyInfo.MaxL1ExitLatency) {
                IDS_HDT_CONSOLE (GNB_TRACE, "  Index %d Latency %d\n", Index, LinkLatencyInfo.MaxL1ExitLatency);
                LaneGroupExitLatency [Index] = LinkLatencyInfo.MaxL1ExitLatency;
              }
            }
          }
        }
        EngineList = PcieLibGetNextDescriptor (EngineList);
      }
      LaneBitmapForPllOffInL1 = 0;
      for (Index = 0; Index < 4; Index++) {
        IDS_HDT_CONSOLE (GNB_TRACE, "  Index %d Final Latency %d\n", Index, LaneGroupExitLatency[Index]);
        if (LaneGroupExitLatency[Index] > PllPowerUpLatency) {
          LaneBitmapForPllOffInL1 |= (0xF << (Index * 4));
        }
      }
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "  Lane bitmap %04x\n", LaneBitmapForPllOffInL1);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieLanesToPowerDownPllInL1 Exit\n");
  return LaneBitmapForPllOffInL1;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Auto-Power Down electrical Idle detector
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */

VOID
PciePwrAutoPowerDownElectricalIdleDetector (
  IN       PCIe_WRAPPER_CONFIG     *Wrapper,
  IN       PCIe_PLATFORM_CONFIG    *Pcie
  )
{
  UINT8   Pif;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrAutoPowerDownElectricalIdleDetector Enter\n");
  for (Pif = 0; Pif < Wrapper->NumberOfPIFs; Pif++) {
    PcieRegisterWriteField (
      Wrapper,
      PIF_SPACE (Wrapper->WrapId, Pif, D0F0xE4_PIF_0010_ADDRESS),
      D0F0xE4_PIF_0010_EiDetCycleMode_OFFSET,
      D0F0xE4_PIF_0010_EiDetCycleMode_WIDTH,
      0x0,
      TRUE,
      Pcie
      );

    PcieRegisterWriteField (
      Wrapper,
      PIF_SPACE (Wrapper->WrapId, Pif, 0x10 ),
      20 ,
      3 /*D0F0xE4_PIF_0010_EiCycleOffTime_WIDTH*/,
      0x2,
      TRUE,
      Pcie
      );

    PcieRegisterWriteField (
      Wrapper,
      PIF_SPACE (Wrapper->WrapId, Pif, D0F0xE4_PIF_0010_ADDRESS),
      D0F0xE4_PIF_0010_EiDetCycleMode_OFFSET,
      D0F0xE4_PIF_0010_EiDetCycleMode_WIDTH,
      0x1,
      TRUE,
      Pcie
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrAutoPowerDownElectricalIdleDetector Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Clock gating
 *
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */

VOID
PciePwrClockGating (
  IN       PCIe_WRAPPER_CONFIG    *Wrapper,
  IN       PCIe_PLATFORM_CONFIG   *Pcie
  )
{
  ex501_STRUCT   ex501 ;
  D0F0xE4_WRAP_8012_STRUCT  D0F0xE4_WRAP_8012;
  D0F0xE4_WRAP_8014_STRUCT  D0F0xE4_WRAP_8014;
  D0F0xE4_WRAP_8015_STRUCT  D0F0xE4_WRAP_8015;
  ex688_STRUCT   ex688 ;
  UINT8                     CoreId;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrClockGating Enter\n");
  D0F0xE4_WRAP_8014.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8014_ADDRESS),
                              Pcie
                              );
  D0F0xE4_WRAP_8015.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8015_ADDRESS),
                              Pcie
                              );

  D0F0xE4_WRAP_8012.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8012_ADDRESS),
                              Pcie
                              );

  ex501.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, 0x8011 ),
                              Pcie
                              );

  if (Wrapper->Features.ClkGating == 0x1) {
    D0F0xE4_WRAP_8014.Field.TxclkPermGateEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.TxclkPrbsGateEnable = 0x1;

    D0F0xE4_WRAP_8014.Field.PcieGatePifA1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifB1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifC1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifD1xEnable = 0x1;

    D0F0xE4_WRAP_8014.Field.PcieGatePifA2p5xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifB2p5xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifC2p5xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifD2p5xEnable = 0x1;


    ex501.Field.TxclkDynGateEnable = 0x1;
    ex501.Field.TxclkRegsGateEnable = 0x1;
    ex501.Field.TxclkLcntGateEnable = 0x1;
    ex501.Field.RcvrDetClkEnable = 0x1;
    ex501.Field.TxclkPermGateEven = 0x1;
    ex501.Field.TxclkDynGateLatency = 0x3f;
    ex501.Field.TxclkRegsGateLatency = 0x3f;
    ex501.Field.TxclkPermGateLatency = 0x3f;

    D0F0xE4_WRAP_8012.Field.Pif2p5xIdleResumeLatency = 0x7;
    D0F0xE4_WRAP_8012.Field.Pif2p5xIdleGateEnable = 0x1;
    D0F0xE4_WRAP_8012.Field.Pif2p5xIdleGateLatency = 0x1;
    D0F0xE4_WRAP_8012.Field.Pif1xIdleResumeLatency = 0x7;
    D0F0xE4_WRAP_8012.Field.Pif1xIdleGateEnable = 0x1;
    D0F0xE4_WRAP_8012.Field.Pif1xIdleGateLatency = 0x1;

    D0F0xE4_WRAP_8015.Field.RefclkBphyGateEnable = 0x1;
    D0F0xE4_WRAP_8015.Field.RefclkBphyGateLatency = 0x0;
    D0F0xE4_WRAP_8015.Field.RefclkRegsGateEnable = 0x1;
    D0F0xE4_WRAP_8015.Field.RefclkRegsGateLatency = 0x3f;

    D0F0xE4_WRAP_8014.Field.DdiGateDigAEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGateDigBEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifA1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifB1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifC1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifD1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifA2p5xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifB2p5xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifC2p5xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.DdiGatePifD2p5xEnable = 0x1;
  }
  if (Wrapper->Features.TxclkGatingPllPowerDown == 0x1) {
    D0F0xE4_WRAP_8014.Field.TxclkPermGateOnlyWhenPllPwrDn = 0x1;
  }
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8014_ADDRESS),
    D0F0xE4_WRAP_8014.Value,
    TRUE,
    Pcie
    );
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8015_ADDRESS),
    D0F0xE4_WRAP_8015.Value,
    TRUE,
    Pcie
    );
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8012_ADDRESS),
    D0F0xE4_WRAP_8012.Value,
    TRUE,
    Pcie
    );
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, 0x8011 ),
    ex501.Value,
    TRUE,
    Pcie
    );
  for (CoreId = Wrapper->StartPcieCoreId; CoreId <= Wrapper->EndPcieCoreId; CoreId++) {
    PcieRegisterWriteField (
      Wrapper,
      CORE_SPACE (CoreId, 0x11 ),
      0 ,
      4 ,
      0xf,
      TRUE,
      Pcie
      );
  }
  if (Wrapper->Features.LclkGating == 0x1) {
    ex688.Value = PcieRegisterRead (
                                Wrapper,
                                WRAP_SPACE (Wrapper->WrapId, 0x8016 ),
                                Pcie
                                );
    ex688.Field.LclkDynGateEnable = 0x1;
    ex688.Field.LclkGateFree = 0x1;
    PcieRegisterWrite (
      Wrapper,
      WRAP_SPACE (Wrapper->WrapId, 0x8016 ),
      ex688.Value,
      TRUE,
      Pcie
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrClockGating Exit\n");
}


