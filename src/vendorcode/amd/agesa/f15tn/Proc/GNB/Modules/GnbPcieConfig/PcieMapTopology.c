/* $NoKeywords:$ */
/**
 * @file
 *
 * Procedure to map user define topology to processor configuration
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
#include  "GnbPcieFamServices.h"
#include  "GeneralServices.h"
#include  "PcieInputParser.h"
#include  "PcieMapTopology.h"
#include  "GnbPcieConfig.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIECONFIG_PCIEMAPTOPOLOGY_FILECODE
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


AGESA_STATUS
STATIC
PcieMapPortsPciAddresses (
  IN      PCIe_SILICON_CONFIG         *Silicon,
  IN      PCIe_PLATFORM_CONFIG        *Pcie
  );

AGESA_STATUS
PcieMapTopologyOnWrapper (
  IN      PCIe_COMPLEX_DESCRIPTOR     *ComplexDescriptor,
  IN OUT  PCIe_WRAPPER_CONFIG         *Wrapper,
  IN      PCIe_PLATFORM_CONFIG        *Pcie
 );

VOID
PcieMapInitializeEngineData (
  IN      PCIe_COMPLEX_DESCRIPTOR     *ComplexDescriptor,
  IN OUT  PCIe_WRAPPER_CONFIG         *Wrapper,
  IN      PCIe_PLATFORM_CONFIG        *Pcie
 );

BOOLEAN
PcieCheckPortPciDeviceMapping (
  IN      PCIe_PORT_DESCRIPTOR        *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG          *Engine
  );

BOOLEAN
PcieIsDescriptorLinkWidthValid (
  IN      PCIe_ENGINE_DESCRIPTOR      *EngineDescriptor
  );

BOOLEAN
PcieCheckLanesMatch (
  IN      PCIe_ENGINE_DESCRIPTOR      *EngineDescriptor,
  IN      PCIe_ENGINE_CONFIG          *Engine
  );

BOOLEAN
PcieCheckDescriptorMapsToWrapper (
  IN      PCIe_ENGINE_DESCRIPTOR      *EngineDescriptor,
  IN      PCIe_WRAPPER_CONFIG         *Wrapper
  );

VOID
PcieAllocateEngine (
  IN      UINT8                       DescriptorIndex,
  IN      PCIe_ENGINE_CONFIG          *Engine
  );
/*----------------------------------------------------------------------------------------*/
/**
 * Configure engine list to support lane allocation according to configuration ID.
 *
 *
 *
 * @param[in]  ComplexDescriptor   Pointer to used define complex descriptor
 * @param[in]  Complex             Pointer to complex descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 * @retval     AGESA_SUCCESS       Topology successfully mapped
 * @retval     AGESA_ERROR         Topology can not be mapped
 */

