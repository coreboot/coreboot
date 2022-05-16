/* $NoKeywords:$ */
/**
 * @file
 *
 * Agesa structures and definitions
 *
 * Contains AMD AGESA core interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Include
 * @e \$Revision: 64574 $   @e \$Date: 2012-01-25 01:01:51 -0600 (Wed, 25 Jan 2012) $
 */
/*****************************************************************************
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
 *
 ***************************************************************************/


#ifndef _AGESA_H_
#define _AGESA_H_

/*
 * This is the delivery package title.
 * This string MUST be exactly 8 characters long.
 */
#define AGESA_PACKAGE_STRING {'c', 'b', '_', 'A', 'g', 'e', 's', 'a'}

/*
 * This is the release version number of the AGESA component.
 * This string MUST be exactly 12 characters long.
 */
#define AGESA_VERSION_STRING {'V', '0', '.', '0', '.', '0', '.', '1', ' ', ' ', ' ', ' '}

#include  "Porting.h"
#include  "AMD.h"

//
//
// AGESA Types and Definitions
//
//

// AGESA BASIC CALLOUTS
#define AGESA_MEM_RELEASE              0x00028000ul

// AGESA ADVANCED CALLOUTS, Processor
#define AGESA_CHECK_UMA                0x00028100ul
#define AGESA_DO_RESET                 0x00028101ul
#define AGESA_ALLOCATE_BUFFER          0x00028102ul
#define AGESA_DEALLOCATE_BUFFER        0x00028103ul
#define AGESA_LOCATE_BUFFER            0x00028104ul
#define AGESA_RUNFUNC_ONAP             0x00028105ul

// AGESA ADVANCED CALLOUTS, HyperTransport

// AGESA ADVANCED CALLOUTS, Memory
#define AGESA_READ_SPD                 0x00028140ul
#define AGESA_HOOKBEFORE_DRAM_INIT     0x00028141ul
#define AGESA_HOOKBEFORE_DQS_TRAINING  0x00028142ul
#define AGESA_READ_SPD_RECOVERY        0x00028143ul
#define AGESA_HOOKBEFORE_EXIT_SELF_REF 0x00028144ul
#define AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY     0x00028145ul

// AGESA IDS CALLOUTS
#define AGESA_GET_IDS_INIT_DATA       0x00028200ul

// AGESA GNB CALLOUTS
#define AGESA_GNB_PCIE_SLOT_RESET      0x00028301ul
#define AGESA_GNB_GFX_GET_VBIOS_IMAGE  0x00028302ul

// AGESA FCH CALLOUTS
#define AGESA_FCH_OEM_CALLOUT          0x00028401ul

//------------------------------------------------------------------------
//
// HyperTransport Interface



//-----------------------------------------------------------------------------
//                         HT DEFINITIONS AND MACROS
//
//-----------------------------------------------------------------------------


// Width equates for call backs
#define HT_WIDTH_8_BITS              8                          ///< Specifies 8 bit, or up to 8 bit widths.
#define HT_WIDTH_16_BITS             16                         ///< Specifies 16 bit, or up to 16 bit widths.
#define HT_WIDTH_4_BITS              4
#define HT_WIDTH_2_BITS              2
#define HT_WIDTH_NO_LIMIT            HT_WIDTH_16_BITS

// Frequency Limit equates for call backs which take a frequency supported mask.
#define HT_FREQUENCY_LIMIT_200M      1                           ///< Specifies a limit of no more than 200 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_400M      7                           ///< Specifies a limit of no more than 400 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_600M      0x1F                        ///< Specifies a limit of no more than 600 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_800M      0x3F                        ///< Specifies a limit of no more than 800 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_1000M     0x7F                        ///< Specifies a limit of no more than 1000 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_HT1_ONLY  0x7F                        ///< Specifies a limit of no more than 1000 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_1200M     0xFF                        ///< Specifies a limit of no more than 1200 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_1400M     0x1FF                       ///< Specifies a limit of no more than 1400 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_1600M     0x3FF                       ///< Specifies a limit of no more than 1600 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_1800M     0x7FF                       ///< Specifies a limit of no more than 1800 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_2000M     0xFFF                       ///< Specifies a limit of no more than 2000 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_2200M     0x1FFF                      ///< Specifies a limit of no more than 2200 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_2400M     0x3FFF                      ///< Specifies a limit of no more than 2400 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_2600M     0x7FFF                      ///< Specifies a limit of no more than 2600 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_2800M     0x27FFFul                   ///< Specifies a limit of no more than 2800 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_3000M     0x67FFFul                   ///< Specifies a limit of no more than 3000 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_3200M     0xE7FFFul                   ///< Specifies a limit of no more than 3200 MHz HT frequency.
#define HT_FREQUENCY_LIMIT_3600M     0x1E7FFFul
#define HT_FREQUENCY_LIMIT_MAX       HT_FREQUENCY_LIMIT_3600M
#define HT_FREQUENCY_NO_LIMIT        0xFFFFFFFFul                ///< Specifies a no limit of HT frequency.

// Unit ID Clumping special values
#define HT_CLUMPING_DISABLE          0x00000000ul
#define HT_CLUMPING_NO_LIMIT         0xFFFFFFFFul

#define HT_LIST_TERMINAL             0xFF             ///< End of list.
#define HT_LIST_MATCH_ANY            0xFE             ///< Match Any value, used for Sockets, Links, IO Chain Depth.
#define HT_LIST_MATCH_INTERNAL_LINK  0xFD             ///< Match all of the internal links.

// Event Notify definitions

// Event definitions.

// Coherent subfunction events
#define HT_EVENT_COH_EVENTS             0x10001000ul
#define HT_EVENT_COH_NO_TOPOLOGY        0x10011000ul    ///< See ::HT_EVENT_DATA_COH_NO_TOPOLOGY.
#define HT_EVENT_COH_OBSOLETE000        0x10021000ul    //   No longer used.
#define HT_EVENT_COH_PROCESSOR_TYPE_MIX 0x10031000ul    ///< See ::HT_EVENT_DATA_COH_PROCESSOR_TYPE_MIX.
#define HT_EVENT_COH_NODE_DISCOVERED    0x10041000ul    ///< See ::HT_EVENT_COH_NODE_DISCOVERED.
#define HT_EVENT_COH_MPCAP_MISMATCH     0x10051000ul    ///< See ::HT_EVENT_COH_MPCAP_MISMATCH.

// Non-coherent subfunction events
#define HT_EVENT_NCOH_EVENTS         0x10002000ul
#define HT_EVENT_NCOH_BUID_EXCEED    0x10012000ul       ///< See ::HT_EVENT_DATA_NCOH_BUID_EXCEED
#define HT_EVENT_NCOH_OBSOLETE000    0x10022000ul       //   No longer used.
#define HT_EVENT_NCOH_BUS_MAX_EXCEED 0x10032000ul       ///< See ::HT_EVENT_DATA_NCOH_BUS_MAX_EXCEED.
#define HT_EVENT_NCOH_CFG_MAP_EXCEED 0x10042000ul       ///< See ::HT_EVENT_DATA_NCOH_CFG_MAP_EXCEED.
#define HT_EVENT_NCOH_DEVICE_FAILED  0x10052000ul       ///< See ::HT_EVENT_DATA_NCOH_DEVICE_FAILED
#define HT_EVENT_NCOH_AUTO_DEPTH     0x10062000ul       ///< See ::HT_EVENT_NCOH_AUTO_DEPTH

// Optimization subfunction events
#define HT_EVENT_OPT_EVENTS               0x10003000ul
#define HT_EVENT_OPT_REQUIRED_CAP_RETRY   0x10013000ul  ///< See ::HT_EVENT_DATA_OPT_REQUIRED_CAP.
#define HT_EVENT_OPT_REQUIRED_CAP_GEN3    0x10023000ul  ///< See ::HT_EVENT_DATA_OPT_REQUIRED_CAP.
#define HT_EVENT_OPT_UNUSED_LINKS         0x10033000ul  ///< See ::HT_EVENT_DATA_OPT_UNUSED_LINKS.
#define HT_EVENT_OPT_LINK_PAIR_EXCEED     0x10043000ul  ///< See ::HT_EVENT_DATA_OPT_LINK_PAIR_EXCEED.

// HW Fault events
#define HT_EVENT_HW_EVENTS           0x10004000ul
#define HT_EVENT_HW_SYNCFLOOD        0x10014000ul       ///< See ::HT_EVENT_DATA_HW_SYNCFLOOD.
#define HT_EVENT_HW_HTCRC            0x10024000ul       ///< See ::HT_EVENT_DATA_HW_HT_CRC.

// The Recovery HT component uses 0x10005000 for events.
// For consistency, we avoid that range here.

#define HT_MAX_NC_BUIDS 32
//----------------------------------------------------------------------------
//                         HT TYPEDEFS, STRUCTURES, ENUMS
//
//----------------------------------------------------------------------------

/// Specify the state redundant links are to be left in after match.
///
/// After matching a link for IGNORE_LINK or SKIP_REGANG, the link may be left alone,
/// or powered off.

typedef enum {
  MATCHED,                               ///< The link matches the requested customization.
                                         ///< When used with IGNORE_LINK,
                                         ///< this will generally require other software to initialize the link.
                                         ///< When used with SKIP_REGANG,
                                         ///< the two unganged links will be available for distribution.

  POWERED_OFF,                           ///< Power the link off.  Support may vary based on processor model.
                                         ///< Power Off is only supported for coherent links.
                                         ///< Link power off may occur at a warm reset rather than immediately.
                                         ///< When used with SKIP_REGANG, the paired sublink is powered off, not the matching link.

  UNMATCHED,                             ///< The link should be processed according to normal defaults.
                                         ///< Effectively, the link does not match the requested customization.
                                         ///< This can be used to exclude links from a following match any.

  MaxFinalLinkState                      ///< Not a final link state, use for limit checking.
} FINAL_LINK_STATE;

/// Swap a device from its current id to a new one.

typedef struct {
  IN       UINT8 FromId;                 ///< The device responding to FromId,
  IN       UINT8 ToId;                   ///< will be moved to ToId.
} BUID_SWAP_ITEM;


/// Each Non-coherent chain may have a list of device swaps.  After performing the swaps,
/// the final in order list of device ids is provided. (There can be more swaps than devices.)
/// The unused entries in both are filled with 0xFF.

typedef struct {
  IN       BUID_SWAP_ITEM Swaps[HT_MAX_NC_BUIDS]; ///< The BUID Swaps to perform
  IN       UINT8 FinalIds[HT_MAX_NC_BUIDS];       ///< The ordered final BUIDs, resulting from the swaps
} BUID_SWAP_LIST;


/// Control Manual Initialization of Non-Coherent Chains
///
/// This interface is checked every time a non-coherent chain is
/// processed.  BUID assignment may be controlled explicitly on a
/// non-coherent chain. Provide a swap list.  Swaps controls the
/// BUID assignment and FinalIds provides the device to device
/// Linking.  Device orientation can be detected automatically, or
/// explicitly.  See interface documentation for more details.
///
/// If a manual swap list is not supplied,
/// automatic non-coherent init assigns BUIDs starting at 1 and incrementing sequentially
/// based on each device's unit count.

typedef struct {
  // Match fields
  IN       UINT8 Socket;                       ///< The Socket on which this chain is located
  IN       UINT8 Link;                         ///< The Link on the host for this chain
  // Override fields
  IN       BUID_SWAP_LIST SwapList;            ///< The swap list
} MANUAL_BUID_SWAP_LIST;


/// Override options for DEVICE_CAP_OVERRIDE.
///
/// Specify which override actions should be performed.  For Checks, 1 means to check the item
/// and 0 means to skip the check.  For the override options, 1 means to apply the override and
/// 0 means to ignore the override.

typedef struct {
  IN       UINT32  IsCheckDevVenId:1;     ///< Check Match on Device/Vendor id
  IN       UINT32  IsCheckRevision:1;     ///< Check Match on device Revision
  IN       UINT32  IsOverrideWidthIn:1;   ///< Override Width In
  IN       UINT32  IsOverrideWidthOut:1;  ///< Override Width Out
  IN       UINT32  IsOverrideFreq:1;      ///< Override Frequency
  IN       UINT32  IsOverrideClumping:1;  ///< Override Clumping
  IN       UINT32  IsDoCallout:1;         ///< Make the optional callout
} DEVICE_CAP_OVERRIDE_OPTIONS;

/// Override capabilities of a device.
///
/// This interface is checked once for every Link on every IO device.
/// Provide the width and frequency capability if needed for this device.
/// This is used along with device capabilities, the limit interfaces, and northbridge
/// limits to compute the default settings.  The components of the device's PCI config
/// address are provided, so its settings can be consulted if need be.
/// The optional callout is a catch all.

typedef struct {
  // Match fields
  IN       UINT8 HostSocket;           ///< The Socket on which this chain is located.
  IN       UINT8 HostLink;             ///< The Link on the host for this chain.
  IN       UINT8 Depth;                ///< The Depth in the I/O chain from the Host.
  IN       UINT32 DevVenId;            ///< The Device's PCI Vendor + Device ID (offset 0x00).
  IN       UINT8 Revision;             ///< The Device's PCI Revision field (offset 0x08).
  IN       UINT8 Link;                 ///< The Device's Link number (0 or 1).
  IN       DEVICE_CAP_OVERRIDE_OPTIONS Options; ///< The options for this device override.
  // Override fields
  IN       UINT8 LinkWidthIn;          ///< modify to change the Link Width In.
  IN       UINT8 LinkWidthOut;         ///< modify to change the Link Width Out.
  IN       UINT32 FreqCap;             ///< modify to change the Link's frequency capability.
  IN       UINT32 Clumping;            ///< modify to change Unit ID clumping support.
  IN       CALLOUT_ENTRY Callout;      ///< optional call for really complex cases, or NULL.
} DEVICE_CAP_OVERRIDE;

/// Callout param struct for override capabilities of a device.
///
/// If the optional callout is implemented this param struct is passed to it.

typedef struct {
  IN       AMD_CONFIG_PARAMS StdHeader; ///< Standard configuration header
  // Match fields
  IN       UINT8 HostSocket;           ///< The Socket on which this chain is located.
  IN       UINT8 HostLink;             ///< The Link on the host for this chain.
  IN       UINT8 Depth;                ///< The Depth in the I/O chain from the Host.
  IN       UINT32 DevVenId;            ///< The Device's PCI Vendor + Device ID (offset 0x00).
  IN       UINT8 Revision;             ///< The Device's PCI Revision field (offset 0x08).
  IN       UINT8 Link;                 ///< The Device's Link number (0 or 1).
  IN       PCI_ADDR PciAddress;        ///< The Device's PCI Address.
  // Override fields
     OUT   UINT8 *LinkWidthIn;          ///< modify to change the Link Width In.
     OUT   UINT8 *LinkWidthOut;         ///< modify to change the Link Width Out.
     OUT   UINT32 *FreqCap;             ///< modify to change the Link's frequency capability.
     OUT   UINT32 *Clumping;            ///< modify to change Unit ID clumping support.
} DEVICE_CAP_CALLOUT_PARAMS;

///  Limits for CPU to CPU Links.
///
///  For each coherent connection this interface is checked once.
///  Provide the frequency and width if needed for this Link (usually based on board
///  restriction).  This is used with CPU device capabilities and northbridge limits
///  to compute the default settings.

typedef struct {
  // Match fields
  IN       UINT8 SocketA;                ///< One Socket on which this Link is located
  IN       UINT8 LinkA;                  ///< The Link on this Node
  IN       UINT8 SocketB;                ///< The other Socket on which this Link is located
  IN       UINT8 LinkB;                  ///< The Link on that Node
  // Limit fields
  IN       UINT8 ABLinkWidthLimit;       ///< modify to change the Link Width A->B
  IN       UINT8 BALinkWidthLimit;       ///< modify to change the Link Width B-<A
  IN       UINT32 PcbFreqCap;            ///< modify to change the Link's frequency capability
} CPU_TO_CPU_PCB_LIMITS;

///  Get limits for non-coherent Links.
///
/// For each non-coherent connection this interface is checked once.
/// Provide the frequency and width if needed for this Link (usually based on board
/// restriction).  This is used with device capabilities, device overrides, and northbridge limits
/// to compute the default settings.
///
typedef struct {
  // Match fields
  IN       UINT8 HostSocket;               ///< The Socket on which this Link is located
  IN       UINT8 HostLink;                 ///< The Link about to be initialized
  IN       UINT8 Depth;                    ///< The Depth in the I/O chain from the Host
  // Limit fields
  IN       UINT8 DownstreamLinkWidthLimit; ///< modify to change the Link Width going away from processor
  IN       UINT8 UpstreamLinkWidthLimit;   ///< modify to change the Link Width moving toward processor
  IN       UINT32 PcbFreqCap;              ///< modify to change the Link's frequency capability
} IO_PCB_LIMITS;

///  Manually control bus number assignment.
///
/// This interface is checked every time a non-coherent chain is processed.
/// If a system can not use the auto Bus numbering feature for non-coherent chain bus
/// assignments, this interface can provide explicit control.  For each chain, provide
/// the bus number range to use.

typedef struct {
  // Match fields
  IN       UINT8 Socket;                 ///< The Socket on which this chain is located
  IN       UINT8 Link;                   ///< The Link on the host for this chain
  // Override fields
  IN       UINT8 SecBus;                 ///< Secondary Bus number for this non-coherent chain
  IN       UINT8 SubBus;                 ///< Subordinate Bus number
} OVERRIDE_BUS_NUMBERS;


///  Ignore a Link.
///
///  This interface is checked every time a coherent Link is found and then every
///  time a non-coherent Link from a CPU is found.
///  Any coherent or non-coherent Link from a CPU can be ignored and not used
///  for discovery or initialization.  Useful for connection based systems.
///  (Note: not checked for IO device to IO Device Links.)
///  (Note: not usable for internal links (MCM processors).)

typedef struct {
  // Match fields
  IN       UINT8 Socket;                 ///< The Socket on which this Link is located
  IN       UINT8 Link;                   ///< The Link about to be initialized
  // Customization fields
  IN       FINAL_LINK_STATE LinkState;   ///< The link may be left uninitialized, or powered off.
} IGNORE_LINK;


///  Skip reganging of subLinks.
///
///  This interface is checked whenever two subLinks are both connected to the same CPUs.
///  Normally, unganged sublinks between the same two CPUs are reganged.
///  Provide a matching structure to leave the Links unganged.

typedef struct {
  // Match fields
  IN       UINT8 SocketA;                ///< One Socket on which this Link is located
  IN       UINT8 LinkA;                  ///< The Link on this Node
  IN       UINT8 SocketB;                ///< The other Socket on which this Link is located
  IN       UINT8 LinkB;                  ///< The Link on that Node
  // Customization fields
  IN       FINAL_LINK_STATE LinkState;   ///< The paired sublink may be active, or powered off.
} SKIP_REGANG;

///  The System Socket layout, which sockets are physically connected.
///
///  The hardware method for Socket naming is preferred.  Use this software method only
///  if required.

typedef struct {
  IN       UINT8 CurrentSocket;    ///< The socket from which this connection originates.
  IN       UINT8 CurrentLink;      ///< The Link from the source socket connects to another socket.
  IN       UINT8 TargetSocket;     ///< The target socket which is connected on that link.
} SYSTEM_PHYSICAL_SOCKET_MAP;

//----------------------------------------------------------------------------
///
/// This is the input structure for AmdHtInitialize.
///
typedef struct {
  // Basic level customization
  IN       UINT8 AutoBusStart;           ///< For automatic bus number assignment, starting bus number - usually zero.
                                         ///< @BldCfgItem{BLDCFG_STARTING_BUSNUM}
  IN       UINT8 AutoBusMax;             ///< For automatic bus number assignment, do not assign above max.
                                         ///< @BldCfgItem{BLDCFG_MAXIMUM_BUSNUM}
  IN       UINT8 AutoBusIncrement;       ///< For automatic bus number assignment, each chain gets this many busses.
                                         ///< @BldCfgItem{BLDCFG_ALLOCATED_BUSNUM}

  // Advanced Level Customization
  IN       MANUAL_BUID_SWAP_LIST *ManualBuidSwapList;     ///< Provide Manual Swap List, if any.
                                                          ///< @BldCfgItem{BLDCFG_BUID_SWAP_LIST}
  IN       DEVICE_CAP_OVERRIDE *DeviceCapOverrideList;    ///< Provide Device Overrides, if any.
                                                          ///< @BldCfgItem{BLDCFG_HTDEVICE_CAPABILITIES_OVERRIDE_LIST}
  IN       CPU_TO_CPU_PCB_LIMITS *CpuToCpuPcbLimitsList;   ///< Provide CPU PCB Limits, if any.
                                                          ///< @BldCfgItem{BLDCFG_HTFABRIC_LIMITS_LIST}.
                                                          ///< @n @e Examples: See @ref FrequencyLimitExamples "Frequency Limit Examples".
  IN       IO_PCB_LIMITS *IoPcbLimitsList;                ///< Provide IO PCB Limits, if any.
                                                          ///< @BldCfgItem{BLDCFG_HTCHAIN_LIMITS_LIST}.
                                                          ///< @n @e Examples: See @ref FrequencyLimitExamples "Frequency Limit Examples".
  IN       OVERRIDE_BUS_NUMBERS *OverrideBusNumbersList;  ///< Provide manual Bus Number assignment, if any.
                                                          ///< Use either auto bus numbering or override bus
                                                          ///< numbers, not both.
                                                          ///< @BldCfgItem{BLDCFG_BUS_NUMBERS_LIST}

  IN       IGNORE_LINK *IgnoreLinkList;                   ///< Provide links to ignore, if any.
                                                          ///< @BldCfgItem{BLDCFG_IGNORE_LINK_LIST}
  IN       SKIP_REGANG *SkipRegangList;                   ///< Provide links to remain unganged, if any.
                                                          ///< @BldCfgItem{BLDCFG_LINK_SKIP_REGANG_LIST}
                                                          ///< @n @e Examples: See @ref PerfPerWattHt "Performance-per-watt Optimization".

  // Expert Level Customization
  IN       UINT8 **Topolist;                         ///< Use this topology list in addition to the built in, if not NULL.
                                                     ///< @BldCfgItem{BLDCFG_ADDITIONAL_TOPOLOGIES_LIST}
  IN       SYSTEM_PHYSICAL_SOCKET_MAP *SystemPhysicalSocketMap;
                                                     ///< The hardware socket naming method is preferred,
                                                     ///<  If it can't be used, this provides a software method.
                                                     ///< @BldCfgItem{BLDCFG_SYSTEM_PHYSICAL_SOCKET_MAP}
} AMD_HT_INTERFACE;

//-----------------------------------------------------------------------------
//
// HT Recovery Interface
//


/*-----------------------------------------------------------------------------
 *              HT Recovery DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

// BBHT subfunction events
#define HT_EVENT_BB_EVENTS         0x10005000ul
#define HT_EVENT_BB_BUID_EXCEED    0x10015000ul
#define HT_EVENT_BB_DEVICE_FAILED  0x10055000ul
#define HT_EVENT_BB_AUTO_DEPTH     0x10065000ul

/*----------------------------------------------------------------------------
 *                      HT Recovery   TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */


/// The Interface structure to Recovery HT.

typedef struct {
  IN       MANUAL_BUID_SWAP_LIST *ManualBuidSwapList;  ///< Option to manually control SB link init
                                                       ///< @BldCfgItem{BLDCFG_BUID_SWAP_LIST}
     OUT   UINT32 Depth;           ///< If auto init was used this is set to the depth of the chain,
                                   ///< else, for manual init unmodified.
} AMD_HT_RESET_INTERFACE;


//-----------------------------------------------------------------------------
//                     FCH DEFINITIONS AND MACROS
//
//-----------------------------------------------------------------------------

/// Configuration values for SdConfig
typedef enum {
  SdDisable = 0,                      ///< Disabled
  SdAmda,                             ///< AMDA,  set 24,18,16,  default
  SdDma,                              ///< DMA clear 24, 16, set 18
  SdPio                               ///< PIO clear 24,18,16
} SD_MODE;

/// Configuration values for SdClockControl
typedef enum {
  Sd50MhzTraceCableLengthWithinSixInches = 4,           ///< 50Mhz, default
  Sd40MhzTraceCableLengthSix2ElevenInches = 6,          ///< 40Mhz
  Sd25MhzTraceCableLengthEleven2TwentyfourInches = 7,   ///< 25Mhz
} SD_CLOCK_CONTROL;

/// Configuration values for AzaliaController
typedef enum {
  AzAuto = 0,                         ///< Auto - Detect Azalia controller automatically
  AzDisable,                          ///< Diable - Disable Azalia controller
  AzEnable                            ///< Enable - Enable Azalia controller
} HDA_CONFIG;

/// Configuration values for IrConfig
typedef enum {
  IrDisable  = 0,                     ///< Disable
  IrRxTx0    = 1,                     ///< Rx and Tx0
  IrRxTx1    = 2,                     ///< Rx and Tx1
  IrRxTx0Tx1 = 3                      ///< Rx and both Tx0,Tx1
} IR_CONFIG;

/// Configuration values for SataClass
typedef enum {
  SataNativeIde = 0,                  ///< Native IDE mode
  SataRaid,                           ///< RAID mode
  SataAhci,                           ///< AHCI mode
  SataLegacyIde,                      ///< Legacy IDE mode
  SataIde2Ahci,                       ///< IDE->AHCI mode
  SataAhci7804,                       ///< AHCI mode as 7804 ID (AMD driver)
  SataIde2Ahci7804                    ///< IDE->AHCI mode as 7804 ID (AMD driver)
} SATA_CLASS;

/// Configuration values for BLDCFG_FCH_GPP_LINK_CONFIG
typedef enum {
  PortA4       = 0,                   ///< 4:0:0:0
  PortA2B2     = 2,                   ///< 2:2:0:0
  PortA2B1C1   = 3,                   ///< 2:1:1:0
  PortA1B1C1D1 = 4                    ///< 1:1:1:1
} GPP_LINKMODE;

/// Configuration values for FchPowerFail
typedef enum {
  AlwaysOff   = 0,                    ///< Always power off after power resumes
  AlwaysOn    = 1,                    ///< Always power on after power resumes
  UsePrevious = 3,                    ///< Resume to same setting when power fails
} POWER_FAIL;


/// Configuration values for SATA Link Speed
typedef enum {
  Gen1   = 1,                         ///< SATA port GEN1 speed
  Gen2   = 2,                         ///< SATA port GEN2 speed
  Gen3   = 3,                         ///< SATA port GEN3 speed
} SATA_SPEED;


/// Configuration values for GPIO function
typedef enum {
  Function0   = 0,                    ///< GPIO Function 1
  Function1   = 1,                    ///< GPIO Function 1
  Function2   = 2,                    ///< GPIO Function 2
  Function3   = 3,                    ///< GPIO Function 3
} GPIO_FUN;


/// Configuration values for GPIO_CFG
typedef enum {
  OwnedByEc   = 1 << 0,               ///< This bit can only be written by EC
  OwnedByHost = 1 << 1,               ///< This bit can only be written by host (BIOS)
  Sticky      = 1 << 2,               ///< If set, [6:3] are sticky
  PullUpB     = 1 << 3,               ///< 0: Pullup enable; 1: Pullup disabled
  PullDown    = 1 << 4,               ///< 0: Pulldown disabled; 1: Pulldown enable
  GpioOutEnB  = 1 << 5,               ///< 0: Output enable; 1: Output disable
  GpioOut     = 1 << 6,               ///< Output state when GpioOutEnB is 0
  GpioIn      = 1 << 7,               ///< This bit is read only - current pin state
} CFG_BYTE;

/// FCH GPIO CONTROL
typedef struct {
  IN         UINT8        GpioPin;               ///< Gpio Pin, valid range: 0-67, 128-150, 160-228
  IN         GPIO_FUN     PinFunction;           ///< Multi-function selection
  IN         CFG_BYTE     CfgByte;               ///< GPIO Register value
} GPIO_CONTROL;

