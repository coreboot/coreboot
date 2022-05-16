/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe component definitions.
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

#ifndef _GNBPCIE_H_
#define _GNBPCIE_H_

#pragma pack (push, 1)

#define MAX_NUMBER_OF_COMPLEXES                4

#define DESCRIPTOR_TERMINATE_GNB               0x40000000ull
#define DESCRIPTOR_TERMINATE_TOPOLOGY          0x20000000ull
#define DESCRIPTOR_ALLOCATED                   0x10000000ull
#define DESCRIPTOR_VIRTUAL                     0x08000000ull
#define DESCRIPTOR_PLATFORM                    0x04000000ull
#define DESCRIPTOR_COMPLEX                     0x02000000ull
#define DESCRIPTOR_SILICON                     0x01000000ull
#define DESCRIPTOR_PCIE_WRAPPER                0x00800000ull
#define DESCRIPTOR_DDI_WRAPPER                 0x00400000ull
#define DESCRIPTOR_PCIE_ENGINE                 0x00200000ull
#define DESCRIPTOR_DDI_ENGINE                  0x00100000ull

#define DESCRIPTOR_ALL_WRAPPERS                (DESCRIPTOR_DDI_WRAPPER | DESCRIPTOR_PCIE_WRAPPER)
#define DESCRIPTOR_ALL_ENGINES                 (DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_PCIE_ENGINE)

#define DESCRIPTOR_ALL_TYPES                   (DESCRIPTOR_ALL_WRAPPERS | DESCRIPTOR_ALL_ENGINES | DESCRIPTOR_SILICON | DESCRIPTOR_PLATFORM)

#define UNUSED_LANE_ID                         128
//#define PCIE_LINK_RECEIVER_DETECTION_POOLING   (60 * 1000)
//#define PCIE_LINK_L0_POOLING                   (60 * 1000)
//#define PCIE_LINK_GPIO_RESET_ASSERT_TIME       (2  * 1000)
//#define PCIE_LINK_RESET_TO_TRAINING_TIME       (2  * 1000)

// Get lowest PHY lane on engine
#define PcieLibGetLoPhyLane(Engine) (Engine != NULL ? ((Engine->EngineData.StartLane > Engine->EngineData.EndLane) ? Engine->EngineData.EndLane : Engine->EngineData.StartLane) : 0)
// Get highest PHY lane on engine
#define PcieLibGetHiPhyLane(Engine) (Engine != NULL ? ((Engine->EngineData.StartLane > Engine->EngineData.EndLane) ? Engine->EngineData.StartLane : Engine->EngineData.EndLane) : 0)
// Get number of lanes on wrapper
#define PcieLibWrapperNumberOfLanes(Wrapper) (Wrapper != NULL ? ((UINT8)(Wrapper->EndPhyLane - Wrapper->StartPhyLane + 1)) : 0)
// Check if virtual descriptor
#define PcieLibIsVirtualDesciptor(Descriptor) (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_VIRTUAL) != 0) : FALSE)
// Check if it is allocated descriptor
#define PcieLibIsEngineAllocated(Descriptor) (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_ALLOCATED) != 0) : FALSE)
// Check if it is last descriptor in list
#define PcieLibIsLastDescriptor(Descriptor) (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_TERMINATE_LIST) != 0) : TRUE)
// Check if descriptor a PCIe engine
#define PcieLibIsPcieEngine(Descriptor) (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_PCIE_ENGINE) != 0) : FALSE)
// Check if descriptor a DDI engine
#define PcieLibIsDdiEngine(Descriptor) (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_DDI_ENGINE) != 0) : FALSE)
// Check if descriptor a DDI wrapper
#define PcieLibIsDdiWrapper(Descriptor) (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_DDI_WRAPPER) != 0) : FALSE)
// Check if descriptor a PCIe wrapper
#define PcieLibIsPcieWrapper(Descriptor) (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_PCIE_WRAPPER) != 0) : FALSE)
// Check if descriptor a PCIe wrapper
#define PcieLibGetNextDescriptor(Descriptor) (Descriptor != NULL ? (((Descriptor->Header.DescriptorFlags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : (Descriptor+1)) : NULL)