AGESA_STATUS
PcieMapTopologyOnComplex (
  IN      PCIe_COMPLEX_DESCRIPTOR     *ComplexDescriptor,
  IN      PCIe_COMPLEX_CONFIG         *Complex,
  IN      PCIe_PLATFORM_CONFIG        *Pcie
  )
{
  PCIe_SILICON_CONFIG *Silicon;
  PCIe_WRAPPER_CONFIG *Wrapper;
  AGESA_STATUS        AgesaStatus;
  AGESA_STATUS        Status;

  AgesaStatus = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMapTopologyOnComplex Enter\n");
  Silicon = PcieConfigGetChildSilicon (Complex);
  while (Silicon != NULL) {
    Wrapper = PcieConfigGetChildWrapper (Silicon);
    while (Wrapper != NULL) {
      Status = PcieMapTopologyOnWrapper (ComplexDescriptor, Wrapper, Pcie);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
      if (Status == AGESA_ERROR) {
        PcieConfigDisableAllEngines (PciePortEngine | PcieDdiEngine, Wrapper);
        IDS_HDT_CONSOLE (PCIE_MISC, "  ERROR! Fail to map topology on %s Wrapper\n",
          PcieFmDebugGetWrapperNameString (Wrapper)
          );
        ASSERT (FALSE);
      }
      Wrapper = PcieLibGetNextDescriptor (Wrapper);
    }
    Status = PcieMapPortsPciAddresses (Silicon, Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    Silicon = PcieLibGetNextDescriptor (Silicon);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMapTopologyOnComplex Exit [%x]\n", AgesaStatus);
  return AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Configure engine list to support lane allocation according to configuration ID.
 *
 *
 *
 * @param[in]  EngineType          Engine type
 * @param[in]  ComplexDescriptor   Pointer to used define complex descriptor
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @retval     AGESA_SUCCESS       Topology successfully mapped
 * @retval     AGESA_ERROR         Topology can not be mapped
 */
STATIC AGESA_STATUS
PcieEnginesToWrapper (
  IN      PCIE_ENGINE_TYPE            EngineType,
  IN      PCIe_COMPLEX_DESCRIPTOR     *ComplexDescriptor,
  IN      PCIe_WRAPPER_CONFIG         *Wrapper
  )
{
  AGESA_STATUS                Status;
  PCIe_ENGINE_CONFIG          *EngineList;
  PCIe_ENGINE_DESCRIPTOR      *EngineDescriptor;
  UINT8                       ConfigurationId;
  UINT8                       Allocations;
  UINTN                       Index;
  UINTN                       NumberOfDescriptors;

  ConfigurationId = 0;
  Allocations = 0;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEnginesToWrapper Enter\n");
  NumberOfDescriptors = PcieInputParserGetNumberOfEngines (ComplexDescriptor);
  do {
    Status = PcieFmConfigureEnginesLaneAllocation (Wrapper, EngineType, ConfigurationId++);
    if (Status == AGESA_SUCCESS) {
      Allocations = 0;
      for (Index = 0; Index < NumberOfDescriptors; Index++) {
        EngineDescriptor = PcieInputParserGetEngineDescriptor (ComplexDescriptor, Index);
        if (EngineDescriptor->EngineData.EngineType == EngineType) {
          // Step 1, belongs to wrapper check.
          if (PcieCheckDescriptorMapsToWrapper (EngineDescriptor, Wrapper)) {
            ++Allocations;
            EngineList = PcieConfigGetChildEngine (Wrapper);
            while (EngineList != NULL) {
              if (!PcieLibIsEngineAllocated (EngineList)) {
                // Step 2.user descriptor less or equal to link width of engine
                if (PcieCheckLanesMatch (EngineDescriptor, EngineList)) {
                  // Step 3, Check if link width is correct.x1, x2, x4, x8, x16.
                  if (!PcieIsDescriptorLinkWidthValid (EngineDescriptor)) {
                    PcieConfigDisableEngine (EngineList);
                    return AGESA_ERROR;
                  }
                  if (EngineDescriptor->EngineData.EngineType == PciePortEngine) {
                    // Step 4, Family specifc, port device number match engine device
                    if (PcieCheckPortPciDeviceMapping ((PCIe_PORT_DESCRIPTOR*) EngineDescriptor, EngineList)) {
                      //Step 5, Family specifc, lanes can be muxed.
                      if (PcieFmCheckPortPcieLaneCanBeMuxed ((PCIe_PORT_DESCRIPTOR*) EngineDescriptor, EngineList)) {
                        PcieAllocateEngine ((UINT8) Index, EngineList);
                        --Allocations;
                        break;
                      }
                    }
                  } else {
                    PcieAllocateEngine ((UINT8) Index, EngineList);
                    --Allocations;
                    break;
                  }
                }
              } //end if PcieLibIsEngineAllocated
              EngineList = PcieLibGetNextDescriptor (EngineList);
            }
          } //end if PcieCheckDescriptorMapsToWrapper
        } // end if EngineType
      } //end for
    }
  } while (Status == AGESA_SUCCESS && Allocations != 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEnginesToWrapper Exit [%x]\n", Status);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check if lane from  user port descriptor (PCIe_PORT_DESCRIPTOR) belongs to wrapper (PCIe_WRAPPER_CONFIG)
 *
 *
 * @param[in]  EngineDescriptor    Pointer to used define engine descriptor
 * @param[in]  Wrapper             Pointer to PCIe_WRAPPER_CONFIG
 * @retval     TRUE                Belongs to wrapper
 * @retval     FALSE               Not belongs to wrapper
 */
BOOLEAN
PcieCheckDescriptorMapsToWrapper (
  IN      PCIe_ENGINE_DESCRIPTOR      *EngineDescriptor,
  IN      PCIe_WRAPPER_CONFIG         *Wrapper
  )
{
  BOOLEAN Result;
  UINT16  DescriptorHiLane;
  UINT16  DescriptorLoLane;
  UINT16  DescriptorNumberOfLanes;

  DescriptorLoLane = MIN (EngineDescriptor->EngineData.StartLane, EngineDescriptor->EngineData.EndLane);
  DescriptorHiLane = MAX (EngineDescriptor->EngineData.StartLane, EngineDescriptor->EngineData.EndLane);
  DescriptorNumberOfLanes = DescriptorHiLane - DescriptorLoLane + 1;
  Result = FALSE;

  if (Wrapper->StartPhyLane <= DescriptorLoLane && DescriptorHiLane <= Wrapper->EndPhyLane) {
    // Lanes of descriptor belongs to wrapper
    Result = TRUE;
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set Engine to be allocated.
 *
 *
 * @param[in]  DescriptorIndex    UINT8 index
 * @param[in]  Engine             Pointer to engine config
 */
VOID
PcieAllocateEngine (
  IN      UINT8                       DescriptorIndex,
  IN      PCIe_ENGINE_CONFIG          *Engine
  )
{
  PcieConfigSetDescriptorFlags (Engine, DESCRIPTOR_ALLOCATED);
  Engine->Scratch = DescriptorIndex;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Configure engine list to support lane allocation according to configuration ID.
 *
 * PCIE port
 *
 *
 * 1 Check if lane from  user port descriptor (PCIe_PORT_DESCRIPTOR) belongs to wrapper (PCIe_WRAPPER_CONFIG)
 * 2 Check if link width from user descriptor less or equal to link width of engine (PCIe_ENGINE_CONFIG)
 * 3 Check if link width is correct. Correct link width for PCIe port x1, x2, x4, x8, x16, correct link width for DDI x4, x8
 * 4 Check if user port device number (PCIe_PORT_DESCRIPTOR) match engine port device number (PCIe_ENGINE_CONFIG)
 * 5 Check if lane can be muxed
 *
 *
 * DDI Link
 *
 * 1 Check if lane from  user port descriptor (PCIe_DDI_DESCRIPTOR) belongs to wrapper (PCIe_WRAPPER_CONFIG)
 * 2 Check lane from  (PCIe_DDI_DESCRIPTOR) match exactly phy lane (PCIe_ENGINE_CONFIG)
 *
 *
 *
 * @param[in]     ComplexDescriptor   Pointer to used define complex descriptor
 * @param[in,out] Wrapper             Pointer to wrapper config descriptor
 * @param[in]     Pcie                Pointer to global PCIe configuration
 * @retval        AGESA_SUCCESS       Topology successfully mapped
 * @retval        AGESA_ERROR         Topology can not be mapped
 */
AGESA_STATUS
PcieMapTopologyOnWrapper (
  IN       PCIe_COMPLEX_DESCRIPTOR     *ComplexDescriptor,
  IN OUT   PCIe_WRAPPER_CONFIG         *Wrapper,
  IN       PCIe_PLATFORM_CONFIG        *Pcie
  )
{
  AGESA_STATUS                AgesaStatus;
  AGESA_STATUS                Status;
  PCIe_ENGINE_CONFIG          *EngineList;
  UINT32                      WrapperPhyLaneBitMap;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMapTopologyOnWrapper Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  if (PcieLibIsPcieWrapper (Wrapper)) {
    Status = PcieEnginesToWrapper (PciePortEngine, ComplexDescriptor, Wrapper);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    if (Status == AGESA_ERROR) {
      // If we can not map topology on wrapper we can not enable any engines.
      PutEventLog (
        AGESA_ERROR,
        GNB_EVENT_INVALID_PCIE_TOPOLOGY_CONFIGURATION,
        Wrapper->WrapId,
        Wrapper->StartPhyLane,
        Wrapper->EndPhyLane,
        0,
        GnbLibGetHeader (Pcie)
        );
      PcieConfigDisableAllEngines (PciePortEngine, Wrapper);
    }
  }
  if (PcieLibIsDdiWrapper (Wrapper)) {
    Status = PcieEnginesToWrapper (PcieDdiEngine, ComplexDescriptor, Wrapper);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    if (Status == AGESA_ERROR) {
      // If we can not map topology on wrapper we can not enable any engines.
      PutEventLog (
        AGESA_ERROR,
        GNB_EVENT_INVALID_DDI_TOPOLOGY_CONFIGURATION,
        Wrapper->WrapId,
        Wrapper->StartPhyLane,
        Wrapper->EndPhyLane,
        0,
        GnbLibGetHeader (Pcie)
        );
      PcieConfigDisableAllEngines (PcieDdiEngine, Wrapper);
    }
  }
  // Copy engine data
  PcieMapInitializeEngineData (ComplexDescriptor, Wrapper, Pcie);

  EngineList = PcieConfigGetChildEngine (Wrapper);
  // Verify if we oversubscribe lanes and PHY link width
  WrapperPhyLaneBitMap = 0;
  while (EngineList != NULL) {
    UINT32  EnginePhyLaneBitMap;
    if (PcieLibIsEngineAllocated (EngineList)) {
      EnginePhyLaneBitMap = PcieConfigGetEnginePhyLaneBitMap (EngineList);
      if ((WrapperPhyLaneBitMap & EnginePhyLaneBitMap) != 0) {
        IDS_HDT_CONSOLE (PCIE_MISC, "  ERROR! Lanes double subscribe lanes [Engine Lanes %d..%d]\n",
          EngineList->EngineData.StartLane,
          EngineList->EngineData.EndLane
          );
        PutEventLog (
          AGESA_ERROR,
          GNB_EVENT_INVALID_LANES_CONFIGURATION,
          EngineList->EngineData.StartLane,
          EngineList->EngineData.EndLane,
          0,
          0,
          GnbLibGetHeader (Pcie)
          );
        PcieConfigDisableEngine (EngineList);
        Status = AGESA_ERROR;
        AGESA_STATUS_UPDATE (Status, AgesaStatus);
      } else {
        WrapperPhyLaneBitMap |= EnginePhyLaneBitMap;
      }
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMapTopologyOnWrapper Exit [%d]\n", AgesaStatus);
  return AgesaStatus;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Initialize engine data
 *
 *
 *
 * @param[in]     ComplexDescriptor   Pointer to user defined complex descriptor
 * @param[in,out] Wrapper             Pointer to wrapper config descriptor
 * @param[in]     Pcie                Pointer to global PCIe configuration
 */
VOID
PcieMapInitializeEngineData (
  IN       PCIe_COMPLEX_DESCRIPTOR     *ComplexDescriptor,
  IN OUT   PCIe_WRAPPER_CONFIG         *Wrapper,
  IN       PCIe_PLATFORM_CONFIG        *Pcie
  )
{
  PCIe_ENGINE_CONFIG        *EngineList;
  PCIe_ENGINE_DESCRIPTOR    *EngineDescriptor;

  EngineList = PcieConfigGetChildEngine (Wrapper);
  while (EngineList != NULL) {
    if (PcieLibIsEngineAllocated (EngineList)) {
      if (EngineList->Scratch != 0xFF) {
        EngineDescriptor = PcieInputParserGetEngineDescriptor (ComplexDescriptor, EngineList->Scratch);
        LibAmdMemCopy (&EngineList->EngineData, &EngineDescriptor->EngineData, sizeof (EngineDescriptor->EngineData), GnbLibGetHeader (Pcie));
        if (PcieLibIsDdiEngine (EngineList)) {
          LibAmdMemCopy (&EngineList->Type.Ddi, &((PCIe_DDI_DESCRIPTOR*) EngineDescriptor)->Ddi, sizeof (PCIe_DDI_DATA), GnbLibGetHeader (Pcie));
          EngineList->Type.Ddi.DisplayPriorityIndex = (UINT8) EngineList->Scratch;
        } else if (PcieLibIsPcieEngine (EngineList)) {
          LibAmdMemCopy (&EngineList->Type.Port, &((PCIe_PORT_DESCRIPTOR*) EngineDescriptor)->Port, sizeof (PCIe_PORT_DATA), GnbLibGetHeader (Pcie));
        }
      }
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Allocate PCI addresses for all PCIe engines on silicon
 *
 *
 *
 * @param[in]  PortDescriptor      Pointer to user defined engine descriptor
 * @param[in]  Engine              Pointer engine configuration
 * @retval     TRUE                Descriptor can be mapped to engine
 * @retval     FALSE               Descriptor can NOT be mapped to engine
 */

BOOLEAN
PcieCheckPortPciDeviceMapping (
  IN      PCIe_PORT_DESCRIPTOR  *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG    *Engine
  )
{
  BOOLEAN Result;

  if ((PortDescriptor->Port.DeviceNumber == Engine->Type.Port.NativeDevNumber &&
    PortDescriptor->Port.FunctionNumber == Engine->Type.Port.NativeFunNumber) ||
    (PortDescriptor->Port.DeviceNumber == 0 && PortDescriptor->Port.FunctionNumber == 0)) {
    Result = TRUE;
  } else {
    Result = PcieFmCheckPortPciDeviceMapping (PortDescriptor, Engine);
  }

  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Allocate PCI addresses for all PCIe engines on silicon
 *
 *
 *
 * @param[in]  Silicon             Pointer to silicon configurration
 * @param[in]  Pcie                Pointer PCIe configuration
 * @retval     AGESA_ERROR         Fail to allocate PCI device address
 * @retval     AGESA_SUCCESS       Successfully allocate PCI address for all PCIe ports
 */

AGESA_STATUS
STATIC
PcieMapPortsPciAddresses (
  IN      PCIe_SILICON_CONFIG    *Silicon,
  IN      PCIe_PLATFORM_CONFIG   *Pcie
  )
{
  AGESA_STATUS        Status;
  AGESA_STATUS        AgesaStatus;
  PCIe_WRAPPER_CONFIG *WrapperList;
  PCIe_ENGINE_CONFIG  *EngineList;
  AgesaStatus = AGESA_SUCCESS;
  WrapperList = PcieConfigGetChildWrapper (Silicon);
  while (WrapperList != NULL) {
    EngineList = PcieConfigGetChildEngine (WrapperList);
    while (EngineList != NULL) {
      if (PcieLibIsPcieEngine (EngineList) && PcieLibIsEngineAllocated (EngineList)) {
        Status = PcieFmMapPortPciAddress (EngineList);
        AGESA_STATUS_UPDATE (Status, AgesaStatus);
        if (Status == AGESA_SUCCESS) {
          EngineList->Type.Port.Address.AddressValue = MAKE_SBDFO (
                                                         0,
                                                         Silicon->Address.Address.Bus,
                                                         EngineList->Type.Port.PortData.DeviceNumber,
                                                         EngineList->Type.Port.PortData.FunctionNumber,
                                                         0
                                                         );
        } else {
          EngineList->Type.Port.PortData.PortPresent = OFF;
          IDS_HDT_CONSOLE (PCIE_MISC, "  ERROR! Fail to allocate PCI address for PCIe port\n"
            );
          //Report error
          PutEventLog (
            AGESA_ERROR,
            GNB_EVENT_INVALID_PCIE_PORT_CONFIGURATION,
            EngineList->Type.Port.PortData.DeviceNumber,
            0,
            0,
            0,
            GnbLibGetHeader (Pcie)
            );
        }
      }
      EngineList = PcieLibGetNextDescriptor (EngineList);
    }
    WrapperList = PcieLibGetNextDescriptor (WrapperList);
  }
  return AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * If link width from user descriptor less or equal to link width of engine
 *
 *
 * @param[in]  EngineDescriptor    Pointer to used define engine descriptor
 * @param[in]  Engine              Pointer to engine config
 * @retval     TRUE                Descriptor can be mapped to engine
 * @retval     FALSE               Descriptor can NOT be mapped to engine
 */

BOOLEAN
PcieCheckLanesMatch (
  IN      PCIe_ENGINE_DESCRIPTOR      *EngineDescriptor,
  IN      PCIe_ENGINE_CONFIG          *Engine
  )
{
  BOOLEAN Result;
  UINT16  DescriptorHiLane;
  UINT16  DescriptorLoLane;
  UINT16  DescriptorNumberOfLanes;

  DescriptorLoLane = MIN (EngineDescriptor->EngineData.StartLane, EngineDescriptor->EngineData.EndLane);
  DescriptorHiLane = MAX (EngineDescriptor->EngineData.StartLane, EngineDescriptor->EngineData.EndLane);
  DescriptorNumberOfLanes = DescriptorHiLane - DescriptorLoLane + 1;
  Result = FALSE;

  if (EngineDescriptor->EngineData.EngineType == PciePortEngine) {
    //
    // If link width from user descriptor less or equal to link width of engine (PCIe_ENGINE_CONFIG)
    //
    if (DescriptorNumberOfLanes <= PcieConfigGetNumberOfCoreLane (Engine)) {
      Result = TRUE;
    }
  } else if (EngineDescriptor->EngineData.EngineType == PcieDdiEngine) {
    //
    //For Ddi, check lane from  (PCIe_DDI_DESCRIPTOR) match exactly phy lane (PCIe_ENGINE_CONFIG)
    //
    if ((Engine->EngineData.StartLane == DescriptorLoLane) && (Engine->EngineData.EndLane == DescriptorHiLane)) {
      Result = TRUE;
    }
  }

  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Correct link width for PCIe port x1, x2, x4, x8, x16, correct link width for DDI x4, x8
 *
 *
 * @param[in]  EngineDescriptor    A pointer of PCIe_ENGINE_DESCRIPTOR
 * @retval     TRUE                Descriptor can be mapped to engine
 * @retval     FALSE               Descriptor can NOT be mapped to engine
 */

BOOLEAN
PcieIsDescriptorLinkWidthValid (
  IN      PCIe_ENGINE_DESCRIPTOR      *EngineDescriptor
  )
{
  BOOLEAN   Result;
  UINT16    DescriptorHiLane;
  UINT16    DescriptorLoLane;
  UINT16    DescriptorNumberOfLanes;

  Result = FALSE;
  DescriptorLoLane = MIN (EngineDescriptor->EngineData.StartLane, EngineDescriptor->EngineData.EndLane);
  DescriptorHiLane = MAX (EngineDescriptor->EngineData.StartLane, EngineDescriptor->EngineData.EndLane);
  DescriptorNumberOfLanes = DescriptorHiLane - DescriptorLoLane + 1;

  if (EngineDescriptor->EngineData.EngineType == PciePortEngine) {
    if (DescriptorNumberOfLanes == 1 || DescriptorNumberOfLanes == 2 || DescriptorNumberOfLanes == 4 ||
        DescriptorNumberOfLanes == 8 || DescriptorNumberOfLanes == 16) {
      Result = TRUE;
    }
  } else if (EngineDescriptor->EngineData.EngineType == PcieDdiEngine) {
    if (DescriptorNumberOfLanes == 4 || DescriptorNumberOfLanes == 8 || DescriptorNumberOfLanes == 7) {
      Result = TRUE;
    }
  }

  GNB_DEBUG_CODE (
    if (!Result) {
      IDS_HDT_CONSOLE (PCIE_MISC, "  Invalid Link width [Engine Lanes %d..%d]\n",
        DescriptorLoLane,
        DescriptorHiLane
      );
    }
  );

  return Result;
}