///
/// FCH SCI MAP CONTROL
///
typedef struct {
  IN         UINT8        InputPin;              ///< Input Pin, valid range 0-63
  IN         UINT8        GpeMap;                ///< Gpe Map, valid range 0-31
} SCI_MAP_CONTROL;

///
/// FCH SATA PHY CONTROL
///
typedef struct {
  IN         BOOLEAN      CommonPhy;             ///< Common PHY or not
                                      ///<   @li <b>FALSE</b> - Only applied to specified port
                                      ///<   @li <b>TRUE</b>  - Apply to all SATA ports
  IN         SATA_SPEED   Gen;                   ///< SATA speed
  IN         UINT8        Port;                  ///< Port number, valid range: 0-7
  IN         UINT32       PhyData;               ///< SATA PHY data, valid range: 0-0xFFFFFFFF
} SATA_PHY_CONTROL;

///
/// FCH Component Data Structure in InitReset stage
///
typedef struct {
  IN       BOOLEAN      UmiGen2;             ///< Enable Gen2 data rate of UMI
                                             ///<   @li <b>FALSE</b> - Disable Gen2
                                             ///<   @li <b>TRUE</b>  - Enable Gen2

  IN       BOOLEAN      SataEnable;          ///< SATA controller function
                                             ///<   @li <b>FALSE</b> - SATA controller is disabled
                                             ///<   @li <b>TRUE</b> - SATA controller is enabled

  IN       BOOLEAN      IdeEnable;           ///< SATA IDE controller mode enabled/disabled
                                             ///<   @li <b>FALSE</b> - IDE controller is disabled
                                             ///<   @li <b>TRUE</b> - IDE controller is enabled

  IN       BOOLEAN      GppEnable;           ///< Master switch of GPP function
                                             ///<   @li <b>FALSE</b> - GPP disabled
                                             ///<   @li <b>TRUE</b> - GPP enabled

  IN       BOOLEAN      Xhci0Enable;         ///< XHCI0 controller function
                                             ///<   @li <b>FALSE</b> - XHCI0 controller disabled
                                             ///<   @li <b>TRUE</b> - XHCI0 controller enabled

  IN       BOOLEAN      Xhci1Enable;         ///< XHCI1 controller function
                                             ///<   @li <b>FALSE</b> - XHCI1 controller disabled
                                             ///<   @li <b>TRUE</b> - XHCI1 controller enabled
} FCH_RESET_INTERFACE;


///
/// FCH Component Data Structure from InitEnv stage
///
typedef struct {
  IN       SD_MODE      SdConfig;            ///< Secure Digital (SD) controller mode
  IN       HDA_CONFIG   AzaliaController;    ///< Azalia HD Audio Controller

  IN       IR_CONFIG    IrConfig;            ///< Infrared (IR) Configuration
  IN       BOOLEAN      UmiGen2;             ///< Enable Gen2 data rate of UMI
                                             ///<   @li <b>FALSE</b> - Disable Gen2
                                             ///<   @li <b>TRUE</b>  - Enable Gen2

  IN       SATA_CLASS   SataClass;           ///< SATA controller mode
  IN       BOOLEAN      SataEnable;          ///< SATA controller function
                                             ///<   @li <b>FALSE</b> - SATA controller is disabled
                                             ///<   @li <b>TRUE</b> - SATA controller is enabled

  IN       BOOLEAN      IdeEnable;           ///< SATA IDE controller mode enabled/disabled
                                             ///<   @li <b>FALSE</b> - IDE controller is disabled
                                             ///<   @li <b>TRUE</b> - IDE controller is enabled

  IN       BOOLEAN      SataIdeMode;         ///< Native mode of SATA IDE controller
                                             ///<   @li <b>FALSE</b> - Legacy IDE mode
                                             ///<   @li <b>TRUE</b> - Native IDE mode

  IN       BOOLEAN      Ohci1Enable;         ///< OHCI controller #1 Function
                                             ///<   @li <b>FALSE</b> - OHCI1 is disabled
                                             ///<   @li <b>TRUE</b> - OHCI1 is enabled

  IN       BOOLEAN      Ohci2Enable;         ///< OHCI controller #2 Function
                                             ///<   @li <b>FALSE</b> - OHCI2 is disabled
                                             ///<   @li <b>TRUE</b> - OHCI2 is enabled

  IN       BOOLEAN      Ohci3Enable;         ///< OHCI controller #3 Function
                                             ///<   @li <b>FALSE</b> - OHCI3 is disabled
                                             ///<   @li <b>TRUE</b> - OHCI3 is enabled

  IN       BOOLEAN      Ohci4Enable;         ///< OHCI controller #4 Function
                                             ///<   @li <b>FALSE</b> - OHCI4 is disabled
                                             ///<   @li <b>TRUE</b> - OHCI4 is enabled

  IN       BOOLEAN      XhciSwitch;          ///< XHCI controller Function
                                             ///<   @li <b>FALSE</b> - XHCI is disabled
                                             ///<   @li <b>TRUE</b> - XHCI is enabled

  IN       BOOLEAN      GppEnable;           ///< Master switch of GPP function
                                             ///<   @li <b>FALSE</b> - GPP disabled
                                             ///<   @li <b>TRUE</b> - GPP enabled

  IN       POWER_FAIL   FchPowerFail;        ///< FCH power failure option
} FCH_INTERFACE;


/*----------------------------------------------------------------------------
 *   CPU Feature related info
 *----------------------------------------------------------------------------
 */

/// Build Configuration values for BLDCFG_PLATFORM_C1E_MODE
typedef enum {
  C1eModeDisabled                   = 0,   ///< Disabled
  C1eModeAuto                       = 1,   ///< Auto mode enables the best C1e method for the
                                           ///< currently installed processor
  C1eModeHardware                   = 2,   ///< Hardware method
  C1eModeMsgBased                   = 3,   ///< Message-based method
  C1eModeSoftwareDeprecated         = 4,   ///< Deprecated software SMI method.
                                           ///< Refer to "Addendum\Examples\C1eSMMHandler.asm" for
                                           ///< example host BIOS SMM Handler implementation
  C1eModeHardwareSoftwareDeprecated = 5,   ///< Hardware or deprecated software SMI method
  MaxC1eMode                        = 6    ///< Not a valid value, used for verifying input
} PLATFORM_C1E_MODES;

/// Build Configuration values for BLDCFG_PLATFORM_CSTATE_MODE
typedef enum {
  CStateModeDisabled = 0,           ///< Disabled
  CStateModeC6       = 1,           ///< C6 State
  MaxCStateMode      = 2            ///< Not a valid value, used for verifying input
} PLATFORM_CSTATE_MODES;

/// Build Configuration values for BLDCFG_PLATFORM_CPB_MODE
typedef enum {
  CpbModeAuto     = 0,           ///< Auto
  CpbModeDisabled = 1,           ///< Disabled
  MaxCpbMode      = 2            ///< Not a valid value, used for verifying input
} PLATFORM_CPB_MODES;

/// Build Configuration values for BLDCFG_LOW_POWER_PSTATE_FOR_PROCHOT_MODE
typedef enum {
  LOW_POWER_PSTATE_FOR_PROCHOT_AUTO      = 0,           ///< Auto
  LOW_POWER_PSTATE_FOR_PROCHOT_DISABLE   = 1,           ///< Disabled
  MAX_LOW_POWER_PSTATE_FOR_PROCHOT_MODE  = 2            ///< Not a valid value, used for verifying input
} PLATFORM_LOW_POWER_PSTATE_MODES;

/*----------------------------------------------------------------------------
 *   GNB PCIe configuration info
 *----------------------------------------------------------------------------
 */

// Event definitions

#define GNB_EVENT_INVALID_CONFIGURATION               0x20010000ul   // User configuration invalid
#define GNB_EVENT_INVALID_PCIE_TOPOLOGY_CONFIGURATION 0x20010001ul   // Requested lane allocation for PCIe port can not be supported
#define GNB_EVENT_INVALID_PCIE_PORT_CONFIGURATION     0x20010002ul   // Requested incorrect PCIe port device address
#define GNB_EVENT_INVALID_DDI_LINK_CONFIGURATION      0x20010003ul   // Incorrect parameter in DDI link configuration
#define GNB_EVENT_INVALID_LINK_WIDTH_CONFIGURATION    0x20010004ul   // Invalid with for PCIe port or DDI link
#define GNB_EVENT_INVALID_LANES_CONFIGURATION         0x20010005ul   // Lane double subscribe lanes
#define GNB_EVENT_INVALID_DDI_TOPOLOGY_CONFIGURATION  0x20010006ul   // Requested lane allocation for DDI link(s) can not be supported
#define GNB_EVENT_LINK_TRAINING_FAIL                  0x20020000ul   // PCIe Link training fail
#define GNB_EVENT_BROKEN_LANE_RECOVERY                0x20030000ul   // Broken lane workaround applied to recover link training
#define GNB_EVENT_GEN2_SUPPORT_RECOVERY               0x20040000ul   // Scale back to GEN1 to recover link training


#define DESCRIPTOR_TERMINATE_LIST           0x80000000ull
#define DESCRIPTOR_IGNORE                   0x40000000ull

/// PCIe link initialization
typedef enum {
  EndpointDetect = 0,                                     ///< Detect endpoint presence
  EndpointNotPresent                                      ///< Endpoint not present (or connected). Used in case there is alternative way to determine
                                                          ///< if device present on board or in slot. For example GPIO can be used to determine device presence.
} PCIE_ENDPOINT_STATUS;


/// PCIe port misc extended controls
typedef struct  {
  IN      UINT8                     LinkComplianceMode :1;  ///< Force port into compliance mode (device will not be trained, port output compliance pattern)
  IN      UINT8                     LinkSafeMode       :2;  /**< Safe mode PCIe capability. (Parameter may limit PCIe speed requested through PCIe_PORT_DATA::LinkSpeedCapability)
                                                             *  @li @b 0 - port can advertize muximum supported capability
                                                             *  @li @b 1 - port limit advertized capability and speed to PCIe Gen1
                                                             */
  IN      UINT8                     SbLink             :1;  /**< PCIe link type
                                                             *  @li @b 0 - General purpose port
                                                             *  @li @b 1 - Port connected to SB
                                                             */
  IN      UINT8                     ClkPmSupport       :1;  /**< Clock Power Management Support
                                                             *  @li @b 0 - Clock Power Management not configured
                                                             *  @li @b 1 - Clock Power Management configured according to PCIe device capability
                                                             */
} PCIe_PORT_MISC_CONTROL;

/// The IO APIC Interrupt Mapping Info
typedef struct {
  IN      UINT8                     GroupMap;               /**< Group mapping for slot or endpoint device (connected to PCIE port) interrupts .
                                                             *  @li <b>0</b> - IGNORE THIS STRUCTURE AND USE RECOMMENDED SETTINGS
                                                             *  @li <b>1</b> - mapped to Grp 0 (Interrupts 0..3   of IO APIC redirection table)
                                                             *  @li <b>2</b> - mapped to Grp 1 (Interrupts 4..7   of IO APIC redirection table)
                                                             *  @li ...
                                                             *  @li <b>8</b> - mapped to Grp 7 (Interrupts 28..31 of IO APIC redirection table)
                                                             */
  IN      UINT8                     Swizzle;                /**< Swizzle interrupt in the Group.
                                                             *  @li <b>0</b> - ABCD
                                                             *  @li <b>1</b> - BCDA
                                                             *  @li <b>2</b> - CDAB
                                                             *  @li <b>3</b> - DABC
                                                             */
  IN      UINT8                     BridgeInt;              /**< IOAPIC redirection table entry for PCIE bridge interrupt
                                                             *  @li <b>0</b>  - Entry 0  of IO APIC redirection table
                                                             *  @li <b>1</b>  - Entry 1  of IO APIC redirection table
                                                             *  @li ...
                                                             *  @li <b>31</b> - Entry 31 of IO APIC redirection table
                                                             */
} APIC_DEVICE_INFO;

/// PCIe port configuration data
typedef struct  {
  IN       UINT8                   PortPresent;              ///< Enable PCIe port for initialization.
  IN       UINT8                   ChannelType;              /**< Channel type.
                                                               *  @li @b 0 - "lowLoss",
                                                               *  @li @b 1 - "highLoss",
                                                               *  @li @b 2 - "mob0db",
                                                               *  @li @b 3 - "mob3db",
                                                               *  @li @b 4 - "extnd6db"
                                                               *  @li @b 5 - "extnd8db"
                                                               */
  IN       UINT8                   DeviceNumber;             /**< PCI Device number for port.
                                                               *   @li @b 0 - Native port device number
                                                               *   @li @b N - Port device number (See available configurations @ref F12LaneConfigurations "Family 0x12",  @ref F14ONLaneConfigurations "Family 0x14(ON)")
                                                               */
  IN       UINT8                   FunctionNumber;           ///< Reserved for future use
  IN       UINT8                   LinkSpeedCapability;      /**< PCIe link speed/
                                                               *  @li @b 0 - Maximum supported by silicon
                                                               *  @li @b 1 - Gen1
                                                               *  @li @b 2 - Gen2
                                                               *  @li @b 3 - Gen3
                                                               */
  IN       UINT8                   LinkAspm;                 /**< ASPM control. (see AgesaPcieLinkAspm for additional option to control ASPM)
                                                               *  @li @b 0 - Disabled
                                                               *  @li @b 1 - L0s only
                                                               *  @li @b 2 - L1 only
                                                               *  @li @b 3 - L0s and L1
                                                               */
  IN       UINT8                   LinkHotplug;              /**< Hotplug control.
                                                               *  @li @b 0 - Disabled
                                                               *  @li @b 1 - Basic
                                                               *  @li @b 2 - Server
                                                               *  @li @b 3 - Enhanced
                                                               */
  IN       UINT8                   ResetId;                  /**< Arbitrary number greater than 0 assigned by platform firmware for GPIO
                                                               *   identification which control reset for given port.
                                                               *   Each port with unique GPIO should have unique ResetId assigned.
                                                               *   All ports use same GPIO to control reset should have same ResetId assigned.
                                                               *   see AgesaPcieSlotResetContol.
                                                               */
  IN       PCIe_PORT_MISC_CONTROL  MiscControls;             ///< Misc extended controls
  IN       APIC_DEVICE_INFO        ApicDeviceInfo;           ///< IOAPIC device programming info
  IN       PCIE_ENDPOINT_STATUS    EndpointStatus;           ///< PCIe endpoint (device connected to PCIe port) status
} PCIe_PORT_DATA;

/// DDI channel lane mapping
typedef struct {                                          ///< Structure that discribe lane mapping
  IN      UINT8              Lane0   :2;                  /**< Lane 0 mapping
                                                           *  @li @b 0 - Map to lane 0
                                                           *  @li @b 1 - Map to lane 1
                                                           *  @li @b 2 - Map to lane 2
                                                           *  @li @b 2 - Map to lane 3
                                                           */
  IN      UINT8              Lane1   :2;                  ///< Lane 1 mapping (see "Lane 0 mapping")
  IN      UINT8              Lane2   :2;                  ///< Lane 2 mapping (see "Lane 0 mapping")
  IN      UINT8              Lane3   :2;                  ///< Lane 3 mapping (see "Lane 0 mapping")
} CHANNEL_MAPPING;                                        ///< Lane mapping

/// Common Channel Mapping
typedef union {
  IN      UINT8                ChannelMappingValue;       ///< Raw lane mapping
  IN      CHANNEL_MAPPING      ChannelMapping;            ///< Channel mapping
} CONN_CHANNEL_MAPPING;

/// DDI Configuration data
typedef struct  {
  IN       UINT8                ConnectorType;            /**< Display Connector Type
                                                            *  @li @b 0 - DP
                                                            *  @li @b 1 - eDP
                                                            *  @li @b 2 - Single Link DVI-D
                                                            *  @li @b 3 - Dual  Link DVI-D (see @ref F12DualLinkDviDescription "Family 0x12 Dual Link DVI connector description")
                                                            *  @li @b 4 - HDMI
                                                            *  @li @b 5 - Travis DP-to-VGA
                                                            *  @li @b 6 - Travis DP-to-LVDS
                                                            *  @li @b 7 - Hudson-2 NutMeg DP-to-VGA
                                                            *  @li @b 8 - Single Link DVI-I
                                                            *  @li @b 9 - Native CRT (Family 0x14)
                                                            *  @li @b 10 - Native LVDS (Family 0x14)
                                                            *  @li @b 11 - Auto detect LCD panel connector type. VBIOS is able to auto detect the LVDS connector type: native LVDS, eDP or Travis-LVDS
                                                            *              The auto detection method only support panel with EDID.
                                                            */
  IN       UINT8                AuxIndex;                 /**< Indicates which AUX or DDC Line is used
                                                            *  @li @b 0 - AUX1
                                                            *  @li @b 1 - AUX2
                                                            *  @li @b 2 - AUX3
                                                            *  @li @b 3 - AUX4
                                                            *  @li @b 4 - AUX5
                                                            *  @li @b 5 - AUX6
                                                            */
  IN       UINT8                HdpIndex;                 /**< Indicates which HDP pin is used
                                                            *  @li @b 0 - HDP1
                                                            *  @li @b 1 - HDP2
                                                            *  @li @b 2 - HDP3
                                                            *  @li @b 3 - HDP4
                                                            *  @li @b 4 - HDP5
                                                            *  @li @b 5 - HDP6
                                                            */
  IN       CONN_CHANNEL_MAPPING Mapping[2];               /**< Set specific mapping of lanes to connector pins
                                                            *  @li Mapping[0] define mapping for group of 4 lanes starting at PCIe_ENGINE_DATA.StartLane
                                                            *  @li Mapping[1] define mapping for group of 4 lanes ending at PCIe_ENGINE_DATA.EndLane (only applicable for Dual DDI link)
                                                            *  if Mapping[x] set to 0 than default mapping assumed
                                                            */
  IN       UINT8                LanePnInversionMask;      /**< Specifies whether to invert the state of P and N for each lane. Each bit represents a PCIe lane on the DDI port.
                                                            *  @li 0 - Do not invert (default)
                                                            *  @li 1 - Invert P and N on this lane
                                                            */
  IN       UINT8                Flags;                    /**< Capabilities flags
                                                            *  @li Flags bit[0] DDI_DATA_FLAGS_DP1_1_ONLY Selects downgrade PHY link to DP1.1
                                                            *  @li Flags bit[7:1] Reserved
                                                            */
} PCIe_DDI_DATA;

/// Engine Configuration
typedef struct {
  IN       UINT8                EngineType;               /**< Engine type
                                                           *  @li @b 0 -  Ignore engine configuration
                                                           *  @li @b 1 -  PCIe port
                                                           *  @li @b 2 -  DDI
                                                           */
  IN       UINT16               StartLane;                /**< Start Lane ID (in reversed configuration StartLane > EndLane)
                                                           * See lane description for @ref F12PcieLaneDescription "Family 0x12"
                                                           * @ref F14ONPcieLaneDescription "Family 0x14(ON)".
                                                           * See lane configurations for @ref F12LaneConfigurations "Family 0x12"
                                                           * @ref F14ONLaneConfigurations "Family 0x14(ON)".
                                                           */
  IN       UINT16               EndLane;                  /**< End lane ID (in reversed configuration StartLane > EndLane)
                                                           * See lane description for @ref F12PcieLaneDescription "Family 0x12",
                                                           * @ref F14ONPcieLaneDescription "Family 0x14(ON)".
                                                           * See lane configurations for @ref F12LaneConfigurations "Family 0x12"
                                                           * @ref F14ONLaneConfigurations "Family 0x14(ON)".
                                                           */

} PCIe_ENGINE_DATA;

/// PCIe port descriptor
typedef struct {
  IN       UINT32               Flags;                    /**< Descriptor flags
                                                           * @li @b Bit31 - last descriptor in complex
                                                           */
  IN       PCIe_ENGINE_DATA     EngineData;               ///< Engine data
  IN       PCIe_PORT_DATA       Port;                     ///< PCIe port specific configuration info
} PCIe_PORT_DESCRIPTOR;

/// DDI descriptor
typedef struct {
  IN       UINT32               Flags;                    /**< Descriptor flags
                                                           * @li @b Bit31 - last descriptor in complex
                                                           */
  IN       PCIe_ENGINE_DATA     EngineData;               ///< Engine data
  IN       PCIe_DDI_DATA        Ddi;                      ///< DDI port specific configuration info
} PCIe_DDI_DESCRIPTOR;

/// PCIe Complex descriptor
typedef struct {
  IN       UINT32               Flags;                    /**< Descriptor flags
                                                           * @li @b Bit31 - last descriptor in topology
                                                           */
  IN       UINT32               SocketId;                 ///< Socket Id
  IN       CONST PCIe_PORT_DESCRIPTOR *PciePortList;            ///< Pointer to array of PCIe port descriptors or NULL (Last element of array must be terminated with DESCRIPTOR_TERMINATE_LIST).
  IN       CONST PCIe_DDI_DESCRIPTOR  *DdiLinkList;             ///< Pointer to array DDI link descriptors (Last element of array must be terminated with DESCRIPTOR_TERMINATE_LIST).
  IN       VOID                 *Reserved;                ///< Reserved for future use
} PCIe_COMPLEX_DESCRIPTOR;

/// Action to control PCIe slot reset
typedef enum {
  AssertSlotReset,                                        ///< Assert slot reset
  DeassertSlotReset                                       ///< Deassert slot reset
} PCIE_RESET_CONTROL;

///Slot Reset Info
typedef struct {
  IN      AMD_CONFIG_PARAMS     StdHeader;                ///< Standard configuration header
  IN      UINT8                 ResetId;                  ///< Slot reset ID as specified in PCIe_PORT_DESCRIPTOR
  IN      UINT8                 ResetControl;             ///< Reset control as in PCIE_RESET_CONTROL
} PCIe_SLOT_RESET_INFO;

#define GFX_VBIOS_IMAGE_FLAG_SPECIAL_POST  0x1

///VBIOS image info
typedef struct {
  IN      AMD_CONFIG_PARAMS     StdHeader;                ///< Standard configuration header
  OUT     VOID                  *ImagePtr;                ///< Pointer to VBIOS image
  IN      PCI_ADDR              GfxPciAddress;            ///< PCI address of integrated graphics controller
  IN      UINT32                Flags;                    ///< BIT[0] - special repost requred
} GFX_VBIOS_IMAGE_INFO;

/// Engine descriptor type
typedef enum {
  PcieUnusedEngine = 0,                                   ///< Unused descriptor
  PciePortEngine = 1,                                     ///< PCIe port
  PcieDdiEngine = 2,                                      ///< DDI
  MaxPcieEngine                                           ///< Max engine type for boundary check.
} PCIE_ENGINE_TYPE;

/// PCIe link capability/speed
typedef enum  {
  PcieGenMaxSupported,                                    ///< Maximum supported
  PcieGen1 = 1,                                           ///< Gen1
  PcieGen2,                                               ///< Gen2
  PcieGen3,                                               ///< Gen3
  MaxPcieGen                                              ///< Max Gen for boundary check
} PCIE_LINK_SPEED_CAP;

/// PCIe PSPP Power policy
typedef enum  {
  PsppDisabled,                                           ///< PSPP disabled
  PsppPerformance = 1,                                    ///< Performance
  PsppBalanceHigh,                                        ///< Balance-High
  PsppBalanceLow,                                         ///< Balance-Low
  PsppPowerSaving,                                        ///< Power Saving
  MaxPspp                                                 ///< Max Pspp for boundary check
} PCIE_PSPP_POLICY;

/// DDI display connector type
typedef enum {
  ConnectorTypeDP,                                        ///< DP
  ConnectorTypeEDP,                                       ///< eDP
  ConnectorTypeSingleLinkDVI,                             ///< Single Link DVI-D
  ConnectorTypeDualLinkDVI,                               ///< Dual  Link DVI-D
  ConnectorTypeHDMI,                                      ///< HDMI
  ConnectorTypeTravisDpToVga,                             ///< Travis DP-to-VGA
  ConnectorTypeTravisDpToLvds,                            ///< Travis DP-to-LVDS
  ConnectorTypeNutmegDpToVga,                             ///< Hudson-2 NutMeg DP-to-VGA
  ConnectorTypeSingleLinkDviI,                            ///< Single Link DVI-I
  ConnectorTypeCrt,                                       ///< CRT (VGA)
  ConnectorTypeLvds,                                      ///< LVDS
  ConnectorTypeEDPToLvds,                                 ///< 3rd party common eDP-to-LVDS translator chip without AMD SW init
  ConnectorTypeEDPToRealtecLvds,                          ///< Realtek eDP-to-LVDS tansaltor which require AMD SW init
  ConnectorTypeAutoDetect,                                ///< VBIOS auto detect connector type (native LVDS, eDP or Travis-LVDS)
  MaxConnectorType                                        ///< Not valid value, used to verify input
} PCIE_CONNECTOR_TYPE;

/// PCIe link channel type
typedef enum {
  ChannelTypeLowLoss,                                     ///< Low Loss
  ChannelTypeHighLoss,                                    ///< High Loss
  ChannelTypeMob0db,                                      ///< Mobile 0dB
  ChannelTypeMob3db,                                      ///< Mobile 3dB
  ChannelTypeExt6db,                                      ///< Extended 6dB
  ChannelTypeExt8db,                                      ///< Extended 8dB
  MaxChannelType                                          ///< Not valid value, used to verify input
} PCIE_CHANNEL_TYPE;

/// PCIe link ASPM
typedef enum {
  AspmDisabled,                                           ///< Disabled
  AspmL0s,                                                ///< PCIe L0s link state
  AspmL1,                                                 ///< PCIe L1 link state
  AspmL0sL1,                                              ///< PCIe L0s & L1 link state
  MaxAspm                                                 ///< Not valid value, used to verify input
} PCIE_ASPM_TYPE;

/// PCIe link hotplug support
typedef enum {
  HotplugDisabled,                                        ///< Hotplug disable
  HotplugBasic,                                           ///< Basic Hotplug
  HotplugServer,                                          ///< Server Hotplug
  HotplugEnhanced,                                        ///< Enhanced
  HotplugInboard,                                         ///< Inboard
  MaxHotplug                                              ///< Not valid value, used to verify input
} PCIE_HOTPLUG_TYPE;

/// PCIe link initialization
typedef enum {
  PortDisabled,                                           ///< Disable
  PortEnabled                                             ///< Enable
} PCIE_PORT_ENABLE;

/// DDI Aux channel
typedef enum {
  Aux1,                                                   ///< Aux1
  Aux2,                                                   ///< Aux2
  Aux3,                                                   ///< Aux3
  Aux4,                                                   ///< Aux4
  Aux5,                                                   ///< Aux5
  Aux6,                                                   ///< Aux6
  MaxAux                                                  ///< Not valid value, used to verify input
} PCIE_AUX_TYPE;

/// DDI Hdp Index
typedef enum {
  Hdp1,                                                   ///< Hdp1
  Hdp2,                                                   ///< Hdp2
  Hdp3,                                                   ///< Hdp3
  Hdp4,                                                   ///< Hdp4
  Hdp5,                                                   ///< Hdp5
  Hdp6,                                                   ///< Hdp6
  MaxHdp                                                  ///< Not valid value, used to verify input
} PCIE_HDP_TYPE;

/// PCIe_DDI_DATA.Flags definitions
#define DDI_DATA_FLAGS_DP1_1_ONLY   0x01                  ///< BIT[0] Selects downgrade PHY link to DP1.1


