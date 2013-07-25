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
#include  "heapManager.h"
#include  "OptionGnb.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "GnbFamServices.h"
#include  "cpuServices.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "PcieMapTopology.h"
#include  "PcieInputParser.h"
#include  "PcieConfigLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIECONFIG_PCIECONFIGDATA_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern BUILD_OPT_CFG UserOptions;
extern GNB_BUILD_OPTIONS ROMDATA GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

#define PcieConfigAttachChild(P, C)  (P)->Child = (UINT16) ((UINT8 *) C - (UINT8 *) P);
#define PcieConfigAttachParent(P, C) (C)->Parent = (UINT16) ((UINT8 *) C - (UINT8 *) P);

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
STATIC
PcieConfigAttachComplexes (
  IN  OUT   PCIe_COMPLEX_CONFIG             *Base,
  IN  OUT   PCIe_COMPLEX_CONFIG             *New
  );

AGESA_STATUS
PcieUpdateConfigurationData (
  IN       PCIe_PLATFORM_CONFIG             *Pcie
  );

PCIe_COMPLEX_DESCRIPTOR *
PcieConfigProcessUserConfig (
  IN       PCIe_COMPLEX_DESCRIPTOR         *PcieComplexList,
  IN       AMD_CONFIG_PARAMS               *StdHeader
  );

AGESA_STATUS
PcieConfigurationInit (
  IN       AMD_CONFIG_PARAMS               *StdHeader
  );

AGESA_STATUS
PcieConfigurationMap (
  IN       AMD_CONFIG_PARAMS               *StdHeader
  );
/*----------------------------------------------------------------------------------------*/
/**
 * Create internal PCIe configuration topology
 *
 *
 *
 * @param[in]  StdHeader       Standard configuration header
 * @retval     AGESA_SUCCESS   Configuration data successfully allocated.
 * @retval     AGESA_FATAL     Configuration data allocation failed.
 */

