/**
 * @file
 *
 * PCIE definitions.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/

#ifndef _NBPCIE_H_
#define _NBPCIE_H_

#pragma pack(push, 1)

/// PCIe Link Aspm mode
typedef enum {
  PcieLinkAspmDisabled,                     ///< Disabled
  PcieLinkAspmL0s,                          ///< L0s only
  PcieLinkAspmL1,                           ///< L1 only
  PcieLinkAspmL0sAndL1,                     ///< L0s and L1
  PcieLinkAspmL0sDownstreamOnly,            ///< L0s Donnstream Port Only
  PcieLinkAspmL0sDownstreamOnlyAndL1        ///< L0s Donnstream Port and L1
} PCIE_LINK_ASPM;

/// PCIe device type
typedef enum {
  PcieDeviceEndPoint,                       ///< Endpoint
  PcieDeviceLegacyEndPoint,                 ///< Legacy endpoint
  PcieDeviceRootComplex = 4,                ///< Root complex
  PcieDeviceUpstreamPort,                   ///< Upstream port
  PcieDeviceDownstreamPort,                 ///< Downstream Port
  PcieDevicePcieToPcix,                     ///< PCIe to PCI/PCIx bridge
  PcieDevicePcixToPcie,                     ///< PCI/PCIx to PCIe bridge
  PcieNotPcieDevice = 0xff                  ///< unknown device
} PCIE_DEVICE_TYPE;

/// PCIe Link Mode
typedef enum {
  PcieLinkModeGen2,                         ///< Gen 2
  PcieLinkModeGen1,                         ///< Gen 1
  PcieLinkModeGen2SoftwareInitiated,        ///< Gen 2 software
  PcieLinkModeGen2AdvertizeOnly             ///< Gen 2 advertise only
} PCIE_LINK_MODE;

/// PCIE Link Status
typedef enum {
  PcieLinkStatusNotConnected,               ///< not connected
  PcieLinkStatusConnected,                  ///< connected
  PcieLinkStatusInCompliance,               ///< compliant
  PcieLinkStatusTrainingInProgress,         ///< training in progress
  PcieLinkStatusVcoNegotiationInProgress,   ///< Vco negotiation in progress
} PCIE_LINK_STATUS;

/// PCIE Link Width Information
typedef enum {
  PcieLinkMaxWidth,                         ///< max width
  PcieLinkCurrentWidth,                     ///< current width
} PCIE_LINK_WIDTH_INFO;

/// PCIE Link Training
typedef enum {
  PcieLinkTrainingRelease,                  ///< training release
  PcieLinkTrainingHold                      ///< training hold
} PCIE_LINK_TRAINING;

/// PCIE Strap Mode
typedef enum {
  PcieCoreStrapConfigStart,                 ///< start
  PcieCoreStrapConfigStop                   ///< stop
} PCIE_STRAP_MODE;

/// PCIE Link Width
typedef enum {
  PcieLinkWidth_x0 = 0,                     ///< x0
  PcieLinkWidth_x1 = 1,                     ///< x1
  PcieLinkWidth_x2,                         ///< x2
  PcieLinkWidth_x4,                         ///< x4
  PcieLinkWidth_x8,                         ///< x8
  PcieLinkWidth_x12,                        ///< x12
  PcieLinkWidth_x16                         ///< x16
} PCIE_LINK_WIDTH;

/// PCIe Transmitter deemphasis advertise
typedef enum {
  PcieTxDeemphasis6dB = 0,                  ///< -6dB
  PcieTxDeemphasis3p5dB,                    ///< -3.5dB
} PCIE_LINK_DEEMPASIS;

/// PCIe Transmitter deemphasis advertise
typedef enum {
  PcieTxDriveStrangth26mA = 0,              ///< 26mA
  PcieTxDriveStrangth20mA,                  ///< 20mA
  PcieTxDriveStrangth22mA,                  ///< 22mA
  PcieTxDriveStrangth24mA,                  ///< 24mA
} PCIE_LINK_DRIVE_STRANGTH;

/// PCIe Channel type
typedef enum {
  PcieShortChannel   = 1,                   ///< Short Channel
  PcieMediumChannel,                        ///< Medium Channel
  PcieLongChannel,                          ///< Long Channel
} NB_PCIE_CHANNEL_TYPE;

/// PCI Core Reset
typedef enum {
  PcieCoreResetAllDeassert = 1,             ///< deassert
  PcieCoreResetAllAssert,                   ///< assert
  PcieCoreResetAllCheck,                    ///< check
} PCI_CORE_RESET;

/// Misc PCIE Core Setting
typedef struct {
  UINT32  CoreDisabled            :1;       ///< Core  not present or disabled
  UINT32  PowerOffPll             :1;       ///< Enable power off PLL if group of lanes controlled by PLL unused
  UINT32  PowerOffPllInL1         :1;       ///< Enable Power off PLL in L1
  UINT32  LclkClockGating         :1;       ///< Enable LCLK clock gating
  UINT32  TxClockGating           :1;       ///< Enable TX clock gating
  UINT32  PowerOffUnusedLanes     :1;       ///< Enable Power off pads for unused Lanes
  UINT32  CplBufferAllocation     :1;       ///< Enable special/optimized CPL buffer allocation
  UINT32  PerformanceMode         :1;       ///< Enable support PCIe Reference Clock overclocking. In addition to rump-up PCIe reference clock
  UINT32  TxDriveStrength         :2;       /**< TX Drive strength (Only applicable if PCIE_CORE_SETTING::ChannelType == 0).
                                             *   @li @b 0 - 26mA
                                             *   @li @b 1 - 20mA
                                             *   @li @b 2 - 22mA
                                             *   @li @b 3 - 24mA
                                             */
  UINT32  SkipConfiguration       :1;       ///< Special case to skip core configuration (configured outside of CIMx)
  UINT32  TxHalfSwingMode         :1;       ///< Half Swing Mode for PCIe Transmitters (Only applicable if PCIE_CORE_SETTING::ChannelType == 0).
  UINT32  ChannelType             :3;        /**< Group PCIe PHY setting for channel with specific trace length
                                             *   @li @b 0 - Use individual parameters to configure PCIe PHY (see PCIE_CORE_SETTING::TxHalfSwingMode,
                                                 PCIE_CORE_SETTING::TxDriveStrength, PCIE_EXT_PORT_CONFIG::PortDeemphasis).
                                             *   @li @b 1 - Short Channel.
                                             *   @li @b 2 - Midium Channel.
                                             *   @li @b 3 - Long  Channel.
                                             */
  UINT32  DetectPowerOffPllInL1   :1;       ///< Enable detection if endpoint L1 acceptable latency allow Enable Power off PLL in L1.
  UINT32  TxClockOff              :1;       ///< Disable TX clock if possible
  UINT32  LclkClockOff            :1;       ///< Disable LCLK clock if possible
  UINT32  RefClockInput           :1;       ///< Use dedicated ref. clock input (only applicable GPP1 and GPP2 cores). By default SB ref clock is used.
  UINT32  Reserved                :2;       ///<
  UINT32  CoreDisableStatus       :1;       /**< Output status of core disable/enable
                                             *   @li @b 0 = Core not disabled
                                             *   @li @b 1 = Core Disabled
                                             */
} PCIE_CORE_SETTING;