// Macro for statically initialization of various structures
#define  PCIE_ENGINE_DATA_INITIALIZER(mType, mStartLane, mEndLane) {mType, mStartLane, mEndLane}
#define  PCIE_PORT_DATA_INITIALIZER(mPortPresent, mChannelType, mDevAddress, mHotplug, mMaxLinkSpeed, mMaxLinkCap, mAspm, mResetId) \
{mPortPresent, mChannelType, mDevAddress, 0, mMaxLinkSpeed, mAspm, mHotplug, mResetId, {0, mMaxLinkCap, 0, 0}, {0, 0, 0}, EndpointDetect}
#define  PCIE_DDI_DATA_INITIALIZER(mConnectorType, mAuxIndex, mHpdIndex ) \
{mConnectorType, mAuxIndex, mHpdIndex, {{0}, {0}}, 0, 0}
#define  PCIE_DDI_DATA_INITIALIZER_V1(mConnectorType, mAuxIndex, mHpdIndex, mMapping0, mMapping1, mPNInversion) \
{mConnectorType, mAuxIndex, mHpdIndex, {mMapping0, mMapping1}, mPNInversion, 0}
#define  PCIE_DDI_DATA_INITIALIZER_V2(mConnectorType, mAuxIndex, mHpdIndex, mMapping0, mMapping1, mPNInversion, mFlags) \
{mConnectorType, mAuxIndex, mHpdIndex, {mMapping0, mMapping1}, mPNInversion, mFlags}

///IOMMU requestor ID
typedef struct {
  IN       UINT16     Bus       :8;                                ///< Bus
  IN       UINT16     Device    :5;                                ///< Device
  IN       UINT16     Function  :3;                                ///< Function
} IOMMU_REQUESTOR_ID;

/// IVMD exclusion range descriptor
typedef struct {
  IN       UINT32               Flags;                    /**< Descriptor flags
                                                           * @li @b Flags[31] - Terminate descriptor array.
                                                           * @li @b Flags[30] - Ignore descriptor.
                                                           */
  IN       IOMMU_REQUESTOR_ID   RequestorIdStart;         ///< Requestor ID start
  IN       IOMMU_REQUESTOR_ID   RequestorIdEnd;           ///< Requestor ID end (use same as start for single ID)
  IN       UINT64               RangeBaseAddress;         ///< Phisical base address of exclusion range
  IN       UINT64               RangeLength;              ///< Length of exclusion range in bytes
} IOMMU_EXCLUSION_RANGE_DESCRIPTOR;

/*----------------------------------------------------------------------------
 *   GNB configuration info
 *----------------------------------------------------------------------------
 */

/// LVDS Misc Control Field
typedef struct {
  IN  UINT8     FpdiMode:1;          ///< This item configures LVDS 888bit panel mode
                                     ///< @li FALSE = LVDS 888 panel in LDI mode
                                     ///< @li TRUE =  LVDS 888 panel in FPDI mode
                                     ///< @BldCfgItem{BLDCFG_LVDS_MISC_888_FPDI_MODE}
  IN  UINT8     DlChSwap:1;          ///< This item configures LVDS panel lower and upper link mapping
                                     ///< @li FALSE = Lower link and upper link not swap
                                     ///< @li TRUE = Lower link and upper link are swapped
                                     ///< @BldCfgItem{BLDCFG_LVDS_MISC_DL_CH_SWAP}
  IN  UINT8     VsyncActiveLow:1;    ///< This item configures polarity of frame pulse encoded in lvds data stream
                                     ///< @li FALSE = Active high Frame Pulse/Vsync
                                     ///< @li TRUE = Active low Frame Pulse/Vsync
                                     ///< @BldCfgItem{BLDCFG_LVDS_MISC_VSYNC_ACTIVE_LOW}
  IN  UINT8     HsyncActiveLow:1;    ///< This item configures polarity of line pulse encoded in lvds data
                                     ///< @li FALSE = Active high Line Pulse
                                     ///< @li TRUE = Active low Line Pulse / Hsync
                                     ///< @BldCfgItem{BLDCFG_LVDS_MISC_HSYNC_ACTIVE_LOW}
  IN  UINT8     BLONActiveLow:1;     ///< This item configures polarity of signal sent to digital BLON output pin
                                     ///< @li FALSE = Not inverted(active high)
                                     ///< @li TRUE = Inverted (active low)
                                     ///< @BldCfgItem{BLDCFG_LVDS_MISC_BLON_ACTIVE_LOW}
  IN  UINT8     TravisLvdsVoltOverwriteEn:1;  ///< This item configures polarity of Travis LVDS output voltage overwrite
                                              ///< @li FALSE = Travis LVDS output voltage overwrite disable, use VBIOS default setting.
                                              ///< @li TRUE = Use ucTravisLVDSVolAdjust value to program Travis register LVDS_CTRL_4
                                              ///< @BldCfgItem{BLDCFG_LVDS_MISC_VOLT_OVERWRITE_ENABLE}
  IN  UINT8     Reserved:2;          ///< Reserved
} LVDS_MISC_CONTROL_FIELD;

/// LVDS Misc Control
typedef union _LVDS_MISC_CONTROL {
  IN LVDS_MISC_CONTROL_FIELD Field;  ///< LVDS_MISC_CONTROL_FIELD
  IN UINT8   Value;                  ///< LVDS Misc Control Value
} LVDS_MISC_CONTROL;

/// Display Misc Control Field
typedef struct {
  IN  UINT8     Reserved1:3;                  ///< Reserved
  IN  UINT8     VbiosFastBootEn:1;            ///< This item configures VBIOS skip display device detection in every set mode if LCD panel is connect and LID is open.
                                              ///< @li FALSE = VBIOS fast boot is disable.
                                              ///< @li TRUE = VBIOS fast boot is enable.
                                              ///< @BldCfgItem{BLDCFG_DISPLAY_MISC_VBIOS_FAST_BOOT_ENABLE}
  IN  UINT8     Reserved2:4;                  ///< Reserved
} DISPLAY_MISC_CONTROL_FIELD;

/// LVDS Misc Control
typedef union _DISPLAY_MISC_CONTROL {
  IN DISPLAY_MISC_CONTROL_FIELD Field;  ///< DISPLAY_MISC_CONTROL_FIELD
  IN UINT8   Value;                     ///< Display Misc Control Value
} DISPLAY_MISC_CONTROL;

/// POST Configuration settings for GNB.
typedef struct {
  IN UINT8       IgpuEnableDisablePolicy;   ///< This item defines the iGPU Enable/Disable policy
                                            ///< @li 0 = Auto - use existing default -
                                            ///< @li 1 = Disable iGPU if any PCIe/PCI graphics card present
                                            ///< @BldCfgItem{BLDCFG_IGPU_ENABLE_DISABLE_POLICY}
} GNB_POST_CONFIGURATION;

/// iGPU Enable/Disable Policy values
#define IGPU_DISABLE_AUTO        0        ///< Auto setting - disable iGPU if ANY PCI graphics or non-AMD PCIe graphics
#define IGPU_DISABLE_ANY_PCIE    1        ///< Disable iGPU if any PCI or PCIE graphics card is present

/// ENV Configuration settings for GNB.
typedef struct {
  IN  UINT8     Gnb3dStereoPinIndex;      ///< 3D Stereo Pin ID.
                                          ///< @li 0 = Stereo 3D is disabled (default).
                                          ///< @li 1 = Use processor pin HPD1.
                                          ///< @li 2 = Use processor pin HPD2
                                          ///< @li 3 = Use processor pin HPD3
                                          ///< @li 4 = Use processor pin HPD4
                                          ///< @li 5 = Use processor pin HPD5
                                          ///< @li 6 = Use processor pin HPD6
                                          ///< @BldCfgItem{BLDCFG_STEREO_3D_PINOUT}
  IN  BOOLEAN    IommuSupport;            ///< IOMMU support.
                                          ///< @li FALSE = Disabled. Disable and hide IOMMU device.
                                          ///< @li TRUE  = Initialize IOMMU subsystem. Generate ACPI IVRS table.
                                          ///< BldCfgItem{BLDCFG_IOMMU_SUPPORT}
  IN  UINT16     LvdsSpreadSpectrum;      ///< Spread spectrum value in 0.01 %
                                          ///< BldCfgItem{BLDCFG_GFX_LVDS_SPREAD_SPECTRUM}
  IN  UINT16     LvdsSpreadSpectrumRate;  ///< Spread spectrum frequency used by SS hardware logic in unit of 10Hz, 0 - default frequency 40kHz
                                          ///< BldCfgItem{BLDCFG_GFX_LVDS_SPREAD_SPECTRUM_RATE}
  IN  UINT8      LvdsPowerOnSeqDigonToDe;    ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWER_ON_SEQ_DIGON_TO_DE}
  IN  UINT8      LvdsPowerOnSeqDeToVaryBl;   ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWER_ON_SEQ_DE_TO_VARY_BL}
  IN  UINT8      LvdsPowerOnSeqDeToDigon;    ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWER_ON_SEQ_DE_TO_DIGON}
  IN  UINT8      LvdsPowerOnSeqVaryBlToDe;   ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWERS_ON_SEQ_VARY_BL_TO_DE}
  IN  UINT8      LvdsPowerOnSeqOnToOffDelay; ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWER_ON_SEQ_ON_TO_OFF_DELAY}
  IN  UINT8      LvdsPowerOnSeqVaryBlToBlon; ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWER_ON_SEQ_VARY_BL_TO_BLON}
  IN  UINT8      LvdsPowerOnSeqBlonToVaryBl; ///< This item configures panel initialization timing.
                                             ///< @BldCfgItem{BLDCFG_LVDS_POWER_ON_SEQ_BLON_TO_VARY_BL}
  IN  UINT16     LvdsMaxPixelClockFreq;      ///< This item configures the maximum pixel clock frequency supported.
                                             ///< @BldCfgItem{BLDCFG_LVDS_MAX_PIXEL_CLOCK_FREQ}
  IN  UINT32     LcdBitDepthControlValue;    ///< This item configures the LCD bit depth control settings.
                                             ///< @BldCfgItem{BLDCFG_LCD_BIT_DEPTH_CONTROL_VALUE}
  IN  UINT8      Lvds24bbpPanelMode;         ///< This item configures the LVDS 24 BBP mode.
                                             ///< @BldCfgItem{BLDCFG_LVDS_24BBP_PANEL_MODE}
  IN  LVDS_MISC_CONTROL      LvdsMiscControl;///< This item configures LVDS swap/Hsync/Vsync/BLON
  IN  UINT16     PcieRefClkSpreadSpectrum;   ///< Spread spectrum value in 0.01 %
                                             ///< @BldCfgItem{BLDCFG_PCIE_REFCLK_SPREAD_SPECTRUM}
  IN  BOOLEAN    GnbRemoteDisplaySupport;    ///< This item enables Wireless Display Support
                                             ///< @li TRUE  = Enable Wireless Display Support
                                             ///< @li FALSE = Disable Wireless Display Support
                                             ///< @BldCfgItem{BLDCFG_REMOTE_DISPLAY_SUPPORT}
  IN UINT8       LvdsMiscVoltAdjustment;     ///< Travis register LVDS_CTRL_4 to adjust LVDS output voltage
                                             ///< @BldCfgItem{BLDCFG_LVDS_MISC_VOL_ADJUSTMENT}
  IN DISPLAY_MISC_CONTROL DisplayMiscControl;///< This item configures display misc control
} GNB_ENV_CONFIGURATION;

/// Configuration settings for GNB.
typedef struct {
  IN  UINT8     iGpuVgaMode;                 ///< VGA resourses decoding configuration for iGPU
                                             ///< @li 0 = iGPU decode all VGA resourses (must be promary VGA adapter)
                                             ///< @li 1 = iGPU will not decode any VGA resourses (must be secondary graphics adapter)
} GNB_MID_CONFIGURATION;

/// GNB configuration info
typedef struct {
  IN       const PCIe_COMPLEX_DESCRIPTOR  *PcieComplexList;  /**< Pointer to array of structures describe PCIe topology on each processor package or NULL.
                                                        * Last element of array must be terminated with DESCRIPTOR_TERMINATE_LIST
                                                        * Example of topology definition for single socket system:
                                                        * @code
                                                        *  PCIe_PORT_DESCRIPTOR PortList [] = {
                                                        *    // Initialize Port descriptor (PCIe port, Lanes 8:15, PCI Device Number 2, ...)
                                                        *    {
                                                        *      0,   //Descriptor flags
                                                        *      PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 8, 15),
                                                        *      PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 2, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 0)
                                                        *    },
                                                        *    // Initialize Port descriptor (PCIe port, Lanes 16:19, PCI Device Number 3, ...)
                                                        *    {
                                                        *      0,   //Descriptor flags
                                                        *      PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 16, 19),
                                                        *      PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 3, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 0)
                                                        *    },
                                                        *    // Initialize Port descriptor (PCIe port, Lanes 4, PCI Device Number 4, ...)
                                                        *    {
                                                        *      DESCRIPTOR_TERMINATE_LIST, //Descriptor flags  !!!IMPORTANT!!! Terminate last element of array
                                                        *      PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 4, 4),
                                                        *      PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 4, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 0)
                                                        *    }
                                                        *  };
                                                        *  PCIe_PORT_DESCRIPTOR DdiList [] = {
                                                        *    // Initialize Ddi descriptor (DDI interface Lanes 24:27, Display Port Connector, ...)
                                                        *    {
                                                        *      0,   //Descriptor flags
                                                        *      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 24, 27),
                                                        *      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDP, Aux1, Hdp1)
                                                        *    },
                                                        *    // Initialize Ddi descriptor (DDI interface Lanes 28:31, HDMI, ...)
                                                        *    {
                                                        *      DESCRIPTOR_TERMINATE_LIST, //Descriptor flags  !!!IMPORTANT!!! Terminate last element of array
                                                        *      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 28, 31),
                                                        *      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeHDMI, Aux2, Hdp2)
                                                        *    }
                                                        *  };
                                                        * PCIe_COMPLEX_DESCRIPTOR PlatformTopology = {
                                                        *   DESCRIPTOR_TERMINATE_LIST, //Descriptor flags  !!!IMPORTANT!!! Terminate complexes list
                                                        *   0,  //Socket ID
                                                        *   &PortList[0],
                                                        *   &DdiList[0],
                                                        * }
                                                        * @endcode
                                                        */
  IN       UINT8                    PsppPolicy;         /**< PSPP (PCIe Speed Power Policy)
                                                         *  @li @b 0 - Disabled
                                                         *  @li @b 1 - Performance
                                                         *  @li @b 2 - Balance-High
                                                         *  @li @b 3 - Balance-Low
                                                         *  @li @b 4 - Power Saving
                                                         */

} GNB_CONFIGURATION;
//
//  MEMORY-SPECIFIC DATA STRUCTURES
//
//
//
//
// AGESA MAXIMIUM VALUES
//
//   These Max values are used to define array sizes and associated loop
//   counts in the code.  They reflect the maximum values that AGESA
//   currently supports and does not necessarily reflect the hardware
//   capabilities of configuration.
//

#define MAX_SOCKETS_SUPPORTED   8   ///< Max number of sockets in system
#define MAX_CHANNELS_PER_SOCKET 4   ///< Max Channels per sockets
#define MAX_DIMMS_PER_CHANNEL   4   ///< Max DIMMs on a memory channel (independent of platform)
#define NUMBER_OF_DELAY_TABLES  9   ///< Number of tables defined in CH_DEF_STRUCT.
                                    ///< Eg: UINT16  *RcvEnDlys;
                                    ///<     UINT8   *WrDqsDlys;
                                    ///<     UINT8   *RdDqsDlys;
                                    ///<     UINT8   *WrDatDlys;
                                    ///<     UINT8   *RdDqsMinDlys;
                                    ///<     UINT8   *RdDqsMaxDlys;
                                    ///<     UINT8   *WrDatMinDlys;
                                    ///<     UINT8   *WrDatMaxDlys;
#define NUMBER_OF_FAILURE_MASK_TABLES 1 ///< Number of failure mask tables

#define MAX_PLATFORM_TYPES     16   ///< Platform types per system

#define MCT_TRNG_KEEPOUT_START  0x00004000ul    ///< base [39:8]
#define MCT_TRNG_KEEPOUT_END    0x00007FFFul    ///< base [39:8]

#define UMA_ATTRIBUTE_INTERLEAVE 0x80000000ul   ///< Uma Region is interleaved
#define UMA_ATTRIBUTE_ON_DCT0    0x40000000ul   ///< UMA resides on memory that belongs to DCT0
#define UMA_ATTRIBUTE_ON_DCT1    0x20000000ul   ///< UMA resides on memory that belongs to DCT1

typedef UINT8 PSO_TABLE;            ///< Platform Configuration Table

//        AGESA DEFINITIONS
//
//        Many of these are derived from the platform and hardware specific definitions

/// EccSymbolSize override value
#define ECCSYMBOLSIZE_USE_BKDG      0   ///< Use BKDG Recommended Value
#define ECCSYMBOLSIZE_FORCE_X4      4   ///< Force to x4
#define ECCSYMBOLSIZE_FORCE_X8      8   ///< Force to x8
/// CPU Package Type
#define PT_L1       0                 ///< L1 Package type
#define PT_M2       1                 ///< AM Package type
#define PT_S1       2                 ///< S1 Package type

/// Structures use to pass system Logical CPU-ID
typedef struct {
  IN OUT   UINT64 Family;             ///< Indicates logical ID Family
  IN OUT   UINT64 Revision;           ///< Indicates logical ID Family
} CPU_LOGICAL_ID;

/// Build Configuration values for BLDCFG_AMD_PLATFORM_TYPE
typedef enum {
  AMD_PLATFORM_SERVER = 0x8000,     ///< Server
  AMD_PLATFORM_DESKTOP = 0x10000,   ///< Desktop
  AMD_PLATFORM_MOBILE = 0x20000,    ///< Mobile
} AMD_PLATFORM_TYPE;

/// Dram technology type
typedef enum {
  DDR2_TECHNOLOGY,      ///< DDR2 technology
  DDR3_TECHNOLOGY       ///< DDR3 technology
} TECHNOLOGY_TYPE;

/// Build Configuration values for BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT & BLDCFG_MEMORY_CLOCK_SELECT
typedef unsigned int MEMORY_BUS_SPEED;

#define DDR400_FREQUENCY		 200 ///< DDR  400
#define DDR533_FREQUENCY		 266 ///< DDR  533
#define DDR667_FREQUENCY		 333 ///< DDR  667
#define DDR800_FREQUENCY		 400 ///< DDR  800
#define DDR1066_FREQUENCY		 533 ///< DDR 1066
#define DDR1333_FREQUENCY		 667 ///< DDR 1333
#define DDR1600_FREQUENCY		 800 ///< DDR 1600
#define DDR1866_FREQUENCY		 933 ///< DDR 1866
#define DDR2100_FREQUENCY		1050 ///< DDR 2100
#define DDR2133_FREQUENCY		1066 ///< DDR 2133
#define DDR2400_FREQUENCY		1200 ///< DDR 2400
#define UNSUPPORTED_DDR_FREQUENCY	1201 ///< Highest limit of DDR frequency

/// Build Configuration values for BLDCFG_MEMORY_QUADRANK_TYPE
typedef unsigned int QUANDRANK_TYPE;

#define QUADRANK_REGISTERED		0 ///< Quadrank registered DIMM
#define QUADRANK_UNBUFFERED		1 ///< Quadrank unbuffered DIMM

/// Build Configuration values for BLDCFG_TIMING_MODE_SELECT
typedef unsigned int USER_MEMORY_TIMING_MODE;

#define TIMING_MODE_AUTO		0 ///< Use best rate possible
#define TIMING_MODE_LIMITED		1 ///< Set user top limit
#define TIMING_MODE_SPECIFIC		2 ///< Set user specified speed

/// Build Configuration values for BLDCFG_POWER_DOWN_MODE
typedef unsigned int POWER_DOWN_MODE;

#define POWER_DOWN_BY_CHANNEL		0 ///< Channel power down mode
#define POWER_DOWN_BY_CHIP_SELECT	1 ///< Chip select power down mode
#define POWER_DOWN_MODE_AUTO		2 ///< AGESA to select power down mode

/// Low voltage support
typedef enum {
  VOLT_INITIAL,              ///< Initial value for VDDIO
  VOLT1_5,                   ///< 1.5 Volt
  VOLT1_35,                  ///< 1.35 Volt
  VOLT1_25,                  ///< 1.25 Volt
  VOLT_UNSUPPORTED = 0xFF    ///< No common voltage found
} DIMM_VOLTAGE;

/// UMA Mode
typedef enum {
  UMA_NONE = 0,              ///< UMA None
  UMA_SPECIFIED = 1,         ///< UMA Specified
  UMA_AUTO = 2               ///< UMA Auto
} UMA_MODE;

/// Force Training Mode
typedef enum {
  FORCE_TRAIN_1D = 0,              ///< 1D Training only
  FORCE_ENUM1  = 1,
  FORCE_TRAIN_AUTO = 2
} FORCE_TRAIN_MODE;

///  The possible DRAM prefetch mode settings.
typedef enum  {
  DRAM_PREFETCHER_AUTO,                         ///< Use the recommended setting for the processor. In most cases, the recommended setting is enabled.
  DISABLE_DRAM_PREFETCH_FOR_IO,                 ///< Disable DRAM prefetching for I/O requests only.
  DISABLE_DRAM_PREFETCH_FOR_CPU,                ///< Disable DRAM prefetching for requests from processor cores only.
  DISABLE_DRAM_PREFETCHER,                      ///< Disable DRAM prefetching.
  MAX_DRAM_FREFETCH_MODE                        ///< Not a DRAM prefetch mode, use for limit checking.
} DRAM_PREFETCH_MODE;

/// Build Configuration values for BLDCFG_UMA_ALIGNMENT
typedef enum {
  NO_UMA_ALIGNED = 0x00FFFFFF,           ///< NO UMA aligned
  UMA_4MB_ALIGNED = 0x00FFFFC0,          ///< UMA 4MB aligned
  UMA_128MB_ALIGNED = 0x00FFF800,        ///< UMA 128MB aligned
  UMA_256MB_ALIGNED = 0x00FFF000,        ///< UMA 256MB aligned
  UMA_512MB_ALIGNED = 0x00FFE000,        ///< UMA 512MB aligned
} UMA_ALIGNMENT;

///
///   Global MCT Configuration Status Word (GStatus)
///
typedef enum {
  GsbMTRRshort,              ///< Ran out of MTRRs while mapping memory
  GsbAllECCDimms,            ///< All banks of all Nodes are ECC capable
  GsbDramECCDis,             ///< Dram ECC requested but not enabled.
  GsbSoftHole,               ///< A Node Base gap was created
  GsbHWHole,                 ///< A HW dram remap was created
  GsbNodeIntlv,              ///< Node Memory interleaving was enabled
  GsbSpIntRemapHole,         ///< Special condition for Node Interleave and HW remapping
  GsbEnDIMMSpareNW,          ///< Indicates that DIMM Spare can be used without a warm reset

  GsbEOL                     ///< End of list
} GLOBAL_STATUS_FIELD;

///
///   Local Error Status (DIE_STRUCT.ErrStatus[31:0])
///
typedef enum {
  EsbNoDimms,                  ///< No DIMMs
  EsbSpdChkSum,                ///< SPD Checksum fail
  EsbDimmMismatchM,            ///< dimm module type(buffer) mismatch
  EsbDimmMismatchT,            ///< dimm CL/T mismatch
  EsbDimmMismatchO,            ///< dimm organization mismatch (128-bit)
  EsbNoTrcTrfc,                ///< SPD missing Trc or Trfc info
  EsbNoCycTime,                ///< SPD missing byte 23 or 25
  EsbBkIntDis,                 ///< Bank interleave requested but not enabled
  EsbDramECCDis,               ///< Dram ECC requested but not enabled
  EsbSpareDis,                 ///< Online spare requested but not enabled
  EsbMinimumMode,              ///< Running in Minimum Mode
  EsbNoRcvrEn,                 ///< No DQS Receiver Enable pass window found
  EsbSmallRcvr,                ///< DQS Rcvr En pass window too small (far right of dynamic range)
  EsbNoDqsPos,                 ///< No DQS-DQ passing positions
  EsbSmallDqs,                 ///< DQS-DQ passing window too small
  EsbDCBKScrubDis,             ///< DCache scrub requested but not enabled

  EsbEMPNotSupported,          ///< Processor is not capable for EMP.
  EsbEMPConflict,               ///< EMP requested but cannot be enabled since
                               ///< channel interleaving, bank interleaving, or bank swizzle is enabled.
  EsbEMPDis,                   ///< EMP requested but cannot be enabled since
                               ///< memory size of each DCT is not a power of two.

  EsbEOL                       ///< End of list
} ERROR_STATUS_FIELD;

///
///  Local Configuration Status (DIE_STRUCT.Status[31:0])
///
typedef enum {
  SbRegistered,                ///< All DIMMs are Registered
  SbEccDimms,                  ///< All banks ECC capable
  SbParDimms,                  ///< All banks Addr/CMD Parity capable
  SbDiagClks,                  ///< Jedec ALL slots clock enable diag mode
  Sb128bitmode,                ///< DCT in 128-bit mode operation
  Sb64MuxedMode,               ///< DCT in 64-bit mux'ed mode.
  Sb2TMode,                    ///< 2T CMD timing mode is enabled.
  SbSWNodeHole,                ///< Remapping of Node Base on this Node to create a gap.
  SbHWHole,                    ///< Memory Hole created on this Node using HW remapping.
  SbOver400Mhz,                ///< DCT freq greater than or equal to 400MHz flag
  SbDQSPosPass2,               ///< Used for TrainDQSPos DIMM0/1, when freq greater than or equal to 400MHz
  SbDQSRcvLimit,               ///< Used for DQSRcvEnTrain to know we have reached the upper bound.
  SbExtConfig,                 ///< Indicate the default setting for extended PCI configuration support
  SbLrdimms,                   ///< All DIMMs are LRDIMMs

  SbEOL                        ///< End of list
} LOCAL_STATUS_FIELD;


///< CPU MSR Register definitions ------------------------------------------
#define SYS_CFG     0xC0010010ul
#define TOP_MEM     0xC001001Aul
#define TOP_MEM2    0xC001001Dul
#define HWCR        0xC0010015ul
#define NB_CFG      0xC001001Ful

#define FS_BASE     0xC0000100ul
#define IORR0_BASE  0xC0010016ul
#define IORR0_MASK  0xC0010017ul
#define BU_CFG      0xC0011023ul
#define BU_CFG2     0xC001102Aul
#define COFVID_STAT 0xC0010071ul
#define TSC         0x10

//-----------------------------------------------------------------------------
///
/// SPD Data for each DIMM.
///
#define DDR3_SPD_SIZE 256
typedef struct _SPD_DEF_STRUCT {
  IN BOOLEAN DimmPresent;       ///< Indicates that the DIMM is present and Data is valid
  IN UINT8 Data[DDR3_SPD_SIZE]; ///< Buffer for 256 Bytes of SPD data from DIMM
} SPD_DEF_STRUCT;