AGESA_STATUS
PcieConfigurationInit (
  IN       AMD_CONFIG_PARAMS               *StdHeader
  )
{

  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  PCIe_SILICON_CONFIG   *Silicon;
  UINT8                 SocketId;
  UINTN                 CurrentComplexesDataLength;
  UINTN                 ComplexesDataLength;
  UINT8                 ComplexIndex;
  VOID                  *Buffer;
  ComplexesDataLength = 0;
  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieConfigurationInit Enter\n");
  for (SocketId = 0; SocketId < GetPlatformNumberOfSockets (); SocketId++) {
    if (IsProcessorPresent (SocketId, StdHeader)) {
      Status = PcieFmGetComplexDataLength (SocketId, &CurrentComplexesDataLength, StdHeader);
      ASSERT (Status == AGESA_SUCCESS);
      ComplexesDataLength += CurrentComplexesDataLength;
    }
  }
  ComplexIndex = 0;
  Pcie = GnbAllocateHeapBufferAndClear (AMD_PCIE_COMPLEX_DATA_HANDLE, sizeof (PCIe_PLATFORM_CONFIG) + ComplexesDataLength, StdHeader);
  ASSERT (Pcie != NULL);
  if (Pcie != NULL) {
    PcieConfigAttachChild (&Pcie->Header, &Pcie->ComplexList[ComplexIndex].Header);
    PcieConfigSetDescriptorFlags (Pcie, DESCRIPTOR_PLATFORM | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_TOPOLOGY);
    Buffer = (UINT8 *) (Pcie) + sizeof (PCIe_PLATFORM_CONFIG);
    for (SocketId = 0; SocketId < GetPlatformNumberOfSockets (); SocketId++) {
      if (IsProcessorPresent (SocketId, StdHeader)) {
        Pcie->ComplexList[ComplexIndex].SocketId = SocketId;
        //Attache Comples to Silicon which will be created by PcieFmBuildComplexConfiguration
        PcieConfigAttachChild (&Pcie->ComplexList[ComplexIndex].Header, &((PCIe_SILICON_CONFIG *) Buffer)->Header);
        //Attach Comples to Pcie
        PcieConfigAttachParent (&Pcie->Header, &Pcie->ComplexList[ComplexIndex].Header);
        PcieConfigSetDescriptorFlags (&Pcie->ComplexList[ComplexIndex], DESCRIPTOR_COMPLEX | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY);
        PcieFmBuildComplexConfiguration (SocketId, Buffer, StdHeader);
        Silicon = PcieConfigGetChildSilicon (&Pcie->ComplexList[ComplexIndex]);
        while (Silicon != NULL) {
          PcieConfigAttachParent (&Pcie->ComplexList[ComplexIndex].Header, &Silicon->Header);
          GetNodeId (SocketId, Silicon->SiliconId, &Silicon->NodeId, StdHeader);
          GnbFmGetLinkId ((GNB_HANDLE*) Silicon, &Silicon->LinkId, StdHeader);
          Silicon = (PCIe_SILICON_CONFIG *) PcieConfigGetNextTopologyDescriptor (Silicon, DESCRIPTOR_TERMINATE_TOPOLOGY);
        }

        if (ComplexIndex > 0) {
          PcieConfigAttachComplexes (&Pcie->ComplexList[ComplexIndex - 1], &Pcie->ComplexList[ComplexIndex]);
        }
        PcieFmGetComplexDataLength (SocketId, &CurrentComplexesDataLength, StdHeader);
        Buffer = (VOID *) ((UINT8 *) Buffer + CurrentComplexesDataLength);
        ComplexIndex++;
      }
    }
  } else {
    Status = AGESA_FATAL;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieConfigurationInit Exit [0x%x]\n", Status);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create internal PCIe configuration topology
 *
 *
 *
 * @param[in]  StdHeader       Standard configuration header
 * @retval     AGESA_SUCCESS   Configuration data successfully allocated.
 * @retval     AGESA_FATAL     Configuration data allocation failed.
 */

AGESA_STATUS
PcieConfigurationMap (
  IN       AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AMD_EARLY_PARAMS            *EarlyParamsPtr;
  PCIe_COMPLEX_DESCRIPTOR     *PcieComplexList;
  PCIe_PLATFORM_CONFIG        *Pcie;
  PCIe_COMPLEX_CONFIG         *Complex;
  PCIe_COMPLEX_DESCRIPTOR     *ComplexDescriptor;
  AGESA_STATUS                Status;
  AGESA_STATUS                AgesaStatus;
  UINTN                       Index;
  UINTN                       NumberOfComplexes;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieConfigurationMap Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  EarlyParamsPtr = (AMD_EARLY_PARAMS *) StdHeader;
  PcieComplexList = PcieConfigProcessUserConfig (EarlyParamsPtr->GnbConfig.PcieComplexList, StdHeader);
  GNB_DEBUG_CODE (
    if (PcieComplexList != NULL) {
      PcieUserConfigConfigDump (PcieComplexList);
    }
  );
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  ASSERT (Status == AGESA_SUCCESS);
  if (Status == AGESA_SUCCESS) {
    Complex = (PCIe_COMPLEX_CONFIG *) PcieConfigGetChild (DESCRIPTOR_COMPLEX, &Pcie->Header);
    NumberOfComplexes = PcieInputParserGetNumberOfComplexes (PcieComplexList);
    while (Complex != NULL) {
      for (Index = 0; Index < NumberOfComplexes; Index++) {
        ComplexDescriptor = PcieInputParserGetComplexDescriptor (PcieComplexList, Index);
        if (ComplexDescriptor->SocketId == Complex->SocketId) {
          Status = PcieMapTopologyOnComplex (ComplexDescriptor, Complex, Pcie);
          AGESA_STATUS_UPDATE (Status, AgesaStatus);
        }
      }
      Complex = PcieLibGetNextDescriptor (Complex);
    }
  }
  Pcie->LinkReceiverDetectionPooling = GnbBuildOptions.CfgGnbLinkReceiverDetectionPooling;
  Pcie->LinkL0Pooling = GnbBuildOptions.CfgGnbLinkL0Pooling;
  Pcie->LinkGpioResetAssertionTime = GnbBuildOptions.CfgGnbLinkGpioResetAssertionTime;
  Pcie->LinkResetToTrainingTime = GnbBuildOptions.CfgGnbLinkResetToTrainingTime;
  Pcie->GfxCardWorkaround = GfxWorkaroundEnable;
  Pcie->TrainingExitState = LinkStateTrainingCompleted;
  Pcie->TrainingAlgorithm = GnbBuildOptions.CfgGnbTrainingAlgorithm;
  if ((UserOptions.CfgAmdPlatformType  & AMD_PLATFORM_MOBILE) != 0) {
    Pcie->GfxCardWorkaround = GfxWorkaroundDisable;
  }
  Pcie->PsppPolicy =  EarlyParamsPtr->GnbConfig.PsppPolicy;
  IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_PCIE_PLATFORM_CONFIG, Pcie, StdHeader);
  GNB_DEBUG_CODE (
    PcieConfigDebugDump (Pcie);
    );
  HeapDeallocateBuffer (AMD_GNB_TEMP_DATA_HANDLE, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieConfigurationMap Exit [0x%x]\n", AgesaStatus);
  return AgesaStatus;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Locate global PCIe configuration data
 *
 *
 *
 * @param[in]  PcieComplexList User PCIe topology configuration
 * @param[out] StdHeader       Standard configuration header
 * @retval     Updated topology configuration
 */
PCIe_COMPLEX_DESCRIPTOR *
PcieConfigProcessUserConfig (
  IN       PCIe_COMPLEX_DESCRIPTOR         *PcieComplexList,
  IN       AMD_CONFIG_PARAMS               *StdHeader
  )
{
  UINT32                    Node0SocketId;
  UINT32                    Node0SiliconId;
  UINTN                     NumberOfComplexes;
  UINTN                     NumberOfPorts;
  UINTN                     Index;
  UINT16                    DescriptorLoLane;
  UINT16                    DescriptorHiLane;
  PCIe_COMPLEX_DESCRIPTOR   *ResultComplexConfig;
  PCIe_COMPLEX_DESCRIPTOR   *SbComplexDescriptor;
  PCIe_PORT_DESCRIPTOR      *SbPortDescriptor;
  PCIe_PORT_DESCRIPTOR      DefaultSbPortDescriptor;
  PCIe_ENGINE_DESCRIPTOR    *EngineDescriptor;
  AGESA_STATUS              Status;
  SbPortDescriptor = NULL;
  GetSocketModuleOfNode (0, &Node0SocketId, &Node0SiliconId, StdHeader);
  Status = PcieFmGetSbConfigInfo ((UINT8) Node0SocketId, &DefaultSbPortDescriptor, StdHeader);
  if (Status == AGESA_UNSUPPORTED) {
    return PcieComplexList;
  }
  if (PcieComplexList == NULL) {
    // No complex descriptor for any silicon was provided
    // 1. Create complex descriptor
    // 2. Create SB port descriptor
    // 3. Attach SB descriptor to complex descriptor created in step #1
    ResultComplexConfig = (PCIe_COMPLEX_DESCRIPTOR *) GnbAllocateHeapBufferAndClear (
                                                        AMD_GNB_TEMP_DATA_HANDLE,
                                                        sizeof (PCIe_COMPLEX_DESCRIPTOR) + sizeof (PCIe_PORT_DESCRIPTOR),
                                                        StdHeader
                                                        );
    SbComplexDescriptor = ResultComplexConfig;
    SbPortDescriptor = (PCIe_PORT_DESCRIPTOR *) ((UINT8 *) ResultComplexConfig + sizeof (PCIe_COMPLEX_DESCRIPTOR));
    LibAmdMemCopy (SbPortDescriptor, &DefaultSbPortDescriptor, sizeof (PCIe_PORT_DESCRIPTOR), StdHeader);
    SbPortDescriptor->Flags |= DESCRIPTOR_TERMINATE_LIST;
    // Attach post array to complex descriptor
    SbComplexDescriptor->PciePortList = SbPortDescriptor;
    SbComplexDescriptor->SocketId = Node0SocketId;
    SbComplexDescriptor->Flags |= DESCRIPTOR_TERMINATE_LIST;
  } else {
    NumberOfComplexes = PcieInputParserGetNumberOfComplexes (PcieComplexList);
    SbComplexDescriptor = PcieInputParserGetComplexDescriptorOfSocket (PcieComplexList, Node0SocketId);
    if (SbComplexDescriptor == NULL) {
      // No complex descriptor for silicon that have SB attached.
      // 1. Create complex descriptor. Will be first one in the list
      // 2. Create SB port descriptor
      // 3. Attach SB descriptor to complex descriptor created in step #1
      ResultComplexConfig = (PCIe_COMPLEX_DESCRIPTOR *) GnbAllocateHeapBufferAndClear (
                                                          AMD_GNB_TEMP_DATA_HANDLE,
                                                          (NumberOfComplexes + 1) * sizeof (PCIe_COMPLEX_DESCRIPTOR) + sizeof (PCIe_PORT_DESCRIPTOR),
                                                          StdHeader
                                                          );
      SbComplexDescriptor = ResultComplexConfig;
      SbPortDescriptor = (PCIe_PORT_DESCRIPTOR *) ((UINT8 *) ResultComplexConfig + (NumberOfComplexes + 1) * sizeof (PCIe_COMPLEX_DESCRIPTOR));
      LibAmdMemCopy (SbPortDescriptor, &DefaultSbPortDescriptor, sizeof (PCIe_PORT_DESCRIPTOR), StdHeader);
      SbPortDescriptor->Flags |= DESCRIPTOR_TERMINATE_LIST;
      // Attach post array to complex descriptor
      SbComplexDescriptor->PciePortList = SbPortDescriptor;
      SbComplexDescriptor->SocketId = Node0SocketId;
      SbComplexDescriptor->Flags |= DESCRIPTOR_TERMINATE_LIST;
      LibAmdMemCopy (
        (UINT8 *) ResultComplexConfig + sizeof (PCIe_COMPLEX_DESCRIPTOR),
        PcieComplexList,
        NumberOfComplexes * sizeof (PCIe_COMPLEX_DESCRIPTOR),
        StdHeader
        );

    } else {
      // Complex descriptor that represent silicon that have SB attached exist
      // 1. Determine if complex have descriptor for SB
      // 2. Create new descriptor for SB if needed
      NumberOfPorts = PcieInputParserGetLengthOfPcieEnginesList (SbComplexDescriptor);
      ResultComplexConfig = (PCIe_COMPLEX_DESCRIPTOR *) GnbAllocateHeapBuffer (
                                                          AMD_GNB_TEMP_DATA_HANDLE,
                                                          NumberOfComplexes * sizeof (PCIe_COMPLEX_DESCRIPTOR) + (NumberOfPorts + 1) * sizeof (PCIe_PORT_DESCRIPTOR),
                                                          StdHeader
                                                          );
      // Copy complex descriptor array
      LibAmdMemCopy (
        ResultComplexConfig,
        PcieComplexList,
        NumberOfComplexes * sizeof (PCIe_COMPLEX_DESCRIPTOR),
        StdHeader
        );
      if (NumberOfPorts != 0) {
        // Copy port descriptor array associated with complex with SB attached
        LibAmdMemCopy (
          (UINT8*) ResultComplexConfig + NumberOfComplexes * sizeof (PCIe_COMPLEX_DESCRIPTOR) + sizeof (PCIe_PORT_DESCRIPTOR),
          SbComplexDescriptor->PciePortList,
          NumberOfPorts * sizeof (PCIe_PORT_DESCRIPTOR),
          StdHeader
          );
        // Update SB complex pointer on in memory list
        SbComplexDescriptor = PcieInputParserGetComplexDescriptorOfSocket ((PCIe_COMPLEX_DESCRIPTOR *) ResultComplexConfig, Node0SocketId);
        // Attach port descriptor array to complex
        SbComplexDescriptor->PciePortList = (PCIe_PORT_DESCRIPTOR *) ((UINT8*) ResultComplexConfig + NumberOfComplexes * sizeof (PCIe_COMPLEX_DESCRIPTOR) + sizeof (PCIe_PORT_DESCRIPTOR));
        for (Index = 0; Index < NumberOfPorts; ++Index) {
          EngineDescriptor = PcieInputParserGetEngineDescriptor (SbComplexDescriptor, Index);
          if (EngineDescriptor->EngineData.EngineType == PciePortEngine) {
            DescriptorLoLane = MIN (EngineDescriptor->EngineData.StartLane, EngineDescriptor->EngineData.EndLane);
            DescriptorHiLane = MAX (EngineDescriptor->EngineData.StartLane, EngineDescriptor->EngineData.EndLane);
            if (DescriptorLoLane >= DefaultSbPortDescriptor.EngineData.StartLane && DescriptorLoLane <= DefaultSbPortDescriptor.EngineData.EndLane) {
              SbPortDescriptor = (PCIe_PORT_DESCRIPTOR *) EngineDescriptor;
            }
          }
        }
      }
      if (SbPortDescriptor == NULL) {
        // No descriptor that represent SB where found, create new one, will be first one in list
        SbPortDescriptor = (PCIe_PORT_DESCRIPTOR *) ((UINT8*) ResultComplexConfig + NumberOfComplexes * sizeof (PCIe_COMPLEX_DESCRIPTOR));
        // Copy default config info
        LibAmdMemCopy (SbPortDescriptor, &DefaultSbPortDescriptor, sizeof (PCIe_PORT_DESCRIPTOR), StdHeader);
        // Reattach descriptor list to complex
        SbComplexDescriptor->PciePortList = SbPortDescriptor;
      } else {
        // Move SB descriptor to be first one in array
        LibAmdMemCopy (
          (UINT8*) ResultComplexConfig + NumberOfComplexes * sizeof (PCIe_COMPLEX_DESCRIPTOR),
          SbPortDescriptor,
          sizeof (PCIe_PORT_DESCRIPTOR),
          StdHeader
          );
        // Disable original SB descriptor
        SbPortDescriptor->EngineData.EngineType = PcieUnusedEngine;
        //Update pointer to new SB descriptor
        SbPortDescriptor = (PCIe_PORT_DESCRIPTOR *) ((UINT8*) ResultComplexConfig + NumberOfComplexes * sizeof (PCIe_COMPLEX_DESCRIPTOR));
        //It is no longer a descriptor that terminates list
        SbPortDescriptor->Flags &= (~ DESCRIPTOR_TERMINATE_LIST);
        // Reattach descriptor list to complex
        SbComplexDescriptor->PciePortList = SbPortDescriptor;
      }
    }
  }
  // Mark descriptor as SB link
  SbPortDescriptor->Port.MiscControls.SbLink = 0x1;
  return ResultComplexConfig;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Locate global PCIe configuration data
 *
 *
 *
 * @param[in]  StdHeader       Standard configuration header
 * @param[out] Pcie            Pointer to global PCIe configuration
 * @retval     AGESA_SUCCESS   Configuration data successfully located
 * @retval     AGESA_FATAL     Configuration can not be located.
 */
AGESA_STATUS
PcieLocateConfigurationData (
  IN       AMD_CONFIG_PARAMS               *StdHeader,
     OUT   PCIe_PLATFORM_CONFIG            **Pcie
  )
{
  *Pcie = GnbLocateHeapBuffer (AMD_PCIE_COMPLEX_DATA_HANDLE, StdHeader);
  if (*Pcie == NULL) {
    IDS_ERROR_TRAP;
    return AGESA_FATAL;
  }
  (*Pcie)->StdHeader = (PVOID) StdHeader;
  PcieUpdateConfigurationData (*Pcie);
  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Attache descriptors
 *
 *
 * @param[in]      Type       Descriptor type
 * @param[in,out]  Base       Base descriptor
 * @param[in,out]  New      New  descriptor
 */
VOID
STATIC
PcieConfigAttachDescriptors (
  IN       UINT32                         Type,
  IN OUT   PCIe_DESCRIPTOR_HEADER         *Base,
  IN OUT   PCIe_DESCRIPTOR_HEADER         *New
  )
{
  PCIe_DESCRIPTOR_HEADER  *Left;
  PCIe_DESCRIPTOR_HEADER  *Right;

  Left = PcieConfigGetPeer (DESCRIPTOR_TERMINATE_GNB, PcieConfigGetChild (Type, Base));
  ASSERT (Left != NULL);
  Right = PcieConfigGetChild (Type, New);
  Left->Peer = (UINT16) ((UINT8 *) Right - (UINT8 *) Left);
  PcieConfigResetDescriptorFlags (Left, DESCRIPTOR_TERMINATE_TOPOLOGY);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Attach configurations of two GNB to each other.
 *
 * Function will link all data structure to linked lists
 *
 * @param[in,out]  Base       Base complex descriptor
 * @param[in,out]  New        New complex descriptor
 */
VOID
STATIC
PcieConfigAttachComplexes (
  IN OUT   PCIe_COMPLEX_CONFIG            *Base,
  IN OUT   PCIe_COMPLEX_CONFIG            *New
  )
{
  // Connect Complex
  Base->Header.Peer = (UINT16) ((UINT8 *) New - (UINT8 *) Base);
  PcieConfigResetDescriptorFlags (Base, DESCRIPTOR_TERMINATE_TOPOLOGY);
  // Connect Silicon
  PcieConfigAttachDescriptors (DESCRIPTOR_SILICON, &Base->Header, &New->Header);
  // Connect Wrappers
  PcieConfigAttachDescriptors (DESCRIPTOR_PCIE_WRAPPER | DESCRIPTOR_DDI_WRAPPER, &Base->Header, &New->Header);
  // Connect Engines
  PcieConfigAttachDescriptors (DESCRIPTOR_PCIE_ENGINE | DESCRIPTOR_DDI_ENGINE, &Base->Header, &New->Header);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Update configuration data
 *
 * Puprouse of this structure to update config data that base on programming of
 * other silicon compoments. For instance PCI address of GNB and PCIe ports
 * can change by AGESA or external agent
 *
 *
 * @param[in,out]   Pcie        Pointer to global PCIe configuration
 * @retval     AGESA_SUCCESS    Configuration data successfully update
 * @retval     AGESA_FATAL      Failt to update configuration
 */
AGESA_STATUS
PcieUpdateConfigurationData (
  IN       PCIe_PLATFORM_CONFIG            *Pcie
  )
{
  PCIe_SILICON_CONFIG   *Silicon;
  PCIe_ENGINE_CONFIG    *Engine;
  PCI_ADDR              NewAddress;
  // Update silicon configuration
  Silicon = PcieConfigGetChildSilicon (Pcie);
  while (Silicon != NULL) {
    NewAddress = GnbFmGetPciAddress ((GNB_HANDLE *) PcieConfigGetParentComplex (Silicon), GnbLibGetHeader (Pcie));
    if (Silicon->Address.AddressValue != NewAddress.AddressValue) {
      Silicon->Address.AddressValue = NewAddress.AddressValue;
      Engine = PcieConfigGetChildEngine (Silicon);
      while (Engine != NULL) {
        if (PcieConfigIsPcieEngine (Engine)) {
          Engine->Type.Port.Address.Address.Bus = Silicon->Address.Address.Bus;
        }
        Engine = (PCIe_ENGINE_CONFIG *) PcieConfigGetNextTopologyDescriptor (Engine, DESCRIPTOR_TERMINATE_GNB);
      }
    }
    Silicon = (PCIe_SILICON_CONFIG *) PcieConfigGetNextTopologyDescriptor (Silicon, DESCRIPTOR_TERMINATE_TOPOLOGY);
  }
  return  AGESA_SUCCESS;
}