#define LANE_TYPE_PCIE_CORE_CONFIG              0x00000001ul
#define LANE_TYPE_PCIE_CORE_ALLOC               0x00000002ul
#define LANE_TYPE_PCIE_CORE_ACTIVE              0x00000004ul
#define LANE_TYPE_PCIE_SB_CORE_CONFIG           0x00000008ul
#define LANE_TYPE_PCIE_CORE_HOTPLUG             0x00000010ul
#define LANE_TYPE_PCIE_CORE_ALLOC_ACTIVE        0x00000020ul
#define LANE_TYPE_PCIE_PHY                      0x00000100ul
#define LANE_TYPE_PCIE_PHY_NATIVE               0x00000200ul
#define LANE_TYPE_PCIE_PHY_NATIVE_ACTIVE        0x00000400ul
#define LANE_TYPE_PCIE_PHY_NATIVE_HOTPLUG       0x00000800ul
#define LANE_TYPE_PCIE_PHY_NATIVE_ALLOC_ACTIVE  0x00001000ul
#define LANE_TYPE_DDI_PHY                       0x00010000ul
#define LANE_TYPE_DDI_PHY_NATIVE                0x00020000ul
#define LANE_TYPE_DDI_PHY_NATIVE_ACTIVE         0x00040000ul
#define LANE_TYPE_PHY_NATIVE_ALL                0x00100000ul
#define LANE_TYPE_PCIE_PHY_NATIVE_MASTER_PLL    0x00200000ul
#define LANE_TYPE_CORE_ALL                      LANE_TYPE_PHY_NATIVE_ALL
#define LANE_TYPE_ALL                           LANE_TYPE_PHY_NATIVE_ALL

#define LANE_TYPE_PCIE_LANES (LANE_TYPE_PCIE_CORE_ACTIVE | LANE_TYPE_PCIE_SB_CORE_CONFIG | \
                              LANE_TYPE_PCIE_CORE_HOTPLUG | LANE_TYPE_PCIE_CORE_ALLOC | \
                              LANE_TYPE_PCIE_PHY | LANE_TYPE_PCIE_PHY_NATIVE | \
                              LANE_TYPE_PCIE_PHY_NATIVE_ACTIVE | LANE_TYPE_PCIE_PHY_NATIVE_HOTPLUG | \
                              LANE_TYPE_PCIE_CORE_CONFIG | LANE_TYPE_PCIE_PHY_NATIVE_ALLOC_ACTIVE | \
                              LANE_TYPE_PCIE_CORE_ALLOC_ACTIVE)

#define LANE_TYPE_DDI_LANES (LANE_TYPE_DDI_PHY | LANE_TYPE_DDI_PHY_NATIVE | LANE_TYPE_DDI_PHY_NATIVE_ACTIVE)


#define INIT_STATUS_PCIE_PORT_GEN2_RECOVERY          0x00000001ull
#define INIT_STATUS_PCIE_PORT_BROKEN_LANE_RECOVERY   0x00000002ull
#define INIT_STATUS_PCIE_PORT_TRAINING_FAIL          0x00000004ull
#define INIT_STATUS_PCIE_TRAINING_SUCCESS            0x00000008ull
#define INIT_STATUS_PCIE_EP_NOT_PRESENT              0x00000010ull
#define INIT_STATUS_PCIE_PORT_IN_COMPLIANCE          0x00000020ull
#define INIT_STATUS_DDI_ACTIVE                       0x00000040ull
#define INIT_STATUS_ALLOCATED                        0x00000080ull

#define PCIE_PORT_GEN_CAP_BOOT                       0x00000001ul
#define PCIE_PORT_GEN_CAP_MAX                        0x00000002ul
#define PCIE_GLOBAL_GEN_CAP_ALL_PORTS                0x00000010ul
#define PCIE_GLOBAL_GEN_CAP_TRAINED_PORTS            0x00000011ul
#define PCIE_GLOBAL_GEN_CAP_HOTPLUG_PORTS            0x00000012ul

#define PCIE_POWERGATING_SKIP_CORE                   0x00000001ul
#define PCIE_POWERGATING_SKIP_PHY                    0x00000002ul