///
/// Channel Definition Structure.
/// This data structure defines entries that are specific to the channel initialization
///
typedef struct _CH_DEF_STRUCT {
  OUT UINT8   ChannelID;         ///< Physical channel ID of a socket(0 = CH A, 1 = CH B, 2 = CH C, 3 = CH D)
  OUT TECHNOLOGY_TYPE TechType;  ///< Technology type of this channel
  OUT UINT8   ChDimmPresent;     ///< For each bit n 0..7, 1 = DIMM n is present.
                                 ///<  DIMM#  Select Signal
                                 ///<  0      MA0_CS_L[0, 1]
                                 ///<  1      MB0_CS_L[0, 1]
                                 ///<  2      MA1_CS_L[0, 1]
                                 ///<  3      MB1_CS_L[0, 1]
                                 ///<  4      MA2_CS_L[0, 1]
                                 ///<  5      MB2_CS_L[0, 1]
                                 ///<  6      MA3_CS_L[0, 1]
                                 ///<  7      MB3_CS_L[0, 1]

  OUT struct _DCT_STRUCT *DCTPtr; ///< Pointer to the DCT data of this channel.
  OUT struct _DIE_STRUCT *MCTPtr; ///< Pointer to the node data of this channel.
  OUT SPD_DEF_STRUCT *SpdPtr;    ///< Pointer to the SPD data for this channel. (Setup by NB Constructor)
  OUT SPD_DEF_STRUCT *DimmSpdPtr[MAX_DIMMS_PER_CHANNEL]; ///< Array of pointers to
                                 ///<   SPD Data for each Dimm. (Setup by Tech Block Constructor)
  OUT UINT8   ChDimmValid;       ///< For each bit n 0..3, 1 = DIMM n is valid and is/will be configured where 4..7 are reserved.
                                 ///<
  OUT UINT8   RegDimmPresent;    ///< For each bit n 0..3, 1 = DIMM n is a registered DIMM where 4..7 are reserved.
  OUT UINT8   LrDimmPresent;     ///< For each bit n 0..3, 1 = DIMM n is Load Reduced DIMM where 4..7 are reserved.
  OUT UINT8   SODimmPresent;     ///< For each bit n 0..3, 1 = DIMM n is a SO-DIMM, where 4..7 are reserved.
  OUT UINT8   Loads;             ///< Number of devices loading bus
  OUT UINT8   Dimms;             ///< Number of DIMMs loading Channel
  OUT UINT8   Ranks;             ///< Number of ranks loading Channel DATA
  OUT BOOLEAN SlowMode;          ///< 1T or 2T CMD mode (slow access mode)
                                 ///< FALSE = 1T
                                 ///< TRUE = 2T
  ///< The following pointers will be pointed to dynamically allocated buffers.
  ///< Each buffer is two dimensional (RowCount x ColumnCount) and is lay-outed as in below.
  ///< Example: If DIMM and Byte based training, then
  ///< XX is a value in Hex
  ///<                        BYTE 0, BYTE 1, BYTE 2, BYTE 3, BYTE 4, BYTE 5, BYTE 6, BYTE 7, ECC BYTE
  ///<  Row1 -  Logical DIMM0    XX      XX      XX      XX      XX      XX      XX      XX      XX
  ///<  Row2 -  Logical DIMM1    XX      XX      XX      XX      XX      XX      XX      XX      XX
  OUT UINT16  *RcvEnDlys;       ///< DQS Receiver Enable Delays
  OUT UINT8   *WrDqsDlys;       ///< Write DQS delays (only valid for DDR3)
  OUT UINT8   *RdDqsDlys;       ///< Read Dqs delays
  OUT UINT8   *WrDatDlys;       ///< Write Data delays
  OUT UINT8   *RdDqs2dDlys;     ///< 2d Read DQS data
  OUT UINT8   *RdDqsMinDlys;    ///< Minimum Window for Read DQS
  OUT UINT8   *RdDqsMaxDlys;    ///< Maximum Window for Read DQS
  OUT UINT8   *WrDatMinDlys;    ///< Minimum Window for Write data
  OUT UINT8   *WrDatMaxDlys;    ///< Maximum Window for Write data
  OUT UINT16  *RcvEnDlysMemPs1;       ///< DQS Receiver Enable Delays for Mem Pstate 1
  OUT UINT8   *WrDqsDlysMemPs1;       ///< Write DQS delays (only valid for DDR3) for Mem Pstate 1
  OUT UINT8   *RdDqsDlysMemPs1;       ///< Read Dqs delays for Memory Pstate 1
  OUT UINT8   *WrDatDlysMemPs1;       ///< Write Data delays for Memory Pstate 1
  OUT UINT8   *RdDqs2dDlysMemPs1;     ///< 2d Read DQS data for Memory Pstate 1
  OUT UINT8   *RdDqsMinDlysMemPs1;    ///< Minimum Window for Read DQS for Memory Pstate 1
  OUT UINT8   *RdDqsMaxDlysMemPs1;    ///< Maximum Window for Read DQS for Memory Pstate 1
  OUT UINT8   *WrDatMinDlysMemPs1;    ///< Minimum Window for Write data for Memory Pstate 1
  OUT UINT8   *WrDatMaxDlysMemPs1;    ///< Maximum Window for Write data for Memory Pstate 1
  OUT UINT8   RowCount;         ///< Number of rows of the allocated buffer.
  OUT UINT8   ColumnCount;      ///< Number of columns of the allocated buffer.
  OUT UINT8   *FailingBitMask;    ///< Table of masks to Track Failing bits
  OUT UINT8   *FailingBitMaskMemPs1;    ///< Table of masks to Track Failing bits for Memory Pstate 1
  OUT UINT32  DctOdcCtl;          ///< Output Driver Strength (see BKDG FN2:Offset 9Ch, index 00h)
  OUT UINT32  DctAddrTmg;         ///< Address Bus Timing (see BKDG FN2:Offset 9Ch, index 04h)
  OUT UINT32  PhyRODTCSLow;       ///< Phy Read ODT Pattern Chip Select low (see BKDG FN2:Offset 9Ch, index 180h)
  OUT UINT32  PhyRODTCSHigh;      ///< Phy Read ODT Pattern Chip Select high (see BKDG FN2:Offset 9Ch, index 181h)
  OUT UINT32  PhyWODTCSLow;       ///< Phy Write ODT Pattern Chip Select low (see BKDG FN2:Offset 9Ch, index 182h)
  OUT UINT32  PhyWODTCSHigh;      ///< Phy Write ODT Pattern Chip Select high (see BKDG FN2:Offset 9Ch, index 183)
  OUT UINT8   PhyWLODT[4];        ///< Write Levelization ODT Pattern for Dimm 0-3 or CS 0-7(see BKDG FN2:Offset 9Ch, index 0x8[11:8])
  OUT UINT16  DctEccDqsLike;      ///< DCT DQS ECC UINT8 like...
  OUT UINT8   DctEccDqsScale;     ///< DCT DQS ECC UINT8 scale
  OUT UINT16  PtrPatternBufA;     ///< Ptr on stack to aligned DQS testing pattern
  OUT UINT16  PtrPatternBufB;     ///< Ptr on stack to aligned DQS testing pattern
  OUT UINT8   ByteLane;           ///< Current UINT8 Lane (0..7)
  OUT UINT8   Direction;          ///< Current DQS-DQ training write direction (0=read, 1=write)
  OUT UINT8   Pattern;            ///< Current pattern
  OUT UINT8   DqsDelay;           ///< Current DQS delay value
  OUT UINT16  HostBiosSrvc1;      ///< UINT16 sized general purpose field for use by host BIOS.  Scratch space.
  OUT UINT32  HostBiosSrvc2;      ///< UINT32 sized general purpose field for use by host BIOS.  Scratch space.
  OUT UINT16  DctMaxRdLat[4];     ///< Max Read Latency (ns) for the DCT
                                  ///< DctMaxRdLat [i] is for NBPstate i
  OUT UINT8   DIMMValidCh;        ///< DIMM# in CH
  OUT UINT8   MaxCh;              ///< Max number of CH in system
  OUT UINT8   Dct;                ///< Dct pointer
  OUT UINT8   WrDatGrossH;        ///< Write Data Gross delay high value
  OUT UINT8   DqsRcvEnGrossL;     ///< DQS Receive Enable Gross Delay low

  OUT UINT8   TrwtWB;             ///<  Non-SPD timing value for TrwtWB
  OUT UINT8   CurrRcvrDctADelay;  ///< for keep current RcvrEnDly
  OUT UINT16  T1000;              ///< get the T1000 figure (cycle time (ns) * 1K)
  OUT UINT8   DqsRcvEnPass;       ///< for TrainRcvrEn UINT8 lane pass flag
  OUT UINT8   DqsRcvEnSaved;      ///< for TrainRcvrEn UINT8 lane saved flag
  OUT UINT8   SeedPass1Remainder; ///< for Phy assisted DQS receiver enable training

  OUT UINT8   ClToNbFlag;         ///< is used to restore ClLinesToNbDis bit after memory
  OUT UINT32  NodeSysBase;        ///< for channel interleave usage
  OUT UINT8   RefRawCard[MAX_DIMMS_PER_CHANNEL];   ///< Array of rawcards detected
  OUT UINT8   CtrlWrd02[MAX_DIMMS_PER_CHANNEL];    ///< Control Word 2 values per DIMM
  OUT UINT8   CtrlWrd03[MAX_DIMMS_PER_CHANNEL];    ///< Control Word 3 values per DIMM
  OUT UINT8   CtrlWrd04[MAX_DIMMS_PER_CHANNEL];    ///< Control Word 4 values per DIMM
  OUT UINT8   CtrlWrd05[MAX_DIMMS_PER_CHANNEL];    ///< Control Word 5 values per DIMM
  OUT UINT8   CtrlWrd08[MAX_DIMMS_PER_CHANNEL];    ///< Control Word 8 values per DIMM

  OUT UINT16  CsPresentDCT;       ///< For each bit n 0..7, 1 = Chip-select n is present
  OUT UINT8   DimmMirrorPresent;  ///< For each bit n 0..3, 1 = DIMM n is OnDimmMirror capable where 4..7 are reserved.
  OUT UINT8   DimmSpdCse;         ///< For each bit n 0..3, 1 = DIMM n SPD checksum error where 4..7 are reserved.
  OUT UINT8   DimmExclude;        ///< For each bit n 0..3, 1 = DIMM n gets excluded where 4..7 are reserved.
  OUT UINT8   DimmYr06;           ///< Bitmap indicating which Dimms have a manufacturer's year code <= 2006
  OUT UINT8   DimmWk2406;         ///< Bitmap indicating which Dimms have a manufacturer's week code <= 24 of 2006 (June)
  OUT UINT8   DimmPlPresent;      ///< Bitmap indicating that Planar (1) or Stacked (0) Dimms are present.
  OUT UINT8   DimmQrPresent;      ///< QuadRank DIMM present?
  OUT UINT8   DimmDrPresent;      ///< Bitmap indicating that Dual Rank Dimms are present
  OUT UINT8   DimmSRPresent;      ///< Bitmap indicating that Single Rank Dimms are present
  OUT UINT8   Dimmx4Present;      ///< For each bit n 0..3, 1 = DIMM n contains x4 data devices. where 4..7 are reserved.
  OUT UINT8   Dimmx8Present;      ///< For each bit n 0..3, 1 = DIMM n contains x8 data devices. where 4..7 are reserved.
  OUT UINT8   Dimmx16Present;     ///< For each bit n 0..3, 1 = DIMM n contains x16 data devices. where 4..7 are reserved.
  OUT UINT8   LrdimmPhysicalRanks[MAX_DIMMS_PER_CHANNEL];///< Number of Physical Ranks for LRDIMMs
  OUT UINT8   LrDimmLogicalRanks[MAX_DIMMS_PER_CHANNEL];///< Number of LRDIMM Logical ranks in this configuration
  OUT UINT8   LrDimmRankMult[MAX_DIMMS_PER_CHANNEL];///< Rank Multipication factor per dimm.
  OUT UINT8   DimmNibbleAccess;   ///< For each bit n 0..3, 1 = DIMM n will use nibble signaling. Where 4..7 are reserved.
  OUT UINT8   *MemClkDisMap;      ///<  This pointer will be set to point to an array that describes
                                  ///<  the routing of M[B,A]_CLK pins to the DIMMs' ranks. AGESA will
                                  ///<  base on this array to disable unused MemClk to save power.
                                  ///<
                                  ///<  The array must have 8 entries. Each entry, which associates with
                                  ///<  one MemClkDis bit, is a bitmap of 8 CS that that MemClk is routed to.
                                  ///<    Example:
                                  ///<    BKDG definition of Fn2x88[MemClkDis] bitmap for AM3 package
                                  ///<    is like below:
                                  ///<         Bit AM3/S1g3 pin name
                                  ///<         0   M[B,A]_CLK_H/L[0]
                                  ///<         1   M[B,A]_CLK_H/L[1]
                                  ///<         2   M[B,A]_CLK_H/L[2]
                                  ///<         3   M[B,A]_CLK_H/L[3]
                                  ///<         4   M[B,A]_CLK_H/L[4]
                                  ///<         5   M[B,A]_CLK_H/L[5]
                                  ///<         6   M[B,A]_CLK_H/L[6]
                                  ///<         7   M[B,A]_CLK_H/L[7]
                                  ///<    And platform has the following routing:
                                  ///<         CS0   M[B,A]_CLK_H/L[4]
                                  ///<         CS1   M[B,A]_CLK_H/L[2]
                                  ///<         CS2   M[B,A]_CLK_H/L[3]
                                  ///<         CS3   M[B,A]_CLK_H/L[5]
                                  ///<    Then MemClkDisMap should be pointed to the following array:
                                  ///<               CLK_2 CLK_3 CLK_4 CLK_5
                                  ///<    0x00, 0x00, 0x02, 0x04, 0x01, 0x08, 0x00, 0x00
                                  ///<  Each entry of the array is the bitmask of 8 chip selects.

  OUT UINT8   *CKETriMap;         ///<  This pointer will be set to point to an array that describes
                                  ///<  the routing of CKE pins to the DIMMs' ranks.
                                  ///<  The array must have 2 entries. Each entry, which associates with
                                  ///<  one CKE pin, is a bitmap of 8 CS that that CKE is routed to.
                                  ///<  AGESA will base on this array to disable unused CKE pins to save power.

  OUT UINT8   *ODTTriMap;         ///<  This pointer will be set to point to an array that describes
                                  ///<  the routing of ODT pins to the DIMMs' ranks.
                                  ///<  The array must have 4 entries. Each entry, which associates with
                                  ///<  one ODT pin, is a bitmap of 8 CS that that ODT is routed to.
                                  ///<  AGESA will base on this array to disable unused ODT pins to save power.

  OUT UINT8   *ChipSelTriMap;     ///<  This pointer will be set to point to an array that describes
                                  ///<  the routing of chip select pins to the DIMMs' ranks.
                                  ///<  The array must have 8 entries. Each entry is a bitmap of 8 CS.
                                  ///<  AGESA will base on this array to disable unused Chip select pins to save power.

  OUT BOOLEAN   ExtendTmp;        ///<  If extended temperature is supported on all dimms on a channel.

  OUT UINT8   MaxVref;            ///<  Maximum Vref Value for channel

  OUT UINT8   Reserved[100];      ///< Reserved
} CH_DEF_STRUCT;

///
/// DCT Channel Timing Parameters.
/// This data structure sets timings that are specific to the channel.
///
typedef struct _CH_TIMING_STRUCT {
  OUT UINT16  DctDimmValid;       ///< For each bit n 0..3, 1=DIMM n is valid and is/will be configured where 4..7 are reserved.
  OUT UINT16  DimmMirrorPresent;  ///< For each bit n 0..3, 1=DIMM n is OnDimmMirror capable where 4..7 are reserved.
  OUT UINT16  DimmSpdCse;         ///< For each bit n 0..3, 1=DIMM n SPD checksum error where 4..7 are reserved.
  OUT UINT16  DimmExclude;        ///< For each bit n 0..3, 1 = DIMM n gets excluded where 4..7 are reserved.
  OUT UINT16  CsPresent;          ///< For each bit n 0..7, 1=Chip-select n is present
  OUT UINT16  CsEnabled;          ///< For each bit n 0..7, 1=Chip-select n is enabled
  OUT UINT16  CsTestFail;         ///< For each bit n 0..7, 1=Chip-select n is present but disabled
  OUT UINT16  CsTrainFail;        ///< Bitmap showing which chipselects failed training
  OUT UINT16  DIMM1KPage;         ///< For each bit n 0..3, 1=DIMM n contains 1K page devices. where 4..7 are reserved
  OUT UINT16  DimmQrPresent;      ///< QuadRank DIMM present?
  OUT UINT16  DimmDrPresent;      ///< Bitmap indicating that Dual Rank Dimms are present , where 4..7 are reserved
  OUT UINT8   DimmSRPresent;      ///< Bitmap indicating that Single Rank Dimms are present, where 4..7 are reserved
  OUT UINT16  Dimmx4Present;      ///< For each bit n 0..3, 1=DIMM n contains x4 data devices. where 4..7 are reserved
  OUT UINT16  Dimmx8Present;      ///< For each bit n 0..3, 1=DIMM n contains x8 data devices. where 4..7 are reserved
  OUT UINT16  Dimmx16Present;     ///< For each bit n 0..3, 1=DIMM n contains x16 data devices. where 4..7 are reserved

  OUT UINT16  DIMMTrcd;           ///< Minimax Trcd*40 (ns) of DIMMs
  OUT UINT16  DIMMTrp;            ///< Minimax Trp*40 (ns) of DIMMs
  OUT UINT16  DIMMTrtp;           ///< Minimax Trtp*40 (ns) of DIMMs
  OUT UINT16  DIMMTras;           ///< Minimax Tras*40 (ns) of DIMMs
  OUT UINT16  DIMMTrc;            ///< Minimax Trc*40 (ns) of DIMMs
  OUT UINT16  DIMMTwr;            ///< Minimax Twr*40 (ns) of DIMMs
  OUT UINT16  DIMMTrrd;           ///< Minimax Trrd*40 (ns) of DIMMs
  OUT UINT16  DIMMTwtr;           ///< Minimax Twtr*40 (ns) of DIMMs
  OUT UINT16  DIMMTfaw;           ///< Minimax Tfaw*40 (ns) of DIMMs
  OUT UINT16  TargetSpeed;        ///< Target DRAM bus speed in MHz
  OUT UINT16  Speed;              ///< DRAM bus speed in MHz
                                  ///<  400 (MHz)
                                  ///<  533 (MHz)
                                  ///<  667 (MHz)
                                  ///<  800 (MHz)
                                  ///<  and so on...
  OUT UINT8   CasL;               ///< CAS latency DCT setting (busclocks)
  OUT UINT8   Trcd;               ///< DCT Trcd (busclocks)
  OUT UINT8   Trp;                ///< DCT Trp (busclocks)
  OUT UINT8   Trtp;               ///< DCT Trtp (busclocks)
  OUT UINT8   Tras;               ///< DCT Tras (busclocks)
  OUT UINT8   Trc;                ///< DCT Trc (busclocks)
  OUT UINT8   Twr;                ///< DCT Twr (busclocks)
  OUT UINT8   Trrd;               ///< DCT Trrd (busclocks)
  OUT UINT8   Twtr;               ///< DCT Twtr (busclocks)
  OUT UINT8   Tfaw;               ///< DCT Tfaw (busclocks)
  OUT UINT8   Trfc0;              ///< DCT Logical DIMM0 Trfc
                                  ///<  0 = 75ns (for 256Mb devs)
                                  ///<  1 = 105ns (for 512Mb devs)
                                  ///<  2 = 127.5ns (for 1Gb devs)
                                  ///<  3 = 195ns (for 2Gb devs)
                                  ///<  4 = 327.5ns (for 4Gb devs)
  OUT UINT8   Trfc1;              ///< DCT Logical DIMM1 Trfc (see Trfc0 for format)
  OUT UINT8   Trfc2;              ///< DCT Logical DIMM2 Trfc (see Trfc0 for format)
  OUT UINT8   Trfc3;              ///< DCT Logical DIMM3 Trfc (see Trfc0 for format)
  OUT UINT32  DctMemSize;         ///< Base[47:16], total DRAM size controlled by this DCT.
                                  ///<
  OUT BOOLEAN SlowMode;           ///< 1T or 2T CMD mode (slow access mode)
                                  ///< FALSE = 1T
                                  ///< TRUE = 2T
  OUT UINT8   TrwtTO;             ///< DCT TrwtTO (busclocks)
  OUT UINT8   Twrrd;              ///< DCT Twrrd (busclocks)
  OUT UINT8   Twrwr;              ///< DCT Twrwr (busclocks)
  OUT UINT8   Trdrd;              ///< DCT Trdrd (busclocks)
  OUT UINT8   TrwtWB;             ///< DCT TrwtWB (busclocks)
  OUT UINT8   TrdrdSD;            ///< DCT TrdrdSD (busclocks)
  OUT UINT8   TwrwrSD;            ///< DCT TwrwrSD (busclocks)
  OUT UINT8   TwrrdSD;            ///< DCT TwrrdSD (busclocks)
  OUT UINT16  MaxRdLat;           ///< Max Read Latency
  OUT UINT8   WrDatGrossH;        ///< Temporary variables must be removed
  OUT UINT8   DqsRcvEnGrossL;     ///< Temporary variables must be removed
} CH_TIMING_STRUCT;

///
/// Data for each DCT.
/// This data structure defines data used to configure each DRAM controller.
///
typedef struct _DCT_STRUCT {
  OUT UINT8   Dct;                ///< Current Dct
  OUT CH_TIMING_STRUCT Timings;   ///< Channel Timing structure
  OUT CH_TIMING_STRUCT *TimingsMemPs1;   ///< Pointed to channel timing structure for memory Pstate 1
  OUT CH_DEF_STRUCT    *ChData;   ///< Pointed to a dynamically allocated array of Channel structures
  OUT UINT8   ChannelCount;       ///< Number of channel per this DCT
  OUT BOOLEAN BkIntDis;           ///< Bank interleave requested but not enabled on current DCT
} DCT_STRUCT;


///
/// Data Structure defining each Die.
/// This data structure contains information that is used to configure each Die.
///
typedef struct _DIE_STRUCT {

  /// Advanced:

  OUT UINT8   NodeId;              ///< Node ID of current controller
  OUT UINT8   SocketId;            ///< Socket ID of this Die
  OUT UINT8   DieId;               ///< ID of this die relative to the socket
  OUT PCI_ADDR      PciAddr;       ///< Pci bus and device number of this controller.
  OUT AGESA_STATUS  ErrCode;       ///< Current error condition of Node
                                   ///<  0x0 = AGESA_SUCCESS
                                   ///<  0x1 = AGESA_UNSUPPORTED
                                   ///<  0x2 = AGESA_BOUNDS_CHK
                                   ///<  0x3 = AGESA_ALERT
                                   ///<  0x4 = AGESA_WARNING
                                   ///<  0x5 = AGESA_ERROR
                                   ///<  0x6 = AGESA_CRITICAL
                                   ///<  0x7 = AGESA_FATAL
                                   ///<
  OUT BOOLEAN ErrStatus[EsbEOL];   ///< Error Status bit Field
                                   ///<
  OUT BOOLEAN Status[SbEOL];       ///< Status bit Field
                                   ///<
  OUT UINT32  NodeMemSize;         ///< Base[47:16], total DRAM size controlled by both DCT0 and DCT1 of this Node.
                                   ///<
  OUT UINT32  NodeSysBase;         ///< Base[47:16] (system address) DRAM base address of this Node.
                                   ///<
  OUT UINT32  NodeHoleBase;        ///< If not zero, Base[47:16] (system address) of dram hole for HW remapping.  Dram hole exists on this Node
                                   ///<
  OUT UINT32  NodeSysLimit;        ///< Base[47:16] (system address) DRAM limit address of this Node.
                                   ///<
  OUT UINT32  DimmPresent;         ///< For each bit n 0..7, 1 = DIMM n is present.
                                   ///<   DIMM#  Select Signal
                                   ///<   0      MA0_CS_L[0, 1]
                                   ///<   1      MB0_CS_L[0, 1]
                                   ///<   2      MA1_CS_L[0, 1]
                                   ///<   3      MB1_CS_L[0, 1]
                                   ///<   4      MA2_CS_L[0, 1]
                                   ///<   5      MB2_CS_L[0, 1]
                                   ///<   6      MA3_CS_L[0, 1]
                                   ///<   7      MB3_CS_L[0, 1]
                                   ///<
  OUT UINT32  DimmValid;           ///< For each bit n 0..7, 1 = DIMM n is valid and is / will be configured
  OUT UINT32  RegDimmPresent;      ///< For each bit n 0..7, 1 = DIMM n is registered DIMM
  OUT UINT32  LrDimmPresent;       ///< For each bit n 0..7, 1 = DIMM n is Load Reduced DIMM
  OUT UINT32  DimmEccPresent;      ///< For each bit n 0..7, 1 = DIMM n is ECC capable.
  OUT UINT32  DimmParPresent;      ///< For each bit n 0..7, 1 = DIMM n is ADR/CMD Parity capable.
                                   ///<
  OUT UINT16  DimmTrainFail;       ///< Bitmap showing which dimms failed training
  OUT UINT16  ChannelTrainFail;    ///< Bitmap showing the channel information about failed Chip Selects
                                   ///<  0 in any bit field indicates Channel 0
                                   ///<  1 in any bit field indicates Channel 1
  OUT UINT8   Dct;                 ///<  Need to be removed
                                   ///<  DCT pointer
  OUT BOOLEAN GangedMode;          ///< Ganged mode
                                   ///<  0 = disabled
                                   ///<  1 = enabled
  OUT CPU_LOGICAL_ID LogicalCpuid; ///< The logical CPUID of the node
                                   ///<
  OUT UINT16  HostBiosSrvc1;       ///< UINT16 sized general purpose field for use by host BIOS.  Scratch space.
                                   ///<
  OUT UINT32  HostBiosSrvc2;       ///< UINT32 sized general purpose field for use by host BIOS.  Scratch space.
                                   ///<
  OUT UINT8   MLoad;               ///< Need to be removed
                                   ///< Number of devices loading MAA bus
                                   ///<
  OUT UINT8   MaxAsyncLat;         ///< Legacy wrapper
                                   ///<
  OUT UINT8   ChbD3Rcvrdly;        ///< Legacy wrapper
                                   ///<
  OUT UINT16  ChaMaxRdLat;         ///< Max Read Latency (ns) for DCT 0
                                   ///<
  OUT UINT8   ChbD3BcRcvrdly;      ///< CHB DIMM 3 Check UINT8 Receiver Enable Delay

  OUT DCT_STRUCT *DctData;         ///< Pointed to a dynamically allocated array of DCT_STRUCTs
  OUT UINT8   DctCount;            ///< Number of DCTs per this Die
  OUT UINT8   Reserved[16];        ///< Reserved
} DIE_STRUCT;

/**********************************************************************
 * S3 Support structure
 **********************************************************************/
/// AmdInitResume, AmdS3LateRestore, and AmdS3Save param structure
typedef struct {
     OUT   UINT32 Signature;           ///< "ASTR" for AMD Suspend-To-RAM
     OUT   UINT16 Version;             ///< S3 Params version number
  IN OUT   UINT32 Flags;               ///< Indicates operation
  IN OUT   VOID   *NvStorage;          ///< Pointer to memory critical save state data
  IN OUT   UINT32 NvStorageSize;       ///< Size in bytes of the NvStorage region
  IN OUT   VOID   *VolatileStorage;    ///< Pointer to remaining AMD save state data
  IN OUT   UINT32 VolatileStorageSize; ///< Size in bytes of the VolatileStorage region
} AMD_S3_PARAMS;