/// Misc Configuration
typedef struct {
  UINT32  DisableHideUnusedPorts  :1;       ///< Hide unused ports if no EP was detected and port  non hotpluggable
  UINT32  Peer2Peer               :1;       ///< Enable Peer to Peer.
  UINT32  DisableGfxWorkaround    :1;       ///< disable RV370/RV380 workaround
  UINT32  NbSbVc1                 :1;       ///< Enable VC1 for NB SB Audio traffic
} PCIE_MISC_CONFIG;

/// Extended PCIE Port Configuration
typedef struct {
  UINT32  PortL1ImmediateACK      :1;       ///< Validation feature
  UINT32  PortLinkWidth           :3;       /**< Port Link width
                                             *   @li @b 0 - Auto. Default max link width.
                                             *   @li @b 1 - x1
                                             *   @li @b 2 - x2
                                             *   @li @b 3 - x4
                                             *   @li @b 4 - x8
                                             *   @li @b 6 - x16
                                             */
  UINT32  PortMapping             :4;       /**< Device number mapping info
                                             *   @li @b 0 - Default mapping
                                             *   @li @b n - PCI device number for port (Valid device numbers are 2/3/4/5/6/7/9/10/11/12/13).
                                             */
  UINT32  PortHotplugDevMap       :2;       /**< PCA9539 device map.
                                             *Only valid if PortHotplug = 1
                                             */
  UINT32  PortHotplugByteMap      :1;       /**< PCA9539 channel map.
                                             *Only valid if PortHotplug = 1
                                             */
  UINT32  PortPowerLimit          :8;       ///< Slot power limit in W
  UINT32  Reserved                :2;       ///< Reserved
  UINT32  PortDeemphasis          :2;       /**< Port deempasis adverise (Only applicable if PCIE_CORE_SETTING::ChannelType == 0).
                                             *   @li @b 0 - 6dB
                                             *   @li @b 1 - 3.5dB
                                             */

} PCIE_EXT_PORT_CONFIG;

