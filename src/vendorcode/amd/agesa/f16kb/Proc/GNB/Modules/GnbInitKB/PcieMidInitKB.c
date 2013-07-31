/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe mid post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 88079 $   @e \$Date: 2013-02-15 15:28:53 -0600 (Fri, 15 Feb 2013) $
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
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbPcieInitLibV4.h"
#include  "GnbPcieInitLibV5.h"
#include  "GnbFamServices.h"
#include  "PcieLibKB.h"
#include  "PciePortServicesV4.h"
#include  "GnbRegistersKB.h"
#include  "Filecode.h"

#define FILECODE PROC_GNB_MODULES_GNBINITKB_PCIEMIDINITKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern CONST PCIE_PORT_REGISTER_TABLE_HEADER ROMDATA PortInitMidTableKB;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
PcieMidInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );



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
PcieMidPortInitCallbackKB (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PciePortProgramRegisterTable (PortInitMidTableKB.Table, PortInitMidTableKB.Length, Engine, TRUE, Pcie);
  if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS) || Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled) {
    PcieEnableSlotPowerLimitV5 (Engine, Pcie);

    // If StartLane == 4/7 and EndLane == 7/4, this is GFX port
    if (!(((Engine->EngineData.StartLane == 4) && (Engine->EngineData.EndLane == 7)) ||
          ((Engine->EngineData.StartLane == 7) && (Engine->EngineData.EndLane == 4)))) {
      // Only count active Gpp ports
      *(UINT8 *)Buffer += 1;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init ASPM on all active ports
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          PortCount
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PcieMidAspmInitCallbackKB (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMidAspmInitCallbackKB Enter\n");

  IDS_HDT_CONSOLE (GNB_TRACE, "  PortCount = %02x\n", *(UINT8 *)Buffer);

  // If StartLane == 4/7 and EndLane == 7/4, this is GFX port
  if (!(((Engine->EngineData.StartLane == 4) && (Engine->EngineData.EndLane == 7)) ||
        ((Engine->EngineData.StartLane == 7) && (Engine->EngineData.EndLane == 4)))) {
    // For GPP ports only set STRAP_MED_yTSx_COUNT=2, but only if active ports is > 2
    switch (*(UINT8 *)Buffer) {
    case 0:
    case 1:
      break;
    case 2:
      PciePortRegisterRMW (
        Engine,
        DxFxxE4_xA0_ADDRESS,
        DxFxxE4_xA0_Reserved_26_25_MASK |
        DxFxxE4_xA0_Reserved_28_28_MASK,
        (0 << DxFxxE4_xA0_Reserved_26_25_OFFSET) |
        (1 << DxFxxE4_xA0_Reserved_28_28_OFFSET),
        TRUE,
        Pcie
        );
      break;
    default:
      PciePortRegisterRMW (
        Engine,
        0xC0,
        0x30,
        0x2 << 4,
        TRUE,
        Pcie
        );
      break;
    }
  }
  PcieEnableAspm (Engine,  Pcie);

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMidAspmInitCallbackKB Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Master procedure to init various features on all active ports
 *
 *
 *
 *
 * @param[in]   Pcie            Pointer to global PCIe configuration
 * @retval      AGESA_STATUS
 *
 */

AGESA_STATUS
STATIC
PcieMidPortInitKB (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS         Status;
  PCIE_LINK_SPEED_CAP  GlobalSpeedCap;
  UINT8 PortCount;

  Status = AGESA_SUCCESS;
  PortCount = 0;

  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PcieMidPortInitCallbackKB,
    &PortCount,
    Pcie
    );

  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PcieMidAspmInitCallbackKB,
    &PortCount,
    Pcie
    );

  GlobalSpeedCap = PcieUtilGlobalGenCapability (
                     PCIE_PORT_GEN_CAP_BOOT | PCIE_GLOBAL_GEN_CAP_TRAINED_PORTS | PCIE_GLOBAL_GEN_CAP_HOTPLUG_PORTS,
                     Pcie
                     );


  PcieSetVoltageKB (GlobalSpeedCap, Pcie);
  return Status;
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

STATIC VOID
PciePwrClockGatingKB (
  IN       PCIe_WRAPPER_CONFIG    *Wrapper,
  IN       PCIe_PLATFORM_CONFIG   *Pcie
  )
{
  D0F0xE4_WRAP_8011_STRUCT  D0F0xE4_WRAP_8011;
  D0F0xE4_WRAP_8012_STRUCT  D0F0xE4_WRAP_8012;
  D0F0xE4_WRAP_8014_STRUCT  D0F0xE4_WRAP_8014;
  D0F0xE4_WRAP_8015_STRUCT  D0F0xE4_WRAP_8015;
  D0F0xE4_WRAP_8016_STRUCT  D0F0xE4_WRAP_8016;
  UINT8                     CoreId;

  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrClockGatingKB Enter\n");
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

  D0F0xE4_WRAP_8011.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8011_ADDRESS),
                              Pcie
                              );

  if (Wrapper->Features.ClkGating == 0x1) {
    D0F0xE4_WRAP_8011.Field.TxclkPermGateLatency = 0;
    D0F0xE4_WRAP_8011.Field.Reserved_16_16 = 0x1;
    D0F0xE4_WRAP_8011.Field.TxclkPermGateEven = 0x1;
    D0F0xE4_WRAP_8011.Field.TxclkPermStop = 0;
    D0F0xE4_WRAP_8011.Field.TxclkDynGateEnable = 0x1;
    D0F0xE4_WRAP_8011.Field.TxclkDynGateLatency = 0;
    D0F0xE4_WRAP_8011.Field.TxclkRegsGateEnable = 0x1;
    D0F0xE4_WRAP_8011.Field.TxclkRegsGateLatency = 0;
    D0F0xE4_WRAP_8011.Field.TxclkLcntGateEnable = 0x1;

    D0F0xE4_WRAP_8012.Field.Pif1xIdleResumeLatency = 0x7;
    D0F0xE4_WRAP_8012.Field.Pif1xIdleGateEnable = 0x1;
    D0F0xE4_WRAP_8012.Field.Pif1xIdleGateLatency = 0;

    D0F0xE4_WRAP_8014.Field.TxclkPermGateEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.TxclkPrbsGateEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifA1xEnable = 0x1;
    D0F0xE4_WRAP_8014.Field.PcieGatePifB1xEnable = 0x1;

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
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8011_ADDRESS),
    D0F0xE4_WRAP_8011.Value,
    TRUE,
    Pcie
    );

  for (CoreId = Wrapper->StartPcieCoreId; CoreId <= Wrapper->EndPcieCoreId; CoreId++) {
    PcieRegisterWriteField (
      Wrapper,
      CORE_SPACE (CoreId, D0F0xE4_CORE_0011_ADDRESS),
      D0F0xE4_CORE_0011_DynClkLatency_OFFSET,
      D0F0xE4_CORE_0011_DynClkLatency_WIDTH,
      0xf,
      TRUE,
      Pcie
      );
  }

  if (Wrapper->Features.LclkGating == 0x1) {
    D0F0xE4_WRAP_8016.Value = PcieRegisterRead (
                                Wrapper,
                                WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8016_ADDRESS),
                                Pcie
                                );
    D0F0xE4_WRAP_8016.Field.LclkDynGateEnable = 0x1;
    D0F0xE4_WRAP_8016.Field.LclkGateFree = 0x1;
    D0F0xE4_WRAP_8016.Field.LclkDynGateLatency = 0x3F;
    PcieRegisterWrite (
      Wrapper,
      WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8016_ADDRESS),
      D0F0xE4_WRAP_8016.Value,
      TRUE,
      Pcie
      );
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrClockGatingKB Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Per wrapper Pcie Late Init.
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper configuration descriptor
 * @param[in]  Buffer          Pointer buffer
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
AGESA_STATUS
STATIC
PcieMidInitCallbackKB (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  PciePwrPowerDownUnusedLanesKB (Wrapper, Pcie);
  PciePwrClockGatingKB (Wrapper, Pcie);
  PcieLockRegisters (Wrapper, Pcie);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Pcie Late Init
 *
 *   Late PCIe initialization
 *
 * @param[in]  Pcie                Pointer to global PCIe configuration
 * @retval     AGESA_SUCCESS       Topology successfully mapped
 * @retval     AGESA_ERROR         Topology can not be mapped
 */

AGESA_STATUS
STATIC
PcieMidInitKB (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMidInitKB Enter\n");
  AgesaStatus = AGESA_SUCCESS;

  Status = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, PcieMidInitCallbackKB, NULL, Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMidInitKB Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Mid Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */
AGESA_STATUS
PcieMidInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMidInterfaceKB Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControlV5 (UnhidePorts, Pcie);

    Status = PcieMidPortInitKB (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieMidInitKB (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    PciePortsVisibilityControlV5 (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMidInterfaceKB Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}