/// PCIe Link Training State
typedef enum {
  PcieTrainingStandard,                                ///< Standard training algorithm. Training contained to AmdEarlyInit.
                                                       ///< PCIe device accessible after AmdEarlyInit complete
  PcieTrainingDistributed,                             ///< Distribute training algorithm. Training distributed across AmdEarlyInit/AmdPostInit/AmdS3LateRestore
                                                       ///< PCIe device accessible after AmdPostInit complete.
                                                       ///< Algorithm potentially save up to 60ms in S3 resume time by skipping training empty slots.
} PCIE_TRAINING_ALGORITHM;

/// PCIe Link Training State
typedef enum {
  LinkStateResetAssert,                                 ///< Assert port GPIO reset
  LinkStateResetDuration,                               ///< Timeout for reset duration
  LinkStateResetExit,                                   ///< Deassert port GPIO reset
  LinkTrainingResetTimeout,                             ///< Port GPIO reset timeout
  LinkStateReleaseTraining,                             ///< Release link training
  LinkStateDetectPresence,                              ///< Detect device presence
  LinkStateDetecting,                                   ///< Detect link training.
  LinkStateBrokenLane,                                  ///< Check and handle broken lane
  LinkStateGen2Fail,                                    ///< Check and handle device that fail training if GEN2 capability advertised
  LinkStateL0,                                          ///< Device trained to L0
  LinkStateVcoNegotiation,                              ///< Check VCO negotiation complete
  LinkStateRetrain,                                     ///< Force retrain link.
  LinkStateTrainingFail,                                ///< Link training fail
  LinkStateTrainingSuccess,                             ///< Link training success
  LinkStateGfxWorkaround,                               ///< GFX workaround
  LinkStateCompliance,                                  ///< Link in compliance mode
  LinkStateDeviceNotPresent,                            ///< Link is not connected
  LinkStateTrainingCompleted                            ///< Link training completed
} PCIE_LINK_TRAINING_STATE;

/// PCIe Port Visibility
typedef enum {
  UnhidePorts,                                          ///< Command to unhide port
  HidePorts,                                            ///< Command to hide unused ports
} PCIE_PORT_VISIBILITY;


/// Table Register Entry
typedef struct {
  UINT16          Reg;                                  ///< Address
  UINT32          Mask;                                 ///< Mask
  UINT32          Data;                                 ///< Data
} PCIE_PORT_REGISTER_ENTRY;

/// Table Register Entry
typedef struct {
  CONST PCIE_PORT_REGISTER_ENTRY  *Table;                     ///< Table
  UINT32                    Length;                     ///< Length
} PCIE_PORT_REGISTER_TABLE_HEADER;

/// Table Register Entry
typedef struct {
  UINT32          Reg;                                  ///< Address
  UINT32          Mask;                                 ///< Mask
  UINT32          Data;                                 ///< Data
} PCIE_HOST_REGISTER_ENTRY;

/// Table Register Entry
typedef struct {
  CONST PCIE_HOST_REGISTER_ENTRY  *Table;                     ///< Table
  UINT32                    Length;                     ///< Length
} PCIE_HOST_REGISTER_TABLE_HEADER;

///Link ASPM info
typedef struct {
  PCI_ADDR        DownstreamPort;                       ///< PCI address of downstream port
  PCIE_ASPM_TYPE  DownstreamAspm;                       ///< Downstream Device Aspm
  PCI_ADDR        UpstreamPort;                         ///< PCI address of upstream port
  PCIE_ASPM_TYPE  UpstreamAspm;                         ///< Upstream Device Capability
  PCIE_ASPM_TYPE  RequestedAspm;                        ///< Requested ASPM
} PCIe_LINK_ASPM;

///PCIe ASPM Latency Information
typedef struct {
  UINT8  MaxL0sExitLatency;                             ///< Max L0s exit latency in us
  UINT8  MaxL1ExitLatency;                              ///< Max L1 exit latency in us
} PCIe_ASPM_LATENCY_INFO;

/// PCI address association
typedef struct {
  UINT8 NewDeviceAddress;                                ///< New PCI address (Device,Fucntion)
  UINT8 NativeDeviceAddress;                             ///< Native PCI address (Device,Fucntion)
} PCI_ADDR_LIST;

