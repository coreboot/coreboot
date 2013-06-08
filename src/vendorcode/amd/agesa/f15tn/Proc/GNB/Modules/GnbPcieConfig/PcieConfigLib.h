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

#ifndef _PCIECONFIGLIB_H_
#define _PCIECONFIGLIB_H_

typedef VOID (*PCIe_RUN_ON_ENGINE_CALLBACK) (
  IN      PCIe_ENGINE_CONFIG                *Engine,
  IN OUT  VOID                              *Buffer,
  IN      PCIe_PLATFORM_CONFIG              *Pcie
  );

typedef AGESA_STATUS (*PCIe_RUN_ON_WRAPPER_CALLBACK) (
  IN      PCIe_WRAPPER_CONFIG               *Wrapper,
  IN OUT  VOID                              *Buffer,
  IN      PCIe_PLATFORM_CONFIG              *Pcie
  );

typedef AGESA_STATUS (*PCIe_RUN_ON_DESCRIPTOR_CALLBACK) (
  IN      PCIe_DESCRIPTOR_HEADER            *Descriptor,
  IN OUT  VOID                              *Buffer,
  IN      PCIe_PLATFORM_CONFIG              *Pcie
  );

UINT8
PcieConfigGetPcieEngineMasterLane (
  IN      PCIe_ENGINE_CONFIG                *Engine
  );

UINT8
PcieConfigGetNumberOfCoreLane (
  IN      PCIe_ENGINE_CONFIG                *Engine
  );

VOID
PcieConfigDisableAllEngines (
  IN      UINTN                             EngineTypeMask,
  IN      PCIe_WRAPPER_CONFIG               *Wrapper
  );

VOID
PcieConfigDisableEngine (
  IN      PCIe_ENGINE_CONFIG                *Engine
  );

UINT32
PcieConfigGetEnginePhyLaneBitMap (
  IN      PCIe_ENGINE_CONFIG                *Engine
  );

UINT8
PcieConfigGetNumberOfPhyLane (
  IN      PCIe_ENGINE_CONFIG                *Engine
  );

UINT64
PcieConfigGetConfigurationSignature (
  IN      PCIe_WRAPPER_CONFIG               *Wrapper,
  IN      UINT8                             CoreId
  );

BOOLEAN
PcieConfigCheckPortStatus (
  IN       PCIe_ENGINE_CONFIG               *Engine,
  IN       UINT32                           PortStatus
  );

UINT16
PcieConfigUpdatePortStatus (
  IN       PCIe_ENGINE_CONFIG               *Engine,
  IN       PCIe_ENGINE_INIT_STATUS          SetStatus,
  IN       PCIe_ENGINE_INIT_STATUS          ResetStatus
  );

VOID
PcieConfigRunProcForAllEngines (
  IN       UINT32                           DescriptorFlags,
  IN       PCIe_RUN_ON_ENGINE_CALLBACK      Callback,
  IN OUT   VOID                             *Buffer,
  IN       PCIe_PLATFORM_CONFIG             *Pcie
  );

AGESA_STATUS
PcieConfigRunProcForAllWrappers (
  IN       UINT32                           DescriptorFlags,
  IN       PCIe_RUN_ON_WRAPPER_CALLBACK     Callback,
  IN OUT   VOID                             *Buffer,
  IN       PCIe_PLATFORM_CONFIG             *Pcie
  );

AGESA_STATUS
PcieConfigRunProcForAllDescriptors (
  IN       UINT32                           InDescriptorFlags,
  IN       UINT32                           OutDescriptorFlags,
  IN       UINT32                           TerminationFlags,
  IN       PCIe_RUN_ON_DESCRIPTOR_CALLBACK  Callback,
  IN OUT   VOID                             *Buffer,
  IN       PCIe_PLATFORM_CONFIG             *Pcie
  );

PCIe_DESCRIPTOR_HEADER *
PcieConfigGetParent (
  IN       UINT32                           Type,
  IN       PCIe_DESCRIPTOR_HEADER           *Descriptor
  );

PCIe_DESCRIPTOR_HEADER *
PcieConfigGetChild (
  IN       UINT32                           Type,
  IN       PCIe_DESCRIPTOR_HEADER           *Descriptor
  );

PCIe_DESCRIPTOR_HEADER *
PcieConfigGetPeer (
  IN       UINT32                           Type,
  IN       PCIe_DESCRIPTOR_HEADER           *Descriptor
 );

BOOLEAN
PcieConfigIsActivePcieEngine (
  IN      PCIe_ENGINE_CONFIG                *Engine
  );

PCIe_ENGINE_CONFIG *
PcieConfigLocateSbEngine (
  IN      PCIe_WRAPPER_CONFIG               *Wrapper
  );

VOID
PcieConfigDebugDump (
  IN      PCIe_PLATFORM_CONFIG              *Pcie
  );

