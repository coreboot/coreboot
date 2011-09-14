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
#include  "heapManager.h"
#include  "OptionGnb.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  GNB_MODULE_DEFINITIONS (GnbPcieConfig)
#include  "PcieConfigData.h"
#include  "PcieMapTopology.h"
#include  "PcieInputParser.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIECONFIG_PCIECONFIGDATA_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


#define REBASE_PTR( Ptr, OldBase, NewBase)  *(UINTN *)Ptr = (*(UINTN *)Ptr + (UINTN) NewBase - (UINTN) OldBase);

extern BUILD_OPT_CFG UserOptions;
extern GNB_BUILD_OPTIONS ROMDATA GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
PcieConfigDebugDump (
  IN      PCIe_PLATFORM_CONFIG        *Pcie
  );


/*----------------------------------------------------------------------------------------*/
/**
 * Create internal PCIe configuration data
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
  AMD_EARLY_PARAMS              *EarlyParamsPtr;
  PCIe_COMPLEX_DESCRIPTOR       *ComplexList;
  PCIe_PLATFORM_CONFIG          *Pcie;
  AGESA_STATUS                  AgesaStatus;
  AGESA_STATUS                  Status;
  PCIe_COMPLEX_DESCRIPTOR       *ComplexDescriptor;
  UINTN                         ComplexesDataLength;
  UINTN                         ComplexIndex;
  UINTN                         NumberOfComplexes;
  VOID                          *Buffer;
  UINTN                         Index;
  UINT32                        NumberOfSockets;
  UINT32                        SocketId;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieConfigurationInit Enter\n");
  EarlyParamsPtr = (AMD_EARLY_PARAMS *) StdHeader;
  ComplexList = EarlyParamsPtr->GnbConfig.PcieComplexList;
  AgesaStatus = AGESA_SUCCESS;
  ComplexesDataLength = 0;
  NumberOfSockets = GnbGetNumberOfSockets (StdHeader);
  for (SocketId = 0; SocketId <  NumberOfSockets; SocketId++) {
    if (GnbIsDevicePresentInSocket (SocketId, StdHeader)) {
      UINTN   CurrentComplexesDataLength;
      Status = PcieFmGetComplexDataLength (SocketId, &CurrentComplexesDataLength);
      ASSERT (Status == AGESA_SUCCESS);
      ComplexesDataLength += CurrentComplexesDataLength;
    }
  }
  NumberOfComplexes = PcieInputParserGetNumberOfComplexes (ComplexList);
  Pcie = GnbAllocateHeapBuffer (AMD_PCIE_COMPLEX_DATA_HANDLE, sizeof (PCIe_PLATFORM_CONFIG) + ComplexesDataLength, StdHeader);
  if (Pcie == NULL) {
    IDS_ERROR_TRAP;
    return AGESA_FATAL;
  }
  LibAmdMemFill (Pcie, 0x00, sizeof (PCIe_PLATFORM_CONFIG) + ComplexesDataLength, StdHeader);
  Pcie->StdHeader = StdHeader;
  Pcie->This = (UINTN) (Pcie);
  Buffer = (UINT8 *) (Pcie) + sizeof (PCIe_PLATFORM_CONFIG);
  ComplexIndex = 0;
  for (SocketId = 0; SocketId <  NumberOfSockets; SocketId++) {
    if (GnbIsDevicePresentInSocket (SocketId, StdHeader)) {
      UINTN CurrentComplexesDataLength;
      if (ComplexIndex > MAX_NUMBER_OF_COMPLEXES) {
        IDS_ERROR_TRAP;
        return AGESA_FATAL;
      }
      Pcie->ComplexList[ComplexIndex].SiliconList = (PCIe_SILICON_CONFIG *) Buffer;
      PcieFmBuildComplexConfiguration (Buffer, StdHeader);
      for (Index = 0; Index < NumberOfComplexes; Index++) {
        ComplexDescriptor = PcieInputParserGetComplexDescriptor (ComplexList, Index);
        if (ComplexDescriptor->SocketId == SocketId) {
          Status = PcieMapTopologyOnComplex (ComplexDescriptor, &Pcie->ComplexList[Index], Pcie);
          AGESA_STATUS_UPDATE (Status, AgesaStatus);
        }
      }
      PcieFmGetComplexDataLength (SocketId, &CurrentComplexesDataLength);
      Buffer = (VOID *) ((UINT8 *)Buffer + CurrentComplexesDataLength);
      ComplexIndex++;
    }
  }
  Pcie->ComplexList[ComplexIndex - 1].Flags |= DESCRIPTOR_TERMINATE_LIST;
  Pcie->LinkReceiverDetectionPooling = GnbBuildOptions.LinkReceiverDetectionPooling;
  Pcie->LinkL0Pooling = GnbBuildOptions.LinkL0Pooling;
  Pcie->LinkGpioResetAssertionTime = GnbBuildOptions.LinkGpioResetAssertionTime;
  Pcie->LinkResetToTrainingTime = GnbBuildOptions.LinkResetToTrainingTime;
  Pcie->GfxCardWorkaround = GfxWorkaroundEnable;
  Pcie->TrainingExitState = LinkStateTrainingCompleted;
  Pcie->TrainingAlgorithm = GnbBuildOptions.TrainingAlgorithm;
  if ((UserOptions.CfgAmdPlatformType  & AMD_PLATFORM_MOBILE) != 0) {
    Pcie->GfxCardWorkaround = GfxWorkaroundDisable;
  }
  Pcie->PsppPolicy =  EarlyParamsPtr->GnbConfig.PsppPolicy;
  IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_PCIE_PLATFORM_CONFIG, Pcie, StdHeader);
  GNB_DEBUG_CODE (
    PcieConfigDebugDump (Pcie);
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieConfigurationInit Exit [0x%x]\n", AgesaStatus);
  return AgesaStatus;
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
  PCIe_COMPLEX_CONFIG   *Complex;
  *Pcie = GnbLocateHeapBuffer (AMD_PCIE_COMPLEX_DATA_HANDLE, StdHeader);
  if (*Pcie == NULL) {
    IDS_ERROR_TRAP;
    return AGESA_FATAL;
  }
  if ((UINTN) (*Pcie) != (UINTN) (*Pcie)->This) {
    Complex = &(*Pcie)->ComplexList[0];
    while (Complex != NULL) {
      PCIe_SILICON_CONFIG  *SiliconList;
      REBASE_PTR (&Complex->SiliconList, (UINTN) (*Pcie)->This, (UINTN)*Pcie);
      SiliconList = PcieComplexGetSiliconList (Complex);
      PcieRebaseConfigurationData (SiliconList, (UINTN) (*Pcie)->This, (UINTN)*Pcie);
      Complex = PcieLibGetNextDescriptor (Complex);
    }
    (*Pcie)->This = (UINTN)(*Pcie);
  }
  (*Pcie)->StdHeader = StdHeader;
  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Rebase all pointers in Complex Configuration Data
 *
 *
 *
 * @param[in]     SiliconList   Pointer to first silicon descriptor of the complex
 * @param[in]     OldBase       Old base address of the configuration data
 * @param[in]     NewBase       New base address of the configuration data
 */