/// The return status for GFX Card Workaround.
typedef enum {
  GFX_WORKAROUND_DEVICE_NOT_READY,                       ///< GFX Workaround device is not ready.
  GFX_WORKAROUND_RESET_DEVICE,                           ///< GFX Workaround device need reset.
  GFX_WORKAROUND_SUCCESS                                 ///< The service completed normally.
} GFX_WORKAROUND_STATUS;

/// GFX workaround control
typedef enum {
  GfxWorkaroundDisable,                                   ///< GFX Workaround disabled
  GfxWorkaroundEnable                                     ///< GFX Workaround enabled
} GFX_WORKAROUND_CONTROL;

/// PIF lane power state
typedef enum {
  PifPowerStateL0,                                        ///<
  PifPowerStateLS1,                                       ///<
  PifPowerStateLS2,                                       ///<
  PifPowerStateOff = 0x7,                                 ///<
} PCIE_PIF_POWER_STATE;

/// PIF lane power control
typedef enum {
  PowerDownPifs,                                          ///<
  PowerUpPifs                                             ///<
} PCIE_PIF_POWER_CONTROL;

///PLL rumup time
typedef enum {
  NormalRampup,                                           ///<
  LongRampup,                                             ///<
} PCIE_PLL_RAMPUP_TIME;

typedef UINT16 PCIe_ENGINE_INIT_STATUS;

/// PCIe port configuration info
typedef struct {
  PCIe_PORT_DATA          PortData;                     ///< Port data
  UINT8                   StartCoreLane;                ///< Start Core Lane
  UINT8                   EndCoreLane;                  ///< End Core lane
  UINT8                   NativeDevNumber :5;           ///< Native PCI device number of the port
  UINT8                   NativeFunNumber :3;           ///< Native PCI function number of the port
  UINT8                   CoreId :4;                    ///< PCIe core ID
  UINT8                   PortId :4;                    ///< Port ID on wrapper
  PCI_ADDR                Address;                      ///< PCI address of the port
  UINT8                   State;                        ///< Training state
  UINT8                   PcieBridgeId:4;               ///< IOC PCIe bridge ID
  UINT16                  UnitId:12;                    ///< Port start unit ID
  UINT16                  NumberOfUnitId:4;             ///< Def number of unitIDs assigned to port
  UINT8                   GfxWrkRetryCount:4;           ///< Number of retry for GFX workaround
  UINT32                  TimeStamp;                    ///< Time stamp used to during training process
  UINT8                   LogicalBridgeId;              ///< Logical Bridge ID
} PCIe_PORT_CONFIG;

///Descriptor header
typedef struct {
  UINT32                  DescriptorFlags;              ///< Descriptor flags
  UINT16                  Parent;                       ///< Offset of parent descriptor
  UINT16                  Peer;                         ///< Offset of the peer descriptor
  UINT16                  Child;                        ///< Offset of the list of child descriptors
} PCIe_DESCRIPTOR_HEADER;

/// DDI (Digital Display Interface) configuration info
typedef struct {
  PCIe_DDI_DATA           DdiData;                      ///< DDI Data
  UINT8                   DisplayPriorityIndex;         ///< Display priority index
  UINT8                   ConnectorId;                  ///< Connector id determined by enumeration
  UINT8                   DisplayDeviceId;              ///< Display device id determined by enumeration
} PCIe_DDI_CONFIG;


/// Engine configuration data
typedef struct {
  PCIe_DESCRIPTOR_HEADER  Header;                       ///< Descripto header
  PCIe_ENGINE_DATA        EngineData;                   ///< Engine Data
  PCIe_ENGINE_INIT_STATUS InitStatus;                   ///< Initialization Status
  UINT8                   Scratch;                      ///< Scratch pad
  union {
    PCIe_PORT_CONFIG      Port;                         ///< PCIe port configuration data
    PCIe_DDI_CONFIG       Ddi;                          ///< DDI configuration data
  } Type;
} PCIe_ENGINE_CONFIG;