///===============================================================================
/// MEM_PARAMETER_STRUCT
/// This data structure is used to pass wrapper parameters to the memory configuration code
///
typedef struct _MEM_PARAMETER_STRUCT {

  // Basic (Return parameters)
  // (This section contains the outbound parameters from the memory init code)

  OUT BOOLEAN GStatus[GsbEOL];    ///< Global Status bitfield.
                                  ///<
  OUT UINT32 HoleBase;            ///< If not zero Base[47:16] (system address) of sub 4GB dram hole for HW remapping.
                                  ///<
  OUT UINT32 Sub4GCacheTop;       ///< If not zero, the 32-bit top of cacheable memory.
                                  ///<
  OUT UINT32 Sub1THoleBase;       ///< If not zero Base[47:16] (system address) of sub 1TB dram hole.
                                  ///<
  OUT UINT32 SysLimit;            ///< Limit[47:16] (system address).
                                  ///<
  OUT DIMM_VOLTAGE DDR3Voltage;   ///< Find support voltage and send back to platform BIOS.
                                  ///<
  OUT UINT8 ExternalVrefValue;    ///< Target reference voltage for external Vref for 2D training
                                  ///<
  OUT struct _MEM_DATA_STRUCT *MemData;   ///< Access to global memory init data.

  //  Advanced (Optional parameters)
  //  Optional (all defaults values will be initialized by the
  //  'AmdMemInitDataStructDef' based on AMD defaults. It is up
  //  to the IBV/OEM to change the defaults after initialization
  //  but prior to the main entry to the memory code):

  // Memory Map/Mgt.

  IN UINT16  BottomIo;             ///< Bottom of 32-bit IO space (8-bits).
                                   ///<   NV_BOTTOM_IO[7:0]=Addr[31:24]
                                   ///<
  IN BOOLEAN MemHoleRemapping;     ///< Memory Hole Remapping (1-bit).
                                   ///<  FALSE = disable
                                   ///<  TRUE  = enable
                                   ///<
  IN BOOLEAN LimitMemoryToBelow1Tb;///< Limit memory address space to below 1 TB
                                   ///<  FALSE = disable
                                   ///<  TRUE  = enable
                                   ///<
                                   ///< @BldCfgItem{BLDCFG_LIMIT_MEMORY_TO_BELOW_1TB}


  // Dram Timing

  IN USER_MEMORY_TIMING_MODE   UserTimingMode;  ///< User Memclock Mode.
                                                ///< @BldCfgItem{BLDCFG_TIMING_MODE_SELECT}

  IN MEMORY_BUS_SPEED          MemClockValue;   ///< Memory Clock Value.
                                                ///< @BldCfgItem{BLDCFG_MEMORY_CLOCK_SELECT}


  // Dram Configuration

  IN BOOLEAN EnableBankIntlv;      ///< Dram Bank (chip-select) Interleaving (1-bit).
                                   ///<  - FALSE =disable (default)
                                   ///<  - TRUE = enable
                                   ///<
                                   ///< @BldCfgItem{BLDCFG_MEMORY_ENABLE_BANK_INTERLEAVING}

  IN BOOLEAN EnableNodeIntlv;      ///< Node Memory Interleaving (1-bit).
                                   ///<   - FALSE = disable (default)
                                   ///<   - TRUE = enable
                                   ///<
                                   ///< @BldCfgItem{BLDCFG_MEMORY_ENABLE_NODE_INTERLEAVING}

  IN BOOLEAN EnableChannelIntlv;   ///< Channel Interleaving (1-bit).
                                   ///<   - FALSE = disable (default)
                                   ///<   - TRUE = enable
                                   ///<
                                   ///< @BldCfgItem{BLDCFG_MEMORY_CHANNEL_INTERLEAVING}
  // ECC

  IN BOOLEAN EnableEccFeature;     ///< enable ECC error to go into MCE.
                                   ///<   - FALSE = disable (default)
                                   ///<   - TRUE = enable
                                   ///<
                                   ///< @BldCfgItem{BLDCFG_ENABLE_ECC_FEATURE}
  // Dram Power

  IN BOOLEAN EnablePowerDown;      ///< CKE based power down mode (1-bit).
                                   ///<   - FALSE =disable (default)
                                   ///<   - TRUE =enable
                                   ///<
                                   ///< @BldCfgItem{BLDCFG_MEMORY_POWER_DOWN}

  // Online Spare

  IN BOOLEAN EnableOnLineSpareCtl; ///< Chip Select Spare Control bit 0.
                                   ///<  - FALSE = disable Spare (default)
                                   ///<  - TRUE = enable Spare
                                   ///<
                                   ///< @BldCfgItem{BLDCFG_ONLINE_SPARE}

  IN UINT8 *TableBasedAlterations; ///< Desired modifications to register settings.

  IN PSO_TABLE *PlatformMemoryConfiguration;
                                   ///< A table that contains platform specific settings.
                                   ///< For example, MemClk routing, the number of DIMM slots per channel, ....
                                   ///< AGESA initializes this pointer with DefaultPlatformMemoryConfiguration that
                                   ///< contains default conservative settings. Platform BIOS can either tweak
                                   ///< DefaultPlatformMemoryConfiguration or reassign this pointer to its own table.
                                   ///<
  IN BOOLEAN EnableParity;         ///< Parity control.
                                   ///<  - TRUE = enable
                                   ///<  - FALSE = disable (default)
                                   ///<
                                   ///< @BldCfgItem{BLDCFG_MEMORY_PARITY_ENABLE}

  IN BOOLEAN EnableBankSwizzle;    ///< BankSwizzle control.
                                   ///<  - FALSE = disable
                                   ///<  - TRUE = enable  (default)
                                   ///<
                                   ///< @BldCfgItem{BLDCFG_BANK_SWIZZLE}

                                   ///<

  IN BOOLEAN EnableMemClr;         ///< Memory Clear functionality control.
                                   ///<  - FALSE = disable
                                   ///<  - TRUE = enable  (default)
                                   ///<

  // Uma Configuration

  IN UMA_MODE UmaMode;             ///<  Uma Mode
                                   ///<  0 = None
                                   ///<  1 = Specified
                                   ///<  2 = Auto
  IN OUT UINT32 UmaSize;           ///<  The size of shared graphics dram (16-bits)
                                   ///<  NV_UMA_Size[31:0]=Addr[47:16]
                                   ///<
  OUT UINT32 UmaBase;              ///<  The allocated Uma base address (32-bits)
                                   ///<  NV_UMA_Base[31:0]=Addr[47:16]
                                   ///<

  /// Memory Restore Feature

  IN BOOLEAN MemRestoreCtl;        ///< Memory context restore control
                                   ///<   FALSE = perform memory init as normal (AMD default)
                                   ///<   TRUE = restore memory context and skip training. This requires
                                   ///<          MemContext is valid before AmdInitPost
                                   ///<
  IN BOOLEAN SaveMemContextCtl;    ///< Control switch to save memory context at the end of MemAuto
                                   ///<   TRUE = AGESA will setup MemContext block before exit AmdInitPost
                                   ///<   FALSE = AGESA will not setup MemContext block. Platform is
                                   ///<           expected to call S3Save later in POST if it wants to
                                   ///<           use memory context restore feature.
                                   ///<
  IN OUT AMD_S3_PARAMS MemContext; ///< Memory context block describes the data that platform needs to
                                   ///< save and restore for memory context restore feature to work.
                                   ///< It uses the subset of S3Save block to save/restore. Hence platform
                                   ///< may save only S3 block and uses it for both S3 resume and
                                   ///< memory context restore.
                                   ///<  - If MemRestoreCtl is TRUE, platform needs to pass in MemContext
                                   ///<    before AmdInitPost.
                                   ///<  - If SaveMemContextCtl is TRUE, platform needs to save MemContext
                                   ///<    right after AmdInitPost.
                                   ///<
  IN BOOLEAN ExternalVrefCtl;      ///< Control the use of external Vref
                                   ///<   TRUE = AGESA will use the function defined in "AGESA_EXTERNAL_VREF_CHANGE" in function list
                                   ///<          to change the vref
                                   ///<   FALSE = AGESA will will use the internal vref control.
                                   ///< @BldCfgItem{BLDCFG_ENABLE_EXTERNAL_VREF_FEATURE}
                                   ///<
  IN FORCE_TRAIN_MODE ForceTrainMode;   ///<  Training Mode
                                   ///<  0 = Force 1D Training for all configurations
                                   ///<  2 = Auto - AGESA will control 1D or 2D
} MEM_PARAMETER_STRUCT;


///
/// Function definition.
/// This data structure passes function pointers to the memory configuration code.
/// The wrapper can use this structure with customized versions.
///
typedef struct _MEM_FUNCTION_STRUCT {

  // PUBLIC required Internal functions

  IN OUT BOOLEAN (*amdMemGetPsCfgU) ( VOID *pMemData);  ///< Proc for Unbuffered DIMMs, platform specific
  IN OUT BOOLEAN (*amdMemGetPsCfgR) (VOID *pMemData);   ///< Proc for Registered DIMMs, platform specific

  // PUBLIC optional functions

  IN OUT VOID (*amdMemEccInit) (VOID *pMemData);                  ///< NB proc for ECC feature
  IN OUT VOID (*amdMemChipSelectInterleaveInit) (VOID *pMemData); ///< NB proc for CS interleave feature
  IN OUT VOID (*amdMemDctInterleavingInit) (VOID *pMemData);      ///< NB proc for Channel interleave feature
  IN OUT VOID (*amdMemMctInterleavingInit) (VOID *pMemData);      ///< NB proc for Node interleave feature
  IN OUT VOID (*amdMemParallelTraining) (VOID *pMemData);         ///< NB proc for parallel training feature
  IN OUT VOID (*amdMemEarlySampleSupport) (VOID *pMemData);       ///< NB code for early sample support feature
  IN OUT VOID (*amdMemMultiPartInitSupport) (VOID *pMemData);     ///< NB code for 'multi-part'
  IN OUT VOID (*amdMemOnlineSpareSupport) (VOID *pMemData);       ///< NB code for On-Line Spare feature
  IN OUT VOID (*amdMemUDimmInit) (VOID *pMemData);                ///< NB code for UDIMMs
  IN OUT VOID (*amdMemRDimmInit) (VOID *pMemData);                ///< NB code for RDIMMs
  IN OUT VOID (*amdMemLrDimmInit) (VOID *pMemData);               ///< NB code for LRDIMMs
  IN OUT UINT32   Reserved[100]; ///< Reserved for later function definition
} MEM_FUNCTION_STRUCT;

///
/// Socket Structure
///
///
typedef struct _MEM_SOCKET_STRUCT {
  OUT VOID *ChannelPtr[MAX_CHANNELS_PER_SOCKET];  ///< Pointers to each channels training data

  OUT VOID *TimingsPtr[MAX_CHANNELS_PER_SOCKET];  ///< Pointers to each channels timing data
} MEM_SOCKET_STRUCT;

///
/// Contains all data relevant to Memory Initialization.
///
typedef struct _MEM_DATA_STRUCT {
  IN AMD_CONFIG_PARAMS StdHeader;             ///< Standard configuration header

  IN MEM_PARAMETER_STRUCT *ParameterListPtr;  ///< List of input Parameters

  OUT MEM_FUNCTION_STRUCT FunctionList;       ///< List of function Pointers

  IN OUT AGESA_STATUS (*GetPlatformCfg[MAX_PLATFORM_TYPES]) (struct _MEM_DATA_STRUCT *MemData, UINT8 SocketID, CH_DEF_STRUCT *CurrentChannel); ///< look-up platform info

  IN OUT BOOLEAN (*ErrorHandling)(struct _DIE_STRUCT *MCTPtr, UINT8 DCT, UINT16 ChipSelMask, AMD_CONFIG_PARAMS *StdHeader); ///< Error Handling


  OUT MEM_SOCKET_STRUCT SocketList[MAX_SOCKETS_SUPPORTED];  ///< Socket list for memory code.
                                   ///< SocketList is a shortcut for IBVs to retrieve training
                                   ///< and timing data for each channel indexed by socket/channel,
                                   ///< eliminating their need to parse die/dct/channel etc.
                                   ///< It contains pointers to the populated data structures for
                                   ///< each channel and skips the channel structures that are
                                   ///< unpopulated. In the case of channels sharing the same DCT,
                                   ///< the pTimings pointers will point to the same DCT Timing data.

  OUT DIE_STRUCT *DiesPerSystem;  ///< Pointed to an array of DIE_STRUCTs
  OUT UINT8      DieCount;        ///< Number of MCTs in the system.

  IN SPD_DEF_STRUCT *SpdDataStructure;              ///< Pointer to SPD Data structure

  IN OUT  struct _PLATFORM_CONFIGURATION   *PlatFormConfig;    ///< Platform profile/build option config structure

  IN OUT BOOLEAN IsFlowControlSupported;    ///< Indicates if flow control is supported

  OUT UINT32 TscRate;             ///< The rate at which the TSC increments in megahertz.

} MEM_DATA_STRUCT;

///
/// Uma Structure
///
///
typedef struct _UMA_INFO {
  OUT UINT64 UmaBase;          ///< UmaBase[63:0] = Addr[63:0]
  OUT UINT32 UmaSize;          ///< UmaSize[31:0] = Addr[31:0]
  OUT UINT32 UmaAttributes;    ///< Indicate the attribute of Uma
  OUT UINT8 UmaMode;           ///< Indicate the mode of Uma
  OUT UINT16 MemClock;         ///< Indicate memory running speed in MHz
  OUT UINT8 Reserved[3];       ///< Reserved for future usage
} UMA_INFO;

/// Bitfield for ID
typedef struct {
  OUT UINT16 SocketId:8;       ///< Socket ID
  OUT UINT16 ModuleId:8;       ///< Module ID
} ID_FIELD;
///
/// Union for ID of socket and module that will be passed out in call out
///
typedef union {
  OUT ID_FIELD IdField;         ///< Bitfield for ID
  OUT UINT16 IdInformation;     ///< ID information for call out
} ID_INFO;

//  AGESA MEMORY ERRORS

// AGESA_ALERT Memory Errors
#define MEM_ALERT_USER_TMG_MODE_OVERRULED   0x04010000ul       ///< TIMING_MODE_SPECIFIC is requested but
                                                               ///< cannot be applied to current configurations.
#define MEM_ALERT_ORG_MISMATCH_DIMM 0x04010100ul               ///< DIMM organization miss-match
#define MEM_ALERT_BK_INT_DIS 0x04010200ul                      ///< Bank interleaving disable for internal issue

// AGESA_ERROR Memory Errors
#define MEM_ERROR_NO_DQS_POS_RD_WINDOW 0x04010300ul            ///< No DQS Position window for RD DQS
#define MEM_ERROR_SMALL_DQS_POS_RD_WINDOW 0x04020300ul         ///< Small DQS Position window for RD DQS
#define MEM_ERROR_NO_DQS_POS_WR_WINDOW 0x04030300ul            ///< No DQS Position window for WR DQS
#define MEM_ERROR_SMALL_DQS_POS_WR_WINDOW 0x04040300ul         ///< Small DQS Position window for WR DQS
#define MEM_ERROR_DIMM_SPARING_NOT_ENABLED 0x04010500ul        ///< DIMM sparing has not been enabled for an internal issues
#define MEM_ERROR_RCVR_EN_VALUE_TOO_LARGE 0x04050300ul         ///< Receive Enable value is too large
#define MEM_ERROR_RCVR_EN_NO_PASSING_WINDOW 0x04060300ul       ///< There is no DQS receiver enable window
#define MEM_ERROR_DRAM_ENABLED_TIME_OUT 0x04010600ul           ///< Time out when polling DramEnabled bit
#define MEM_ERROR_DCT_ACCESS_DONE_TIME_OUT 0x04010700ul        ///< Time out when polling DctAccessDone bit
#define MEM_ERROR_SEND_CTRL_WORD_TIME_OUT 0x04010800ul         ///< Time out when polling SendCtrlWord bit
#define MEM_ERROR_PREF_DRAM_TRAIN_MODE_TIME_OUT 0x04010900ul   ///< Time out when polling PrefDramTrainMode bit
#define MEM_ERROR_ENTER_SELF_REF_TIME_OUT 0x04010A00ul         ///< Time out when polling EnterSelfRef bit
#define MEM_ERROR_FREQ_CHG_IN_PROG_TIME_OUT 0x04010B00ul       ///< Time out when polling FreqChgInProg bit
#define MEM_ERROR_EXIT_SELF_REF_TIME_OUT 0x04020A00ul          ///< Time out when polling ExitSelfRef bit
#define MEM_ERROR_SEND_MRS_CMD_TIME_OUT 0x04010C00ul           ///< Time out when polling SendMrsCmd bit
#define MEM_ERROR_SEND_ZQ_CMD_TIME_OUT 0x04010D00ul            ///< Time out when polling SendZQCmd bit
#define MEM_ERROR_DCT_EXTRA_ACCESS_DONE_TIME_OUT 0x04010E00ul  ///< Time out when polling DctExtraAccessDone bit
#define MEM_ERROR_MEM_CLR_BUSY_TIME_OUT 0x04010F00ul           ///< Time out when polling MemClrBusy bit
#define MEM_ERROR_MEM_CLEARED_TIME_OUT 0x04020F00ul            ///< Time out when polling MemCleared bit
#define MEM_ERROR_FLUSH_WR_TIME_OUT 0x04011000ul               ///< Time out when polling FlushWr bit
#define MEM_ERROR_NBPSTATE_TRANSITION_TIME_OUT 0x04012600ul    ///< Time out when polling CurNBPstate bit
#define MEM_ERROR_MAX_LAT_NO_WINDOW 0x04070300ul               ///< Fail to find pass during Max Rd Latency training
#define MEM_ERROR_PARALLEL_TRAINING_LAUNCH_FAIL 0x04080300ul   ///< Fail to launch training code on an AP
#define MEM_ERROR_PARALLEL_TRAINING_TIME_OUT 0x04090300ul      ///< Fail to finish parallel training
#define MEM_ERROR_NO_ADDRESS_MAPPING 0x04011100ul              ///< No address mapping found for a dimm
#define MEM_ERROR_RCVR_EN_NO_PASSING_WINDOW_EQUAL_LIMIT  0x040A0300ul ///< There is no DQS receiver enable window and the value is equal to the largest value
#define MEM_ERROR_RCVR_EN_VALUE_TOO_LARGE_LIMIT_LESS_ONE 0x040B0300ul ///< Receive Enable value is too large and is 1 less than limit
#define MEM_ERROR_CHECKSUM_NV_SPDCHK_RESTRT_ERROR  0x04011200ul       ///< SPD Checksum error for NV_SPDCHK_RESTRT
#define MEM_ERROR_NO_CHIPSELECT 0x04011300ul                   ///< No chipselects found
#define MEM_ERROR_UNSUPPORTED_333MHZ_UDIMM 0x04011500ul        ///< Unbuffered dimm is not supported at 333MHz
#define MEM_ERROR_WL_PRE_OUT_OF_RANGE 0x040C0300ul             ///< Returned PRE value during write levelizzation was out of range
#define MEM_ERROR_LR_IBT_NOT_FOUND  0x04013500ul               ///< No LR dimm IBT value is found
#define MEM_ERROR_MR0_NOT_FOUND  0x04023500ul                  ///< No MR0 value is found
#define MEM_ERROR_ODT_PATTERN_NOT_FOUND  0x04033500ul          ///< No odt pattern value is found
#define MEM_ERROR_RC2_IBT_NOT_FOUND  0x04043500ul              ///< No RC2 IBT value is found
#define MEM_ERROR_RC10_OP_SPEED_NOT_FOUND  0x04053500ul        ///< No RC10 op speed is found
#define MEM_ERROR_RTT_NOT_FOUND  0x04063500ul                  ///< No RTT value is found
#define MEM_ERROR_P2D_NOT_FOUND  0x04073500ul                  ///< No 2D training config value is found
#define MEM_ERROR_SAO_NOT_FOUND  0x04083500ul                  ///< No slow access mode, Address timing and Output driver compensation value is found
#define MEM_ERROR_CLK_DIS_MAP_NOT_FOUND  0x04093500ul          ///< No CLK disable map is found
#define MEM_ERROR_CKE_TRI_MAP_NOT_FOUND  0x040A3500ul          ///< No CKE tristate map is found
#define MEM_ERROR_ODT_TRI_MAP_NOT_FOUND  0x040B3500ul          ///< No ODT tristate map is found
#define MEM_ERROR_CS_TRI_MAP_NOT_FOUND  0x040C3500ul           ///< No CS tristate map is found
#define MEM_ERROR_TRAINING_SEED_NOT_FOUND  0x040D3500ul        ///< No training seed is found

// AGESA_WARNING Memory Errors
#define MEM_WARNING_UNSUPPORTED_QRDIMM      0x04011600ul       ///< QR DIMMs detected but not supported
#define MEM_WARNING_UNSUPPORTED_UDIMM       0x04021600ul       ///< U DIMMs detected but not supported
#define MEM_WARNING_UNSUPPORTED_SODIMM      0x04031600ul       ///< SO-DIMMs detected but not supported
#define MEM_WARNING_UNSUPPORTED_X4DIMM      0x04041600ul       ///< x4 DIMMs detected but not supported
#define MEM_WARNING_UNSUPPORTED_RDIMM       0x04051600ul       ///< R DIMMs detected but not supported
#define MEM_WARNING_UNSUPPORTED_LRDIMM      0x04061600ul       ///< LR DIMMs detected but not supported
#define MEM_WARNING_EMP_NOT_SUPPORTED       0x04011700ul       ///< Processor is not capable for EMP
#define MEM_WARNING_EMP_CONFLICT            0x04021700ul       ///< EMP cannot be enabled if channel interleaving,
#define MEM_WARNING_EMP_NOT_ENABLED         0x04031700ul       ///< Memory size is not power of two.
#define MEM_WARNING_ECC_DIS                 0x04041700ul       ///< ECC has been disabled as a result of an internal issue
#define MEM_WARNING_PERFORMANCE_ENABLED_BATTERY_LIFE_PREFERRED 0x04011800ul  ///< Performance has been enabled, but battery life is preferred.
                                                                             ///< bank interleaving, or bank swizzle is enabled.
#define MEM_WARNING_NO_SPDTRC_FOUND               0x04011900ul ///< No Trc timing value found in SPD of a dimm.
#define MEM_WARNING_NODE_INTERLEAVING_NOT_ENABLED 0x04012000ul ///< Node Interleaveing Requested, but could not be enabled
#define MEM_WARNING_CHANNEL_INTERLEAVING_NOT_ENABLED 0x04012100ul ///< Channel Interleaveing Requested, but could not be enabled
#define MEM_WARNING_BANK_INTERLEAVING_NOT_ENABLED 0x04012200ul ///< Bank Interleaveing Requested, but could not be enabled
#define MEM_WARNING_VOLTAGE_1_35_NOT_SUPPORTED    0x04012300ul ///< Voltage 1.35 determined, but could not be supported
#define MEM_WARNING_INITIAL_DDR3VOLT_NONZERO      0x04012400ul ///< DDR3 voltage initial value is not 0
#define MEM_WARNING_NO_COMMONLY_SUPPORTED_VDDIO   0x04012500ul ///< Cannot find a commonly supported VDDIO

// AGESA_FATAL Memory Errors
#define MEM_ERROR_MINIMUM_MODE              0x04011A00ul       ///< Running in minimum mode
#define MEM_ERROR_MODULE_TYPE_MISMATCH_DIMM 0x04011B00ul       ///< DIMM modules are miss-matched
#define MEM_ERROR_NO_DIMM_FOUND_ON_SYSTEM   0x04011C00ul       ///< No DIMMs have been found
#define MEM_ERROR_MISMATCH_DIMM_CLOCKS      0x04011D00ul       ///< DIMM clocks miss-matched
#define MEM_ERROR_NO_CYC_TIME               0x04011E00ul       ///< No cycle time found
#define MEM_ERROR_HEAP_ALLOCATE_DYN_STORING_OF_TRAINED_TIMINGS     0x04011F00ul  ///< Heap allocation error with dynamic storing of trained timings
#define MEM_ERROR_HEAP_ALLOCATE_FOR_DCT_STRUCT_AND_CH_DEF_STRUCTs  0x04021F00ul  ///< Heap allocation error for DCT_STRUCT and CH_DEF_STRUCT
#define MEM_ERROR_HEAP_ALLOCATE_FOR_REMOTE_TRAINING_ENV   0x04031F00ul           ///< Heap allocation error with REMOTE_TRAINING_ENV
#define MEM_ERROR_HEAP_ALLOCATE_FOR_SPD               0x04041F00ul    ///< Heap allocation error for SPD data
#define MEM_ERROR_HEAP_ALLOCATE_FOR_RECEIVED_DATA     0x04051F00ul    ///< Heap allocation error for RECEIVED_DATA during parallel training
#define MEM_ERROR_HEAP_ALLOCATE_FOR_S3_SPECIAL_CASE_REGISTERS     0x04061F00ul   ///< Heap allocation error for S3 "SPECIAL_CASE_REGISTER"
#define MEM_ERROR_HEAP_ALLOCATE_FOR_TRAINING_DATA     0x04071F00ul    ///< Heap allocation error for Training Data
#define MEM_ERROR_HEAP_ALLOCATE_FOR_IDENTIFY_DIMM_MEM_NB_BLOCK    0x04081F00ul   ///< Heap allocation error for  DIMM Identify "MEM_NB_BLOCK
#define MEM_ERROR_NO_CONSTRUCTOR_FOR_IDENTIFY_DIMM    0x04022300ul    ///< No Constructor for DIMM Identify
#define MEM_ERROR_VDDIO_UNSUPPORTED                   0x04022500ul    ///< VDDIO of the dimms on the board is not supported
#define MEM_ERROR_HEAP_ALLOCATE_FOR_2D                0x040B1F00ul    ///< Heap allocation error for 2D training data
#define MEM_ERROR_HEAP_DEALLOCATE_FOR_2D              0x040C1F00ul    ///< Heap de-allocation error for 2D training data

// AGESA_CRITICAL Memory Errors
#define MEM_ERROR_HEAP_ALLOCATE_FOR_DMI_TABLE_DDR3    0x04091F00ul    ///< Heap allocation error for DMI table for DDR3
#define MEM_ERROR_HEAP_ALLOCATE_FOR_DMI_TABLE_DDR2    0x040A1F00ul    ///< Heap allocation error for DMI table for DDR2
#define MEM_ERROR_UNSUPPORTED_DIMM_CONFIG             0x04011400ul    ///< Dimm population is not supported
#define MEM_ERROR_HEAP_ALLOCATE_FOR_CRAT_MEM_AFFINITY 0x040D1F00ul    ///< Heap allocation error for CRAT memory affinity info



/*----------------------------------------------------------------------------
 *
 *                END OF MEMORY-SPECIFIC DATA STRUCTURES
 *
 *----------------------------------------------------------------------------
 */




/*----------------------------------------------------------------------------
 *
 *                    CPU RELATED DEFINITIONS
 *
 *----------------------------------------------------------------------------
 */

// CPU Event definitions.

// Defines used to filter CPU events based on functional blocks
#define CPU_EVENT_PM_EVENT_MASK                         0xFF00FF00ul
#define CPU_EVENT_PM_EVENT_CLASS                        0x08000400ul

//================================================================
// CPU General events
//    Heap allocation                     (AppFunction =      01h)
#define CPU_ERROR_HEAP_BUFFER_IS_NOT_PRESENT            0x08000100ul
#define CPU_ERROR_HEAP_IS_ALREADY_INITIALIZED           0x08010100ul
#define CPU_ERROR_HEAP_IS_FULL                          0x08020100ul
#define CPU_ERROR_HEAP_BUFFER_HANDLE_IS_ALREADY_USED    0x08030100ul
#define CPU_ERROR_HEAP_BUFFER_HANDLE_IS_NOT_PRESENT     0x08040100ul
//    BrandId                             (AppFunction =      02h)
#define CPU_ERROR_BRANDID_HEAP_NOT_AVAILABLE            0x08000200ul
//    Micro code patch                    (AppFunction =      03h)
#define CPU_ERROR_MICRO_CODE_PATCH_IS_NOT_LOADED        0x08000300ul
//    Power management                    (AppFunction =      04h)
#define CPU_EVENT_PM_PSTATE_OVERCURRENT                 0x08000400ul
#define CPU_EVENT_PM_ALL_PSTATE_OVERCURRENT             0x08010400ul
#define CPU_ERROR_PSTATE_HEAP_NOT_AVAILABLE             0x08020400ul
#define CPU_ERROR_PM_NB_PSTATE_MISMATCH                 0x08030400ul
//    Other CPU events                    (AppFunction =      05h)
#define CPU_EVENT_BIST_ERROR                            0x08000500ul
#define CPU_EVENT_UNKNOWN_PROCESSOR_FAMILY              0x08010500ul
#define CPU_EVENT_STACK_REENTRY                         0x08020500ul
#define CPU_EVENT_CORE_NOT_IDENTIFIED                   0x08030500ul

//=================================================================
// CPU Feature events
//    Execution cache                     (AppFunction =      21h)
//        AGESA_CACHE_SIZE_REDUCED                            2101
//        AGESA_CACHE_REGIONS_ACROSS_1MB                      2102
//        AGESA_CACHE_REGIONS_ACROSS_4GB                      2103
//        AGESA_REGION_NOT_ALIGNED_ON_BOUNDARY                2104
//        AGESA_CACHE_START_ADDRESS_LESS_D0000                2105
//        AGESA_THREE_CACHE_REGIONS_ABOVE_1MB                 2106
//        AGESA_DEALLOCATE_CACHE_REGIONS                      2107
#define CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR      0x08002100ul
//    Core Leveling                       (AppFunction =      22h)
#define CPU_WARNING_ADJUSTED_LEVELING_MODE              0x08002200ul
//    HT Assist                           (AppFunction =      23h)
#define CPU_WARNING_NONOPTIMAL_HT_ASSIST_CFG            0x08002300ul

