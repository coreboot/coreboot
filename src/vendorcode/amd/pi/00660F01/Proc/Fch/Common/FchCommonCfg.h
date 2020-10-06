/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH Function Support Definition
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 311507 $   @e \$Date: 2015-01-22 06:57:51 +0800 (Thu, 22 Jan 2015) $
 *
 */
 /*****************************************************************************
 *
 * Copyright (c) 2008 - 2015, Advanced Micro Devices, Inc.
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
 ***************************************************************************/
#ifndef _FCH_COMMON_CFG_H_
#define _FCH_COMMON_CFG_H_

#pragma pack (push, 1)

//-----------------------------------------------------------------------------
//                     FCH DEFINITIONS AND MACROS
//-----------------------------------------------------------------------------

//
// FCH Component Data Structure Definitions
//

/// PCI_ADDRESS - PCI access structure
#define PCI_ADDRESS(bus, dev, func, reg) \
                   (UINT32) ( (((UINT32)bus) << 24) + (((UINT32)dev) << 19) + (((UINT32)func) << 16) + ((UINT32)reg) )

#define CPUID_FMF        0x80000001ul  // Family Model Features information
///
///  - Byte Register R/W structure
///
typedef struct _REG8_MASK {
  UINT8                 RegIndex;                       /// RegIndex - Reserved
  UINT8                 AndMask;                        /// AndMask - Reserved
  UINT8                 OrMask;                         /// OrMask - Reserved
} REG8_MASK;


///
/// PCIE Reset Block
///
typedef enum {
  NbBlock,                                              ///< Reset for NB PCIE
  FchBlock                                              ///< Reset for FCH GPP
} RESET_BLOCK;

///
/// PCIE Reset Operation
///
typedef enum {
  DeassertReset,                                        ///< DeassertRese - Deassert reset
  AssertReset                                           ///< AssertReset - Assert reset
} RESET_OP;


///
/// Fch Run Time Parameters
///
typedef struct {
  UINT32                PcieMmioBase;                              ///< PcieMmioBase
  UINT32                FchDeviceEnableMap;                        ///< FchDeviceEnableMap
  UINT32                FchDeviceD3ColdMap;                        ///< FchDeviceD3ColdMap
  UINT16                XHCI_PMx04_XhciFwRomAddr_Rom;              ///< XHCI_PMx04_XhciFwRomAddr_Rom
  UINT32                XHCI_PMx08_xHCI_Firmware_Addr_1_Rom;       ///< XHCI_PMx08_xHCI_Firmware_Addr_1_Ram
  UINT16                XHCI_PMx04_XhciFwRomAddr_Ram;              ///< XHCI_PMx04_XhciFwRomAddr_Rom
  UINT32                XHCI_PMx08_xHCI_Firmware_Addr_1_Ram;       ///< XHCI_PMx08_xHCI_Firmware_Addr_1_Ram
  UINT8                 SataDevSlpPort0S5Pin;                      ///< SataDevSlpPort0S5Pin - Reserved
  UINT8                 SataDevSlpPort1S5Pin;                      ///< SataDevSlpPort1S5Pin - Reserved
  UINT16                Dummy16;                                   ///< Dummy16 - Reserved
  UINT32                SdMmioBase;                                ///< Sd Mmio Base - Reserved
  UINT32                EhciMmioBase;                              ///< Ehci Mmio Base - Reserved
  UINT32                XhciMmioBase;                              ///< Xhci Mmio Base - Reserved
  UINT32                SataMmioBase;                              ///< Sata Mmio Base - Reserved
} FCH_RUNTIME;

///
/// SD structure
///
typedef struct {
  SD_MODE               SdConfig;                       ///< SD Mode configuration
                                                        ///   @li <b>00</b> - Disabled
                                                        ///   @li <b>00</b> - AMDA
                                                        ///   @li <b>01</b> - DMA
                                                        ///   @li <b>10</b> - PIO
                                                        ///
  UINT8                 SdSpeed;                        ///< SD Speed
                                                        ///   @li <b>0</b> - Low speed
                                                        ///   @li <b>1</b> - High speed
                                                        ///
  UINT8                 SdBitWidth;                     ///< SD Bit Width
                                                        ///   @li <b>0</b> - 32BIT clear 23
                                                        ///   @li <b>1</b> - 64BIT, set 23,default
                                                        ///
  UINT32                SdSsid;                         ///< SD Subsystem ID
  SD_CLOCK_CONTROL      SdClockControl;                 ///< SD Clock Control
  BOOLEAN               SdClockMultiplier;              ///< SD Clock Multiplier enable/disable
  UINT8                 SdReTuningMode;                 ///< SD Re-tuning modes select
                                                        ///    @li <b>0</b> - mode 1
                                                        ///    @li <b>1</b> - mode 2
                                                        ///    @li <b>2</b> - mode 3
  UINT8                 SdHostControllerVersion;        ///< SD controller Version
                                                        ///    @li <b>1</b> - SD 2.0
                                                        ///    @li <b>2</b> - SD 3.0
  UINT8                 SdrCapabilities;                ///< SDR Capability mode select
                                                        ///    @li <b>00</b> - SDR25/15
                                                        ///    @li <b>01</b> - SDR50
                                                        ///    @li <b>11</b> - SDR104
  UINT8                 SdSlotType;                     ///< SDR Slot Type select
                                                        ///    @li <b>00</b> - Removable Card Slot
                                                        ///    @li <b>01</b> - Embedded Slot for One Device
                                                        ///    @li <b>10</b> - Shared Bus Slot
  BOOLEAN               SdForce18;                      ///< SD Force18
  UINT8                 SdDbgConfig;                       ///< SD Mode configuration
                                                        ///   @li <b>00</b> - Disabled
                                                        ///   @li <b>00</b> - AMDA
                                                        ///   @li <b>01</b> - DMA
                                                        ///   @li <b>10</b> - PIO
                                                        ///
} FCH_SD;

///
/// CODEC_ENTRY - Fch HD Audio OEM Codec structure
///
typedef struct _CODEC_ENTRY {
  UINT8                 Nid;                            /// Nid - Reserved
  UINT32                Byte40;                         /// Byte40 - Reserved
} CODEC_ENTRY;

///
/// CODEC_TBL_LIST - Fch HD Audio Codec table list
///
typedef struct _CODEC_TBL_LIST {
  UINT32                CodecId;                        /// CodecID - Codec ID
  CODEC_ENTRY*          CodecTablePtr;                  /// CodecTablePtr - Codec table pointer
} CODEC_TBL_LIST;

///
/// AZALIA_PIN - HID Azalia or GPIO define structure.
///
typedef struct _AZALIA_PIN {
  UINT8                 AzaliaSdin0;                    ///< AzaliaSdin0
                                                        ///   @par
                                                        ///   @li <b>00</b> - GPIO PIN
                                                        ///   @li <b>10</b> - As a Azalia SDIN pin

  UINT8                 AzaliaSdin1;                    ///< AzaliaSdin1
                                                        ///   @par
                                                        /// SDIN1 is defined at BIT2 & BIT3
                                                        ///   @li <b>00</b> - GPIO PIN
                                                        ///   @li <b>10</b> - As a Azalia SDIN pin

  UINT8                 AzaliaSdin2;                    ///< AzaliaSdin2
                                                        ///   @par
                                                        /// SDIN2 is defined at BIT4 & BIT5
                                                        ///   @li <b>00</b> - GPIO PIN
                                                        ///   @li <b>10</b> - As a Azalia SDIN pin

  UINT8                 AzaliaSdin3;                    ///< AzaliaSdin3
                                                        ///   @par
                                                        /// SDIN3 is defined at BIT6 & BIT7
                                                        ///   @li <b>00</b> - GPIO PIN
                                                        ///   @li <b>10</b> - As a Azalia SDIN pin
} AZALIA_PIN;

///
/// Azalia structure
///
typedef struct {
  HDA_CONFIG            AzaliaEnable;                   ///< AzaliaEnable       - Azalia function configuration
  BOOLEAN               AzaliaMsiEnable;                ///< AzaliaMsiEnable    - Azalia MSI capability
  UINT32                AzaliaSsid;                     ///< AzaliaSsid         - Azalia Subsystem ID
  UINT8                 AzaliaPinCfg;                   ///< AzaliaPinCfg       - Azalia Controller SDIN pin Configuration
                                                        ///  @par
                                                        ///  @li <b>0</b>       - disable
                                                        ///  @li <b>1</b>       - enable

  UINT8                 AzaliaFrontPanel;               ///< AzaliaFrontPanel   - Azalia Controller Front Panel Configuration
                                                        ///   @par
                                                        /// Support Front Panel configuration
                                                        ///   @li <b>0</b>      - Auto
                                                        ///   @li <b>1</b>      - disable
                                                        ///   @li <b>2</b>      - enable

  UINT8                 FrontPanelDetected;             ///< FrontPanelDetected - Force Azalia Controller Front Panel Configuration
                                                        ///  @par
                                                        /// Force Front Panel configuration
                                                        ///  @li <b>0</b>       - Not Detected
                                                        ///  @li <b>1</b>       - Detected

  UINT8                 AzaliaSnoop;                    ///< AzaliaSnoop        - Azalia Controller Snoop feature Configuration
                                                        ///   @par
                                                        /// Azalia Controller Snoop feature Configuration
                                                        ///   @li <b>0</b>      - disable
                                                        ///   @li <b>1</b>      - enable

  UINT8                 AzaliaDummy;                    /// AzaliaDummy         - Reserved */

  AZALIA_PIN            AzaliaConfig;                   /// AzaliaConfig        - Azaliz Pin Configuration

///
/// AZOEMTBL - Azalia Controller OEM Codec Table Pointer
///
  CODEC_TBL_LIST        *AzaliaOemCodecTablePtr;        /// AzaliaOemCodecTablePtr - Oem Azalia Codec Table Pointer

///
/// AZOEMFPTBL - Azalia Controller Front Panel OEM Table Pointer
///
  VOID                  *AzaliaOemFpCodecTablePtr;      /// AzaliaOemFpCodecTablePtr - Oem Front Panel Codec Table Pointer
} FCH_AZALIA;