/// Wrapper configuration data
typedef struct {
  PCIe_DESCRIPTOR_HEADER  Header;                       ///< Descrptor Header
  UINT8                   WrapId;                       ///< Wrapper ID
  UINT8                   NumberOfPIFs;                 ///< Number of PIFs on wrapper
  UINT8                   StartPhyLane;                 ///< Start PHY Lane
  UINT8                   EndPhyLane;                   ///< End PHY Lane
  UINT8                   StartPcieCoreId:4;            ///< Start PCIe Core ID
  UINT8                   EndPcieCoreId:4;              ///< End PCIe Core ID
  UINT8                   NumberOfLanes;                ///< Number of lanes
  struct {
    UINT8                 PowerOffUnusedLanes:1;        ///< Power Off unused lanes
    UINT8                 PowerOffUnusedPlls:1;         ///< Power Off unused Plls
    UINT8                 ClkGating:1;                  ///< TXCLK gating
    UINT8                 LclkGating:1;                 ///< LCLK gating
    UINT8                 TxclkGatingPllPowerDown:1;    ///< TXCLK clock gating PLL power down
    UINT8                 PllOffInL1:1;                 ///< PLL off in L1
    UINT8                 AccessEncoding:1;             ///< Reg access encoding
  } Features;
  UINT8                   MasterPll;                    ///< Bitmap of master PLL
} PCIe_WRAPPER_CONFIG;


/// Silicon configuration data
typedef struct  {
  PCIe_DESCRIPTOR_HEADER  Header;                       ///< Descrptor Header
  UINT8                   SiliconId;                    ///< Gnb silicon(module) ID
  UINT8                   NodeId;                       ///< Node to which GNB connected
  UINT8                   LinkId;                       ///< Link to which GNB connected if LinkId > 3 GNB connected to sublink = LinkId - 4
  PCI_ADDR                Address;                      ///< PCI address of GNB host bridge
} PCIe_SILICON_CONFIG;

typedef PCIe_SILICON_CONFIG GNB_HANDLE;

/// Complex configuration data
typedef struct {
  PCIe_DESCRIPTOR_HEADER  Header;                       ///< Descrptor Header
  UINT8                   SocketId;                     ///< Processor socket ID
} PCIe_COMPLEX_CONFIG;

/// PCIe platform configuration info
typedef struct {
  PCIe_DESCRIPTOR_HEADER  Header;                       ///< Descrptor Header
  UINTN                   StdHeader;                    ///< Standard configuration header TODO:Used to be PVOID
  UINT32                  LinkReceiverDetectionPooling; ///< Receiver pooling detection time in us.
  UINT32                  LinkL0Pooling;                ///< Pooling for link to get to L0 in us
  UINT32                  LinkGpioResetAssertionTime;   ///< Gpio reset assertion time in us
  UINT32                  LinkResetToTrainingTime;      ///< Time duration between deassert GPIO reset and release training in us                                                      ///
  UINT8                   GfxCardWorkaround;            ///< GFX Card Workaround
  UINT8                   PsppPolicy;                   ///< PSPP policy
  UINT8                   TrainingExitState;            ///< State at which training should exit (see PCIE_LINK_TRAINING_STATE)
  UINT8                   TrainingAlgorithm;            ///< Training algorithm (see PCIE_TRAINING_ALGORITHM)
  PCIe_COMPLEX_CONFIG     ComplexList[MAX_NUMBER_OF_COMPLEXES];  ///< Complex
} PCIe_PLATFORM_CONFIG;

/// PCIe Engine Description
typedef struct {
  UINT32                  Flags;                        /**< Descriptor flags
                                                         * @li @b Bit31 - last descriptor on wrapper
                                                         * @li @b Bit30 - Descriptor allocated for PCIe port or DDI
                                                         */
  PCIe_ENGINE_DATA        EngineData;                   ///< Engine Data
} PCIe_ENGINE_DESCRIPTOR;

/// PCIe Lane allocation descriptor
typedef struct {
  UINT32                  Flags;                        ///< Flags
  UINT8                   WrapId;                       ///< Wrapper ID
  UINT8                   EngineType;                   ///< Engine Type
  UINT8                   NumberOfEngines;              ///< Number of engines to configure
  UINT8                   NumberOfConfigurations;       ///< Number of possible configurations
  UINT8                   *ConfigTable;                 ///< Pointer to config table
} PCIe_LANE_ALLOC_DESCRIPTOR;

#pragma pack (pop)

#endif