VOID
PcieRebaseConfigurationData (
  IN      PCIe_SILICON_CONFIG             *SiliconList,
  IN      UINTN                           OldBase,
  IN      UINTN                           NewBase
  )
{
  while (SiliconList != NULL) {
    PCIe_WRAPPER_CONFIG *WrapperList;
    REBASE_PTR (&SiliconList->WrapperList, OldBase, NewBase);
    REBASE_PTR (&SiliconList->FmSilicon, OldBase, NewBase);
    WrapperList = PcieSiliconGetWrapperList (SiliconList);
    while (WrapperList != NULL) {
      PCIe_ENGINE_CONFIG *EngineList;
      REBASE_PTR (&WrapperList->EngineList, OldBase, NewBase);
      REBASE_PTR (&WrapperList->FmWrapper, OldBase, NewBase);
      REBASE_PTR (&WrapperList->Silicon, OldBase, NewBase);
      EngineList = PcieWrapperGetEngineList (WrapperList);
      while (EngineList != NULL) {
        REBASE_PTR (&EngineList->Wrapper, OldBase, NewBase);
        EngineList = PcieLibGetNextDescriptor (EngineList);
      }
      WrapperList = PcieLibGetNextDescriptor (WrapperList);
    }
    SiliconList = PcieLibGetNextDescriptor (SiliconList);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump configuration to debug out
 *
 *
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PcieConfigDebugDump (
  IN      PCIe_PLATFORM_CONFIG        *Pcie
  )
{
  PCIe_ENGINE_CONFIG    *EngineList;
  PCIe_SILICON_CONFIG   *SiliconList;
  PCIe_WRAPPER_CONFIG   *WrapperList;
  PCIe_COMPLEX_CONFIG   *ComplexList;
  ComplexList = &Pcie->ComplexList[0];
  IDS_HDT_CONSOLE (PCIE_MISC, "<-------------- PCIe Config Start----------------->\n");
  IDS_HDT_CONSOLE (PCIE_MISC, "  PSPP Policy - %s\n",
   (Pcie->PsppPolicy == PsppPowerSaving) ? "Power Saving" :
   (Pcie->PsppPolicy == PsppBalanceHigh) ? "Balance-High" : (
   (Pcie->PsppPolicy == PsppBalanceLow) ? "Balance-Low" : (
   (Pcie->PsppPolicy == PsppPerformance) ? "Performance" : (
   (Pcie->PsppPolicy == PsppDisabled) ? "Disabled" : "Unknown")))
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "  GFX Workaround - %s\n",
   (Pcie->GfxCardWorkaround == 0) ? "Disabled" : "Enabled"
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "  LinkL0Pooling - %dus\n",
   Pcie->LinkL0Pooling
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "  LinkGpioResetAssertionTime - %dus\n",
   Pcie->LinkGpioResetAssertionTime
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "  LinkReceiverDetectionPooling - %dus\n",
   Pcie->LinkReceiverDetectionPooling
    );
  SiliconList = PcieComplexGetSiliconList (ComplexList);
  while (SiliconList != NULL) {
    WrapperList = PcieSiliconGetWrapperList (SiliconList);
    while (WrapperList != NULL) {
      IDS_HDT_CONSOLE (PCIE_MISC, "  <---------Wrapper - %s Config -------->\n",
        PcieFmDebugGetWrapperNameString (WrapperList)
        );
      IDS_HDT_CONSOLE (PCIE_MISC, "    PowerOffUnusedLanes - %x\n    PowerOffUnusedPlls - %x\n    ClkGating - %x\n"
                                  "    LclkGating - %x\n    TxclkGatingPllPowerDown - %x\n    PllOffInL1 - %x\n",
        WrapperList->Features.PowerOffUnusedLanes,
        WrapperList->Features.PowerOffUnusedPlls,
        WrapperList->Features.ClkGating,
        WrapperList->Features.LclkGating,
        WrapperList->Features.TxclkGatingPllPowerDown,
        WrapperList->Features.PllOffInL1
        );
      IDS_HDT_CONSOLE (PCIE_MISC, "  <---------Wrapper - %s Config End----->\n",
        PcieFmDebugGetWrapperNameString (WrapperList)
        );
      EngineList = PcieWrapperGetEngineList (WrapperList);
      while (EngineList != NULL) {
        if (PcieLibIsEngineAllocated (EngineList)) {
          IDS_HDT_CONSOLE (PCIE_MISC, "  Engine Type - %s\n    Start Phy Lane - %d\n    End   Phy Lane - %d\n",
            ((EngineList->EngineData.EngineType == PciePortEngine) ? "PCIe Port" : "DDI Link"),
            EngineList->EngineData.StartLane,
            EngineList->EngineData.EndLane
            );
          if (PcieLibIsPcieEngine (EngineList)) {
            IDS_HDT_CONSOLE (PCIE_MISC, "    PCIe port configuration:\n");
            IDS_HDT_CONSOLE (PCIE_MISC, "      Port Training - %s\n",
              (EngineList->Type.Port.PortData.PortPresent == PortDisabled) ? "Disable" : "Enabled"
              );
            IDS_HDT_CONSOLE (PCIE_MISC, "      Start Core Lane - %d\n", EngineList->Type.Port.StartCoreLane);
            IDS_HDT_CONSOLE (PCIE_MISC, "      End Core Lane   - %d\n", EngineList->Type.Port.EndCoreLane);
            IDS_HDT_CONSOLE (PCIE_MISC, "      PCI Address - %d:%d:%d\n",
              EngineList->Type.Port.Address.Address.Bus,
              EngineList->Type.Port.Address.Address.Device,
              EngineList->Type.Port.Address.Address.Function
              );
            IDS_HDT_CONSOLE (PCIE_MISC, "      Native PCI Dev Number  - %d\n", EngineList->Type.Port.NativeDevNumber);
            IDS_HDT_CONSOLE (PCIE_MISC, "      Native PCI Func Number - %d\n", EngineList->Type.Port.NativeFunNumber);
            IDS_HDT_CONSOLE (PCIE_MISC, "      Hotplug - %s\n",
              (EngineList->Type.Port.PortData.LinkHotplug == 0) ? "Disabled" : (
              (EngineList->Type.Port.PortData.LinkHotplug == 1) ? "Basic" : (
              (EngineList->Type.Port.PortData.LinkHotplug == 2) ? "Server" : (
              (EngineList->Type.Port.PortData.LinkHotplug == 2) ? "Enhanced" : "Unknown")))
              );
            IDS_HDT_CONSOLE (PCIE_MISC, "      ASPM    - %s\n",
              (EngineList->Type.Port.PortData.LinkAspm == 0) ? "Disabled" : (
              (EngineList->Type.Port.PortData.LinkAspm == 1) ? "L0s" : (
              (EngineList->Type.Port.PortData.LinkAspm == 2) ? "L1" :  (
              (EngineList->Type.Port.PortData.LinkAspm == 3) ? "L0s & L1" : "Unknown")))
              );
            IDS_HDT_CONSOLE (PCIE_MISC, "      Speed   - %d\n",
              EngineList->Type.Port.PortData.LinkSpeedCapability
              );
          } else {
            IDS_HDT_CONSOLE (PCIE_MISC, "    DDI configuration:\n");
            IDS_HDT_CONSOLE (PCIE_MISC, "      Connector - %s\n",
              (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeDP) ? "DP" : (
              (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeEDP) ? "eDP" : (
              (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeSingleLinkDVI) ? "Single Link DVI" : (
              (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeDualLinkDVI) ? "Dual Link DVI" : (
              (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeHDMI) ? "HDMI" : (
              (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeTravisDpToVga) ? "Travis DP-to-VGA" : (
              (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeTravisDpToLvds) ? "Travis DP-to-LVDS" : (
              (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeLvds) ? "LVDS" : (
              (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeNutmegDpToVga) ? "Hudson-2 Nutmeg DP-to-VGA" : "Unknown"))))))))
              );
            IDS_HDT_CONSOLE (PCIE_MISC, "      Aux - Aux%d\n", EngineList->Type.Ddi.DdiData.AuxIndex + 1);
            IDS_HDT_CONSOLE (PCIE_MISC, "      Hdp - Hdp%d\n", EngineList->Type.Ddi.DdiData.HdpIndex + 1);
          }
        }
        EngineList = PcieLibGetNextDescriptor (EngineList);
      }
      WrapperList = PcieLibGetNextDescriptor (WrapperList);
    }
    SiliconList = PcieLibGetNextDescriptor (SiliconList);
  }
  IDS_HDT_CONSOLE (PCIE_MISC, "<-------------- PCIe Config End------------------>\n");
}