// CPU Build Configuration structures and definitions

/// Build Configuration structure for BLDCFG_AP_MTRR_SETTINGS
typedef struct {
  IN  UINT32 MsrAddr;     ///< Fixed-Sized MTRR address
  IN  UINT64 MsrData;     ///< MTRR Settings
} AP_MTRR_SETTINGS;

#define AMD_AP_MTRR_FIX64k_00000    0x00000250ul
#define AMD_AP_MTRR_FIX16k_80000    0x00000258ul
#define AMD_AP_MTRR_FIX16k_A0000    0x00000259ul
#define AMD_AP_MTRR_FIX4k_C0000     0x00000268ul
#define AMD_AP_MTRR_FIX4k_C8000     0x00000269ul
#define AMD_AP_MTRR_FIX4k_D0000     0x0000026Aul
#define AMD_AP_MTRR_FIX4k_D8000     0x0000026Bul
#define AMD_AP_MTRR_FIX4k_E0000     0x0000026Cul
#define AMD_AP_MTRR_FIX4k_E8000     0x0000026Dul
#define AMD_AP_MTRR_FIX4k_F0000     0x0000026Eul
#define AMD_AP_MTRR_FIX4k_F8000     0x0000026Ful
#define CPU_LIST_TERMINAL           0xFFFFFFFFul

/// Data structure for the Mapping Item between Unified ID for IDS Setup Option
/// and the option value.
///
typedef struct {
  IN    UINT16 IdsNvId;           ///< Unified ID for IDS Setup Option.
  OUT UINT16 IdsNvValue;        ///< The value of IDS Setup Option.
} IDS_NV_ITEM;

/// Data Structure for IDS CallOut Function
typedef struct {
  IN    AMD_CONFIG_PARAMS StdHeader;        ///< Standard configuration header
  IN    IDS_NV_ITEM *IdsNvPtr;              ///< Memory Pointer of IDS NV Table
  IN OUT UINTN Reserved;              ///< reserved
} IDS_CALLOUT_STRUCT;

/************************************************************************
 *
 *  AGESA interface Call-Out function parameter structures
 *
 ***********************************************************************/

/// Parameters structure for interface call-out AgesaAllocateBuffer
typedef struct {
  IN OUT    AMD_CONFIG_PARAMS   StdHeader;      ///< Standard configuration header
  IN OUT    UINT32              BufferLength;   ///< Size of buffer to allocate
  IN        UINT32              BufferHandle;   ///< Identifier or name for the buffer
  OUT       VOID                *BufferPointer; ///< location of the created buffer
} AGESA_BUFFER_PARAMS;

/// Parameters structure for interface call-out AgesaRunCodeOnAp
typedef struct {
  IN OUT    AMD_CONFIG_PARAMS   StdHeader;            ///< Standard configuration header
  IN        UINT32              FunctionNumber;       ///< Index of the procedure to execute
  IN        VOID                *RelatedDataBlock;    ///< Location of data structure the procedure will use
  IN        UINT32              RelatedBlockLength;   ///< Size of the related data block
} AP_EXE_PARAMS;

/// Parameters structure for the interface call-out AgesaReadSpd & AgesaReadSpdRecovery
typedef struct {
  IN OUT    AMD_CONFIG_PARAMS   StdHeader;      ///< Standard configuration header
  IN        UINT8               SocketId;       ///< Address of SPD - socket ID
  IN        UINT8               MemChannelId;   ///< Address of SPD - memory channel ID
  IN        UINT8               DimmId;         ///< Address of SPD - DIMM ID
  IN OUT    UINT8               *Buffer;        ///< Location where to place the SPD content
  IN OUT    MEM_DATA_STRUCT     *MemData;       ///< Location of the MemData structure, for reference
} AGESA_READ_SPD_PARAMS;

/// Buffer Handles
typedef enum {
  AMD_DMI_INFO_BUFFER_HANDLE = 0x000D000,       ///< Assign 0x000D000 buffer handle to DMI function
  AMD_PSTATE_DATA_BUFFER_HANDLE,                ///< Assign 0x000D001 buffer handle to Pstate data
  AMD_PSTATE_ACPI_BUFFER_HANDLE,                ///< Assign 0x000D002 buffer handle to Pstate table
  AMD_BRAND_ID_BUFFER_HANDLE,                   ///< Assign 0x000D003 buffer handle to Brand ID
  AMD_ACPI_SLIT_BUFFER_HANDLE,                  ///< Assign 0x000D004 buffer handle to SLIT function
  AMD_SRAT_INFO_BUFFER_HANDLE,                  ///< Assign 0x000D005 buffer handle to SRAT function
  AMD_WHEA_BUFFER_HANDLE,                       ///< Assign 0x000D006 buffer handle to WHEA function
  AMD_S3_INFO_BUFFER_HANDLE,                    ///< Assign 0x000D007 buffer handle to S3 function
  AMD_S3_NB_INFO_BUFFER_HANDLE,                 ///< Assign 0x000D008 buffer handle to S3 NB device info
  AMD_ACPI_ALIB_BUFFER_HANDLE,                  ///< Assign 0x000D009 buffer handle to ALIB SSDT table
  AMD_ACPI_IVRS_BUFFER_HANDLE,                  ///< Assign 0x000D00A buffer handle to IOMMU IVRS table
  AMD_CRAT_INFO_BUFFER_HANDLE,                  ///< Assign 0x000D00B buffer handle to CRAT function
  AMD_ACPI_CDIT_BUFFER_HANDLE                   ///< Assign 0x000D00C buffer handle to CDIT function
} AMD_BUFFER_HANDLE;


/************************************************************************
 *
 *  AGESA interface Call-Out function prototypes
 *
 ***********************************************************************/

VOID
AgesaDoReset (
  IN        UINTN               ResetType,
  IN OUT    AMD_CONFIG_PARAMS   *StdHeader
  );

AGESA_STATUS
AgesaAllocateBuffer (
  IN      UINTN                 FcnData,
  IN OUT  AGESA_BUFFER_PARAMS   *AllocParams
  );

AGESA_STATUS
AgesaDeallocateBuffer (
  IN      UINTN                 FcnData,
  IN OUT  AGESA_BUFFER_PARAMS   *DeallocParams
  );

AGESA_STATUS
AgesaLocateBuffer (
  IN      UINTN                 FcnData,
  IN OUT  AGESA_BUFFER_PARAMS   *LocateParams
  );

AGESA_STATUS
AgesaReadSpd (
  IN        UINTN                 FcnData,
  IN OUT    AGESA_READ_SPD_PARAMS *ReadSpd
  );

AGESA_STATUS
AgesaReadSpdRecovery (
  IN        UINTN                 FcnData,
  IN OUT    AGESA_READ_SPD_PARAMS *ReadSpd
  );

AGESA_STATUS
AgesaHookBeforeDramInitRecovery (
  IN       UINTN           FcnData,
  IN OUT   MEM_DATA_STRUCT *MemData
  );

AGESA_STATUS
AgesaRunFcnOnAp (
  IN        UINTN               ApicIdOfCore,
  IN        AP_EXE_PARAMS       *LaunchApParams
  );

AGESA_STATUS
AgesaHookBeforeDramInit (
  IN        UINTN               SocketIdModuleId,
  IN OUT    MEM_DATA_STRUCT     *MemData
  );

AGESA_STATUS
AgesaHookBeforeDQSTraining (
  IN        UINTN               SocketIdModuleId,
  IN OUT    MEM_DATA_STRUCT     *MemData
  );

AGESA_STATUS
AgesaHookBeforeExitSelfRefresh (
  IN        UINTN               FcnData,
  IN OUT    MEM_DATA_STRUCT     *MemData
  );

AGESA_STATUS
AgesaPcieSlotResetControl (
  IN      UINTN                 FcnData,
  IN      PCIe_SLOT_RESET_INFO  *ResetInfo
 );

AGESA_STATUS
AgesaGetVbiosImage (
  IN       UINTN                 FcnData,
  IN OUT   GFX_VBIOS_IMAGE_INFO *VbiosImageInfo
  );

AGESA_STATUS
AgesaFchOemCallout (
  IN      VOID                  *FchData
 );

AGESA_STATUS
excel331  (
  IN        UINTN               SocketIdModuleId,
  IN OUT    MEM_DATA_STRUCT     *MemData
  );

AGESA_STATUS
AgesaGetIdsData  (
  IN       UINTN              Data,
  IN OUT   IDS_CALLOUT_STRUCT *IdsCalloutData
  );
/************************************************************************
 *
 *  AGESA interface structure definition and function prototypes
 *
 ***********************************************************************/

/**********************************************************************
 * Platform Configuration:  The parameters in boot branch function
 **********************************************************************/

///  The possible platform control flow settings.
typedef enum  {
  Nfcm,                                          ///< Normal Flow Control Mode.
  UmaDr,                                         ///< UMA using Display Refresh flow control.
  UmaIfcm,                                       ///< UMA using Isochronous Flow Control.
  Ifcm,                                          ///< Isochronous Flow Control Mode (other than for UMA).
  Iommu,                                         ///< An IOMMU is in use in the system.
  MaxControlFlow                                 ///< Not a control flow mode, use for limit checking.
} PLATFORM_CONTROL_FLOW;

///  Platform Deemphasis Levels.
///
/// The deemphasis level is set for the receiver, based on link characterization.  The DCV level is
/// set based on the level of the far transmitter.
typedef enum {
  DeemphasisLevelNone,                           ///< No Deemphasis.
  DeemphasisLevelMinus3,                         ///< Minus 3 db deemphasis.
  DeemphasisLevelMinus6,                         ///< Minus 6 db deemphasis.
  DeemphasisLevelMinus8,                         ///< Minus 8 db deemphasis.
  DeemphasisLevelMinus11,                        ///< Minus 11 db deemphasis.
  DeemphasisLevelMinus11pre8,                    ///< Minus 11, Minus 8 precursor db deemphasis.
  DcvLevelNone = 16,                             ///< No DCV Deemphasis.
  DcvLevelMinus2,                                ///< Minus 2 db DCV deemphasis.
  DcvLevelMinus3,                                ///< Minus 3 db DCV deemphasis.
  DcvLevelMinus5,                                ///< Minus 5 db DCV deemphasis.
  DcvLevelMinus6,                                ///< Minus 6 db DCV deemphasis.
  DcvLevelMinus7,                                ///< Minus 7 db DCV deemphasis.
  DcvLevelMinus8,                                ///< Minus 8 db DCV deemphasis.
  DcvLevelMinus9,                                ///< Minus 9 db DCV deemphasis.
  DcvLevelMinus11,                               ///< Minus 11 db DCV deemphasis.
  MaxPlatformDeemphasisLevel                     ///< Not a deemphasis level, use for limit checking.
} PLATFORM_DEEMPHASIS_LEVEL;

///  Provide Deemphasis Levels for HT Links.
///
///  For each CPU to CPU or CPU to IO device HT link, the list of Deemphasis Levels will
///  be checked for a match.  The item matches for a Socket, Link if the link frequency is
///  is in the inclusive range HighFreq:LoFreq.
///  AGESA does not set deemphasis in IO devices, only in processors.

typedef struct {
  // Match fields
  IN       UINT8 Socket;                                        ///< One Socket on which this Link is located
  IN       UINT8 Link;                                          ///< The Link on this Processor.
  IN       UINT8 LoFreq;                                        ///< If the link is set to this frequency or greater, apply these levels, and
  IN       UINT8 HighFreq;                                      ///< If the link is set to this frequency or less, apply these levels.
  // Value fields
  IN       PLATFORM_DEEMPHASIS_LEVEL     ReceiverDeemphasis;    ///< The deemphasis level for this link
  IN       PLATFORM_DEEMPHASIS_LEVEL     DcvDeemphasis;         ///< The DCV, or far transmitter deemphasis level.
} CPU_HT_DEEMPHASIS_LEVEL;


///  The possible hardware prefetch mode settings.
typedef enum  {
  HARDWARE_PREFETCHER_AUTO,                     ///< Use the recommended setting for the processor. In most cases, the recommended setting is enabled.
  DISABLE_L1_PREFETCHER,                        ///< Use the recommended settings for the hardware prefetcher, but disable L1 prefetching.
  DISABLE_HW_PREFETCHER_TRAINING_ON_SOFTWARE_PREFETCHES,  ///< Use the recommended setting for the hardware prefetcher, but disable training on software prefetches.
  DISABLE_L1_PREFETCHER_AND_HW_PREFETCHER_TRAINING_ON_SOFTWARE_PREFETCHES,  ///< Use the recommended settings for the hardware prefetcher, but disable both the L1 prefetcher and training on software prefetches.
  DISABLE_HARDWARE_PREFETCH,                    ///< Disable hardware prefetching.
  MAX_HARDWARE_PREFETCH_MODE                    ///< Not a hardware prefetch mode, use for limit checking.
} HARDWARE_PREFETCH_MODE;

///  The possible software prefetch mode settings.
typedef enum  {
  SOFTWARE_PREFETCHES_AUTO,                     ///< Use the recommended setting for the processor. In most cases, the recommended setting is enabled.
  DISABLE_SOFTWARE_PREFETCHES,                  ///< Disable software prefetches (convert software prefetch instructions to NOP).
  MAX_SOFTWARE_PREFETCH_MODE                    ///< Not a software prefetch mode, use for limit checking.
} SOFTWARE_PREFETCH_MODE;

/// Advanced performance tunings, prefetchers.
/// These settings provide for performance tuning to optimize for specific workloads.
typedef struct {
  IN HARDWARE_PREFETCH_MODE  HardwarePrefetchMode; ///< This value provides for advanced performance tuning by controlling the hardware prefetcher setting.
  IN SOFTWARE_PREFETCH_MODE  SoftwarePrefetchMode; ///< This value provides for advanced performance tuning by controlling the software prefetch instructions.
  IN DRAM_PREFETCH_MODE      DramPrefetchMode;     ///< This value provides for advanced performance tuning by controlling the DRAM prefetcher setting.
} ADVANCED_PERFORMANCE_PROFILE;

///  The possible platform power policy settings.
typedef enum  {
  Performance,                                   ///< Optimize for performance.
  BatteryLife,                                   ///< Optimize for battery life.
  MaxPowerPolicy                                 ///< Not a power policy mode, use for limit checking.
} PLATFORM_POWER_POLICY;

///  Platform performance settings for optimized settings.
///  Several configuration settings for the processor depend upon other parts and
///  general designer choices for the system. The determination of these data points
///  is not standard for all platforms, so the host environment needs to provide these
///  to specify how the system is to be configured.
typedef struct {
  IN PLATFORM_CONTROL_FLOW PlatformControlFlowMode;    ///< The platform's control flow mode for optimum platform performance.
                                                       ///< @BldCfgItem{BLDCFG_PLATFORM_CONTROL_FLOW_MODE}
  IN BOOLEAN               UseHtAssist;                ///< HyperTransport link traffic optimization.
                                                       ///< @BldCfgItem{BLDCFG_USE_HT_ASSIST}
  IN BOOLEAN               UseAtmMode;                 ///< HyperTransport link traffic optimization.
                                                       ///< @BldCfgItem{BLDCFG_USE_ATM_MODE}
  IN BOOLEAN               Use32ByteRefresh;           ///< Display Refresh traffic generates 32 byte requests.
                                                       ///< @BldCfgItem{BLDCFG_USE_32_BYTE_REFRESH}
  IN BOOLEAN               UseVariableMctIsocPriority; ///< The Memory controller will be set to Variable Isoc Priority.
                                                       ///< @BldCfgItem{BLDCFG_USE_VARIABLE_MCT_ISOC_PRIORITY}
  IN ADVANCED_PERFORMANCE_PROFILE AdvancedPerformanceProfile;   ///< The advanced platform performance settings.
  IN PLATFORM_POWER_POLICY PlatformPowerPolicy;        ///< The platform's desired power policy
                                                       ///< @BldCfgItem{BLDCFG_PLATFORM_POWER_POLICY_MODE}
} PERFORMANCE_PROFILE;

///  Platform settings that describe the voltage regulator modules of the system.
///  Many power management settings are dependent upon the characteristics of the
///  on-board voltage regulator module (VRM).  The host environment needs to provide
///  these to specify how the system is to be configured.
typedef struct {
  IN UINT32  CurrentLimit;                         ///< Vrm Current Limit.
                                                   ///< @BldCfgItem{BLDCFG_VRM_CURRENT_LIMIT}
                                                   ///< @BldCfgItem{BLDCFG_VRM_NB_CURRENT_LIMIT}
  IN UINT32  LowPowerThreshold;                    ///< Vrm Low Power Threshold.
                                                   ///< @BldCfgItem{BLDCFG_VRM_LOW_POWER_THRESHOLD}
                                                   ///< @BldCfgItem{BLDCFG_VRM_NB_LOW_POWER_THRESHOLD}
  IN UINT32  SlewRate;                             ///< Vrm Slew Rate.
                                                   ///< @BldCfgItem{BLDCFG_VRM_SLEW_RATE}
                                                   ///< @BldCfgItem{BLDCFG_VRM_NB_SLEW_RATE}
  IN UINT32  AdditionalDelay;                      ///< Vrm Additional Delay.
                                                   ///< @BldCfgItem{BLDCFG_VRM_ADDITIONAL_DELAY}
                                                   ///< @BldCfgItem{BLDCFG_VRM_NB_ADDITIONAL_DELAY}
  IN BOOLEAN HiSpeedEnable;                        ///< Select high speed VRM.
                                                   ///< @BldCfgItem{BLDCFG_VRM_HIGH_SPEED_ENABLE}
                                                   ///< @BldCfgItem{BLDCFG_VRM_NB_HIGH_SPEED_ENABLE}
  IN UINT32  InrushCurrentLimit;                   ///< Vrm Inrush Current Limit.
                                                   ///< @BldCfgItem{BLDCFG_VRM_MAXIMUM_CURRENT_LIMIT}
                                                   ///< @BldCfgItem{BLDCFG_VRM_NB_MAXIMUM_CURRENT_LIMIT}
  IN UINT32  SviOcpLevel;                          ///< SVI OCP Level.
                                                   ///< @BldCfgItem{BLDCFG_VRM_SVI_OCP_LEVEL}
                                                   ///< @BldCfgItem{BLDCFG_VRM_NB_SVI_OCP_LEVEL}
} PLATFORM_VRM_CONFIGURATION;

///  The VRM types to characterize.
typedef enum  {
  CoreVrm,                                       ///< VDD plane.
  NbVrm,                                         ///< VDDNB plane.
  MaxVrmType                                     ///< Not a valid VRM type, use for limit checking.
} PLATFORM_VRM_TYPE;


/// FCH Platform Configuration Policy
typedef struct {
  IN UINT16     CfgSmbus0BaseAddress;             ///< SMBUS0 Controller Base Address
  IN UINT16     CfgSmbus1BaseAddress;             ///< SMBUS1 Controller Base Address
  IN UINT16     CfgSioPmeBaseAddress;             ///< I/O base address for LPC I/O target range
  IN UINT16     CfgAcpiPm1EvtBlkAddr;             ///< I/O base address of ACPI power management Event Block
  IN UINT16     CfgAcpiPm1CntBlkAddr;             ///< I/O base address of ACPI power management Control Block
  IN UINT16     CfgAcpiPmTmrBlkAddr;              ///< I/O base address of ACPI power management Timer Block
  IN UINT16     CfgCpuControlBlkAddr;             ///< I/O base address of ACPI power management CPU Control Block
  IN UINT16     CfgAcpiGpe0BlkAddr;               ///< I/O base address of ACPI power management General Purpose Event Block
  IN UINT16     CfgSmiCmdPortAddr;                ///< I/O base address of ACPI SMI Command Block
  IN UINT16     CfgAcpiPmaCntBlkAddr;             ///< I/O base address of ACPI power management additional control block
  IN UINT32     CfgGecShadowRomBase;              ///< 32-bit base address to the GEC shadow ROM
  IN UINT32     CfgWatchDogTimerBase;             ///< Watchdog Timer base address
  IN UINT32     CfgSpiRomBaseAddress;             ///< Base address for the SPI ROM controller
  IN UINT32     CfgHpetBaseAddress;               ///< HPET MMIO base address
  IN UINT32     CfgAzaliaSsid;                    ///< Subsystem ID of HD Audio controller
  IN UINT32     CfgSmbusSsid;                     ///< Subsystem ID of SMBUS controller
  IN UINT32     CfgIdeSsid;                       ///< Subsystem ID of IDE controller
  IN UINT32     CfgSataAhciSsid;                  ///< Subsystem ID of SATA controller in AHCI mode
  IN UINT32     CfgSataIdeSsid;                   ///< Subsystem ID of SATA controller in IDE mode
  IN UINT32     CfgSataRaid5Ssid;                 ///< Subsystem ID of SATA controller in RAID5 mode
  IN UINT32     CfgSataRaidSsid;                  ///< Subsystem ID of SATA controller in RAID mode
  IN UINT32     CfgEhciSsid;                      ///< Subsystem ID of EHCI
  IN UINT32     CfgOhciSsid;                      ///< Subsystem ID of OHCI
  IN UINT32     CfgLpcSsid;                       ///< Subsystem ID of LPC ISA Bridge
  IN UINT32     CfgSdSsid;                        ///< Subsystem ID of SecureDigital controller
  IN UINT32     CfgXhciSsid;                      ///< Subsystem ID of XHCI
  IN BOOLEAN    CfgFchPort80BehindPcib;           ///< Is port80 cycle going to the PCI bridge
  IN BOOLEAN    CfgFchEnableAcpiSleepTrap;        ///< ACPI sleep SMI enable/disable
  IN GPP_LINKMODE CfgFchGppLinkConfig;            ///< GPP link configuration
  IN BOOLEAN    CfgFchGppPort0Present;            ///< Is FCH GPP port 0 present
  IN BOOLEAN    CfgFchGppPort1Present;            ///< Is FCH GPP port 1 present
  IN BOOLEAN    CfgFchGppPort2Present;            ///< Is FCH GPP port 2 present
  IN BOOLEAN    CfgFchGppPort3Present;            ///< Is FCH GPP port 3 present
  IN BOOLEAN    CfgFchGppPort0HotPlug;            ///< Is FCH GPP port 0 hotplug capable
  IN BOOLEAN    CfgFchGppPort1HotPlug;            ///< Is FCH GPP port 1 hotplug capable
  IN BOOLEAN    CfgFchGppPort2HotPlug;            ///< Is FCH GPP port 2 hotplug capable
  IN BOOLEAN    CfgFchGppPort3HotPlug;            ///< Is FCH GPP port 3 hotplug capable

  IN UINT8   CfgFchEsataPortBitMap;               ///< ESATA Port definition, eg: [0]=1, means port 0 is ESATA capable
  IN UINT8   CfgFchIrPinControl;                  ///< Register bitfield describing Infrared Pin Control:
                                                  ///<   [0] - IR Enable 0
                                                  ///<   [1] - IR Enable 1
                                                  ///<   [2] - IR Tx0
                                                  ///<   [3] - IR Tx1
                                                  ///<   [4] - IR Open Drain
                                                  ///<   [5] - IR Enable LED
  IN SD_CLOCK_CONTROL CfgFchSdClockControl;       ///< FCH SD Clock Control
  CONST IN SCI_MAP_CONTROL  *CfgFchSciMapControl;       ///< FCH SCI Mapping Control
  IN SATA_PHY_CONTROL *CfgFchSataPhyControl;      ///< FCH SATA PHY Control
  CONST IN GPIO_CONTROL     *CfgFchGpioControl;         ///< FCH GPIO Control
} FCH_PLATFORM_POLICY;


