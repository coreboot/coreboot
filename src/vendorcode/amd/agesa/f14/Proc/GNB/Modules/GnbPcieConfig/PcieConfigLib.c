/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB function to create/locate PCIe configuration data area
 *
 * Contain code that create/locate and rebase configuration data area.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 39898 $   @e \$Date: 2010-10-15 17:08:45 -0400 (Fri, 15 Oct 2010) $
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
#include  "GnbPcieFamServices.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  GNB_MODULE_DEFINITIONS (GnbPcieConfig)
#include  "PcieMapTopology.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIECONFIG_PCIECONFIGLIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Get number of core lanes
 *
 *
 *
 * @param[in]  Engine      Pointer to engine descriptor
 * @retval                 Number of core lane
 */
UINT8
PcieConfigGetNumberOfCoreLane (
  IN      PCIe_ENGINE_CONFIG   *Engine
  )
{
  if (Engine->Type.Port.StartCoreLane >= UNUSED_LANE_ID || Engine->Type.Port.EndCoreLane >= UNUSED_LANE_ID) {
    return 0;
  }
  return (UINT8) (Engine->Type.Port.EndCoreLane - Engine->Type.Port.StartCoreLane + 1);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Disable engine
 *
 *
 *
 * @param[in]   Engine              Pointer to engine config descriptor
 */
VOID
PcieConfigDisableEngine (
  IN      PCIe_ENGINE_CONFIG    *Engine
  )
{
  if (Engine->Type.Port.IsSB) {
    return;
  }
  Engine->Flags &= ~DESCRIPTOR_ALLOCATED;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Disable all engines on wrapper
 *
 *
 *
 * @param[in]  EngineTypeMask Engine type bitmap.
 * @param[in]  Wrapper        Pointer to wrapper config descriptor
 */
VOID
PcieConfigDisableAllEngines (
  IN      UINTN                EngineTypeMask,
  IN      PCIe_WRAPPER_CONFIG  *Wrapper
  )
{
  PCIe_ENGINE_CONFIG        *EngineList;
  EngineList = PcieWrapperGetEngineList (Wrapper);
  while (EngineList != NULL) {
    if ((EngineList->EngineData.EngineType & EngineTypeMask) != 0) {
      PcieConfigDisableEngine (EngineList);
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get engine PHY lanes bitmap
 *
 *
 *
 * @param[in]   Engine              Pointer to engine config descriptor
 */
UINT32
PcieConfigGetEnginePhyLaneBitMap (
  IN      PCIe_ENGINE_CONFIG    *Engine
  )
{
  UINT32  LaneBitMap;
  LaneBitMap = 0;
  if (PcieLibIsEngineAllocated (Engine)) {
    LaneBitMap = ((1 << PcieConfigGetNumberOfPhyLane (Engine)) - 1) <<  (PcieLibGetLoPhyLane (Engine) - PcieEngineGetParentWrapper (Engine)->StartPhyLane);
  }
  return LaneBitMap;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get number of phy lanes
 *
 *
 *
 * @param[in]  Engine      Pointer to engine config descriptor
 * @retval                 Number of Phy lane
 */
UINT8
PcieConfigGetNumberOfPhyLane (
  IN      PCIe_ENGINE_CONFIG   *Engine
  )
{
  if (Engine->EngineData.StartLane >= UNUSED_LANE_ID || Engine->EngineData.StartLane >= UNUSED_LANE_ID) {
    return 0;
  }
  if (Engine->EngineData.StartLane > Engine->EngineData.EndLane) {
    return (UINT8) (Engine->EngineData.StartLane - Engine->EngineData.EndLane + 1);
  } else {
    return (UINT8) (Engine->EngineData.EndLane - Engine->EngineData.StartLane + 1);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get port configuration signature for given wrapper and core
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Wrapper     Pointer to wrapper config descriptor
 * @param[in]  CoreId      Core ID
 * @retval                 Configuration Signature
 */
UINT64
PcieConfigGetConfigurationSignature (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT8                 CoreId
  )
{
  UINT64              ConfigurationSignature;
  PCIe_ENGINE_CONFIG  *EngineList;
  ConfigurationSignature = 0;
  EngineList = PcieWrapperGetEngineList (Wrapper);
  while (EngineList != NULL) {
    if (EngineList->Type.Port.CoreId == CoreId) {
      ConfigurationSignature = (ConfigurationSignature << 8) | PcieConfigGetNumberOfCoreLane (EngineList);
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
  return ConfigurationSignature;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check Port Status
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  PortStatus      Check if status asserted for port
 * @retval                     TRUE if status asserted
 */
BOOLEAN
PcieConfigCheckPortStatus (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN       UINT32                PortStatus
  )
{
  return (Engine->InitStatus & PortStatus) == 0 ? FALSE : TRUE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set/Reset port status
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  SetStatus       SetStatus
 * @param[in]  ResetStatus     ResetStatus
 *
 */
UINT32
PcieConfigUpdatePortStatus (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN       UINT32                SetStatus,
  IN       UINT32                ResetStatus
  )
{
  Engine->InitStatus |= SetStatus;
  Engine->InitStatus &= (~ResetStatus);
  return Engine->InitStatus;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Execute callback on all engine in topology
 *
 *
 * @param[in]       DescriptorFlags   Wrapper Flags
 * @param[in]       Callback          Pointer to callback function
 * @param[in, out]  Buffer            Pointer to buffer to pass information to callback
 * @param[in]       Pcie              Pointer to global PCIe configuration
 */

AGESA_STATUS
PcieConfigRunProcForAllWrappers (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_WRAPPER_CALLBACK  Callback,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_COMPLEX_CONFIG   *Complex;
  AgesaStatus = AGESA_SUCCESS;
  Complex = Pcie->ComplexList;
  while (Complex != NULL) {
    PCIe_SILICON_CONFIG *Silicon;
    Silicon = PcieComplexGetSiliconList (Complex);
    while (Silicon != NULL) {
      PCIe_WRAPPER_CONFIG *Wrapper;
      Wrapper = PcieSiliconGetWrapperList (Silicon);
      while (Wrapper != NULL) {
        if (!(PcieLibIsVirtualDesciptor (Wrapper) && (DescriptorFlags & DESCRIPTOR_VIRTUAL) == 0)) {
          if ((DescriptorFlags & DESCRIPTOR_ALL_WRAPPERS & Wrapper->Flags) != 0) {
            Status = Callback (Wrapper, Buffer, Pcie);
            AGESA_STATUS_UPDATE (Status, AgesaStatus);
          }
        }
        Wrapper = PcieLibGetNextDescriptor (Wrapper);
      }
      Silicon = PcieLibGetNextDescriptor (Silicon);
    }
    Complex = PcieLibGetNextDescriptor (Complex);
  }
  return AgesaStatus;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Execute callback on all engine in topology
 *
 *
 * @param[in]       DescriptorFlags Engine flags.
 * @param[in]       Callback        Pointer to callback function
 * @param[in, out]  Buffer          Pointer to buffer to pass information to callback
 * @param[in]       Pcie            Pointer to global PCIe configuration
 */

VOID
PcieConfigRunProcForAllEngines (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_ENGINE_CALLBACK   Callback,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  PCIe_COMPLEX_CONFIG *Complex;
  Complex = Pcie->ComplexList;
  while (Complex != NULL) {
    PCIe_SILICON_CONFIG *Silicon;
    Silicon = PcieComplexGetSiliconList (Complex);
    while (Silicon != NULL) {
      PCIe_WRAPPER_CONFIG *Wrapper;
      Wrapper = PcieSiliconGetWrapperList (Silicon);
      while (Wrapper != NULL) {
        PCIe_ENGINE_CONFIG *Engine;
        Engine = PcieWrapperGetEngineList (Wrapper);
        while (Engine != NULL) {
          if (!(PcieLibIsVirtualDesciptor (Engine) && (DescriptorFlags & DESCRIPTOR_VIRTUAL) == 0)) {
            if (!((DescriptorFlags & DESCRIPTOR_ALLOCATED) != 0 && !PcieLibIsEngineAllocated (Engine))) {
              if ((Engine->Flags & DESCRIPTOR_ALL_ENGINES & DescriptorFlags) != 0) {
                Callback (Engine, Buffer, Pcie);
              }
            }
          }
          Engine = PcieLibGetNextDescriptor (Engine);
        }
        Wrapper = PcieLibGetNextDescriptor (Wrapper);
      }
      Silicon = PcieLibGetNextDescriptor (Silicon);
    }
    Complex = PcieLibGetNextDescriptor (Complex);
  }
}