/// PCIE Port Configuration
typedef struct {
  UINT32  PortPresent             :1;       /**< Port connection
                                             *   @li @b 0 - Port has no slot or EP connected. Link not needs to be trained.
                                             *   @li @b 1 - Has slot or EP connected. Link needs to be trained.
                                             */
  UINT32  PortDetected            :1;       /**< Scratch bit to record status of training
                                             *   @li @b 0 - EP not detected
                                             *   @li @b 1 - EP detected
                                             */
  UINT32  PortCompliance          :1;       /**< Link compliance mode
                                             *   @li @b 0 - Link in operational mode
                                             *   @li @b 1 - Force link into compliance mode
                                             */
  UINT32  PortLinkMode            :2;       /**< Link speed mode configuration
                                             *   @li @b 0 - GEN2 Autonomous (GEN2 capability advertized and and immediate link speed change initiated).
                                             *   @li @b 1 - GEN1
                                             *   @li @b 2 - GEN2 Software Initiated (Port trained to Gen1 thereafter if EP report GEN2 capability port reconfigured to GEN2)
                                             *   @li @b 3 - GEN2 advertize only (RC only advertize GEN2 capability and not initiate transition to GEN2 speed)
                                             */
  UINT32  PortHotplug             :2;       /**< Port Hotplug configuration
                                             *   @li @b 0 - Hotplug Disabled
                                             *   @li @b 1 - Server Hotplug Enabled
                                             *   @li @b 2 - Reserved
                                             *   @li @b 3 - Reserved
                                             */
  UINT32  PortAspm                :3;       /**< Port ASPM support
                                             *   @li @b 0 - Disabled
                                             *   @li @b 1 - L0s enable
                                             *   @li @b 2 - L1 enable
                                             *   @li @b 3 - L0s + L1 enable
                                             *   @li @b 4 - L0s Downstream Only
                                             *   @li @b 5 - L0s Downstream Only + L1
                                             *   @li <b>4..7</b> - Reserved
                                             */
  UINT32  PortReversed            :1;       /**< Port lanes reversed
                                             *   @li @b 0 - Lanes non reversed
                                             *   @li @b 1 - Lanes reversed
                                             */
  UINT32  ForcePortDisable        :1;       /**< Port Disable after PCIE training
                                             *   @li @b 0 - Do not force port disable
                                             *   @li @b 1 - Force port disable
                                             */
  UINT32  PortAlwaysVisible       :1;       /**< Port always visible
                                             *   @li @b 1 - Port always visible
                                             */
} PCIE_PORT_CONFIG;