/// Build Option/Configuration Boolean Structure.
typedef struct {
  IN  AMD_CODE_HEADER VersionString;              ///< AMD embedded code version string

  //Build Option Area
  IN BOOLEAN OptionUDimms;                        ///< @ref BLDOPT_REMOVE_UDIMMS_SUPPORT "BLDOPT_REMOVE_UDIMMS_SUPPORT"
  IN BOOLEAN OptionRDimms;                        ///< @ref BLDOPT_REMOVE_RDIMMS_SUPPORT "BLDOPT_REMOVE_RDIMMS_SUPPORT"
  IN BOOLEAN OptionLrDimms;                      ///< @ref BLDOPT_REMOVE_LRDIMMS_SUPPORT "BLDOPT_REMOVE_LRDIMMS_SUPPORT"
  IN BOOLEAN OptionEcc;                           ///< @ref BLDOPT_REMOVE_ECC_SUPPORT "BLDOPT_REMOVE_ECC_SUPPORT"
  IN BOOLEAN OptionBankInterleave;                ///< @ref BLDOPT_REMOVE_BANK_INTERLEAVE "BLDOPT_REMOVE_BANK_INTERLEAVE"
  IN BOOLEAN OptionDctInterleave;                 ///< @ref BLDOPT_REMOVE_DCT_INTERLEAVE "BLDOPT_REMOVE_DCT_INTERLEAVE"
  IN BOOLEAN OptionNodeInterleave;                ///< @ref BLDOPT_REMOVE_NODE_INTERLEAVE "BLDOPT_REMOVE_NODE_INTERLEAVE"
  IN BOOLEAN OptionParallelTraining;              ///< @ref BLDOPT_REMOVE_PARALLEL_TRAINING "BLDOPT_REMOVE_PARALLEL_TRAINING"
  IN BOOLEAN OptionOnlineSpare;                   ///< @ref BLDOPT_REMOVE_ONLINE_SPARE_SUPPORT "BLDOPT_REMOVE_ONLINE_SPARE_SUPPORT"
  IN BOOLEAN OptionMemRestore;                    ///< @ref BLDOPT_REMOVE_MEM_RESTORE_SUPPORT "BLDOPT_REMOVE_MEM_RESTORE_SUPPORT"
  IN BOOLEAN OptionMultisocket;                   ///< @ref BLDOPT_REMOVE_MULTISOCKET_SUPPORT "BLDOPT_REMOVE_MULTISOCKET_SUPPORT"
  IN BOOLEAN OptionAcpiPstates;                   ///< @ref BLDOPT_REMOVE_ACPI_PSTATES "BLDOPT_REMOVE_ACPI_PSTATES"
  IN BOOLEAN OptionPStatesInHpcMode;              ///< @ref BLDCFG_PSTATE_HPC_MODE "BLDCFG_PSTATE_HPC_MODE"
  IN BOOLEAN OptionCrat;                          ///< @ref BLDOPT_REMOVE_CRAT "BLDOPT_REMOVE_CRAT"
  IN BOOLEAN OptionCdit;                          ///< @ref BLDOPT_REMOVE_CDIT "BLDOPT_REMOVE_CDIT"
  IN BOOLEAN OptionSrat;                          ///< @ref BLDOPT_REMOVE_SRAT "BLDOPT_REMOVE_SRAT"
  IN BOOLEAN OptionSlit;                          ///< @ref BLDOPT_REMOVE_SLIT "BLDOPT_REMOVE_SLIT"
  IN BOOLEAN OptionWhea;                          ///< @ref BLDOPT_REMOVE_WHEA "BLDOPT_REMOVE_WHEA"
  IN BOOLEAN OptionDmi;                           ///< @ref BLDOPT_REMOVE_DMI "BLDOPT_REMOVE_DMI"
  IN BOOLEAN OptionEarlySamples;                  ///< @ref BLDOPT_REMOVE_EARLY_SAMPLES "BLDOPT_REMOVE_EARLY_SAMPLES"
  IN BOOLEAN OptionAddrToCsTranslator;            ///< ADDR_TO_CS_TRANSLATOR

  //Build Configuration Area
  IN UINT64 CfgPciMmioAddress;                    ///< Pci Mmio Base Address to use for PCI Config accesses.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_PCI_MMIO_BASE}
  IN UINT32 CfgPciMmioSize;                       ///< Pci Mmio region Size.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_PCI_MMIO_SIZE}
  IN PLATFORM_VRM_CONFIGURATION CfgPlatVrmCfg[MaxVrmType]; ///< Several configuration settings for the voltage regulator modules.
  IN UINT32 CfgPlatNumIoApics;                    ///< The number of IO APICS for the platform.
  IN UINT32 CfgMemInitPstate;                     ///< Memory Init Pstate.
  IN PLATFORM_C1E_MODES CfgPlatformC1eMode;       ///< Select the C1e Mode that will used.
  IN UINT32 CfgPlatformC1eOpData;                 ///< An IO port or additional C1e setup data, depends on C1e mode.
  IN UINT32 CfgPlatformC1eOpData1;                ///< An IO port or additional C1e setup data, depends on C1e mode.
  IN UINT32 CfgPlatformC1eOpData2;                ///< An IO port or additional C1e setup data, depends on C1e mode.
  IN UINT32 CfgPlatformC1eOpData3;                ///< An IO port or additional C1e setup data, depends on C1e mode.
  IN PLATFORM_CSTATE_MODES CfgPlatformCStateMode; ///< Select the C-State Mode that will used.
  IN UINT32 CfgPlatformCStateOpData;              ///< An IO port or additional C-State setup data, depends on C-State mode.
  IN UINT16 CfgPlatformCStateIoBaseAddress;       ///< Specifies I/O ports that can be used to allow CPU to enter CStates
  IN PLATFORM_CPB_MODES CfgPlatformCpbMode;       ///< Enable or disable core performance boost
  IN PLATFORM_LOW_POWER_PSTATE_MODES CfgLowPowerPstateForProcHot; ///< Low power Pstate for PROCHOT mode
  IN UINT32 CfgCoreLevelingMode;                  ///< Apply any downcoring or core count leveling as specified.
  IN PERFORMANCE_PROFILE CfgPerformanceProfile;   ///< The platform's control flow mode and platform performance settings.
  IN CPU_HT_DEEMPHASIS_LEVEL *CfgPlatformDeemphasisList; ///< Deemphasis levels for the platform's HT links.

  IN UINT32 CfgAmdPlatformType;                   ///< Designate the platform as a Server, Desktop, or Mobile.
  IN UINT32 CfgAmdPstateCapValue;                 ///< Amd pstate ceiling enabling deck

  IN MEMORY_BUS_SPEED CfgMemoryBusFrequencyLimit; ///< Memory Bus Frequency Limit.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT}
  IN BOOLEAN CfgMemoryModeUnganged;               ///< Memory Mode Unganged.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_MODE_UNGANGED}
  IN BOOLEAN CfgMemoryQuadRankCapable;            ///< Memory Quad Rank Capable.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_QUAD_RANK_CAPABLE}
  IN QUANDRANK_TYPE CfgMemoryQuadrankType;        ///< Memory Quadrank Type.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_QUADRANK_TYPE}
  IN BOOLEAN CfgMemoryRDimmCapable;               ///< Memory RDIMM Capable.
  IN BOOLEAN CfgMemoryLRDimmCapable;              ///< Memory LRDIMM Capable.
  IN BOOLEAN CfgMemoryUDimmCapable;               ///< Memory UDIMM Capable.
  IN BOOLEAN CfgMemorySODimmCapable;              ///< Memory SODimm Capable.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_SODIMM_CAPABLE}
  IN BOOLEAN CfgLimitMemoryToBelow1Tb;            ///< Limit memory address space to below 1TB
  IN BOOLEAN CfgMemoryEnableBankInterleaving;     ///< Memory Enable Bank Interleaving.
  IN BOOLEAN CfgMemoryEnableNodeInterleaving;     ///< Memory Enable Node Interleaving.
  IN BOOLEAN CfgMemoryChannelInterleaving;        ///< Memory Channel Interleaving.
  IN BOOLEAN CfgMemoryPowerDown;                  ///< Memory Power Down.
  IN POWER_DOWN_MODE CfgPowerDownMode;            ///< Power Down Mode.
  IN BOOLEAN CfgOnlineSpare;                      ///< Online Spare.
  IN BOOLEAN CfgMemoryParityEnable;               ///< Memory Parity Enable.
  IN BOOLEAN CfgBankSwizzle;                      ///< Bank Swizzle.
  IN USER_MEMORY_TIMING_MODE CfgTimingModeSelect; ///< Timing Mode Select.
  IN MEMORY_BUS_SPEED CfgMemoryClockSelect;       ///< Memory Clock Select.
  IN BOOLEAN CfgDqsTrainingControl;               ///< Dqs Training Control.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_DQS_TRAINING_CONTROL}
  IN BOOLEAN CfgIgnoreSpdChecksum;                ///< Ignore Spd Checksum.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_IGNORE_SPD_CHECKSUM}
  IN BOOLEAN CfgUseBurstMode;                     ///< Use Burst Mode.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_USE_BURST_MODE}
  IN BOOLEAN CfgMemoryAllClocksOn;                ///< Memory All Clocks On.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_MEMORY_ALL_CLOCKS_ON}
  IN BOOLEAN CfgEnableEccFeature;                 ///< Enable ECC Feature.
  IN BOOLEAN CfgEccRedirection;                   ///< ECC Redirection.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_ECC_REDIRECTION}
  IN UINT16  CfgScrubDramRate;                    ///< Scrub Dram Rate.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_DRAM_RATE}
  IN UINT16  CfgScrubL2Rate;                      ///< Scrub L2Rate.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_L2_RATE}
  IN UINT16  CfgScrubL3Rate;                      ///< Scrub L3Rate.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_L3_RATE}
  IN UINT16  CfgScrubIcRate;                      ///< Scrub Ic Rate.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_IC_RATE}
  IN UINT16  CfgScrubDcRate;                      ///< Scrub Dc Rate.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_SCRUB_DC_RATE}
  IN BOOLEAN CfgEccSyncFlood;                     ///< ECC Sync Flood.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_ECC_SYNC_FLOOD}
  IN UINT16  CfgEccSymbolSize;                    ///< ECC Symbol Size.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_ECC_SYMBOL_SIZE}
  IN UINT64  CfgHeapDramAddress;                  ///< Heap contents will be temporarily stored in this address during the transition.
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_HEAP_DRAM_ADDRESS}
  IN BOOLEAN CfgNodeMem1GBAlign;                  ///< Node Mem 1GB boundary Alignment
  IN BOOLEAN CfgS3LateRestore;                    ///< S3 Late Restore
  IN BOOLEAN CfgAcpiPstateIndependent;            ///< PSD method dependent/Independent
  IN AP_MTRR_SETTINGS *CfgApMtrrSettingsList;     ///< The AP's MTRR settings before final halt
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_AP_MTRR_SETTINGS_LIST}
  IN UMA_MODE CfgUmaMode;                         ///< Uma Mode
  IN UINT32 CfgUmaSize;                           ///< Uma Size [31:0]=Addr[47:16]
  IN BOOLEAN CfgUmaAbove4G;                       ///< Uma Above 4G Support
  IN UMA_ALIGNMENT CfgUmaAlignment;               ///< Uma alignment
  IN BOOLEAN CfgProcessorScopeInSb;               ///< ACPI Processor Object in \\_SB scope
  IN CHAR8   CfgProcessorScopeName0;              ///< OEM specific 1st character of processor scope name.
  IN CHAR8   CfgProcessorScopeName1;              ///< OEM specific 2nd character of processor scope name.
  IN UINT8   CfgGnbHdAudio;                       ///< GNB HD Audio
  IN UINT8   CfgAbmSupport;                       ///< Abm Support
  IN UINT8   CfgDynamicRefreshRate;               ///< DRR Dynamic Refresh Rate
  IN UINT16  CfgLcdBackLightControl;              ///< LCD Backlight Control
  IN UINT8   CfgGnb3dStereoPinIndex;                ///< 3D Stereo Pin ID.
  IN UINT32  CfgTempPcieMmioBaseAddress;          ///< Temp pcie MMIO base Address
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_TEMP_PCIE_MMIO_BASE_ADDRESS}
  IN UINT32  CfgGnbIGPUSSID;                      ///< Gnb internal GPU SSID
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_IGPU_SUBSYSTEM_ID}
  IN UINT32  CfgGnbHDAudioSSID;                   ///< Gnb HD Audio SSID
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_IGPU_HD_AUDIO_SUBSYSTEM_ID}
  IN UINT32  CfgGnbPcieSSID;                      ///< Gnb PCIe SSID
                                                  ///< Build-time customizable only - @BldCfgItem{BLFCFG_APU_PCIE_PORTS_SUBSYSTEM_ID}
  IN UINT16  CfgLvdsSpreadSpectrum;               ///< Lvds Spread Spectrum
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_GFX_LVDS_SPREAD_SPECTRUM}
  IN UINT16  CfgLvdsSpreadSpectrumRate;           ///< Lvds Spread Spectrum Rate
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_GFX_LVDS_SPREAD_SPECTRUM_RATE}
  CONST IN FCH_PLATFORM_POLICY  *FchBldCfg;             ///< FCH platform build configuration policy

  IN BOOLEAN    CfgIommuSupport;                  ///< IOMMU support
  IN UINT8      CfgLvdsPowerOnSeqDigonToDe;       ///< Panel initialization timing
  IN UINT8      CfgLvdsPowerOnSeqDeToVaryBl;      ///< Panel initialization timing
  IN UINT8      CfgLvdsPowerOnSeqDeToDigon;       ///< Panel initialization timing
  IN UINT8      CfgLvdsPowerOnSeqVaryBlToDe;      ///< Panel initialization timing
  IN UINT8      CfgLvdsPowerOnSeqOnToOffDelay;    ///< Panel initialization timing
  IN UINT8      CfgLvdsPowerOnSeqVaryBlToBlon;    ///< Panel initialization timing
  IN UINT8      CfgLvdsPowerOnSeqBlonToVaryBl;    ///< Panel initialization timing
  IN UINT16     CfgLvdsMaxPixelClockFreq;         ///< The maximum pixel clock frequency supported
  IN UINT32     CfgLcdBitDepthControlValue;       ///< The LCD bit depth control settings
  IN UINT8      CfgLvds24bbpPanelMode;            ///< The LVDS 24 BBP mode
  IN LVDS_MISC_CONTROL CfgLvdsMiscControl;        ///< THe LVDS Misc control
  IN UINT16     CfgPcieRefClkSpreadSpectrum;      ///< PCIe Reference Clock Spread Spectrum
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_PCIE_REFCLK_SPREAD_SPECTRUM}
  IN BOOLEAN    CfgExternalVrefCtlFeature;        ///< External Vref control
  IN FORCE_TRAIN_MODE   CfgForceTrainMode;        ///< Force Train Mode
  IN BOOLEAN    CfgGnbRemoteDisplaySupport;       ///< Wireless Display Support
  IN IOMMU_EXCLUSION_RANGE_DESCRIPTOR *CfgIvrsExclusionRangeList;
  IN BOOLEAN    CfgGnbSyncFloodPinAsNmi;          ///< @ref BLDCFG_USE_SYNCFLOOD_AS_NMI "BLDCFG_USE_SYNCFLOOD_AS_NMI"
  IN UINT8      CfgIgpuEnableDisablePolicy;       ///< This item defines the iGPU Enable/Disable policy
                                                  ///< @li 0 = Auto - use current default
                                                  ///< @li 2 = Disable iGPU if ANY PCI or PCIe Graphics card is present
                                                  ///< @BldCfgItem{BLDCFG_IGPU_ENABLE_DISABLE_POLICY}
  IN UINT8      CfgGnbSwTjOffset;                 ///< Software-writeable TjOffset to account for changes in junction temperature
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_GNB_THERMAL_SENSOR_CORRECTION}
  IN UINT8      CfgLvdsMiscVoltAdjustment;        ///< Travis register LVDS_CTRL_4 to adjust LVDS output voltage
                                                  ///< Build-time customizable only - @BldCfgItem{BLDCFG_LVDS_MISC_VOL_ADJUSTMENT}
  IN DISPLAY_MISC_CONTROL CfgDisplayMiscControl;  ///< The Display Misc control
  IN BOOLEAN Reserved;                            ///< reserved...
} BUILD_OPT_CFG;

///  A structure containing platform specific operational characteristics. This
///  structure is initially populated by the initializer with a copy of the same
///  structure that was created at build time using the build configuration controls.
typedef struct _PLATFORM_CONFIGURATION {
  IN PERFORMANCE_PROFILE PlatformProfile;             ///< Several configuration settings for the processor.
  IN CPU_HT_DEEMPHASIS_LEVEL *PlatformDeemphasisList; ///< Deemphasis levels for the platform's HT links.
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_DEEMPHASIS_LIST}.
                                                      ///< @n @e Examples: See @ref DeemphasisExamples "Deemphasis List Examples".
  IN UINT8               CoreLevelingMode;            ///< Indicates how to balance the number of cores per processor.
                                                      ///< @BldCfgItem{BLDCFG_CORE_LEVELING_MODE}
  IN PLATFORM_C1E_MODES  C1eMode;                     ///< Specifies the method of C1e enablement - Disabled, HW, or message based.
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_C1E_MODE}
  IN UINT32              C1ePlatformData;             ///< If C1eMode is HW, specifies the P_LVL3 I/O port of the platform.
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_C1E_OPDATA}
  IN UINT32              C1ePlatformData1;            ///< If C1eMode is SW, specifies the address of chipset's SMI command port.
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_C1E_OPDATA1}
  IN UINT32              C1ePlatformData2;            ///< If C1eMode is SW, specifies the unique number used by the SMI handler to identify SMI source.
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_C1E_OPDATA2}
  IN UINT32              C1ePlatformData3;            ///< If C1eMode is Auto, specifies the P_LVL3 I/O port of the platform for HW C1e
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_C1E_OPDATA3}
  IN PLATFORM_CSTATE_MODES  CStateMode;               ///< Specifies the method of C-State enablement - Disabled, or C6.
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_CSTATE_MODE}
  IN UINT32              CStatePlatformData;          ///< This element specifies some pertinent data needed for the operation of the Cstate feature
                                                      ///< If CStateMode is CStateModeC6, this item is reserved
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_CSTATE_OPDATA}
  IN UINT16              CStateIoBaseAddress;         ///< This item specifies a free block of 8 consecutive bytes of I/O ports that
                                                      ///< can be used to allow the CPU to enter Cstates.
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_CSTATE_IO_BASE_ADDRESS}
  IN PLATFORM_CPB_MODES  CpbMode;                     ///< Specifies the method of core performance boost enablement - Disabled, or Auto.
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_CPB_MODE}
  IN BOOLEAN             UserOptionDmi;               ///< When set to TRUE, the DMI data table is generated.
  IN BOOLEAN             UserOptionPState;            ///< When set to TRUE, the PState data tables are generated.
  IN BOOLEAN             UserOptionCrat;              ///< When set to TRUE, the CRAT data table is generated.
  IN BOOLEAN             UserOptionCdit;              ///< When set to TRUE, the CDIT data table is generated.
  IN BOOLEAN             UserOptionSrat;              ///< When set to TRUE, the SRAT data table is generated.
  IN BOOLEAN             UserOptionSlit;              ///< When set to TRUE, the SLIT data table is generated.
  IN BOOLEAN             UserOptionWhea;              ///< When set to TRUE, the WHEA data table is generated.
  IN PLATFORM_LOW_POWER_PSTATE_MODES  LowPowerPstateForProcHot; ///< Specifies the method of low power Pstate for PROCHOT enablement - Disabled, or Auto.
  IN UINT32              PowerCeiling;                ///< P-State Ceiling Enabling Deck - Max power milli-watts.
  IN BOOLEAN             ForcePstateIndependent;      ///< P-State _PSD independence or dependence.
                                                      ///< @BldCfgItem{BLDCFG_FORCE_INDEPENDENT_PSD_OBJECT}
  IN BOOLEAN             PStatesInHpcMode;            ///< @BldCfgItem{BLDCFG_PSTATE_HPC_MODE}
  IN UINT32              NumberOfIoApics;             ///< Number of I/O APICs in the system
                                                      ///< @BldCfgItem{BLDCFG_PLATFORM_NUM_IO_APICS}
  IN PLATFORM_VRM_CONFIGURATION VrmProperties[MaxVrmType]; ///< Several configuration settings for the voltage regulator modules.
  IN BOOLEAN             ProcessorScopeInSb;          ///< ACPI Processor Object in \\_SB scope
                                                      ///< @BldCfgItem{BLDCFG_PROCESSOR_SCOPE_IN_SB}
  IN CHAR8               ProcessorScopeName0;         ///< OEM specific 1st character of processor scope name.
                                                      ///< @BldCfgItem{BLDCFG_PROCESSOR_SCOPE_NAME0}
  IN CHAR8               ProcessorScopeName1;         ///< OEM specific 2nd character of processor scope name.
                                                      ///< @BldCfgItem{BLDCFG_PROCESSOR_SCOPE_NAME1}
  IN UINT8               GnbHdAudio;                  ///< Control GFX HD Audio controller(Used for HDMI and DP display output),
                                                      ///< essentially it enables function 1 of graphics device.
                                                      ///< @li 0 = HD Audio disable
                                                      ///< @li 1 = HD Audio enable
                                                      ///< @BldCfgItem{BLDCFG_CFG_GNB_HD_AUDIO}
  IN UINT8               AbmSupport;                  ///< Automatic adjust LVDS/eDP Back light level support.It is
                                                      ///< characteristic specific to display panel which used by platform design.
                                                      ///< @li 0 = ABM support disabled
                                                      ///< @li 1 = ABM support enabled
                                                      ///< @BldCfgItem{BLDCFG_CFG_ABM_SUPPORT}
  IN UINT8               DynamicRefreshRate;          ///< Adjust refresh rate on LVDS/eDP.
                                                      ///< @BldCfgItem{BLDCFG_CFG_DYNAMIC_REFRESH_RATE}
  IN UINT16              LcdBackLightControl;         ///< The PWM frequency to LCD backlight control.
                                                      ///< If equal to 0 backlight not controlled by iGPU
                                                      ///< @BldCfgItem{BLDCFG_CFG_LCD_BACK_LIGHT_CONTROL}
} PLATFORM_CONFIGURATION;


/**********************************************************************
 * Structures for: AmdInitLate
 **********************************************************************/
#define PROC_VERSION_LENGTH  48
#define MAX_DIMMS_PER_SOCKET 16
#define PROC_MANU_LENGTH     29

/*  Interface Parameter Structures  */
/// DMI Type4 - Processor ID
typedef struct {
  OUT UINT32                    ProcIdLsd;              ///< Lower half of 64b ID
  OUT UINT32                    ProcIdMsd;              ///< Upper half of 64b ID
} TYPE4_PROC_ID;

/// DMI Type 4 - Processor information
typedef struct {
  OUT UINT8                     T4ProcType;             ///< CPU Type
  OUT UINT8                     T4ProcFamily;           ///< Family 1
  OUT TYPE4_PROC_ID             T4ProcId;               ///< Id
  OUT UINT8                     T4Voltage;              ///< Voltage
  OUT UINT16                    T4ExternalClock;        ///< External clock
  OUT UINT16                    T4MaxSpeed;             ///< Max speed
  OUT UINT16                    T4CurrentSpeed;         ///< Current speed
  OUT UINT8                     T4Status;               ///< Status
  OUT UINT8                     T4ProcUpgrade;          ///< Up grade
  OUT UINT8                     T4CoreCount;            ///< Core count
  OUT UINT8                     T4CoreEnabled;          ///< Core Enable
  OUT UINT8                     T4ThreadCount;          ///< Thread count
  OUT UINT16                    T4ProcCharacteristics;  ///< Characteristics
  OUT UINT16                    T4ProcFamily2;          ///< Family 2
  OUT CHAR8                     T4ProcVersion[PROC_VERSION_LENGTH]; ///< Cpu version
  OUT CHAR8                     T4ProcManufacturer[PROC_MANU_LENGTH]; ///< Manufacturer
} TYPE4_DMI_INFO;

/// DMI Type 7 - Cache information
typedef struct _TYPE7_DMI_INFO {
  OUT UINT16                    T7CacheCfg;             ///< Cache cfg
  OUT UINT16                    T7MaxCacheSize;         ///< Max size
  OUT UINT16                    T7InstallSize;          ///< Install size
  OUT UINT16                    T7SupportedSramType;    ///< Supported Sram Type
  OUT UINT16                    T7CurrentSramType;      ///< Current type
  OUT UINT8                     T7CacheSpeed;           ///< Speed
  OUT UINT8                     T7ErrorCorrectionType;  ///< ECC type
  OUT UINT8                     T7SystemCacheType;      ///< Cache type
  OUT UINT8                     T7Associativity;        ///< Associativity
} TYPE7_DMI_INFO;

/// DMI Type 16 offset 04h - Location
typedef enum {
  OtherLocation = 0x01,                                 ///< Assign 01 to Other
  UnknownLocation,                                      ///< Assign 02 to Unknown
  SystemboardOrMotherboard,                             ///< Assign 03 to systemboard or motherboard
  IsaAddonCard,                                         ///< Assign 04 to ISA add-on card
  EisaAddonCard,                                        ///< Assign 05 to EISA add-on card
  PciAddonCard,                                         ///< Assign 06 to PCI add-on card
  McaAddonCard,                                         ///< Assign 07 to MCA add-on card
  PcmciaAddonCard,                                      ///< Assign 08 to PCMCIA add-on card
  ProprietaryAddonCard,                                 ///< Assign 09 to proprietary add-on card
  NuBus,                                                ///< Assign 0A to NuBus
  Pc98C20AddonCard,                                     ///< Assign 0A0 to PC-98/C20 add-on card
  Pc98C24AddonCard,                                     ///< Assign 0A1 to PC-98/C24 add-on card
  Pc98EAddoncard,                                       ///< Assign 0A2 to PC-98/E add-on card
  Pc98LocalBusAddonCard                                 ///< Assign 0A3 to PC-98/Local bus add-on card
} DMI_T16_LOCATION;

/// DMI Type 16 offset 05h - Memory Error Correction
typedef enum {
  OtherUse = 0x01,                                      ///< Assign 01 to Other
  UnknownUse,                                           ///< Assign 02 to Unknown
  SystemMemory,                                         ///< Assign 03 to system memory
  VideoMemory,                                          ///< Assign 04 to video memory
  FlashMemory,                                          ///< Assign 05 to flash memory
  NonvolatileRam,                                       ///< Assign 06 to non-volatile RAM
  CacheMemory                                           ///< Assign 07 to cache memory
} DMI_T16_USE;

/// DMI Type 16 offset 07h - Maximum Capacity
typedef enum {
  Dmi16OtherErrCorrection = 0x01,                       ///< Assign 01 to Other
  Dmi16UnknownErrCorrection,                            ///< Assign 02 to Unknown
  Dmi16NoneErrCorrection,                               ///< Assign 03 to None
  Dmi16Parity,                                          ///< Assign 04 to parity
  Dmi16SingleBitEcc,                                    ///< Assign 05 to Single-bit ECC
  Dmi16MultiBitEcc,                                     ///< Assign 06 to Multi-bit ECC
  Dmi16Crc                                              ///< Assign 07 to CRC
} DMI_T16_ERROR_CORRECTION;

/// DMI Type 16 - Physical Memory Array
typedef struct {
  OUT DMI_T16_LOCATION          Location;               ///< The physical location of the Memory Array,
                                                        ///< whether on the system board or an add-in board.
  OUT DMI_T16_USE               Use;                    ///< Identifies the function for which the array
                                                        ///< is used.
  OUT DMI_T16_ERROR_CORRECTION  MemoryErrorCorrection;  ///< The primary hardware error correction or
                                                        ///< detection method supported by this memory array.
  OUT UINT32                    MaximumCapacity;        ///< The maximum memory capacity, in kilobytes,
                                                        ///< for the array.
  OUT UINT16                    NumberOfMemoryDevices;  ///< The number of slots or sockets available
                                                        ///< for memory devices in this array.
  OUT UINT64                    ExtMaxCapacity;         ///< The maximum memory capacity, in bytes,
                                                        ///< for this array.
} TYPE16_DMI_INFO;

/// DMI Type 17 offset 0Eh - Form Factor
typedef enum {
  OtherFormFactor = 0x01,                               ///< Assign 01 to Other
  UnknowFormFactor,                                     ///< Assign 02 to Unknown
  SimmFormFactor,                                       ///< Assign 03 to SIMM
  SipFormFactor,                                        ///< Assign 04 to SIP
  ChipFormFactor,                                       ///< Assign 05 to Chip
  DipFormFactor,                                        ///< Assign 06 to DIP
  ZipFormFactor,                                        ///< Assign 07 to ZIP
  ProprietaryCardFormFactor,                            ///< Assign 08 to Proprietary Card
  DimmFormFactorFormFactor,                             ///< Assign 09 to DIMM
  TsopFormFactor,                                       ///< Assign 10 to TSOP
  RowOfChipsFormFactor,                                 ///< Assign 11 to Row of chips
  RimmFormFactor,                                       ///< Assign 12 to RIMM
  SodimmFormFactor,                                     ///< Assign 13 to SODIMM
  SrimmFormFactor,                                      ///< Assign 14 to SRIMM
  FbDimmFormFactor                                      ///< Assign 15 to FB-DIMM
} DMI_T17_FORM_FACTOR;

/// DMI Type 17 offset 12h - Memory Type
typedef enum {
  OtherMemType = 0x01,                                  ///< Assign 01 to Other
  UnknownMemType,                                       ///< Assign 02 to Unknown
  DramMemType,                                          ///< Assign 03 to DRAM
  EdramMemType,                                         ///< Assign 04 to EDRAM
  VramMemType,                                          ///< Assign 05 to VRAM
  SramMemType,                                          ///< Assign 06 to SRAM
  RamMemType,                                           ///< Assign 07 to RAM
  RomMemType,                                           ///< Assign 08 to ROM
  FlashMemType,                                         ///< Assign 09 to Flash
  EepromMemType,                                        ///< Assign 10 to EEPROM
  FepromMemType,                                        ///< Assign 11 to FEPROM
  EpromMemType,                                         ///< Assign 12 to EPROM
  CdramMemType,                                         ///< Assign 13 to CDRAM
  ThreeDramMemType,                                     ///< Assign 14 to 3DRAM
  SdramMemType,                                         ///< Assign 15 to SDRAM
  SgramMemType,                                         ///< Assign 16 to SGRAM
  RdramMemType,                                         ///< Assign 17 to RDRAM
  DdrMemType,                                           ///< Assign 18 to DDR
  Ddr2MemType,                                          ///< Assign 19 to DDR2
  Ddr2FbdimmMemType,                                    ///< Assign 20 to DDR2 FB-DIMM
  Ddr3MemType = 0x18,                                   ///< Assign 24 to DDR3
  Fbd2MemType                                           ///< Assign 25 to FBD2
} DMI_T17_MEMORY_TYPE;

/// DMI Type 17 offset 13h - Type Detail
typedef struct {
  OUT UINT16                    Reserved1:1;            ///< Reserved
  OUT UINT16                    Other:1;                ///< Other
  OUT UINT16                    Unknown:1;              ///< Unknown
  OUT UINT16                    FastPaged:1;            ///< Fast-Paged
  OUT UINT16                    StaticColumn:1;         ///< Static column
  OUT UINT16                    PseudoStatic:1;         ///< Pseudo-static
  OUT UINT16                    Rambus:1;               ///< RAMBUS
  OUT UINT16                    Synchronous:1;          ///< Synchronous
  OUT UINT16                    Cmos:1;                 ///< CMOS
  OUT UINT16                    Edo:1;                  ///< EDO
  OUT UINT16                    WindowDram:1;           ///< Window DRAM
  OUT UINT16                    CacheDram:1;            ///< Cache Dram
  OUT UINT16                    NonVolatile:1;          ///< Non-volatile
  OUT UINT16                    Registered:1;           ///< Registered (Buffered)
  OUT UINT16                    Unbuffered:1;           ///< Unbuffered (Unregistered)
  OUT UINT16                    Reserved2:1;            ///< Reserved
} DMI_T17_TYPE_DETAIL;

