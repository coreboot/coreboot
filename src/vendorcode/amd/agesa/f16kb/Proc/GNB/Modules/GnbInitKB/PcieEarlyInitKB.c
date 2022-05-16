/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe early post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 87701 $   @e \$Date: 2013-02-07 12:58:51 -0600 (Thu, 07 Feb 2013) $
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
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbUra.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieTrainingV2.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbPcieInitLibV4.h"
#include  "GnbPcieInitLibV5.h"
#include  "PcieLibKB.h"
#include  "PcieComplexDataKB.h"
#include  "GnbRegistersKB.h"
#include  "GnbRegisterAccKB.h"
#include  "OptionGnb.h"
#include  "GnbSmuInitLibV7.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_PCIEEARLYINITKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_BUILD_OPTIONS GnbBuildOptions;
extern BUILD_OPT_CFG UserOptions;
extern CONST PCIE_HOST_REGISTER_TABLE_HEADER ROMDATA PcieInitEarlyTableKB;
extern CONST PCIE_HOST_REGISTER_TABLE_HEADER ROMDATA CoreInitTableKB;
extern CONST PCIE_PORT_REGISTER_TABLE_HEADER ROMDATA PortInitEarlyTableKB;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
PcieEarlyInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Set port device/function mapping
 *
 *
 *
 * @param[in]  Descriptor          Silicon descriptor
 * @param[in]  Buffer              Pointer to buffer
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
STATIC AGESA_STATUS
PciePortMapInitCallbackKB (
  IN       PCIe_DESCRIPTOR_HEADER            *Descriptor,
  IN OUT   VOID                              *Buffer,
  IN       PCIe_PLATFORM_CONFIG              *Pcie
  )
{
  PcieSetPortPciAddressMapKB ((PCIe_SILICON_CONFIG *) Descriptor);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Static init for various registers.
 *
 *
 *
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
VOID
STATIC
PcieEarlyStaticInitKB (
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  UINTN Index;

  for (Index = 0; Index < PcieInitEarlyTableKB.Length; Index++) {
    GnbLibPciIndirectRMW (
      MAKE_SBDFO (0,0,0,0, D0F0xE0_ADDRESS),
      PcieInitEarlyTableKB.Table[Index].Reg,
      AccessS3SaveWidth32,
      (UINT32)~PcieInitEarlyTableKB.Table[Index].Mask,
      PcieInitEarlyTableKB.Table[Index].Data,
      GnbLibGetHeader (Pcie)
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init core registers.
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper configuration descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
VOID
STATIC
PcieEarlyCoreInitKB (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  UINT8   CoreId;
  UINTN   Index;
  if (PcieLibIsPcieWrapper (Wrapper)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyCoreInitKB Enter\n");
    for (CoreId = Wrapper->StartPcieCoreId; CoreId <= Wrapper->EndPcieCoreId; CoreId++) {
      for (Index = 0; Index < CoreInitTableKB.Length; Index++) {
        UINT32 Value;
        Value = PcieRegisterRead (
                  Wrapper,
                  CORE_SPACE (CoreId, CoreInitTableKB.Table[Index].Reg),
                  Pcie
                  );
        Value &= (~CoreInitTableKB.Table[Index].Mask);
        Value |= CoreInitTableKB.Table[Index].Data;
        PcieRegisterWrite (
          Wrapper,
          CORE_SPACE (CoreId, CoreInitTableKB.Table[Index].Reg),
          Value,
          FALSE,
          Pcie
          );
      }
    }
    IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyCoreInitKB Exit\n");
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set Pcie Phy Isolation
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper configuration descriptor
 * @param[in]  Pcie            Pointer to PCIe configuration data area
 */
VOID
STATIC
PciePhyIsolationKB (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32  ActiveLaneBitmap;
  UINT32  PhyRxIsoDis;
  D0F0xE4_WRAP_8013_STRUCT  D0F0xE4_WRAP_8013;
  UINT32  D0F0xE4_WRAP_8021;
  UINT32  D0F0xE4_WRAP_8022;
  UINT32  D0F0xE4_WRAP_8025;
  UINT32  D0F0xE4_WRAP_8026;

  IDS_HDT_CONSOLE (GNB_TRACE, "PciePhyIsolationKB Enter\n");

  // Apply lane mux
  D0F0xE4_WRAP_8021 = 0x07060504;
  D0F0xE4_WRAP_8022 = 0x03020100;
  D0F0xE4_WRAP_8025 = 0x07060504;
  D0F0xE4_WRAP_8026 = 0x03020100;

  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8021_ADDRESS),
    D0F0xE4_WRAP_8021,
    FALSE,
    Pcie
    );
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8022_ADDRESS),
    D0F0xE4_WRAP_8022,
    FALSE,
    Pcie
    );
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8025_ADDRESS),
    D0F0xE4_WRAP_8025,
    FALSE,
    Pcie
    );
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8026_ADDRESS),
    D0F0xE4_WRAP_8026,
    FALSE,
    Pcie
    );

  PhyRxIsoDis = GnbBuildOptions.CfgPciePhyIsolationEnable ? 0 : 3;
  ActiveLaneBitmap = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_PCIE_PHY_NATIVE, 0, Wrapper);
  if ((ActiveLaneBitmap & 0xF0) != 0) {
    PhyRxIsoDis = 3;
  }

  IDS_OPTION_HOOK (IDS_GNB_PCIE_PHY_ISOLATION, &PhyRxIsoDis, GnbLibGetHeader (Pcie));

  D0F0xE4_WRAP_8013.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8013_ADDRESS),
                              Pcie
                              );
  D0F0xE4_WRAP_8013.Field.PhyRxIsoDis = PhyRxIsoDis;
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8013_ADDRESS),
    D0F0xE4_WRAP_8013.Value,
    FALSE,
    Pcie
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "    ActiveLaneBitmap = 0x%x, PhyRxIsoDis = %d\n", ActiveLaneBitmap, PhyRxIsoDis);
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePhyIsolationKB Exit\n");
}