/// PCIE default configuration parameters structure
typedef  struct {
  PCIE_MISC_CONFIG          PcieConfiguration;        ///< PCIE configuration
  PCIE_CORE_SETTING         CoreSetting[5];           ///< Core Setting
  UINT16                    DeviceInitMaskS1;         ///< Bit mask of ports id to be initialized at stage 1
  UINT16                    DeviceInitMaskS2;         ///< Bit mask of ports id to be initialized at stage 2
  UINT16                    ResetToTrainingDelay;     ///< Delay (in 1ms) after reset deassert before training started
  UINT16                    TrainingToLinkTestDelay;  ///< Delay (in 1ms) after training started but before pooling link state
  UINT16                    ReceiverDetectionPooling; ///< Total amount time (in 1ms of pooling for passing receiver detection stage
} PCIE_DEFAULT_CONFIG;

/// Link Info
typedef struct {
  UINT8                     LinkWidth;                ///< width
  UINT8                     MaxLinkWidth;             ///< max width
  UINT8                     Line0Offset;              ///< line 0 offset
} LINK_INFO;

/// Port Static Info
typedef struct {
  UINT8                     TrainingAddress;          ///< training address
  UINT8                     ReversalAddress;          ///< reversal address
  UINT8                     DeemphasisAddress;        ///< de-emphasis address
  UINT8                     MappingAddress;           ///< mapping address
  UINT8                     HotplugAddress;           ///< Hotplug address
} PORT_STATIC_INFO;

/// Core Info
typedef struct {
  UINT32                    CoreSelector;                   ///< core selector
  UINT16                    PortIdBitMap;                   ///< port Id
  UINT8                     TrainingRegister;               ///< training
  UINT8                     DeemphasisRegister;             ///< de-emphasis
  UINT8                     StrapRegister;                  ///< strap
  UINT8                     StrapAddress;                   ///< strap address
  UINT8                     HotplugRegister;                ///< Hotplug descriptor register
  UINT8                     TxDriveStrengthRegister;        ///< Tx drive strength register
  UINT8                     TxDriveStrengthOffset;          ///< Tx drive strength bit offeset
  UINT8                     TxHalfSwingRegister;            ///< Tx half swing register
  UINT8                     TxHalfSwingOffset;              ///< Tx half swing bit offset
  UINT8                     TxHalfSwingDeepmhasisRegister;  ///< Tx half swing deephasis register
  UINT8                     TxHalfSwingDeepmhasisOffset;    ///< Tx half swing deephasis register
  UINT8                     TxOffOffset;                    ///< Tx shutdown enable offset
  UINT8                     LclkOffOffset;                  ///< Lclk shutdown enable offset
  UINT8                     LclkPermOffOffset;              ///< Lclk Perm shutdown enable offset
} CORE_INFO;

/// Port Information
typedef struct {
  UINT8     MaxLinkWidth;                   ///< max link width
  UINT8     Line0Offset;                    ///< offset
  UINT8     SlaveCplBuffers;                ///< Alternative to default CPL buffer count
} PORT_INFO;

/// GPP Configuration Info
typedef struct {
  PORT_INFO *PortInfoPtr;                   ///< port information
  UINT32    PortIdMap;                      ///< port id map
} GPP_CFG_INFO;


#define   GPP1_CORE               0x40000
#define   GPP2_CORE               0x60000
#define   GPP3a_CORE              0x70000
#define   GPP3b_CORE              0x30000
#define   SB_CORE                 0x50000

#define   GPP_CONFIG_GPP420000    0x01
#define   GPP_CONFIG_GPP411000    0x02
#define   GPP_CONFIG_GPP222000    0x03
#define   GPP_CONFIG_GPP221100    0x04
#define   GPP_CONFIG_GPP211110    0x05
#define   GPP_CONFIG_GPP111111    0x06

#define   GFX_CONFIG_A            0x01
#define   GFX_CONFIG_B            0x02

#define   GFX_CONFIG_AAAA         (GFX_CONFIG_A | (GFX_CONFIG_A << 8) | (GFX_CONFIG_A << 16) | (GFX_CONFIG_A << 24))
#define   GFX_CONFIG_AABB         (GFX_CONFIG_A | (GFX_CONFIG_A << 8) | (GFX_CONFIG_B << 16) | (GFX_CONFIG_B << 24))

#define   PCIE_CAP_ID             0x10


#pragma pack(pop)

#endif
