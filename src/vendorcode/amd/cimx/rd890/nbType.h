/*
 * @file
 *
 * Misc definitions.
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

#ifndef _NBTYPE_H_
#define _NBTYPE_H_

//#pragma pack(push, 1) //GCC ERROR

#define CIMX_NB_REVISION  "1.0.1.7"
#define CIMX_NB_ID        "SR5690"

#ifndef MAX_NB_COUNT
  //#define MAX_NB_COUNT    4
  #error hi, MAX_NB_COUNT not define
#endif

#define MAX_PORT_ID       13
#define MIN_PORT_ID       2

#define MAX_CORE_ID       4


typedef  UINT32  PORT;
typedef  UINT32  CORE;



/// The HT Path to Northbridge
typedef  struct {
  UINT8                     NodeID;                 ///< Node ID
  UINT8                     LinkID;                 /**< HT Link ID
                                                      *   @par
                                                      *   LinkID[3:0]- Link Id
                                                      *   @li <b>0</b> - Link 0
                                                      *   @li <b>1</b> - Link 1
                                                      *   @li <b>2</b> - Link 2
                                                      *   @li <b>3</b> - Link 3
                                                      *   @par
                                                      *    LinkID[7:4]- Sublink Id
                                                      *   @li <b>1</b> - Sublink 0
                                                      *   @li <b>2</b> - Sublink 1
                                                      *
                                                      *  @PlatformDependant
                                                      */
} HT_PATH;

/// The configuration structure common header.
typedef  struct {
  UINT16                    Version;                ///< Version of this structure
  UINT16                    Reserved;               ///< Reserved for future use
  UINT32                    InitializerID;          ///< Signature of initializer
} AMD_COMMON_STRUCT_HEADER;

/// The PCIE configuration parameters
typedef  struct {
  AMD_COMMON_STRUCT_HEADER  sHeader;                  ///< Standard structure header
  PCIE_MISC_CONFIG          PcieConfiguration;        ///< General configuration option
  UINT32                    ExtPcieConfiguration;     ///< Extended General configuration option (Reserved for future use)
  UINT32                    CoreConfiguration[5];      /**< Core configuration
                                                       *  @li <b>CoreConfiguration[0]</b> - GPP1 Core Configuration
                                                       *  <TABLE border="0">
                                                       *     <TR><TD class="indexkey" width=120> Dual Port 2x8 </TD><TD class="indexvalue">if CoreConfiguration[0] == 0 && PortConfiguration[3].PortPresent == 1 </TD></TR>
                                                       *     <TR><TD class="indexkey" width=120> Dual Port 2x8 </TD><TD class="indexvalue">CoreConfiguration[0] == 0x02020101</TD></TR>
                                                       *     <TR><TD class="indexkey" width=120> Single Port 1x16 </TD><TD class="indexvalue">if CoreConfiguration[0] == 0 && PortConfiguration[3].PortPresent == 0</TD></TR>
                                                       *     <TR><TD class="indexkey" width=120> Single Port 1x16 </TD><TD class="indexvalue">CoreConfiguration[0] == 0x01010101</TD></TR>
                                                       *  </TABLE>
                                                       *  @li <b>CoreConfiguration[1]</b> - GPP2 Core Configuration
                                                       *  <TABLE border="0">
                                                       *     <TR><TD class="indexkey" width=120> Dual Port 2x8 </TD><TD class="indexvalue">if CoreConfiguration[1] == 0 && PortConfiguration[12].PortPresent == 1 </TD></TR>
                                                       *     <TR><TD class="indexkey" width=120> Dual Port 2x8 </TD><TD class="indexvalue">CoreConfiguration[1] == 0x02020101</TD></TR>
                                                       *     <TR><TD class="indexkey" width=120> Single Port 1x16 </TD><TD class="indexvalue">if CoreConfiguration[1] == 0 && PortConfiguration[12].PortPresent == 0</TD></TR>
                                                       *     <TR><TD class="indexkey" width=120> Single Port 1x16 </TD><TD class="indexvalue">CoreConfiguration[1] == 0x01010101</TD></TR>
                                                       *  </TABLE>
                                                       *  @li <b>CoreConfiguration[2]</b> - GPP3a Core Configuration
                                                       *  <TABLE border="0">
                                                       *     <TR><TD class="indexkey" width=120> 4:2:0:0:0:0 </TD><TD class="indexvalue">CoreConfiguration[2] == 0x1 </TD></TR>
                                                       *     <TR><TD class="indexkey" width=120> 4:1:1:0:0:0 </TD><TD class="indexvalue">CoreConfiguration[2] == 0x2</TD></TR>
                                                       *     <TR><TD class="indexkey" width=120> 2:2:2:0:0:0 </TD><TD class="indexvalue">CoreConfiguration[2] == 0x3</TD></TR>
                                                       *     <TR><TD class="indexkey" width=120> 2:2:1:1:0:0 </TD><TD class="indexvalue">CoreConfiguration[2] == 0x4</TD></TR>
                                                       *     <TR><TD class="indexkey" width=120> 2:1:1:1:1:0 </TD><TD class="indexvalue">CoreConfiguration[2] == 0x5</TD></TR>
                                                       *     <TR><TD class="indexkey" width=120> 1:1:1:1:1:1 </TD><TD class="indexvalue">CoreConfiguration[2] == 0x6</TD></TR>
                                                       *  </TABLE>
                                                       *  @li <b>CoreConfiguration[3]</b> - Reserved
                                                       *  @li <b>CoreConfiguration[4]</b> - Reserved

                                                       *  @PlatformDependant
                                                       */
  PCIE_CORE_SETTING         CoreSetting[5];            /**< Core setting
                                                       *  see PCIE_CORE_SETTING for details
                                                       *  @li <b>CoreSetting[0]</b> - GPP1 Core Settings
                                                       *  @li <b>CoreSetting[1]</b> - GPP2 Core Settings
                                                       *  @li <b>CoreSetting[2]</b> - GPP3a Core Settings
                                                       *  @li <b>CoreSetting[3]</b> - GPP3b Core Settings
                                                       *  @li <b>CoreSetting[4]</b> - SB Core setting
                                                       */
  PCIE_PORT_CONFIG          PortConfiguration[15];    /**< Port configuration
                                                       *   see PCIE_PORT_CONFIG for details
                                                       *   @li <b>PortConfiguration[0]</b> - Reserved
                                                       *   @li <b>PortConfiguration[1]</b> - Reserved
                                                       *   @li <b>PortConfiguration[2]</b> - PCIE Port Device 2 configuration
                                                       *   @li <b>PortConfiguration[3]</b> - PCIE Port Device 3 configuration
                                                       *   @li ...
                                                       *   @li <b>PortConfiguration[13]</b> - PCIE Port Device 13 configuration
                                                       *
                                                       *  @PlatformDependant
                                                       */
  PCIE_EXT_PORT_CONFIG      ExtPortConfiguration[15];  /**<  Extended Port configuration
                                                       *   see  PCIE_EXT_PORT_CONFIG for details.
                                                       *   @li <b>ExtPortConfiguration[0]</b> - Reserved
                                                       *   @li <b>ExtPortConfiguration[1]</b> - Reserved
                                                       *   @li <b>ExtPortConfiguration[2]</b> - PCIE Port Device 2 configuration
                                                       *   @li <b>ExtPortConfiguration[3]</b> - PCIE Port Device 3 configuration
                                                       *   @li ...
                                                       *   @li <b>ExtPortConfiguration[13]</b> - PCIE Port Device 13 configuration
                                                       *
                                                       *  @PlatformDependant
                                                       */

  UINT16                    PcieMmioBaseAddress;       /**< PCIE Extended MMIO base address in 1MB Unit.
                                                       *  If PcieMmioBaseAddress == 0  assume MMIO setup already done.
                                                       *  PCIE MMIO range should be programmed in CPU F1x[BC:80] Memory Mapped IO Base/Limit Registers as Non Posted if enabled in NB.
                                                       *  <b>IMPORTANT!!! </b>Platform which use Rev10 and later CPU family should use PCIE MMIO functionality provided by CPU See "Configuration Space" in "BIOS and Kernel Developer's Guide For AMD Family 10h"
                                                       *
                                                       *  @PlatformDependant
                                                       */

  UINT16                    PcieMmioSize;              /**< PCIE Extended MMIO size in 1MB Unit.
                                                       *   if PcieMmioBaseAddress != 0 and PcieMmioSize == 0  ignore both parameters and assume MMIO setup already done
                                                       *   if PcieMmioBaseAddress == 0 and PcieMmioSize != 0  ignore both parameters and assume MMIO setup already done
                                                       *   PCIE MMIO range should be programmed in CPU F1x[BC:80] Memory Mapped IO Base/Limit Registers as Non Posted if enabled in NB.
                                                       *   <b>IMPORTANT!!!</b>Platform which use Rev10 and later CPU family should use PCIE MMIO functionality provided by CPU See "Configuration Space" in "BIOS and Kernel Developer's Guide For AMD Family 10h"
                                                       *
                                                       *   @PlatformDependant
                                                       */
  UINT16                    TempMmioBaseAddress;      /**< Temporary MMIO base in 1MB Unit
                                                        *  MMIO base address for temporary 256MB MMIO range.
                                                        *  Range should be programmed in CPU F1x[BC:80] Memory Mapped IO Base/Limit Registers as Posted
                                                        *
                                                        *  @PlatformDependant
                                                        */
  UINT16                    DeviceInitMaskS1;          ///< Bit mask of ports to be initialized at stage 1
  UINT16                    DeviceInitMaskS2;          ///< Bit mask of ports to be initialized at stage 2
  UINT16                    ResetToTrainingDelay;      ///< Delay (in 1ms) after reset deassert before training started
  UINT16                    TrainingToLinkTestDelay;   ///< Delay (in 1ms) after training started but before pooling link state
  UINT16                    ReceiverDetectionPooling;  ///< Total amount time (in 1ms of pooling for passing receiver detection stage
  UINT32                    Reserved12[16];            ///< Reserved for internal use.
} PCIE_CONFIG;