///
/// _SPI_DEVICE_PROFILE Spi Device Profile structure
///
typedef struct _SPI_DEVICE_PROFILE {
  UINT32                JEDEC_ID;                       /// JEDEC ID
  UINT32                RomSize;                        /// ROM Size
  UINT32                SectorSize;                     /// Sector Size
  UINT16                MaxNormalSpeed;                 /// Max Normal Speed
  UINT16                MaxFastSpeed;                   /// Max Fast Speed
  UINT16                MaxDualSpeed;                   /// Max Dual Speed
  UINT16                MaxQuadSpeed;                   /// Max Quad Speed
  UINT8                 QeReadRegister;                 /// QE Read Register
  UINT8                 QeWriteRegister;                /// QE Write Register
  UINT8                 QeOperateSize;                  /// QE Operate Size 1byte/2bytes
  UINT16                QeLocation;                     // QE Location in the register
} SPI_DEVICE_PROFILE;

///
/// _SPI_CONTROLLER_PROFILE Spi Device Profile structure
///
typedef struct _SPI_CONTROLLER_PROFILE {
//  UINT32                SPI_CONTROLLER_ID;                       /// SPI Controller ID
  UINT16                FifoSize;                       /// FIFO Size
  UINT16                MaxNormalSpeed;                 /// Max Normal Speed
  UINT16                MaxFastSpeed;                   /// Max Fast Speed
  UINT16                MaxDualSpeed;                   /// Max Dual Speed
  UINT16                MaxQuadSpeed;                   /// Max Quad Speed
} SPI_CONTROLLER_PROFILE;

///
/// SPI structure
///
typedef struct {
  BOOLEAN               LpcMsiEnable;                   ///< LPC MSI capability
  UINT32                LpcSsid;                        ///< LPC Subsystem ID
  UINT32                RomBaseAddress;                 ///< SpiRomBaseAddress
                                                        ///   @par
                                                        ///   SPI ROM BASE Address
                                                        ///
  UINT8                 SpiSpeed;                       ///< SpiSpeed - Spi Frequency
                                                        ///  @par
                                                        ///  SPI Speed [1.0] - the clock speed for non-fast read command
                                                        ///   @li <b>00</b> - 66Mhz
                                                        ///   @li <b>01</b> - 33Mhz
                                                        ///   @li <b>10</b> - 22Mhz
                                                        ///   @li <b>11</b> - 16.5Mhz
                                                        ///
  UINT8                 SpiFastSpeed;                   ///< FastSpeed  - Spi Fast Speed feature
                                                        ///  SPIFastSpeed [1.0] - the clock speed for Fast Speed Feature
                                                        ///   @li <b>00</b> - 66Mhz
                                                        ///   @li <b>01</b> - 33Mhz
                                                        ///   @li <b>10</b> - 22Mhz
                                                        ///   @li <b>11</b> - 16.5Mhz
                                                        ///
  UINT8                 WriteSpeed;                     ///< WriteSpeed - Spi Write Speed
                                                        /// @par
                                                        ///  WriteSpeed [1.0] - the clock speed for Spi write command
                                                        ///   @li <b>00</b> - 66Mhz
                                                        ///   @li <b>01</b> - 33Mhz
                                                        ///   @li <b>10</b> - 22Mhz
                                                        ///   @li <b>11</b> - 16.5Mhz
                                                        ///
  UINT8                 SpiMode;                        ///< SpiMode    - Spi Mode Setting
                                                        /// @par
                                                        ///  @li <b>101</b> - Qual-io 1-4-4
                                                        ///  @li <b>100</b> - Dual-io 1-2-2
                                                        ///  @li <b>011</b> - Qual-io 1-1-4
                                                        ///  @li <b>010</b> - Dual-io 1-1-2
                                                        ///  @li <b>111</b> - FastRead
                                                        ///  @li <b>110</b> - Normal
                                                        ///
  UINT8                 AutoMode;                       ///< AutoMode   - Spi Auto Mode
                                                        /// @par
                                                        ///  SPI Auto Mode
                                                        ///  @li <b>0</b> - Disabled
                                                        ///  @li <b>1</b> - Enabled
                                                        ///
  UINT8                 SpiBurstWrite;                  ///< SpiBurstWrite - Spi Burst Write Mode
                                                        /// @par
                                                        ///  SPI Burst Write
                                                        ///  @li <b>0</b> - Disabled
                                                        ///  @li <b>1</b> - Enabled
  BOOLEAN               LpcClk0;                        ///< Lclk0En - LPCCLK0
                                                        /// @par
                                                        ///  LPC Clock 0 mode
                                                        ///  @li <b>0</b> - forced to stop
                                                        ///  @li <b>1</b> - functioning with CLKRUN protocol
  BOOLEAN               LpcClk1;                        ///< Lclk1En - LPCCLK1
                                                        /// @par
                                                        ///  LPC Clock 1 mode
                                                        ///  @li <b>0</b> - forced to stop
                                                        ///  @li <b>1</b> - functioning with CLKRUN protocol
//  UINT32                SPI100_RX_Timing_Config_Register_38;                 ///< SPI100_RX_Timing_Config_Register_38
//  UINT16                SPI100_RX_Timing_Config_Register_3C;                 ///< SPI100_RX_Timing_Config_Register_3C
//  UINT8                 SpiProtectEn0_1d_34;                  ///
  UINT8                 SPI100_Enable;                  ///< Spi 100 Enable
  SPI_DEVICE_PROFILE    SpiDeviceProfile;               ///< Spi Device Profile
} FCH_SPI;


///
/// IDE structure
///
typedef struct {
  BOOLEAN               IdeEnable;                      ///< IDE function switch
  BOOLEAN               IdeMsiEnable;                   ///< IDE MSI capability
  UINT32                IdeSsid;                        ///< IDE controller Subsystem ID
} FCH_IDE;

///
/// IR Structure
///
typedef struct {
  IR_CONFIG             IrConfig;                       ///< IrConfig
  UINT8                 IrPinControl;                   ///< IrPinControl
} FCH_IR;


///
/// PCI Bridge Structure
///
typedef struct {
  BOOLEAN               PcibMsiEnable;                  ///< PCI-PCI Bridge MSI capability
  UINT32                PcibSsid;                       ///< PCI-PCI Bridge Subsystem ID
  UINT8                 PciClks;                        ///< 33MHz PCICLK0/1/2/3 Enable, bits [0:3] used
                                                        ///   @li <b>0</b> - disable
                                                        ///   @li <b>1</b> - enable
                                                        ///
  UINT16                PcibClkStopOverride;            ///< PCIB_CLK_Stop Override
  BOOLEAN               PcibClockRun;                   ///< Enable the auto clkrun functionality
                                                        ///   @li <b>0</b> - disable
                                                        ///   @li <b>1</b> - enable
                                                        ///
} FCH_PCIB;


///
/// - SATA Phy setting structure
///
typedef struct _SATA_PHY_SETTING {
  UINT16                PhyCoreControlWord;             /// PhyCoreControlWord - Reserved
  UINT32                PhyFineTuneDword;               /// PhyFineTuneDword - Reserved
} SATA_PHY_SETTING;

///
/// SATA main setting structure
///
typedef struct _SATA_ST {
  UINT8                 SataModeReg;                    ///< SataModeReg - Sata Controller Mode
  BOOLEAN               SataEnable;                     ///< SataEnable - Sata Controller Function
                                                        ///   @par
                                                        /// Sata Controller
                                                        ///   @li <b>0</b> - disable
                                                        ///   @li <b>1</b> - enable
                                                        ///
  UINT8                 Sata6AhciCap;                   ///< Sata6AhciCap - Reserved */
  BOOLEAN               SataSetMaxGen2;                 ///< SataSetMaxGen2 - Set Sata Max Gen2 mode
                                                        ///   @par
                                                        /// Sata Controller Set to Max Gen2 mode
                                                        ///   @li <b>0</b> - disable
                                                        ///   @li <b>1</b> - enable
                                                        ///
  BOOLEAN               IdeEnable;                      ///< IdeEnable - Hidden IDE
                                                        ///  @par
                                                        /// Sata IDE Controller Combined Mode
                                                        ///   Enable -  SATA controller has control over Port0 through Port3,
                                                        ///     IDE controller has control over Port4 and Port7.
                                                        ///   Disable - SATA controller has full control of all 8 Ports
                                                        ///     when operating in non-IDE mode.
                                                        ///  @li <b>0</b> - enable
                                                        ///  @li <b>1</b> - disable
                                                        ///
  UINT8                 SataClkMode;                    /// SataClkMode - Reserved
} SATA_ST;