VOID
PcieConfigWrapperDebugDump (
  IN      PCIe_WRAPPER_CONFIG               *WrapperList
  );

VOID
PcieConfigEngineDebugDump (
  IN      PCIe_ENGINE_CONFIG                *EngineList
  );

VOID
PcieUserConfigConfigDump (
  IN      PCIe_COMPLEX_DESCRIPTOR           *ComplexDescriptor
  );

VOID
PcieUserDescriptorConfigDump (
  IN      PCIe_ENGINE_DESCRIPTOR            *EngineDescriptor
  );

#define PcieConfigGetParentWrapper(Descriptor)    ((PCIe_WRAPPER_CONFIG *) PcieConfigGetParent (DESCRIPTOR_ALL_WRAPPERS, &((Descriptor)->Header)))
#define PcieConfigGetParentSilicon(Descriptor)    ((PCIe_SILICON_CONFIG *) PcieConfigGetParent (DESCRIPTOR_SILICON, &((Descriptor)->Header)))
#define PcieConfigGetParentComplex(Descriptor)    ((PCIe_COMPLEX_CONFIG *) PcieConfigGetParent (DESCRIPTOR_COMPLEX, &((Descriptor)->Header)))
#define PcieConfigGetPlatform(Descriptor)         ((PCIe_PLATFORM_CONFIG *) PcieConfigGetParent (DESCRIPTOR_PLATFORM, &((Descriptor)->Header)))
#define PcieConfigGetChildWrapper(Descriptor)     ((PCIe_WRAPPER_CONFIG *) PcieConfigGetChild (DESCRIPTOR_ALL_WRAPPERS, &((Descriptor)->Header)))
#define PcieConfigGetChildEngine(Descriptor)      ((PCIe_ENGINE_CONFIG *) PcieConfigGetChild (DESCRIPTOR_ALL_ENGINES, &((Descriptor)->Header)))
#define PcieConfigGetChildSilicon(Descriptor)     ((PCIe_SILICON_CONFIG *) PcieConfigGetChild (DESCRIPTOR_SILICON, &((Descriptor)->Header)))
#define PcieConfigGetNextDescriptor(Descriptor)   ((((Descriptor->Header.DescriptorFlags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : (++Descriptor)))
#define PcieConfigIsPcieEngine(Descriptor)        (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_PCIE_ENGINE) != 0) : FALSE)
#define PcieConfigIsDdiEngine(Descriptor)         (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_DDI_ENGINE) != 0) : FALSE)
#define PcieConfigIsPcieWrapper(Descriptor)       (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_PCIE_WRAPPER) != 0) : FALSE)
#define PcieConfigIsSbPcieEngine(Engine)          (Engine != NULL ? ((BOOLEAN) (Engine->Type.Port.PortData.MiscControls.SbLink)) : FALSE)
#define PcieConfigIsDdiWrapper(Descriptor)        (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_DDI_WRAPPER) != 0) : FALSE)
#define PcieConfigIsEngineAllocated(Descriptor)   (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_ALLOCATED) != 0) : FALSE)
#define PcieConfigIsVirtualDesciptor(Descriptor)  (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_VIRTUAL) != 0) : FALSE)
#define PcieConfigSetDescriptorFlags(Descriptor, SetDescriptorFlags)   if (Descriptor != NULL) (Descriptor)->Header.DescriptorFlags |= SetDescriptorFlags
#define PcieConfigResetDescriptorFlags(Descriptor, ResetDescriptorFlags) if (Descriptor != NULL) ((PCIe_DESCRIPTOR_HEADER *) Descriptor)->DescriptorFlags &= (~(ResetDescriptorFlags))
#define PcieInputParsetGetNextDescriptor(Descriptor) (Descriptor != NULL ? ((((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : (Descriptor+1))) : NULL)
#define PcieConfigGetNextTopologyDescriptor(Descriptor, Termination) (Descriptor != NULL ? (((((PCIe_DESCRIPTOR_HEADER *) Descriptor)->DescriptorFlags & Termination) != 0) ? NULL : ((UINT8 *) Descriptor + ((PCIe_DESCRIPTOR_HEADER *) Descriptor)->Peer)) : NULL)
#define GnbGetNextHandle(Descriptor) (GNB_HANDLE *) PcieConfigGetNextTopologyDescriptor (Descriptor, DESCRIPTOR_TERMINATE_TOPOLOGY)
#define PcieConfigGetNextDataDescriptor(Descriptor) ((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != 0 ? NULL : ++Descriptor)

#define PcieConfigGetStdHeader(Descriptor)         ((AMD_CONFIG_PARAMS *)((PCIe_PLATFORM_CONFIG *) PcieConfigGetParent (DESCRIPTOR_PLATFORM, &((Descriptor)->Header)))->StdHeader)

#endif