/// The NB/IOMMU  configuration
typedef  struct {
  AMD_COMMON_STRUCT_HEADER  sHeader;                  ///< Standard structure header
  UINT8                     UnitIdClumping;           /**< Unit ID clamping.
                                                        *  UnitId clamping configuration base on PCIE port 3/12 presence.
                                                        *  @li <b>0</b> - Disable
                                                        *  @li <b>1</b> - Dev3
                                                        *  @li <b>2</b> - Dev12
                                                        *  @li <b>3</b> - Dev3 & Dev12

                                                        *  @FilledByInitializer
                                                        */
  UINT8                     P2PMode;                  /**< Peer-To-Peer (p2p) Modes
                                                        * Peep-To-Peer mode selection
                                                        *  @li <b>0</b> - Default
                                                        *  @li <b>1</b> - Mode 1
                                                        *  @li <b>2</b> - Mode 2
                                                        *  @li <b>0x8x</b> - Skip Initialization

                                                        *  @FilledByInitializer
                                                        */
  UINT8                     Reserved2[2];             ///< Reserved for internal use.
  UINT16                    SysMemoryTomBelow4G;      /**< Top of System memory below 4G in 1MB unit
                                                       *  Top of physical memory including all reserved system memory etc.
                                                       *  @PlatformDependant
                                                       */
  UINT32                    SysMemoryTomAbove4G;      /**< Top of System memory above 4G in 1MB unit
                                                       *  Top of physical memory including all reserved system memory etc.
                                                       *  @PlatformDependant
                                                       */
  UINT16                    Reserved;                 ///< Used for internally to save APIC ID of Core 0.
  UINT64                    IommuBaseAddress;         /**< IOMMU base address.
                                                        * 16kb aligned base address for IOMMU control registers
                                                        * @PlatformDependant
                                                        */
  UINT64                    BroadcastBaseAddress;      /**< PCIE Broadcast MMIO base address.
                                                       *  This feature can be utilized broadcast address aware driver (AMD GFX driver)
                                                       *   If BroadcastBaseAddress == 0  MMIO will not be enabled
                                                       *   @PlatformDependant
                                                       */
  UINT16                    BroadcastSize;            /**< PCIE Broadcast MMIO size in MB.
                                                       *   If BroadcastSize == 0  MMIO will not be enabled
                                                       *   @PlatformDependant
                                                       */
  UINT16                    Reserved3;                ///< Reserved for internal use.
  UINT64                    IoApicBaseAddress;        /**< NB IO APIC Base address.
                                                       *   If IoApicBaseAddress == 0  IOAPIC will not be enabled
                                                       *   @PlatformDependant
                                                       */
  UINT32                    SSID;                     ///< NB Subsystem/Subvendor ID
  UINT32                    IommuTpologyInfo;         ///< For intrernal use only
  UINT32                    Reserved4[8];             ///< Reserved for internal use.
} NB_CONFIG;


/// The  NB Buffer Allocation/request parameters
typedef struct {
  IN  UINTN                 BufferLength;               ///< Buffer length
  IN  UINT32                BufferHandle;               ///< Buffer handle
  OUT VOID*                 BufferPtr;                  ///< Pointer to the buffer
} NB_BUFFER_PARAMS;


/// The  IOMMU exclusion range
typedef struct {
  UINT64                    Start;                      ///< Range Start  address
  UINT64                    Length;                     ///< Range length
} IOMMU_EXCLUSIONRANGE;

//#pragma warning (push)
//#pragma warning (disable: 4200)