///
/// SATA_PORT_ST - SATA PORT structure
///
typedef struct _SATA_PORT_ST {
  UINT8                 SataPortReg;                    ///< SATA Port bit map - bits[0:7] for ports 0 ~ 7
                                                        ///  @li <b>0</b> - disable
                                                        ///  @li <b>1</b> - enable
                                                        ///
  BOOLEAN               Port0;                          ///< PORT0 - 0:disable, 1:enable
  BOOLEAN               Port1;                          ///< PORT1 - 0:disable, 1:enable
  BOOLEAN               Port2;                          ///< PORT2 - 0:disable, 1:enable
  BOOLEAN               Port3;                          ///< PORT3 - 0:disable, 1:enable
  BOOLEAN               Port4;                          ///< PORT4 - 0:disable, 1:enable
  BOOLEAN               Port5;                          ///< PORT5 - 0:disable, 1:enable
  BOOLEAN               Port6;                          ///< PORT6 - 0:disable, 1:enable
  BOOLEAN               Port7;                          ///< PORT7 - 0:disable, 1:enable
} SATA_PORT_ST;

///
///< _SATA_PORT_MD - Force Each PORT to GEN1/GEN2 mode
///
typedef struct _SATA_PORT_MD {
  UINT16                SataPortMode;                   ///< SATA Port GEN1/GEN2 mode bit map - bits [0:15] for ports 0 ~ 7
  UINT8                 Port0;                          ///< PORT0 - set BIT0 to GEN1, BIT1 - PORT0 set to GEN2
  UINT8                 Port1;                          ///< PORT1 - set BIT2 to GEN1, BIT3 - PORT1 set to GEN2
  UINT8                 Port2;                          ///< PORT2 - set BIT4 to GEN1, BIT5 - PORT2 set to GEN2
  UINT8                 Port3;                          ///< PORT3 - set BIT6 to GEN1, BIT7 - PORT3 set to GEN2
  UINT8                 Port4;                          ///< PORT4 - set BIT8 to GEN1, BIT9 - PORT4 set to GEN2
  UINT8                 Port5;                          ///< PORT5 - set BIT10 to GEN1, BIT11 - PORT5 set to GEN2
  UINT8                 Port6;                          ///< PORT6 - set BIT12 to GEN1, BIT13 - PORT6 set to GEN2
  UINT8                 Port7;                          ///< PORT7 - set BIT14 to GEN1, BIT15 - PORT7 set to GEN2
} SATA_PORT_MD;
///
/// SATA structure
///
typedef struct {
  BOOLEAN               SataMsiEnable;                  ///< SATA MSI capability
  UINT32                SataIdeSsid;                    ///< SATA IDE mode SSID
  UINT32                SataRaidSsid;                   ///< SATA RAID mode SSID
  UINT32                SataRaid5Ssid;                  ///< SATA RAID 5 mode SSID
  UINT32                SataAhciSsid;                   ///< SATA AHCI mode SSID

  SATA_ST               SataMode;                       /// SataMode - Reserved
  SATA_CLASS            SataClass;                      ///< SataClass - SATA Controller mode [2:0]
  UINT8                 SataIdeMode;                    ///< SataIdeMode - Sata IDE Controller mode
                                                        ///  @par
                                                        ///   @li <b>0</b> - Legacy IDE mode
                                                        ///   @li <b>1</b> - Native IDE mode
                                                        ///
  UINT8                 SataDisUnusedIdePChannel;       ///< SataDisUnusedIdePChannel-Disable Unused IDE Primary Channel
                                                        ///  @par
                                                        ///   @li <b>0</b> - Channel Enable
                                                        ///   @li <b>1</b> - Channel Disable
                                                        ///
  UINT8                 SataDisUnusedIdeSChannel;       ///< SataDisUnusedIdeSChannel - Disable Unused IDE Secondary Channel
                                                        ///   @par
                                                        ///    @li <b>0</b> - Channel Enable
                                                        ///    @li <b>1</b> - Channel Disable
                                                        ///
  UINT8                 IdeDisUnusedIdePChannel;        ///< IdeDisUnusedIdePChannel-Disable Unused IDE Primary Channel
                                                        ///   @par
                                                        ///    @li <b>0</b> - Channel Enable
                                                        ///    @li <b>1</b> - Channel Disable
                                                        ///
  UINT8                 IdeDisUnusedIdeSChannel;        ///< IdeDisUnusedIdeSChannel-Disable Unused IDE Secondary Channel
                                                        ///   @par
                                                        ///    @li <b>0</b> - Channel Enable
                                                        ///    @li <b>1</b> - Channel Disable
                                                        ///
  UINT8                 SataOptionReserved;             /// SataOptionReserved - Reserved

  SATA_PORT_ST          SataEspPort;                    ///<  SataEspPort - SATA port is external accessible on a signal only connector (eSATA:)

  SATA_PORT_ST          SataPortPower;                  ///<  SataPortPower - Port Power configuration

  SATA_PORT_MD          SataPortMd;                     ///<  SataPortMd - Port Mode

  UINT8                 SataAggrLinkPmCap;              /// SataAggrLinkPmCap - 0:OFF   1:ON
  UINT8                 SataPortMultCap;                /// SataPortMultCap - 0:OFF   1:ON
  UINT8                 SataClkAutoOff;                 /// SataClkAutoOff - AutoClockOff 0:Disabled, 1:Enabled
  UINT8                 SataPscCap;                     /// SataPscCap 1:Enable PSC, 0:Disable PSC capability
  UINT8                 BiosOsHandOff;                  /// BiosOsHandOff - Reserved
  UINT8                 SataFisBasedSwitching;          /// SataFisBasedSwitching - Reserved
  UINT8                 SataCccSupport;                 /// SataCccSupport - Reserved
  UINT8                 SataSscCap;                     /// SataSscCap - 1:Enable, 0:Disable SSC capability
  UINT8                 SataMsiCapability;              /// SataMsiCapability 0:Hidden 1:Visible
  UINT8                 SataForceRaid;                  /// SataForceRaid   0:No function 1:Force RAID
  UINT8                 SataInternal100Spread;          /// SataInternal100Spread - Reserved
  UINT8                 SataDebugDummy;                 /// SataDebugDummy - Reserved
  UINT8                 SataTargetSupport8Device;       /// SataTargetSupport8Device - Reserved
  UINT8                 SataDisableGenericMode;         /// SataDisableGenericMode - Reserved
  BOOLEAN               SataAhciEnclosureManagement;    /// SataAhciEnclosureManagement - Reserved
  UINT8                 SataSgpio0;                     /// SataSgpio0 - Reserved
  UINT8                 SataSgpio1;                     /// SataSgpio1 - Reserved
  UINT8                 SataPhyPllShutDown;             /// SataPhyPllShutDown - Reserved
  BOOLEAN               SataHotRemovalEnh;              /// SataHotRemovalEnh - Reserved

  SATA_PORT_ST          SataHotRemovalEnhPort;          ///<  SataHotRemovalEnhPort - Hot Remove

  BOOLEAN               SataOobDetectionEnh;            /// SataOobDetectionEnh - TRUE
  BOOLEAN               SataPowerSavingEnh;             /// SataPowerSavingEnh - TRUE
  UINT8                 SataMemoryPowerSaving;          /// SataMemoryPowerSaving - 0-3 Default [3]
  BOOLEAN               SataRasSupport;                 /// SataRasSupport - Support RAS function TRUE: Enable FALSE: Disable
  BOOLEAN               SataAhciDisPrefetchFunction;    /// SataAhciDisPrefetchFunction - Disable AHCI Prefetch Function Support
  BOOLEAN               SataDevSlpPort0;                /// SataDevSlpPort0 - Reserved
  BOOLEAN               SataDevSlpPort1;                /// SataDevSlpPort1 - Reserved
//  UINT8                 SataDevSlpPort0S5Pin;           /// SataDevSlpPort0S5Pin - Reserved
//  UINT8                 SataDevSlpPort1S5Pin;           /// SataDevSlpPort1S5Pin - Reserved
  UINT8                 SataDbgTX_DRV_STR ;           /// TX_DRV_STR - Reserved
  UINT8                 SataDbgTX_DE_EMPH_STR ;           /// TX_DE_EMPH_STR - Reserved
  UINT32                TempMmio;                       /// TempMmio - Reserved
} FCH_SATA;


//
// IMC Message Register Software Interface
//
#define CPU_MISC_BUS_DEV_FUN          ((0x18 << 3) + 3)

#define MSG_SYS_TO_IMC                0x80
#define Fun_80                        0x80
#define Fun_81                        0x81
#define Fun_82                        0x82
#define Fun_83                        0x83
#define Fun_84                        0x84
#define Fun_85                        0x85
#define Fun_86                        0x86
#define Fun_87                        0x87
#define Fun_88                        0x88
#define Fun_89                        0x89
#define Fun_90                        0x90
#define MSG_IMC_TO_SYS                0x81
#define MSG_REG0                      0x82
#define MSG_REG1                      0x83
#define MSG_REG2                      0x84
#define MSG_REG3                      0x85
#define MSG_REG4                      0x86
#define MSG_REG5                      0x87
#define MSG_REG6                      0x88
#define MSG_REG7                      0x89
#define MSG_REG8                      0x8A
#define MSG_REG9                      0x8B
#define MSG_REGA                      0x8C
#define MSG_REGB                      0x8D
#define MSG_REGC                      0x8E
#define MSG_REGD                      0x8F

#define DISABLED                      0
#define ENABLED                       1