CONST UINT8 LaneMuxSelectorArrayKB[] = { 7, 6, 5, 4, 3, 2, 1, 0 };

/*----------------------------------------------------------------------------------------*/
/**
 * Locate mux array index
 *
 *
 *
 * @param[in, out]  LaneMuxSelectorArrayPtr     Pointer to mux selector array
 * @param[in]       LaneMuxValue                The value that match to array
 * @retval          Index                       Index successfully mapped
 */
STATIC UINT8
PcieTopologyLocateMuxIndexKB (
  IN OUT   UINT8                *LaneMuxSelectorArrayPtr,
  IN       UINT8                LaneMuxValue
  )
{
  UINT8  Index;
  for (Index = 0; Index < sizeof (LaneMuxSelectorArrayKB); Index++ ) {
    if (LaneMuxSelectorArrayPtr [Index] ==  LaneMuxValue) {
      return Index;
    }
  }
  return 0;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Apply lane mux
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */

STATIC VOID
PcieTopologyApplyLaneMuxKB (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_ENGINE_CONFIG  *EngineList;
  UINT32              Index;
  UINT8               RxLaneMuxSelectorArray [sizeof (LaneMuxSelectorArrayKB)];
  UINT8               TxLaneMuxSelectorArray [sizeof (LaneMuxSelectorArrayKB)];

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieTopologyApplyLaneMuxKB Enter\n");
  if (PcieLibIsPcieWrapper (Wrapper)) {
    LibAmdMemCopy (
      &TxLaneMuxSelectorArray[0],
      &LaneMuxSelectorArrayKB[0],
      sizeof (LaneMuxSelectorArrayKB),
      GnbLibGetHeader (Pcie)
      );
    LibAmdMemCopy (
      &RxLaneMuxSelectorArray[0],
      &LaneMuxSelectorArrayKB[0],
      sizeof (LaneMuxSelectorArrayKB),
      GnbLibGetHeader (Pcie)
      );
    EngineList = PcieConfigGetChildEngine (Wrapper);
    while (EngineList != NULL) {
      if (PcieLibIsPcieEngine (EngineList) && PcieLibIsEngineAllocated (EngineList)) {
        UINT32  CoreLaneBitmap;
        UINT32  PifLaneBitmap;
        UINT8   CurrentCoreLane;
        UINT8   CurrentPifLane;

        CoreLaneBitmap = PcieUtilGetEngineLaneBitMap (LANE_TYPE_PCIE_CORE_ALLOC, 0, EngineList);
        PifLaneBitmap = PcieUtilGetEngineLaneBitMap (LANE_TYPE_PCIE_PHY_NATIVE, 0, EngineList);

        while (CoreLaneBitmap != 0) {
          CurrentCoreLane = LibAmdBitScanForward (CoreLaneBitmap);
          CurrentPifLane = LibAmdBitScanForward (PifLaneBitmap);

          if (TxLaneMuxSelectorArray[CurrentPifLane] != CurrentCoreLane) {
            TxLaneMuxSelectorArray[PcieTopologyLocateMuxIndexKB (TxLaneMuxSelectorArray, CurrentCoreLane)] = TxLaneMuxSelectorArray[CurrentPifLane];
            TxLaneMuxSelectorArray[CurrentPifLane] = CurrentCoreLane;
          }

          if (RxLaneMuxSelectorArray[CurrentCoreLane] != CurrentPifLane) {
            RxLaneMuxSelectorArray[PcieTopologyLocateMuxIndexKB (RxLaneMuxSelectorArray, CurrentPifLane)] = RxLaneMuxSelectorArray[CurrentCoreLane];
            RxLaneMuxSelectorArray[CurrentCoreLane] = CurrentPifLane;
          }

          CoreLaneBitmap &= (~ (1 << CurrentCoreLane));
          PifLaneBitmap &= (~ (1 << CurrentPifLane));
        }
      }
      EngineList = PcieLibGetNextDescriptor (EngineList);
    }
    for (Index = 0; Index < 2; ++Index) {
      PcieRegisterWrite (
        Wrapper,
        WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8021_ADDRESS + Index),
        ((UINT32 *) TxLaneMuxSelectorArray) [Index],
        FALSE,
        Pcie
        );
      PcieRegisterWrite (
        Wrapper,
        WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8025_ADDRESS + Index),
        ((UINT32 *) RxLaneMuxSelectorArray) [Index],
        FALSE,
        Pcie
        );
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieTopologyApplyLaneMuxKB Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Execute/clean up reconfiguration
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
STATIC VOID
PcieTopologyExecuteReconfigKB (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  D0F0xE4_WRAP_8062_STRUCT  D0F0xE4_WRAP_8062;
  PCIe_SILICON_CONFIG       *Silicon;
  DEV_OBJECT                DevObject;

  if (PcieLibIsPcieWrapper (Wrapper)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "PcieTopologyExecuteReconfigKB Enter\n");

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

    GnbLibPciIndirectRMW (
      Silicon->Address.AddressValue | D0F0xB8_ADDRESS,
      0x3f81c,
      AccessWidth32,
      (UINT32) ~0xff00,
      Wrapper->WrapId << 8,
      GnbLibGetHeader (Pcie)
      );

    DevObject.StdHeader = GnbLibGetHeader (Pcie);
    DevObject.GnbHandle = GnbGetHandle (GnbLibGetHeader (Pcie));
    DevObject.DevPciAddress.AddressValue = Silicon->Address.AddressValue;
    GnbSmuServiceRequestV7 (
      &DevObject,
      25, //SMC_MSG_RECONFIGURE,
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

    IDS_HDT_CONSOLE (GNB_TRACE, "PcieTopologyExecuteReconfigKB Exit\n");
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Apply Misc settings for given wrapper
 *
 *
 *
 * @param[in]  Wrapper     Pointer to Wrapper config descriptor
 * @param[in]  Pcie        Pointer to PICe configuration data area
 */
STATIC VOID
PcieMiscInitKB (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  D0F0xE4_WRAP_8011_STRUCT  D0F0xE4_WRAP_8011;
  GnbRegistersKB4915_STRUCT  GnbRegistersKB4915;
  GnbRegistersKB4940_STRUCT  GnbRegistersKB4940;
  GnbRegistersKB4965_STRUCT  GnbRegistersKB4965;
  GnbRegistersKB4990_STRUCT  GnbRegistersKB4990;
  GnbRegistersKB5015_STRUCT  GnbRegistersKB5015;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMiscInitKB Enter\n");
  D0F0xE4_WRAP_8011.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8011_ADDRESS),
                              Pcie
                              );

  D0F0xE4_WRAP_8011.Field.Bitfield_23_23 = 0;

  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8011_ADDRESS),
    D0F0xE4_WRAP_8011.Value,
    TRUE,
    Pcie
    );

  GnbRegistersKB4915.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, 0x805),
                              Pcie
                              );

  GnbRegistersKB4940.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, 0x905),
                              Pcie
                              );

  GnbRegistersKB4965.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, 0x0A05),
                              Pcie
                              );
  GnbRegistersKB4990.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, 0x0B05),
                              Pcie
                              );

  GnbRegistersKB5015.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, 0x0C05),
                              Pcie
                              );

  GnbRegistersKB4915.Field.bit_31_24 = 0x40;
  GnbRegistersKB4940.Field.bit_31_24 = 0x40;
  GnbRegistersKB4965.Field.bit_31_24 = 0x40;
  GnbRegistersKB4990.Field.bit_31_24 = 0x40;
  GnbRegistersKB5015.Field.bit_31_24 = 0x40;

  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, 0x805),
    GnbRegistersKB4915.Value,
    TRUE,
    Pcie
    );

  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, 0x905),
    GnbRegistersKB4940.Value,
    TRUE,
    Pcie
    );
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, 0x0A05),
    GnbRegistersKB4965.Value,
    TRUE,
    Pcie
    );
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, 0x0B05),
    GnbRegistersKB4990.Value,
    TRUE,
    Pcie
    );
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, 0x0C05),
    GnbRegistersKB5015.Value,
    TRUE,
    Pcie
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMiscInitKB Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Switch to PCIe Native Gen1 PLL.
 *
 *
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
AGESA_STATUS
STATIC
PcieNativeGen1PLLSwitchKB (
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  PCIE_LINK_SPEED_CAP  GlobalSpeedCap;
  UINT32               ParamValue;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieNativeGen1PLLSwitchKB Enter\n");

  GlobalSpeedCap = PcieUtilGlobalGenCapability (
                     PCIE_PORT_GEN_CAP_MAX | PCIE_GLOBAL_GEN_CAP_TRAINED_PORTS | PCIE_GLOBAL_GEN_CAP_HOTPLUG_PORTS,
                     Pcie
                     );

  ParamValue = 0;
  if (GlobalSpeedCap == PcieGen1) {
    GnbRegisterWriteKB (GnbGetHandle (GnbLibGetHeader (Pcie)), TYPE_SMU_MSG, 87, &ParamValue, 0, GnbLibGetHeader (Pcie));
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieNativeGen1PLLSwitchKB Exit\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Per wrapper Pcie Init prior training.
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper configuration descriptor
 * @param[in]  Buffer          Pointer buffer
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
AGESA_STATUS
STATIC
PcieEarlyInitCallbackKB (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  AGESA_STATUS  Status;
  BOOLEAN       CoreConfigChanged;
  BOOLEAN       PllConfigChanged;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyInitCallbackKB Enter\n");
  CoreConfigChanged = FALSE;
  PllConfigChanged = FALSE;
  IDS_OPTION_HOOK (IDS_BEFORE_RECONFIGURATION, Pcie, GnbLibGetHeader (Pcie));
  PcieTopologyPrepareForReconfig (Wrapper, Pcie);
  Status = PcieTopologySetCoreConfig (Wrapper, &CoreConfigChanged, Pcie);
  ASSERT (Status == AGESA_SUCCESS);
  PciePhyIsolationKB (Wrapper, Pcie);
  PcieTopologyApplyLaneMuxKB (Wrapper, Pcie);
  PciePifSetRxDetectPowerMode (Wrapper, Pcie);
  PciePifSetLs2ExitTimeV5 (Wrapper, Pcie);
  PciePifApplyGanging (Wrapper, Pcie);
  PcieTopologySelectMasterPllKB (Wrapper, &PllConfigChanged, Pcie);
  PcieMiscInitKB (Wrapper, Pcie);
  PcieTopologyExecuteReconfigKB (Wrapper, Pcie);
  PcieTopologyCleanUpReconfig (Wrapper, Pcie);
  PcieTopologySetLinkReversalV4 (Wrapper, Pcie);
  PciePifPllConfigureKB (Wrapper, Pcie);
  PcieTopologyLaneControlV5 (
    DisableLanes,
    PcieUtilGetWrapperLaneBitMap (LANE_TYPE_CORE_ALL, LANE_TYPE_PCIE_CORE_ALLOC, Wrapper),
    Wrapper,
    Pcie
    );

  PciePollPifForCompeletion (Wrapper, Pcie);
  PcieEarlyCoreInitKB (Wrapper, Pcie);
  PcieSetSsidV4 (UserOptions.CfgGnbPcieSSID, Wrapper, Pcie);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyInitCallbackKB Exit [%x]\n", Status);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Pcie Init
 *
 *
 *
 * @param[in]  Pcie                Pointer to global PCIe configuration
 * @retval     AGESA_SUCCESS       Topology successfully mapped
 * @retval     AGESA_ERROR         Topology can not be mapped
 */

AGESA_STATUS
STATIC
PcieEarlyInitKB (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS        Status;
  AGESA_STATUS        AgesaStatus;
  BOOLEAN             NativeGen1PLL;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyInitKB Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  NativeGen1PLL = GnbBuildOptions.CfgNativeGen1PLL;

  Status = PcieConfigRunProcForAllDescriptors (DESCRIPTOR_SILICON, 0, DESCRIPTOR_TERMINATE_TOPOLOGY, PciePortMapInitCallbackKB, NULL, Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);

  IDS_OPTION_HOOK (IDS_GNB_PMM_NATIVEGEN1PLL, &NativeGen1PLL, GnbLibGetHeader (Pcie));

  if (NativeGen1PLL == TRUE) {
    Status = PcieNativeGen1PLLSwitchKB (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
  }

  Status = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, PcieEarlyInitCallbackKB, NULL, Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  PcieEarlyStaticInitKB (Pcie);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyInitKB Exit [%x]\n", AgesaStatus);
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set misc slot capability
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieLinkSetSlotCapKB (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32 IntPin;
  PCIe_WRAPPER_CONFIG         *Wrapper;

  GnbLibPciRMW (
    Engine->Type.Port.Address.AddressValue | DxF0x58_ADDRESS,
    AccessWidth32,
    0xffffffff,
    1 << DxF0x58_SlotImplemented_OFFSET,
    GnbLibGetHeader (Pcie)
    );

  if (Engine->Type.Port.Address.Address.Function < 5) {
    IntPin = Engine->Type.Port.Address.Address.Function;
  } else {
    IntPin = Engine->Type.Port.Address.Address.Function - 4;
  }

  GnbLibPciRMW (
    Engine->Type.Port.Address.AddressValue | DxF0x3C_ADDRESS,
    AccessWidth32,
    0xffffffff,
    IntPin << DxF0x3C_IntPin_OFFSET,
    GnbLibGetHeader (Pcie)
    );

  // Set MaxPayload straps for port
  if (Engine->EngineData.StartLane == Engine->EngineData.EndLane) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Set MaxPayload strap for StartLane = %d and EndLane = %d\n", Engine->EngineData.StartLane, Engine->EngineData.EndLane);
    Wrapper = PcieConfigGetParentWrapper (Engine);
    PcieRegisterRMW (
      Wrapper,
      WRAP_SPACE (Wrapper->WrapId, 0x804 + (Engine->Type.Port.PortId) * 0x100),
      0xe,
      MAX_PAYLOAD_256 << 1,
      FALSE,
      Pcie
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init various features on all active ports
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PcieEarlyPortInitCallbackKB (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyPortInitCallbackKB Enter\n");
  ASSERT (Engine->EngineData.EngineType == PciePortEngine);
  PciePortProgramRegisterTable (PortInitEarlyTableKB.Table, PortInitEarlyTableKB.Length, Engine, FALSE, Pcie);
  PcieSetLinkSpeedCapV4 (PcieGen1, Engine, Pcie);
  PcieSetLinkWidthCap (Engine, Pcie);
  PcieCompletionTimeout (Engine, Pcie);
  PcieLinkSetSlotCapKB (Engine, Pcie);
  PcieLinkInitHotplugV5 (Engine, Pcie);
  PciePhyChannelCharacteristicV5 (Engine, Pcie);
  if (Engine->Type.Port.PortData.PortPresent == PortDisabled ||
      (Engine->Type.Port.PortData.EndpointStatus == EndpointNotPresent &&
       Engine->Type.Port.PortData.LinkHotplug != HotplugEnhanced &&
       Engine->Type.Port.PortData.LinkHotplug != HotplugServer)) {
    ASSERT (!PcieConfigIsSbPcieEngine (Engine));
    //
    //  Pass endpoint status in scratch
    //
    PciePortRegisterRMW (
      Engine,
      0x1,
      0x1,
      0x1,
      FALSE,
      Pcie
      );
    PcieTrainingSetPortState (Engine, LinkStateDeviceNotPresent, FALSE, Pcie);
  }
  if (PcieConfigIsSbPcieEngine (Engine)) {
    PcieTrainingSetPortState (Engine, LinkStateTrainingSuccess, FALSE, Pcie);
  }
  if (Engine->Type.Port.PortData.MiscControls.LinkComplianceMode == 0x1) {
    PcieTrainingSetPortState (Engine, LinkStateTrainingCompleted, FALSE, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyPortInitCallbackKB Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init various features on all active ports
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */
VOID
STATIC
DdiEarlyPortInitCallbackKB (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32         GMMx6464;
  UINT32         GMMx5C6C;
  UINT32         GMMx5C90;

  IDS_HDT_CONSOLE (GNB_TRACE, "DdiEarlyPortInitCallbackKB Enter\n");
  if ((Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeEDP) ||
      (Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeDpToLvds) ||
      (Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeEDPToLvds) ||
      (Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeEDPToLvdsSwInit)) {

    IDS_HDT_CONSOLE (GNB_TRACE, "Found eDP/LVDS Connector\n");

    GnbRegisterReadKB (GnbGetHandle (GnbLibGetHeader (Pcie)), 0x12, 0x6464, &GMMx6464, 0, GnbLibGetHeader (Pcie));
    GnbRegisterReadKB (GnbGetHandle (GnbLibGetHeader (Pcie)), 0x12, 0x5c90, &GMMx5C90, 0, GnbLibGetHeader (Pcie));
    GnbRegisterReadKB (GnbGetHandle (GnbLibGetHeader (Pcie)), 0x12, 0x5c6c, &GMMx5C6C, 0, GnbLibGetHeader (Pcie));
    GMMx6464 |= 1;
    GMMx6464 |= 1 << 4;
    GMMx6464 |= 1 << 25;
    GMMx5C90 &= ~0x3f00; GMMx5C90 |= 1 << 8;
    GMMx5C6C &= ~0x1800; GMMx5C6C |= 1 << 13;
    GnbRegisterWriteKB (GnbGetHandle (GnbLibGetHeader (Pcie)), 0x12, 0x6464, &GMMx6464, 0, GnbLibGetHeader (Pcie));
    GnbRegisterWriteKB (GnbGetHandle (GnbLibGetHeader (Pcie)), 0x12, 0x5c90, &GMMx5C90, 0, GnbLibGetHeader (Pcie));
    GnbRegisterWriteKB (GnbGetHandle (GnbLibGetHeader (Pcie)), 0x12, 0x5c6c, &GMMx5C6C, 0, GnbLibGetHeader (Pcie));
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "DdiEarlyPortInitCallbackKB Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Master procedure to init various features on all active ports
 *
 *
 * @param[in]   Pcie            Pointer to global PCIe configuration
 * @retval      AGESA_STATUS
 *
 */
AGESA_STATUS
STATIC
PcieEarlyPortInitKB (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;
  // Leave all device in Presence Detect Presence state for distributed training will be completed at PciePortPostEarlyInit
  if (Pcie->TrainingAlgorithm == PcieTrainingDistributed) {
    Pcie->TrainingExitState = LinkStateResetExit;
  }

  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PcieEarlyPortInitCallbackKB,
    NULL,
    Pcie
    );

  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_VIRTUAL,
    DdiEarlyPortInitCallbackKB,
    NULL,
    Pcie
    );

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Early Post Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
PcieEarlyInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          Status;
  AGESA_STATUS          AgesaStatus;
  PCIe_PLATFORM_CONFIG  *Pcie;
  AgesaStatus = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyInterfaceKB Enter\n");
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControlV5 (UnhidePorts, Pcie);

    Status = PcieEarlyInitKB (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieEarlyPortInitKB (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieTraining (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_PCIE_PHY_CONFIG, Pcie, StdHeader);
    PciePortsVisibilityControlV5 (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyInterfaceKB Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}