/// The  IOMMU exclusion table
typedef struct {
  UINTN                     TableLength;                ///< Exclusion table length
  IOMMU_EXCLUSIONRANGE      ExclusionRange[];           ///< Array of exclusion range entries.
} IOMMU_EXCLUSIONTABLE;
//#pragma warning (pop)

/// The  HT configuration structure
typedef  struct {
  AMD_COMMON_STRUCT_HEADER  sHeader;                     ///< Standard structure header
  UINT8                     LSx;                         /**< HT link LS state enable
                                                         *  @li <b>0</b> - LS0
                                                         *  @li <b>1</b> - LS1
                                                         *  @li <b>2</b> - LS2
                                                         *  @li <b>3</b> - LS3
                                                         *  @li <b>4</b> - Same as CPU (use CPU setting setup by AGESA)
                                                         *  @li <b>0x8x</b> - Skip Setting

                                                         *  @FilledByInitializer
                                                         */
  UINT8                     Reserved;                    ///< Reserved for internal use.
  UINT8                     LinkBufferOptimization;      /**< CPU - NB HT link optimization.
                                                          *  @li <b>0</b> - Disabled
                                                          *  @li <b>1</b> - Enable
                                                          *  @li <b>2..7</b> - Reserved
                                                          */
  UINT8                     HtExtendedAddressSupport;    /**<Extended HT Address Support
                                                          *  @li <b>0</b> - Disable
                                                          *  @li <b>1</b> - Enable
                                                          *  @li <b>0x8x</b> - Skip setting

                                                          *  @FilledByInitializer
                                                          */
  UINT8                     HtLinkTriState;              /**< HT Link  tristate control
                                                          *  @li <b>1</b> - Disable
                                                          *  @li <b>2</b> - CAD/CTL
                                                          *  @li <b>3</b> - CAD/CTL/CLK
                                                          *  @li <b>0x8x</b> - Skip setting

                                                          *  @FilledByInitializer
                                                          */
  UINT8                     NbTransmitterDeemphasis;     /**< NB deemphasis level
                                                          * @li <b>0</b> - Disabled
                                                          * @li <b>1</b> - 1.32dB (0 to 4.5" trace length)
                                                          * @li <b>2</b> - 2.08dB (4.5" to 8" trace length)
                                                          * @li <b>3</b> - 3.10dB (8" to 11" trace length)
                                                          * @li <b>4</b> - 4.22dB (11" to 14" trace length)
                                                          * @li <b>5</b> - 5.50dB (14" to 18" trace length)
                                                          * @li <b>6</b> - 7.05dB (18+" trace length)

                                                          * @PlatformDependant
                                                          */
  UINT16                    HtReferenceClock;            /**< HT Reference clock.
                                                          *
                                                          *  @FilledByInitializer
                                                          */
  UINT32                    Reserved1[10];               ///< Reserved
} HT_CONFIG;


/// The  NB configuration structure
struct _AMD_NB_CONFIG_BLOCK;
typedef  struct _AMD_NB_CONFIG_BLOCK AMD_NB_CONFIG_BLOCK;

/// The  NB configuration structure
typedef  struct {
  AMD_COMMON_STRUCT_HEADER  sHeader;                  ///< Standard structure header
  PCI_ADDR                  NbPciAddress;             /**<PCI address of NB
                                                        * PCI Address to Access NB. Depends on HT topology and configuration for multi NB platform.
                                                        * Always 0:0:0 on single NB platform.
                                                        * @PlatformDependant
                                                        */
  HT_PATH                   NbHtPath;                 /**< HT path to NB.
                                                       *  Path to NB CPU->Ht Link Number->NB
                                                       *  @PlatformDependant
                                                       */
  UINT16                    Reserved21;               ///< Reserved for internal use.
  NB_CONFIG                 *pNbConfig;               ///< Pointer to NB configuration structure
  HT_CONFIG                 *pHtConfig;               ///< Pointer to HT configuration structure
  PCIE_CONFIG               *pPcieConfig;             ///< Pointer to PCIE configuration structure
  AMD_NB_CONFIG_BLOCK       **ConfigPtr;              ///< Pointer to main config block structure
  VOID                      *ReservedPtr;             ///< Reserved for internal use.
} AMD_NB_CONFIG;


/// The  configuration block for all NB in system.
struct _AMD_NB_CONFIG_BLOCK {
  AMD_CONFIG_PARAMS         StandardHeader;             ///< Standard structure header
  UINT8                     NumberOfNorthbridges;       /**< Number of AMD_NB_CONFIG configuration structures
                                                         *  @li <b>0</b> - One NB.
                                                         *  @li <b>1</b> - Two NB.
                                                         *  @li <b>2</b> - Three NB.
                                                         *  @li <b>3</b> - Four NB.

                                                         *  @PlatformDependant
                                                         */
  UINT8                     Scratch;                    /**< Variable for internal use
                                                        *  @li <b>Bit[0]</b> = 0 Enable DebugOut, = 1 Disable DebugOut(only applicable for DebugOut enabled binaries).
                                                        */
  UINT8                     PlatformType;               /**< Platform Type (Server/Desktop).
                                                         *  @li<b>0</b> - Unknown
                                                         *  @li<b>1</b> - Desktop
                                                         *  @li<b>2</b> - Server

                                                         *  @PlatformDependant
                                                         */
  UINT8                     CurrentNorthbridge;         /**< Northbridge ID which currently being initialized.
                                                         *  Variable filled by CIMx and can be used during callback
                                                         *  to identify to which NB callback belongs.
                                                         */
  AMD_NB_CONFIG             Northbridges[MAX_NB_COUNT]; ///< Array of configuration structures for one or more NB in system
};

/// The IO APIC Interrupt Mapping Info
typedef struct {
  UINT8                     Group;                      /**< Group mapping for slot or endpoint device (connected to PCIE port) interrupts .
                                                             @li <b>0</b> - mapped to Grp 0 (pin 0..3   of IO APIC)
                                                             @li <b>1</b> - mapped to Grp 1 (pin 4..7   of IO APIC)
                                                             @li ...
                                                             @li <b>7</b> - mapped to Grp 7 (pin 28..31 of IO APIC)
                                                        */
  UINT8                     Swizzle;                    /**< Swizzle interrupt in the Group.
                                                             @li <b>0</b> - ABCD
                                                             @li <b>1</b> - BCDA
                                                             @li <b>2</b> - CDAB
                                                             @li <b>3</b> - DABC
                                                        */
  UINT8                     Pin;                        /**<Interrupt pin for PCIE bridge
                                                             @li <b>0</b>  - Pin 0  of IO APIC
                                                             @li <b>1</b>  - Pin 1  of IO APIC
                                                             @li ...
                                                             @li <b>31</b> - Pin 31 of IO APIC
                                                        */
} APIC_DEVICE_INFO;

typedef AGESA_STATUS (*SYSTEM_API) (AMD_NB_CONFIG_BLOCK  *ConfigPtr);
typedef AGESA_STATUS (*NB_API) (AMD_NB_CONFIG  *NbConfigPtr);