///
/// EC structure
///
typedef struct _FCH_EC {
  UINT8                 MsgFun81Zone0MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun81Zone0MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun81Zone0MsgReg2;           ///<Thermal zone control byte 1
  UINT8                 MsgFun81Zone0MsgReg3;           ///<Thermal zone control byte 2
  UINT8                 MsgFun81Zone0MsgReg4;           ///<Bit[3:0] - Thermal diode offset adjustment in degrees Celsius.
  UINT8                 MsgFun81Zone0MsgReg5;           ///<Hysteresis information
  UINT8                 MsgFun81Zone0MsgReg6;           ///<SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032
  UINT8                 MsgFun81Zone0MsgReg7;           ///<Bit[1:0]: 0 - 2,  SMBUS bus number where the SMBUS based temperature sensor is located.
  UINT8                 MsgFun81Zone0MsgReg8;           ///<Fan PWM stepping rate in unit of PWM level percentage
  UINT8                 MsgFun81Zone0MsgReg9;           ///<Fan PWM ramping rate in 5ms unit
//
// EC LDN9 function 81 zone 1
//
  UINT8                 MsgFun81Zone1MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun81Zone1MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun81Zone1MsgReg2;           ///<Thermal zone control byte 1
  UINT8                 MsgFun81Zone1MsgReg3;           ///<Thermal zone control byte 2
  UINT8                 MsgFun81Zone1MsgReg4;           ///<Bit[3:0] - Thermal diode offset adjustment in degrees Celsius.
  UINT8                 MsgFun81Zone1MsgReg5;           ///<Hysteresis information
  UINT8                 MsgFun81Zone1MsgReg6;           ///<SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032
  UINT8                 MsgFun81Zone1MsgReg7;           ///<Bit[1:0]: 0 - 2,  SMBUS bus number where the SMBUS based temperature sensor is located.
  UINT8                 MsgFun81Zone1MsgReg8;           ///<Fan PWM stepping rate in unit of PWM level percentage
  UINT8                 MsgFun81Zone1MsgReg9;           ///<Fan PWM ramping rate in 5ms unit
//
//EC LDN9 function 81 zone 2
//
  UINT8                 MsgFun81Zone2MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun81Zone2MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun81Zone2MsgReg2;           ///<Thermal zone control byte 1
  UINT8                 MsgFun81Zone2MsgReg3;           ///<Thermal zone control byte 2
  UINT8                 MsgFun81Zone2MsgReg4;           ///<Bit[3:0] - Thermal diode offset adjustment in degrees Celsius.
  UINT8                 MsgFun81Zone2MsgReg5;           ///<Hysteresis information
  UINT8                 MsgFun81Zone2MsgReg6;           ///<SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032
  UINT8                 MsgFun81Zone2MsgReg7;           ///<Bit[1:0]: 0 - 2,  SMBUS bus number where the SMBUS based temperature sensor is located.
  UINT8                 MsgFun81Zone2MsgReg8;           ///<Fan PWM stepping rate in unit of PWM level percentage
  UINT8                 MsgFun81Zone2MsgReg9;           ///<Fan PWM ramping rate in 5ms unit
//
//EC LDN9 function 81 zone 3
//
  UINT8                 MsgFun81Zone3MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun81Zone3MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun81Zone3MsgReg2;           ///<Thermal zone control byte 1
  UINT8                 MsgFun81Zone3MsgReg3;           ///<Thermal zone control byte 2
  UINT8                 MsgFun81Zone3MsgReg4;           ///<Bit[3:0] - Thermal diode offset adjustment in degrees Celsius.
  UINT8                 MsgFun81Zone3MsgReg5;           ///<Hysteresis information
  UINT8                 MsgFun81Zone3MsgReg6;           ///<SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032
  UINT8                 MsgFun81Zone3MsgReg7;           ///<Bit[1:0]: 0 - 2,  SMBUS bus number where the SMBUS based temperature sensor is located.
  UINT8                 MsgFun81Zone3MsgReg8;           ///<Fan PWM stepping rate in unit of PWM level percentage
  UINT8                 MsgFun81Zone3MsgReg9;           ///<Fan PWM ramping rate in 5ms unit
//
//EC LDN9 function 83 zone 0
//
  UINT8                 MsgFun83Zone0MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun83Zone0MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun83Zone0MsgReg2;           ///<_AC0
  UINT8                 MsgFun83Zone0MsgReg3;           ///<_AC1
  UINT8                 MsgFun83Zone0MsgReg4;           ///<_AC2
  UINT8                 MsgFun83Zone0MsgReg5;           ///<_AC3
  UINT8                 MsgFun83Zone0MsgReg6;           ///<_AC4
  UINT8                 MsgFun83Zone0MsgReg7;           ///<_AC5
  UINT8                 MsgFun83Zone0MsgReg8;           ///<_AC6
  UINT8                 MsgFun83Zone0MsgReg9;           ///<_AC7
  UINT8                 MsgFun83Zone0MsgRegA;           ///<_CRT
  UINT8                 MsgFun83Zone0MsgRegB;           ///<_PSV
//
//EC LDN9 function 83 zone 1
//
  UINT8                 MsgFun83Zone1MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun83Zone1MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun83Zone1MsgReg2;           ///<_AC0
  UINT8                 MsgFun83Zone1MsgReg3;           ///<_AC1
  UINT8                 MsgFun83Zone1MsgReg4;           ///<_AC2
  UINT8                 MsgFun83Zone1MsgReg5;           ///<_AC3
  UINT8                 MsgFun83Zone1MsgReg6;           ///<_AC4
  UINT8                 MsgFun83Zone1MsgReg7;           ///<_AC5
  UINT8                 MsgFun83Zone1MsgReg8;           ///<_AC6
  UINT8                 MsgFun83Zone1MsgReg9;           ///<_AC7
  UINT8                 MsgFun83Zone1MsgRegA;           ///<_CRT
  UINT8                 MsgFun83Zone1MsgRegB;           ///<_PSV
//
//EC LDN9 function 83 zone 2
//
  UINT8                 MsgFun83Zone2MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun83Zone2MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun83Zone2MsgReg2;           ///<_AC0
  UINT8                 MsgFun83Zone2MsgReg3;           ///<_AC1
  UINT8                 MsgFun83Zone2MsgReg4;           ///<_AC2
  UINT8                 MsgFun83Zone2MsgReg5;           ///<_AC3
  UINT8                 MsgFun83Zone2MsgReg6;           ///<_AC4
  UINT8                 MsgFun83Zone2MsgReg7;           ///<_AC5
  UINT8                 MsgFun83Zone2MsgReg8;           ///<_AC6
  UINT8                 MsgFun83Zone2MsgReg9;           ///<_AC7
  UINT8                 MsgFun83Zone2MsgRegA;           ///<_CRT
  UINT8                 MsgFun83Zone2MsgRegB;           ///<_PSV
//
//EC LDN9 function 83 zone 3
//
  UINT8                 MsgFun83Zone3MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun83Zone3MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun83Zone3MsgReg2;           ///<_AC0
  UINT8                 MsgFun83Zone3MsgReg3;           ///<_AC1
  UINT8                 MsgFun83Zone3MsgReg4;           ///<_AC2
  UINT8                 MsgFun83Zone3MsgReg5;           ///<_AC3
  UINT8                 MsgFun83Zone3MsgReg6;           ///<_AC4
  UINT8                 MsgFun83Zone3MsgReg7;           ///<_AC5
  UINT8                 MsgFun83Zone3MsgReg8;           ///<_AC6
  UINT8                 MsgFun83Zone3MsgReg9;           ///<_AC7
  UINT8                 MsgFun83Zone3MsgRegA;           ///<_CRT
  UINT8                 MsgFun83Zone3MsgRegB;           ///<_PSV
//
//EC LDN9 function 85 zone 0
//
  UINT8                 MsgFun85Zone0MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun85Zone0MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun85Zone0MsgReg2;           ///<AL0 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone0MsgReg3;           ///<AL1 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone0MsgReg4;           ///<AL2 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone0MsgReg5;           ///<AL3 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone0MsgReg6;           ///<AL4 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone0MsgReg7;           ///<AL5 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone0MsgReg8;           ///<AL6 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone0MsgReg9;           ///<AL7 PWM level in percentage (0 - 100%)
//
//EC LDN9 function 85 zone 1
//
  UINT8                 MsgFun85Zone1MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun85Zone1MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun85Zone1MsgReg2;           ///<AL0 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone1MsgReg3;           ///<AL1 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone1MsgReg4;           ///<AL2 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone1MsgReg5;           ///<AL3 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone1MsgReg6;           ///<AL4 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone1MsgReg7;           ///<AL5 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone1MsgReg8;           ///<AL6 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone1MsgReg9;           ///<AL7 PWM level in percentage (0 - 100%)
//
//EC LDN9 function 85 zone 2
//
  UINT8                 MsgFun85Zone2MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun85Zone2MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun85Zone2MsgReg2;           ///<AL0 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone2MsgReg3;           ///<AL1 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone2MsgReg4;           ///<AL2 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone2MsgReg5;           ///<AL3 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone2MsgReg6;           ///<AL4 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone2MsgReg7;           ///<AL5 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone2MsgReg8;           ///<AL6 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone2MsgReg9;           ///<AL7 PWM level in percentage (0 - 100%)
//
//EC LDN9 function 85 zone 3
//
  UINT8                 MsgFun85Zone3MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun85Zone3MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun85Zone3MsgReg2;           ///<AL0 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone3MsgReg3;           ///<AL1 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone3MsgReg4;           ///<AL2 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone3MsgReg5;           ///<AL3 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone3MsgReg6;           ///<AL4 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone3MsgReg7;           ///<AL5 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone3MsgReg8;           ///<AL6 PWM level in percentage (0 - 100%)
  UINT8                 MsgFun85Zone3MsgReg9;           ///<AL7 PWM level in percentage (0 - 100%)
//
//EC LDN9 function 89 TEMPIN channel 0
//
  UINT8                 MsgFun89Zone0MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun89Zone0MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun89Zone0MsgReg2;           ///<At DWORD bit 0-7
  UINT8                 MsgFun89Zone0MsgReg3;           ///<At DWORD bit 15-8
  UINT8                 MsgFun89Zone0MsgReg4;           ///<At DWORD bit 23-16
  UINT8                 MsgFun89Zone0MsgReg5;           ///<At DWORD bit 31-24
  UINT8                 MsgFun89Zone0MsgReg6;           ///<Ct DWORD bit 0-7
  UINT8                 MsgFun89Zone0MsgReg7;           ///<Ct DWORD bit 15-8
  UINT8                 MsgFun89Zone0MsgReg8;           ///<Ct DWORD bit 23-16
  UINT8                 MsgFun89Zone0MsgReg9;           ///<Ct DWORD bit 31-24
  UINT8                 MsgFun89Zone0MsgRegA;           ///<Mode bit 0-7
//
//EC LDN9 function 89 TEMPIN channel 1
//
  UINT8                 MsgFun89Zone1MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun89Zone1MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun89Zone1MsgReg2;           ///<At DWORD bit 0-7
  UINT8                 MsgFun89Zone1MsgReg3;           ///<At DWORD bit 15-8
  UINT8                 MsgFun89Zone1MsgReg4;           ///<At DWORD bit 23-16
  UINT8                 MsgFun89Zone1MsgReg5;           ///<At DWORD bit 31-24
  UINT8                 MsgFun89Zone1MsgReg6;           ///<Ct DWORD bit 0-7
  UINT8                 MsgFun89Zone1MsgReg7;           ///<Ct DWORD bit 15-8
  UINT8                 MsgFun89Zone1MsgReg8;           ///<Ct DWORD bit 23-16
  UINT8                 MsgFun89Zone1MsgReg9;           ///<Ct DWORD bit 31-24
  UINT8                 MsgFun89Zone1MsgRegA;           ///<Mode bit 0-7
//
//EC LDN9 function 89 TEMPIN channel 2
//
  UINT8                 MsgFun89Zone2MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun89Zone2MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun89Zone2MsgReg2;           ///<At DWORD bit 0-7
  UINT8                 MsgFun89Zone2MsgReg3;           ///<At DWORD bit 15-8
  UINT8                 MsgFun89Zone2MsgReg4;           ///<At DWORD bit 23-16
  UINT8                 MsgFun89Zone2MsgReg5;           ///<At DWORD bit 31-24
  UINT8                 MsgFun89Zone2MsgReg6;           ///<Ct DWORD bit 0-7
  UINT8                 MsgFun89Zone2MsgReg7;           ///<Ct DWORD bit 15-8
  UINT8                 MsgFun89Zone2MsgReg8;           ///<Ct DWORD bit 23-16
  UINT8                 MsgFun89Zone2MsgReg9;           ///<Ct DWORD bit 31-24
  UINT8                 MsgFun89Zone2MsgRegA;           ///<Mode bit 0-7
//
//EC LDN9 function 89 TEMPIN channel 3
//
  UINT8                 MsgFun89Zone3MsgReg0;           ///<Thermal zone
  UINT8                 MsgFun89Zone3MsgReg1;           ///<Thermal zone
  UINT8                 MsgFun89Zone3MsgReg2;           ///<At DWORD bit 0-7
  UINT8                 MsgFun89Zone3MsgReg3;           ///<At DWORD bit 15-8
  UINT8                 MsgFun89Zone3MsgReg4;           ///<At DWORD bit 23-16
  UINT8                 MsgFun89Zone3MsgReg5;           ///<At DWORD bit 31-24
  UINT8                 MsgFun89Zone3MsgReg6;           ///<Ct DWORD bit 0-7
  UINT8                 MsgFun89Zone3MsgReg7;           ///<Ct DWORD bit 15-8
  UINT8                 MsgFun89Zone3MsgReg8;           ///<Ct DWORD bit 23-16
  UINT8                 MsgFun89Zone3MsgReg9;           ///<Ct DWORD bit 31-24
  UINT8                 MsgFun89Zone3MsgRegA;           ///<Mode bit 0-7
//
// FLAG for Fun83/85/89 support
//
  UINT16                IMCFUNSupportBitMap;            ///< Bit0=81FunZone0 support(1=On;0=Off); bit1-3=81FunZone1-Zone3;Bit4-7=83FunZone0-Zone3;Bit8-11=85FunZone0-Zone3;Bit11-15=89FunZone0-Zone3;
} FCH_EC;

