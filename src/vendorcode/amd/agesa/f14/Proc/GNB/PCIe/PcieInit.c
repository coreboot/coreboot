/* $NoKeywords:$ */
/**
 * @file
 *
 * Pre-training PCIe subsystem initialization routines.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 39275 $   @e \$Date: 2010-10-09 08:22:05 +0800 (Sat, 09 Oct 2010) $
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
#include  "PcieFamilyServices.h"
#include  "PcieInit.h"
#include  "PcieMiscLib.h"
#include  GNB_MODULE_DEFINITIONS (GnbPcieInitLibV1)
#include  GNB_MODULE_DEFINITIONS (GnbPcieConfig)
#include  GNB_MODULE_DEFINITIONS (GnbPcieTrainingV1)
#include  "GnbRegistersON.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_PCIE_PCIEINIT_FILECODE
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
PcieCommonCoreInit (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

AGESA_STATUS
PcieInitSrbmCallback (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  );

AGESA_STATUS
PcieInitCallback (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  );

AGESA_STATUS
PciePostInitCallback (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Control port visibility in PCI config space
 *
 *
 * @param[in]  Control         Make port Hide/Unhide ports
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
VOID
PciePortsVisibilityControl (
  IN      PCIE_PORT_VISIBILITY  Control,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_COMPLEX_CONFIG  *ComplexList;
  ComplexList = &Pcie->ComplexList[0];
  while (ComplexList != NULL) {
    PCIe_SILICON_CONFIG  *SiliconList;
    SiliconList = PcieComplexGetSiliconList (ComplexList);
    while (SiliconList != NULL) {
      PcieFmPortVisabilityControl (Control, SiliconList, Pcie);
      SiliconList = PcieLibGetNextDescriptor (SiliconList);
    }
    ComplexList = PcieLibGetNextDescriptor (ComplexList);
  }
}


CONST PCIE_HOST_REGISTER_ENTRY CoreInitTable [] = {
  {
    D0F0xE4_CORE_0020_ADDRESS,
    D0F0xE4_CORE_0020_CiRcOrderingDis_MASK,
    (0x1 << D0F0xE4_CORE_0020_CiRcOrderingDis_OFFSET)
  },
  {
    0x10,
    0x1c00,
    (0x4 << 10)
  },
  {
    D0F0xE4_CORE_001C_ADDRESS,
    D0F0xE4_CORE_001C_TxArbRoundRobinEn_MASK |
    D0F0xE4_CORE_001C_TxArbSlvLimit_MASK |
    D0F0xE4_CORE_001C_TxArbMstLimit_MASK,
    (0x1 << D0F0xE4_CORE_001C_TxArbRoundRobinEn_OFFSET) |
    (0x4 << D0F0xE4_CORE_001C_TxArbSlvLimit_OFFSET) |
    (0x4 << D0F0xE4_CORE_001C_TxArbMstLimit_OFFSET)
  },
  {
    D0F0xE4_CORE_0040_ADDRESS,
    D0F0xE4_CORE_0040_PElecIdleMode_MASK,
    (0x2 << D0F0xE4_CORE_0040_PElecIdleMode_OFFSET)
  },
  {
    D0F0xE4_CORE_0002_ADDRESS,
    D0F0xE4_CORE_0002_HwDebug_0__MASK,
    (0x1 << D0F0xE4_CORE_0002_HwDebug_0__OFFSET)
  },
  {
    D0F0xE4_CORE_00C1_ADDRESS,
    D0F0xE4_CORE_00C1_StrapLinkBwNotificationCapEn_MASK |
    D0F0xE4_CORE_00C1_StrapGen2Compliance_MASK,
    (0x1 << D0F0xE4_CORE_00C1_StrapLinkBwNotificationCapEn_OFFSET) |
    (0x1 << D0F0xE4_CORE_00C1_StrapGen2Compliance_OFFSET)
  },
  {
    D0F0xE4_CORE_00B0_ADDRESS,
    D0F0xE4_CORE_00B0_StrapF0MsiEn_MASK,
    (0x1 << D0F0xE4_CORE_00B0_StrapF0MsiEn_OFFSET)
  }
};

/*----------------------------------------------------------------------------------------*/
/**
 * Common Core Init
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper configuration descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
VOID
PcieCommonCoreInit (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8   CoreId;
  UINTN   Index;
  if (PcieLibIsPcieWrapper (Wrapper)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "PcieCommonCoreInit Enter\n");
    for (CoreId = Wrapper->StartPcieCoreId; CoreId <= Wrapper->EndPcieCoreId; CoreId++) {
      for (Index = 0; Index < ARRAY_SIZE(CoreInitTable); Index++) {
        UINT32 Value;
        Value = PcieRegisterRead (
                  Wrapper,
                  CORE_SPACE (CoreId, CoreInitTable[Index].Reg),
                  Pcie
                  );
        Value &= (~CoreInitTable[Index].Mask);
        Value |= CoreInitTable[Index].Data;
        PcieRegisterWrite (
          Wrapper,
          CORE_SPACE (CoreId, CoreInitTable[Index].Reg),
          Value,
          FALSE,
          Pcie
          );
      }
    }
    IDS_HDT_CONSOLE (GNB_TRACE, "PcieCommonCoreInit Exit\n");
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Per wrapper Pcie Init SRBM reset prior Aaccess to wrapper registers.
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper configuration descriptor
 * @param[in]  Buffer          Pointer buffer
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
AGESA_STATUS
PcieInitSrbmCallback (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  PcieTopologyInitSrbmReset (TRUE, Wrapper, Pcie);
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
PcieInitCallback (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  AGESA_STATUS  Status;
  PcieTopologyPrepareForReconfig (Wrapper, Pcie);
  Status = PcieTopologySetCoreConfig (Wrapper, Pcie);
  ASSERT (Status == AGESA_SUCCESS);
  PcieTopologyApplyLaneMux (Wrapper, Pcie);
  PcieFmPifSetRxDetectPowerMode (Wrapper, Pcie);
  PciePifSetLs2ExitTime (Wrapper, Pcie);
  PcieTopologySelectMasterPll (Wrapper, Pcie);
  PcieTopologyExecuteReconfig (Wrapper, Pcie);
  PcieTopologySetLinkReversal (Wrapper, Pcie);
  PciePifApplyGanging (Wrapper, Pcie);
  PcieFmPhyApplyGanging (Wrapper, Pcie);
  PciePifPllInitForDdi (Wrapper, Pcie);
  PcieTopologyLaneControl (
    DisableLanes,
    PcieUtilGetWrapperLaneBitMap (LANE_TYPE_ALL, LANE_TYPE_PCIE_ALLOCATED, Wrapper, Pcie),
    Wrapper,
    Pcie
    );
  PcieSetDdiOwnPhy (Wrapper, Pcie);
  PciePollPifForCompeletion (Wrapper, Pcie);
  PcieFmAvertClockPickers (Wrapper, Pcie);
  PcieFmConfigureClock (PcieGen1, Wrapper, Pcie);
  PcieCommonCoreInit (Wrapper, Pcie);
  PciePifDisableFifoReset (Wrapper, Pcie);
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
PcieInit (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS        Status;
  AGESA_STATUS        AgesaStatus;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieInit Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, PcieInitSrbmCallback, NULL, Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  PcieFmPreInit (Pcie);
  Status = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, PcieInitCallback, NULL, Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  PcieFmSetBootUpVoltage (PcieGen1, Pcie);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieInit Exit [%x]\n", AgesaStatus);
  return  AgesaStatus;
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
PciePostInitCallback (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  AGESA_STATUS  Status;

  Status = AGESA_SUCCESS;
  PcieFmConfigureClock (
    PcieUtilGlobalGenCapability (PCIE_PORT_GEN_CAP_BOOT | PCIE_GLOBAL_GEN_CAP_ALL_PORTS, Pcie),
    Wrapper,
    Pcie
    );
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
PciePostInit (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS        Status;
  AGESA_STATUS        AgesaStatus;

  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostInit Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieConfigRunProcForAllWrappers (DESCRIPTOR_PCIE_WRAPPER, PciePostInitCallback, NULL, Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  PcieFmSetBootUpVoltage (
    PcieUtilGlobalGenCapability (PCIE_PORT_GEN_CAP_BOOT | PCIE_GLOBAL_GEN_CAP_ALL_PORTS, Pcie),
    Pcie
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostInit Exit [%x]\n", AgesaStatus);
  return  AgesaStatus;
}