/// Northbridge info
typedef  struct {
  UINT8                     Type;                         ///< NB Model (RS780/RD790/RD890...)
  UINT8                     Revision;                     ///< NB Revision ID
} NB_INFO;

/// API workspace
typedef struct {
  AMD_NB_CONFIG_BLOCK       *ConfigPtr;                   ///< NB congiguration
  AGESA_STATUS              Status;                       ///< return status
} API_WORKSPACE;

/// Indirect register entry
typedef struct {
  UINT32  Register;                                       ///< register
  UINT32  Mask;                                           ///< AND mask
  UINT32  Data;                                           ///< data
} INDIRECT_REG_ENTRY;

/// Register entry
typedef struct {
  UINT8  Register;                                        ///< register
  UINT8  Mask;                                            ///< AND mask
  UINT8  Data;                                            ///< data
} REGISTER_ENTRY;

/// Scratchpad
typedef  struct {
  UINT32                    ResetCount          :4;       ///< PCIe reset count
  UINT32                    PortGen2Disable     :12;      ///< PCIe Gen 2 disable
  UINT32                    MaskMemoryInit      :1;       ///< Mask memory init complete
} SCRATCH_1;

/// Scratchpad
typedef  struct {
  UINT32                    GlobalInterruptBase :8;       ///< TBD
} SCRATCH_4;


/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page LegacyInterfaceCalls  Legacy Interface Calls
 *  <TD>@subpage PH_Initializer_Page "PH_Initializer"</TD><TD></TD>
 *  <TD>@subpage PH_AmdPowerOnResetInit_Page "PH_AmdPowerOnResetInit"</TD><TD></TD>
 *  <TD>@subpage PH_AmdNbHtInit_Page "PH_AmdNbHtInit"</TD><TD></TD>
 *  <TD>@subpage PH_AmdPcieEarlyInit_Page "PH_AmdPcieEarlyInit"</TD><TD></TD>
 *  <TD>@subpage PH_AmdEarlyPostInit_Page "PH_AmdEarlyPostInit"</TD><TD></TD>
 *  <TD>@subpage PH_AmdMidPostInit_Page "PH_AmdMidPostInit"</TD><TD></TD>
 *  <TD>@subpage PH_AmdLatePostInit_Page "PH_AmdLatePostInit"</TD><TD></TD>
 *  <TD>@subpage PH_AmdS3Init_Page "PH_AmdS3Init"</TD><TD></TD>
 *
*/