///
/// IMC structure
///
typedef struct _FCH_IMC {
  UINT8                 ImcEnable;                      ///< ImcEnable - IMC Enable
  UINT8                 ImcEnabled;                     ///< ImcEnabled - IMC Enable
  UINT8                 ImcSureBootTimer;               ///< ImcSureBootTimer - IMc SureBootTimer function
  FCH_EC                EcStruct;                       ///< EC structure
  UINT8                 ImcEnableOverWrite;             ///< OverWrite IMC with the EC structure
                                                        ///   @li <b>00</b> - by default strapping
                                                        ///   @li <b>01</b> - enable
                                                        ///   @li <b>10</b> - disable
                                                        ///
} FCH_IMC;


///
/// Hpet structure
///
typedef struct {
  BOOLEAN               HpetEnable;                     ///< HPET function switch

  BOOLEAN               HpetMsiDis;                     ///< HpetMsiDis - South Bridge HPET MSI Configuration
                                                        ///   @par
                                                        ///   @li <b>1</b> - disable
                                                        ///   @li <b>0</b> - enable

  UINT32                HpetBase;                       ///< HpetBase
                                                        ///   @par
                                                        ///  HPET Base address
} FCH_HPET;


///
/// GCPU related parameters
///
typedef struct {
  UINT8                 AcDcMsg;                        ///< Send a message to CPU to indicate the power mode (AC vs battery)
                                                        ///   @li <b>1</b> - disable
                                                        ///   @li <b>0</b> - enable

  UINT8                 TimerTickTrack;                 ///< Send a message to CPU to indicate the latest periodic timer interval
                                                        ///   @li <b>1</b> - disable
                                                        ///   @li <b>0</b> - enable

  UINT8                 ClockInterruptTag;              ///< Mark the periodic timer interrupt
                                                        ///   @li <b>1</b> - disable
                                                        ///   @li <b>0</b> - enable

  UINT8                 OhciTrafficHanding;             ///< Cause CPU to break out from C state when USB OHCI has pending traffic
                                                        ///   @li <b>1</b> - disable
                                                        ///   @li <b>0</b> - enable

  UINT8                 EhciTrafficHanding;             ///< Cause CPU to break out from C state when USB EHCI has pending traffic
                                                        ///   @li <b>1</b> - disable
                                                        ///   @li <b>0</b> - enable

  UINT8                 GcpuMsgCMultiCore;              ///< Track of CPU C state by monitoring each core's C state message
                                                        ///   @li <b>1</b> - disable
                                                        ///   @li <b>0</b> - enable

  UINT8                 GcpuMsgCStage;                  ///< Enable the FCH C state coordination logic
                                                        ///   @li <b>1</b> - disable
                                                        ///   @li <b>0</b> - enable
} FCH_GCPU;


///
/// Timer
///
typedef struct {
  BOOLEAN               Enable;                         ///< Whether to register timer SMI in POST
  BOOLEAN               StartNow;                       ///< Whether to start the SMI immediately during registration
  UINT16                CycleDuration;                  ///< [14:0] - Actual cycle duration = CycleDuration + 1
} TIMER_SMI;

///
/// CS support
///
typedef struct {
  BOOLEAN               FchCsD3Cold;                    ///< FCH Cs D3 Cold function
  BOOLEAN               FchCsHwReduced;                 ///< FCH Cs hardware reduced ACPI flag
  BOOLEAN               FchCsPwrBtn;                    ///< FCH Cs Power Button function
  BOOLEAN               FchCsAcDc;                      ///< FCH Cs AcDc function
  BOOLEAN               AsfNfcEnable;                   ///< FCH Cs NFC function
  UINT8                 AsfNfcInterruptPin;             ///<    NFC Interrupt pin define
  UINT8                 AsfNfcRegPuPin;                 ///<    NFC RegPu pin define
  UINT8                 AsfNfcWakePin;                  ///<    NFC Wake Pin define
  UINT8                 PowerButtonGpe;                 ///<    GPE# used by Power Button device
  UINT8                 AcDcTimerGpe;                   ///<    GPE# used by Timer device
} FCH_CS;


///
/// MISC structure
///
typedef struct {
  BOOLEAN               NativePcieSupport;              /// PCIe NativePcieSupport - Debug function. 1:Enabled, 0:Disabled
  BOOLEAN               S3Resume;                       /// S3Resume - Flag of ACPI S3 Resume.
  BOOLEAN               RebootRequired;                 /// RebootRequired - Flag of Reboot system is required.
  UINT8                 FchVariant;                     /// FchVariant - FCH Variant value.
  UINT8                 Cg2Pll;                         ///< CG2 PLL - 0:disable, 1:enable
  TIMER_SMI             LongTimer;                      ///< Long Timer SMI
  TIMER_SMI             ShortTimer;                     ///< Short Timer SMI
  UINT32                FchCpuId;                       ///< Saving CpuId for FCH Module.
  BOOLEAN               NoneSioKbcSupport;              ///< NoneSioKbcSupport - No KBC/SIO controller ( Turn on Inchip KBC emulation function )
  FCH_CS                FchCsSupport;                   ///< FCH Cs function structure
} FCH_MISC;