/// DMI Type 17 - Memory Device
typedef struct {
  OUT UINT16                    TotalWidth;             ///< Total Width, in bits, of this memory device, including any check or error-correction bits.
  OUT UINT16                    DataWidth;              ///< Data Width, in bits, of this memory device.
  OUT UINT16                    MemorySize;             ///< The size of the memory device.
  OUT DMI_T17_FORM_FACTOR       FormFactor;             ///< The implementation form factor for this memory device.
  OUT UINT8                     DeviceSet;              ///< Identifies when the Memory Device is one of a set of
                                                        ///< Memory Devices that must be populated with all devices of
                                                        ///< the same type and size, and the set to which this device belongs.
  OUT CHAR8                     DeviceLocator[8];       ///< The string number of the string that identifies the physically labeled socket or board position where the memory device is located.
  OUT CHAR8                     BankLocator[10];        ///< The string number of the string that identifies the physically labeled bank where the memory device is located.
  OUT DMI_T17_MEMORY_TYPE       MemoryType;             ///< The type of memory used in this device.
  OUT DMI_T17_TYPE_DETAIL       TypeDetail;             ///< Additional detail on the memory device type
  OUT UINT16                    Speed;                  ///< Identifies the speed of the device, in megahertz (MHz).
  OUT UINT64                    ManufacturerIdCode;     ///< Manufacturer ID code.
  OUT CHAR8                     SerialNumber[9];        ///< Serial Number.
  OUT CHAR8                     PartNumber[19];         ///< Part Number.
  OUT UINT8                     Attributes;             ///< Bits 7-4: Reserved, Bits 3-0: rank.
  OUT UINT32                    ExtSize;                ///< Extended Size.
  OUT UINT16                    ConfigSpeed;            ///< Configured memory clock speed
} TYPE17_DMI_INFO;

/// Memory DMI Type 17 and 20 - for memory use
typedef struct {
  OUT UINT16                    TotalWidth;             ///< Total Width, in bits, of this memory device, including any check or error-correction bits.
  OUT UINT16                    DataWidth;              ///< Data Width, in bits, of this memory device.
  OUT UINT16                    MemorySize;             ///< The size of the memory device.
  OUT DMI_T17_FORM_FACTOR       FormFactor;             ///< The implementation form factor for this memory device.
  OUT UINT8                     DeviceLocator;          ///< The string number of the string that identifies the physically labeled socket or board position where the memory device is located.
  OUT UINT8                     BankLocator;            ///< The string number of the string that identifies the physically labeled bank where the memory device is located.
  OUT UINT16                    Speed;                  ///< Identifies the speed of the device, in megahertz (MHz).
  OUT UINT64                    ManufacturerIdCode;     ///< Manufacturer ID code.
  OUT UINT8                     SerialNumber[4];        ///< Serial Number.
  OUT UINT8                     PartNumber[18];         ///< Part Number.
  OUT UINT8                     Attributes;             ///< Bits 7-4: Reserved, Bits 3-0: rank.
  OUT UINT32                    ExtSize;                ///< Extended Size.
  OUT UINT8                     Socket:3;               ///< Socket ID
  OUT UINT8                     Channel:2;              ///< Channel ID
  OUT UINT8                     Dimm:2;                 ///< DIMM ID
  OUT UINT8                     DimmPresent:1;          ///< Dimm Present
  OUT UINT32                    StartingAddr;           ///< The physical address, in kilobytes, of a range
                                                        ///< of memory mapped to the referenced Memory Device.
  OUT UINT32                    EndingAddr;             ///< The handle, or instance number, associated with
                                                        ///< the Memory Device structure to which this address
                                                        ///< range is mapped.
  OUT UINT16                    ConfigSpeed;            ///< Configured memory clock speed
  OUT UINT64                    ExtStartingAddr;        ///< The physical address, in bytes, of a range of
                                                        ///< memory mapped to the referenced Memory Device.
  OUT UINT64                    ExtEndingAddr;          ///< The physical ending address, in bytes, of the last of
                                                        ///< a range of addresses mapped to the referenced Memory Device.
} MEM_DMI_INFO;

/// DMI Type 19 - Memory Array Mapped Address
typedef struct {
  OUT UINT32                    StartingAddr;           ///< The physical address, in kilobytes,
                                                        ///< of a range of memory mapped to the
                                                        ///< specified physical memory array.
  OUT UINT32                    EndingAddr;             ///< The physical ending address of the
                                                        ///< last kilobyte of a range of addresses
                                                        ///< mapped to the specified physical memory array.
  OUT UINT16                    MemoryArrayHandle;      ///< The handle, or instance number, associated
                                                        ///< with the physical memory array to which this
                                                        ///< address range is mapped.
  OUT UINT8                     PartitionWidth;         ///< Identifies the number of memory devices that
                                                        ///< form a single row of memory for the address
                                                        ///< partition defined by this structure.
  OUT UINT64                    ExtStartingAddr;        ///< The physical address, in bytes, of a range of
                                                        ///< memory mapped to the specified Physical Memory Array.
  OUT UINT64                    ExtEndingAddr;          ///< The physical address, in bytes, of a range of
                                                        ///< memory mapped to the specified Physical Memory Array.
} TYPE19_DMI_INFO;

///DMI Type 20 - Memory Device Mapped Address
typedef struct {
  OUT UINT32                    StartingAddr;           ///< The physical address, in kilobytes, of a range
                                                        ///< of memory mapped to the referenced Memory Device.
  OUT UINT32                    EndingAddr;             ///< The handle, or instance number, associated with
                                                        ///< the Memory Device structure to which this address
                                                        ///< range is mapped.
  OUT UINT16                    MemoryDeviceHandle;     ///< The handle, or instance number, associated with
                                                        ///< the Memory Device structure to which this address
                                                        ///< range is mapped.
  OUT UINT16                    MemoryArrayMappedAddressHandle; ///< The handle, or instance number, associated
                                                        ///< with the Memory Array Mapped Address structure to
                                                        ///< which this device address range is mapped.
  OUT UINT8                     PartitionRowPosition;   ///< Identifies the position of the referenced Memory
                                                        ///< Device in a row of the address partition.
  OUT UINT8                     InterleavePosition;     ///< The position of the referenced Memory Device in
                                                        ///< an interleave.
  OUT UINT8                     InterleavedDataDepth;   ///< The maximum number of consecutive rows from the
                                                        ///< referenced Memory Device that are accessed in a
                                                        ///< single interleaved transfer.
  OUT UINT64                    ExtStartingAddr;        ///< The physical address, in bytes, of a range of
                                                        ///< memory mapped to the referenced Memory Device.
  OUT UINT64                    ExtEndingAddr;          ///< The physical ending address, in bytes, of the last of
                                                        ///< a range of addresses mapped to the referenced Memory Device.
} TYPE20_DMI_INFO;

/// Collection of pointers to the DMI records
typedef struct {
  OUT TYPE4_DMI_INFO            T4[MAX_SOCKETS_SUPPORTED];    ///< Type 4 struc
  OUT TYPE7_DMI_INFO            T7L1[MAX_SOCKETS_SUPPORTED];  ///< Type 7 struc 1
  OUT TYPE7_DMI_INFO            T7L2[MAX_SOCKETS_SUPPORTED];  ///< Type 7 struc 2
  OUT TYPE7_DMI_INFO            T7L3[MAX_SOCKETS_SUPPORTED];  ///< Type 7 struc 3
  OUT TYPE16_DMI_INFO           T16;                          ///< Type 16 struc
  OUT TYPE17_DMI_INFO           T17[MAX_SOCKETS_SUPPORTED][MAX_CHANNELS_PER_SOCKET][MAX_DIMMS_PER_CHANNEL]; ///< Type 17 struc
  OUT TYPE19_DMI_INFO           T19;                          ///< Type 19 struc
  OUT TYPE20_DMI_INFO           T20[MAX_SOCKETS_SUPPORTED][MAX_CHANNELS_PER_SOCKET][MAX_DIMMS_PER_CHANNEL]; ///< Type 20 struc
} DMI_INFO;

/**********************************************************************
 * Interface call:  AllocateExecutionCache
 **********************************************************************/
#define MAX_CACHE_REGIONS    3

/// AllocateExecutionCache sub param structure for cached memory region
typedef struct {
  IN OUT   UINT32               ExeCacheStartAddr;      ///< Start address
  IN OUT   UINT32               ExeCacheSize;           ///< Size
} EXECUTION_CACHE_REGION;

/**********************************************************************
 * Interface call:  AmdGetAvailableExeCacheSize
 **********************************************************************/
/// Get available Cache remain
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS    StdHeader;              ///< Standard configuration header
     OUT   UINT32               AvailableExeCacheSize;  ///< Remain size
} AMD_GET_EXE_SIZE_PARAMS;

AGESA_STATUS
AmdGetAvailableExeCacheSize (
  IN OUT   AMD_GET_EXE_SIZE_PARAMS *AmdGetExeSizeParams
  );

/// Selection type for core leveling
typedef enum {
  CORE_LEVEL_LOWEST,            ///< Level to lowest common denominator
  CORE_LEVEL_TWO,               ///< Level to 2 cores
  CORE_LEVEL_POWER_OF_TWO,      ///< Level to 1,2,4 or 8
  CORE_LEVEL_NONE,              ///< Do no leveling
  CORE_LEVEL_COMPUTE_UNIT,      ///< Level cores to one core per compute unit
  CORE_LEVEL_ONE,               ///< Level to 1 core
  CORE_LEVEL_THREE,             ///< Level to 3 cores
  CORE_LEVEL_FOUR,              ///< Level to 4 cores
  CORE_LEVEL_FIVE,              ///< Level to 5 cores
  CORE_LEVEL_SIX,               ///< Level to 6 cores
  CORE_LEVEL_SEVEN,             ///< Level to 7 cores
  CORE_LEVEL_EIGHT,             ///< Level to 8 cores
  CORE_LEVEL_NINE,              ///< Level to 9 cores
  CORE_LEVEL_TEN,               ///< Level to 10 cores
  CORE_LEVEL_ELEVEN,            ///< Level to 11 cores
  CORE_LEVEL_TWELVE,            ///< Level to 12 cores
  CORE_LEVEL_THIRTEEN,          ///< Level to 13 cores
  CORE_LEVEL_FOURTEEN,          ///< Level to 14 cores
  CORE_LEVEL_FIFTEEN,           ///< Level to 15 cores
  CoreLevelModeMax              ///< Used for bounds checking
} CORE_LEVELING_TYPE;





/************************************************************************
 *
 *  AGESA Basic Level interface structure definition and function prototypes
 *
 ***********************************************************************/

/**********************************************************************
 * Interface call:  AmdCreateStruct
 **********************************************************************/
AGESA_STATUS
AmdCreateStruct (
  IN OUT   AMD_INTERFACE_PARAMS *InterfaceParams
  );

/**********************************************************************
 * Interface call:  AmdReleaseStruct
 **********************************************************************/
AGESA_STATUS
AmdReleaseStruct (
  IN OUT   AMD_INTERFACE_PARAMS *InterfaceParams
  );

/**********************************************************************
 * Interface call:  AmdInitReset
 **********************************************************************/
/// AmdInitReset param structure
typedef struct {
  IN       AMD_CONFIG_PARAMS         StdHeader;        ///< Standard configuration header
  IN       EXECUTION_CACHE_REGION    CacheRegion[3];   ///< The cached memory region
  IN       AMD_HT_RESET_INTERFACE    HtConfig;         ///< The interface for Ht Recovery
  IN       FCH_RESET_INTERFACE       FchInterface;     ///< Interface for FCH configuration
} AMD_RESET_PARAMS;

AGESA_STATUS
AmdInitReset (
  IN OUT   AMD_RESET_PARAMS     *ResetParams
  );


/**********************************************************************
 * Interface call:  AmdInitEarly
 **********************************************************************/
/// InitEarly param structure
///
/// Provide defaults or customizations to each service performed in AmdInitEarly.
///
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS      StdHeader;        ///< Standard configuration header
  IN       EXECUTION_CACHE_REGION CacheRegion[3];   ///< Execution Map Interface
  IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< platform operational characteristics.
  IN       AMD_HT_INTERFACE       HtConfig;         ///< HyperTransport Interface
  IN       GNB_CONFIGURATION      GnbConfig;        ///< GNB configuration
} AMD_EARLY_PARAMS;

AGESA_STATUS
AmdInitEarly (
  IN OUT   AMD_EARLY_PARAMS     *EarlyParams
  );


/**********************************************************************
 * Interface call:  AmdInitPost
 **********************************************************************/
/// AmdInitPost param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS      StdHeader;        ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< platform operational characteristics.
  IN       MEM_PARAMETER_STRUCT   MemConfig;        ///< Memory post param
  IN       GNB_POST_CONFIGURATION GnbPostConfig;    ///< GNB post param
} AMD_POST_PARAMS;

AGESA_STATUS
AmdInitPost (
  IN OUT   AMD_POST_PARAMS      *PostParams         ///< Amd Cpu init param
  );


/**********************************************************************
 * Interface call:  AmdInitEnv
 **********************************************************************/
/// AmdInitEnv param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS      StdHeader;            ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;       ///< platform operational characteristics.
  IN       GNB_ENV_CONFIGURATION  GnbEnvConfiguration;  ///< GNB configuration
  IN       FCH_INTERFACE          FchInterface;         ///< FCH configuration
} AMD_ENV_PARAMS;

AGESA_STATUS
AmdInitEnv (
  IN OUT   AMD_ENV_PARAMS       *EnvParams
  );


/**********************************************************************
 * Interface call:  AmdInitMid
 **********************************************************************/
/// AmdInitMid param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS      StdHeader;        ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< platform operational characteristics.
  IN       GNB_MID_CONFIGURATION  GnbMidConfiguration;  ///< GNB configuration
  IN       FCH_INTERFACE          FchInterface;     ///< FCH configuration
} AMD_MID_PARAMS;

AGESA_STATUS
AmdInitMid (
  IN OUT   AMD_MID_PARAMS       *MidParams
  );


/**********************************************************************
 * Interface call:  AmdInitLate
 **********************************************************************/
/// AmdInitLate param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS      StdHeader;              ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;         ///< platform operational characteristics.
  IN       IOMMU_EXCLUSION_RANGE_DESCRIPTOR  *IvrsExclusionRangeList;   ///< Pointer to array of exclusion ranges
     OUT   DMI_INFO               *DmiTable;              ///< DMI Interface
     OUT   VOID                   *AcpiPState;            ///< Acpi Pstate SSDT Table
     OUT   VOID                   *AcpiSrat;              ///< SRAT Table
     OUT   VOID                   *AcpiSlit;              ///< SLIT Table
     OUT   VOID                   *AcpiWheaMce;           ///< WHEA MCE Table
     OUT   VOID                   *AcpiWheaCmc;           ///< WHEA CMC Table
     OUT   VOID                   *AcpiAlib;              ///< ACPI SSDT table with ALIB implementation
     OUT   VOID                   *AcpiIvrs;              ///< IOMMU ACPI IVRS(I/O Virtualization Reporting Structure) table
     OUT   VOID                   *AcpiCrat;              ///< Component Resource Affinity Table table
     OUT   VOID                   *AcpiCdit;              ///< Component Locality Distance Information table
} AMD_LATE_PARAMS;

AGESA_STATUS
AmdInitLate (
  IN OUT   AMD_LATE_PARAMS      *LateParams
  );

/**********************************************************************
 * Interface call:  AmdInitRecovery
 **********************************************************************/
/// CPU Recovery Parameters
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS StdHeader;             ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< platform operational characteristics.
} AMD_CPU_RECOVERY_PARAMS;

/// AmdInitRecovery param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS        StdHeader;            ///< Standard configuration header
  IN       MEM_PARAMETER_STRUCT     MemConfig;            ///< Memory post param
  IN       EXECUTION_CACHE_REGION   CacheRegion[3];       ///< The cached memory region. And the max cache region is 3
  IN       AMD_CPU_RECOVERY_PARAMS  CpuRecoveryParams;    ///< Params for CPU related recovery init.
} AMD_RECOVERY_PARAMS;

AGESA_STATUS
AmdInitRecovery (
  IN OUT   AMD_RECOVERY_PARAMS    *RecoveryParams
  );

/**********************************************************************
 * Interface call:  AmdInitResume
 **********************************************************************/
/// AmdInitResume param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS      StdHeader;      ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig; ///< Platform operational characteristics
  IN       AMD_S3_PARAMS          S3DataBlock;    ///< Save state data
} AMD_RESUME_PARAMS;

AGESA_STATUS
AmdInitResume (
  IN       AMD_RESUME_PARAMS    *ResumeParams
  );


/**********************************************************************
 * Interface call:  AmdS3LateRestore
 **********************************************************************/
/// AmdS3LateRestore param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS    StdHeader;          ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< platform operational characteristics.
  IN       AMD_S3_PARAMS          S3DataBlock;      ///< Save state data
} AMD_S3LATE_PARAMS;

AGESA_STATUS
AmdS3LateRestore (
  IN OUT   AMD_S3LATE_PARAMS    *S3LateParams
  );


/**********************************************************************
 * Interface call:  AmdS3Save
 **********************************************************************/
/// AmdS3Save param structure
typedef struct {
  IN OUT   AMD_CONFIG_PARAMS    StdHeader;          ///< Standard configuration header
  IN       PLATFORM_CONFIGURATION PlatformConfig;   ///< platform operational characteristics.
     OUT   AMD_S3_PARAMS          S3DataBlock;      ///< Standard header
  IN       FCH_INTERFACE          FchInterface;     ///< FCH configuration
} AMD_S3SAVE_PARAMS;

AGESA_STATUS
AmdS3Save (
  IN OUT   AMD_S3SAVE_PARAMS    *AmdS3SaveParams
  );


/**********************************************************************
 * Interface call:  AmdLateRunApTask
 **********************************************************************/
/**
 * Entry point for AP tasking.
 */
AGESA_STATUS
AmdLateRunApTask (
  IN       AP_EXE_PARAMS  *AmdApExeParams
);

//
// General Services API
//

/**********************************************************************
 * Interface service call:  AmdGetApicId
 **********************************************************************/
/// Request the APIC ID of a particular core.

typedef struct {
  IN       AMD_CONFIG_PARAMS StdHeader;        ///< Standard configuration header
  IN       UINT8             Socket;           ///< The Core's Socket.
  IN       UINT8             Core;             ///< The Core id.
     OUT   BOOLEAN           IsPresent;        ///< The Core is present, and  ApicAddress is valid.
     OUT   UINT8             ApicAddress;      ///< The Core's APIC ID.
} AMD_APIC_PARAMS;

/**
 * Get a specified Core's APIC ID.
 */
AGESA_STATUS
AmdGetApicId (
  IN OUT AMD_APIC_PARAMS *AmdParamApic
);

/**********************************************************************
 * Interface service call:  AmdGetPciAddress
 **********************************************************************/
/// Request the PCI Address of a Processor Module (that is, its Northbridge)

typedef struct {
  IN       AMD_CONFIG_PARAMS StdHeader;        ///< Standard configuration header
  IN       UINT8             Socket;           ///< The Processor's socket
  IN       UINT8             Module;           ///< The Module in that Processor
     OUT   BOOLEAN           IsPresent;        ///< The Core is present, and  PciAddress is valid.
     OUT   PCI_ADDR          PciAddress;       ///< The Processor's PCI Config Space address (Function 0, Register 0)
} AMD_GET_PCI_PARAMS;

/**
 * Get Processor Module's PCI Config Space address.
 */
AGESA_STATUS
AmdGetPciAddress (
  IN OUT   AMD_GET_PCI_PARAMS *AmdParamGetPci
);

/**********************************************************************
 * Interface service call:  AmdIdentifyCore
 **********************************************************************/
/// Request the identity (Socket, Module, Core) of the current Processor Core

typedef struct {
  IN       AMD_CONFIG_PARAMS StdHeader;         ///< Standard configuration header
     OUT   UINT8             Socket;            ///< The current Core's Socket
     OUT   UINT8             Module;            ///< The current Core's Processor Module
     OUT   UINT8             Core;              ///< The current Core's core id.
} AMD_IDENTIFY_PARAMS;

/**
 * "Who am I" for the current running core.
 */
AGESA_STATUS
AmdIdentifyCore (
  IN OUT  AMD_IDENTIFY_PARAMS *AmdParamIdentify
);

/**********************************************************************
 * Interface service call:  AmdReadEventLog
 **********************************************************************/
/// An Event Log Entry.
typedef struct {
  IN       AMD_CONFIG_PARAMS StdHeader;         ///< Standard configuration header
     OUT   UINT32            EventClass;        ///< The severity of this event, matches AGESA_STATUS.
     OUT   UINT32            EventInfo;         ///< The unique event identifier, zero means "no event".
     OUT   UINT32            DataParam1;        ///< Data specific to the Event.
     OUT   UINT32            DataParam2;        ///< Data specific to the Event.
     OUT   UINT32            DataParam3;        ///< Data specific to the Event.
     OUT   UINT32            DataParam4;        ///< Data specific to the Event.
} EVENT_PARAMS;

/**
 * Read an Event from the Event Log.
 */
AGESA_STATUS
AmdReadEventLog (
  IN       EVENT_PARAMS *Event
);

/**********************************************************************
 * Interface service call:  AmdIdentifyDimm
 **********************************************************************/
/// Request the identity of dimm from system address

typedef struct {
  IN OUT   AMD_CONFIG_PARAMS StdHeader;            ///< Standard configuration header
  IN       UINT64            MemoryAddress;        ///< System Address that needs to be translated to dimm identification.
  OUT      UINT8             SocketId;             ///< The socket on which the targeted address locates.
  OUT      UINT8             MemChannelId;         ///< The channel on which the targeted address locates.
  OUT      UINT8             DimmId;               ///< The dimm on which the targeted address locates.
} AMD_IDENTIFY_DIMM;

/**
 * Get the dimm identification for the address.
 */
AGESA_STATUS
AmdIdentifyDimm (
  IN OUT   AMD_IDENTIFY_DIMM *AmdDimmIdentify
);

AGESA_STATUS
AmdIdsRunApTaskLate (
  IN       AP_EXE_PARAMS  *AmdApExeParams
  );


#define AGESA_IDS_DFT_VAL   0xFFFF                  ///<  Default value of every uninitlized NV item, the action for it will be ignored
#define AGESA_IDS_NV_END    0xFFFF                  ///< Flag specify end of option structure
/// WARNING: Don't change the comment below, it used as signature for script
/// AGESA IDS NV ID Definitions
typedef enum {
  AGESA_IDS_EXT_ID_START                   = 0x0000,///< 0x0000                                                             specify the start of external NV id

  AGESA_IDS_NV_UCODE,                               ///< 0x0001                                                            Enable or disable microcode patching

  AGESA_IDS_NV_TARGET_PSTATE,                       ///< 0x0002                                                        Set the P-state required to be activated
  AGESA_IDS_NV_POSTPSTATE,                          ///< 0x0003                                           Set the P-state required to be activated through POST

  AGESA_IDS_NV_BANK_INTERLEAVE,                     ///< 0x0004                                                               Enable or disable Bank Interleave
  AGESA_IDS_NV_CHANNEL_INTERLEAVE,                  ///< 0x0005                                                            Enable or disable Channel Interleave
  AGESA_IDS_NV_NODE_INTERLEAVE,                     ///< 0x0006                                                               Enable or disable Node Interleave
  AGESA_IDS_NV_MEMHOLE,                             ///< 0x0007                                                                  Enables or disable memory hole

  AGESA_IDS_NV_SCRUB_REDIRECTION,                   ///< 0x0008                                           Enable or disable a write to dram with corrected data
  AGESA_IDS_NV_DRAM_SCRUB,                          ///< 0x0009                                                   Set the rate of background scrubbing for DRAM
  AGESA_IDS_NV_DCACHE_SCRUB,                        ///< 0x000A                                            Set the rate of background scrubbing for the DCache.
  AGESA_IDS_NV_L2_SCRUB,                            ///< 0x000B                                           Set the rate of background scrubbing for the L2 cache
  AGESA_IDS_NV_L3_SCRUB,                            ///< 0x000C                                           Set the rate of background scrubbing for the L3 cache
  AGESA_IDS_NV_ICACHE_SCRUB,                        ///< 0x000D                                             Set the rate of background scrubbing for the Icache
  AGESA_IDS_NV_SYNC_ON_ECC_ERROR,                   ///< 0x000E                                    Enable or disable the sync flood on un-correctable ECC error
  AGESA_IDS_NV_ECC_SYMBOL_SIZE,                     ///< 0x000F                                                                             Set ECC symbol size

  AGESA_IDS_NV_ALL_MEMCLKS,                         ///< 0x0010                                                      Enable or disable all memory clocks enable
  AGESA_IDS_NV_DCT_GANGING_MODE,                    ///< 0x0011                                                                             Set the Ganged mode
  AGESA_IDS_NV_DRAM_BURST_LENGTH32,                 ///< 0x0012                                                                    Set the DRAM Burst Length 32
  AGESA_IDS_NV_MEMORY_POWER_DOWN,                   ///< 0x0013                                                        Enable or disable Memory power down mode
  AGESA_IDS_NV_MEMORY_POWER_DOWN_MODE,              ///< 0x0014                                                                  Set the Memory power down mode
  AGESA_IDS_NV_DLL_SHUT_DOWN,                       ///< 0x0015                                                                   Enable or disable DLLShutdown
  AGESA_IDS_NV_ONLINE_SPARE,                        ///< 0x0016      Enable or disable the Dram controller to designate a DIMM bank as a spare for logical swap

  AGESA_IDS_NV_HT_ASSIST,                           ///< 0x0017                                                                     Enable or Disable HT Assist
  AGESA_IDS_NV_ATMMODE,                             ///< 0x0018                                                                      Enable or Disable ATM mode

  AGESA_IDS_NV_HDTOUT,                              ///< 0x0019                                                                Enable or disable HDTOUT feature

  AGESA_IDS_NV_HTLINKSOCKET,                        ///< 0x001A                                                                                  HT Link Socket
  AGESA_IDS_NV_HTLINKPORT,                          ///< 0x001B                                                                                    HT Link Port
  AGESA_IDS_NV_HTLINKFREQ,                          ///< 0x001C                                                                               HT Link Frequency
  AGESA_IDS_NV_HTLINKWIDTHIN,                       ///< 0x001D                                                                                HT Link In Width
  AGESA_IDS_NV_HTLINKWIDTHOUT,                      ///< 0x001E                                                                               HT Link Out Width

  AGESA_IDS_NV_GNBHDAUDIOEN,                        ///< 0x001F                                                                  Enable or disable GNB HD Audio

  AGESA_IDS_NV_CPB_EN,                              ///< 0x0020                                                                          Core Performance Boost

  AGESA_IDS_NV_HTC_EN,                              ///< 0x0021                                                                                      HTC Enable
  AGESA_IDS_NV_HTC_OVERRIDE,                        ///< 0x0022                                                                                    HTC Override
  AGESA_IDS_NV_HTC_PSTATE_LIMIT,                    ///< 0x0023                                                                        HTC P-state limit select
  AGESA_IDS_NV_HTC_TEMP_HYS,                        ///< 0x0024                                                                      HTC Temperature Hysteresis
  AGESA_IDS_NV_HTC_ACT_TEMP,                        ///< 0x0025                                                                             HTC Activation Temp

  AGESA_IDS_NV_POWER_POLICY,                        ///< 0x0026                                                                    Select Platform Power Policy
  AGESA_IDS_EXT_ID_END,                             ///< 0x0027                                                               specify the end of external NV ID
} IDS_EX_NV_ID;


#define IDS_NUM_EXT_NV_ITEM (AGESA_IDS_EXT_ID_END - AGESA_IDS_EXT_ID_START + 1)


#endif // _AGESA_H_