/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page PH_Initializer_Page PH_AmdNbInitializer
 *  @section PH_Initializer PH_AmdNbInitializer Interface Call
 *  Initialize structure referenced by AMD_NB_CONFIG::pHtConfig, AMD_NB_CONFIG::pPcieConfig and AMD_NB_CONFIG::pNbConfig to default recommended value.(Except platform dependant parameters See @ref PlatformDependParam "Platform Dependant Parameters" )
 *  @subsection PH_Initializer_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(ImageEntryPtr)(AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PH_Initializer_Callback Callback`s
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdReportEvent_Page "PHCB_AmdReportEvent"</TD></TR>
 *  </TABLE>
 *  @subsection PH_Initializer_Page_Initializer Initializer
 *  @par
 *     Not Applicable
 *  @subsection PH_Initializer_Data Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG_BLOCK::StandardHeader </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG_BLOCK::NumberOfNorthbridges </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG::NbPciAddress </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *  </TABLE>
 *
 */
#define  PH_AmdInitializer                 0x100

/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page PH_AmdPowerOnResetInit_Page PH_AmdPowerOnResetInit
 *  @section PH_AmdPowerOnResetInit PH_AmdPowerOnResetInit Interface Call
 *  Initialize Northbridge registers on power-on reset.
 *  @subsection PH_AmdPowerOnResetInit_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(ImageEntryPtr)(AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PH_AmdPowerOnResetInit_Callback Callback`s
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdReportEvent_Page "PHCB_AmdReportEvent"</TD></TR>
 *  </TABLE>
 *  @subsection PH_AmdPowerOnResetInit_Initializer Initializer
 *  @par
 *     Not Required
 *  @subsection PH_AmdPowerOnResetInit_Data Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG_BLOCK::StandardHeader </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG_BLOCK::NumberOfNorthbridges </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG::NbPciAddress </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *  </TABLE>
 *
 */
#define  PH_AmdPowerOnResetInit            0x00
/**
 *  @page PH_AmdNbHtInit_Page PH_AmdNbHtInit
 *  @section PH_AmdNbHtInit PH_AmdNbHtInit Interface Call
 *  Initialize NB HT subsystem.
 *  @subsection PH_AmdNbHtInit_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(ImageEntryPtr)(AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PH_AmdNbHtInit_Callback Callback`s
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdReportEvent_Page "PHCB_AmdReportEvent"</TD></TR>
 *  </TABLE>
 *  @subsection PH_AmdNbHtInit_Initializer Initializer
 *  @par
 *     Required (see @ref PH_Initializer_Page "PH_Initializer")
 *  @subsection PH_AmdNbHtInit_Data Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG_BLOCK::StandardHeader </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG_BLOCK::NumberOfNorthbridges </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG::NbPciAddress </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG::NbHtPath  </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG::pHtConfig </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *  </TABLE>
 *
 */
#define  PH_AmdNbHtInit                    0x10
/**
 *  @page PH_AmdEarlyPostInit_Page PH_AmdEarlyPostInit
 *  @section PH_AmdEarlyPostInit PH_AmdEarlyPostInit Interface Call
 *   Initialize misc Northbridge feature at Early Post.
 *  @subsection PH_AmdEarlyPostInit_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(ImageEntryPtr)(AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PH_AmdEarlyPostInit_Callback Callback`s
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey"  width=380> @ref PHCB_AmdReportEvent_Page "PHCB_AmdReportEvent"</TD></TR>
 *  </TABLE>
  *  @subsection PH_AmdEarlyPostInit_Initializer Initializer
 *  @par
 *     Required. (see @ref PH_Initializer_Page)
 *     PH_AmdEarlyPostInit/PH_AmdMidPostInit/PH_AmdLatePostInit/PH_AmdS3Init
 *     must use same copy of AMD_NB_CONFIG_BLOCK
 *  @subsection PH_AmdEarlyPostInit_Data Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG_BLOCK::StandardHeader </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG_BLOCK::NumberOfNorthbridges </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG::NbPciAddress </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG::NbHtPath  </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG::pNbConfig </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG::pPcieConfig </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *     <TR><TD class="indexkey" width=380> NB_CONFIG::SysMemoryTomBelow4G </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> NB_CONFIG::SysMemoryTomAbove4G </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *  </TABLE>
 *
 */
#define  PH_AmdEarlyPostInit               0x30
/**
 *  @page PH_AmdMidPostInit_Page PH_AmdMidPostInit
 *  @section PH_AmdMidPostInit PH_AmdMidPostInit Interface Call
 *   Initialize misc Northbridge feature at Mid Post.
 *  @subsection PH_AmdMidPostInit_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(ImageEntryPtr)(AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PH_AmdMidPostInit_Callback Callback`s
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdUpdateApicInterruptMapping_Page "PHCB_AmdUpdateApicInterruptMapping"</TD></TR>
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdAllocateBuffer_Page "PHCB_AmdAllocateBuffer "</TD></TR>
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdGetExclusionTable_Page "PHCB_AmdGetExclusionTable "</TD></TR>
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdReportEvent_Page "PHCB_AmdReportEvent"</TD></TR>
 *  </TABLE>
 *  @subsection PH_AmdMidPostInit_Initializer Initializer
 *  @par
 *     PH_AmdEarlyPostInit/PH_AmdMidPostInit/PH_AmdLatePostInit/PH_AmdS3Init must use same copy of AMD_NB_CONFIG_BLOCK structure.
 *  @subsection PH_AmdMidPostInit_Data Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG_BLOCK::StandardHeader </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> NB_CONFIG::IommuBaseAddress </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> NB_CONFIG::IoApicBaseAddress </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *  </TABLE>
 *
 */
#define  PH_AmdMidPostInit                  0x40
/**
 *  @page PH_AmdLatePostInit_Page PH_AmdLatePostInit
 *  @section PH_AmdLatePostInit PH_AmdLatePostInit Interface Call
 *  Initialize misc Northbridge feature at Late Post.
 *  @subsection PH_AmdLatePostInit_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(ImageEntryPtr)(AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PH_AmdLatePostInit_Callback Callback`s
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdReportEvent_Page "PHCB_AmdReportEvent"</TD></TR>
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdPcieAsmpInfo "PHCB_AmdPcieAsmpInfo"</TD></TR>
 *  </TABLE>
 *  @subsection PH_AmdLatePostInit_Initializer Initializer
 *  @par
 *     PH_AmdEarlyPostInit/PH_AmdMidPostInit/PH_AmdLatePostInit/PH_AmdS3Init must use same copy of AMD_NB_CONFIG_BLOCK structure.
 *  @subsection PH_AmdLatePostInit_Data Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG_BLOCK::StandardHeader</TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *  </TABLE>
 *
 */
#define  PH_AmdLatePostInit                0x50

 /**
 *  @page PH_AmdPcieEarlyInit_Page PH_AmdPcieEarlyInit
 *  @section PH_AmdPcieEarlyInit PH_AmdPcieEarlyInit Interface Call
 *  Init PCI Express Subsystem. Train link on all enabled Ports. Initialize hotplug.
 *  @subsection PH_AmdPcieEarlyInit_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(ImageEntryPtr)(AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PH_AmdPcieEarlyInit_Callback Callback`s
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdPortTrainingCompleted_Page "PHCB_AmdPortTrainingCompleted"</TD></TR>
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdPortResetSupported_Page "PHCB_AmdPortResetSupported"</TD></TR>
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdPortResetAssert_Page "PHCB_AmdPortResetAssert"</TD></TR>
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdPortResetDeassert_Page "PHCB_AmdPortResetDeassert"</TD></TR>
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdReportEvent_Page "PHCB_AmdReportEvent"</TD></TR>
 *  </TABLE>
 *  @subsection PH_AmdPcieEarlyInit_Initializer Initializer
 *  @par
 *     Required.
 *  @subsection PH_AmdPcieEarlyInit_Data Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG_BLOCK::StandardHeader </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG_BLOCK::NumberOfNorthbridges </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG::NbPciAddress </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG::pPcieConfig </TD><TD class="indexvalue"><B>Required</B></TD></TR>
 *  </TABLE>
 *
 */
#define  PH_AmdPcieEarlyInit               0x20
/**
 *  @page PH_AmdS3Init_Page  PH_AmdS3Init
 *  @section PH_AmdS3Init PH_AmdS3Init Interface Call
 *  Init misc. feature PCI Express Subsystem. Enable power management  feature. Power off unused lanes/PLL etc.
 *  @subsection PH_AmdPcieLateInit_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(ImageEntryPtr)(AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PH_AmdS3Init_Callback Callback`s
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdUpdateApicInterruptMapping_Page "PHCB_AmdUpdateApicInterruptMapping"</TD></TR>
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdReportEvent_Page "PHCB_AmdReportEvent"</TD></TR>
 *     <TR><TD class="indexkey" width=380> @ref PHCB_AmdPcieAsmpInfo "PHCB_AmdPcieAsmpInfo"</TD></TR>
 *  </TABLE>
 *  @subsection PH_AmdS3Init_Initializer Initializer
 *  @par
 *     PH_AmdEarlyPostInit/PH_AmdMidPostInit/PH_AmdLatePostInit/PH_AmdS3Init must use same copy of AMD_NB_CONFIG_BLOCK structure.
 *  @subsection PH_AmdS3Init_Data Configuration Data.
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=380> AMD_NB_CONFIG_BLOCK::StandardHeader </TD><TD class="indexvalue"><B>Required </B></TD></TR>
 *  </TABLE>
 *
 */
#define  PH_AmdS3Init                      0x60

/*
  Secondary level interface function
*/
#define  PH_AmdPcieS3Init                  0x61
#define  PH_AmdNbS3Init                    0x62
#define  PH_AmdPcieLateInit                0x51
#define  PH_AmdNbLateInit                  0x52

/**
 *This function PH_AmdPcieValidatePortState must be called
 *  after the PH_AmdPcieEarlyInit and before PH_AmdLatePostInit
 */
#define  PH_AmdPcieValidatePortState       0x70


/**
 *  @page PHCB_AmdPortTrainingCompleted_Page PHCB_AmdPortTrainingCompleted
 *  @section PHCB_AmdPortTrainingCompleted PHCB_AmdPortTrainingCompleted Callback
 *  PCIE Port Initialization Completed endpoint detected.
 *  @subsection PHCB_AmdPortTrainingCompleted_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(CallBackPtr)(UINT32 CallBackId, UINTN PortId, AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PH_AmdS3Init_Parameters Parameters
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>CallBackId</TD><TD class="indexvalue" width=500>0x8000</TD></TR>
 *     <TR><TD class="indexkey" width=160>PortId</TD><TD class="indexvalue" width=500>PCI Express Port Id</TD></TR>
 *  </TABLE>
 *  @subsection PHCB_AmdPortTrainingCompleted_Retrun Return Value
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>AGESA_UNSUPPORTED</TD><TD class="indexvalue" width=500>Callback  not supported</TD></TR>
 *     <TR><TD class="indexkey" width=160>AGESA_ERROR</TD><TD class="indexvalue" width=500>Disable Port</TD></TR>
 *  </TABLE>
 *
 */
#define  PHCB_AmdPortTrainingCompleted     0x8000
/**
 *  @page PHCB_AmdPortResetDeassert_Page PHCB_AmdPortResetDeassert
 *  @section PHCB_AmdPortResetDeassert PHCB_AmdPortResetDeassert Callback
 *  Deassert reset for device or slot connected to PCIE Port.
 *  @subsection PHCB_AmdPortResetDeassert_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(CallBackPtr)(UINT32 CallBackId, UINTN PortIdBitMap, AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PHCB_AmdPortTrainingCompleted_Parameters Parameters
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>CallBackId</TD><TD class="indexvalue" width=500> 0x8001</TD></TR>
 *     <TR><TD class="indexkey" width=160>PortId</TD><TD class="indexvalue" width=500>bitmap of port id to deassert reset (0x4 - PortId 2, 0x8 PortId 3, ...)</TD></TR>
 *  </TABLE>
 *  @subsection PHCB_AmdPortTrainingCompleted_Retrun Return Value
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>AGESA_UNSUPPORTED</TD><TD class="indexvalue" width=500>Callback  not supported</TD></TR>
 *     <TR><TD class="indexkey" width=160>AGESA_SUCCESS</TD><TD class="indexvalue" width=500>Reset successfully deasserted</TD></TR>
 *  </TABLE>
 *
 */
#define  PHCB_AmdPortResetDeassert         0x8001
/**
 *  @page PHCB_AmdPortResetAssert_Page PHCB_AmdPortResetAssert
 *  @section PHCB_AmdPortResetAssert PHCB_AmdPortResetAssert Callback
 *  Assert reset for device connected to PCIE Port.
 *  @subsection PHCB_AmdPortResetDeassert_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(CallBackPtr)(UINT32 CallBackId, UINTN PortIdBitMap, AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PHCB_AmdPortResetAssert_Parameters Parameters
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>CallBackId</TD><TD class="indexvalue" width=500>0x8002</TD></TR>
 *     <TR><TD class="indexkey" width=160>PortIdBitMap</TD><TD class="indexvalue" width=500>Bitmap of port id to assert reset (0x4 - PortId 2, 0x8 PortId 3, ...)</TD></TR>
 *  </TABLE>
 *  @subsection PHCB_AmdPortResetAssert_Retrun Return Value
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>AGESA_UNSUPPORTED</TD><TD class="indexvalue" width=500>Callback  not supported</TD></TR>
 *     <TR><TD class="indexkey" width=160>AGESA_SUCCESS</TD><TD class="indexvalue" width=500>Reset successfully asserted</TD></TR>
 *  </TABLE>
 *
 */
#define  PHCB_AmdPortResetAssert           0x8002
/**
 *  @page PHCB_AmdPortResetSupported_Page PHCB_AmdPortResetSupported
 *  @section PHCB_AmdPortResetSupported PHCB_AmdPortResetSupported Callback
 *  Test if controllable reset logic present for PCIE Port.
 *  @subsection PHCB_AmdPortResetSupported_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(CallBackPtr)(UINT32 CallBackId, UINTN PortId, AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PHCB_AmdPortResetSupported_Parameters Parameters
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>CallBackId</TD><TD class="indexvalue" width=500>0x8003</TD></TR>
 *     <TR><TD class="indexkey" width=160>PortId</TD><TD class="indexvalue" width=500> Port ID to check GPIO controlled reset logic present for slot or endpoint connected to this port</TD></TR>
 *  </TABLE>
 *  @subsection PHCB_AmdPortResetSupported_Retrun Return Value
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>AGESA_UNSUPPORTED</TD><TD class="indexvalue" width=500>Callback  not supported</TD></TR>
 *     <TR><TD class="indexkey" width=160>AGESA_SUCCESS</TD><TD class="indexvalue" width=500>Slot/Device connected to port has GPIO controlled reset logic</TD></TR>
 *  </TABLE>
 *
 */
#define  PHCB_AmdPortResetSupported        0x8003


/**
 *  @page PHCB_AmdGeneratePciReset_Page PHCB_AmdGeneratePciReset
 *  @section PHCB_AmdPortResetSupported PHCB_AmdGeneratePciReset Callback
 *  Request PCI reset generation.
 *  @subsection PHCB_AmdGeneratePciReset_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(CallBackPtr)(UINT32 CallBackId, UINTN ResetType, AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PHCB_AmdGeneratePciReset_Parameters Parameters
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>CallBackId</TD><TD class="indexvalue" width=500>0x8004</TD></TR>
 *     <TR><TD class="indexkey" width=160>PortId</TD><TD class="indexvalue" width=500> Reset type. 0x01 - Warm Reset,  0x02 - Cold Reset</TD></TR>
 *  </TABLE>
 *  @subsection PHCB_AmdGeneratePciReset_Retrun Return Value
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>"Any"</TD><TD class="indexvalue" width=500>CIMx will generate reset by writing port 0xCF9</TD></TR>
 *     <TR><TD class="indexkey" width=160>AGESA_SUCCESS</TD><TD class="indexvalue" width=500>Debug feature to completely avoid reset generation</TD></TR>
 *  </TABLE>
 *
 */
#define  PHCB_AmdGeneratePciReset          0x8004

/**
 *  @page PHCB_AmdGetExclusionTable_Page PHCB_AmdGetExclusionTable
 *  @section PHCB_AmdGetExclusionTable PHCB_AmdGetExclusionTable Callback
 *  Return the IOMMU exclusion table related to the current configuration block
 *  @subsection PHCB_AmdGetExclusionTable_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(CallBackPtr)(UINT32 CallBackId, IOMMU_EXCLUSIONTABLE *ExclusionTable, AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PHCB_AmdGetExclusionTable_Parameters Parameters
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>CallBackId</TD><TD class="indexvalue" width=500>0x8005</TD></TR>
 *     <TR><TD class="indexkey" width=160>ExclusionTable</TD><TD class="indexvalue" width=500>Pointer to IOMMU exclusion table</TD></TR>
 *  </TABLE>
 *  @subsection PHCB_AmdGetExclusionTable_Return Return Value
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>AGESA_UNSUPPORTED</TD><TD class="indexvalue" width=500>Callback not supported</TD></TR>
 *     <TR><TD class="indexkey" width=160>AGESA_SUCCESS</TD><TD class="indexvalue" width=500>Valid table returned</TD></TR>
 *  </TABLE>
 *
 */
#define  PHCB_AmdGetExclusionTable           0x8005

/**
 *  @page PHCB_AmdAllocateBuffer_Page PHCB_AmdAllocateBuffer
 *  @section PHCB_AmdAllocateBuffer PHCB_AmdAllocateBuffer Callback
 *  Return the address of a memory buffer (size in bytes).
 *
 *  ACPI handles used:
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>'SRVI'</TD><TD class="indexvalue" width=500>IO virtualization table</TD></TR>
 *  </TABLE>
 *  @par
 *  Usage: Buffer can be be allocated via PMM function 0 with handle and size specified in BufferParamsPtr, and
 *         the address must be returned in BufferParamsPtr.
 *  @par
 *  <b>Important Note:</b>  Allocation for ACPI table will be requested during AmdEarlyPostInit or AmdMidPostInit and must be linked to
 *          ACPI table structure prior AmdLatePostInit interface call.
 *  @subsection PHCB_AmdAllocateBuffer_Callin Call Prototype
 *  @par
 *     AGESA_STATUS *(CallBackPtr)(UINT32 CallBackId, NB_BUFFER_PARAMS *BufferParamPtr, AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PHCB_AmdAllocateBuffer_Parameters Parameters
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>CallBackId</TD><TD class="indexvalue" width=500>0x8006</TD></TR>
 *     <TR><TD class="indexkey" width=160>BufferParamPtr</TD><TD class="indexvalue" width=500>Pointer to buffer parameters</TD></TR>
 *  </TABLE>
 *  @subsection PHCB_AmdAllocateBuffer_Return Return Value
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>AGESA_UNSUPPORTED</TD><TD class="indexvalue" width=500>Callback not supported</TD></TR>
 *     <TR><TD class="indexkey" width=160>AGESA_SUCCESS</TD><TD class="indexvalue" width=500>Buffer returned</TD></TR>
 *  </TABLE>
 *
 */
#define  PHCB_AmdAllocateBuffer               0x8006

/**
 *  @page PHCB_AmdUpdateApicInterruptMapping_Page PHCB_AmdUpdateApicInterruptMapping
 *  @section PHCB_AmdUpdateApicInterruptMapping PHCB_AmdUpdateApicInterruptMapping Callback
 *  Provide pointer to default IOAPIC interrupt mapping table
 *  @subsection PHCB_AmdUpdateApicInterruptMapping_Callin Call Prototype
 *  @par
 *     AGESA_STATUS *(CallBackPtr)(UINT32 CallBackId, APIC_DEVICE_INFO *pApicPortInfo, AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PHCB_AmdUpdateApicInterruptMapping_Parameters Parameters
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>CallBackId</TD><TD class="indexvalue" width=500>0x8007</TD></TR>
 *     <TR><TD class="indexkey" width=160>pApicPortInfo</TD><TD class="indexvalue" width=500>Pointer to array of structures containing default IO APIC interrupt mapping info. For default interrupt mapping info see \ref gDefaultApicDeviceInfoTable</TD></TR>
 *  </TABLE>
 *  @subsection PHCB_AmdUpdateApicInterruptMapping_Return Return Value
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>AGESA_UNSUPPORTED</TD><TD class="indexvalue" width=500>Callback not supported</TD></TR>
 *     <TR><TD class="indexkey" width=160>AGESA_SUCCESS</TD><TD class="indexvalue" width=500>Interrupt mapping configuration completed</TD></TR>
 *  </TABLE>
 *
 */

#define  PHCB_AmdUpdateApicInterruptMapping     0x8007

/**
 *  @page PHCB_AmdFreeBuffer_Page PHCB_AmdFreeBuffer
 *  @section PHCB_AmdFreeBuffer PHCB_AmdFreeBuffer Callback
 *  Free a specific memory buffer by handle.
 *
 *  ACPI handles used:
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>'SRVI'</TD><TD class="indexvalue" width=500>IO virtualization table</TD></TR>
 *  </TABLE>
 *  @par
 *  <b>Important Note:</b> ACPI tables  as listed above will not be explicitly freed and must be linked into the system ACPI table structure.
 *
 *  @subsection PHCB_AmdFreeBuffer_Callin Call Prototype
 *  @par
 *     AGESA_STATUS *(CallBackPtr)(UINT32 CallBackId, NB_BUFFER_PARAMS *BufferParamPtr, AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PHCB_AmdFreeBuffer_Parameters Parameters
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>CallBackId</TD><TD class="indexvalue" width=500>0x8008</TD></TR>
 *     <TR><TD class="indexkey" width=160>BufferParamPtr</TD><TD class="indexvalue" width=500>Pointer to buffer parameters</TD></TR>
 *  </TABLE>
 *  @subsection PHCB_AmdFreeBuffer_Return Return Value
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>AGESA_UNSUPPORTED</TD><TD class="indexvalue" width=500>Callback not supported</TD></TR>
 *     <TR><TD class="indexkey" width=160>AGESA_SUCCESS</TD><TD class="indexvalue" width=500>Buffer freed</TD></TR>
 *  </TABLE>
 *
 */
#define  PHCB_AmdFreeBuffer                       0x8008

/**
 *  @page PHCB_AmdLocateBuffer_Page PHCB_AmdLocateBuffer
 *  @section PHCB_AmdLocateBuffer PHCB_AmdLocateBuffer Callback
 *  Locate a specific memory buffer by handle (See also @ref PHCB_AmdAllocateBuffer_Page "PHCB_AmdAllocateBuffer").
 *
 *  @subsection PHCB_AmdLocateBuffer_Callin Call Prototype
 *  @par
 *     AGESA_STATUS *(CallBackPtr)(UINT32 CallBackId, NB_BUFFER_PARAMS *BufferParamPtr, AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PHCB_AmdLocateBuffer_Parameters Parameters
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>CallBackId</TD><TD class="indexvalue" width=500>0x8009</TD></TR>
 *     <TR><TD class="indexkey" width=160>BufferParamPtr</TD><TD class="indexvalue" width=500>Pointer to buffer parameters</TD></TR>
 *  </TABLE>
 *  @subsection PHCB_AmdLocateBuffer_Return Return Value
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>AGESA_UNSUPPORTED</TD><TD class="indexvalue" width=500>Callback not supported</TD></TR>
 *     <TR><TD class="indexkey" width=160>AGESA_SUCCESS</TD><TD class="indexvalue" width=500>Buffer Located</TD></TR>
 *  </TABLE>
 *
 */
#define  PHCB_AmdLocateBuffer                     0x8009

/**
 *  @page PHCB_AmdReportEvent_Page PHCB_AmdReportEvent
 *  @section PHCB_AmdReportEvent PHCB_AmdReportEvent Callback
 *  Report event to platform firmware
 *  To exclude an entry, set the value of DeviceId to 0xFFFF
 *
 *  @subsection PHCB_AmdReportEvent_Callin Call Prototype
 *  @par
 *     AGESA_STATUS *(CallBackPtr)(UINT32 CallBackId, AGESA_EVENT *Event, AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PHCB_AmdReportEvent_Parameters Parameters
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160> CallBackId</TD><TD class="indexvalue" width=500>0x8010</TD></TR>
 *     <TR><TD class="indexkey" width=160> Event</TD><TD class="indexvalue" width=500>pointer to event structure</TD></TR>
 *  </TABLE>
 *  @subsection PHCB_AmdReportEvent_Events Events
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160> Event Class</TD><TD class="indexkey">Event Info </TD><TD class="indexkey">Description</TD></TR>
 *     <TR><TD class="indexvalue" width=160> AGESA_ERROR</TD><TD class="indexvalue">0x20010100</TD><TD class="indexvalue" >Hotplug controller firmware initialization fail</TD></TR>
 *     <TR><TD class="indexvalue" width=160> AGESA_ERROR</TD><TD class="indexvalue">0x20010200</TD><TD class="indexvalue">Link training fail</TD></TR>
 *     <TR><TD class="indexvalue" width=160> AGESA_ERROR</TD><TD class="indexvalue">0x20010300</TD><TD class="indexvalue">Incorrect PCIE Core COnfiguration requested</TD></TR>
 *     <TR><TD class="indexvalue" width=160> AGESA_WARNING</TD><TD class="indexvalue">0x20010400</TD><TD class="indexvalue">Link width downgraded </TD></TR>
 *     <TR><TD class="indexvalue" width=160> AGESA_WARNING</TD><TD class="indexvalue">0x20010500</TD><TD class="indexvalue">Link speed forced to Gen1.</TD></TR>
 *     <TR><TD class="indexvalue" width=160> AGESA_WARNING</TD><TD class="indexvalue">0x20010600</TD><TD class="indexvalue">VCO negotiation fail.</TD></TR>
 *     <TR><TD class="indexvalue" width=160> AGESA_WARNING</TD><TD class="indexvalue">0x20010700</TD><TD class="indexvalue">Incorrect port device number remapping configuration</TD></TR>
 *     <TR><TD class="indexvalue" width=160> AGESA_FATAL</TD><TD class="indexvalue">0x20000100</TD><TD class="indexvalue" >Invalid configuration structure</TD></TR>
 *     <TR><TD class="indexvalue" width=160> AGESA_WARRNING</TD><TD class="indexvalue">0x20000200</TD><TD class="indexvalue">NB not present</TD></TR>
 *     <TR><TD class="indexvalue" width=160> AGESA_ERROR</TD><TD class="indexvalue">0x20000300</TD><TD class="indexvalue">Can not locate ACPI table</TD></TR>
 *  </TABLE>
 *  @subsection PHCB_AmdReportEvent_Return Return Value
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>AGESA_UNSUPPORTED</TD><TD class="indexvalue" width=500>Callback not supported</TD></TR>
 *     <TR><TD class="indexkey" width=160>AGESA_SUCCESS</TD><TD class="indexvalue" width=500>Event successfully logged</TD></TR>
 *  </TABLE>
 *
 */
#define PHCB_AmdReportEvent                    0x8010
/**
 *  @page PHCB_AmdPcieAsmpInfo_Page PHCB_AmdPcieAsmpInfo
 *  @section PHCB_AmdPcieAsmpInfo PHCB_AmdPcieAsmpInfo Callback
 *  Give platform chance to update PCIe link ASPM setting.
 *  @subsection PHCB_AmdPcieAsmpInfo_CallIn Call Prototype
 *  @par
 *     AGESA_STATUS *(CallBackPtr)(UINT32 CallBackId, ASPM_LINK_INFO *AspmLinkInfoPtr, AMD_NB_CONFIG_BLOCK *ConfigPtr)
 *  @subsection PHCB_AmdPcieAsmpInfo_Parameters Parameters
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>CallBackId</TD><TD class="indexvalue" width=500>0x8011</TD></TR>
 *     <TR><TD class="indexkey" width=160>AspmLinkInfoPtr</TD><TD class="indexvalue" width=500> ASPM link info see ASPM_LINK_INFO for details.</TD></TR>
 *  </TABLE>
 *  @subsection PHCB_AmdPcieAsmpInfo_Retrun Return Value
 *  @par
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160>"Any"</TD><TD class="indexvalue" width=500>CIMx will use content on ASPM_LINK_INFO to enable ASPM</TD></TR>
 *  </TABLE>
 *
 */
#define  PHCB_AmdPcieAsmpInfo                   0x8011


#define CB_AmdSetNbPorConfig                    0x9000
#define CB_AmdSetHtConfig                       0x9001
#define CB_AmdSetPcieEarlyConfig                0x9002
#define CB_AmdSetEarlyPostConfig                0x9003
#define CB_AmdSetMidPostConfig                  0x9004
#define CB_AmdSetLatePostConfig                 0x9005
#define CB_AmdSetRecoveryConfig                 0x9006

#define ON                                      0x1
#define OFF                                     0x0

#define NB_RD890TV                              0x00
#define NB_RD780                                0x01
#define NB_RX780                                0x02
#define NB_SR5690                               0x10
#define NB_SR5670                               0x15
#define NB_SR5650                               0x20
#define NB_RD890                                0x25
#define NB_990FX                                0x25        //990FX= RD890
#define NB_990X                                 0x30        //990X= SR5650
#define NB_970                                  0x35

#define NB_UNKNOWN                              0xff

#define NB_REV_A11                              0x00
#define NB_REV_A12                              0x02

#define DEV3_CLUMPING                           1
#define DEV12_CLUMPING                          2

#define INITIALIZED_BY_INITIALIZER              0xAA

#define CPU_FAMILY_NPT                          0x00000000
#define CPU_FAMILY_GH                           0x00100000

#define GET_NB_CONFIG_PTR(x)                    x->pNbConfig
#define GET_HT_CONFIG_PTR(x)                    x->pHtConfig
#define GET_PCIE_CONFIG_PTR(x)                  x->pPcieConfig
#define GET_BLOCK_CONFIG_PTR(x)                 (*(x->ConfigPtr))
#define NB_SBDFO                                pConfig->NbPciAddress.AddressValue

#define ABCD                                    0
#define BCDA                                    1
#define CDAB                                    2
#define DABC                                    3

/// Platform Type
typedef enum {
  DetectPlatform,                           ///< Autodetect platform type based on NB skew
  DesktopPlatform,                          ///< Desktop platform
  ServerPlatform                            ///< Server platform
} PLATFORM_TYPE;

/// APIC register info
typedef struct {
  UINT8 EpRoutingOffset;                    ///< Ep routing offset
  UINT8 EpRoutingRegister;                  ///< Ep routing reg
  UINT8 RcRoutingOffset;                    ///< Rc routing offset
  UINT8 RcRoutingRegister;                  ///< Rc routing reg
} APIC_REGISTER_INFO;


#define PortInterruptPinMap(Pin, Port) (Pin << Port)

#define HtPinMapOffset                        0
#define IommuPinMapOffset                     8


#define CIMX_MIN(x, y) (((x) > (y)) ? (y) : (x))
#define CIMX_MAX(x, y) (((x) > (y)) ? (x) : (y))
//#pragma pack(pop)

#endif