///
/// SMBus structure
///
typedef struct {
  UINT32                SmbusSsid;                      ///< SMBUS controller Subsystem ID
} FCH_SMBUS;


///
/// Acpi structure
///
typedef struct {
  UINT16                Smbus0BaseAddress;              ///< Smbus0BaseAddress
                                                        ///   @par
                                                        ///  Smbus BASE Address
                                                        ///
  UINT16                Smbus1BaseAddress;              ///< Smbus1BaseAddress
                                                        ///   @par
                                                        ///  Smbus1 (ASF) BASE Address
                                                        ///
  UINT16                SioPmeBaseAddress;              ///< SioPmeBaseAddress
                                                        ///   @par
                                                        ///  SIO PME BASE Address
                                                        ///
  UINT32                WatchDogTimerBase;              ///< WatchDogTimerBase
                                                        ///   @par
                                                        ///  Watch Dog Timer Address
                                                        ///
  UINT16                AcpiPm1EvtBlkAddr;              ///< AcpiPm1EvtBlkAddr
                                                        ///   @par
                                                        ///  ACPI PM1 event block Address
                                                        ///
  UINT16                AcpiPm1CntBlkAddr;              ///< AcpiPm1CntBlkAddr
                                                        ///   @par
                                                        ///  ACPI PM1 Control block Address
                                                        ///
  UINT16                AcpiPmTmrBlkAddr;               ///< AcpiPmTmrBlkAddr
                                                        ///   @par
                                                        ///  ACPI PM timer block Address
                                                        ///
  UINT16                CpuControlBlkAddr;              ///< CpuControlBlkAddr
                                                        ///   @par
                                                        ///  ACPI CPU control block Address
                                                        ///
  UINT16                AcpiGpe0BlkAddr;                ///< AcpiGpe0BlkAddr
                                                        ///   @par
                                                        ///  ACPI GPE0 block Address
                                                        ///
  UINT16                SmiCmdPortAddr;                 ///< SmiCmdPortAddr
                                                        ///   @par
                                                        ///  SMI command port Address
                                                        ///
  UINT16                AcpiPmaCntBlkAddr;              ///< AcpiPmaCntBlkAddr
                                                        ///   @par
                                                        ///  ACPI PMA Control block Address
                                                        ///
  BOOLEAN               AnyHt200MhzLink;                ///< AnyHt200MhzLink
                                                        ///   @par
                                                        ///  HT Link Speed on 200MHz option for each CPU specific LDTSTP# (Force enable)
                                                        ///
  BOOLEAN               SpreadSpectrum;                 ///< SpreadSpectrum
                                                        ///  @par
                                                        ///  Spread Spectrum function
                                                        ///   @li <b>0</b> - disable
                                                        ///   @li <b>1</b> - enable
                                                        ///
  POWER_FAIL            PwrFailShadow;                  ///< PwrFailShadow = PM_Reg: 5Bh [3:0]
                                                        ///  @par
                                                        ///   @li  <b>00</b> - Always off
                                                        ///   @li  <b>01</b> - Always on
                                                        ///   @li  <b>11</b> - Use previous
                                                        ///
  UINT8                 StressResetMode;                ///< StressResetMode 01-10
                                                        ///   @li  <b>00</b> - Disabed
                                                        ///   @li  <b>01</b> - Io Write 0x64 with 0xfe
                                                        ///   @li  <b>10</b> - Io Write 0xcf9 with 0x06
                                                        ///   @li  <b>11</b> - Io Write 0xcf9 with 0x0e
                                                        ///
  BOOLEAN               MtC1eEnable;                    /// MtC1eEnable - Enable MtC1e
  VOID*                 OemProgrammingTablePtr;         /// Pointer of ACPI OEM table
  UINT8                 SpreadSpectrumOptions;          /// SpreadSpectrumOptions - Spread Spectrum Option
  BOOLEAN               PwrDownDisp2ClkPcieR;           /// Power down DISP2_CLK and PCIE_RCLK_Output for power savings
  BOOLEAN               NoClearThermalTripSts;          /// Skip clearing ThermalTrip status
} FCH_ACPI;


///
/// HWM temp parameter structure
///
typedef struct _FCH_HWM_TEMP_PAR {
  UINT16                At;                             ///< At
  UINT16                Ct;                             ///< Ct
  UINT8                 Mode;                           ///< Mode BIT0:HiRatio BIT1:HiCurrent
} FCH_HWM_TEMP_PAR;

///
/// HWM Current structure
///
typedef struct _FCH_HWM_CUR {
  UINT16                FanSpeed[5];                    ///< FanSpeed    - fan Speed
  UINT16                Temperature[5];                 ///< Temperature - temperature
  UINT16                Voltage[8];                     ///< Voltage     - voltage
} FCH_HWM_CUR;

///
/// HWM fan control structure
///
typedef struct _FCH_HWM_FAN_CTR {
  UINT8                 InputControlReg00;              /// Fan Input Control register, PM2 offset [0:4]0
  UINT8                 ControlReg01;                   /// Fan control register, PM2 offset [0:4]1
  UINT8                 FreqReg02;                      /// Fan frequency register, PM2 offset [0:4]2
  UINT8                 LowDutyReg03;                   /// Low Duty register, PM2 offset [0:4]3
  UINT8                 MedDutyReg04;                   /// Med Duty register, PM2 offset [0:4]4
  UINT8                 MultiplierReg05;                /// Multiplier register, PM2 offset [0:4]5
  UINT16                LowTempReg06;                   /// Low Temp register, PM2 offset [0:4]6
  UINT16                MedTempReg08;                   /// Med Temp register, PM2 offset [0:4]8
  UINT16                HighTempReg0A;                  /// High Temp register, PM2 offset [0:4]A
  UINT8                 LinearRangeReg0C;               /// Linear Range register, PM2 offset [0:4]C
  UINT8                 LinearHoldCountReg0D;           /// Linear Hold Count register, PM2 offset [0:4]D
} FCH_HWM_FAN_CTR;

///
/// Hwm structure
///
typedef struct _FCH_HWM {
  UINT8                 HwMonitorEnable;                ///< HwMonitorEnable
  UINT32                HwmControl;                     ///< hwmControl
                                                        ///   @par
                                                        ///  HWM control configuration
                                                        ///   @li <b>0</b> - HWM is Enabled
                                                        ///   @li <b>1</b> - IMC is Enabled
                                                        ///
  UINT8                 FanSampleFreqDiv;               ///< Sampling rate of Fan Speed
                                                        ///   @li <b>00</b> - Base(22.5KHz)
                                                        ///   @li <b>01</b> - Base(22.5KHz)/2
                                                        ///   @li <b>10</b> - Base(22.5KHz)/4
                                                        ///   @li <b>11</b> - Base(22.5KHz)/8
                                                        ///
  UINT8                 HwmFchtsiAutoPoll;              ///< TSI Auto Polling
                                                        ///   @li <b>0</b> - disable
                                                        ///   @li <b>1</b> - enable
                                                        ///
  UINT8                 HwmFchtsiAutoPollStarted;       ///< HwmSbtsiAutoPollStarted
  UINT8                 FanLinearEnhanceEn;             ///< FanLinearEnhanceEn
  UINT8                 FanLinearHoldFix;               ///< FanLinearHoldFix
  UINT8                 FanLinearRangeOutLimit;         ///< FanLinearRangeOutLimit
  UINT16                HwmCalibrationFactor;           /// Calibration Factor
  FCH_HWM_CUR           HwmCurrent;                     /// HWM Current structure
  FCH_HWM_CUR           HwmCurrentRaw;                  /// HWM Current Raw structure
  FCH_HWM_TEMP_PAR      HwmTempPar[5];                  /// HWM Temp parameter structure
  FCH_HWM_FAN_CTR       HwmFanControl[5];               /// HWM Fan Control structure
  FCH_HWM_FAN_CTR       HwmFanControlCooked[5];               /// HWM Fan Control structure
} FCH_HWM;


///
/// Gec structure
///
typedef struct {
  BOOLEAN               GecEnable;                      ///< GecEnable - GEC function switch
  UINT8                 GecPhyStatus;                   /// GEC PHY Status
  UINT8                 GecPowerPolicy;                 /// GEC Power Policy
                                                        ///   @li <b>00</b> - GEC is powered down in S3 and S5
                                                        ///   @li <b>01</b> - GEC is powered down only in S5
                                                        ///   @li <b>10</b> - GEC is powered down only in S3
                                                        ///   @li <b>11</b> - GEC is never powered down
                                                        ///
  UINT8                 GecDebugBus;                    /// GEC Debug Bus
                                                        ///   @li <b>0</b> - disable
                                                        ///   @li <b>1</b> - enable
                                                        ///
  UINT32                GecShadowRomBase;               ///< GecShadowRomBase
                                                        ///   @par
                                                        ///  GEC (NIC) SHADOWROM BASE Address
                                                        ///
  VOID                  *PtrDynamicGecRomAddress;       /// Pointer of Dynamic GEC ROM Address
} FCH_GEC;


///
/// _ABTblEntry - AB link register table R/W structure
///
typedef struct _AB_TBL_ENTRY {
  UINT8                 RegType;                        /// RegType  : AB Register Type (ABCFG, AXCFG and so on)
  UINT32                RegIndex;                       /// RegIndex : AB Register Index
  UINT32                RegMask;                        /// RegMask  : AB Register Mask
  UINT32                RegData;                        /// RegData  : AB Register Data
} AB_TBL_ENTRY;

///
/// AB structure
///
typedef struct {
  BOOLEAN               AbMsiEnable;                    ///< ABlink MSI capability
  UINT8                 ALinkClkGateOff;                /// Alink Clock Gate-Off function - 0:disable, 1:enable *KR
  UINT8                 BLinkClkGateOff;                /// Blink Clock Gate-Off function - 0:disable, 1:enable *KR
  UINT8                 GppClockRequest0;               /// GPP Clock Request.
  UINT8                 GppClockRequest1;               /// GPP Clock Request.
  UINT8                 GppClockRequest2;               /// GPP Clock Request.
  UINT8                 GppClockRequest3;               /// GPP Clock Request.
  UINT8                 GfxClockRequest;                /// GPP Clock Request.
  UINT8                 AbClockGating;                  /// AB Clock Gating - 0:disable, 1:enable *KR *CZ
  UINT8                 GppClockGating;                 /// GPP Clock Gating - 0:disable, 1:enable
  UINT8                 UmiL1TimerOverride;             /// UMI L1 inactivity timer overwrite value
  UINT8                 UmiLinkWidth;                   /// UMI Link Width
  UINT8                 UmiDynamicSpeedChange;          /// UMI Dynamic Speed Change - 0:disable, 1:enable
  UINT8                 PcieRefClockOverClocking;       /// PCIe Ref Clock OverClocking value
  UINT8                 UmiGppTxDriverStrength;         /// UMI GPP TX Driver Strength
  BOOLEAN               NbSbGen2;                       /// UMI link Gen2 - 0:Gen1, 1:Gen2
  UINT8                 PcieOrderRule;                  /// PCIe Order Rule - 0:disable, 1:enable *KR AB Posted Pass Non-Posted
  UINT8                 SlowSpeedAbLinkClock;           /// Slow Speed AB Link Clock - 0:disable, 1:enable *KR
  BOOLEAN               ResetCpuOnSyncFlood;            /// Reset Cpu On Sync Flood - 0:disable, 1:enable *KR
  BOOLEAN               AbDmaMemoryWrtie3264B;          /// AB DMA Memory Write 32/64 BYTE Support *KR only
  BOOLEAN               AbMemoryPowerSaving;            /// AB Memory Power Saving *KR *CZ
  BOOLEAN               SbgDmaMemoryWrtie3264ByteCount; /// SBG DMA Memory Write 32/64 BYTE Count Support *KR only
  BOOLEAN               SbgMemoryPowerSaving;           /// SBG Memory Power Saving *KR *CZ
  BOOLEAN               SbgClockGating;                 /// SBG Clock Gate *CZ
  BOOLEAN               XdmaDmaWrite16ByteMode;         /// XDMA DMA Write 16 byte mode *CZ
  BOOLEAN               XdmaMemoryPowerSaving;          /// XDMA memory power saving *CZ
  UINT8                 XdmaPendingNprThreshold;        /// XDMA PENDING NPR THRESHOLD *CZ
  BOOLEAN               XdmaDncplOrderDis;              /// XDMA DNCPL ORDER DIS *CZ
} FCH_AB;


/**
 * PCIE_CAP_ID - PCIe Cap ID
 *
 */
#define  PCIE_CAP_ID    0x10

///
/// FCH_GPP_PORT_CONFIG - Fch GPP port config structure
///
typedef struct {
  BOOLEAN               PortPresent;                    ///< Port connection
                                                        ///  @par
                                                        ///  @li <b>0</b> - Port doesn't have slot. No need to train the link
                                                        ///  @li <b>1</b> - Port connection defined and needs to be trained
                                                        ///
  BOOLEAN               PortDetected;                   ///< Link training status
                                                        ///  @par
                                                        ///  @li <b>0</b> - EP not detected
                                                        ///  @li <b>1</b> - EP detected
                                                        ///
  BOOLEAN               PortIsGen2;                     ///< Port link speed configuration
                                                        ///  @par
                                                        ///  @li <b>00</b> - Auto
                                                        ///  @li <b>01</b> - Forced GEN1
                                                        ///  @li <b>10</b> - Forced GEN2
                                                        ///  @li <b>11</b> - Reserved
                                                        ///
  BOOLEAN               PortHotPlug;                    ///< Support hot plug?
                                                        ///  @par
                                                        ///  @li <b>0</b> - No support
                                                        ///  @li <b>1</b> - support
                                                        ///
  UINT8                 PortMisc;                       ///  PortMisc - Reserved
} FCH_GPP_PORT_CONFIG;

///
/// GPP structure
///
typedef struct {
  FCH_GPP_PORT_CONFIG   PortCfg[4];                     /// GPP port configuration structure
  GPP_LINKMODE          GppLinkConfig;                  ///< GppLinkConfig - PCIE_GPP_Enable[3:0]
                                                        ///  @li  <b>0000</b> - Port ABCD -> 4:0:0:0
                                                        ///  @li  <b>0010</b> - Port ABCD -> 2:2:0:0
                                                        ///  @li  <b>0011</b> - Port ABCD -> 2:1:1:0
                                                        ///  @li  <b>0100</b> - Port ABCD -> 1:1:1:1
                                                        ///
  BOOLEAN               GppFunctionEnable;              ///< GPP Function - 0:disable, 1:enable
  BOOLEAN               GppToggleReset;                 ///< Toggle GPP core reset
  UINT8                 GppHotPlugGeventNum;            ///< Hotplug GEVENT # - valid value 0-31
  UINT8                 GppFoundGfxDev;                 ///< Gpp Found Gfx Device
                                                        ///  @li   <b>0</b> - Not found
                                                        ///  @li   <b>1</b> - Found
                                                        ///
  BOOLEAN               GppGen2;                        ///< GPP Gen2 - 0:disable, 1:enable
  UINT8                 GppGen2Strap;                   ///< GPP Gen2 Strap - 0:disable, 1:enable, FCH itself uses this
  BOOLEAN               GppMemWrImprove;                ///< GPP Memory Write Improve - 0:disable, 1:enable
  BOOLEAN               GppUnhidePorts;                 ///< GPP Unhide Ports - 0:disable, 1:enable
  UINT8                 GppPortAspm;                    ///< GppPortAspm - ASPM state for all GPP ports
                                                        ///  @li   <b>01</b> - Disabled
                                                        ///  @li   <b>01</b> - L0s
                                                        ///  @li   <b>10</b> - L1
                                                        ///  @li   <b>11</b> - L0s + L1
                                                        ///
  BOOLEAN               GppLaneReversal;                ///< GPP Lane Reversal - 0:disable, 1:enable
  BOOLEAN               GppPhyPllPowerDown;             ///< GPP PHY PLL Power Down - 0:disable, 1:enable
  BOOLEAN               GppDynamicPowerSaving;          ///< GPP Dynamic Power Saving - 0:disable, 1:enable
  BOOLEAN               PcieAer;                        ///< PcieAer - Advanced Error Report: 0/1-disable/enable
  BOOLEAN               PcieRas;                        ///< PCIe RAS - 0:disable, 1:enable
  BOOLEAN               PcieCompliance;                 ///< PCIe Compliance - 0:disable, 1:enable
  BOOLEAN               PcieSoftwareDownGrade;          ///< PCIe Software Down Grade
  BOOLEAN               UmiPhyPllPowerDown;             ///< UMI PHY PLL Power Down - 0:disable, 1:enable
  BOOLEAN               SerialDebugBusEnable;           ///< Serial Debug Bus Enable
  UINT8                 GppHardwareDownGrade;           ///< GppHardwareDownGrade - Gpp HW Down Grade function 0:Disable, 1-4: portA-D
  UINT8                 GppL1ImmediateAck;              ///< GppL1ImmediateAck - Gpp L1 Immediate ACK 0: enable, 1: disable
  BOOLEAN               NewGppAlgorithm;                ///< NewGppAlgorithm - New GPP procedure
  UINT8                 HotPlugPortsStatus;             ///< HotPlugPortsStatus - Save Hot-Plug Ports Status
  UINT8                 FailPortsStatus;                ///< FailPortsStatus - Save Failure Ports Status
  UINT8                 GppPortMinPollingTime;          ///< GppPortMinPollingTime - Min. Polling time for Gpp Port Training
  BOOLEAN               IsCapsuleMode;                  ///< IsCapsuleMode - Support Capsule Mode in FCH
} FCH_GPP;


///
/// FCH USB3 Debug Sturcture
///
typedef struct {
  BOOLEAN               ServiceIntervalEnable;      ///< Service Interval Enable
  BOOLEAN               BandwidthExpandEnable;      ///< Bandwidth Expand Enable
  BOOLEAN               AoacEnable;                 ///< Aoac Enable
  BOOLEAN               HwLpmEnable;                ///< HwLpm Enable
  BOOLEAN               DbcEnable;                  ///< DBC Enable
  BOOLEAN               MiscPlusEnable;             ///< Misc Plus Enable
  BOOLEAN               EcoFixEnable;               ///< Eco Fix Enable
  BOOLEAN               SsifEnable;                 ///< SSIF Enable
  BOOLEAN               U2ifEnable;                 ///< U2IF Enable
  BOOLEAN               FseEnable;                  ///< FSE Enable
  BOOLEAN               XhcPmeEnable;               ///< Xhc Pme Enable
} USB3_DEBUG;

///
/// FCH IoMux Sturcture
///
typedef struct {
  UINT8         CbsDbgFchSmbusI2c2Egpio;                          ///< SMBUS/I2C_2/EGPIO_113_114
  UINT8         CbsDbgFchAsfI2c3Egpio;                            ///< ASF/I2C_3/EGPIO_019_020
} FCH_IOMUX;

///
/// FCH USB sturcture
///
typedef struct {
  BOOLEAN               Ohci1Enable;                    ///< OHCI1 controller enable
  BOOLEAN               Ohci2Enable;                    ///< OHCI2 controller enable
  BOOLEAN               Ohci3Enable;                    ///< OHCI3 controller enable
  BOOLEAN               Ohci4Enable;                    ///< OHCI4 controller enable
  BOOLEAN               Ehci1Enable;                    ///< EHCI1 controller enable
  BOOLEAN               Ehci2Enable;                    ///< EHCI2 controller enable
  BOOLEAN               Ehci3Enable;                    ///< EHCI3 controller enable
  BOOLEAN               Xhci0Enable;                    ///< XHCI0 controller enable
  BOOLEAN               Xhci1Enable;                    ///< XHCI1 controller enable
  BOOLEAN               UsbMsiEnable;                   ///< USB MSI capability
  UINT32                OhciSsid;                       ///< OHCI SSID
  UINT32                Ohci4Ssid;                      ///< OHCI 4 SSID
  UINT32                EhciSsid;                       ///< EHCI SSID
  UINT32                XhciSsid;                       ///< XHCI SSID
  BOOLEAN               UsbPhyPowerDown;                ///< USB PHY Power Down - 0:disable, 1:enable
  UINT32                UserDefineXhciRomAddr;          ///< XHCI ROM address define by platform BIOS
  UINT8                 Ehci1Phy[5];                    ///< EHCI1 USB PHY Driving Strength value table
  UINT8                 Ehci2Phy[5];                    ///< EHCI2 USB PHY Driving Strength value table
  UINT8                 Ehci3Phy[4];                    ///< EHCI3 USB PHY Driving Strength value table
  UINT8                 Xhci20Phy[4];                   ///< XHCI USB 2.0 PHY Driving Strength value table
  UINT8                 Ehci1DebugPortSel;              ///< DebugPortSel for Ehci1 Hub
                                                        ///  @li   <b>000</b> - Disable
                                                        ///  @li   <b>001</b> - HubDownStreamPort0
                                                        ///  @li   <b>010</b> - HubDownStreamPort1
                                                        ///  @li   <b>011</b> - HubDownStreamPort2
                                                        ///  @li   <b>100</b> - HubDownStreamPort3
  UINT8                 Ehci2DebugPortSel;              ///< DebugPortSel for Ehci2 Hub
                                                        ///  @li   <b>000</b> - Disable
                                                        ///  @li   <b>001</b> - HubDownStreamPort0
                                                        ///  @li   <b>010</b> - HubDownStreamPort1
                                                        ///  @li   <b>011</b> - HubDownStreamPort2
                                                        ///  @li   <b>100</b> - HubDownStreamPort3
  UINT8                 Ehci3DebugPortSel;              ///< DebugPortSel for Ehci3 Hub
                                                        ///  @li   <b>000</b> - Disable
                                                        ///  @li   <b>001</b> - HubDownStreamPort0
                                                        ///  @li   <b>010</b> - HubDownStreamPort1
                                                        ///  @li   <b>011</b> - HubDownStreamPort2
                                                        ///  @li   <b>100</b> - HubDownStreamPort3
  UINT8                 Ehci1NPort;                     ///< N_Port for Ehci1, 2 to 4
  UINT8                 Ehci2NPort;                     ///< N_Port for Ehci2, 2 to 4
  USB3_DEBUG            Usb3Debug;                      ///< Usb3 Debug Options
  BOOLEAN               EhciSimpleDebugPort;            ///< Ehci Simple Debug Port
  BOOLEAN               UsbBatteryChargeEnable;         ///< USB Battery Charge Enable
  BOOLEAN               ReduceUSB3PortToLastTwo;        ///< Reduce USB3.0 ports to last 2
  UINT8                 USB30PortInit;                  ///< USB 3.0 Port Init
} FCH_USB;


/// Private: FCH_DATA_BLOCK_RESET
typedef struct _FCH_RESET_DATA_BLOCK {
  AMD_CONFIG_PARAMS     *StdHeader;                     ///< Header structure
  FCH_RESET_INTERFACE   FchReset;                       ///< Reset interface

  UINT8                 FastSpeed;                      ///< SPI FastSpeed: 1-66MHz, 2-33MHz, 3-22MHz, 4-16.5MHz, 5-100Mhz
  UINT8                 WriteSpeed;                     ///< SPI Write Speed: 1-66MHz, 2-33MHz, 3-22MHz, 4-16.5MHz, 5-100Mhz
  UINT8                 Mode;                           ///< SPI Mode
                                                        ///  @li   <b>101</b> - Qual-io 1-4-4
                                                        ///  @li   <b>100</b> - Dual-io 1-2-2
                                                        ///  @li   <b>011</b> - Qual-io 1-1-4
                                                        ///  @li   <b>010</b> - Dual-io 1-1-2
                                                        ///  @li   <b>111</b> - FastRead
                                                        ///  @li   <b>110</b> - Normal
                                                        ///
  UINT8                 AutoMode;                       ///< SPI Auto Mode - 0:disable, 1:enable
  UINT8                 BurstWrite;                     ///< SPI Burst Write - 0:disable, 1:enable
  BOOLEAN               Sata6AhciCap;                   ///< SATA 6 AHCI Capability - TRUE:enable, FALSE:disable
  UINT8                 Cg2Pll;                         ///< CG2 PLL - 0:disable, 1:enable
  BOOLEAN               EcKbd;                          ///< EC KBD - 0:disable, 1:enable
  BOOLEAN               LegacyFree;                     ///< Legacy Free - 0:disable, 1:enable
  BOOLEAN               SataSetMaxGen2;                 ///< SATA enable maximum GEN2
  UINT8                 SataClkMode;                    ///< SATA reference clock selector and divider
  UINT8                 SataModeReg;                    ///< Output: SATAConfig PMIO:0xDA
  BOOLEAN               SataInternal100Spread;          ///< SATA internal 100MHz spread ON/OFF
  UINT8                 SpiSpeed;                       ///< SPI NormSpeed: 1-66MHz, 2-33MHz, 3-22MHz, 4-16.5MHz, 5-100Mhz
//  UINT32                SPI100_RX_Timing_Config_Register_38;                 ///< SPI100_RX_Timing_Config_Register_38
//  UINT16                SPI100_RX_Timing_Config_Register_3C;                 ///< SPI100_RX_Timing_Config_Register_3C
//  UINT8                 SpiProtectEn0_1d_34;                  ///
  UINT8                 SPI100_Enable;                  ///
  BOOLEAN               EcChannel0;                     ///< Enable EC channel 0
  FCH_GPP               Gpp;                            ///< GPP subsystem
  FCH_SPI               Spi;                            ///< SPI subsystem
  BOOLEAN               QeEnabled;                      /// Quad Mode Enabled
  BOOLEAN               FchOscout1ClkContinous;         ///< FCH OSCOUT1_CLK Continous
  UINT8                 LpcClockDriveStrength;          ///< Lpc Clock Drive Strength
  const VOID*           EarlyOemGpioTable;              /// Pointer of Early OEM GPIO table
//  VOID*                 OemSpiDeviceTable;              /// Pointer of OEM Spi Device table
} FCH_RESET_DATA_BLOCK;


/// Private: FCH_DATA_BLOCK
typedef struct _FCH_DATA_BLOCK {
  AMD_CONFIG_PARAMS     *StdHeader;                     ///< Header structure
  FCH_RUNTIME           FchRunTime;                     ///< FCH Run Time Parameters

  FCH_ACPI              HwAcpi;                         ///< ACPI structure
  FCH_AB                Ab;                             ///< AB structure
  FCH_GPP               Gpp;                            ///< GPP structure
  FCH_USB               Usb;                            ///< USB structure
  FCH_SATA              Sata;                           ///< SATA structure
  FCH_SMBUS             Smbus;                          ///< SMBus structure
  FCH_IDE               Ide;                            ///< IDE structure
  FCH_AZALIA            Azalia;                         ///< Azalia structure
  FCH_SPI               Spi;                            ///< SPI structure
  FCH_PCIB              Pcib;                           ///< PCIB structure
  FCH_GEC               Gec;                            ///< GEC structure
  FCH_SD                Sd;                             ///< SD structure
  FCH_HWM               Hwm;                            ///< Hardware Moniter structure
  FCH_IR                Ir;                             ///< IR structure
  FCH_HPET              Hpet;                           ///< HPET structure
  FCH_GCPU              Gcpu;                           ///< GCPU structure
  FCH_IMC               Imc;                            ///< IMC structure
  FCH_MISC              Misc;                           ///< MISC structure
  FCH_IOMUX             IoMux;                          ///< MISC structure
  VOID*                 PostOemGpioTable;              /// Pointer of Post OEM GPIO table
} FCH_DATA_BLOCK;

#pragma pack (pop)

#endif
